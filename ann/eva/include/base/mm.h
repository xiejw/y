#ifndef EVA_MM_H
#define EVA_MM_H

// NOTE: this module should be moved to separated branch (rather than 'base')

// eva
#include <adt/vec.h>

// -----------------------------------------------------------------------------
//
// ------
// design
// ------
//
// 'struct arr' is a memory management data structure. It serves as as a
// allocator, or can be considered as array like or arrangement.
//
// --------
// use case
// --------
//
// - For fixed size contiguous memory (without huge amount of heap overheads),
//   consider use malloc/free directly.
//
// - For auto-growable contiguous memory, use vec_t. You need to pay the
//   amortized cost.
//
// - For auto-growable element-wise access but cache friendly, 'struct arr' is
//   good.  It manages auto-growable arrays (same size), so it is suitable for
//   stacks/queues but very cache friend. Internally, it maintains a bunch of
//   buffer lines and grows the number of lines if needed. The amortized cost
//   covers the line management not element level.
//
// - For file descriptor like memory management, i.e., reusing the slot with
// lowest index number, 'struct arr' is also well suited. The slot is provided
// as is. So, the caller can use it properly without initialization if the
// workload fits (think slab allocator). Internally, a bitmap is used to track
// the slot indices.
//
// --------------
// future changes
// --------------
// - allow auto shrink when user opts in.
// - allocate pages not 2 powers internally.
//
// -----------------------------------------------------------------------------
struct arr {
    // internal fields

    size_t slot_byte_size;    // byte size per slot
    size_t slots_per_line;    // num slots per memory line
    vec_t( void * ) lines;    // holds lines' address. auto grow.
    size_t       cap;         // current cap (slot count)
    size_t       mask;        // mask to find line level index.
    unsigned int sbits : 10;  // shift_bits
    unsigned int mode : 1;    // either stack 0 or bitmap
    unsigned int flag : 7;    // for future use
};

//-----------------------------------------------------------------------------
// common low level apis
//-----------------------------------------------------------------------------
extern void  arrFree( struct arr  *);
extern void *arrGet( struct arr *, size_t index );  // NULL if index is invalid

//-----------------------------------------------------------------------------
// stack mode
//-----------------------------------------------------------------------------

// @param slot_byte_size the memory size for each slot. Typically, should be
//        the result of `sizeof`
// @param suggested_slots_per_line the suggested slot count per line. Note this
//        is not guaranteed. A bigger number (2 powers) might be chosen for
//        performance reason.
// @param init_num_slots initial suggested capacity. Again, a bigger number
//        might be chosen.
extern struct arr *arrStackNew( size_t slot_byte_size,
                                size_t suggested_slots_per_line,
                                size_t init_num_slots );

//-------------
// stack apis
extern int  arrIsEmpty( struct arr  *);
extern void arrShrink( struct arr * );  // unimpled yet.

// Push returns the address for the new slot.
// Pop returns NULL if empty or the address of the popped item.
extern void *arrPush( struct arr * );
extern void *arrPop( struct arr * );  // NULL if empty

//-------------
// stack macros
#define ARR_STACK_NEW( type ) arrStackNew( sizeof( type ), 32, 128 )

#define ARR_PUSH( arr, v )                     \
    do {                                       \
        __typeof__( v ) *ptr = arrPush( arr ); \
        *ptr                 = ( v );          \
    } while ( 0 )

// cannot be empty
#define ARR_POP( v, arr )                     \
    do {                                      \
        __typeof__( v ) *ptr = arrPop( arr ); \
        assert( ptr != NULL );                \
        ( v ) = ( *ptr );                     \
    } while ( 0 )

//-----------------------------------------------------------------------------
// bitmap mode (unimpled yet)
//-----------------------------------------------------------------------------
extern struct arr *arrNewBitmap( size_t slot_byte_size,
                                 size_t suggested_slots_per_line,
                                 size_t forced_max_slots );

#endif  // EVA_MM_H
