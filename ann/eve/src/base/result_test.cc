#include <eve/base/result.h>

#include <eve/adt/sds.h>
#include <eve/testing/testing.h>

using eve::adt::Sds;
using eve::base::Error;
using eve::base::Result;

EVE_TEST( BaseResult, OK )
{
    auto r = Result<int>::ofValue( 1 );
    EVE_TEST_EXPECT( r.isOk( ), "ok" );
    EVE_TEST_EXPECT( 1 == r.getValue( ), "v" );
    EVE_TEST_PASS( );
}

EVE_TEST( BaseResult, ImplicitValue )
{
    auto r = Result<Sds>::ofValue( Sds{ "hello" } );
    EVE_TEST_EXPECT( r.isOk( ), "ok" );
    EVE_TEST_EXPECT_STR_EQ( "hello", r.getValue( ), "value" );
    EVE_TEST_EXPECT_STR_EQ( "hello", r.getValue( ).getData( ), "value" );
    EVE_TEST_PASS( );
}

EVE_TEST( BaseResult, Error )
{
    auto r = Result<Sds>::ofError( Error( "hello" ) );
    EVE_TEST_EXPECT( !r.isOk( ), "ok" );
    EVE_TEST_PASS( );
}

EVE_TEST( BaseResult, ImplicitErrorMsg )
{
    auto r = Result<Sds>::ofError( "hello" );
    EVE_TEST_EXPECT( !r.isOk( ), "ok" );
    EVE_TEST_PASS( );
}
