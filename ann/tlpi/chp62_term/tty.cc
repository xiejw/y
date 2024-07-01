#include "tty.h"

#include <termios.h>
#include <unistd.h>

namespace eve::tty {

namespace {
error_t
SetCbreak( int fd, struct termios *prevTermios )
{
    struct termios t;
    if ( tcgetattr( fd, &t ) == -1 ) return ERR;
    if ( prevTermios != NULL ) *prevTermios = t;
    t.c_lflag &= ~( ICANON | ECHO );
    t.c_lflag |= ISIG;
    t.c_iflag &= ~ICRNL;
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
    error_t         err;
    struct termios  save;
    struct termios *psave = NULL;

    err = SetCbreak( 0, &save );
    if ( err != OK ) {
        goto cleanup;
    }
    psave = &save;  // cleanup will handle now.

    char buf[256];
    for ( ;; ) {
        size_t n = read( 0, buf, 256 );
        buf[n]   = 0;

        err = fn( data, buf );
        if ( err == EEOF ) break;
    }

cleanup:

    if ( psave != NULL ) {
        Reset( 0, psave );
    }

    return err;
}
}  // namespace eve::tty
