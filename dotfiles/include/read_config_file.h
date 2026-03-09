#ifndef READ_CONFIG_FILE_H_
#define READ_CONFIG_FILE_H_

#include "error.h"

// Reads the repo-list config file at `config_path`.
//
// Each non-empty line is treated as a local repository path. On success,
// `*repo_list` is allocated (caller must free each entry and the array),
// and `*count` is set to the number of entries read. At most `max_count`
// entries are read; exceeding this limit returns EUNSPECIFIED.
//
// If the file does not exist the function returns OK with *count == 0.
// Returns ENOTPATH, EOPENFILE, or EREADFILE on other failures.
extern error_t readRepoListFromConfig( char *config_path, char ***repo_list,
                                       int *count, int max_count );

#endif  // READ_CONFIG_FILE_H_
