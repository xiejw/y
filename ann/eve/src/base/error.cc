#include <eve/base/error.h>

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <ranges>

namespace eve::base {

Error::Error( Error &&e )
{
    this->m_holder = e.m_holder;
    e.m_holder     = nullptr;
}
Error &
Error::operator=( Error &&e )
{
    this->m_holder = e.m_holder;
    e.m_holder     = nullptr;
    return *this;
}

ErrorHolder::ErrorHolder( eve::adt::Sds &&msg )
    : m_kind{ ErrorKind::ERROR }, m_msg{ }
{
    EmitNote( std::move( msg ) );
}

ErrorHolder &
ErrorHolder::EmitNote( eve::adt::Sds &&msg )
{
    this->m_msg.push_back( std::move( msg ) );
    return *this;
}

eve::adt::Sds
ErrorHolder::Msg( ) const noexcept
{
    eve::adt::Sds sds{ };
    for ( auto &msg : this->m_msg | std::views::reverse ) {
        sds.CatPrintf( "  > %s\n", msg.Data( ) );
    }
    return sds;
}

void
errFatalAndExit_( const char *file, int line_no, const char *fmt, ... )
{
    fprintf( stderr, "fatal error: at %s line %d:\n  -> ", file, line_no );
    va_list args;
    va_start( args, fmt );
    vfprintf( stderr, fmt, args );
    va_end( args );
    fprintf( stderr, "\n" );

    fflush( stderr );
    exit( 1 );
}

}  // namespace eve::base
