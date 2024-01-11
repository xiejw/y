#include <mlvm/vm.h>

#include <stdlib.h>  // calloc
#include <string.h>  // memset

// eva internal
#include <mlvm/kernel.h>
#include <mlvm/vm_internal.h>

// -----------------------------------------------------------------------------
// impl for vm apis
// -----------------------------------------------------------------------------
struct vm *
vmNew()
{
        return calloc(1, sizeof(struct vm));
}

void
vmFree(struct vm *vm)
{
        // release tensors
        for (int i = 0; i < MLVM_MAX_TENSOR_COUNT; i++) {
                struct vm_tensor *t = &vm->handles[i];
                if (t->used) vmReleaseHandle(t);
        }

        // release shapes managed by vm
        struct _vm_list *curr = vm->shapes;
        struct _vm_list *next;
        while (curr != NULL) {
                next = curr->next;
                spDecRef(curr->data);
                free(curr);
                curr = next;
        }

        // release vm
        free(vm);
}

struct vm_sp *
vmSpNew(struct vm *vm, int rank, int dims[])
{
        struct vm_sp *s = spNew(rank, dims);

        // attach to vm shape lists
        struct _vm_list *n = malloc(sizeof(struct _vm_list));
        n->data            = s;
        if (vm->shapes == NULL) {
                n->next    = NULL;
                vm->shapes = n;
        } else {
                n->next    = vm->shapes;
                vm->shapes = n;
        }

        return s;
}

error_t
vmBatch(struct vm *vm, size_t size, const struct vm_oparg *args)
{
        const struct vm_opopt *opt;
        const struct vm_oparg *arg;
        error_t err;

        for (size_t i = 0; i < size; i++) {
                arg = &args[i];
                if (arg->has_opt)
                        opt = &arg->opt;
                else {
                        opt = NULL;
                        assert(arg->opt.mode == 0);
                }

                err = vmExec(vm, arg->op, opt, arg->dst, arg->t1, arg->t2);
                if (err) {
                        return errEmitNote("failed to exec %d-th instruction.",
                                           i);
                }
        }

        return OK;
}

#define VM_OP(op) [VM_OP_##op] = #op,
static const char *op_names[] = {
#include "mlvm/op.h"
};
#undef VM_OP

void
vmOpsToSds(sds_t *psds, size_t size, const struct vm_oparg *args)
{
        const struct vm_oparg *arg;

        for (size_t i = 0; i < size; i++) {
                arg = &args[i];
                if (arg->has_opt)
                        errFatalAndExit1("has opt does not support yet");

                const char *op_name = op_names[arg->op];
                sdsCatPrintf(
                    psds,
                    "{ .op = \"%10s\", .dst = %3d, .t1 = %3d, .t2 = %3d }\n",
                    op_name, arg->dst, arg->t1, arg->t2);
        }
}

// -----------------------------------------------------------------------------
// impl for vm exec
// -----------------------------------------------------------------------------
error_t
vmExec(struct vm *vm, enum vm_opcode op, const struct vm_opopt *opt, int dst,
       int lhs, int rhs)
{
        struct vm_tensor *td = vmGrabHandle(vm, dst);
        struct vm_tensor *t1 = (lhs != -1) ? vmGrabHandle(vm, lhs) : NULL;
        struct vm_tensor *t2 = (rhs != -1) ? vmGrabHandle(vm, rhs) : NULL;

        switch (op) {
#define CASE_ELEWISE_OP(OP, API)                                               \
        case VM_OP_##OP:                                                       \
                assert(t1 != NULL);                                            \
                if (td->dtype == VM_F32) {                                     \
                        if (opt == NULL) {                                     \
                                assert(t2 != NULL);                            \
                                return vmOp##API##F32(td, t1, t2);             \
                        } else {                                               \
                                assert(t2 == NULL);                            \
                                assert(VM_OPT_MODE_GET_F_BIT(*opt));           \
                                return vmOp##API##SF32(td, t1, opt->f);        \
                        }                                                      \
                }                                                              \
                                                                               \
                return errNewWithNote(ENOTIMPL,                                \
                                      "unimpl forVM_OP_" #OP " with dtype %d", \
                                      td->dtype);

                CASE_ELEWISE_OP(ADD, Add)
                CASE_ELEWISE_OP(MUL, Mul)
                CASE_ELEWISE_OP(MINUS, Minus)
                CASE_ELEWISE_OP(DIVIDE, Divide)
                CASE_ELEWISE_OP(MAX, Max)
                CASE_ELEWISE_OP(CMPL, CmpL)
                CASE_ELEWISE_OP(EQ, Eq)

#undef CASE_ELEWISE_OP

        case VM_OP_MATMUL:
                assert(t1 != NULL);
                assert(t2 != NULL);
                if (td->dtype == VM_F32) {
                        int trans_lhs = 0;
                        int trans_rhs = 0;
                        if (opt != NULL &&
                            opt->mode != VM_OPT_MATMUL_TRANS_NOT) {
                                if (opt->mode == VM_OPT_MATMUL_TRANS_LHS) {
                                        trans_lhs = 1;
                                } else {
                                        assert(opt->mode ==
                                               VM_OPT_MATMUL_TRANS_RHS);
                                        trans_rhs = 1;
                                }
                        }
                        return vmOpMatmulF32(td, t1, t2, trans_lhs, trans_rhs);
                }

                return errNewWithNote(ENOTIMPL,
                                      "unimpl forVM_OP_MATMUL with dtype %d",
                                      td->dtype);

        case VM_OP_RNG:
                assert(opt != NULL);
                assert(t1 == NULL);
                assert(t2 == NULL);
                assert(VM_OPT_MODE_GET_R_BIT(*opt));
                assert((opt->mode & VM_OPT_MODE_UNMASK) == 0);
                struct rng64 rng = opt->r;
                if (td->dtype == VM_F32) {
                        return vmOpRngF32(td, opt->mode & VM_OPT_MODE_UNMASK,
                                          &rng);
                }

                return errNewWithNote(
                    ENOTIMPL, "unimpl forVM_OP_RNG with dtype %d", td->dtype);

        case VM_OP_ARGMAX:
                assert(opt == NULL);
                assert(t1 != NULL);
                assert(t2 == NULL);

                if (td->dtype == VM_F32) {
                        return vmOpArgMaxF32(td, t1);
                }

                return errNewWithNote(ENOTIMPL,
                                      "unimpl forVM_OP_ARGMAX with dtype %d",
                                      td->dtype);

        case VM_OP_REDUCE:
                assert(opt != NULL);
                assert((opt->mode & VM_OPT_MODE_UNMASK) == 0);
                assert(t1 != NULL);
                assert(t2 == NULL);
                if (td->dtype != VM_F32) {
                        return errNewWithNote(
                            ENOTIMPL, "unimpl forVM_OP_REDUCE with dtype %d",
                            td->dtype);
                }

                int axis = 0;
                if (VM_OPT_MODE_GET_I_BIT(*opt)) {
                        axis = opt->i;
                } else {
                        assert(opt->i == 0);
                }
                return vmOpReduceF32(td, t1, opt->mode & VM_OPT_MODE_UNMASK,
                                     axis);

        case VM_OP_FILL:
                assert(t1 == NULL);
                assert(t2 == NULL);
                if (td->dtype != VM_F32) {
                        return errNewWithNote(
                            ENOTIMPL, "unimpl forVM_OP_FILL with dtype %d",
                            td->dtype);
                }

                if (opt == NULL) {
                        memset(td->data, 0, td->shape->size * sizeof(f32_t));
                        return OK;
                }
                assert((opt->mode & VM_OPT_MODE_UNMASK) == 0);
                assert(VM_OPT_MODE_GET_F_BIT(*opt));
                return vmOpFillF32(td, opt->f);

        case VM_OP_ISQRT:
                assert(t2 == NULL);
                if (td->dtype != VM_F32) {
                        return errNewWithNote(
                            ENOTIMPL, "unimpl forVM_OP_ISQRT with dtype %d",
                            td->dtype);
                }

                if (opt == NULL) {
                        return vmOpISqrtF32(td, t1, NULL,
                                            /*mode=*/-1);
                }

                assert((opt->mode & VM_OPT_MODE_UNMASK) >> 1 == 0);
                assert(VM_OPT_MODE_GET_F_BIT(*opt));
                return vmOpISqrtF32(td, t1, &opt->f,
                                    /*mode=*/opt->mode & VM_OPT_MODE_UNMASK);

        case VM_OP_LS_SCEL:
                assert(t1 != NULL);
                assert(t2 != NULL);
                struct vm_tensor *tg = NULL;
                if (td->dtype != VM_F32) {
                        return errNewWithNote(
                            ENOTIMPL, "unimpl forVM_OP_LS_SCEL with dtype %d",
                            td->dtype);
                }

                if (opt != NULL) {
                        if (VM_OPT_MODE_GET_I_BIT(*opt)) {
                                tg = vmGrabHandle(
                                    vm,
                                    opt->i);  // handle of the gradient.
                        } else {
                                assert(opt->i == 0);
                        }
                }
                return vmOpLossSCELF32(td, t1, t2, tg);

        default:
                return errNewWithNote(ENOTIMPL, "unimpl op for vmExec: %d", op);
        }
}
