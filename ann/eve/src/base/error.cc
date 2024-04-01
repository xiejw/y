#include <eve/base/error.h>

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <ranges>

namespace eve::base {

//
// ErrorHolder
//

Error::ErrorHolder &
Error::ErrorHolder::emitNote( eve::adt::Sds &&msg ) noexcept
{
    this->mMsg.push_back( std::move( msg ) );
    return *this;
}

eve::adt::Sds
Error::ErrorHolder::getMsg( ) const noexcept
{
    assert( mMsg.size( ) > 0 );
    eve::adt::Sds sds{ };
    for ( auto &msg : this->mMsg | std::views::reverse ) {
        sds.catPrintf( "  > %s\n", msg.getData( ) );
    }
    return sds;
}

//
// Error
//

Error::Error( Error &&e )
{
    mHolder   = e.mHolder;
    e.mHolder = nullptr;
}
Error &
Error::operator=( Error &&e )
{
    mHolder   = e.mHolder;
    e.mHolder = nullptr;
    return *this;
}

void
panic_impl_( const char *msg, const char *file, int line_no, const char *fmt,
             ... )
{
    fprintf( stderr, "%s: at %s line %d:\n  -> ", msg, file, line_no );
    va_list args;
    va_start( args, fmt );
    vfprintf( stderr, fmt, args );
    va_end( args );
    fprintf( stderr, "\n" );

    fflush( stderr );
    exit( 1 );
}

}  // namespace eve::base
