#include "eve/adt/sds.h"

#include <eve/testing/testing.h>

#include <cstring>
#include <iostream>

using namespace eve::adt::literals;
using eve::adt::Sds;

EVE_TEST( AdtSds, NewEmptyString )
{
    Sds sds{ };
    EVE_TEST_EXPECT( strcmp( sds.getData( ), "" ) == 0, "expect emptr str" );
    EVE_TEST_EXPECT( sds.getLen( ) == 0, "len" );
    EVE_TEST_EXPECT( sds.getCap( ) > 0, "cap" );
    EVE_TEST_PASS( );
}

EVE_TEST( AdtSds, NewString )
{
    Sds sds{ "hello" };
    EVE_TEST_EXPECT( strcmp( sds.getData( ), "hello" ) == 0, "expect str" );
    EVE_TEST_EXPECT( sds.getLen( ) == 5, "len" );
    EVE_TEST_EXPECT( sds.getCap( ) >= 5, "cap" );
    EVE_TEST_PASS( );
}

EVE_TEST( AdtSds, Cap )
{
    Sds sds{ 4096 };
    EVE_TEST_EXPECT( strcmp( sds.getData( ), "" ) == 0, "expect str" );
    EVE_TEST_EXPECT( sds.getLen( ) == 0, "len" );
    EVE_TEST_EXPECT( sds.getCap( ) >= 4096, "cap" );
    EVE_TEST_PASS( );
}

EVE_TEST( AdtSds, Move )
{
    Sds old_sds{ "hello" };
    Sds new_sds = std::move( old_sds );
    EVE_TEST_EXPECT( strcmp( new_sds.getData( ), "hello" ) == 0, "expect str" );
    EVE_TEST_EXPECT( new_sds.getLen( ) == 5, "len" );
    EVE_TEST_EXPECT( new_sds.getCap( ) >= 5, "cap" );
    EVE_TEST_PASS( );
}

EVE_TEST( AdtSds, MoveConstructor )
{
    Sds sds1{ "hello" };
    Sds sds{ std::move( sds1 ) };
    EVE_TEST_EXPECT( strcmp( sds.getData( ), "hello" ) == 0, "expect str" );
    EVE_TEST_EXPECT( sds.getLen( ) == 5, "len" );
    EVE_TEST_EXPECT( sds.getCap( ) >= 5, "cap" );
    EVE_TEST_PASS( );
}

EVE_TEST( AdtSds, ConcatSmall )
{
    Sds  sds{ 4096 };
    auto old_cap = sds.getCap( );
    EVE_TEST_EXPECT( strcmp( sds.getData( ), "" ) == 0, "expect str" );
    EVE_TEST_EXPECT( sds.getLen( ) == 0, "len" );
    EVE_TEST_EXPECT( sds.getCap( ) == old_cap, "cap" );

    sds.catPrintf( "%d", 1 );
    EVE_TEST_EXPECT( strcmp( sds.getData( ), "1" ) == 0, "expect str" );
    EVE_TEST_EXPECT( sds.getLen( ) == 1, "len" );
    EVE_TEST_EXPECT( sds.getCap( ) == old_cap, "cap" );

    sds.catPrintf( "%d", 2 );
    EVE_TEST_EXPECT( strcmp( sds.getData( ), "12" ) == 0, "expect str" );
    EVE_TEST_EXPECT( sds.getLen( ) == 2, "len" );
    EVE_TEST_EXPECT( sds.getCap( ) == old_cap, "cap" );

    sds.catPrintf( "%s", "hello" );
    EVE_TEST_EXPECT( strcmp( sds.getData( ), "12hello" ) == 0, "expect str" );
    EVE_TEST_EXPECT( sds.getLen( ) == 7, "len" );
    EVE_TEST_EXPECT( sds.getCap( ) == old_cap, "cap" );

    EVE_TEST_PASS( );
}

EVE_TEST( AdtSds, ConcatLarge )
{
    Sds  sds{ "" };
    auto old_cap = sds.getCap( );
    EVE_TEST_EXPECT( strcmp( sds.getData( ), "" ) == 0, "expect str" );
    EVE_TEST_EXPECT( sds.getLen( ) == 0, "len" );
    EVE_TEST_EXPECT( sds.getCap( ) == old_cap, "cap" );

    auto str = "a very very very very very long long long long long str";
    sds.catPrintf( "%s", str );
    EVE_TEST_EXPECT( strcmp( sds.getData( ), str ) == 0, "expect str" );
    EVE_TEST_EXPECT( sds.getLen( ) == strlen( str ), "len" );
    EVE_TEST_EXPECT( sds.getCap( ) > old_cap, "cap" );

    EVE_TEST_PASS( );
}

EVE_TEST( AdtSds, Cat )
{
    Sds sds{ "hello " };
    sds.cat( "world" );
    EVE_TEST_EXPECT( strcmp( sds.getData( ), "hello world" ) == 0,
                     "expect str" );

    EVE_TEST_PASS( );
}

EVE_TEST( AdtSds, Reserve )
{
    Sds sds{ };
    EVE_TEST_EXPECT( sds.getCap( ) < 4096, "old cap" );
    sds.reserve( 4096 );
    EVE_TEST_EXPECT( sds.getCap( ) >= 4096, "new cap" );

    EVE_TEST_PASS( );
}

EVE_TEST( AdtSds, EqOp )
{
    Sds sds{ "hello" };
    EVE_TEST_EXPECT( sds == "hello", "eq op" );
    EVE_TEST_EXPECT( sds != "hello1", "neq op" );
    EVE_TEST_EXPECT( sds == std::string{ "hello" }, "eq op" );
    EVE_TEST_EXPECT( sds != std::string{ "hello1" }, "neq op" );

    EVE_TEST_PASS( );
}

EVE_TEST( AdtSds, Literal )
{
    auto sds = "hello"_sds;
    EVE_TEST_EXPECT( sds == "hello", "eq op" );
    EVE_TEST_EXPECT( sds.getSize( ) == 5, "len" );
    EVE_TEST_EXPECT( sds.getCap( ) >= 5, "cap" );

    EVE_TEST_PASS( );
}
