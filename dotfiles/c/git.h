#ifndef GIT_H_
#define GIT_H_

#include <stdlib.h>
#include <sys/wait.h>  // WIFSIGNALED
#include <unistd.h>

#include "c/error.h"

// -----------------------------------------------------------------------------
// prototypes
// -----------------------------------------------------------------------------

typedef struct {
        char *path;  // set/own by owner.
} git_status_t;

// The gitReadStatus reads the repository status. `status.path` must be set by
// caller.
extern error_t gitReadStatus( git_status_t *status );

// -----------------------------------------------------------------------------
// implementation
// -----------------------------------------------------------------------------

#ifdef INLINE_C_CODE

error_t
gitReadStatus( git_status_t *status )
{
        char *const path = status->path;

        if ( OK != chdir( path ) ) return ENOTPATH;

        int s = system( "git pull --rebase" );
        if ( OK != s ) {
                if ( WIFSIGNALED( s ) ) {
                        // The chid process is terminated by signals. Read man
                        // pages for system(3) and waitpid(2).
                        return ETERMSIG;
                } else {
                        return EGITPULL;
                }
        }
        return OK;
}
#endif  // INLINE_C_CODE

#endif  // GIT_H_
