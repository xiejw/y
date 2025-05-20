#include "testing/testing.h"

#include "adt/vec.h"

static char *
test_init( void )
{
        vec_t( int ) v = vecNew( );
        ASSERT_TRUE( "size is 0", vecSize( v ) == 0 );
        ASSERT_TRUE( "cap is 0", vecCap( v ) == 0 );
        ASSERT_TRUE( "isempty", vecIsEmpty( v ) == 1 );
        vecFree( v );
        return NULL;
}

static char *
test_push_back( void )
{
        vec_t( int ) v = vecNew( );
        ASSERT_TRUE( "no err", OK == vecPushBack( &v, 123 ) );
        ASSERT_TRUE( "v[0] is 123", v[0] == 123 );
        ASSERT_TRUE( "size is 1", vecSize( v ) == 1 );
        ASSERT_TRUE( "cap is >0", vecCap( v ) > 0 );
        vecFree( v );
        return NULL;
}

static char *
test_pop_back( void )
{
        vec_t( int ) v = vecNew( );
        ASSERT_TRUE( "no err", OK == vecPushBack( &v, 123 ) );
        size_t old_cap = vecCap( v );

        ASSERT_TRUE( "pop back", 123 == vecPopBack( v ) );
        ASSERT_TRUE( "size is 0", vecSize( v ) == 0 );
        ASSERT_TRUE( "cap", old_cap == vecCap( v ) );
        vecFree( v );
        return NULL;
}

static char *
test_extend( void )
{
        vec_t( int ) v1 = vecNew( );
        vec_t( int ) v2 = vecNew( );
        vecPushBack( &v1, 789 );
        vecPushBack( &v2, 123 );
        vecPushBack( &v2, 456 );

        ASSERT_TRUE( "no err", OK == vecExtend( &v1, v2 ) );

        ASSERT_TRUE( "size is 3", 3 == vecSize( v1 ) );
        ASSERT_TRUE( "cap", 3 <= vecCap( v1 ) );
        ASSERT_TRUE( "ele[0]", 789 == v1[0] );
        ASSERT_TRUE( "ele[1]", 123 == v1[1] );
        ASSERT_TRUE( "ele[2]", 456 == v1[2] );

        vecFree( v1 );
        vecFree( v2 );
        return NULL;
}

static char *
test_extend_null( void )
{
        vec_t( int ) v1 = vecNew( );
        vec_t( int ) v2 = vecNew( );
        vecPushBack( &v2, 123 );
        vecPushBack( &v2, 456 );

        ASSERT_TRUE( "no err", OK == vecExtend( &v1, v2 ) );

        ASSERT_TRUE( "size is 2", 2 == vecSize( v1 ) );
        ASSERT_TRUE( "cap", 2 <= vecCap( v1 ) );
        ASSERT_TRUE( "ele[0]", 123 == v1[0] );
        ASSERT_TRUE( "ele[1]", 456 == v1[1] );

        vecFree( v1 );
        vecFree( v2 );
        return NULL;
}

static char *
test_reserve( void )
{
        vec_t( int ) v = vecNew( );
        vecReserve( &v, 100 );
        ASSERT_TRUE( "size is 0", vecSize( v ) == 0 );
        ASSERT_TRUE( "cap is 100", vecCap( v ) == 100 );
        vecFree( v );
        return NULL;
}

static char *
test_set_size( void )
{
        vec_t( size_t ) v = vecNew( );
        vecReserve( &v, 100 );
        ASSERT_TRUE( "size is 0", vecSize( v ) == 0 );
        ASSERT_TRUE( "cap is 100", vecCap( v ) == 100 );

        vecSetSize( v, 10 );
        ASSERT_TRUE( "size is 10", vecSize( v ) == 10 );
        ASSERT_TRUE( "cap is 100", vecCap( v ) == 100 );

        vecFree( v );
        return NULL;
}

static char *
test_grow( void )
{
        vec_t( int ) v = vecNew( );
        ASSERT_TRUE( "no err", OK == vecReserve( &v, 2 ) );
        ASSERT_TRUE( "size is 0", vecSize( v ) == 0 );
        ASSERT_TRUE( "cap is 2", vecCap( v ) == 2 );

        ASSERT_TRUE( "no err", OK == vecPushBack( &v, 0 ) );
        ASSERT_TRUE( "v[i]", v[0] == 0 );
        ASSERT_TRUE( "size", vecSize( v ) == 1 );
        ASSERT_TRUE( "cap", vecCap( v ) == 2 );

        ASSERT_TRUE( "no err", OK == vecPushBack( &v, 1 ) );
        ASSERT_TRUE( "v[i]", v[1] == 1 );
        ASSERT_TRUE( "size", vecSize( v ) == 2 );
        ASSERT_TRUE( "cap", vecCap( v ) == 2 );

        ASSERT_TRUE( "no err", OK == vecPushBack( &v, 2 ) );
        ASSERT_TRUE( "v[i]", v[2] == 2 );
        ASSERT_TRUE( "size", vecSize( v ) == 3 );
        ASSERT_TRUE( "cap", vecCap( v ) == 4 );

        vecFree( v );
        return NULL;
}

DECLARE_TEST_SUITE( adt_vec )
{
        RUN_TEST( test_init );
        RUN_TEST( test_push_back );
        RUN_TEST( test_pop_back );
        RUN_TEST( test_extend );
        RUN_TEST( test_extend_null );
        RUN_TEST( test_reserve );
        RUN_TEST( test_set_size );
        RUN_TEST( test_grow );
        return NULL;
}
