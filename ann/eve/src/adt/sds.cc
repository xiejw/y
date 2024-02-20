#include "eve/adt/sds.h"

namespace eve ::adt {

namespace {

constexpr int SDS_DEFAULT_ALLOCATE_SPACE = 16;

}

void
Sds::Init( const void *init, size_t len, size_t cap )
{
    cap = cap > SDS_DEFAULT_ALLOCATE_SPACE ? cap : SDS_DEFAULT_ALLOCATE_SPACE;
    void *buf = malloc( cap + 1 );

    if ( len ) {
        if ( init == nullptr )
            memset( buf, 0, len );
        else
            memcpy( buf, init, len );
    }

    ( (std::byte *)( buf ) )[len] = std::byte( 0 );

    this->len   = len;
    this->alloc = cap;
    this->ptr.reset( buf );
}

Sds::Sds( std::string_view v )
{
    std::size_t len      = v.size( );
    std::size_t init_cap = len;

    const void *init = v.data( );

    Init( init, len, init_cap );
}
}  // namespace eve::adt
