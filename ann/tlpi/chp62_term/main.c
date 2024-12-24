// This is a demo program to let user guess a number between -10 and 10 (both
// included).
//
// - User can use left and right arrow to select the guessed number.
// - User can use Enter to test (check) the number.
// - User can type ESC (twice) to quit.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <base.h>
#include <tty.h>

#define logInfo( fmt, ... ) printf( fmt "\n" __VA_OPT__(, ) __VA_ARGS__ )
#define logFatal( fmt, ... )                           \
    do {                                               \
        printf( fmt "\n" __VA_OPT__(, ) __VA_ARGS__ ); \
        exit( 1 );                                     \
    } while ( 0 )

static int     gen_rng( );
static error_t get_user_input( int numToGuess, int *quitSignal,
                               const tty_key_info_s *Info );

typedef struct {
    int to_guess;
    int quit;
} guess_game_info_s;

static error_t
guess_user_input( void *udp, const tty_key_info_s *info )
{
    guess_game_info_s *game = udp;
    return get_user_input( game->to_guess, &game->quit, info );
}

int
main( )
{
    error_t           err  = OK;
    guess_game_info_s game = { .to_guess = gen_rng( ), .quit = 0 };

    while ( 1 ) {
        logInfo(
            "Your Choice between -10 (inc) to 10 (inc) ? [Left or Right arrow "
            "to select]:" );

        err = tty_run( &game, guess_user_input );

        // Handle the cases whether we should quite or continue.
        if ( err == ERR_EOF ) {
            if ( game.quit == 1 ) {
                logInfo( "Bye." );
                err = OK;
                break;
            }

            continue;  // Next iteration.
        }

        if ( err != OK ) {  // Now it is totally unexpected.
            logFatal( "Unexpected error. Quit." );
            break;
        }
    }

    return err;
}

int
gen_rng( )
{
    srand( (unsigned)time( NULL ) );
    return ( (int)rand( ) % 21 ) - 10;
}

error_t
get_user_input( int numToGuess, int *quitSignal, const tty_key_info_s *Info )
{
    static int Answer      = 0;
    static int EscapeCount = 0;

    if ( Info->kind == Key_Esc ) {
        if ( EscapeCount == 1 ) {
            *quitSignal = 1;
            logInfo( "[Quit]" );
            return ERR_EOF;
        }

        EscapeCount++;
        logInfo( "[Escape] One more time to quit." );
        return OK;
    }

    EscapeCount = 0;

    switch ( Info->kind ) {
    case Key_Enter:
        if ( Answer == numToGuess ) {
            logInfo( "[Enter] Choice %d. Bingo! ", Answer );
            *quitSignal = 1;
        } else {
            logInfo( "[Enter] Choice %d. %s ", Answer,
                     Answer > numToGuess ? "Too big" : "Too small" );
            *quitSignal = 0;
        }

        return ERR_EOF;

    case Key_ArrowLeft:
        Answer--;
        logInfo( "Choice %d [Press Enter to test]", Answer );
        return OK;

    case Key_ArrowRight:
        Answer++;
        logInfo( "Choice %d [Press Enter to test]", Answer );
        return OK;

    default:
        const char *InputStr = Info->str;
        if ( *InputStr == '\x1b' ) {
            // Debugging Purpose.
            logInfo( "[RAW %lu] ESC %s", strlen( InputStr ), InputStr + 1 );
            return OK;
        }
        return OK;
    }
}
