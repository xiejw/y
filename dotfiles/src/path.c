#include "include/path.h"

#include <string.h>
#include <wordexp.h>

error_t
ExpandTildePath( char *original_path, char *dst )
{
        wordexp_t exp_result;
        if ( OK != wordexp( original_path, &exp_result, /*flags=*/0 ) ) {
                return ENOTPATH;
        }

        // Only the simpliest case is supported: one output. So error out for
        // others.
        if ( exp_result.we_wordc != 1 ) {
                wordfree( &exp_result );
                return ETOOMANYRESULTS;
        }

        strcpy( dst, exp_result.we_wordv[0] );
        wordfree( &exp_result );
        return OK;
}
