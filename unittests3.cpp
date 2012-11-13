#include "qdiilog3.hpp"
#include <UnitTest++.h>
#include <iostream>
#include <fstream>

using namespace qlog;

TEST( OneString )
{
    std::cout << "OneString" << std::endl;
    logger<loglevel::error> logger;
    std::ostringstream output;
    logger.set_output( output );
    logger << "a b c";

    CHECK_EQUAL( "a b c", output.str() );
}

TEST( TwoStrings )
{
    std::cout << "TwoStrings" << std::endl;
    logger<loglevel::error> logger;
    std::ostringstream output;
    logger.set_output( output );
    logger << "a b c " << "1 2 3";

    CHECK_EQUAL( "a b c 1 2 3", output.str() );
}

TEST( CarriageReturn )
{
    std::cout << "CarriageReturn" << std::endl;
    logger<loglevel::error> logger;
    std::ostringstream output;
    logger.set_output( output );
    logger << "a b c\n1 2 3";

    CHECK_EQUAL( "a b c\n1 2 3", output.str() );
}

TEST( StdEndl )
{
    std::cout << "StdEndl" << std::endl;
    logger<loglevel::error> logger;
    std::ostringstream output;
    logger.set_output( output );
    logger << "a b c" << std::endl << "1 2 3";

    CHECK_EQUAL( "a b c\n1 2 3", output.str() );
}

TEST( ConditionalLogging )
{
    std::cout << "ConditionalLogging" << std::endl;
    logger<loglevel::error> logger;
    std::ostringstream output;
    logger.set_output( output );
    logger( true ) << "a b c";
    logger( false ) << "1 2 3";

    CHECK_EQUAL( "a b c", output.str() );
}

TEST( Numbers )
{
    std::cout << "Numbers" << std::endl;
    logger<loglevel::error> logger;
    std::ostringstream output;
    logger.set_output( output );
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
        logger.set_output( output );
        logger << "a";

        CHECK_EQUAL( "a", output.str() );
    }
    {
        logger<loglevel::warning> logger;
        std::ostringstream output;
        logger.set_output( output );
        logger << "b";

        CHECK_EQUAL( "b", output.str() );
    }
    {
        logger<loglevel::info> logger;
        std::ostringstream output;
        logger.set_output( output );
        logger << "b";

        CHECK_EQUAL( "b", output.str() );
    }
    {
        logger<loglevel::trace> logger;
        std::ostringstream output;
        logger.set_output( output );
        logger << "c";

        CHECK_EQUAL( "", output.str() );
    }
    {
        logger<loglevel::debug> logger;
        std::ostringstream output;
        logger.set_output( output );
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
        logger.set_output( output );
        logger << "a" << std::endl;

        CHECK_EQUAL( "a\n", output.str() );
    }
    {
        logger<loglevel::warning> logger;
        std::ostringstream output;
        logger.set_output( output );
        logger << "a" << std::endl;

        CHECK_EQUAL( "a\n", output.str() );
    }
    {
        logger<loglevel::info> logger;
        std::ostringstream output;
        logger.set_output( output );
        logger << "a" << std::endl;

        CHECK_EQUAL( "a\n", output.str() );
    }
    {
        logger<loglevel::trace> logger;
        std::ostringstream output;
        logger.set_output( output );
        logger << "a" << std::endl;

        CHECK_EQUAL( "", output.str() );
    }
    {
        logger<loglevel::debug> logger;
        std::ostringstream output;
        logger.set_output( output );
        logger << "a" << std::endl;

        CHECK_EQUAL( "", output.str() );
    }
}

TEST( MultipleWritingToSingleOutput )
{
    std::cout << "MultipleWritingToSingleOutput" << std::endl;
    std::ostringstream output;
    set_output( output );

    // in error mode, only errors are logged
    set_loglevel( loglevel::warning );

    qlog::debug( true ) << 1UL;
    qlog::debug( false ) << 2UL;
    qlog::trace( true ) << 3UL;
    qlog::trace( false ) << 4UL;
    qlog::info( true ) << 5UL;
    qlog::info( false ) << 6UL;
    qlog::warning( true ) << 7UL;
    qlog::warning( false ) << 8UL;
    qlog::error( true ) << 9UL;
    qlog::error( false ) << 0UL;

    CHECK_EQUAL(output.str(), "79");
}

TEST( WritingToAFile )
{
    std::cout << "WritingToAFile" << std::endl;
    const std::string filename("tmptestfile");
    std::remove(filename.c_str());

    {
        std::ofstream outputFile( filename.c_str() );
        set_output( outputFile );

        // in error mode, only errors are logged
        set_loglevel( loglevel::warning );

        qlog::debug( true ) << 1UL;
        qlog::debug( false ) << 2UL;
        qlog::trace( true ) << 3UL;
        qlog::trace( false ) << 4UL;
        qlog::info( true ) << 5UL;
        qlog::info( false ) << 6UL;
        qlog::warning( true ) << 7UL;
        qlog::warning( false ) << 8UL;
        qlog::error( true ) << 9UL;
        qlog::error( false ) << 0UL;
    }

    {
        std::ifstream inputFile( filename.c_str() );
        CHECK(inputFile.good());
        int x;
        inputFile >> x;
        CHECK_EQUAL(79,x);

        std::remove(filename.c_str());
    }
}

TEST( AppendOneString )
{
    std::cout << "AppendOneString" << std::endl;
    // setting every output to our test stream
    std::ostringstream output;
    set_output( output );

    set_loglevel( loglevel::debug );
    qlog::debug.append("a");

    qlog::debug << "1";
    qlog::trace << "2";
    qlog::info << "3";
    qlog::warning << "4";
    qlog::error << "5";

    CHECK_EQUAL( "1a2345", output.str() );
}

TEST( AppendTwoStrings )
{
    std::cout << "AppendTwoStrings" << std::endl;
    // setting every output to our test stream
    std::ostringstream output;
    set_output( output );

    set_loglevel( loglevel::debug );
    qlog::debug.append("a");

    qlog::debug << "1" << "2";
    qlog::trace << "2";
    qlog::info << "3";
    qlog::warning << "4";
    qlog::error << "5";

    CHECK_EQUAL( "12a2345", output.str() );
}

TEST( AppendTwoStringsAndEndl  )
{
    std::cout << "AppendTwoStringsAndEndl" << std::endl;

    // setting every output to our test stream
    std::ostringstream output;
    set_output( output );

    set_loglevel( loglevel::debug );
    qlog::debug.append("a");
    qlog::trace.append("b");

    qlog::debug << "1" << "2" << std::endl;
    qlog::trace << "2";
    qlog::info << "3";
    qlog::warning << "4";
    qlog::error << "5";

    CHECK_EQUAL( "12\na2b345", output.str() );
}

TEST( AppendEndl )
{
    std::cout << "AppendEndl" << std::endl;
    // setting every output to our test stream
    // setting every output to our test stream
    logger<loglevel::error> logger;
    std::ostringstream output;
    logger.set_output( output );
    logger.append("a");

    logger << std::endl;

    CHECK_EQUAL( "\na", output.str() );
}

TEST( PrependOneString )
{
    std::cout << "PrependOneString" << std::endl;
    // setting every output to our test stream
    logger<loglevel::error> logger;
    std::ostringstream output;
    logger.set_output( output );
    logger.prepend("a");

    logger << "1";

    CHECK_EQUAL( "a1", output.str() );
}

TEST( PrependTwoStrings )
{
    std::cout << "PrependTwoStrings" << std::endl;
    // setting every output to our test stream
    logger<loglevel::error> logger;
    std::ostringstream output;
    logger.set_output( output );
    logger.prepend("a");

    logger << "1" << "2";

    CHECK_EQUAL( "a12", output.str() );
}
TEST( PrependTwoStringsAndEndl )
{
    std::cout << "PrependTwoStringsAndEndl" << std::endl;
    // setting every output to our test stream
    logger<loglevel::error> logger;
    std::ostringstream output;
    logger.set_output( output );
    logger.prepend("a");

    logger << "1" << "2" << std::endl;

    CHECK_EQUAL( "a12\n", output.str() );
}

TEST( PrependEndl )
{
    std::cout << "PrependOneString" << std::endl;
    // setting every output to our test stream
    logger<loglevel::error> logger;
    std::ostringstream output;
    logger.set_output( output );
    logger.prepend("a");
    logger << std::endl;

    CHECK_EQUAL( "a\n", output.str() );
}

TEST( Colors )
{
    std::cout << "Colors" << std::endl;
	qlog::init();

    // setting every output to our test stream
    logger<loglevel::error> logger;
    std::ostringstream output;
    logger.set_output( std::cout );
	logger << color() << "This line should not have special colors\n";
    logger << color(green) << "This line should be green on black\n";
	logger << color(red) << "This line should be red on black\n";
	logger << color(blue) << "This line should be blue on black\n";
	logger << color(green, true) << "This line should be bold green on black" << color() << '\n';
	logger << color(yellow) << "This line should be yellow on black\n";
	logger << color(yellow, true) << "This line should be bold yellow on black\n";
	logger << color(red, true) << "This line should be bold red on black" << color() << '\n';
	logger << color(blue, true) << "This line should be bold blue on black" << color() << '\n';
	logger << color(green, red) << "This line should be green on red" << color() << '\n';
	logger << color(blue, red) << "This line should be blue on red" << color() << '\n';
	logger << color(red, green) << "This line should be red on green" << color() << '\n';
	logger << color(red, blue) << "This line should be red on blue" << color() << '\n';
	logger << "This " << color(blue, red) << "word" << color() << " should be blue on red\n";
	logger << "This " << color(green, blue, true) << "word" << color() << " should be bold green on blue\n";

	qlog::destroy();
}


int main( int , char ** )
{
    return UnitTest::RunAllTests();
}
