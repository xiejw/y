#include <cstdio>
#include <cstring>

#include "base.h"
#include "tty.h"

using eve::tty::KeyInfo;
using eve::tty::KeyKind;
using eve::tty::Run;

auto GetUserInput( int &QuitSignal, const KeyInfo *Info ) -> error_t;

auto
main( ) -> int
{
    error_t err        = OK;
    int     QuitSignal = 0;

    while ( 1 ) {
        printf( "Your Choice? [Left or Right arrow to select]: \n" );
        fflush( stdout );

        err = Run( [&]( auto *info ) -> error_t {
            return GetUserInput( QuitSignal, info );
        } );
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

auto
GetUserInput( int &QuitSignal, const KeyInfo *Info ) -> error_t
{
    static int  answer      = 0;
    static int  EscapeCount = 0;
    const char *input       = Info->Str;

    if ( Info->Kind == KeyKind::Esc ) {
        if ( EscapeCount == 1 ) {
            QuitSignal = 1;
            printf( "[Quit]\n" );
            return EEOF;
        }

        EscapeCount++;
        printf( "[Escape] One more time to quit.\n" );
        fflush( stdout );
        return OK;
    }

    EscapeCount = 0;

    switch ( Info->Kind ) {
    case KeyKind::Enter:
        printf( "[Enter] Choice %d\n", answer );
        fflush( stdout );
        QuitSignal = 0;
        return EEOF;

    case KeyKind::ArrowLeft:
        answer--;
        printf( "Choice %d\n", answer );
        fflush( stdout );
        return OK;

    case KeyKind::ArrowRight:
        answer++;
        printf( "Choice %d\n", answer );
        fflush( stdout );
        return OK;

    default:
        if ( *input == '\x1b' ) {
            // Debugging Purpose.
            printf( "[RAW %lu] ESC %s\n", strlen( input ), input + 1 );
            fflush( stdout );
            return OK;
        }
        return OK;
    }
}
