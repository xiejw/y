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
std::unique_ptr<KeyMapping> Mapping{ };  // Guarded by MappingOnceFlag.
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
Run( CallbackFn fn ) -> error_t
{
    error_t         Err{ };
    struct termios  SavedTerm;  // Will be set later.
    struct termios *PtrToSavedTerm =
        NULL;  // Upon SavedTerm is set, point to SavedTerm.

    Err = SetCbreak( 0, &SavedTerm );
    if ( Err != OK ) {
        goto cleanup;
    }
    PtrToSavedTerm = &SavedTerm;  // Goto `cleanup` will handle restoring.

    // Init the work.
    InitMapping( );
    char Buf[MAX_BUF_LIMIT];

    // Loop until returning non-OK;
    for ( ;; ) {
        ssize_t NumRead = read( 0, Buf, MAX_BUF_LIMIT );

        if ( NumRead == -1 ) {  // This should really not happen for stdin.
            Err = ERR_IO;
            goto cleanup;
        }

        if ( NumRead == MAX_BUF_LIMIT ) {  // Should not happen though.
            Err = ERR_LIMIT;
            goto cleanup;
        }

        Buf[NumRead] = 0;

        // Prepare the Info
        auto    Kind = LookupMapping( Buf );
        KeyInfo Info = { Kind, Buf };

        Err = fn( &Info );
        if ( Err != OK ) goto cleanup;
    }

cleanup:
    if ( PtrToSavedTerm != NULL ) {
        Reset( 0, PtrToSavedTerm );
    }
    return Err;
}
}  // namespace eve::tty
