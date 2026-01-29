#include "eve/testing/testing.h"

#include <iostream>
#include <vector>

namespace eve::testing {

namespace {

struct Suite {
        std::string    name;
        SuiteBaseTest *func;
};

std::vector<Suite> &
GetSuites( )
{
        static auto suites = new std::vector<Suite>{ };
        return *suites;
};

}  // namespace

void
SuiteDriver::RunAllSuites( )
{
        auto failed_counter = 0;
        for ( auto &suite : GetSuites( ) ) {
                std::cout << "Running: " << suite.name;
                const char *result = suite.func->TestBody( );
                if ( result == nullptr ) {
                        std::cout << "\033[1;32m...OK\033[0m\n";
                } else {
                        failed_counter++;
                        std::cout << "\033[1;31m...Failed: " << result
                                  << "\033[0m\n";  // Red
                }
        }

        if ( failed_counter == 0 ) {
                std::cout << "\033[1;32mALL TESTS PASSED.\033[0m\n"; /* Green */
        } else {
                std::cout << "\033[1;31mFailed counter: " << failed_counter
                          << "\033[0m\n";  // Red
        }
}

SuiteTestInfo::SuiteTestInfo( std::string test_name, SuiteBaseTest *func )
{
        GetSuites( ).push_back( Suite{ test_name, func } );
}

bool
str_eq_helper_views( const std::string_view &s1, const std::string_view &s2 )
{
        return s1 == s2;
}

}  // namespace eve::testing
