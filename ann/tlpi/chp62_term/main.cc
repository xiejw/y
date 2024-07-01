#include <cstdio>
#include <cstring>

#include "base.h"
#include "tty.h"

using eve::tty::Run;

error_t GetUserInput( void *data, char *in );

int
main( )
{
    error_t err        = OK;
    int     QuitSignal = 0;

    while ( 1 ) {
        printf( "Your Choice? [Left or Right arrow to select]: \n" );
        fflush( stdout );

        err = Run( GetUserInput, &QuitSignal );
        if ( err == EEOF && QuitSignal == 1 ) {
            printf( "Bye.\n" );
            err = OK;
            break;
        }
        if ( err == EEOF && QuitSignal == 0 ) {
            continue;
        }

        if ( err != OK ) {  // Now it is totally unexpected.
            printf( "Unexpected error. Quit.\n" );
            break;
        }
    }

    return err;
}

error_t
GetUserInput( void *QuitSignal, char *input )
{
    static int answer      = 0;
    static int EscapeCount = 0;

    if ( strcmp( input, "\x1b" ) == 0 ) {  // Esc
        if ( EscapeCount == 1 ) {
            *(int *)QuitSignal = 1;
            printf( "[Quit]\n" );
            return EEOF;
        }

        EscapeCount++;
        printf( "[Escape] One more time to quit.\n" );
        fflush( stdout );
        return OK;
    }

    EscapeCount = 0;

    if ( strcmp( input, "\r" ) == 0 ) {  // Enter
        answer++;
        printf( "[Enter] Choice %d\n", answer );
        fflush( stdout );
        *(int *)QuitSignal = 0;
        return EEOF;
    }

    // if (*input == 0) {return EEOF;}
    if ( strcmp( input, "\x1b[D" ) == 0 ) {
        answer--;
        printf( "Choice %d\n", answer );
        fflush( stdout );
        return OK;
    } else if ( strcmp( input, "\x1b[C" ) == 0 ) {
        answer++;
        printf( "Choice %d\n", answer );
        fflush( stdout );
        return OK;
    } else if ( *input == '\x1b' ) {
        printf( "[RAW %lu] ESC %s\n", strlen( input ), input + 1 );
        fflush( stdout );
        return OK;
    } else {
        size_t len = strlen( input );
        if ( len == 1 ) {
            printf( "[RAW] %s, (%x)\n", input, *input );
        } else {
            printf( "[RAW %lu] %s\n", len, input );
        }
        fflush( stdout );
        return OK;
    }
}
