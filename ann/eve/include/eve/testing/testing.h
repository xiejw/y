// vim: ft=cpp
#pragma once

#include <iostream>
#include <string>

namespace eve::testing {

struct SuiteBaseTest {
    virtual const char *TestBody( ) = 0;
};

struct SuiteDriver {
    void RunAllSuites( );
};

struct SuiteTestInfo {
    SuiteTestInfo( std::string test_name, SuiteBaseTest *func );
};

}  // namespace eve::testing

#define EVE_TEST( test_cls, test_name )                                       \
    class EVA_TEST##test_cls##test_name : eve::testing::SuiteBaseTest {       \
      public:                                                                 \
        const char *TestBody( ) override;                                     \
                                                                              \
      private:                                                                \
        static eve::testing::SuiteTestInfo *test_info;                        \
    };                                                                        \
                                                                              \
    eve::testing::SuiteTestInfo *EVA_TEST##test_cls##test_name::test_info =   \
        new eve::testing::SuiteTestInfo{ #test_cls "::" #test_name,           \
                                         new EVA_TEST##test_cls##test_name }; \
                                                                              \
    const char *EVA_TEST##test_cls##test_name::TestBody( )

#define EVE_TEST_EXPECT( cond, msg ) \
    EVE_TEST_EXPECT_( cond, msg, __FILE__, __LINE__ )

#define EVE_TEST_EXPECT_( cond, msg, file, line )                         \
    do {                                                                  \
        if ( !( cond ) ) {                                                \
            std::cerr << "\033[1;33m...FILE " << file << " LINE " << line \
                      << "\033[0m";                                       \
            return ( msg );                                               \
        }                                                                 \
    } while ( 0 )

#define EVE_TEST_EXPECT_STR_EQ( s1, s2, msg ) \
    EVE_TEST_EXPECT_STR_EQ_( s1, s2, msg, __FILE__, __LINE__ )

#define EVE_TEST_EXPECT_STR_EQ_( s1, s2, msg, file, line )                \
    do {                                                                  \
        const auto &ref1 = ( s1 );                                        \
        const auto &ref2 = ( s2 );                                        \
        if ( !( ref1 == ref2 ) ) {                                        \
            std::cerr << "\033[1;33m...FILE " << file << " LINE " << line \
                      << "\033[0m";                                       \
            std::cerr << "\033[1;33m...\nLHS\n```\n"                      \
                      << ref1 << "\n```\nRHS\n```\n"                      \
                      << ref2 << "\n```\n"                                \
                      << "\033[0m";                                       \
            return ( msg );                                               \
        }                                                                 \
    } while ( 0 )

#define EVE_TEST_PASS( ) return nullptr
