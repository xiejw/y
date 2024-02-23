#include <eve/base/error_or.h>

#include <eve/testing/testing.h>

using eve::base::Error;
using eve::base::ErrorOr;

EVE_TEST( BaseErrorOr, OK )
{
    ErrorOr<int> r{ 1 };
    EVE_TEST_EXPECT( r.Ok( ), "ok" );
    EVE_TEST_EXPECT( !r.Error( ), "err" );
    EVE_TEST_PASS( );
}

EVE_TEST( BaseErrorOr, Error )
{
    ErrorOr<int> r{ Error{ "error" } };
    EVE_TEST_EXPECT( !r.Ok( ), "ok" );
    EVE_TEST_EXPECT( r.Error( ), "err" );
    EVE_TEST_PASS( );
}
