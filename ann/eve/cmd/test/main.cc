#include <iostream>

#include <string>
#include <vector>

struct SuiteBaseTest {
    virtual char *TestBody( ) = 0;
};

struct Suite {
    std::string    name;
    SuiteBaseTest *func;
};

class SuiteUmbrella {
  public:
    static std::vector<Suite> suites;

    void RunAllSuites( )
    {
        for ( auto &suite : suites ) {
            std::cout << "Running: " << suite.name << "\n";
            suite.func->TestBody( );
        }
    }
};

std::vector<Suite> SuiteUmbrella::suites{ };

class SuiteTestInfo {
  public:
    SuiteTestInfo( std::string test_name, SuiteBaseTest *func )
    {
        SuiteUmbrella::suites.push_back( Suite{ test_name, func } );
    }
};

class SuiteTestName : SuiteBaseTest {
  public:
    char *TestBody( ) override;

  private:
    static SuiteTestInfo *test_info;
};

SuiteTestInfo *SuiteTestName::test_info =
    new SuiteTestInfo{ "hello_test_name", new SuiteTestName };

char *
SuiteTestName::TestBody( )
{
    std::cout << "I am SuiteTestName\n";
    return nullptr;
}

int
main( )
{
    SuiteUmbrella suites{ };
    suites.RunAllSuites( );
}
