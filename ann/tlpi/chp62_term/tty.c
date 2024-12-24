#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <tty.h>

#define MAX_BUF_LIMIT 256  // Max key input buf length limit.

// -----------------------------------------------------------------------------
// Terminal Control
//

static error_t
tty_set_cbreak( int fd, struct termios *prevTermios )
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

static error_t
tty_reset( int fd, struct termios *prevTermios )
{
    if ( tcsetattr( fd, TCSAFLUSH, prevTermios ) == -1 ) return ERR;
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

// auto
// Run( CallbackFn fn ) -> error_t
//{
//     error_t         Err{ };
//     struct termios  SavedTerm;  // Will be set later.
//     struct termios *PtrToSavedTerm =
//         NULL;  // Upon SavedTerm is set, point to SavedTerm.
//
//     Err = tty_set_cbreak( 0, &SavedTerm );
//     if ( Err != OK ) {
//         goto cleanup;
//     }
//     PtrToSavedTerm = &SavedTerm;  // Goto `cleanup` will handle restoring.
//
//     // Init the work.
//     InitMapping( );
//     char Buf[MAX_BUF_LIMIT];
//
//     // Loop until returning non-OK;
//     for ( ;; ) {
//         ssize_t NumRead = read( 0, Buf, MAX_BUF_LIMIT );
//
//         if ( NumRead == -1 ) {  // This should really not happen for stdin.
//             Err = ERR_IO;
//             goto cleanup;
//         }
//
//         if ( NumRead == MAX_BUF_LIMIT ) {  // Should not happen though.
//             Err = ERR_LIMIT;
//             goto cleanup;
//         }
//
//         Buf[NumRead] = 0;
//
//         // Prepare the Info
//         auto    Kind = LookupMapping( Buf );
//         KeyInfo Info = { Kind, Buf };
//
//         Err = fn( &Info );
//         if ( Err != OK ) goto cleanup;
//     }
//
// cleanup:
//     if ( PtrToSavedTerm != NULL ) {
//         Reset( 0, PtrToSavedTerm );
//     }
//     return Err;
// }
// }  // namespace eve::tty
