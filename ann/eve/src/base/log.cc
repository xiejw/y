// copyright: see license_rxi-mit file.

#include <eve/base/error.h>
#include <eve/base/log.h>

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>  // strlen
#include <ctime>
#include <iostream>

namespace eve::base {

// -----------------------------------------------------------------------------
// private data structure
// -----------------------------------------------------------------------------

struct log_event {
        va_list     ap;
        const char *fmt;   // unowned
        const char *file;  // unowned
        struct tm  *time;  // unowned
        void       *out;   // unowned, used as FILE* now
        int         line;
        int         level;
};

typedef void ( *log_lock_fn_t )( int lock, void *udata );

// TODO we should add a default one for lock_fn when PTHREAD is on.
static struct {
        void         *udata;  // pass to lock_fn
        log_lock_fn_t lock_fn;
        int           level;
        int           quiet;
} L;

static const char *level_strings[] = { "TRACE", "DEBUG", "INFO",
                                       "WARN",  "ERROR", "FATAL" };

// see here for color codes.
// https://docs.microsoft.com/en-us/windows/terminal/images/one-half-dark-color-scheme.png
static const char *level_colors[] = { "\x1b[94m", "\x1b[36m", "\x1b[32m",
                                      "\x1b[33m", "\x1b[31m", "\x1b[35m" };

static void
stdout_callback( struct log_event *ev )
{
        char buf[25];
        buf[strftime( buf, sizeof( buf ), "%F %H:%M:%S", ev->time )] = '\0';
        fprintf( (FILE *)ev->out, "%s %s%-5s\x1b[0m \x1b[1;34m%s:%d:\x1b[0m ",
                 buf, level_colors[ev->level], level_strings[ev->level],
                 ev->file, ev->line );

        const char *fmt = ev->fmt;
        vfprintf( (FILE *)ev->out, fmt, ev->ap );

        // only prints \n iff the fmt does not end with that.
        size_t fmtlen = strlen( fmt );
        if ( fmtlen == 0 || fmt[fmtlen - 1] != '\n' ) {
                fprintf( (FILE *)ev->out, "\n" );
        }
        fflush( (FILE *)ev->out );
}

static void
init_event( struct log_event *ev, void *out )
{
        if ( !ev->time ) {
                time_t t = time( NULL );
                ev->time = localtime( &t );
        }
        ev->out = out;
}

// -----------------------------------------------------------------------------
// impl for private funcs.
// -----------------------------------------------------------------------------

static inline void
lock( void )
{
        if ( L.lock_fn ) {
                L.lock_fn( 1, L.udata );
        }
}

static inline void
unlock( void )
{
        if ( L.lock_fn ) {
                L.lock_fn( 0, L.udata );
        }
}

// -----------------------------------------------------------------------------
// impl for apis
// -----------------------------------------------------------------------------
void
loge_( int level, const char *file, int line, const char *fmt, ... )
{
        if ( isLogLevelOn( level ) ) {
#if defined( __clang__ )
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#endif

#if defined( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
                struct log_event ev = {
                    .fmt   = fmt,
                    .file  = file,
                    .line  = line,
                    .level = level,
                };
#if defined( __GNUC__ )
#pragma GCC diagnostic pop
#endif

#if defined( __clang__ )
#pragma clang diagnostic pop
#endif

                lock( );

                init_event( &ev, stderr );
                va_start( ev.ap, fmt );
                stdout_callback( &ev );
                va_end( ev.ap );

                unlock( );
        }

        if ( level == LOG_FATAL ) {
                exit( 1 );
        }
}

int
setLogLevel( int level )
{
        int old_v = L.level;
        L.level   = level;
        return old_v;
}

int
setLogQuiet( int enable )
{
        int old_v = L.quiet;
        L.quiet   = enable;
        return old_v;
}

int
isLogLevelOn( int level )
{
        return !L.quiet && level >= L.level;
}

}  // namespace eve::base
