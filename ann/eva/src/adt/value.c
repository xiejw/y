#include "adt/value.h"

#include <float.h>  // FLT_EPSILON
#include <stdlib.h>
#include <string.h>

// eva
#include "adt/sds.h"

// -----------------------------------------------------------------------------
// ints and floats
// -----------------------------------------------------------------------------

int
valueCmpI64( void *privdata, const union value key1, const union value key2 )
{
    return key1.i64 == key2.i64;
}

int
valueCmpF32( void *privdata, const union value key1, const union value key2 )
{
    float d = key1.f32 - key2.f32;
    return d >= -FLT_EPSILON && d <= FLT_EPSILON;
}

u64_t
valueHashFnI64( const union value key )
{
    return (u64_t)key.i64;
}

u64_t
valueHashFnU64( const union value key )
{
    return key.u64;
}

// -----------------------------------------------------------------------------
// str and sds
// -----------------------------------------------------------------------------

u64_t
hashFnStr( const union value key )
{
    const unsigned char *str  = (const unsigned char *)key.data;
    u64_t                hash = 5381;
    while ( *str ) hash = ( ( hash << 5 ) + hash ) ^ *str++;
    return hash;
}

int
keyCmpFnStr( void *privdata, const union value key1, const union value key2 )
{
    return 0 == strcmp( (char *)key1.data, (char *)key2.data );
}

void *
dupFnStr( void *privdata, const void *old_str )
{
    char *new_str =
        malloc( sizeof( char ) * ( strlen( (const char *)old_str ) + 1 ) );
    strcpy( new_str, (const char *)old_str );
    return new_str;
}

void
freeFnStr( void *privdata, void *p )
{
    free( p );
}

void *
dupFnSds( void *privdata, const void *old_str )
{
    return sdsNew( old_str );
}

void
freeFnSds( void *privdata, void *p )
{
    sdsFree( p );
}

// -----------------------------------------------------------------------------
// ptr
// -----------------------------------------------------------------------------

u64_t
hashFnPtr( const union value key )
{
    return (u64_t)key.data;
}

int
keyCmpFnPtr( void *privdata, const union value key1, const union value key2 )
{
    return key1.data == key2.data;
}
