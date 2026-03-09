#ifndef FILE_READER_H_
#define FILE_READER_H_

#include <unistd.h>

#include "constants.h"
#include "error.h"

// A simple line reader. Default maximum line size is MAX_STR_LINE_LEN.

// Opaque handle for file line reading.
typedef struct {
        /* Internal fields. */
        int            fd_;
        unsigned char *buffer_;
        char           end_of_file_;

        ssize_t pos_;
        ssize_t allocated_;
} fr_handle_t;

// Opens the file at `path` and initialises `*handle`.
// Returns OK or EOPENFILE.
error_t frOpen( fr_handle_t **handle, char *path );

// Opens the file descriptor `fd` and initialises `*handle`.
// Returns OK.
error_t frDOpen( fr_handle_t **handle, int fd );

// Closes the reader and releases all resources.
void frClose( fr_handle_t *handle );

// Reads the next line into `dst` (caller-owned, at least MAX_STR_LINE_LEN+1
// bytes). Returns the number of characters read (excluding newline) on
// success, EEOF at end of file, or another negative error code on failure.
int frNextLine( fr_handle_t *handle, char *dst );

#endif  // FILE_READER_H_
