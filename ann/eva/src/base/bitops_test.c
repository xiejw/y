#include "testing/testing.h"

#include "base/bitops.h"

#include <limits.h>  // ULONG_MAX

#define BITS_PER_BYTE 8
#define BITS_PER_LONG ( sizeof( unsigned long ) * BITS_PER_BYTE )

static char *
test_empty( )
{
    unsigned long *p = NULL;
    long           r = bitOpsNextZero( p, 0 );
    ASSERT_TRUE( "empty", r == -1 );
    return NULL;
}

static char *
test_no_zeros( )
{
    unsigned long p = ULONG_MAX;
    long          r = bitOpsNextZero( &p, 1 );
    ASSERT_TRUE( "no zeros", r == -1 );
    return NULL;
}

static char *
test_long( )
{
    for ( size_t i = 0; i < sizeof( unsigned long ) * 8; i++ ) {
        unsigned long p = 0;
        p |= 1L << i;
        p ^= ULONG_MAX;

        long r = bitOpsNextZero( &p, 1 );
        // DEBUG: printf("i %zu r %ld\n", i, r);
        ASSERT_TRUE( "bit position", r == (long)i );
    }
    return NULL;
}

static char *
test_longs( )
{
    unsigned long longs[3];
    longs[0] = ULONG_MAX;
    longs[2] = ULONG_MAX;

    for ( size_t i = 0; i < sizeof( unsigned long ) * 8; i++ ) {
        unsigned long p = 0;
        p |= 1L << i;
        p ^= ULONG_MAX;

        longs[1] = p;
        long r   = bitOpsNextZero( longs, 3 );
        ASSERT_TRUE( "bit position", r == (long)( i + BITS_PER_LONG ) );
    }
    return NULL;
}

static char *
test_get_bits( )
{
    unsigned long bit_for_zero = 11;
    unsigned long longs[3];
    longs[0] = ULONG_MAX;
    longs[2] = ULONG_MAX;

    unsigned long p = 0;
    p |= 1L << bit_for_zero;
    p |= 1;
    p ^= ULONG_MAX;

    longs[1] = p;
    for ( unsigned long i = 0; i < 3 * BITS_PER_LONG; i++ ) {
        long bit = bitOpsGetBit( longs, i );

        if ( i == BITS_PER_LONG + bit_for_zero )
            ASSERT_TRUE( "bit value", bit == 0 );
        else if ( i == BITS_PER_LONG )
            ASSERT_TRUE( "bit value", bit == 0 );
        else
            ASSERT_TRUE( "bit value", bit == 1 );
    }
    return NULL;
}

static char *
test_set_bits( )
{
    unsigned long longs[2];
    unsigned long p = 0;
    p |= 1;

    longs[0] = p;
    longs[1] = p;

    long b1 = bitOpsSetBit( longs, 0 );                  // the first long
    long b2 = bitOpsSetBit( longs, 1 );                  // the first long
    long b3 = bitOpsSetBit( longs, BITS_PER_LONG + 0 );  // the second long
    long b4 = bitOpsSetBit( longs, BITS_PER_LONG + 1 );  // the second long

    ASSERT_TRUE( "1st long", longs[0] == 3L );
    ASSERT_TRUE( "2nd long", longs[1] == 3L );

    ASSERT_TRUE( "1st bit", b1 == 1L );
    ASSERT_TRUE( "2nd bit", b2 == 0L );
    ASSERT_TRUE( "3th bit", b3 == 1L );
    ASSERT_TRUE( "4th bit", b4 == 0L );

    return NULL;
}

static char *
test_clr_bits( )
{
    unsigned long longs[2];
    unsigned long p = 0;
    p |= 5;

    longs[0] = p;
    longs[1] = p;

    long b1 = bitOpsClrBit( longs, 0 );                  // the first long
    long b2 = bitOpsClrBit( longs, 1 );                  // the first long
    long b3 = bitOpsClrBit( longs, BITS_PER_LONG + 0 );  // the second long
    long b4 = bitOpsClrBit( longs, BITS_PER_LONG + 1 );  // the second long

    ASSERT_TRUE( "1st long", longs[0] == 4L );
    ASSERT_TRUE( "2nd long", longs[1] == 4L );

    ASSERT_TRUE( "1st bit", b1 == 1L );
    ASSERT_TRUE( "2nd bit", b2 == 0L );
    ASSERT_TRUE( "3th bit", b3 == 1L );
    ASSERT_TRUE( "4th bit", b4 == 0L );

    return NULL;
}

static char *
test_next_2_power( )
{
    long          bits = 7L;  // any random value
    unsigned long r;

    r = bitOpsNext2Power( 1L, &bits );
    ASSERT_TRUE( "r", r == 1L );
    ASSERT_TRUE( "s", bits == 0L );

    r = bitOpsNext2Power( 2L, &bits );
    ASSERT_TRUE( "r", r == 2L );
    ASSERT_TRUE( "s", bits == 1L );

    r = bitOpsNext2Power( 3L, &bits );
    ASSERT_TRUE( "r", r == 4L );
    ASSERT_TRUE( "s", bits == 2L );

    r = bitOpsNext2Power( 7L, &bits );
    ASSERT_TRUE( "r", r == 8L );
    ASSERT_TRUE( "s", bits == 3L );

    r = bitOpsNext2Power( 8L, &bits );
    ASSERT_TRUE( "r", r == 8L );
    ASSERT_TRUE( "s", bits == 3L );

    return NULL;
}

static char *
test_next_2_power_overflow( )
{
    const unsigned long max_ulong_supported = 1UL << ( BITS_PER_LONG - 1 );
    unsigned long       r;

    long bits = 7L;  // any random value

    r = bitOpsNext2Power( max_ulong_supported, &bits );
    ASSERT_TRUE( "r", r == max_ulong_supported );
    ASSERT_TRUE( "s", bits == ( BITS_PER_LONG - 1 ) );

    bits = 7L;
    r    = bitOpsNext2Power( max_ulong_supported + 1UL, &bits );
    ASSERT_TRUE( "r", r == 0 );
    ASSERT_TRUE( "s", bits == -1 );

    bits = 7L;
    r    = bitOpsNext2Power( max_ulong_supported + 123UL, &bits );
    ASSERT_TRUE( "r", r == 0 );
    ASSERT_TRUE( "s", bits == -1 );

    return NULL;
}

DECLARE_TEST_SUITE( base_bitops )
{
    RUN_TEST( test_empty );
    RUN_TEST( test_no_zeros );
    RUN_TEST( test_long );
    RUN_TEST( test_longs );
    RUN_TEST( test_get_bits );
    RUN_TEST( test_set_bits );
    RUN_TEST( test_clr_bits );
    RUN_TEST( test_next_2_power );
    RUN_TEST( test_next_2_power_overflow );
    return NULL;
}
