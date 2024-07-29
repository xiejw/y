#include <testing/testing.h>
#include <testing/testing_macros.h>

#include "crypto/sha256.h"

#include <string.h>

static char *
test_empty_str( )
{
    sds_t sds = sha256DigestStr( "" );
    ASSERT_TRUE( "digest", 0 == strcmp( "e3b0c44298fc1c149afbf4c8996fb92427ae"
                                        "41e4649b934ca495991b7852b855",
                                        sds ) );
    ASSERT_TRUE( "len", 64 == sdsLen( sds ) );
    sdsFree( sds );
    return NULL;
}

static char *
test_str( )
{
    sds_t sds = sha256DigestStr( "grape" );
    ASSERT_TRUE( "digest", 0 == strcmp( "0f78fcc486f5315418fbf095e71c0675ee07"
                                        "d318e5ac4d150050cd8e57966496",
                                        sds ) );
    ASSERT_TRUE( "len", 64 == sdsLen( sds ) );
    sdsFree( sds );
    return NULL;
}

static char *
test_sds( )
{
    sds_t msg = sdsNew( "grape" );
    sds_t sds = sha256DigestSds( msg );
    ASSERT_TRUE( "digest", 0 == strcmp( "0f78fcc486f5315418fbf095e71c0675ee07"
                                        "d318e5ac4d150050cd8e57966496",
                                        sds ) );
    ASSERT_TRUE( "len", 64 == sdsLen( sds ) );
    sdsFree( sds );
    sdsFree( msg );
    return NULL;
}

static char *
test_str_padding_one_more( )
{
    // prepare a message with size as 64-9 < size < 64. so we need to pad
    // one more chunk (see sha256Finalize).
    sds_t msg = sdsEmptyWithCap( 64 );
    memset( msg, 1, 60 );
    sdsSetLen( msg, 60 );
    msg[60] = 0;
    ASSERT_TRUE( "msg len", 60 == strlen( msg ) );

    sds_t sds = sha256DigestStr( msg );
    ASSERT_TRUE( "digest", 0 == strcmp( "5e4084eff2f37d637e6502bf9472b0029755"
                                        "bbd130ebb52c8c33bb8148c31fd2",
                                        sds ) );
    sdsFree( sds );
    sdsFree( msg );
    return NULL;
}

static char *
test_long_str( )
{
    sds_t sds = sha256DigestStr(
        "Vanilla Policy Gradient is the most basic, entry-level algorithm "
        "in the deep RL space because it completely predates the advent of "
        "deep RL altogether. The core elements of VPG go all the way back "
        "to the late 80s / early 90s. It started a trail of research which "
        "ultimately led to stronger algorithms such as TRPO and then PPO "
        "soon after.\n\nA key feature of this line of work is that all of "
        "these algorithms are on-policy: that is, they don’t use old data, "
        "which makes them weaker on sample efficiency. But this is for a "
        "good reason: these algorithms directly optimize the objective you "
        "care about—policy performance—and it works out mathematically "
        "that you need on-policy data to calculate the updates. So, this "
        "family of algorithms trades off sample efficiency in favor of "
        "stability—but you can see the progression of techniques (from VPG "
        "to TRPO to PPO) working to make up the deficit on sample "
        "efficiency." );
    ASSERT_TRUE( "digest", 0 == strcmp( "2c2ca89ccbfa44169a32172be34361b85050"
                                        "0b0dd8121a1ef52d4bb43b767266",
                                        sds ) );
    sdsFree( sds );
    return NULL;
}

static char *
test_stream( )
{
    const char *msg = "grape";

    struct sha256 s;
    sha256Reset( &s );
    for ( int i = 0; i < 5; i++ ) {
        sha256Update( &s, (const unsigned char *)msg + i, 1 );
    }
    sds_t sds = sha256Digest( &s );

    ASSERT_TRUE( "digest", 0 == strcmp( "0f78fcc486f5315418fbf095e71c0675ee07"
                                        "d318e5ac4d150050cd8e57966496",
                                        sds ) );
    sdsFree( sds );
    return NULL;
}

static char *
test_file( )
{
    sds_t   sds;
    error_t err = sha256DigestFile( "./doc/eva/license", &sds );
    ASSERT_NO_ERR( err );
    ASSERT_STR_EQ(
        "digest",
        // generate from go code.
        "e9c565ef921aff58f0730ce2a5e33ebabc6ed05e7c3bd685e0065ce2cd7ef767",
        sds );
    ASSERT_TRUE( "len", 64 == sdsLen( sds ) );
    sdsFree( sds );
    return NULL;
}

DECLARE_TEST_SUITE( crypto_sha256 )
{
    RUN_TEST( test_empty_str );
    RUN_TEST( test_str );
    RUN_TEST( test_sds );
    RUN_TEST( test_str_padding_one_more );
    RUN_TEST( test_long_str );
    RUN_TEST( test_stream );
    RUN_TEST( test_file );
    return NULL;
}
