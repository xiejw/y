#include <testing/testing.h>
#include <testing/testing_macros.h>

#include <string.h>

// eva
#include <adt/dict.h>
#include <adt/dict_int.h>
#include <adt/dict_ptr.h>
#include <adt/dict_str.h>

// -----------------------------------------------------------------------------
// high level api
// -----------------------------------------------------------------------------

static char *
test_high_level_str_no_dup_free_fn( void )
{
        struct dict t;
        dictStrInit( &t, NULL, NULL, NULL );

        // get
        ASSERT_TRUE( "no data", NULL == dictStrGetData( &t, "hello" ) );

        // put and get
        dictStrPutData( &t, "hello", "hi" );
        ASSERT_STR_EQ( "data match", "hi", dictStrGetData( &t, "hello" ) );

        // delete
        dictStrDel( &t, "not_exited" );
        dictStrDel( &t, "hello" );
        ASSERT_TRUE( "no data", NULL == dictStrGetData( &t, "hello" ) );

        dictStrDeinit( &t );
        return NULL;
}

static char *
test_high_level_str_dup_free_fn( void )
{
        struct dict t;
        dictStrInit( &t, NULL, dupFnStr, freeFnStr );

        sds_t key = sdsNew( "hello" );

        // get
        ASSERT_TRUE( "no data", NULL == dictStrGetData( &t, key ) );
        sdsFree( key );

        key     = sdsNew( "hello" );  // new one
        sds_t v = sdsNew( "hi" );     // new one

        // put and get
        dictStrPutData( &t, key, v );

        sdsFree( key );  // free as it is dupped.
        sdsFree( v );    // free as it is dupped.

        ASSERT_STR_EQ( "data match", "hi", dictStrGetData( &t, "hello" ) );

        // delete
        dictStrDel( &t, "not_exited" );
        dictStrDel( &t, "hello" );
        ASSERT_TRUE( "no data", NULL == dictStrGetData( &t, "hello" ) );

        dictStrDeinit( &t );
        return NULL;
}

static char *
test_high_level_ptr_no_dup_free_fn( void )
{
        struct dict t;
        dictPtrInit( &t, NULL, NULL, NULL );

        int data[3];

        // get
        ASSERT_TRUE( "no data", NULL == dictPtrGetData( &t, data ) );

        // put and get
        dictPtrPutData( &t, data, "hi" );
        ASSERT_STR_EQ( "data match", "hi", dictPtrGetData( &t, data ) );

        // delete
        dictPtrDel( &t, "not_exited" );
        dictPtrDel( &t, data );
        ASSERT_TRUE( "no data", NULL == dictPtrGetData( &t, data ) );

        dictPtrDeinit( &t );
        return NULL;
}

static char *
test_high_level_ptr_dup_free_fn( void )
{
        struct dict t;
        dictPtrInit( &t, NULL, dupFnStr, freeFnStr );

        int data[3];

        // get
        ASSERT_TRUE( "no data", NULL == dictPtrGetData( &t, data ) );

        sds_t v = sdsNew( "hi" );  // new one

        // put and get
        dictPtrPutData( &t, data, v );

        sdsFree( v );  // free as it is dupped.

        ASSERT_STR_EQ( "data match", "hi", dictPtrGetData( &t, data ) );

        // delete
        dictPtrDel( &t, "not_exited" );
        dictPtrDel( &t, data );
        ASSERT_TRUE( "no data", NULL == dictPtrGetData( &t, data ) );

        dictPtrDeinit( &t );
        return NULL;
}

static char *
test_high_level_i64_no_dup_free_fn( void )
{
        struct dict t;
        dictI64Init( &t, NULL, NULL, NULL );

        // get
        ASSERT_TRUE( "no data", NULL == dictI64GetData( &t, 123 ) );

        // put and get
        dictI64PutData( &t, 123, "hi" );
        ASSERT_STR_EQ( "data match", "hi", dictI64GetData( &t, 123 ) );

        // delete
        dictI64Del( &t, 456 );  // not existed
        dictI64Del( &t, 123 );
        ASSERT_TRUE( "no data", NULL == dictI64GetData( &t, 123 ) );

        dictI64Deinit( &t );
        return NULL;
}

static char *
test_high_level_i64_dup_free_fn( void )
{
        struct dict t;
        dictI64Init( &t, NULL, dupFnStr, freeFnStr );

        // get
        ASSERT_TRUE( "no data", NULL == dictI64GetData( &t, 123 ) );

        sds_t v = sdsNew( "hi" );

        // put and get
        dictI64PutData( &t, 123, v );

        sdsFree( v );  // free as it is dupped.

        ASSERT_STR_EQ( "data match", "hi", dictI64GetData( &t, 123 ) );

        // delete
        dictI64Del( &t, 456 );  // not existed
        dictI64Del( &t, 123 );
        ASSERT_TRUE( "no data", NULL == dictI64GetData( &t, 123 ) );

        dictI64Deinit( &t );
        return NULL;
}

// -----------------------------------------------------------------------------
// low level api: str ptr
// -----------------------------------------------------------------------------
static struct dict_type ty_str = {
    .hashFn    = hashFnStr,
    .keyCmpFn  = keyCmpFnStr,
    .keyDupFn  = dupFnStr,
    .valDupFn  = NULL,
    .keyFreeFn = freeFnStr,
    .valFreeFn = NULL,
};

static char *
test_new( void )
{
        struct dict *t = dictNew( &ty_str, NULL );
        dictFree( t );
        return NULL;
}

static char *
test_init( void )
{
        struct dict t;
        dictInit( &t, &ty_str, NULL );
        dictDeinit( &t );
        return NULL;
}

static char *
test_add_and_find( void )
{
        struct dict *t = dictNew( &ty_str, NULL );

        ASSERT_TRUE( "val null", NULL == dictFind( t, VDATA( "bc" ) ) );
        struct dict_entry *en = dictAddOrFind( t, VDATA( "abc" ), NULL );
        dictSetU64( en, 123 );
        ASSERT_TRUE( "val",
                     dictGetU64( dictFind( t, VDATA( "abc" ) ) ) == 123 );
        ASSERT_TRUE( "val null", NULL == dictFind( t, VDATA( "bc" ) ) );

        dictFree( t );
        return NULL;
}

static char *
test_delete( void )
{
        struct dict *t = dictNew( &ty_str, NULL );

        struct dict_entry *en = dictAddOrFind( t, VDATA( "abc" ), NULL );
        dictSetU64( en, 123 );
        ASSERT_TRUE( "val",
                     dictGetU64( dictFind( t, VDATA( "abc" ) ) ) == 123 );
        ASSERT_TRUE( "del", 1 == dictDelete( t, VDATA( "abc" ) ) );  // deleted
        ASSERT_TRUE( "val", NULL == dictFind( t, VDATA( "abc" ) ) );
        ASSERT_TRUE( "del",
                     0 == dictDelete( t, VDATA( "abc" ) ) );  // not found

        dictFree( t );
        return NULL;
}

static char *
test_add_and_find_existed( void )
{
        int          existed;
        struct dict *t = dictNew( &ty_str, NULL );

        struct dict_entry *en = dictAddOrFind( t, VDATA( "abc" ), &existed );
        ASSERT_TRUE( "not existed", existed == 0 );
        dictSetU64( en, 123 );

        en = dictAddOrFind( t, VDATA( "abc" ), &existed );
        ASSERT_TRUE( "existed", existed == 1 );

        ASSERT_TRUE( "same val",
                     dictGetU64( dictFind( t, VDATA( "abc" ) ) ) == 123 );
        dictFree( t );
        return NULL;
}

static char *
test_expand( void )
{
        struct dict *t = dictNew( &ty_str, NULL );

        struct dict_entry *en = dictAddOrFind( t, VDATA( "abc" ), NULL );
        dictSetU64( en, 123 );
        ASSERT_TRUE( "val",
                     dictGetU64( dictFind( t, VDATA( "abc" ) ) ) == 123 );
        size_t size = t->ht.size;

        dictExpand( t, 1024 );
        ASSERT_TRUE( "new size", t->ht.size > size );
        ASSERT_TRUE( "val after hash",
                     dictGetU64( dictFind( t, VDATA( "abc" ) ) ) == 123 );

        dictFree( t );
        return NULL;
}

// -----------------------------------------------------------------------------
// low level api: int64
// -----------------------------------------------------------------------------

static struct dict_type ty_i64 = {
    .hashFn    = valueHashFnI64,
    .keyCmpFn  = valueCmpI64,
    .keyDupFn  = NULL,
    .valDupFn  = NULL,
    .keyFreeFn = NULL,
    .valFreeFn = NULL,
};

static char *
test_add_and_find_existed_i64( void )
{
        int          existed;
        struct dict *t = dictNew( &ty_i64, NULL );

        struct dict_entry *en;
        {
                union value key = { .i64 = 123 };
                en              = dictAddOrFind( t, key, &existed );
                ASSERT_TRUE( "not existed", existed == 0 );
                dictSetU64( en, 456 );
        }

        {
                en = dictAddOrFind( t, VI64( 123 ), &existed );

                ASSERT_TRUE( "existed", existed == 1 );
                ASSERT_TRUE( "same val",
                             dictGetU64( dictFind( t, VI64( 123 ) ) ) == 456 );
        }
        dictFree( t );
        return NULL;
}

// -----------------------------------------------------------------------------
// low level api: sds key with str as value. check replace
// -----------------------------------------------------------------------------

static struct dict_type ty_sds_str = {
    .hashFn    = hashFnStr,
    .keyCmpFn  = keyCmpFnStr,
    .keyDupFn  = dupFnSds,
    .valDupFn  = dupFnStr,
    .keyFreeFn = freeFnSds,
    .valFreeFn = freeFnStr,
};

static char *
test_sds_key_str_val( void )
{
        struct dict *t = dictNew( &ty_sds_str, NULL );

        // pass str to sds dup is ok
        struct dict_entry *en = dictAddOrFind( t, VDATA( "abc" ), NULL );
        dictSetData( t, en, "123" );
        ASSERT_TRUE( "val",
                     0 == strcmp( dictGetData( dictFind( t, VDATA( "abc" ) ) ),
                                  "123" ) );
        ASSERT_TRUE( "val null", NULL == dictFind( t, VDATA( "bc" ) ) );

        // replace existing
        ASSERT_TRUE( "replcae",
                     0 == dictReplace( t, VDATA( "abc" ), VDATA( "456" ) ) );
        ASSERT_TRUE( "new val",
                     0 == strcmp( dictGetData( dictFind( t, VDATA( "abc" ) ) ),
                                  "456" ) );

        // replace new
        ASSERT_TRUE( "replcae",
                     1 == dictReplace( t, VDATA( "bc" ), VDATA( "789" ) ) );
        ASSERT_TRUE(
            "val replaced",
            0 == strcmp( dictGetData( dictFind( t, VDATA( "bc" ) ) ), "789" ) );

        dictFree( t );
        return NULL;
}

DECLARE_TEST_SUITE( adt_dict )
{
        // high level
        RUN_TEST( test_high_level_str_no_dup_free_fn );
        RUN_TEST( test_high_level_str_dup_free_fn );

        RUN_TEST( test_high_level_ptr_no_dup_free_fn );
        RUN_TEST( test_high_level_ptr_dup_free_fn );

        RUN_TEST( test_high_level_i64_no_dup_free_fn );
        RUN_TEST( test_high_level_i64_dup_free_fn );

        // low level
        RUN_TEST( test_new );
        RUN_TEST( test_init );
        RUN_TEST( test_add_and_find );
        RUN_TEST( test_delete );
        RUN_TEST( test_add_and_find_existed );
        RUN_TEST( test_expand );
        RUN_TEST( test_add_and_find_existed_i64 );
        RUN_TEST( test_sds_key_str_val );
        return NULL;
}
