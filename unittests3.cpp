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

TEST( ConditionalLogging )
{
    std::cout << "ConditionalLogging" << std::endl;
    logger<loglevel::error> logger;
    std::ostringstream output;
    logger.setOutput( output );
    logger( true ) << "a b c";
    logger( false ) << "1 2 3";

    CHECK_EQUAL( "a b c", output.str() );
}

TEST( Numbers )
{
    std::cout << "Numbers" << std::endl;
    logger<loglevel::error> logger;
    std::ostringstream output;
    logger.setOutput( output );
    logger << 1L << 2 << 3UL;


    CHECK_EQUAL( "123", output.str() );
}

TEST( Loglevel )
{
    std::cout << "Loglevel" << std::endl;
    set_loglevel( loglevel::info );

    {
        logger<loglevel::error> logger;
        std::ostringstream output;
        logger.setOutput( output );
        logger << "a";

        CHECK_EQUAL( "a", output.str() );
    }
    {
        logger<loglevel::warning> logger;
        std::ostringstream output;
        logger.setOutput( output );
        logger << "b";

        CHECK_EQUAL( "b", output.str() );
    }
    {
        logger<loglevel::info> logger;
        std::ostringstream output;
        logger.setOutput( output );
        logger << "b";

        CHECK_EQUAL( "b", output.str() );
    }
    {
        logger<loglevel::trace> logger;
        std::ostringstream output;
        logger.setOutput( output );
        logger << "c";

        CHECK_EQUAL( "", output.str() );
    }
    {
        logger<loglevel::debug> logger;
        std::ostringstream output;
        logger.setOutput( output );
        logger << "c";

        CHECK_EQUAL( "", output.str() );
    }
}

TEST( LoglevelWithStdEndl )
{
    std::cout << "LoglevelWithStdEndl" << std::endl;
    set_loglevel( loglevel::info );

    {
        logger<loglevel::error> logger;
        std::ostringstream output;
        logger.setOutput( output );
        logger << "a" << std::endl;

        CHECK_EQUAL( "a\n", output.str() );
    }
    {
        logger<loglevel::warning> logger;
        std::ostringstream output;
        logger.setOutput( output );
        logger << "a" << std::endl;

        CHECK_EQUAL( "a\n", output.str() );
    }
    {
        logger<loglevel::info> logger;
        std::ostringstream output;
        logger.setOutput( output );
        logger << "a" << std::endl;

        CHECK_EQUAL( "a\n", output.str() );
    }
    {
        logger<loglevel::trace> logger;
        std::ostringstream output;
        logger.setOutput( output );
        logger << "a" << std::endl;

        CHECK_EQUAL( "", output.str() );
    }
    {
        logger<loglevel::debug> logger;
        std::ostringstream output;
        logger.setOutput( output );
        logger << "a" << std::endl;

        CHECK_EQUAL( "", output.str() );
    }
}

int main( int , char ** )
{
    return UnitTest::RunAllTests();
}
