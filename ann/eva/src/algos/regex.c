#include "algos/regex.h"

// -----------------------------------------------------------------------------
// helper prototypes
// -----------------------------------------------------------------------------

static int matchhere( const char *, const char * );
static int matchstar( int, const char *, const char * );

// -----------------------------------------------------------------------------
// api impl
// -----------------------------------------------------------------------------
int
regex_match( const char *re, const char *text )
{
        if ( re[0] == '^' ) {
                return matchhere( re + 1, text );
        }

        do {  // empty string is allowed
                if ( matchhere( re, text ) ) return 1;
        } while ( *text++ != '\0' );

        return 0;
}

// -----------------------------------------------------------------------------
// helper impl
// -----------------------------------------------------------------------------

// search for 're' at beginning of text
int
matchhere( const char *re, const char *text )
{
        if ( re[0] == '\0' ) return 1;
        if ( re[1] == '*' ) return matchstar( re[0], re + 2, text );
        if ( re[0] == '$' && re[1] == '\0' ) return *text == '\0';
        if ( *text != '\0' && ( re[0] == '.' || re[0] == *text ) )
                return matchhere( re + 1, text + 1 );
        return 0;
}

// search for c*re at beginning of text
int
matchstar( int c, const char *re, const char *text )
{
        do {  // meta char * matches zero or more instances
                if ( matchhere( re, text ) ) return 1;
        } while ( *text != '\0' && ( *text++ == c || c == '.' ) );
        return 0;
}
