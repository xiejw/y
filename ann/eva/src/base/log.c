// copyright: see license_rxi-mit file.

#include "base/log.h"
#include "base/error.h"

#include <stdlib.h>
#include <string.h>  // strlen

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

static struct {
        void *udata;  // pass to lock_fn
        log_lock_fn_t
            lock_fn;  // TODO we should add a default one when PTHREAD is on.
        int level;
        int quiet;
} L;

static const char *level_strings[] = { "TRACE", "DEBUG", "INFO",
                                       "WARN",  "ERROR", "FATAL" };

#ifndef EVA_LOG_NO_COLOR
// see here for color codes.
// https://docs.microsoft.com/en-us/windows/terminal/images/one-half-dark-color-scheme.png
static const char *level_colors[] = { "\x1b[94m", "\x1b[36m", "\x1b[32m",
                                      "\x1b[33m", "\x1b[31m", "\x1b[35m" };
#endif

static void
stdout_callback( struct log_event *ev )
{
        char buf[25];
        buf[strftime( buf, sizeof( buf ), "%F %H:%M:%S", ev->time )] = '\0';
#ifndef EVA_LOG_NO_COLOR
        fprintf( ev->out, "%s %s%-5s\x1b[0m \x1b[1;34m%s:%d:\x1b[0m ", buf,
                 level_colors[ev->level], level_strings[ev->level], ev->file,
                 ev->line );
#else
        fprintf( ev->out, "%s %-5s %s:%d: ", buf, level_strings[ev->level],
                 ev->file, ev->line );
#endif
        const char *fmt = ev->fmt;
        vfprintf( ev->out, fmt, ev->ap );

        // only prints \n iff the fmt does not end with that.
        size_t fmtlen = strlen( fmt );
        if ( fmtlen == 0 || fmt[fmtlen - 1] != '\n' ) {
                fprintf( ev->out, "\n" );
        }
        fflush( ev->out );
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
loge_( int level, int dump, const char *file, int line, const char *fmt, ... )
{
        if ( logIsLevelOn( level ) ) {
                struct log_event ev = {
                    .fmt   = fmt,
                    .file  = file,
                    .line  = line,
                    .level = level,
                };

                lock( );

                init_event( &ev, stderr );
                va_start( ev.ap, fmt );
                stdout_callback( &ev );
                va_end( ev.ap );

                unlock( );
        }

        if ( level == LOG_FATAL ) {
                if ( dump ) {
                        errDump( "current error stack:" );
                } else {
                        exit( 1 );
                }
        }
}

// An extreme simple logging implementations when log_e is a not a good fit. It
// has all the logic we need for the logging functionalities.
void
_simple_loge_( int level, int dump, const char *file, int line, const char *fmt,
               ... )
{
        if ( logIsLevelOn( level ) ) {
                fprintf( stderr, "SIMPLE_LOG " );
                va_list ap;
                va_start( ap, fmt );
                vfprintf( stderr, fmt, ap );
                va_end( ap );

                // only prints \n iff the fmt does not end with that.
                size_t fmtlen = strlen( fmt );
                if ( fmtlen == 0 || fmt[fmtlen - 1] != '\n' ) {
                        fprintf( stderr, "\n" );
                }

                fflush( stderr );
        }
        if ( level == LOG_FATAL ) {
                if ( dump ) {
                        errDump( "current error stack:" );
                } else {
                        exit( 1 );
                }
        }
}

int
logSetLevel( int level )
{
        int old_v = L.level;
        L.level   = level;
        return old_v;
}

int
logSetQuiet( int enable )
{
        int old_v = L.quiet;
        L.quiet   = enable;
        return old_v;
}

int
logIsLevelOn( int level )
{
        return !L.quiet && level >= L.level;
}
