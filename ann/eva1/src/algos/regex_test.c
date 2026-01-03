#include "testing/testing.h"

#include "algos/regex.h"

static char *
test_head( void )
{
        int r = regex_match( "bc", "abc" );
        ASSERT_TRUE( "match", r == 1 );

        r = regex_match( "^ab", "abc" );
        ASSERT_TRUE( "unmatch", r == 1 );

        r = regex_match( "^bc", "abc" );
        ASSERT_TRUE( "unmatch", r == 0 );
        return NULL;
}

static char *
test_tail( void )
{
        int r = regex_match( "b", "abc" );
        ASSERT_TRUE( "match", r == 1 );

        r = regex_match( "c$", "abc" );
        ASSERT_TRUE( "match", r == 1 );

        r = regex_match( "b$", "abc" );
        ASSERT_TRUE( "unmatch", r == 0 );
        return NULL;
}

static char *
test_dot( void )
{
        int r = regex_match( "b.", "abc" );
        ASSERT_TRUE( "match", r == 1 );

        r = regex_match( ".", "abc" );
        ASSERT_TRUE( "match", r == 1 );

        r = regex_match( "abc.", "abc" );
        ASSERT_TRUE( "unmatch", r == 0 );
        return NULL;
}

static char *
test_dot_star( void )
{
        int r = regex_match( "b.*c", "abc" );
        ASSERT_TRUE( "match", r == 1 );

        r = regex_match( "a.*c", "abc" );
        ASSERT_TRUE( "match", r == 1 );

        r = regex_match( "a.*d", "abc" );
        ASSERT_TRUE( "match", r == 0 );

        r = regex_match( "b.*d", "abc" );
        ASSERT_TRUE( "unmatch", r == 0 );
        return NULL;
}

static char *
test_star( void )
{
        int r = regex_match( "abcd*", "abc" );
        ASSERT_TRUE( "match", r == 1 );

        r = regex_match( "abc", "abc" );
        ASSERT_TRUE( "match", r == 1 );

        r = regex_match( "^add*b", "abc" );
        ASSERT_TRUE( "unmatch", r == 0 );
        return NULL;
}

DECLARE_TEST_SUITE( algos_regex )
{
        RUN_TEST( test_head );
        RUN_TEST( test_tail );
        RUN_TEST( test_dot );
        RUN_TEST( test_dot_star );
        RUN_TEST( test_star );
        return NULL;
}
