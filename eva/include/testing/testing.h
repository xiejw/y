#ifndef EVA_TESTING_H_
#define EVA_TESTING_H_

#include <math.h>   // fabs
#include <stdio.h>  // printf

// =============================================================================
//
// -----------------------------------------------------------------------------
// design goal
// -----------------------------------------------------------------------------
//
// Provide minimal number of macros so we dont need to remember the difference.
// Just use them.
//
// -----------------------------------------------------------------------------
// design inspiration
// -----------------------------------------------------------------------------
//
// This unit testing framework is inspired by MinUnit.
//
//     http://www.jera.com/techinfo/jtns/jtn002.html
//
// -----------------------------------------------------------------------------
// core idea
// -----------------------------------------------------------------------------
//
// The test method should have type
//
//     char*
//     test_foo() {
//         return NULL;
//     }
//
// NULL means successful; any non-NULL string is the error message.
//
// -----------------------------------------------------------------------------
// run test and suite
// -----------------------------------------------------------------------------
//
// Unit test and suite are the abstraction not enforced by the core idea. Just
// for convenience.
//
//   - unit test is typically file-level visible and invoked by suite.
//   - suite is global-level (external) visible and consists of a collection
//     unit tests.
//
// There is a global `tests_run` integer counting the total tests run.
//
//   RUN_TEST(test_fn)    // Increase `tests_run`
//   RUN_SUITE(test_fn)   // Does not increase `tests_run`
//
// -----------------------------------------------------------------------------
// declare suite
// -----------------------------------------------------------------------------
//
// DECLARE_TEST_SUITE(foo) will declare the `run_foo_suite` method and mark it
// with "missing-prototypes" clang diagnoses disable.
//
// -----------------------------------------------------------------------------
// assertion
// -----------------------------------------------------------------------------
//
// Only one ASSERT_XXX is introduced as the core api so it is easy to learn.
//
//   ASSERT_TRUE(msg, condition)
//
// For other helpers, see testing_macros.h.
// =============================================================================

extern int tests_run;  // see "test_main.h"

#define RUN_TEST(test) _RUN_TEST_IMPL(test, #test)

#define _RUN_TEST_IMPL(test, name)              \
        do {                                    \
                char *msg;                      \
                printf("  Running: %s", name);  \
                msg = (test)();                 \
                tests_run++;                    \
                if (msg != NULL) {              \
                        printf("...Failed.\n"); \
                        return msg;             \
                } else {                        \
                        printf(".\n");          \
                }                               \
        } while (0)

#define RUN_SUITE(test)                      \
        do {                                 \
                char *msg = (test)();        \
                if (msg != NULL) return msg; \
        } while (0)

// -----------------------------------------------------------------------------
// declare suite
// -----------------------------------------------------------------------------

#define DECLARE_TEST_SUITE(suite_name)                                        \
        _Pragma("clang diagnostic push")                                      \
            _Pragma("clang diagnostic ignored \"-Wmissing-prototypes\"") char \
                *run_##suite_name##_suite(void)                               \
                    _Pragma("clang diagnostic pop")

// -----------------------------------------------------------------------------
// assertion
// -----------------------------------------------------------------------------

#define ASSERT_TRUE(msg, test) _ASSERT_TRUE_IMPL(msg, test, __FILE__, __LINE__)

#define _ASSERT_TRUE_IMPL(msg, test, file, lineno)       \
        do {                                             \
                if (!(test)) {                           \
                        _ASSERT_PRINT_LOC(file, lineno); \
                        return msg;                      \
                }                                        \
        } while (0)

// print the location in two lines with yellow color
#define _ASSERT_PRINT_LOC(file, lineno) \
        printf("\n\033[1;33m-> File: %s\n-> Line %d\033[0m\n", file, lineno)

#endif  // EVA_TESTING_H_
