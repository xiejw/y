#include "testing/testing.h"
#include "testing/testing_macros.h"

#include "base/fpath.h"

#include <string.h>

static char *
test_join_empty( void )
{
        sds_t e = sdsNew( "" );
        sds_t r = fpJoinSds( e, e );

        ASSERT_TRUE( "empty", sdsCmp( r, e ) == 0 );

        sdsFree( r );
        sdsFree( e );
        return NULL;
}

static char *
test_join_e1_empty( void )
{
        sds_t e1 = sdsNew( "" );
        sds_t e2 = sdsNew( "a/b" );
        sds_t r  = fpJoinSds( e1, e2 );

        ASSERT_TRUE( "same as e2", sdsCmp( r, e2 ) == 0 );

        sdsFree( r );
        sdsFree( e1 );
        sdsFree( e2 );
        return NULL;
}

static char *
test_join_e1_empty_e2_with_suffix_slash( void )
{
        sds_t e1 = sdsNew( "" );
        sds_t e2 = sdsNew( "a/b/" );
        sds_t r  = fpJoinSds( e1, e2 );

        ASSERT_TRUE( "no suffix slash", strcmp( r, "a/b" ) == 0 );
        ASSERT_TRUE( "size", sdsLen( r ) == 3 );

        sdsFree( r );
        sdsFree( e1 );
        sdsFree( e2 );
        return NULL;
}

static char *
test_join_e2_empty( void )
{
        sds_t e1 = sdsNew( "a/b" );
        sds_t e2 = sdsNew( "" );
        sds_t r  = fpJoinSds( e1, e2 );

        ASSERT_TRUE( "same as e1", sdsCmp( r, e1 ) == 0 );

        sdsFree( r );
        sdsFree( e1 );
        sdsFree( e2 );
        return NULL;
}

static char *
test_join_e2_empty_e1_with_suffix_slash( void )
{
        sds_t e1 = sdsNew( "a/b/" );
        sds_t e2 = sdsNew( "" );
        sds_t r  = fpJoinSds( e1, e2 );

        ASSERT_TRUE( "no suffix slash", strcmp( r, "a/b" ) == 0 );
        ASSERT_TRUE( "size", sdsLen( r ) == 3 );

        sdsFree( r );
        sdsFree( e1 );
        sdsFree( e2 );
        return NULL;
}

static char *
test_join_common_case( void )
{
        sds_t e1       = sdsNew( "a" );
        sds_t e2       = sdsNew( "b" );
        sds_t expected = sdsNew( "a/b" );
        sds_t r        = fpJoinSds( e1, e2 );

        ASSERT_TRUE( "expected", sdsCmp( expected, r ) == 0 );

        sdsFree( r );
        sdsFree( expected );
        sdsFree( e1 );
        sdsFree( e2 );
        return NULL;
}

static char *
test_join_e1_slash( void )
{
        sds_t e1       = sdsNew( "a/" );
        sds_t e2       = sdsNew( "b" );
        sds_t expected = sdsNew( "a/b" );
        sds_t r        = fpJoinSds( e1, e2 );

        ASSERT_TRUE( "expected", sdsCmp( expected, r ) == 0 );

        sdsFree( r );
        sdsFree( expected );
        sdsFree( e1 );
        sdsFree( e2 );
        return NULL;
}

static char *
test_join_e2_slash( void )
{
        sds_t e1       = sdsNew( "a" );
        sds_t e2       = sdsNew( "b/" );
        sds_t expected = sdsNew( "a/b" );
        sds_t r        = fpJoinSds( e1, e2 );

        ASSERT_TRUE( "expected", sdsCmp( expected, r ) == 0 );

        sdsFree( r );
        sdsFree( expected );
        sdsFree( e1 );
        sdsFree( e2 );
        return NULL;
}

static char *
test_join_both_slashes( void )
{
        sds_t e1       = sdsNew( "a/" );
        sds_t e2       = sdsNew( "b/" );
        sds_t expected = sdsNew( "a/b" );
        sds_t r        = fpJoinSds( e1, e2 );

        ASSERT_TRUE( "expected", sdsCmp( expected, r ) == 0 );

        sdsFree( r );
        sdsFree( expected );
        sdsFree( e1 );
        sdsFree( e2 );
        return NULL;
}

static char *
test_split_ab( void )
{
        sds_t e1 = sdsNew( "" );
        sds_t e2 = sdsNew( "" );
        fpSplit( "a/b", &e1, &e2 );

        ASSERT_STR_EQ( "d", "a/", e1 );
        ASSERT_STR_EQ( "f", "b", e2 );

        sdsFree( e1 );
        sdsFree( e2 );
        return NULL;
}

static char *
test_split_a( void )
{
        sds_t e1 = sdsNew( "" );
        sds_t e2 = sdsNew( "" );
        fpSplit( "a/", &e1, &e2 );

        ASSERT_STR_EQ( "d", "a/", e1 );
        ASSERT_STR_EQ( "f", "", e2 );

        sdsFree( e1 );
        sdsFree( e2 );
        return NULL;
}

static char *
test_split_b( void )
{
        sds_t e1 = sdsNew( "" );
        sds_t e2 = sdsNew( "" );
        fpSplit( "b", &e1, &e2 );

        ASSERT_STR_EQ( "d", "", e1 );
        ASSERT_STR_EQ( "f", "b", e2 );

        sdsFree( e1 );
        sdsFree( e2 );
        return NULL;
}

static char *
test_split_empty( void )
{
        sds_t e1 = sdsNew( "" );
        sds_t e2 = sdsNew( "" );
        fpSplit( "", &e1, &e2 );

        ASSERT_STR_EQ( "d", "", e1 );
        ASSERT_STR_EQ( "f", "", e2 );

        sdsFree( e1 );
        sdsFree( e2 );
        return NULL;
}

static char *
test_exist( void )
{
        char *filename      = "include/base/fpath.h";
        char *filename_fake = "include/base/fpath.h_fake";
        int   existed       = 0;
        ASSERT_NO_ERR( fpExist( filename, &existed, NULL ) );
        ASSERT_TRUE( "exist", existed );

        ASSERT_NO_ERR( fpExist( filename_fake, &existed, NULL ) );
        ASSERT_TRUE( "not exist", !existed );

        // Test again
        ASSERT_NO_ERR( fpExist( filename, &existed, NULL ) );
        ASSERT_TRUE( "exist", existed );
        return NULL;
}

DECLARE_TEST_SUITE( base_fpath )
{
        RUN_TEST( test_join_empty );
        RUN_TEST( test_join_e1_empty );
        RUN_TEST( test_join_e1_empty_e2_with_suffix_slash );
        RUN_TEST( test_join_e2_empty );
        RUN_TEST( test_join_e2_empty_e1_with_suffix_slash );
        RUN_TEST( test_join_common_case );
        RUN_TEST( test_join_e1_slash );
        RUN_TEST( test_join_e2_slash );
        RUN_TEST( test_join_both_slashes );

        RUN_TEST( test_split_ab );
        RUN_TEST( test_split_a );
        RUN_TEST( test_split_b );
        RUN_TEST( test_split_empty );

        RUN_TEST( test_exist );
        return NULL;
}
