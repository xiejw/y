#include <iostream>

#include "eve/testing/testing.h"

EVE_TEST( BaseName, TestName )
{
    std::cout << "I am SuiteTestName\n";
    return nullptr;
}

int
main( )
{
    eve::testing::SuiteDriver driver{ };
    driver.RunAllSuites( );
}
