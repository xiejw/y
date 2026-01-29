#include <eve/base/error.h>

#include <eve/testing/testing.h>

using eve::base::Error;

EVE_TEST( BaseError, OK )
{
        Error Err{ };
        EVE_TEST_EXPECT( Err.isOk( ), "ok" );
        EVE_TEST_PASS( );
}

EVE_TEST( BaseError, Error )
{
        Error Err{ "%s", "error" };
        EVE_TEST_EXPECT( !Err.isOk( ), "ok" );
        EVE_TEST_EXPECT( Err.getKind( ) == Error::ERROR, "err" );
        EVE_TEST_EXPECT_STR_EQ( "  > error\n", Err.getMsg( ), "err" );
        EVE_TEST_PASS( );
}

EVE_TEST( BaseError, ErrorMove )
{
        Error err1{ "%s", "error" };
        Error Err{ std::move( err1 ) };
        EVE_TEST_EXPECT( !Err.isOk( ), "ok" );
        EVE_TEST_EXPECT_STR_EQ( "  > error\n", Err.getMsg( ), "err" );
        EVE_TEST_PASS( );
}

EVE_TEST( BaseError, ErrorNote )
{
        Error Err{ "%s", "error1" };
        Err.emitNote( "%s", "error2" );
        EVE_TEST_EXPECT( !Err.isOk( ), "ok" );
        EVE_TEST_EXPECT_STR_EQ( "  > error2\n  > error1\n", Err.getMsg( ),
                                "err" );
        EVE_TEST_PASS( );
}
