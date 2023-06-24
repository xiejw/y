#include <mlvm/kernel.h>

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>  // memcpy

#include "crypto/rng64.h"

// #ifdef BLIS
// #include "primitives_blis.h"
// #endif

// -----------------------------------------------------------------------------
// element wise
// -----------------------------------------------------------------------------

// Currently, we sacrify the correctness for float coparision for performance.
// It might be a bad idea. We should improve it in future.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"

#define PLUS(x, y) ((x) + (y))
#define MULT(x, y) ((x) * (y))
#define MINU(x, y) ((x) - (y))
#define DIVI(x, y) ((x) / (y))
#define MAXI(x, y) ((x) > (y) ? (x) : (y))
#define EQUA(x, y) ((x) == (y))
#define CMPL(x, y) ((x) > (y) ? (1) : (0))

#define DEF_ELEWISE_OP(OP, op, static_cond, special_handle)                    \
        error_t vmOp##OP##F32(struct vm_tensor *td, struct vm_tensor *t1,      \
                              struct vm_tensor *t2)                            \
        {                                                                      \
                assert(td->dtype == VM_F32);                                   \
                assert(t1->dtype == VM_F32);                                   \
                assert(t2->dtype == VM_F32);                                   \
                                                                               \
                size_t size = t1->shape->size;                                 \
                if (size != td->shape->size) {                                 \
                        return errNew(                                         \
                            "Op " #OP                                          \
                            " support t1 and t2 same shape or t2 as scalar."); \
                }                                                              \
                                                                               \
                f32_t *o   = (f32_t *)td->data;                                \
                f32_t *lhs = (f32_t *)t1->data;                                \
                f32_t *rhs = (f32_t *)t2->data;                                \
                                                                               \
                size_t t2_size = t2->shape->size;                              \
                                                                               \
                if (static_cond) {                                             \
                        special_handle(o, lhs, rhs, size, t2_size);            \
                } else {                                                       \
                        if (t2_size == size) {                                 \
                                for (size_t i = 0; i < size; i++) {            \
                                        o[i] = op(lhs[i], rhs[i]);             \
                                }                                              \
                        } else if (t2_size == 1) {                             \
                                f32_t s = rhs[0];                              \
                                for (size_t i = 0; i < size; i++) {            \
                                        o[i] = op(lhs[i], s);                  \
                                }                                              \
                        } else if (size % t2_size == 0) {                      \
                                size_t loop_c = size / t2_size;                \
                                for (size_t c = 0; c < loop_c; c++) {          \
                                        size_t offset = c * t2_size;           \
                                        for (size_t i = 0; i < t2_size; i++) { \
                                                o[offset + i] = op(            \
                                                    lhs[offset + i], rhs[i]);  \
                                        }                                      \
                                }                                              \
                        } else {                                               \
                                return errNew("Op " #OP                        \
                                              " support t1s==t2s, "            \
                                              "t1s%%t2s==0 or t2s==1.");       \
                        }                                                      \
                }                                                              \
                return OK;                                                     \
        }

#define DUMMY_HANDLER(x, y, v, s, s2)

DEF_ELEWISE_OP(Add, PLUS, 0, DUMMY_HANDLER)
DEF_ELEWISE_OP(Mul, MULT, 0, DUMMY_HANDLER)
DEF_ELEWISE_OP(Divide, DIVI, 0, DUMMY_HANDLER)
DEF_ELEWISE_OP(Max, MAXI, 0, DUMMY_HANDLER)
DEF_ELEWISE_OP(Eq, EQUA, 0, DUMMY_HANDLER)
DEF_ELEWISE_OP(CmpL, CMPL, 0, DUMMY_HANDLER)

// #ifndef BLIS
DEF_ELEWISE_OP(Minus, MINU, 0, DUMMY_HANDLER)
// #else
// DEF_ELEWISE_OP(Minus, MINU, 1, vmBlisMinusF32)
// #endif

#undef DUMMY_HANDLER
#undef DEF_ELEWISE_OP

#define DEF_ELEWISE_OP_S(OP, op, static_cond, special_handle)              \
        error_t vmOp##OP##SF32(struct vm_tensor *td, struct vm_tensor *t1, \
                               f32_t s)                                    \
        {                                                                  \
                assert(td->dtype == VM_F32);                               \
                assert(t1->dtype == VM_F32);                               \
                                                                           \
                size_t size = t1->shape->size;                             \
                assert(size == td->shape->size);                           \
                                                                           \
                f32_t *o   = (f32_t *)td->data;                            \
                f32_t *lhs = (f32_t *)t1->data;                            \
                                                                           \
                if (static_cond) {                                         \
                        special_handle(o, lhs, s, size);                   \
                } else {                                                   \
                        for (size_t i = 0; i < size; i++) {                \
                                o[i] = op(lhs[i], s);                      \
                        }                                                  \
                }                                                          \
                return OK;                                                 \
        }

#define DUMMY_HANDLER(x, y, v, s)

DEF_ELEWISE_OP_S(Add, PLUS, 0, DUMMY_HANDLER)
DEF_ELEWISE_OP_S(Minus, MINU, 0, DUMMY_HANDLER)
DEF_ELEWISE_OP_S(Divide, DIVI, 0, DUMMY_HANDLER)
DEF_ELEWISE_OP_S(Max, MAXI, 0, DUMMY_HANDLER)
DEF_ELEWISE_OP_S(Eq, EQUA, 0, DUMMY_HANDLER)
DEF_ELEWISE_OP_S(CmpL, CMPL, 0, DUMMY_HANDLER)

// #ifndef BLIS
DEF_ELEWISE_OP_S(Mul, MULT, 0, DUMMY_HANDLER)
// #else
// DEF_ELEWISE_OP_S(Mul, MULT, 1, vmBlisMulSF32)
// #endif

#undef DUMMY_HANDLER
#undef DEF_ELEWISE_OP_S

#pragma clang diagnostic pop  // "-Wfloat-equal"

// -----------------------------------------------------------------------------
// rng
// -----------------------------------------------------------------------------
error_t
vmOpRngF32(struct vm_tensor *dst, int mode, struct rng64 *rng)
{
        assert(mode == 0);
        assert(dst->dtype == VM_F32);
        rng64StdNormF32(rng, dst->shape->size, (f32_t *)dst->data);
        return OK;
}

// -----------------------------------------------------------------------------
// arg
// -----------------------------------------------------------------------------
error_t
vmOpArgMaxF32(struct vm_tensor *td, struct vm_tensor *t1)
{
        assert(td->dtype == VM_F32);
        assert(t1->dtype == VM_F32);
        assert(1 == td->shape->rank);
        assert(2 == t1->shape->rank);

        size_t bs = (size_t)(td->shape->dims[0]);
        assert(bs == (size_t)t1->shape->dims[0]);
        size_t ft = (size_t)(t1->shape->dims[1]);

        f32_t *tgt  = (f32_t *)td->data;
        f32_t *data = (f32_t *)t1->data;

        for (size_t i = 0; i < bs; i++) {
                size_t offset = i * ft;
                size_t index  = 0;
                f32_t v       = data[offset];
                for (size_t j = 1; j < ft; j++) {
                        if (v < data[offset + j]) {
                                v     = data[offset + j];
                                index = j;
                        }
                }
                tgt[i] = (f32_t)index;
        }
        return OK;
}

// -----------------------------------------------------------------------------
// reduction
// -----------------------------------------------------------------------------

error_t
vmOpReduceF32(struct vm_tensor *td, struct vm_tensor *t1, int mode, int axis)
{
        assert(mode == 0);  // sum
        assert(t1->dtype == VM_F32);
        assert(td->dtype == VM_F32);

        if (axis == 0) {
                assert(1 == td->shape->size);
                f32_t v     = 0;
                size_t size = t1->shape->size;
                f32_t *data = (f32_t *)t1->data;
                for (size_t i = 0; i < size; i++) {
                        v += data[i];
                }
                *(f32_t *)td->data = v;
                return OK;
        } else if (axis > 0) {
                // reduce heading axes. For [A, B] reducing values B along A.
                assert(axis + td->shape->rank == t1->shape->rank);
                assert(t1->shape->size > td->shape->size);
                size_t value_size = td->shape->size;
                size_t loop_count = t1->shape->size / value_size;

                f32_t *dst = (f32_t *)td->data;
                f32_t *src = (f32_t *)t1->data;
                memcpy(dst, src, value_size * sizeof(f32_t));
                for (size_t i = 1; i < loop_count; i++) {
                        src += value_size;
                        for (size_t j = 0; j < value_size; j++) {
                                dst[j] += src[j];
                        }
                }
                return OK;
        } else {
                assert(axis < 0);
                axis = -axis;
                // reduce heading axes. For [A, B] reducing values B to scalar.
                assert(axis + td->shape->rank == t1->shape->rank);
                assert(t1->shape->size > td->shape->size);
                size_t loop_count = td->shape->size;
                size_t value_size = t1->shape->size / loop_count;

                f32_t *dst = (f32_t *)td->data;
                f32_t *src = (f32_t *)t1->data;
                for (size_t i = 0; i < loop_count; i++) {
                        f32_t v = 0.0;
                        for (size_t j = 0; j < value_size; j++) {
                                v += src[j];
                        }
                        dst[i] = v;
                        src += value_size;
                }
                return OK;
        }
}

// -----------------------------------------------------------------------------
// matmul
// -----------------------------------------------------------------------------
error_t
vmOpMatmulF32(struct vm_tensor *td, struct vm_tensor *t1, struct vm_tensor *t2,
              int trans_lhs, int trans_rhs)
{
        assert(td != t1);
        assert(td != t2);
        assert(td->dtype == VM_F32);
        assert(t1->dtype == VM_F32);
        assert(t2->dtype == VM_F32);
        assert(td->shape->rank == 2);
        assert(t1->shape->rank == 2);
        assert(t2->shape->rank == 2);

        f32_t *o   = (f32_t *)td->data;
        f32_t *lhs = (f32_t *)t1->data;
        f32_t *rhs = (f32_t *)t2->data;

        assert(trans_lhs == 0 || trans_rhs == 0);  // not support both.

        if (!trans_lhs && !trans_rhs) {
                // pm,mq -> pq
                int p = td->shape->dims[0];
                int q = td->shape->dims[1];
                int m = t1->shape->dims[1];

                if (p != t1->shape->dims[0] || m != t2->shape->dims[0] ||
                    q != t2->shape->dims[1]) {
                        return errNew(
                            "invalid matmul shape: %d/%d,%d/%d->%d/%d.",
                            t1->shape->dims[0], t1->shape->dims[1],
                            t2->shape->dims[0], t2->shape->dims[1],
                            td->shape->dims[0], td->shape->dims[1]);
                }

                // #ifdef BLIS
                //                 vmBlisMatmulF32(p, q, m, lhs, rhs, o);
                // #else
                //  stupid impl.
                for (int i = 0; i < p; i++) {
                        for (int j = 0; j < q; j++) {
                                f32_t v = 0;
                                for (int k = 0; k < m; k++) {
                                        // For lhs, stride is (m, 1).
                                        // For rhs, stride is (q, 1).
                                        v += lhs[i * m + k] * rhs[k * q + j];
                                }
                                o[i * q + j] = v;
                        }
                }
                // #endif
                return OK;
        } else if (trans_rhs) {
                // pm,mq -> pq
                int p = td->shape->dims[0];
                int q = td->shape->dims[1];
                int m = t1->shape->dims[1];

                if (p != t1->shape->dims[0] || m != t2->shape->dims[1] ||
                    q != t2->shape->dims[0]) {
                        return errNew(
                            "invalid matmul shape: %d/%d,%d/%d->%d/%d.",
                            t1->shape->dims[0], t1->shape->dims[1],
                            t2->shape->dims[1], t2->shape->dims[0],
                            td->shape->dims[0], td->shape->dims[1]);
                }

                // #ifdef BLIS
                //                 vmBlisMatmulTRF32(p, q, m, lhs, rhs, o);
                // #else
                //  stupid impl.
                for (int i = 0; i < p; i++) {
                        for (int j = 0; j < q; j++) {
                                f32_t v = 0;
                                for (int k = 0; k < m; k++) {
                                        // For lhs, stride is (m, 1).
                                        // For rhs, stride is (1, m).
                                        v += lhs[i * m + k] * rhs[k + j * m];
                                }
                                o[i * q + j] = v;
                        }
                }
                // #endif
                return OK;
        } else {
                assert(trans_lhs);
                // pm,mq -> pq
                int p = td->shape->dims[0];
                int q = td->shape->dims[1];
                int m = t1->shape->dims[0];

                if (p != t1->shape->dims[1] || m != t2->shape->dims[0] ||
                    q != t2->shape->dims[1]) {
                        return errNew(
                            "invalid matmul shape: %d/%d,%d/%d->%d/%d.",
                            t1->shape->dims[1], t1->shape->dims[0],
                            t2->shape->dims[0], t2->shape->dims[1],
                            td->shape->dims[0], td->shape->dims[1]);
                }

                // #ifdef BLIS
                //                 vmBlisMatmulTLF32(p, q, m, lhs, rhs, o);
                // #else
                //  stupid impl.
                for (int i = 0; i < p; i++) {
                        for (int j = 0; j < q; j++) {
                                f32_t v = 0;
                                for (int k = 0; k < m; k++) {
                                        // For lhs, stride is (1, p).
                                        // For rhs, stride is (q, 1).
                                        v += lhs[i + k * p] * rhs[k * q + j];
                                }
                                o[i * q + j] = v;
                        }
                }
                // #endif
                return OK;
        }
}

// -----------------------------------------------------------------------------
// fill
// -----------------------------------------------------------------------------
error_t
vmOpFillF32(struct vm_tensor *td, f32_t v)
{
        assert(td->dtype == VM_F32);
        size_t size = td->shape->size;
        f32_t *data = (f32_t *)td->data;
        for (size_t i = 0; i < size; i++) {
                data[i] = v;
        }
        return OK;
}

// -----------------------------------------------------------------------------
// loss
// -----------------------------------------------------------------------------
error_t
vmOpLossSCELF32(struct vm_tensor *td, struct vm_tensor *t1,
                struct vm_tensor *t2, struct vm_tensor *tg)
{
        assert(td != t1);
        assert(td != t2);
        assert(td->dtype == VM_F32);
        assert(t1->dtype == VM_F32);
        assert(t2->dtype == VM_F32);
        assert(td->shape->rank == 1);
        assert(t1->shape->rank == 2);
        assert(t2->shape->rank == 2);
        size_t bs = (size_t)(td->shape->dims[0]);
        size_t ft = (size_t)(t1->shape->dims[1]);

        if (t1->shape->dims[0] != (int)bs || t2->shape->dims[0] != (int)bs ||
            t2->shape->dims[1] != (int)ft) {
                return errNew("invalid LossSCEL shape: %d/%d,%d/%d->%d.",
                              t1->shape->dims[0], t1->shape->dims[1],
                              t2->shape->dims[0], t2->shape->dims[1],
                              td->shape->dims[0]);
        }
        f32_t *loss = (f32_t *)td->data;
        f32_t *y    = (f32_t *)t1->data;
        f32_t *o    = (f32_t *)t2->data;
        f32_t *g    = NULL;

        if (tg != NULL) {
                // check gradient dtype and shapes.
                assert(tg->dtype == VM_F32);
                assert(tg->shape->rank == 2);
                assert(tg->shape->dims[0] == t2->shape->dims[0]);
                assert(tg->shape->dims[1] == t2->shape->dims[1]);
                g = (f32_t *)tg->data;
        }

#define LOSS_SCEL_COMP(trigger_cond, static_grad_cond)                       \
        if ((trigger_cond)) {                                                \
                for (size_t i = 0; i < bs; i++) {                            \
                        /* find max and shift the value toward num stable.*/ \
                        size_t offset   = i * ft;                            \
                        float_t max_o_k = o[offset];                         \
                        for (size_t k = 1; k < ft; k++) {                    \
                                f32_t o_k = o[offset + k];                   \
                                if (o_k > max_o_k) max_o_k = o_k;            \
                        }                                                    \
                                                                             \
                        /* real formular */                                  \
                        f32_t sum = 0.0;                                     \
                        f32_t l   = 0.0;                                     \
                        for (size_t k = 0; k < ft; k++) {                    \
                                f32_t o_k     = o[offset + k] - max_o_k;     \
                                f32_t exp_o_k = expf(o_k);                   \
                                sum += exp_o_k;                              \
                                if ((static_grad_cond)) {                    \
                                        g[offset + k] = exp_o_k;             \
                                }                                            \
                                l -= y[offset + k] * o_k;                    \
                        }                                                    \
                                                                             \
                        if ((static_grad_cond)) {                            \
                                /* normalize all components. */              \
                                f32_t *gp = g + offset;                      \
                                /* p_i - y_i is the gradient w.r.t. o_i. */  \
                                f32_t *yp = y + offset;                      \
                                for (size_t k = 0; k < ft; k++) {            \
                                        (*gp) = (*gp) / sum - (*yp);         \
                                        gp++;                                \
                                        yp++;                                \
                                }                                            \
                        }                                                    \
                                                                             \
                        loss[i] = l + logf(sum);                             \
                }                                                            \
        }

        // Defines macros to repeat the code twice. The hope is the compiler
        // can remove the control flow to avoid runtime cost.
        LOSS_SCEL_COMP(g != NULL, 1)
        LOSS_SCEL_COMP(g == NULL, 0)

#undef LOSS_SCEL_COMP

        return OK;
}

// -----------------------------------------------------------------------------
// inverse sqrt
// -----------------------------------------------------------------------------
error_t
vmOpISqrtF32(struct vm_tensor *td, struct vm_tensor *t1, const f32_t *e,
             int mode)
{
        assert(td->dtype == VM_F32);
        assert(t1->dtype == VM_F32);

        f32_t *dst = (f32_t *)td->data;
        f32_t *src = (f32_t *)t1->data;

        size_t size = t1->shape->size;
        if (size != td->shape->size) {
                return errNew("Op_ISQRT must have equal size.");
        }

        if (e == NULL) {
                for (size_t i = 0; i < size; i++) {
                        dst[i] = 1.0f / sqrtf(src[i]);
                }
        } else if (mode == 0) {
                f32_t v = *e;
                for (size_t i = 0; i < size; i++) {
                        dst[i] = 1.0f / sqrtf((src[i] + v));
                }
        } else {
                assert(mode == 1);
                f32_t v = *e;
                for (size_t i = 0; i < size; i++) {
                        dst[i] = 1.0f / (sqrtf(src[i]) + v);
                }
        }
        return OK;
}
