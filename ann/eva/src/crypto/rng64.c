#include "crypto/rng64.h"

#include <assert.h>
#include <stdlib.h>

// -----------------------------------------------------------------------------
//
// ---------------------
// The DotMix algorithrm
// ---------------------
//
// 1. At each level in the pedigee, the coefficient `gamma` is used to advance
//    the seed.
//
// 2. The addition-modulo, for the Dot in DotMix, is based on
//
//        George = 2^64 + 13L.
//
//    This is implemented as `rng64_update`. In particular,
//
//   a. Any new seed, after modulo, falls in [0, 2^64) is accepted.
//   b. For [2^64, 2^64+13L), will try again. To avoid a loop, `gamma`, the
//      coefficient, is constructed to be larger than 13L,
//
// 3. The Mix in DotMix is implemented in `rng64_mix64`.
//
//
// 2. To avoid a fixed-size coefficient table.
// The coefficient to generate rng for coefficient.
//
//
// -----------------------------
// Generating random coefficient
// -----------------------------
//
// By using PRNG to generate coefficient for different level, a fixed table can
// be void avoided. The algorithrm is still based on DotMix, but with a
// length-1 pedigee.
//
// 1. The `gamma` coefficient for generating `gamma` is `gamma_gamma_`.
//
// 2. To improve the speed of `rng64_update`, a small Prime number (for
//    modulo) is used for generating `gamma`, called Percy. It is the
//    `gamma_prime_` in the code below.
//
//    With that, the `rng64_update` has high chance take the first branch.
//
// 3. This gamma seed is then mixed by `rng64_mix56` to be _truely` random.
//
// 4. The generated `gamma` should be no smaller than 13L. See DotMix
//    algorithrm.

// -----------------------------------------------------------------------------
// helper methods prototypes.
// -----------------------------------------------------------------------------

/* Cast of uint64_t is needed for 32-bit platform. */
static const uint64_t gamma_prime_ =
    ( ( (uint64_t)1L ) << 56 ) - 5; /* Percy. */
static const uint64_t gamma_gamma_ = 0x00281E2DBA6606F3L;
static const double   double_ulp_  = 1.0 / ( ( (uint64_t)1L ) << 53 );
static const float    float_ulp_   = 1.0 / ( ( (uint64_t)1L ) << 53 );

static uint64_t rng64_update( uint64_t seed, uint64_t gamma );
static uint64_t rng64_mix64( uint64_t z );
static uint64_t rng64_advance_seed( struct rng64 *rng );
static uint64_t rng64_mix56( uint64_t z );

// -----------------------------------------------------------------------------
// implementation.
// -----------------------------------------------------------------------------

struct rng64 *
rng64New( uint64_t seed )
{
        return rng64NewWithGamma( seed, /*gamma_seed=*/0L );
}

struct rng64 *
rng64NewWithGamma( uint64_t seed, uint64_t gamma_seed )
{
        struct rng64 *rng;

        assert( gamma_seed < gamma_prime_ );
        rng = malloc( sizeof( struct rng64 ) );

        rng->seed_ = seed;
        gamma_seed += gamma_gamma_;
        if ( gamma_seed >= gamma_prime_ ) gamma_seed -= gamma_prime_;
        rng->gamma_           = rng64_mix56( gamma_seed ) + 13;
        rng->next_gamma_seed_ = gamma_seed;
        return rng;
}

struct rng64 *
rng64Split( struct rng64 *rng )
{
        uint64_t seed       = rng64_advance_seed( (struct rng64 *)rng );
        uint64_t gamma_seed = rng->next_gamma_seed_;
        return rng64NewWithGamma( seed, gamma_seed );
}

void
rng64To( struct rng64 *rng, u64_t states[2] )
{
        states[0] = rng->seed_;
        states[1] = rng->next_gamma_seed_;
}

void
rng64From( struct rng64 *rng, u64_t states[2] )
{
        rng->seed_            = states[0];
        rng->gamma_           = rng64_mix56( states[1] ) + 13;
        rng->next_gamma_seed_ = states[1];
}

void
rng64Free( struct rng64 *rng )
{
        free( rng );
}

uint64_t
rng64U64( struct rng64 *rng )
{
        return rng64_mix64( rng64_advance_seed( rng ) );
}

uint32_t
rng64U32( struct rng64 *rng )
{
        return (uint32_t)( rng64U64( rng ) );
}

double
rng64F64( struct rng64 *rng )
{
        return ( rng64U64( rng ) >> 11 ) * double_ulp_;
}

float
rng64F32( struct rng64 *rng )
{
        return (float)( rng64U64( rng ) >> 11 ) * float_ulp_;
}

// -----------------------------------------------------------------------------
// helper methods implementation.
// -----------------------------------------------------------------------------

uint64_t
rng64_update( uint64_t seed, uint64_t gamma )
{
        uint64_t p = seed + gamma;
        return ( p >= seed ) ? p : ( p >= 13L ) ? p - 13L : ( p - 13L ) + gamma;
}

uint64_t
rng64_mix64( uint64_t z )
{
        z = ( ( z ^ ( z >> 33 ) ) * 0xff51afd7ed558ccdL );
        z = ( ( z ^ ( z >> 33 ) ) * 0xc4ceb9fe1a85ec53L );
        return z ^ ( z >> 33 );
}

uint64_t
rng64_advance_seed( struct rng64 *rng )
{
        /* Advance one more coefficient at current level. */
        return ( rng->seed_ = rng64_update( rng->seed_, rng->gamma_ ) );
}

uint64_t
rng64_mix56( uint64_t z )
{
        z = ( ( z ^ ( z >> 33 ) ) * 0xff51afd7ed558ccdL ) & 0x00FFFFFFFFFFFFFFL;
        z = ( ( z ^ ( z >> 33 ) ) * 0xc4ceb9fe1a85ec53L ) & 0x00FFFFFFFFFFFFFFL;
        return z ^ ( z >> 33 );
}
