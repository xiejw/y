#include "eve/adt/sds.h"

#include "eve/testing/testing.h"

#include <cstring>
#include <iostream>

using eve::adt::Sds;

EVE_TEST( AdtSds, NewEmptyString )
{
    Sds sds{ };
    EVE_TEST_EXPECT( strcmp( sds.Data( ), "" ) == 0, "expect emptr str" );
    EVE_TEST_EXPECT( sds.Len( ) == 0, "len" );
    EVE_TEST_EXPECT( sds.Cap( ) > 0, "cap" );
    EVE_TEST_PASS( );
}

EVE_TEST( AdtSds, NewString )
{
    Sds sds{ "hello" };
    EVE_TEST_EXPECT( strcmp( sds.Data( ), "hello" ) == 0, "expect str" );
    EVE_TEST_EXPECT( sds.Len( ) == 5, "len" );
    EVE_TEST_EXPECT( sds.Cap( ) >= 5, "cap" );
    EVE_TEST_PASS( );
}

EVE_TEST( AdtSds, Cap )
{
    Sds sds{ 4096 };
    EVE_TEST_EXPECT( strcmp( sds.Data( ), "" ) == 0, "expect str" );
    EVE_TEST_EXPECT( sds.Len( ) == 0, "len" );
    EVE_TEST_EXPECT( sds.Cap( ) >= 4096, "cap" );
    EVE_TEST_PASS( );
}

EVE_TEST( AdtSds, Move )
{
    Sds old_sds{ "hello" };
    Sds new_sds = std::move( old_sds );
    EVE_TEST_EXPECT( strcmp( new_sds.Data( ), "hello" ) == 0, "expect str" );
    EVE_TEST_EXPECT( new_sds.Len( ) == 5, "len" );
    EVE_TEST_EXPECT( new_sds.Cap( ) >= 5, "cap" );
    EVE_TEST_PASS( );
}

EVE_TEST( AdtSds, ConcatSmall )
{
    Sds  sds{ 4096 };
    auto old_cap = sds.Cap( );
    EVE_TEST_EXPECT( strcmp( sds.Data( ), "" ) == 0, "expect str" );
    EVE_TEST_EXPECT( sds.Len( ) == 0, "len" );
    EVE_TEST_EXPECT( sds.Cap( ) == old_cap, "cap" );

    sds.CatPrintf( "%d", 1 );
    EVE_TEST_EXPECT( strcmp( sds.Data( ), "1" ) == 0, "expect str" );
    EVE_TEST_EXPECT( sds.Len( ) == 1, "len" );
    EVE_TEST_EXPECT( sds.Cap( ) == old_cap, "cap" );

    sds.CatPrintf( "%d", 2 );
    EVE_TEST_EXPECT( strcmp( sds.Data( ), "12" ) == 0, "expect str" );
    EVE_TEST_EXPECT( sds.Len( ) == 2, "len" );
    EVE_TEST_EXPECT( sds.Cap( ) == old_cap, "cap" );

    sds.CatPrintf( "%s", "hello" );
    EVE_TEST_EXPECT( strcmp( sds.Data( ), "12hello" ) == 0, "expect str" );
    EVE_TEST_EXPECT( sds.Len( ) == 7, "len" );
    EVE_TEST_EXPECT( sds.Cap( ) == old_cap, "cap" );

    EVE_TEST_PASS( );
}

EVE_TEST( AdtSds, ConcatLarge )
{
    Sds  sds{ "" };
    auto old_cap = sds.Cap( );
    EVE_TEST_EXPECT( strcmp( sds.Data( ), "" ) == 0, "expect str" );
    EVE_TEST_EXPECT( sds.Len( ) == 0, "len" );
    EVE_TEST_EXPECT( sds.Cap( ) == old_cap, "cap" );

    auto str = "a very very very very very long long long long long str";
    sds.CatPrintf( "%s", str );
    EVE_TEST_EXPECT( strcmp( sds.Data( ), str ) == 0, "expect str" );
    EVE_TEST_EXPECT( sds.Len( ) == strlen( str ), "len" );
    EVE_TEST_EXPECT( sds.Cap( ) > old_cap, "cap" );

    EVE_TEST_PASS( );
}

EVE_TEST( AdtSds, Cat )
{
    Sds sds{ "hello " };
    sds.Cat( "world" );
    EVE_TEST_EXPECT( strcmp( sds.Data( ), "hello world" ) == 0, "expect str" );

    EVE_TEST_PASS( );
}

EVE_TEST( AdtSds, Reserve )
{
    Sds sds{ };
    EVE_TEST_EXPECT( sds.Cap( ) < 4096, "old cap" );
    sds.Reserve( 4096 );
    EVE_TEST_EXPECT( sds.Cap( ) >= 4096, "new cap" );

    EVE_TEST_PASS( );
}
