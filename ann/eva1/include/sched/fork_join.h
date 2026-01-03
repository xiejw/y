#ifndef EVA_FORK_JOIN_H_
#define EVA_FORK_JOIN_H_

#include <stdlib.h>

// eva
#include <base/error.h>

#define FORK_JOIN_ADDR_PTR_TO_VAL 0
#define FORK_JOIN_ADDR_PTR_TO_PTR 1

// Launch 'parallelism' pthreads each with 'fn_to_run' as the function to call,
// and pass the argument (based on address mode; see flag).
//
// Note: The implementation is quite simple at the moment, which does not
// support nested fork join tasks.
//
// For address mode:
// - If flag == FORK_JOIN_ADDR_PTR_TO_VAL
//     arg_ptr == (args + nbytes * i) for i = 0, 1, .., parallelism-1.
//
// - If flag == FORK_JOIN_ADDR_PTR_TO_PTR
//     arg_ptr == *(args + nbytes * i) for i = 0, 1, .., parallelism-1.
//
extern error_t fork_join_launch( size_t parallelism,
                                 void *( *fn_to_run )(void *), void *args,
                                 size_t nbytes, int flag );

#endif  // EVA_FORK_JOIN_H_
