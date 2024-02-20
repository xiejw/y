#include "eve/adt/sds.h"

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

}  // namespace eve::adt
