#include "eve/adt/sds.h"

#include <cassert>
#include <cstring>

namespace eve ::adt {

namespace {

constexpr int SDS_DEFAULT_ALLOCATE_SPACE = 16;

}

void
Sds::Init( const void *init, size_t len, size_t cap )
{
    // TODO: Cap - 1 should be the power of 2.
    cap = cap > SDS_DEFAULT_ALLOCATE_SPACE ? cap : SDS_DEFAULT_ALLOCATE_SPACE;
    void *buf = malloc( cap + 1 );

    if ( len ) {
        if ( init == nullptr )
            memset( buf, 0, len );
        else
            memcpy( buf, init, len );
    }

    ( (std::byte *)( buf ) )[len] = std::byte( 0 );

    this->m_len = len;
    this->m_cap = cap;
    this->m_ptr.reset( buf );
}

void
Sds::catPrintf( const char *fmt, ... )
{
    va_list ap;
    va_start( ap, fmt );
    this->catVprintf( fmt, ap );
    va_end( ap );
}

void
Sds::catVprintf( const char *fmt, va_list ap )
{
    va_list cpy;

    // TODO: We make a good guss about the buflen here.
    size_t buflen = strlen( fmt ) * 4;  // a good guess

    assert( buflen >= 2 );

    // Path 1: Fast path first. Use the remaining area if possible for speed.
    {
        size_t avail = this->getCap( ) - this->getSize( );
        if ( buflen <= avail ) {
            size_t cur_len = this->getSize( );

            char *buf       = (char *)this->m_ptr.get( ) + cur_len;
            buf[buflen - 2] = '\0';
            va_copy( cpy, ap );
            vsnprintf( buf, buflen, fmt, cpy );
            va_end( cpy );
            if ( buf[buflen - 2] == '\0' ) {
                size_t inc_len = strlen( buf );
                this->m_len += inc_len;
                return;
            }
            // Fall through
        }
    }

    // Path 2: Slow path now.
    //
    // We try to start using a static buffer for speed.  If not possible we
    // revert to heap allocation.
    char staticbuf[1024], *buf = staticbuf;
    if ( buflen > sizeof( staticbuf ) ) {
        buf = (char *)malloc( buflen );
        if ( buf == NULL ) {
            this->m_ptr.reset( nullptr );
            return;
        }
    } else {
        buflen = sizeof( staticbuf );
    }

    /* Try with buffers two times bigger every time we fail to fit the string
     * in the current buffer size. */
    while ( 1 ) {
        buf[buflen - 2] = '\0';
        va_copy( cpy, ap );
        vsnprintf( buf, buflen, fmt, cpy );
        va_end( cpy );
        if ( buf[buflen - 2] != '\0' ) {
            if ( buf != staticbuf ) free( buf );
            buflen *= 2;
            buf = (char *)malloc( buflen );
            if ( buf == NULL ) {
                this->m_ptr.reset( nullptr );
                return;
            }
            continue;
        }
        break;
    }

    // Finally concat the obtained string to the SDS string and return it.
    this->cat( buf );
    if ( buf != staticbuf ) free( buf );
}

void
Sds::cat( std::string_view s )
{
    size_t curlen = this->m_len;
    size_t newlen = s.size( ) + curlen;
    this->reserve( newlen );
    if ( this->m_ptr.get( ) == nullptr ) return;

    memcpy( (char *)this->m_ptr.get( ) + curlen, s.data( ), s.size( ) );
    this->m_len                            = newlen;
    ( (char *)this->m_ptr.get( ) )[newlen] = '\0';
}

void
Sds::reserve( std::size_t new_len )
{
    size_t cap = this->m_cap;
    if ( cap >= new_len ) return;

    new_len *= 2;

    void *buf = this->m_ptr.release( );
    buf       = realloc( buf, new_len + 1 );
    if ( buf == NULL ) {
        return;
    }
    this->m_ptr.reset( buf );
    this->m_cap = new_len;
}

bool
Sds::operator==( std::string_view s ) const
{
    auto ptr = (char *)this->m_ptr.get( );
    return ptr != nullptr && strcmp( ptr, s.data( ) ) == 0;
}

namespace literals {
Sds
operator""_sds( const char *s, std::size_t )
{
    return Sds{ s };
}
}  // namespace literals

}  // namespace eve::adt
