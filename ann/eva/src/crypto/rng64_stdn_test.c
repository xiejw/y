#include <testing/testing.h>
#include <testing/testing_macros.h>

#include <crypto/rng64.h>

#include <stdlib.h>

#define SIZE 4

static char *
test_normal_f64( )
{
    struct rng64 *rng = rng64New( 456L );

    double expected[] = { 1.348183, -1.670144, 1.431032, 0.632074 };
    double got[SIZE];
    rng64StdNormF64( rng, SIZE, got );

    ASSERT_ARRAY_CLOSE( "result", expected, got, SIZE, 1e-5 );
    rng64Free( rng );

    return NULL;
}

static char *
test_normal_f32( )
{
    struct rng64 *rng = rng64New( 456L );

    float expected[] = { 1.348183f, -1.670144f, 1.431032f, 0.632074f };
    float got[SIZE];
    rng64StdNormF32( rng, SIZE, got );

    ASSERT_ARRAY_CLOSE( "result", expected, got, SIZE, 1e-5f );
    rng64Free( rng );

    return NULL;
}

DECLARE_TEST_SUITE( crypto_rng64_stdn )
{
    RUN_TEST( test_normal_f64 );
    RUN_TEST( test_normal_f32 );
    return NULL;
}
