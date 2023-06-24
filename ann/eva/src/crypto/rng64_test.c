#include <testing/testing.h>

#include <crypto/rng64.h>

#include <inttypes.h>
#include <stdlib.h>

// -----------------------------------------------------------------------------
// prototype of helpers
// -----------------------------------------------------------------------------
static char *assertArrayEqual(u64_t *expected, u64_t *got, size_t size);

// -----------------------------------------------------------------------------
// unit tests
// -----------------------------------------------------------------------------
static char *
test_u64()
{
        struct rng64 *rng = rng64New(456L);
        ASSERT_TRUE("uint64", 1843008821991917904 == rng64U64(rng));
        rng64Free(rng);
        return NULL;
}

static char *
test_split()
{
        struct rng64 *r1 = rng64New(456L);
        struct rng64 *r2 = rng64Split(r1);

        const size_t size = 4;
        u64_t v1[4];
        u64_t v2[4];

        for (size_t i = 0; i < 4; i++) {
                v1[i] = rng64U64(r1);
                v2[i] = rng64U64(r2);
        }

        u64_t e1[] = {
            15828912964246028322ULL,
            5426006831109020538ULL,
            1221103108877787995ULL,
            4684767104136985954ULL,
        };
        u64_t e2[] = {
            13171695844766518471ULL,
            16372514907123123114ULL,
            5062863193646888206ULL,
            5556012337442857035ULL,
        };

        ASSERT_TRUE("v1", NULL == assertArrayEqual(e1, v1, size));
        ASSERT_TRUE("v2", NULL == assertArrayEqual(e2, v2, size));

        rng64Free(r1);
        rng64Free(r2);
        return NULL;
}

static char *
test_to_and_from()
{
        // split r and store the new rng into r
#define SPLIT_AND_DISCARD(r)                     \
        {                                        \
                struct rng64 *t = rng64Split(r); \
                rng64Free(r);                    \
                r = t;                           \
        }

        // we transform the r1 state in a quite non-trivial way.
        struct rng64 *r1 = rng64New(456L);
        SPLIT_AND_DISCARD(r1)
        SPLIT_AND_DISCARD(r1)
        rng64U64(r1);
        rng64U64(r1);
        rng64U64(r1);
        SPLIT_AND_DISCARD(r1)
        SPLIT_AND_DISCARD(r1)

#undef SPLIT_AND_DISCARD

        struct rng64 r2;
        u64_t states[2];

        rng64To(r1, states);
        rng64From(&r2, states);

        // check internal state
        u64_t s1[] = {r1->seed_, r1->gamma_, r1->next_gamma_seed_};
        u64_t s2[] = {r2.seed_, r2.gamma_, r2.next_gamma_seed_};
        ASSERT_TRUE("states", NULL == assertArrayEqual(
                                          s1, s2, sizeof(s1) / sizeof(u64_t)));

        // check generated numbers

        u64_t v1[8];
        u64_t v2[8];

        for (size_t i = 0; i < 8; i++) {
                v1[i] = rng64U64(r1);
                v2[i] = rng64U64(&r2);
        }

        ASSERT_TRUE("values", NULL == assertArrayEqual(v1, v2, 8));

        rng64Free(r1);
        return NULL;
}

// -----------------------------------------------------------------------------
// impl helper
// -----------------------------------------------------------------------------
char *
assertArrayEqual(u64_t *expected, u64_t *got, size_t size)
{
        for (size_t i = 0; i < size; i++) {
                int v = expected[i] == got[i];
                if (v != 1) {
                        fprintf(stderr, "-> unmatched element on index %zu\n",
                                i);
                        fprintf(stderr, "-> expected %" PRIu64 "\n",
                                expected[i]);
                        fprintf(stderr, "-> got %" PRIu64 "\n", got[i]);
                        fflush(stderr);
                }
                ASSERT_TRUE("ele match", v);
        }
        return NULL;
}

DECLARE_TEST_SUITE(crypto_rng64)
{
        RUN_TEST(test_u64);
        RUN_TEST(test_split);
        RUN_TEST(test_to_and_from);
        return NULL;
}
