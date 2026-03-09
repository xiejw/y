#ifndef EXEC_H_
#define EXEC_H_

#include <fcntl.h>      // open
#include <sys/types.h>  // pid_t
#include <unistd.h>

#include "error.h"

// Executes the command in a child process, redirecting its stdout to a pipe.
//
// @param cmd  NULL-terminated argv array (e.g. {"git", "status", NULL}).
// @return     Readable file descriptor for the child's stdout on success,
//             or EPIPEFAIL / EFORKFAIL on error.
int execCmd( char **cmd );

#endif  // EXEC_H_
