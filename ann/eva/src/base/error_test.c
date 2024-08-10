#include <testing/testing.h>
#include <testing/testing_macros.h>

#include "base/error.h"

// eva
#include <adt/sds.h>

static char *
test_non_zero( void )
{
    error_t err = errNewWithNote( -123, "test error" );
    ASSERT_TRUE( "err  code", -123 == err );
    ASSERT_TRUE( "err same", err == errNum( ) );
    errFree( );
    return NULL;
}

static char *
test_dump_sds( void )
{
    error_t err = errNewWithNote( -123, "test error" );
    ASSERT_TRUE( "err  code", -123 == err );
    ASSERT_TRUE( "err same", err == errNum( ) );

    sds_t s = sdsEmpty( );
    errDumpSds( &s, "unexpected error: %d", errNum( ) );
    errFree( );

    ASSERT_STR_EQ( "err msg", "unexpected error: -123\n  > test error\n", s );
    sdsFree( s );
    return NULL;
}

#ifdef PTHREAD
#include <pthread.h>

static void *
test_fn_for_pthread( void *unused )
{
    error_t err = errNewWithNote( -123, "test error" );
    ASSERT_TRUE( "err  code", -123 == err );
    ASSERT_TRUE( "err same", err == errNum( ) );
    errFree( );
    return NULL;
}

static char *
test_pthreads( void )
{
    error_t   err = errNewWithNote( -456, "test error" );
    pthread_t p;
    pthread_create( &p, NULL, test_fn_for_pthread, NULL );
    pthread_join( p, NULL );
    ASSERT_TRUE( "err  code", -456 == err );
    ASSERT_TRUE( "err same", err == errNum( ) );
    errFree( );
    return NULL;
}
#endif  // PTHREAD

DECLARE_TEST_SUITE( base_error )
{
    RUN_TEST( test_non_zero );
    RUN_TEST( test_dump_sds );
#ifdef PTHREAD
    RUN_TEST( test_pthreads );
#endif  // PTHREAD
    return NULL;
}
