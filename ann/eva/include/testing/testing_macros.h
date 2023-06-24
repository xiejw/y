#ifndef EVA_TESTING_MACROS_H_
#define EVA_TESTING_MACROS_H_

#include <testing/testing.h>

#include <string.h>

// eva
#include <adt/sds.h>
#include <algos/regex.h>

// -----------------------------------------------------------------------------
// companion of testing macros. opt-in to use.
//
// The benefit to use these macros is printing more debugging messages for
// detailed context.
// -----------------------------------------------------------------------------

// print error_t stack
#define ASSERT_NO_ERR(e)                                                 \
        do {                                                             \
                if ((e) != OK) {                                         \
                        errDump("unexpected error during testing:");     \
                        errFree();                                       \
                        /* reuse macro to print file and line number. */ \
                        ASSERT_TRUE("unexpected error", 0);              \
                }                                                        \
        } while (0)

// print str1 and str2 in dump.
#define ASSERT_STR_EQ(msg, str1, str2)                                        \
        do {                                                                  \
                _Pragma("clang diagnostic push")                              \
                    _Pragma("clang diagnostic ignored \"-Wshadow\"")(void) 0; \
                const char *t1 = (str1);                                      \
                const char *t2 = (str2);                                      \
                if (0 != strcmp(t1, t2)) {                                    \
                        errNew("got:\n...%s...\n(...vs...)\n...%s...\n", t1,  \
                               t2);                                           \
                        errDump("strs expected to match but not.");           \
                        errFree();                                            \
                        /* reuse macro to print file and line number. */      \
                        ASSERT_TRUE(msg, 0);                                  \
                }                                                             \
                _Pragma("clang diagnostic pop")                               \
        } while (0)

// print the str in dump if not empty.
#define ASSERT_STR_EMPTY(msg, str1)                                            \
        do {                                                                   \
                _Pragma("clang diagnostic push")                               \
                    _Pragma("clang diagnostic ignored \"-Wshadow\"")(void) 0;  \
                const char *t1 = (str1);                                       \
                if (0 != strcmp(t1, "")) {                                     \
                        errDump("expected empty string, but got:\n...%s...\n", \
                                t1);                                           \
                        errFree();                                             \
                        /* reuse macro to print file and line number. */       \
                        ASSERT_TRUE(msg, 0);                                   \
                }                                                              \
                _Pragma("clang diagnostic pop")                                \
        } while (0)

// print long1 and long2 in dump.
#define ASSERT_LONG_EQ(msg, long1, long2)                                     \
        do {                                                                  \
                _Pragma("clang diagnostic push")                              \
                    _Pragma("clang diagnostic ignored \"-Wshadow\"")(void) 0; \
                const long t1 = (long)(long1);                                \
                const long t2 = (long)(long2);                                \
                if (t1 != t2) {                                               \
                        errNew("got: %ld vs %ld\n", t1, t2);                  \
                        errDump("numbers expected to match but not.");        \
                        errFree();                                            \
                        /* reuse macro to print file and line number. */      \
                        ASSERT_TRUE(msg, 0);                                  \
                }                                                             \
                _Pragma("clang diagnostic pop")                               \
        } while (0)

// print error message if regex not match.
#define ASSERT_ERR(msg, regex)                                                \
        do {                                                                  \
                _Pragma("clang diagnostic push")                              \
                    _Pragma("clang diagnostic ignored \"-Wshadow\"")(void) 0; \
                if (OK == errNumOrOK()) {                                     \
                        errNew("did not see error!!!");                       \
                        errDump("expected to see err: %s", (regex));          \
                        errFree();                                            \
                        ASSERT_TRUE(msg, 0);                                  \
                }                                                             \
                sds_t s = sdsEmptyWithCap(128);                               \
                errDumpSds(&s, "got error:");                                 \
                int r = regex_match((regex), s);                              \
                if (r == 1) {                                                 \
                        /* pass */                                            \
                        errFree();                                            \
                        sdsFree(s);                                           \
                } else {                                                      \
                        errDump("expected regex: %s\ngot error:", (regex));   \
                        errFree();                                            \
                        sdsFree(s);                                           \
                        /* reuse macro to print file and line number. */      \
                        ASSERT_TRUE(msg, 0);                                  \
                }                                                             \
                _Pragma("clang diagnostic pop")                               \
        } while (0)

// print which element is different and supports tol for float comparision..
#define ASSERT_ARRAY_CLOSE(msg, expected, got, size, tol)                 \
        _ASSERT_ARRAY_CLOSE_IMPL(msg, expected, got, size, tol, __FILE__, \
                                 __LINE__)

#define _ASSERT_ARRAY_CLOSE_IMPL(msg, expected, got, size, tol, file, lineno) \
        do {                                                                  \
                int i;                                                        \
                for (i = 0; i < size; i++) {                                  \
                        if ((expected[i] - got[i]) >= tol ||                  \
                            (expected[i] - got[i]) <= -tol) {                 \
                                _ASSERT_PRINT_LOC(file, lineno);              \
                                printf(                                       \
                                    "\n-> at element %d\n-> expected %f\n-> " \
                                    "got %f\n",                               \
                                    i, (double)expected[i], (double)got[i]);  \
                                return msg;                                   \
                        }                                                     \
                }                                                             \
        } while (0)

#endif  // EVA_TESTING_MACROS_H_
