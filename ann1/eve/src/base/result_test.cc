#include <eve/base/result.h>

#include <eve/adt/sds.h>
#include <eve/testing/testing.h>

using eve::adt::Sds;
using eve::base::Error;
using eve::base::Result;

EVE_TEST( BaseResult, ValueInt )
{
        auto r = Result<int>::ofValue( 1 );
        EVE_TEST_EXPECT( r.isOk( ), "ok" );
        EVE_TEST_EXPECT( 1 == r.getValue( ), "v" );
        EVE_TEST_PASS( );
}

EVE_TEST( BaseResult, ValueSds )
{
        auto r = Result<Sds>::ofValue( Sds{ "hello" } );
        EVE_TEST_EXPECT( r.isOk( ), "ok" );
        EVE_TEST_EXPECT_STR_EQ( "hello", r.getValue( ), "value" );
        EVE_TEST_EXPECT_STR_EQ( "hello", r.getValue( ).getData( ), "value" );
        EVE_TEST_PASS( );
}

EVE_TEST( BaseResult, ValueMove )
{
        auto r = Result<Sds>::ofValue( Sds{ "hello" } );
        EVE_TEST_EXPECT( r.isOk( ), "ok" );
        EVE_TEST_EXPECT_STR_EQ( "hello", r.getValue( ), "value" );

        r.getValue( ).catPrintf( " %s", "world" );
        EVE_TEST_EXPECT_STR_EQ( "hello world", r.getValue( ), "value" );

        auto s = r.releaseValue( );
        EVE_TEST_EXPECT_STR_EQ( "hello world", s, "value" );

        EVE_TEST_PASS( );
}

EVE_TEST( BaseResult, Error )
{
        auto r = Result<Sds>::ofError( Error( "hello" ) );
        EVE_TEST_EXPECT( !r.isOk( ), "ok" );
        EVE_TEST_PASS( );
}

EVE_TEST( BaseResult, ErrorFromStr )
{
        auto r = Result<Sds>::ofError( "hello" );
        EVE_TEST_EXPECT( !r.isOk( ), "ok" );
        EVE_TEST_PASS( );
}

EVE_TEST( BaseResult, ErrorEmitNote )
{
        auto r = Result<Sds>::ofError( "hello" );
        EVE_TEST_EXPECT( !r.isOk( ), "ok" );
        r.getError( ).emitNote( "world" );

        Error expect{ "hello" };
        expect.emitNote( "world" );
        EVE_TEST_EXPECT_STR_EQ( expect.getMsg( ), r.getError( ).getMsg( ),
                                "error msg" );

        EVE_TEST_PASS( );
}

EVE_TEST( BaseResult, ErrorMove )
{
        auto r = Result<Sds>::ofError( "hello" );
        EVE_TEST_EXPECT( !r.isOk( ), "ok" );
        r.getError( ).emitNote( "world" );
        auto s = r.releaseError( );

        Error expect{ "hello" };
        expect.emitNote( "world" );
        EVE_TEST_EXPECT_STR_EQ( expect.getMsg( ), s.getMsg( ), "error msg" );

        EVE_TEST_PASS( );
}
