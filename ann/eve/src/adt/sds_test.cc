#include "eve/adt/sds.h"

#include "eve/testing/testing.h"

#include <cstring>
#include <iostream>

using eve::adt::Sds;

EVE_TEST( AdtSds, NewString )
{
    Sds sds{ };
    EVE_TEST_EXPECT( strcmp( sds.data( ), "" ) == 0, "expect emptr str" );
    EVE_TEST_PASS( );
}
