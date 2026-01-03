#ifndef EVA_MLVM_KERNEL_H_
#define EVA_MLVM_KERNEL_H_

#include <mlvm/vm.h>

// -----------------------------------------------------------------------------
// element wise
// -----------------------------------------------------------------------------
error_t vmOpAddF32( struct vm_tensor *dst, struct vm_tensor *,
                    struct vm_tensor * );
error_t vmOpMulF32( struct vm_tensor *dst, struct vm_tensor *,
                    struct vm_tensor * );
error_t vmOpMinusF32( struct vm_tensor *dst, struct vm_tensor *,
                      struct vm_tensor * );
error_t vmOpDivideF32( struct vm_tensor *dst, struct vm_tensor *,
                       struct vm_tensor * );
error_t vmOpMaxF32( struct vm_tensor *dst, struct vm_tensor *,
                    struct vm_tensor * );
error_t vmOpEqF32( struct vm_tensor *dst, struct vm_tensor *,
                   struct vm_tensor * );
error_t vmOpCmpLF32( struct vm_tensor *dst, struct vm_tensor *,
                     struct vm_tensor * );

error_t vmOpAddSF32( struct vm_tensor *dst, struct vm_tensor *t1, f32_t s );
error_t vmOpMulSF32( struct vm_tensor *dst, struct vm_tensor *t1, f32_t s );
error_t vmOpMinusSF32( struct vm_tensor *dst, struct vm_tensor *t1, f32_t );
error_t vmOpDivideSF32( struct vm_tensor *dst, struct vm_tensor *t1, f32_t );
error_t vmOpMaxSF32( struct vm_tensor *dst, struct vm_tensor *t1, f32_t );
error_t vmOpEqSF32( struct vm_tensor *dst, struct vm_tensor *t1, f32_t );
error_t vmOpCmpLSF32( struct vm_tensor *dst, struct vm_tensor *t1, f32_t );

// -----------------------------------------------------------------------------
// reduction
// -----------------------------------------------------------------------------
error_t vmOpReduceF32( struct vm_tensor *dst, struct vm_tensor *t1, int mode,
                       int axis );

// -----------------------------------------------------------------------------
// inverse Sqrt
// -----------------------------------------------------------------------------
error_t vmOpISqrtF32( struct vm_tensor *dst, struct vm_tensor *t1,
                      const f32_t *e, int mode );

// -----------------------------------------------------------------------------
// arg max
// -----------------------------------------------------------------------------
error_t vmOpArgMaxF32( struct vm_tensor *dst, struct vm_tensor *t1 );

// -----------------------------------------------------------------------------
// rng
// -----------------------------------------------------------------------------
error_t vmOpRngF32( struct vm_tensor *dst, int mode, struct rng64 *rng );

// -----------------------------------------------------------------------------
// matmul
// -----------------------------------------------------------------------------
error_t vmOpMatmulF32( struct vm_tensor *dst, struct vm_tensor *,
                       struct vm_tensor *, int trans_lhs, int trans_rhs );

// -----------------------------------------------------------------------------
// fill
// -----------------------------------------------------------------------------
error_t vmOpFillF32( struct vm_tensor *dst, f32_t );

// -----------------------------------------------------------------------------
// loss
// -----------------------------------------------------------------------------
error_t vmOpLossSCELF32( struct vm_tensor *dst, struct vm_tensor *y,
                         struct vm_tensor *o, struct vm_tensor *optional_g );

#endif  // EVA_MLVM_KERNEL_H_
