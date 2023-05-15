#ifndef FILE_READER_H_
#define FILE_READER_H_

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "c/error.h"

// -----------------------------------------------------------------------------
// prototypes
// -----------------------------------------------------------------------------

// opaque handle for file line reading.
typedef struct {
        /* Internal fields. */
        int            fd_;
        unsigned char *buffer_;
        char           end_of_file_;

        ssize_t pos_;
        ssize_t allocated_;
} fr_handle_t;

error_t frOpen(fr_handle_t **handle, char *path);
error_t frDOpen(fr_handle_t **handle, int fd);
void    frClose(fr_handle_t *handle);
int     frNextLine(fr_handle_t *handle, char *dst);

// -----------------------------------------------------------------------------
// implementation
// -----------------------------------------------------------------------------

#ifdef INLINE_C_CODE

static unsigned int max_buffer_size_ = 4096 * 16;
static unsigned int max_line_len_    = MAX_STR_LINE_LEN;

static error_t _frLoadNextBuf(fr_handle_t *handle);

error_t
frOpen(fr_handle_t **handle, char *path)
{
        int fd = open(path, O_RDONLY);
        if (fd == -1) return EOPENFILE;
        return frDOpen(handle, fd);
}

error_t
frDOpen(fr_handle_t **handle, int fd)
{
        // Allocate resources.
        *handle            = malloc(sizeof(fr_handle_t));
        (*handle)->buffer_ = malloc(max_buffer_size_ * sizeof(unsigned char));

        (*handle)->fd_          = fd;
        (*handle)->end_of_file_ = 0;
        (*handle)->pos_         = 0;
        (*handle)->allocated_   = 0;
        return OK;
}

void
frClose(fr_handle_t *handle)
{
        close(handle->fd_);
        free(handle->buffer_);
        free(handle);
}

/* Reads one more line.
 *
 * Returns the size of the characters read, upon succeed. Especial case, 0 for
 * EOL, and -1 for error.
 *
 * Result will be stored to `dst`, which is owned by call site.  */
int
frNextLine(fr_handle_t *handle, char *dst)
{
        // Special case, returns immediately if EOF.
        if (handle->end_of_file_) return 0;

        // Allocates the buffer for current line. It is 1 larger than the
        // max_line_len_.
        unsigned char line[max_line_len_ + 1];
        int           current_len = 0;
        int           i;

        // The loop ends in any of the following conditions.
        // 1. EOF.
        // 2. find an EOL.
        // 3. reach maximul line length limit, i.e., error.
        for (;;) {
                // If the current buf is fully used, read next buffer.
                if (handle->pos_ >= handle->allocated_) {
                        error_t err = _frLoadNextBuf(handle);
                        if (OK != err) return err;
                }

                // Checks whether it is EOF.
                if (handle->allocated_ == 0) {
                        handle->end_of_file_ = 1;
                        if (current_len == 0) {
                                // Nothing left after the last EOL.
                                return 0;
                        } else {
                                line[current_len] = '\0';
                                strcpy(dst, (const char *)line);
                                return current_len;
                        }
                }

                // Tries to find end-of-line, i.e., `\n`.
                for (i = handle->pos_; i < handle->allocated_; i++) {
                        if (handle->buffer_[i] == '\n') {
                                int length = i - handle->pos_;

                                if (current_len + length >= max_line_len_)
                                        return ELINELEN;

                                // Move the data from line to handle buffer.
                                memcpy(line + current_len,
                                       handle->buffer_ + handle->pos_, length);
                                current_len += length;
                                handle->pos_ = i + 1;

                                line[current_len] = '\0';
                                strcpy(dst, (const char *)line);
                                return current_len;
                        }
                }

                // We reach the end of the buffer.
                int length = handle->allocated_ - handle->pos_;
                if (current_len + length >= max_line_len_) return ELINELEN;

                memcpy(line + current_len, handle->buffer_ + handle->pos_,
                       length);
                current_len += length;
                handle->pos_ = handle->allocated_;

                // Ready for next iteration.
        }
}

// -----------------------------------------------------------------------------
// impl of private methods
// -----------------------------------------------------------------------------

static error_t
_frLoadNextBuf(fr_handle_t *handle)
{
        handle->pos_ = 0L;
        handle->allocated_ =
            read(handle->fd_, handle->buffer_, max_buffer_size_);

        if (handle->allocated_ == -1) return EREADFILE;

        return OK;
}

#endif  // INLINE_C_CODE

#endif  // FILE_READER_H_
