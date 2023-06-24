#include "testing/testing.h"

#include "sched/fork_join.h"

struct test_data {
        int index;
        int v;
};

static void *
test_fn(void *arg)
{
        struct test_data *d = arg;
        d->v                = d->index;
        return NULL;
}

static char *
test_struct()
{
        struct test_data a[2] = {
            {.index = 10, .v = 0},
            {.index = 20, .v = 0}
        };

        error_t err =
            fork_join_launch(2, test_fn, (void **)a, sizeof(struct test_data),
                             FORK_JOIN_ADDR_PTR_TO_VAL);
        ASSERT_TRUE("no err", err == OK);
        ASSERT_TRUE("0, index", a[0].index == 10);
        ASSERT_TRUE("0, value", a[0].v == 10);
        ASSERT_TRUE("1, index", a[1].index == 20);
        ASSERT_TRUE("1, value", a[1].v == 20);

        return NULL;
}

static char *
test_struct_ptr()
{
        struct test_data *p1   = malloc(sizeof(*p1));
        p1->index              = 10;
        p1->v                  = 0;
        struct test_data *p2   = malloc(sizeof(*p2));
        p2->index              = 20;
        p2->v                  = 0;
        struct test_data *a[2] = {p1, p2};

        error_t err =
            fork_join_launch(2, test_fn, (void **)a, sizeof(struct test_data *),
                             FORK_JOIN_ADDR_PTR_TO_PTR);
        ASSERT_TRUE("no err", err == OK);
        ASSERT_TRUE("0, index", p1->index == 10);
        ASSERT_TRUE("0, value", p1->v == 10);
        ASSERT_TRUE("1, index", p2->index == 20);
        ASSERT_TRUE("1, value", p2->v == 20);

        free(p1);
        free(p2);
        return NULL;
}

DECLARE_TEST_SUITE(sched_fork_join)
{
        RUN_TEST(test_struct);
        RUN_TEST(test_struct_ptr);
        return NULL;
}
