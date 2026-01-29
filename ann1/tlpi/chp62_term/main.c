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

#define log_info( fmt, ... ) printf( fmt "\n" __VA_OPT__(, ) __VA_ARGS__ )
#define log_fatal( fmt, ... )                                  \
        do {                                                   \
                printf( fmt "\n" __VA_OPT__(, ) __VA_ARGS__ ); \
                exit( 1 );                                     \
        } while ( 0 )

static int
gen_rng( )
{
        srand( (unsigned)time( NULL ) );
        return ( (int)rand( ) % 21 ) - 10;
}

static rc_t
get_user_input( int num_to_guess, int *quit_game,
                const tty_key_info_s *key_info )
{
        static int cached_answer = 0;
        static int escape_cnt    = 0;

        if ( key_info->kind == Key_Esc ) {
                if ( escape_cnt == 1 ) {
                        *quit_game = 1;
                        log_info( "[Quit]" );
                        return ERR_EOF;
                }

                escape_cnt++;
                log_info( "[Escape] One more time to quit." );
                return OK;
        }

        escape_cnt = 0;

        switch ( key_info->kind ) {
        case Key_Enter:
                if ( cached_answer == num_to_guess ) {
                        log_info( "[Enter] Choice %d. Bingo! ", cached_answer );
                        *quit_game = 1;
                } else {
                        log_info( "[Enter] Choice %d. %s ", cached_answer,
                                  cached_answer > num_to_guess ? "Too big"
                                                               : "Too small" );
                        *quit_game = 0;
                }

                return ERR_EOF;

        case Key_ArrowLeft:
                cached_answer--;
                log_info( "Choice %d [Press Enter to test]", cached_answer );
                return OK;

        case Key_ArrowRight:
                cached_answer++;
                log_info( "Choice %d [Press Enter to test]", cached_answer );
                return OK;

        default:
                const char *InputStr = key_info->str;
                if ( *InputStr == '\x1b' ) {
                        // Debugging Purpose.
                        log_info( "[DEBUG] /RAW %lu/ ESC %s",
                                  strlen( InputStr ), InputStr + 1 );
                        return OK;
                }
                return OK;
        }
}

typedef struct {
        int to_guess;
        int quit;
} guess_game_info_s;

static rc_t
guess_user_input_cb_fn( void *udp, const tty_key_info_s *info )
{
        guess_game_info_s *game = udp;
        return get_user_input( game->to_guess, &game->quit, info );
}

int
main( )
{
        rc_t              err  = OK;
        guess_game_info_s game = { .to_guess = gen_rng( ), .quit = 0 };

        while ( 1 ) {
                log_info(
                    "Your Choice between -10 (inc) to 10 (inc) ? [Left or "
                    "Right arrow "
                    "to select]:" );

                err = tty_run( &game, guess_user_input_cb_fn );

                // Handle the cases whether we should quit or continue.
                if ( err == ERR_EOF ) {
                        if ( game.quit == 1 ) {
                                log_info( "Bye." );
                                err = OK;
                                break;
                        }

                        continue;  // Next iteration.
                }

                if ( err != OK ) {  // Now it is totally unexpected.
                        log_fatal( "Unexpected error. Quit." );
                        break;
                }
        }

        return err;
}
