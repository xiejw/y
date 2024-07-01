#include "tty.h"

#include <termios.h>
#include <unistd.h>

namespace eve::tty {

namespace {

const int MAX_BUF_LIMIT = 256;  // Max key input buf length limit.

error_t
SetCbreak( int fd, struct termios *prevTermios )
{
    struct termios t;
    if ( tcgetattr( fd, &t ) == -1 ) return ERR;
    if ( prevTermios != NULL ) *prevTermios = t;
    t.c_lflag &= ( tcflag_t ) ~( ICANON | ECHO );
    t.c_lflag |= ISIG;
    t.c_iflag &= (tcflag_t)~ICRNL;
    t.c_cc[VMIN]  = 1;
    t.c_cc[VTIME] = 0;
    if ( tcsetattr( fd, TCSAFLUSH, &t ) == -1 ) return ERR;
    return OK;
}

error_t
Reset( int fd, struct termios *prevTermios )
{
    if ( tcsetattr( fd, TCSAFLUSH, prevTermios ) == -1 ) return ERR;
    return OK;
}
}  // namespace

error_t
Run( CallbackFn fn, void *data )
{
    error_t         err{ };
    struct termios  save;          // Will be set later.
    struct termios *psave = NULL;  // Upon save is set, point to save.

    err = SetCbreak( 0, &save );
    if ( err != OK ) {
        goto cleanup;
    }
    psave = &save;  // Goto `cleanup` will handle restoring.

    char buf[MAX_BUF_LIMIT];
    for ( ;; ) {
        ssize_t n = read( 0, buf, MAX_BUF_LIMIT );

        if ( n == -1 ) {  // This should really not happen for stdin.
            err = ERR;
            goto cleanup;
        }

        if ( n == MAX_BUF_LIMIT ) {  // Should not happen though.
            err = ELIMIT;
            goto cleanup;
        }

        buf[n] = 0;
        err    = fn( data, buf );
        if ( err != OK ) goto cleanup;
    }

cleanup:
    if ( psave != NULL ) {
        Reset( 0, psave );
    }
    return err;
}
}  // namespace eve::tty
