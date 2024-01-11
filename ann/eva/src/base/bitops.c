#include "base/bitops.h"

#include <assert.h>
#include <limits.h>  // ULONG_MAX
#include <stddef.h>  // NULL

// eva
#include <base/error.h>

#define BITS_PER_BYTE 8
#define BITS_PER_LONG (sizeof(unsigned long) * BITS_PER_BYTE)

// The first zero bit for 0000..1111. The table is good for speed performance.
static long table[15] = {
    0,  // 0000
    1,  // 0001
    0,  // 0010
    2,  // 0011
    0,  // 0100
    1,  // 0101
    0,  // 0110
    3,  // 0111
    0,  // 1000
    1,  // 1001
    0,  // 1010
    2,  // 1011
    0,  // 1100
    1,  // 1101
    0,  // 1110
};

// -----------------------------------------------------------------------------
// impl for public apis
// -----------------------------------------------------------------------------
long
bitOpsNextZero(const unsigned long *p, unsigned long num_longs)
{
        if (num_longs == 0) return -1;
        assert(p != NULL);

        unsigned long count = 0;
        long pos            = 0;
        unsigned long v;

        while (count < num_longs) {
                if (*p == ULONG_MAX) {  // no zeros
                        p++;
                        count++;
                        pos += BITS_PER_LONG;
                        continue;
                }

                goto check_long;
        }

        // failed to next zero bit
        return -1;

check_long:
        // now at p, there must be a zero bit in int.

        // the first pass, we find the byte
        v = *p;
        for (size_t i = 0; i < sizeof(unsigned long); i++) {
                unsigned long b = v & 0xFF;

                if (b != 0xFF) {
                        goto check_byte;
                }
                pos += BITS_PER_BYTE;
                v = v >> BITS_PER_BYTE;
        }
        errPanic("should not reach");

check_byte: {
        // check the byte in two halfs
        unsigned long b  = v & 0xFF;
        unsigned long b1 = b & 0xF;
        if (b1 != 0xF) {
                return pos + table[b1];
        }

        unsigned long b2 = b >> 4;
        assert(b2 != 0xF);
        return pos + 4 + table[b2];
}
}

long
bitOpsGetBit(const unsigned long *p, unsigned long offset_bits)
{
        // dont worry about the divide and module ops here. As BITS_PER_LONG is
        // constant, compiler has no trouble to translate them to shift ops.
        unsigned long num_longs = offset_bits / BITS_PER_LONG;
        p += num_longs;
        return (*p >> (offset_bits % BITS_PER_LONG)) & 0x1;
}

long
bitOpsSetBit(unsigned long *p, unsigned long offset_bits)
{
        unsigned long num_longs = offset_bits / BITS_PER_LONG;
        p += num_longs;                               // adjust p
        offset_bits = (offset_bits % BITS_PER_LONG);  // adjust offset

        long old_v = (*p >> offset_bits) & 0x1;
        *p |= 1L << offset_bits;
        return old_v;
}

long
bitOpsClrBit(unsigned long *p, unsigned long offset_bits)
{
        unsigned long num_longs = offset_bits / BITS_PER_LONG;
        p += num_longs;                               // adjust p
        offset_bits = (offset_bits % BITS_PER_LONG);  // adjust offset

        long old_v = (*p >> offset_bits) & 0x1;
        *p &= ~(1L << offset_bits);
        return old_v;
}

unsigned long
bitOpsNext2Power(const unsigned long v, _out_ long *num_bits)
{
        assert(num_bits != NULL);

        unsigned long r = 1;
        long count      = 0;

        while (r < v) {
                count++;
                r = r << 1;

                if (r == 0) {
                        // overflow
                        *num_bits = -1;
                        return 0;
                }
        }

        *num_bits = count;
        return r;
}
