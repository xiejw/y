#ifndef EVA_VALUE_H_
#define EVA_VALUE_H_

// eva
#include <base/types.h>

// =============================================================================
//
// value
//
// Represent a value in the core system used by many data strucutures, e.g.,
// dict and list.
// =============================================================================

union value {
        i64_t i64;
        u64_t u64;
        f64_t f64;
        f32_t f32;
        void *data;
};

// compound literal (in-place)
#define VDATA( x ) ( (union value){ .data = (void *)( x ) } )
#define VI64( x )  ( (union value){ .i64 = ( x ) } )
#define VU64( x )  ( (union value){ .u64 = ( x ) } )
#define VF64( x )  ( (union value){ .f64 = ( x ) } )
#define VF32( x )  ( (union value){ .f32 = ( x ) } )

// =============================================================================
// conform struct dict.
// =============================================================================

// -----------------------------------------------------------------------------
// ints and floats
extern int valueCmpI64( void *privdata, const union value, const union value );
extern int valueCmpU64( void *privdata, const union value, const union value );
extern int valueCmpF32( void *privdata, const union value, const union value );
extern int valueCmpF64( void *privdata, const union value, const union value );

extern u64_t valueHashFnI64( const union value key );
extern u64_t valueHashFnU64( const union value key );
extern u64_t valueHashFnF32( const union value key );
extern u64_t valueHashFnF64( const union value key );

// -----------------------------------------------------------------------------
// strs and sds
extern int   keyCmpFnStr( void *privdata, const union value key1,
                          const union value key2 );
extern u64_t hashFnStr( const union value key );

extern void *dupFnStr( void *privdata, const void *p );
extern void  freeFnStr( void *privdata, void *p );

extern void *dupFnSds( void *privdata, const void *p );
extern void  freeFnSds( void *privdata, void *p );

// -----------------------------------------------------------------------------
// raw ptrs
extern int   keyCmpFnPtr( void *privdata, const union value key1,
                          const union value key2 );
extern u64_t hashFnPtr( const union value key );

#endif  // EVA_VALUE_H_
