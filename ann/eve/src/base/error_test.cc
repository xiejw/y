#include <eve/base/error.h>

#include <eve/testing/testing.h>

using eve::base::Error;

EVE_TEST( BaseError, OK )
{
    Error err{ };
    EVE_TEST_EXPECT( err.Ok( ), "ok" );
    EVE_TEST_PASS( );
}

EVE_TEST( BaseError, Error )
{
    Error err{ "%s", "error" };
    EVE_TEST_EXPECT( !err.Ok( ), "ok" );
    EVE_TEST_EXPECT_STR_EQ( "  > error\n", err.Msg( ), "err" );
    EVE_TEST_PASS( );
}

EVE_TEST( BaseError, ErrorNote )
{
    Error err{ "%s", "error1" };
    err.EmitNote( "%s", "error2" );
    EVE_TEST_EXPECT( !err.Ok( ), "ok" );
    EVE_TEST_EXPECT_STR_EQ( "  > error2\n  > error1\n", err.Msg( ), "err" );
    EVE_TEST_PASS( );
}
