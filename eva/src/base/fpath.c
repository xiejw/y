#include "base/fpath.h"

#include <errno.h>
#include <fcntl.h>     // open
#include <string.h>    // memcpy
#include <sys/stat.h>  // stat
#include <unistd.h>    // close

// -----------------------------------------------------------------------------
// helper prototypes
// -----------------------------------------------------------------------------
#define FP_FILE_BUF_SIZE (16 * 4096)

// pathClean tries to clean up the path e by triming the trailing '/'.
//
// Path e cannot be empty.
static inline void pathClean(sds_t e);

// -----------------------------------------------------------------------------
// public api impl
// -----------------------------------------------------------------------------
sds_t
fpJoinSds(sds_t e1, sds_t e2)
{
        size_t s1 = sdsLen(e1);
        size_t s2 = sdsLen(e2);
        return fpJoin(e1, s1, e2, s2);
}

sds_t
fpJoin(const char *e1, size_t s1, const char *e2, size_t s2)
{
        size_t s = s1 + s2;
        if (s == 0) return sdsEmpty();

        // after this point, the content is not empty.
        sds_t r = sdsEmptyWithCap(s + 1);  // 1 is for "/"

        if (s1 == 0) {
                sdsCpyLen(&r, e2, s2);
                goto out;
        }

        if (s2 == 0) {
                sdsCpyLen(&r, e1, s1);
                goto out;
        }

        // copy the e1
        size_t end = s1;
        memcpy(r, e1, s1);

        // likely, avoid dup '/'
        if (r[end - 1] != '/') {
                r[end++] = '/';
        }

        // copy the e2
        memcpy(r + end, e2, s2);
        end += s2;
        assert(end <= sdsCap(r));

        // sds bookmark
        r[end] = 0;
        sdsSetLen(r, end);

out:
        pathClean(r);

        return r;
}

error_t
fpExist(const char *path, int *pexisted, _out_ size_t *psize)
{
        struct stat stat_buf;
        int r = stat(path, &stat_buf);

        if (r != OK && errno == ENOENT) {
                *pexisted = 0;
                return OK;
        }

        if (r != OK) {
                return errNew("stat error: %s", strerror(errno));
        }

        *pexisted = 1;

        if (psize != NULL) *psize = (size_t)stat_buf.st_size;
        return OK;
}

error_t
fpMkdirAll(const char *path, mode_t mode)
{
        struct stat stat_buf;
        int r = stat(path, &stat_buf);

        // fast path
        if (r == OK) {
                mode_t m = stat_buf.st_mode;
                if (S_ISDIR(m)) {
                        return OK;
                }

                if (S_ISREG(m)) {
                        return errNew(
                            "path is expected to be dir but got file: %s",
                            path);
                }

                return errNew("unsupported stat.st_mode: %d", (int)m);
        }

        if (r != OK && errno != ENOENT) {
                return errNew("mkdir error: %s", strerror(errno));
        }

        // slow path. recursively create parent folder and then mkdir
        size_t end = strlen(path);
        while (end > 0 && path[end - 1] == '/') {
                end--;  // Skip trailing path separator.
        }

        size_t j = end;
        while (j > 0 && path[j - 1] != '/') {
                j--;  // Scan backward over element.
        }

        if (j > 0) {
                char *parent_p = malloc(j + 1);
                memcpy(parent_p, path, j);
                parent_p[j] = 0;
                error_t err = fpMkdirAll(parent_p, mode);
                if (err) {
                        err = errEmitNote("failed to mkdir parent dir: %s",
                                          parent_p);
                        free(parent_p);
                        return err;
                }

                free(parent_p);
        }

        // at this point, the parent dir exists.
        r = mkdir(path, mode);
        if (r != OK) {
                return errNew("mkdir error: %s", strerror(errno));
        }

        return OK;
}

error_t
fpCopy(const char *dst_path, const char *src_path, mode_t mode)
{
        error_t err = OK;
        int src = -1, dst = -1;
        unsigned char buffer[FP_FILE_BUF_SIZE];
        ssize_t size;

        // Open src file.
        src = open(src_path, O_RDONLY);
        if (src == -1) {
                err = errNew("open error: %s", strerror(errno));
                err = errEmitNote("failed to open src file: %v", src_path);
                goto exit;
        }

        // Open dst file.
        dst = open(dst_path, O_WRONLY | O_CREAT, mode);
        if (dst == -1) {
                err = errNew("open error: %s", strerror(errno));
                err = errEmitNote("failed to open dst file: %v", dst_path);
                goto exit;
        }

        // Copy bytes.
        while ((size = read(src, buffer, FP_FILE_BUF_SIZE)) > 0) {
                ssize_t real_size = write(dst, buffer, (size_t)size);

                if (real_size == -1) {
                        err = errNew("faied to write file %s: %s", dst_path,
                                     strerror(errno));
                        goto exit;
                } else if (real_size != size) {
                        err = errNew("faied to write all bytes into file %s",
                                     dst_path);
                        goto exit;
                }
        }

        // File check.
        if (size == -1) {
                err = errNew("faied to read file %s: %s", src_path,
                             strerror(errno));
                goto exit;
        }

exit:
        if (src != -1) {
                close(src);
        }
        if (dst != -1) {
                close(dst);
        }
        return err;
}

void
fpSplit(const char *path, _null_ _mut_ sds_t *pdir, _null_ _mut_ sds_t *pfile)
{
        size_t end = strlen(path);
        size_t j   = end;
        while (j > 0 && path[j - 1] != '/') {
                j--;  // Scan backward over element.
        }

        if (j > 0 && pdir != NULL) {
                sdsCatPrintf(pdir, "%.*s", j, path);
        }

        if (pfile != NULL) {
                sdsCatPrintf(pfile, "%.*s", end - j, path + j);
        }
}

// -----------------------------------------------------------------------------
// helper impl
// -----------------------------------------------------------------------------
void
pathClean(sds_t e)
{
        const size_t s = sdsLen(e);
        assert(s > 0);
        const size_t end = s - 1;

        // likely
        if (e[end] != '/') return;

        e[end] = 0;
        sdsSetLen(e, end);
}
