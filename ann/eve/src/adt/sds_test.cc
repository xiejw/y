#include "eve/adt/sds.h"

#include "eve/testing/testing.h"

#include <cstring>
#include <iostream>

using eve::adt::Sds;

EVE_TEST( AdtSds, NewEmptyString )
{
    Sds sds{ };
    EVE_TEST_EXPECT( strcmp( sds.data( ), "" ) == 0, "expect emptr str" );
    EVE_TEST_EXPECT( sds.len( ) == 0, "len" );
    EVE_TEST_EXPECT( sds.cap( ) > 0, "cap" );
    EVE_TEST_PASS( );
}

EVE_TEST( AdtSds, NewString )
{
    Sds sds{ "hello" };
    EVE_TEST_EXPECT( strcmp( sds.data( ), "hello" ) == 0, "expect str" );
    EVE_TEST_EXPECT( sds.len( ) == 5, "len" );
    EVE_TEST_EXPECT( sds.cap( ) >= 5, "cap" );
    EVE_TEST_PASS( );
}

EVE_TEST( AdtSds, Cap )
{
    Sds sds{ 4096 };
    EVE_TEST_EXPECT( strcmp( sds.data( ), "" ) == 0, "expect str" );
    EVE_TEST_EXPECT( sds.len( ) == 0, "len" );
    EVE_TEST_EXPECT( sds.cap( ) >= 4096, "cap" );
    EVE_TEST_PASS( );
}

EVE_TEST( AdtSds, Move )
{
    Sds old_sds{ "hello" };
    Sds new_sds = std::move( old_sds );
    EVE_TEST_EXPECT( strcmp( new_sds.data( ), "hello" ) == 0, "expect str" );
    EVE_TEST_EXPECT( new_sds.len( ) == 5, "len" );
    EVE_TEST_EXPECT( new_sds.cap( ) >= 5, "cap" );
    EVE_TEST_PASS( );
}
