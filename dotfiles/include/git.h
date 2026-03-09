#ifndef GIT_H_
#define GIT_H_

#include "error.h"

// Status handle for a single git repository.
typedef struct {
        char *path;  // set/owned by caller.
} git_status_t;

// Reads (updates) the repository at status->path via `git pull --rebase`.
// `status->path` must be set by the caller before invoking.
//
// Returns OK on success, ENOTPATH if the path cannot be chdir'd into,
// ETERMSIG if the child process was killed by a signal, or EGITPULL on
// any other git failure.
extern error_t gitReadStatus( git_status_t *status );

#endif  // GIT_H_
