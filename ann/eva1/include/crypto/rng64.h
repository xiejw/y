#ifndef EVA_RNG64_H_
#define EVA_RNG64_H_

#include <stddef.h>  // size_t
#include <stdint.h>  // uxxx_t

// eva
#include <base/types.h>

// -----------------------------------------------------------------------------
// splittable rng generator (rng64.c)
// -----------------------------------------------------------------------------

struct rng64 {
        // interal fields.
        uint64_t seed_;   // must be the first field.
        uint64_t gamma_;  // must be the second field.
        uint64_t next_gamma_seed_;
};

extern struct rng64 *rng64New( uint64_t seed );
extern struct rng64 *rng64NewWithGamma( uint64_t seed, uint64_t gamma_seed );
extern void          rng64Free( struct rng64 *rng );

extern struct rng64 *rng64Split( struct rng64 *rng );

// Dumps the internal states of rng to states (To) and restores it back (From).
extern void rng64To( struct rng64 *rng, u64_t states[2] );
extern void rng64From( struct rng64 *rng, u64_t states[2] );

extern u64_t rng64U64( struct rng64 *rng );
extern u32_t rng64U32( struct rng64 *rng );
extern f64_t rng64F64( struct rng64 *rng );
extern f32_t rng64F32( struct rng64 *rng );

// -----------------------------------------------------------------------------
// distribution related rngs (rng64_stdn.c)
// -----------------------------------------------------------------------------
extern void rng64StdNormF64( struct rng64 *rng, size_t size, double *buffer );
extern void rng64StdNormF32( struct rng64 *rng, size_t size, float *buffer );

#endif  // EVA_RNG64_H_
