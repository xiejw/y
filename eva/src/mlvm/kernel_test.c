#include <testing/testing.h>

#include <mlvm/vm.h>

#include <string.h>

// -----------------------------------------------------------------------------
// helper prototype
// -----------------------------------------------------------------------------
error_t copy_data(struct vm *vm, int td, size_t size, f32_t *src);
error_t expect_dump(struct vm *vm, int td, const char *expected);

#define COPY_DATA(vm, td, size, src)                  \
        do {                                          \
                if (copy_data(vm, td, size, src)) {   \
                        return "failed to copy data"; \
                }                                     \
        } while (0)

#define CHECK_TENSOR(vm, td, expect, fmt, ...)                 \
        do {                                                   \
                if (expect_dump(vm, td, expect)) {             \
                        errDump(fmt, __VA_ARGS__);             \
                        return "failed to expect tensor dump"; \
                }                                              \
        } while (0)

#define NE(e)                                       \
        do {                                        \
                if ((e)) {                          \
                        errDump("error:\n");        \
                        return "unexpected error."; \
                }                                   \
        } while (0)

// -----------------------------------------------------------------------------
// unit tests.
// -----------------------------------------------------------------------------

static char *
test_ele_ops()
{
        struct vm *vm   = vmNew();
        struct vm_sp *s = vmSpNew(vm, 2, (int[]){1, 3});

        int t1 = vmTensorNew(vm, VM_F32, s);
        int t2 = vmTensorNew(vm, VM_F32, s);
        int td = vmTensorNew(vm, VM_F32, s);

        COPY_DATA(vm, t1, 3, ((f32_t[]){2.34f, 5.67f, 2.00f}));
        COPY_DATA(vm, t2, 3, ((f32_t[]){4.34f, 3.67f, 2.00f}));

        enum vm_opcode ops[] = {VM_OP_ADD, VM_OP_MUL, VM_OP_MINUS, VM_OP_DIVIDE,
                                VM_OP_MAX, VM_OP_EQ,  VM_OP_CMPL};

        const char *expected_strs[] = {
            "<1, 3> f32 [6.680, 9.340, 4.000]",
            "<1, 3> f32 [10.156, 20.809, 4.000]",
            "<1, 3> f32 [-2.000, 2.000, 0.000]",
            "<1, 3> f32 [0.539, 1.545, 1.000]",
            "<1, 3> f32 [4.340, 5.670, 2.000]",
            "<1, 3> f32 [0.000, 0.000, 1.000]",
            "<1, 3> f32 [0.000, 1.000, 0.000]",
        };

        for (int i = 0; i < (int)(sizeof(ops) / sizeof(enum vm_opcode)); i++) {
                NE(vmExec(vm, ops[i], NULL, td, t1, t2));
                CHECK_TENSOR(vm, td, expected_strs[i], "failed at %d-th Op\n",
                             i);
        }
        vmFree(vm);
        return NULL;
}

static char *
test_ele_ops_unequal_size()
{
        struct vm *vm    = vmNew();
        struct vm_sp *s1 = vmSpNew(vm, 2, (int[]){1, 4});
        struct vm_sp *s2 = vmSpNew(vm, 2, (int[]){1, 2});

        int t1 = vmTensorNew(vm, VM_F32, s1);
        int t2 = vmTensorNew(vm, VM_F32, s2);
        int td = vmTensorNew(vm, VM_F32, s1);

        COPY_DATA(vm, t1, 4, ((f32_t[]){2.34f, 5.67f, 4.34f, 2.00f}));
        COPY_DATA(vm, t2, 2, ((f32_t[]){3.67f, 2.00f}));

        enum vm_opcode ops[] = {VM_OP_ADD, VM_OP_MUL, VM_OP_MINUS, VM_OP_DIVIDE,
                                VM_OP_MAX, VM_OP_EQ,  VM_OP_CMPL};

        const char *expected_strs[] = {
            "<1, 4> f32 [6.010, 7.670, 8.010, 4.000]",
            "<1, 4> f32 [8.588, 11.340, 15.928, 4.000]",
            "<1, 4> f32 [-1.330, 3.670, 0.670, 0.000]",
            "<1, 4> f32 [0.638, 2.835, 1.183, 1.000]",
            "<1, 4> f32 [3.670, 5.670, 4.340, 2.000]",
            "<1, 4> f32 [0.000, 0.000, 0.000, 1.000]",
            "<1, 4> f32 [0.000, 1.000, 1.000, 0.000]",
        };

        for (int i = 0; i < (int)(sizeof(ops) / sizeof(enum vm_opcode)); i++) {
                NE(vmExec(vm, ops[i], NULL, td, t1, t2));
                CHECK_TENSOR(vm, td, expected_strs[i], "failed at %d-th Op\n",
                             i);
        }
        vmFree(vm);
        return NULL;
}

static char *
test_ele_ops_scalar_operand()
{
        struct vm *vm    = vmNew();
        struct vm_sp *s  = vmSpNew(vm, 2, (int[]){1, 3});
        struct vm_sp *s1 = vmSpNew(vm, 1, (int[]){1});

        int t1 = vmTensorNew(vm, VM_F32, s);
        int t2 = vmTensorNew(vm, VM_F32, s1);
        int td = vmTensorNew(vm, VM_F32, s);

        COPY_DATA(vm, t1, 3, ((f32_t[]){2.34f, 5.67f, 3.67f}));
        COPY_DATA(vm, t2, 1, ((f32_t[]){3.67f}));

        enum vm_opcode ops[] = {VM_OP_ADD, VM_OP_MUL, VM_OP_MINUS, VM_OP_DIVIDE,
                                VM_OP_MAX, VM_OP_EQ,  VM_OP_CMPL};

        const char *expected_strs[] = {
            "<1, 3> f32 [6.010, 9.340, 7.340]",
            "<1, 3> f32 [8.588, 20.809, 13.469]",
            "<1, 3> f32 [-1.330, 2.000, 0.000]",
            "<1, 3> f32 [0.638, 1.545, 1.000]",
            "<1, 3> f32 [3.670, 5.670, 3.670]",
            "<1, 3> f32 [0.000, 0.000, 1.000]",
            "<1, 3> f32 [0.000, 1.000, 0.000]",
        };

        for (int i = 0; i < (int)(sizeof(ops) / sizeof(enum vm_opcode)); i++) {
                NE(vmExec(vm, ops[i], NULL, td, t1, t2));
                CHECK_TENSOR(vm, td, expected_strs[i], "failed at %d-th Op\n",
                             i);
        }
        vmFree(vm);
        return NULL;
}

static char *
test_ele_ops_f_bit()
{
        struct vm *vm   = vmNew();
        struct vm_sp *s = vmSpNew(vm, 2, (int[]){1, 3});

        int t1 = vmTensorNew(vm, VM_F32, s);
        int td = vmTensorNew(vm, VM_F32, s);

        COPY_DATA(vm, t1, 3, ((f32_t[]){2.34f, 5.67f, 3.000f}));

        enum vm_opcode ops[] = {VM_OP_ADD, VM_OP_MUL, VM_OP_MINUS,
                                VM_OP_MAX, VM_OP_EQ,  VM_OP_CMPL};

        const char *expected_strs[] = {
            "<1, 3> f32 [5.340, 8.670, 6.000]",
            "<1, 3> f32 [7.020, 17.010, 9.000]",
            "<1, 3> f32 [-0.660, 2.670, 0.000]",
            "<1, 3> f32 [3.000, 5.670, 3.000]",
            "<1, 3> f32 [0.000, 0.000, 1.000]",
            "<1, 3> f32 [0.000, 1.000, 0.000]",
        };

        const struct vm_opopt opt = {.mode = VM_OPT_MODE_F_BIT, .f = 3};
        for (int i = 0; i < (int)(sizeof(ops) / sizeof(enum vm_opcode)); i++) {
                NE(vmExec(vm, ops[i], &opt, td, t1, -1));
                CHECK_TENSOR(vm, td, expected_strs[i], "failed at %d-th Op\n",
                             i);
        }
        vmFree(vm);
        return NULL;
}

static char *
test_matmul()
{
        struct vm *vm   = vmNew();
        struct vm_sp *s = vmSpNew(vm, 2, (int[]){2, 2});

        int t1 = vmTensorNew(vm, VM_F32, s);
        int t2 = vmTensorNew(vm, VM_F32, s);
        int td = vmTensorNew(vm, VM_F32, s);

        COPY_DATA(vm, t1, 4, ((f32_t[]){2.34f, 5.67f, -1.23f, 2.34f}));
        COPY_DATA(vm, t2, 4, ((f32_t[]){4.34f, 3.67f, -2.24f, 3.45f}));

        struct vm_opopt opt1 = {.mode = VM_OPT_MATMUL_TRANS_NOT};
        struct vm_opopt opt2 = {.mode = VM_OPT_MATMUL_TRANS_LHS};
        struct vm_opopt opt3 = {.mode = VM_OPT_MATMUL_TRANS_RHS};

        struct vm_opopt *opts[] = {NULL, &opt1, &opt2, &opt3};

        const char *expected_strs[] = {
            "<2, 2> f32 [-2.545, 28.149, -10.580, 3.559]",
            "<2, 2> f32 [-2.545, 28.149, -10.580, 3.559]",
            "<2, 2> f32 [12.911, 4.344, 19.366, 28.882]",
            "<2, 2> f32 [30.965, 14.320, 3.250, 10.828]",
        };

        for (int i = 0; i < (int)(sizeof(opts) / sizeof(struct vm_opopt *));
             i++) {
                NE(vmExec(vm, VM_OP_MATMUL, opts[i], td, t1, t2));
                CHECK_TENSOR(vm, td, expected_strs[i], "failed at %d-th opt\n",
                             i);
        }

        vmFree(vm);
        return NULL;
}

static char *
test_argmax()
{
        struct vm *vm    = vmNew();
        struct vm_sp *s1 = vmSpNew(vm, 2, (int[]){2, 2});
        struct vm_sp *s2 = vmSpNew(vm, 1, (int[]){2});

        int t1 = vmTensorNew(vm, VM_F32, s1);
        int td = vmTensorNew(vm, VM_F32, s2);

        COPY_DATA(vm, t1, 4, ((f32_t[]){2.34f, 1.67f, -1.23f, 2.34f}));

        const char *expected_str = "<2> f32 [0.000, 1.000]";

        NE(vmExec(vm, VM_OP_ARGMAX, NULL, td, t1, -1));
        CHECK_TENSOR(vm, td, expected_str, "failed %s", "argmax");

        vmFree(vm);
        return NULL;
}

static char *
test_isqrt()
{
        struct vm *vm   = vmNew();
        struct vm_sp *s = vmSpNew(vm, 2, (int[]){2, 2});

        int t1 = vmTensorNew(vm, VM_F32, s);
        int td = vmTensorNew(vm, VM_F32, s);

        COPY_DATA(vm, t1, 4, ((f32_t[]){2.34f, 5.67f, 0.00f, 2.34f}));

        struct vm_opopt opt1 = {.mode = VM_OPT_MODE_F_BIT, .f = 2.0f};
        struct vm_opopt opt2 = {.mode = 1 | VM_OPT_MODE_F_BIT, .f = 2.0f};

        struct vm_opopt *opts[] = {NULL, &opt1, &opt2};

        const char *expected_strs[] = {
            "<2, 2> f32 [0.654, 0.420, inf, 0.654]",
            "<2, 2> f32 [0.480, 0.361, 0.707, 0.480]",
            "<2, 2> f32 [0.283, 0.228, 0.500, 0.283]",
        };

        for (int i = 0; i < (int)(sizeof(opts) / sizeof(struct vm_opopt *));
             i++) {
                NE(vmExec(vm, VM_OP_ISQRT, opts[i], td, t1, -1));
                CHECK_TENSOR(vm, td, expected_strs[i], "failed at %d-th opt\n",
                             i);
        }

        vmFree(vm);
        return NULL;
}

static char *
test_reduce()
{
        struct vm *vm    = vmNew();
        struct vm_sp *s  = vmSpNew(vm, 2, (int[]){2, 3});
        struct vm_sp *s1 = vmSpNew(vm, 1, (int[]){1});
        struct vm_sp *s2 = vmSpNew(vm, 1, (int[]){3});
        struct vm_sp *s3 = vmSpNew(vm, 1, (int[]){2});

        int t  = vmTensorNew(vm, VM_F32, s);
        int t1 = vmTensorNew(vm, VM_F32, s1);
        int t2 = vmTensorNew(vm, VM_F32, s2);
        int t3 = vmTensorNew(vm, VM_F32, s3);

        COPY_DATA(vm, t, 6,
                  ((f32_t[]){2.34f, 5.67f, 2.00f, 3.00f, 4.00f, 5.00f}));

        struct vm_opopt opts[] = {
            {.mode = 0 | VM_OPT_MODE_I_BIT, .i = 0 },
            {.mode = 0 | VM_OPT_MODE_I_BIT, .i = 1 },
            {.mode = 0 | VM_OPT_MODE_I_BIT, .i = -1},
        };

        const char *expected_strs[] = {
            "<1> f32 [22.010]",
            "<3> f32 [5.340, 9.670, 7.000]",
            "<2> f32 [10.010, 12.000]",
        };

        const int tds[] = {t1, t2, t3};

        for (int i = 0; i < (int)(sizeof(opts) / sizeof(struct vm_opopt));
             i++) {
                NE(vmExec(vm, VM_OP_REDUCE, opts + i, tds[i], t, -1));
                CHECK_TENSOR(vm, tds[i], expected_strs[i],
                             "failed at %d-th Op\n", i);
        }
        vmFree(vm);
        return NULL;
}

static char *
test_rng()
{
        struct vm *vm   = vmNew();
        struct vm_sp *s = vmSpNew(vm, 2, (int[]){2, 3});

        int t = vmTensorNew(vm, VM_F32, s);

        struct rng64 *rng = rng64New(456);

        struct vm_opopt opt = {.mode = 0 | VM_OPT_MODE_R_BIT, .r = *rng};

        const char *expected_str =
            "<2, 3> f32 [1.348, -1.670, 1.431, 0.632, 0.288, 1.630]";

        NE(vmExec(vm, VM_OP_RNG, &opt, t, -1, -1));
        CHECK_TENSOR(vm, t, expected_str, "failed at %s\n", "rng");

        rng64Free(rng);
        vmFree(vm);
        return NULL;
}

static char *
test_fill()
{
        struct vm *vm   = vmNew();
        struct vm_sp *s = vmSpNew(vm, 2, (int[]){2, 2});

        int t = vmTensorNew(vm, VM_F32, s);

        struct vm_opopt opt1    = {.mode = 0 | VM_OPT_MODE_F_BIT, .f = 0.0f};
        struct vm_opopt opt2    = {.mode = 0 | VM_OPT_MODE_F_BIT, .f = 1.0f};
        struct vm_opopt *opts[] = {NULL, &opt1, &opt2};

        const char *expected_strs[] = {
            "<2, 2> f32 [0.000, 0.000, 0.000, 0.000]",
            "<2, 2> f32 [0.000, 0.000, 0.000, 0.000]",
            "<2, 2> f32 [1.000, 1.000, 1.000, 1.000]",
        };

        for (int i = 0; i < (int)(sizeof(opts) / sizeof(struct vm_opopt *));
             i++) {
                NE(vmExec(vm, VM_OP_FILL, opts[i], t, -1, -1));
                CHECK_TENSOR(vm, t, expected_strs[i], "failed at %d-th Op\n",
                             i);
        }
        vmFree(vm);
        return NULL;
}

static char *
test_ls_scel()
{
        struct vm *vm    = vmNew();
        struct vm_sp *s  = vmSpNew(vm, 2, (int[]){2, 3});
        struct vm_sp *s1 = vmSpNew(vm, 1, (int[]){2});

        int t1 = vmTensorNew(vm, VM_F32, s);
        int t2 = vmTensorNew(vm, VM_F32, s);
        int td = vmTensorNew(vm, VM_F32, s1);
        int tg = vmTensorNew(vm, VM_F32, s);

        // label
        COPY_DATA(vm, t1, 6, ((f32_t[]){0.2f, 0.3f, .5f, 0.5f, 0.4f, .1f}));
        // logit
        COPY_DATA(vm, t2, 6,
                  ((f32_t[]){10.0f, 20.0f, -15.0f, 1.0f, 2.0f, -5.0f}));

        const char *expected_loss = "<2> f32 [19.500, 1.514]";
        const char *expected_grad =
            "<2, 3> f32 [-0.200, 0.700, -0.500, -0.231, 0.331, -0.099]";

        // case 1
        NE(vmExec(vm, VM_OP_LS_SCEL, NULL, td, t1, t2));
        CHECK_TENSOR(vm, td, expected_loss, "failed at %s", "loss");

        // case 2
        struct vm_opopt opt = {.mode = 0 | VM_OPT_MODE_I_BIT, .i = tg};
        NE(vmExec(vm, VM_OP_LS_SCEL, &opt, td, t1, t2));
        CHECK_TENSOR(vm, td, expected_loss, "failed at %s", "loss");
        CHECK_TENSOR(vm, tg, expected_grad, "failed at %s", "grad");

        vmFree(vm);
        return NULL;
}

DECLARE_TEST_SUITE(mlvm_kernel)
{
        RUN_TEST(test_ele_ops);
        RUN_TEST(test_ele_ops_unequal_size);
        RUN_TEST(test_ele_ops_scalar_operand);
        RUN_TEST(test_ele_ops_f_bit);
        RUN_TEST(test_matmul);
        RUN_TEST(test_argmax);
        RUN_TEST(test_isqrt);
        RUN_TEST(test_reduce);
        RUN_TEST(test_rng);
        RUN_TEST(test_fill);
        RUN_TEST(test_ls_scel);
        return NULL;
}

// -----------------------------------------------------------------------------
// helper impl
// -----------------------------------------------------------------------------
error_t
copy_data(struct vm *vm, int td, size_t size, f32_t *src)
{
        f32_t *data;
        error_t err = vmTensorData(vm, td, (void **)&data);
        if (err) return err;
        memcpy(data, src, size * sizeof(f32_t));
        return OK;
}

error_t
expect_dump(struct vm *vm, int td, const char *expected)
{
        sds_t s = sdsEmpty();
        vmTensorToSds(&s, vm, td);
        if (0 != strcmp(s, expected)) {
                error_t err =
                    errNew("info:\nexpected: %s\ngot     : %s\n", expected, s);
                sdsFree(s);
                return err;
        }
        sdsFree(s);
        return OK;
}
