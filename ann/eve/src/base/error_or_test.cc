#include <eve/adt/sds.h>
#include <eve/base/error_or.h>

#include <eve/testing/testing.h>

using eve::adt::Sds;
using eve::base::Error;
using eve::base::ErrorOr;

EVE_TEST( BaseErrorOr, OK )
{
    ErrorOr<int> r{ 1 };
    EVE_TEST_EXPECT( r.Ok( ), "ok" );
    EVE_TEST_EXPECT( 1 == r.Value( ), "v" );
    EVE_TEST_PASS( );
}

EVE_TEST( BaseErrorOr, Move )
{
    ErrorOr<Sds> r{ eve::Error( "hello" ) };
    EVE_TEST_EXPECT( !r.Ok( ), "ok" );
    EVE_TEST_PASS( );
}

EVE_TEST( BaseErrorOr, ErrorMove )
{
    Error        old{ "error" };
    auto         msg = old.getMsg( );
    ErrorOr<int> r{ std::move( old ) };
    EVE_TEST_EXPECT( !r.Ok( ), "ok" );

    Error err = r.ReleaseErr( );
    EVE_TEST_EXPECT_STR_EQ( msg.Data( ), err.getMsg( ), "err" );
    EVE_TEST_PASS( );
}
