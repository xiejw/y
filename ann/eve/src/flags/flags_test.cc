#include <eve/flags/flags.h>

#include <eve/testing/testing.h>

EVE_TEST( Flags, DefaultValues )
{
    c_flags_reset_internal( );
    bool     *verbose = c_flag_bool( "verbose", "v", "verbose mode", false );
    uint64_t *batch   = c_flag_uint64( "batchsize", "bs", "batch size", 32 );
    float    *lr      = c_flag_float( "learningrate", "lr", "lr", 1.25 );
    char **explain = c_flag_string( "explain", "e", "explain", "explain more" );

    int         argc         = 1;
    const char *const_argv[] = { (const char *)"binary_name" };
    char      **argv         = (char **)const_argv;

    int rc = c_flags_parse_internal( &argc, &argv );

    EVE_TEST_EXPECT( rc == 0, "result code" );
    EVE_TEST_EXPECT( !*verbose, "verbose" );
    EVE_TEST_EXPECT( *lr == 1.25, "learning_rate" );
    EVE_TEST_EXPECT( *batch == 32, "batch" );
    EVE_TEST_EXPECT_STR_EQ( *explain, "explain more", "explain" );
    EVE_TEST_PASS( );
}

EVE_TEST( Flags, FlipLongFlags )
{
    c_flags_reset_internal( );
    bool     *verbose = c_flag_bool( "verbose", "v", "verbose mode", false );
    uint64_t *batch   = c_flag_uint64( "batchsize", "bs", "batch size", 32 );
    float    *lr      = c_flag_float( "learningrate", "lr", "lr", 1.25 );
    char **explain = c_flag_string( "explain", "e", "explain", "explain more" );

    int argc = 7;

    const char *const_argv[] = { (const char *)"binary_name",
                                 "--batchsize",
                                 "64",
                                 "--explain",
                                 "another reason",
                                 "--learningrate",
                                 "2.25" };
    char      **argv         = (char **)const_argv;

    int rc = c_flags_parse_internal( &argc, &argv );

    EVE_TEST_EXPECT( rc == 0, "result code" );
    EVE_TEST_EXPECT( !*verbose, "verbose" );
    EVE_TEST_EXPECT( *lr == 2.25, "learning_rate" );
    EVE_TEST_EXPECT( *batch == 64, "batch" );
    EVE_TEST_EXPECT_STR_EQ( *explain, "another reason", "explain" );
    EVE_TEST_PASS( );
}

EVE_TEST( Flags, FlipShortFlags )
{
    c_flags_reset_internal( );
    bool     *verbose = c_flag_bool( "verbose", "v", "verbose mode", false );
    uint64_t *batch   = c_flag_uint64( "batchsize", "bs", "batch size", 32 );
    float    *lr      = c_flag_float( "learningrate", "lr", "lr", 1.25 );
    char **explain = c_flag_string( "explain", "e", "explain", "explain more" );

    int argc = 7;

    const char *const_argv[] = {
        (const char *)"binary_name", "-bs", "164", "-e",
        "another reason 123",        "-lr", "4.5" };
    char **argv = (char **)const_argv;

    int rc = c_flags_parse_internal( &argc, &argv );

    EVE_TEST_EXPECT( rc == 0, "result code" );
    EVE_TEST_EXPECT( !*verbose, "verbose" );
    EVE_TEST_EXPECT( *lr == 4.50, "learning_rate" );
    EVE_TEST_EXPECT( *batch == 164, "batch" );
    EVE_TEST_EXPECT_STR_EQ( *explain, "another reason 123", "explain" );
    EVE_TEST_PASS( );
}
