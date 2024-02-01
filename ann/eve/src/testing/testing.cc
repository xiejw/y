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
    for ( auto &suite : GetSuites( ) ) {
        std::cout << "Running: " << suite.name << "\n";
        suite.func->TestBody( );
    }
}

SuiteTestInfo::SuiteTestInfo( std::string test_name, SuiteBaseTest *func )
{
    GetSuites( ).push_back( Suite{ test_name, func } );
}

}  // namespace eve::testing
