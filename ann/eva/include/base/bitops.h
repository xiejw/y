#ifndef EVA_BITOPS_H_
#define EVA_BITOPS_H_

// eva
#include <base/types.h>

// return the bit index for the next zero.
//
// note:
//     The memory addressed by p must be valid until num_longs later. This api
//     will return the bit index for the first zero found.
//
//     This api is not platform depenent. Least significant byte is always
//     searched first.
//
// error:
//     -1 for not found
long bitOpsNextZero( const unsigned long *p, unsigned long num_longs );

// return the bit value (0/1) for current value (before mutation)
//
// note:
//     The memory addressed by p with offset_bits shifted should be valid.
//     All apis will not check for this validness.
long bitOpsSetBit( unsigned long *p, unsigned long offset_bits );
long bitOpsClrBit( unsigned long *p, unsigned long offset_bits );
long bitOpsGetBit( const unsigned long *p, unsigned long offset_bits );

// return the next 2 power value not smaller than v and the num_bits shift to
// get it.
//
// note:
//     For v == 7 or 8, both return 8 and set num_bits to 3.  With that, shift
//     opeeration is 3 and mask is (1 << 3 - 1)
//
// error:
//     If unsigned long is 64 bits and v is > 1<<63, this api will set
//     num_bits as -1 and return 0.
unsigned long bitOpsNext2Power( const unsigned long v, _out_ long *num_bits );

#endif  // EVA_BITOPS_H_
