// vim: ft=cpp
#pragma once

#include <string>

namespace eve::testing {

struct SuiteBaseTest {
    virtual char *TestBody( ) = 0;
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
        char *TestBody( ) override;                                           \
                                                                              \
      private:                                                                \
        static eve::testing::SuiteTestInfo *test_info;                        \
    };                                                                        \
                                                                              \
    eve::testing::SuiteTestInfo *EVA_TEST##test_cls##test_name::test_info =   \
        new eve::testing::SuiteTestInfo{ #test_cls ":" #test_name,            \
                                         new EVA_TEST##test_cls##test_name }; \
                                                                              \
    char *EVA_TEST##test_cls##test_name::TestBody( )
