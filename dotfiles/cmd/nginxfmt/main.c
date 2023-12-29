// This is a very simple nginx conf formatter for my own conf files.
//
// It does super simple job to re-indent all lines, collapse empty lines into
// one of none.
//
// Usage:
//   nginxfmt < input_file > output_file
//
#include <assert.h>
#include <stdio.h>

#define INLINE_C_CODE

#include "c/error.h"
#include "c/file_reader.h"

#define MAX_INDENT_SIZE 256

#define EMIT_LINE( line )     printf( "line: `%s`\n", ( line ) )
#define EMIT_LINE_START( )    printf( "line: `" )
#define EMIT_LINE_RAW( line ) printf( "%s", line )
#define EMIT_LINE_END( )      printf( "`\n" )

// Zero initial state
struct state {
        int  started;
        int  empty_line_count;
        char indent[MAX_INDENT_SIZE];
};

// Process one more line and advance internal state `state`.
error_t advanceOneLine( struct state *state, char *line );
void    trimLine( char *line );
int     isEmptyLine( char *line );
int     isBlockOpen( char *line );
int     isBlockClosed( char *line );
void    shiftLeftIndent( struct state *state );
void    shiftRightIndent( struct state *state );

int
main( void )
{
        int          err = 0;
        fr_handle_t *hl  = NULL;
        char         line[MAX_STR_LINE_LEN + 1];

        struct state state = { 0, 0, { 0 } };

        if ( OK != frDOpen( &hl, 0 ) ) {
                fprintf( stderr, "failed to open stdin. aborted." );
                goto cleanup;
        }

        while ( 1 ) {
                int len_or_err = frNextLine( hl, line );
                if ( len_or_err == EEOF ) break;

                if ( len_or_err < 0 ) {
                        fprintf( stderr, "unknown error. aborted." );
                        goto cleanup;
                }

                advanceOneLine( &state, line );
        }

cleanup:
        if ( hl != NULL ) frClose( hl );
        return err;
}

error_t
advanceOneLine( struct state *state, char *line )
{
        trimLine( line );

        int is_empty = isEmptyLine( line );

        // Skip all empty head lines.
        if ( is_empty && !state->started ) {
                return OK;
        }

        state->started = 1;

        // If empty line, just record and skip it
        if ( is_empty ) {
                state->empty_line_count++;
                return OK;
        }

        if ( isBlockClosed( line ) ) {
                shiftLeftIndent( state );
                // Eat all emptyline lines
                state->empty_line_count = 0;
        } else if ( state->empty_line_count > 0 ) {
                // Collapse all empty lines.
                EMIT_LINE( "" );
                state->empty_line_count = 0;  // reset
        }

        EMIT_LINE_START( );

        EMIT_LINE_RAW( state->indent );
        EMIT_LINE_RAW( line );
        EMIT_LINE_END( );

        if ( isBlockOpen( line ) ) {
                shiftRightIndent( state );
        }
        return OK;
}

void
trimLine( char *line )
{
        int  start_idx = 0;
        char c         = line[0];

        // Identify the first non emtpy char.
        while ( c == ' ' || c == '\t' ) {
                c = line[++start_idx];
        }

        // Trim left
        if ( start_idx != 0 ) {
                int idx = 0;
                while ( 1 ) {
                        c           = line[start_idx + idx];
                        line[idx++] = c;
                        if ( c == '\0' ) break;
                }
        }

        // TODO trim right
}

int
isEmptyLine( char *line )
{
        return line[0] == '\0';
}

int
isBlockOpen( char *line )
{
        int idx = 0;
        while ( 1 ) {
                char c = line[idx++];
                if ( c == '{' ) return 1;
                if ( c == '\0' ) return 0;
        }
}

int
isBlockClosed( char *line )
{
        int idx = 0;
        while ( 1 ) {
                char c = line[idx++];
                if ( c == '}' ) return 1;
                if ( c == '\0' ) return 0;
        }
}

void
shiftRightIndent( struct state *state )
{
        char *line = state->indent;
        int   idx  = 0;
        char  c    = line[0];

        while ( c != '\0' ) {
                c = line[++idx];
        }

        assert( idx + 4 < MAX_INDENT_SIZE );

        line[idx++] = ' ';
        line[idx++] = ' ';
        line[idx++] = ' ';
        line[idx++] = ' ';
        line[idx]   = '\0';
}

void
shiftLeftIndent( struct state *state )
{
        char *line = state->indent;
        int   idx  = 0;
        char  c    = line[0];

        while ( c != '\0' ) {
                c = line[++idx];
        }

        assert( idx - 4 >= 0 );
        line[idx - 4] = '\0';
}
