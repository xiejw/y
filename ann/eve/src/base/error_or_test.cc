#include <eve/adt/sds.h>
#include <eve/base/error_or.h>

#include <eve/testing/testing.h>

using eve::adt::Sds;
using eve::base::Error;
using eve::base::ErrorOr;

EVE_TEST( BaseErrorOr, OK )
{
    auto r = ErrorOr<int>::withValue( 1 );
    EVE_TEST_EXPECT( r.isOk( ), "ok" );
    EVE_TEST_EXPECT( 1 == r.Value( ), "v" );
    EVE_TEST_PASS( );
}

EVE_TEST( BaseErrorOr, ImplicitValue )
{
    auto r = ErrorOr<Sds>::withValue( Sds{ "hello" } );
    EVE_TEST_EXPECT( r.isOk( ), "ok" );
    EVE_TEST_EXPECT_STR_EQ( "hello", r.Value( ), "value" );
    EVE_TEST_EXPECT_STR_EQ( "hello", r.Value( ).getData( ), "value" );
    EVE_TEST_PASS( );
}

EVE_TEST( BaseErrorOr, Error )
{
    auto r = ErrorOr<Sds>::withError( Error( "hello" ) );
    EVE_TEST_EXPECT( !r.isOk( ), "ok" );
    EVE_TEST_PASS( );
}

EVE_TEST( BaseErrorOr, ImplicitErrorMsg )
{
    auto r = ErrorOr<Sds>::withError( "hello" );
    EVE_TEST_EXPECT( !r.isOk( ), "ok" );
    EVE_TEST_PASS( );
}
