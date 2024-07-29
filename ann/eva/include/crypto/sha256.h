#ifndef EVA_SHA256_H_
#define EVA_SHA256_H_

#include <stdint.h>  // uxxx_t

// eva
#include "adt/sds.h"

// -----------------------------------------------------------------------------
// public data structure and apis
// -----------------------------------------------------------------------------

#define SHA224_256_BLOCK_SIZE ( 512 / 8 )  // 64 bytes

struct sha256 {
    // all internal fields.

    unsigned char block[SHA224_256_BLOCK_SIZE];  // unprocessed bytes.
    uint64_t      len;        // unprocessed bytes stored in block.
    uint64_t      total_len;  // total of bytes processed in history.
    uint32_t      h[8];       // hash values.
    int           finalized;  // once marked, must be reset for next usage.
};

extern void    sha256Reset( struct sha256    *);
extern error_t sha256Update( struct sha256 *, const unsigned char *, uint64_t );
extern sds_t   sha256Digest( struct sha256   *);

// -----------------------------------------------------------------------------
// helper methods to generate digest for string.
// -----------------------------------------------------------------------------
extern sds_t   sha256DigestStr( const char   *);
extern sds_t   sha256DigestSds( sds_t );
extern error_t sha256DigestFile( const char *, _out_ sds_t * );

#endif  // EVA_SHA256_H_
