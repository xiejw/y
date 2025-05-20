#include <eve/testing/testing.h>

#include <cstring>
#include <iostream>

using eve::adt::Sds;

EVE_TEST( Testing, StringEq )
{
        Sds sds{ };
        EVE_TEST_EXPECT_STR_EQ( "a", "a", "eq" );
        EVE_TEST_EXPECT_STR_EQ( std::string_view{ "a" },
                                std::string_view{ "a" }, "eq" );
        EVE_TEST_EXPECT_STR_EQ( std::string_view{ "a" }, "a", "eq" );
        EVE_TEST_EXPECT_STR_EQ( "a", std::string_view{ "a" }, "eq" );
        EVE_TEST_EXPECT_STR_EQ( Sds{ "a" }, "a", "eq" );
        EVE_TEST_EXPECT_STR_EQ( Sds{ "a" }, std::string_view{ "a" }, "eq" );
        EVE_TEST_EXPECT_STR_EQ( Sds{ "a" }, Sds{ "a" }, "eq" );
        EVE_TEST_EXPECT_STR_EQ( "a", Sds{ "a" }, "eq" );
        EVE_TEST_EXPECT_STR_EQ( std::string_view{ "a" }, Sds{ "a" }, "eq" );
        EVE_TEST_EXPECT_STR_EQ( Sds{ "a" }, Sds{ "a" }, "eq" );
        EVE_TEST_PASS( );
}

EVE_TEST( Testing, StringNeq )
{
        Sds sds{ };
        EVE_TEST_EXPECT_STR_NEQ( "a", "b", "eq" );
        EVE_TEST_EXPECT_STR_NEQ( std::string_view{ "a" },
                                 std::string_view{ "b" }, "eq" );
        EVE_TEST_EXPECT_STR_NEQ( std::string_view{ "a" }, "b", "eq" );
        EVE_TEST_EXPECT_STR_NEQ( "a", std::string_view{ "b" }, "eq" );
        EVE_TEST_EXPECT_STR_NEQ( Sds{ "a" }, "b", "eq" );
        EVE_TEST_EXPECT_STR_NEQ( Sds{ "a" }, std::string_view{ "b" }, "eq" );
        EVE_TEST_EXPECT_STR_NEQ( "a", Sds{ "b" }, "eq" );
        EVE_TEST_EXPECT_STR_NEQ( std::string_view{ "a" }, Sds{ "b" }, "eq" );
        EVE_TEST_EXPECT_STR_NEQ( Sds{ "a" }, Sds{ "b" }, "eq" );
        EVE_TEST_PASS( );
}
