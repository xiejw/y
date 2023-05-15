#ifndef EXEC_H_
#define EXEC_H_

#include <fcntl.h>      // open
#include <sys/types.h>  // pid_t
#include <unistd.h>

#include "c/error.h"

// ----------------------------------------------------------------------------
// prototype
// ----------------------------------------------------------------------------

int execCmd(char **cmd);

// ----------------------------------------------------------------------------
// implementation
// ----------------------------------------------------------------------------

#ifdef INLINE_C_CODE

// must be a NULL-terminated string list.
int
execCmd(char **cmd)
{
        int mypipe[2];
        if (pipe(mypipe)) return EPIPEFAIL;

        pid_t pid = fork();
        if (pid == (pid_t)0) {
                // This is the child process. Close the reading end.
                close(mypipe[0]);
                dup2(mypipe[1], STDOUT_FILENO);
                dup2(open("/dev/null", O_RDWR),
                     STDERR_FILENO);  // Slience stderr.

                execvp(cmd[0], cmd);
                return 0;  // never reached.
        } else if (pid < (pid_t)0) {
                return EFORKFAIL;
        } else {
                // This is the parent process.  Close writing end first.
                close(mypipe[1]);
                return mypipe[0];
        }
}

#endif  // INLINE_C_CODE

#endif  // EXEC_H_
