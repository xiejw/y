#include "testing/testing.h"

#include <base/mm.h>

//-----------------------------------------------------------------------------
// stack apis
//-----------------------------------------------------------------------------

static char *
test_stk_empty( void )
{
    struct arr *arr = arrStackNew( sizeof( int ), 2, 4 );
    ASSERT_TRUE( "slot size", arr->slot_byte_size == sizeof( int ) );
    ASSERT_TRUE( "slots/line", arr->slots_per_line == 2 );
    ASSERT_TRUE( "num_line", vecSize( arr->lines ) == 2 );
    ASSERT_TRUE( "cap", arr->cap == 4 );
    ASSERT_TRUE( "sbits", arr->sbits == 1 );
    ASSERT_TRUE( "mask", arr->mask == 1 );
    arrFree( arr );
    return NULL;
}

static char *
test_stk_alloc( void )
{
    struct arr *arr = arrStackNew( sizeof( int ), 3, 7 );
    ASSERT_TRUE( "slot size", arr->slot_byte_size == sizeof( int ) );
    ASSERT_TRUE( "slots/line", arr->slots_per_line == 4 );
    ASSERT_TRUE( "num_line", vecSize( arr->lines ) == 2 );
    ASSERT_TRUE( "cap", arr->cap == 8 );
    ASSERT_TRUE( "sbits", arr->sbits == 2 );
    ASSERT_TRUE( "mask", arr->mask == 3 );
    arrFree( arr );
    return NULL;
}

static char *
test_stk_push_pop( void )
{
    struct arr *arr = arrStackNew( sizeof( int ), 4, 16 );
    ASSERT_TRUE( "slot size", arr->slot_byte_size == sizeof( int ) );
    ASSERT_TRUE( "slots/line", arr->slots_per_line == 4 );
    ASSERT_TRUE( "num_line", vecSize( arr->lines ) == 4 );
    ASSERT_TRUE( "cap", arr->cap == 16 );
    ASSERT_TRUE( "sbits", arr->sbits == 2 );
    ASSERT_TRUE( "mask", arr->mask == 3 );

    for ( int i = 0; i < 16; i++ ) {
        *(int *)( arrPush( arr ) ) = i;
    }
    for ( int i = 15; i >= 0; i-- ) {
        ASSERT_TRUE( "pop", *(int *)( arrPop( arr ) ) == i );
    }

    ASSERT_TRUE( "pop empty", arrPop( arr ) == NULL );
    arrFree( arr );
    return NULL;
}

static char *
test_stk_grow_cap( void )
{
    struct arr *arr = arrStackNew( sizeof( int ), 2, 4 );
    ASSERT_TRUE( "slot size", arr->slot_byte_size == sizeof( int ) );
    ASSERT_TRUE( "slots/line", arr->slots_per_line == 2 );
    ASSERT_TRUE( "num_line", vecSize( arr->lines ) == 2 );
    ASSERT_TRUE( "cap", arr->cap == 4 );
    ASSERT_TRUE( "sbits", arr->sbits == 1 );
    ASSERT_TRUE( "mask", arr->mask == 1 );

    for ( int i = 0; i < 16; i++ ) {
        *(int *)( arrPush( arr ) ) = i;
    }
    for ( int i = 15; i >= 0; i-- ) {
        ASSERT_TRUE( "pop", *(int *)( arrPop( arr ) ) == i );
    }

    ASSERT_TRUE( "pop empty", arrPop( arr ) == NULL );

    // check properties again after growing
    //
    // no change
    ASSERT_TRUE( "slot size", arr->slot_byte_size == sizeof( int ) );
    ASSERT_TRUE( "slots/line", arr->slots_per_line == 2 );
    ASSERT_TRUE( "sbits", arr->sbits == 1 );
    ASSERT_TRUE( "mask", arr->mask == 1 );

    // change
    ASSERT_TRUE( "num_line", vecSize( arr->lines ) == 8 );
    ASSERT_TRUE( "cap", arr->cap == 16 );

    arrFree( arr );
    return NULL;
}

//-----------------------------------------------------------------------------
// stack macros
//-----------------------------------------------------------------------------

static char *
test_stk_int( void )
{
    struct arr *arr = ARR_STACK_NEW( int );
    ARR_PUSH( arr, 3 );
    ASSERT_TRUE( "empty", 0 == arrIsEmpty( arr ) );

    int v;
    ARR_POP( v, arr );

    ASSERT_TRUE( "v", 3 == v );
    ASSERT_TRUE( "empty", 1 == arrIsEmpty( arr ) );
    arrFree( arr );
    return NULL;
}

static char *
test_stk_int_ptr( void )
{
    struct arr *arr = ARR_STACK_NEW( int * );
    int         x   = 3;
    ARR_PUSH( arr, &x );
    ASSERT_TRUE( "empty", 0 == arrIsEmpty( arr ) );

    int *v;
    ARR_POP( v, arr );

    ASSERT_TRUE( "v", 3 == *v );
    ASSERT_TRUE( "empty", 1 == arrIsEmpty( arr ) );
    arrFree( arr );
    return NULL;
}

DECLARE_TEST_SUITE( base_mm )
{
    RUN_TEST( test_stk_empty );
    RUN_TEST( test_stk_alloc );
    RUN_TEST( test_stk_push_pop );
    RUN_TEST( test_stk_grow_cap );

    RUN_TEST( test_stk_int );
    RUN_TEST( test_stk_int_ptr );
    return NULL;
}
