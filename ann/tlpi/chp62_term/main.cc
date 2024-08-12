#include <cstdio>
#include <cstring>

#include <eve/base/log.h>

#include "base.h"
#include "tty.h"

using eve::tty::KeyInfo;
using eve::tty::KeyKind;
using eve::tty::Run;

auto GetUserInput( int &QuitSignal, const KeyInfo *Info ) -> error_t;

auto
main( ) -> int
{
    error_t Err        = OK;
    int     QuitSignal = 0;

    while ( 1 ) {
        logInfo( "Your Choice? [Left or Right arrow to select]:" );

        Err = Run( [&]( auto *info ) -> error_t {
            return GetUserInput( QuitSignal, info );
        } );

        // Handle the cases whether we should quite or continue.
        if ( Err == ERR_EOF ) {
            if ( QuitSignal == 1 ) {
                logInfo( "Bye." );
                Err = OK;
                break;
            }

            continue;  // Next iteration.
        }

        if ( Err != OK ) {  // Now it is totally unexpected.
            logFatal( "Unexpected error. Quit." );
            break;
        }
    }

    return Err;
}

auto
GetUserInput( int &QuitSignal, const KeyInfo *Info ) -> error_t
{
    static int Answer      = 0;
    static int EscapeCount = 0;

    if ( Info->Kind == KeyKind::Esc ) {
        if ( EscapeCount == 1 ) {
            QuitSignal = 1;
            logInfo( "[Quit]" );
            return ERR_EOF;
        }

        EscapeCount++;
        logInfo( "[Escape] One more time to quit." );
        return OK;
    }

    EscapeCount = 0;

    switch ( Info->Kind ) {
    case KeyKind::Enter:
        logInfo( "[Enter] Choice %d", Answer );
        QuitSignal = 0;
        return ERR_EOF;

    case KeyKind::ArrowLeft:
        Answer--;
        logInfo( "Choice %d", Answer );
        return OK;

    case KeyKind::ArrowRight:
        Answer++;
        logInfo( "Choice %d", Answer );
        return OK;

    default:
        const char *InputStr = Info->Str;
        if ( *InputStr == '\x1b' ) {
            // Debugging Purpose.
            logInfo( "[RAW %lu] ESC %s", strlen( InputStr ), InputStr + 1 );
            return OK;
        }
        return OK;
    }
}
