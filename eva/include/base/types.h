#ifndef EVA_BASE_TYPES_
#define EVA_BASE_TYPES_

#include <stdint.h>

// provide minimal number of macros/types to improve apis reability.

// import error_t
#include "base/error.h"

// reconcile int types
// also see https://github.com/torvalds/linux/blob/master/include/linux/types.h
typedef uint64_t u64_t;
typedef int64_t i64_t;
typedef uint32_t u32_t;
typedef int32_t i32_t;
typedef float f32_t;
typedef double f64_t;

// func parameter annotation
#define _mut_       // the field might be mutated if new address is allocated
#define _out_       // the field will be set with the output
#define _inout_     // the field will be passed in and then be set as output
#define _moved_in_  // the ownership is moved into the method
#define _null_      // nullable

#endif              // EVA_BASE_TYPES_
