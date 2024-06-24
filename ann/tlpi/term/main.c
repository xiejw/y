#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <term.h>

void PrintCtrlSeq( const char *Msg, const char *Seq );

int
main( )
{
    // TODO check error
    char *const term = getenv( "TERM" );
    // TODO check error
    tgetent( NULL, term );

    PrintCtrlSeq( "kl", tgetstr( "kl", NULL ) );
    PrintCtrlSeq( "key_left", key_left );
    PrintCtrlSeq( "key_right", key_right );
}

void
PrintCtrlSeq( const char *Msg, const char *Seq )
{
    printf( "==> Print control sequence for `%s`\n", Msg );
    size_t Len = strlen( Seq );
    printf( "    Len: %lu\n", Len );

    if ( Len == 0 ) return;

    const char *Ptr = Seq;
    if ( *Ptr == 27 ) {
        printf( "    Seq: ESC " );
    } else {
        printf( "    Seq: %c ", *Ptr );
    }
    Ptr++;
    while ( *Ptr ) {
        printf( "%c(%2d) ", *Ptr, *Ptr );
        Ptr++;
    }
    printf( "\n" );
}
