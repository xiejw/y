#include "tty.h"

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include <termios.h>
#include <unistd.h>

namespace eve::tty {

namespace {

const int MAX_BUF_LIMIT = 256;  // Max key input buf length limit.

// -----------------------------------------------------------------------------
// Terminal Control
//

auto
SetCbreak( int fd, struct termios *prevTermios ) -> error_t
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

auto
Reset( int fd, struct termios *prevTermios ) -> error_t
{
    if ( tcsetattr( fd, TCSAFLUSH, prevTermios ) == -1 ) return ERR;
    return OK;
}

// -----------------------------------------------------------------------------
// Key Mapping
//

using KeyMapping = std::unordered_map<std::string_view, KeyKind>;
std::unique_ptr<KeyMapping> Mapping{ };
std::once_flag              MappingOnceFlag;

auto
InitMapping( )
{
    std::call_once( MappingOnceFlag, [&]( ) {
        Mapping = std::make_unique<KeyMapping>(
            std::initializer_list<KeyMapping::value_type>{
                {    "\r",      KeyKind::Enter},
                {  "\x1b",        KeyKind::Esc},
                {"\x1b[D",  KeyKind::ArrowLeft},
                {"\x1b[C", KeyKind::ArrowRight},
        } );
    } );
}

auto
LookupMapping( const char *Str ) -> KeyKind
{
    auto Result = Mapping->find( Str );
    if ( Result == Mapping->end( ) ) return KeyKind::Na;

    return Result->second;
}

}  // namespace

auto
Run( CallbackFn fn, void *data ) -> error_t
{
    error_t         err{ };
    struct termios  save;          // Will be set later.
    struct termios *psave = NULL;  // Upon save is set, point to save.

    err = SetCbreak( 0, &save );
    if ( err != OK ) {
        goto cleanup;
    }
    psave = &save;  // Goto `cleanup` will handle restoring.

    // Init the work.
    InitMapping( );
    char Buf[MAX_BUF_LIMIT];

    // Loop until returning non-OK;
    for ( ;; ) {
        ssize_t n = read( 0, Buf, MAX_BUF_LIMIT );

        if ( n == -1 ) {  // This should really not happen for stdin.
            err = ERR;
            goto cleanup;
        }

        if ( n == MAX_BUF_LIMIT ) {  // Should not happen though.
            err = ELIMIT;
            goto cleanup;
        }

        Buf[n] = 0;

        // Prepare the Info
        auto    Kind = LookupMapping( Buf );
        KeyInfo Info = { Kind, Buf };

        err = fn( data, &Info );
        if ( err != OK ) goto cleanup;
    }

cleanup:
    if ( psave != NULL ) {
        Reset( 0, psave );
    }
    return err;
}
}  // namespace eve::tty
