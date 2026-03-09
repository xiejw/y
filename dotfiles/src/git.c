#include "include/git.h"

#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

error_t
GitReadStatus( git_status_t *status )
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
