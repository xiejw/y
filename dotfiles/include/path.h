#ifndef PATH_H_
#define PATH_H_

#include "error.h"

// Expands a leading `~` in `original_path` and writes the result into `dst`.
// `dst` must be large enough to hold the expanded path (MAX_PATH_LEN).
//
// Uses wordexp(3); only a single expansion result is supported — returns
// ETOOMANYRESULTS if wordexp produces more than one word.
// Returns OK on success or ENOTPATH if wordexp fails.
extern error_t ExpandTildePath( char *original_path, char *dst );

#endif  // PATH_H_
