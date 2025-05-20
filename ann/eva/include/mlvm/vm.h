#ifndef EVA_MLVM_VM_H_
#define EVA_MLVM_VM_H_

// eva
#include <adt/sds.h>
#include <base/error.h>
#include <base/types.h>
#include <crypto/rng64.h>

// -----------------------------------------------------------------------------
// data structures
// -----------------------------------------------------------------------------

// date type for tensors.
enum vm_dtype {
        VM_F32,  // f32_t
        VM_U64,  // u64_t
};

// shape for tensors, immutable.
//
// sharable via ref counts.
struct vm_sp {
        int      rank;       // rank of the shape, i.e., len of dims
        uint64_t size;       // total number of elements
        int      ref_count;  // not thread safe
        int      dims[];     // size is rank
};

// the tensor structure
struct vm_tensor {
        enum vm_dtype dtype : 7;
        unsigned int  used : 1;
        struct vm_sp *shape;
        void         *data;
};

struct vm;  // forward def

struct vm_opopt {
        int mode;  // see details in op.h
        union {
                i32_t        i;
                f32_t        f;
                struct rng64 r;
        };
};

#define VM_OP( op ) VM_OP_##op,
enum vm_opcode {
#include "mlvm/op.h"
};
#undef VM_OP

// -----------------------------------------------------------------------------
// apis for vm  / vm.c
// -----------------------------------------------------------------------------

struct vm *vmNew( void );
void       vmFree( struct vm * );

error_t vmExec( struct vm *, enum vm_opcode, const struct vm_opopt *, int d,
                int l, int r );

// -----------------------------------------------------------------------------
// apis for batch execution  / vm.c
// -----------------------------------------------------------------------------
struct vm_oparg {
        enum vm_opcode  op;
        int             dst;
        int             t1;
        int             t2;
        int             has_opt;
        struct vm_opopt opt;
};

extern void    vmOpsToSds( _mut_ sds_t *psds, size_t size,
                           const struct vm_oparg * );
extern error_t vmBatch( struct vm *vm, size_t arg_size,
                        const struct vm_oparg * );

// -----------------------------------------------------------------------------
// apis for tensors  / tensor.c
// -----------------------------------------------------------------------------

// vmTensorNew returns tensor handle (non-neg int) upon succeeded.
//
// On failure, returns neg number. call site should clean error stack.
int     vmTensorNew( struct vm *, enum vm_dtype, struct vm_sp * );
error_t vmTensorFree( struct vm *, int t );

error_t vmTensorInfo( struct vm *, int t, _null_ _out_ enum vm_dtype *,
                      _null_ _out_ struct vm_sp ** );
error_t vmTensorData( struct vm *, int t, _out_ void **data );
error_t vmTensorSwap( struct vm *, int t, _out_ void **data );
void    vmTensorToSds( _mut_ sds_t *s, struct vm *, int t );

// low level primitives
void tensorLikeToSds( _mut_ sds_t *s, struct vm_sp *, enum vm_dtype,
                      void        *nullable_data );

// -----------------------------------------------------------------------------
// apis for shapes  / shape.c
// -----------------------------------------------------------------------------

struct vm_sp *spNew( int rank, int *dims );
struct vm_sp *spIncRef( struct vm_sp * );
struct vm_sp *spDecRef( struct vm_sp * );

struct vm_sp *vmSpNew( struct vm *vm, int rank, int *dims );  // vm.c

// macors for shapes
#define VM_VR1S( vm, s1 )     vmSpNew( vm, 1, (int[]){ ( s1 ) } )
#define VM_VR2S( vm, s1, s2 ) vmSpNew( vm, 2, (int[]){ ( s1 ), ( s2 ) } )
#define VM_VR3S( vm, s1, s2, s3 ) \
        vmSpNew( vm, 3, (int[]){ ( s1 ), ( s2 ), ( s3 ) } )

// -----------------------------------------------------------------------------
// opt mask bits.
// -----------------------------------------------------------------------------
#define VM_OPT_MODE_BIT_MASK         0xFF0000
#define VM_OPT_MODE_UNMASK           0x00FFFF
#define VM_OPT_MODE_I_BIT            0x10000
#define VM_OPT_MODE_F_BIT            0x20000
#define VM_OPT_MODE_R_BIT            0x40000
#define VM_OPT_MODE_GET_I_BIT( opt ) ( ( ( opt ).mode ) & VM_OPT_MODE_I_BIT )
#define VM_OPT_MODE_GET_F_BIT( opt ) ( ( ( opt ).mode ) & VM_OPT_MODE_F_BIT )
#define VM_OPT_MODE_GET_R_BIT( opt ) ( ( ( opt ).mode ) & VM_OPT_MODE_R_BIT )

// -----------------------------------------------------------------------------
// opt related macros
// -----------------------------------------------------------------------------
//
// --- Element wise ops
#define VM_OPT_SET_SCALAR_OPERAND( opt, v ) \
        ( ( opt ).mode = VM_OPT_MODE_F_BIT, ( opt ).f = ( v ) )

// --- Matmul
#define VM_OPT_MATMUL_TRANS_NOT 0
#define VM_OPT_MATMUL_TRANS_LHS 2
#define VM_OPT_MATMUL_TRANS_RHS 1

// --- Reduction
#define VM_OPT_SET_REDUCTION_SUM( opt, axis ) \
        ( ( opt ).mode = 0 | VM_OPT_MODE_I_BIT, ( opt ).i = ( axis ) )

// --- Rng
#define VM_OPT_RNG_STD_NORMAL 0

// --- Loss
#define VM_OPT_SET_GRAD_TENSOR_HANDLER( opt, td ) \
        ( ( opt ).mode |= VM_OPT_MODE_I_BIT, ( opt ).i = ( td ) )
#endif  // EVA_MLVM_VM_H_
