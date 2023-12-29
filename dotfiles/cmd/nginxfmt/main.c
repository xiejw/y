// This is a very simple nginx conf formatter for my own conf files.
//
// It does super simple job to re-indent all lines, collapse empty lines into
// one of none.
//
// Usage:
//   nginxfmt < input_file > output_file
//
#include <stdio.h>

#define INLINE_C_CODE

#include "c/error.h"
#include "c/file_reader.h"

#define MAX_INDENT_SIZE 256

// Zero initial state
struct state {
        int  empty_lines;
        char indent[MAX_INDENT_SIZE];
};

// Process one more line and advance internal state `state`.
error_t advanceOneLine( struct state *state, char *line );

int
main( void )
{
        int          err = 0;
        fr_handle_t *hl  = NULL;
        char         line[MAX_STR_LINE_LEN + 1];

        struct state state = { 0, { 0 } };

        if ( OK != frDOpen( &hl, 0 ) ) {
                fprintf( stderr, "failed to open stdin. aborted." );
                goto cleanup;
        }

        while ( 1 ) {
                int s = frNextLine( hl, line );
                if ( s < 0 ) {
                        fprintf( stderr, "unknown error. aborted." );
                        goto cleanup;
                }
                if ( s == 0 ) break;

                advanceOneLine( &state, line );
        }

cleanup:
        if ( hl != NULL ) frClose( hl );
        return err;
}

error_t
advanceOneLine( struct state *state, char *line )
{
        printf( "line: `%s`\n", line );
        return OK;
}
