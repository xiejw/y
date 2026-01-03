#include "testing/testing.h"

#include "adt/value.h"

static char *
test_value( void )
{
        union value v = { .i64 = 123 };
        ASSERT_TRUE( ".i64", v.i64 == 123 );
        return NULL;
}

static char *
test_compound_literal_u64( void )
{
        ASSERT_TRUE( ".u64", VU64( 123 ).u64 == 123 );
        return NULL;
}

static char *
test_compound_literal_data( void )
{
        int x = 3;
        ASSERT_TRUE( ".data", VDATA( &x ).data == &x );
        return NULL;
}

DECLARE_TEST_SUITE( adt_value )
{
        RUN_TEST( test_value );
        RUN_TEST( test_compound_literal_u64 );
        RUN_TEST( test_compound_literal_data );
        return NULL;
}
