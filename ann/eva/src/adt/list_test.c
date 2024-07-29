#include "testing/testing.h"

#include "adt/list.h"

#include <stdlib.h>

static char *
test_new( )
{
    struct list *l = listNew( );

    ASSERT_TRUE( "len", 0 == listLen( l ) );
    ASSERT_TRUE( "head", NULL == listHead( l ) );
    ASSERT_TRUE( "tail", NULL == listTail( l ) );

    listFree( l );
    return NULL;
}

static char *
test_free( )
{
    struct list *l = listNew( );
    listSetFreeFn( l, free );

    void *p1 = malloc( 128 );
    void *p2 = malloc( 128 );

    listAppend( l, p1 );
    listAppend( l, p2 );

    ASSERT_TRUE( "len", 2 == listLen( l ) );
    ASSERT_TRUE( "head", p1 == listNodeData( listHead( l ) ) );
    ASSERT_TRUE( "tail", p2 == listNodeData( listTail( l ) ) );

    listFree( l );
    return NULL;
}

static char *
test_iter( )
{
    struct list *l  = listNew( );
    char        *p1 = "str1";
    char        *p2 = "str2";

    listAppend( l, p1 );
    listAppend( l, p2 );

    struct list_iter *iter = listNewIter( l, 1 );
    struct list_node *n;

    {  // forward
        size_t i = 0;
        while ( ( n = listNext( iter ) ) != NULL ) {
            switch ( i++ ) {
            case 0:
                ASSERT_TRUE( "n0", p1 == listNodeData( n ) );
                break;
            case 1:
                ASSERT_TRUE( "n1", p2 == listNodeData( n ) );
                break;
            default:
                ASSERT_TRUE( "not expected", 0 );
            }
        }

        ASSERT_TRUE( "iter len", i == listLen( l ) );
    }

    {  // backward
        size_t i = 0;
        listRewindTail( l, iter );
        while ( ( n = listNext( iter ) ) != NULL ) {
            switch ( i++ ) {
            case 0:
                ASSERT_TRUE( "n1", p2 == listNodeData( n ) );
                break;
            case 1:
                ASSERT_TRUE( "n0", p1 == listNodeData( n ) );
                break;
            default:
                ASSERT_TRUE( "not expected", 0 );
            }
        }

        ASSERT_TRUE( "iter len", i == listLen( l ) );
    }

    listFreeIter( iter );
    listFree( l );
    return NULL;
}

DECLARE_TEST_SUITE( adt_list )
{
    RUN_TEST( test_new );
    RUN_TEST( test_free );
    RUN_TEST( test_iter );
    return NULL;
}
