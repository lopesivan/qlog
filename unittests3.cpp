#include "qdiilog3.hpp"
#include <UnitTest++.h>
#include <iostream>

using namespace qlog;

TEST( OneString )
{
    std::cout << "OneString" << std::endl;
    logger<loglevel::error> logger;
    std::ostringstream output;
    logger.setOutput( output );
    logger << "a b c";

    CHECK_EQUAL( "a b c", output.str() );
}

TEST( TwoStrings )
{
    std::cout << "TwoStrings" << std::endl;
    logger<loglevel::error> logger;
    std::ostringstream output;
    logger.setOutput( output );
    logger << "a b c " << "1 2 3";

    CHECK_EQUAL( "a b c 1 2 3", output.str() );
}

TEST( CarriageReturn )
{
    std::cout << "CarriageReturn" << std::endl;
    logger<loglevel::error> logger;
    std::ostringstream output;
    logger.setOutput( output );
    logger << "a b c\n1 2 3";

    CHECK_EQUAL( "a b c\n1 2 3", output.str() );
}

TEST( StdEndl )
{
    std::cout << "StdEndl" << std::endl;
    logger<loglevel::error> logger;
    std::ostringstream output;
    logger.setOutput( output );
    logger << "a b c" << std::endl << "1 2 3";

    CHECK_EQUAL( "a b c\n1 2 3", output.str() );
}


int main( int , char ** )
{
    return UnitTest::RunAllTests();
}
