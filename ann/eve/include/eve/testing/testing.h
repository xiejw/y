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

#define EVE_TEST_PASS( ) return nullptr
