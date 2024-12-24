#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <tty.h>

constexpr int MAX_BUF_LIMIT = 256;  // Max key input buf length limit.

// -----------------------------------------------------------------------------
// Terminal Control
//

static rc_t
tty_set_cbreak( int fd, struct termios *prev_term )
{
        struct termios t;
        if ( tcgetattr( fd, &t ) == -1 ) return ERR;
        if ( prev_term != NULL ) *prev_term = t;
        t.c_lflag &= ( tcflag_t ) ~( ICANON | ECHO );
        t.c_lflag |= ISIG;
        t.c_iflag &= (tcflag_t)~ICRNL;
        t.c_cc[VMIN]  = 1;
        t.c_cc[VTIME] = 0;
        if ( tcsetattr( fd, TCSAFLUSH, &t ) == -1 ) return ERR;
        return OK;
}

static rc_t
tty_reset( int fd, struct termios *prev_term )
{
        if ( prev_term == NULL ) return OK;
        if ( tcsetattr( fd, TCSAFLUSH, prev_term ) == -1 ) return ERR;
        return OK;
}

// -----------------------------------------------------------------------------
// Key Mapping
//

static tty_key_kind_e
tty_lookup_mapping( const char *str )
{
        if ( 0 == strcmp( str, "\r" ) ) return Key_Enter;
        if ( 0 == strcmp( str, "\x1b" ) ) return Key_Esc;
        if ( 0 == strcmp( str, "\x1b[D" ) ) return Key_ArrowLeft;
        if ( 0 == strcmp( str, "\x1b[C" ) ) return Key_ArrowRight;
        return Key_Na;
}

rc_t
tty_run( void *udp, tty_callback_fnt fn )
{
        rc_t err = OK;
        /* saved_term will be set. Upon set, ptr_saved_term points to it. */
        struct termios  saved_term;
        struct termios *ptr_saved_term = NULL;

        err = tty_set_cbreak( 0, &saved_term );
        if ( err != OK ) {
                goto cleanup;
        }
        ptr_saved_term = &saved_term;  // Goto `cleanup` will handle restoring.

        // Init the work.
        char buf[MAX_BUF_LIMIT];

        // Loop until returning non-OK;
        for ( ;; ) {
                ssize_t count = read( 0, buf, MAX_BUF_LIMIT );
                if ( count == -1 ) {
                        err = ERR_IO;  // This should NEVER happen for stdin.
                        goto cleanup;
                }

                if ( count == MAX_BUF_LIMIT ) {  // Should NEVER happen.
                        err = ERR_LIMIT;
                        goto cleanup;
                }

                buf[count]          = 0;
                tty_key_info_s info = { .kind = tty_lookup_mapping( buf ),
                                        .str  = buf };

                err = fn( udp, &info );
                if ( err != OK ) goto cleanup;
        }

cleanup:
        if ( ptr_saved_term != NULL ) tty_reset( 0, ptr_saved_term );
        return err;
}
