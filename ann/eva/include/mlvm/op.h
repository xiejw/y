// -----------------------------------------------------------------------------
// Op code with specification.
//
// NOTES: Not designed to be included in general. Include 'vm.h' instead.
// -----------------------------------------------------------------------------

// ====================================================================
// General Design Guidelines.
// ====================================================================
// Option.
//   - Option can be optional. If so) the NULL is provided and it
//     means the default setting.
//
//   - If Option.mode is used only) then macros is recommended) e.g.)
//     VM_OPT_MODE_I_BIT) etc. So the caller can have some readability.
//
//   - If Option union {i)f)r} is used) to avoid the sitution the
//     caller forgets to update them) option.mode be must set in an
//     explicit way so the check can be performed.

// ====================================================================
// Ops Specification
// ====================================================================

// --------------------------------------------------------------------
// Element ops
// --------------------------------------------------------------------
//
// Operands:
//   - both) if opt is NULL.
//   - t1 only if opt is not NULL
//
// Data Types:
//   - only VM_F32.
//
// Shapes:
//   - shapes are equal match.
//   - lhs shape has higher or equal rank and is multiple of rhs shape
//     (broadcasting)) i.e.)
//
//            allowed [4) 4) 4] with    [1) 4) 4]
//            allowed [4) 4) 4] with       [1) 4]
//        not allowed [4) 4) 4] with [1) 1) 1) 4]
//        not allowed [4) 4) 4] with    [4) 1) 4]
//
//   - rhs is scalar) i.e.) [1].
//   - rhs is NULL; uses .f for rhs scalar [F32].
//
// Option:
//   - opt could be NULL
//   - if opt is not NULL) F_BIT must be set and .f specicies the
//     scalar operand (the second operand).
//
// In-Place:
//   - all operand handles can be used as destination.
VM_OP(ADD)
VM_OP(MUL)
VM_OP(MINUS)
VM_OP(DIVIDE)
VM_OP(MAX)
VM_OP(EQ)
VM_OP(CMPL)  // Compare Larger.

// --------------------------------------------------------------------
// Matmul
// --------------------------------------------------------------------
//
// Operands:
//   - both.
//
// Data Types:
//   - only VM_F32.
//
// Shapes:
//   - only rank 2 operands.
//
// Option:
//   - opt could be NULL. It means opt.mode == 0) i.e.)
//     VM_OPT_MATMUL_TRANS_NOT
//
//   - mode == 2 means trans_lhs) i.e.) VM_OPT_MATMUL_TRANS_LHS
//   - mode == 1 means trans_rhs) i.e.) VM_OPT_MATMUL_TRANS_RHS
//   - other values of modes are invalid.
//
// In-Place:
//   - dst must be unique.
VM_OP(MATMUL)

// --------------------------------------------------------------------
// Arg
// --------------------------------------------------------------------
//
// Operands:
//   - t1 only.
//
// Data Types:
//   - only VM_F32.
//
// Shapes:
//   - only rank 2.
//
// Option:
//   - NULL
//
// In-Place:
//   - No. dst is rank 1.
VM_OP(ARGMAX)

// --------------------------------------------------------------------
// Inverse Sqrt
// --------------------------------------------------------------------
//
// Operands:
//   - t1 only.
//
// Data Types:
//   - only VM_F32.
//
// Shapes:
//   - any.
//
// Option (optional):
//   - opt could be NULL. Defaults to no epsilon.
//
//   - If not NULL) then .f specicies the epsilon that
//
//       1 / sqrt(t1+epsilon)    if opt.mode = 0
//       1 / (sqrt(t1)+epsilon)  if opt.mode = 1
//
// In-Place:
//   - dst is allowed to be t1.
VM_OP(ISQRT)

// --------------------------------------------------------------------
// Reduction
// --------------------------------------------------------------------
//
// Operands:
//   - t1 only.
//
// Data Types:
//   - only VM_F32.
//
// Shapes:
//   - any.
//
// Option (required):
//   - opt.mode value table
//
//     | v | reduction op | macro                 |
//     | 0 | sum          | VM_OPT_SET_REDUCTION_SUM |
//
//   - opt.mode I bit
//     - if set) then opt.i specifies the axis. Use
//       VM_OPT_SET_REDUCTION_SUM.
//     - otherwise) opt.i must be 0
//
// In-Place:
//   - not allowed.
VM_OP(REDUCE)

// --------------------------------------------------------------------
// Rng.
// --------------------------------------------------------------------
//
// Operands:
//   - no operands.
//
// Data Types:
//   - only VM_F32.
//
// Shapes:
//   - any.
//
// Option (required):
//   - opt.mode value table
//
//     | v | distribution | macro              |
//     | 0 | std normal   | VM_OPT_RNG_STD_NORMAL |
//
//   - opt.mode R bit
//     must set .r to provide rng (seed).
//
// In-Place:
//   - In-place anyway.
VM_OP(RNG)

// --------------------------------------------------------------------
// Fill.
// --------------------------------------------------------------------
//
// Operands:
//   - no operands.
//
// Data Types:
//   - only VM_F32.
//
// Shapes:
//   - any.
//
// Option (optional):
//   - NULL if fill with zero (optimized).
//   - opt.f (F bit) to value to fill
//
// In-Place:
//   - In-place anyway.
VM_OP(FILL)

// --------------------------------------------------------------------
// Softmax crossentropy with logits loss
// --------------------------------------------------------------------
//
// Operands:
//   - t1 is the label (must be valid distribution)
//   - t2 is the logits (recommmended to be unnormalized).
//
// Data Types:
//   - only VM_F32.
//
// Shapes:
//   - Rank 2 only for both.
//
// Option:
//   - opt could be NULL.
//   - if not NULL) opt.mode I bit must be set. Then opt.i for tensor
//   handle of grad w.r.t. o_i.  Use VM_OPT_SET_GRAD_TENSOR_HANDLER.
//
// In-Place:
//   - Not allowed.
VM_OP(LS_SCEL)
