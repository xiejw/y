#ifndef EVA_FPATH_H
#define EVA_FPATH_H

#include <sys/stat.h>  // mode_t

#include <adt/sds.h>

// -----------------------------------------------------------------------------
// fpJoinSds and fpJoin join two paths into one path
//
// Caller owns the return value.
//
// Examples:
//
//     "a",   "b"   -> "a/b"
//     "a/",  "b"   -> "a/b"
//     "a",   "b/"  -> "a/b"
//     "a/",  "b/"  -> "a/b"
//     "a/b", ""    -> "a/b"
//     "",    "a/b" -> "a/b"
//
sds_t fpJoinSds( sds_t e1, sds_t e2 );
sds_t fpJoin( const char *e1, size_t s1, const char *e2, size_t s2 );

// -----------------------------------------------------------------------------
// fpSplit
//
// Caller owns the _mut_ values
//
// Examples:
//
//     "a/b"    -> "a/", "b"
//     "a/"     -> "a/", ""
//     "b"      -> "", "b"
//     ""       -> "", ""
//
void fpSplit( const char *path, _null_ _mut_ sds_t *pdir,
              _null_ _mut_ sds_t *pfile );

// -----------------------------------------------------------------------------
// fpExist checks whether path (dir or file) exists.
//
// Upon return, pexisted is set and psize if provided is also set.
error_t fpExist( const char *path, _out_ int *pexisted,
                 _null_ _out_ size_t *psize );

// -----------------------------------------------------------------------------
// Permission macros
#define FP_DEFAULT_PERMS      ( S_IRUSR | S_IWUSR | S_IXUSR )
#define FP_DEFAULT_PERMS_FILE ( S_IRUSR | S_IWUSR )

// -----------------------------------------------------------------------------
// fpMkdirAll creates dir at path and all its parent folders if neeed.
//
// If path is already a directory, return OK and do nothing. If not, all dirs
// are created with permission mode.
error_t fpMkdirAll( const char *path, mode_t mode );

// -----------------------------------------------------------------------------
// fpCopy copies the file from src_path to dst_path.
//
// The new file is created with permission mode.
error_t fpCopy( const char *dst_path, const char *src_path, mode_t mode );

#endif  // EVA_FPATH_H
