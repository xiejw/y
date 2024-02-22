#include <eve/base/error.h>

#include <cstdarg>
#include <cstdio>
#include <cstdlib>

namespace eve::base {

void
errFatalAndExit_( const char *file, int line_no, const char *fmt, ... )
{
    fprintf( stderr, "fatal error: at %s line %d:\n  -> ", file, line_no );
    va_list args;
    va_start( args, fmt );
    vfprintf( stderr, fmt, args );
    va_end( args );
    fprintf( stderr, "\n" );

    // if (err_msg_header != NULL) {
    //         errDump("\nfound existing error as follows:");
    // }

    fflush( stderr );
    exit( 1 );
}

}  // namespace eve::base
