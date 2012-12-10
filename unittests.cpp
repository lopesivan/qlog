#define QLOG_USE_ASSERTS
#ifdef TEST_MULTITHREADING
#   ifndef WIN32
#       define QLOG_MULTITHREAD_PTHREAD
#   else
#		define QLOG_MULTITHREAD_WIN32
#	endif
#endif
#include "qlog.hpp"
#include <UnitTest++/UnitTest++.h>
#include <iostream>
#include <fstream>
#ifndef WIN32
#   include <sys/resource.h>
#endif

#ifdef TEST_MULTITHREADING
#	include <thread>
#endif

using namespace qlog;

struct qlog_resetter
{
    qlog_resetter()
    {
        qlog::init();
    }

    virtual ~qlog_resetter()
    {
        qlog::destroy();
    }
};

TEST_FIXTURE( qlog_resetter, OneString )
{
    std::cout << "OneString" << std::endl;
    logger<loglevel::error> logger;
    std::ostringstream output;
    logger.set_output( output );
    logger << "a b c";

    CHECK_EQUAL( "a b c", output.str() );
}

TEST_FIXTURE( qlog_resetter, TwoStrings )
{
    std::cout << "TwoStrings" << std::endl;
    logger<loglevel::error> logger;
    std::ostringstream output;
    logger.set_output( output );
    logger << "a b c " << "1 2 3";

    CHECK_EQUAL( "a b c 1 2 3", output.str() );
}

TEST_FIXTURE( qlog_resetter, Disable )
{
    std::cout << "Disable" << std::endl;
    logger<loglevel::error> logger;
    std::ostringstream output;
    logger.set_output( output );
    logger.disable();
    logger << "a b c" << '\n' << "d e f" << std::endl;
    logger << "2";

    CHECK_EQUAL( 0UL, output.str().size() );
}


TEST_FIXTURE( qlog_resetter, CarriageReturn )
{
    std::cout << "CarriageReturn" << std::endl;
    logger<loglevel::error> logger;
    std::ostringstream output;
    logger.set_output( output );
    logger << "a b c\n1 2 3";

    CHECK_EQUAL( "a b c\n1 2 3", output.str() );
}

TEST_FIXTURE( qlog_resetter, StdEndl )
{
    std::cout << "StdEndl" << std::endl;
    logger<loglevel::error> logger;
    std::ostringstream output;
    logger.set_output( output );
    logger << "a b c" << std::endl << "1 2 3";

    CHECK_EQUAL( "a b c\n1 2 3", output.str() );
}

TEST_FIXTURE( qlog_resetter, ConditionalLogging )
{
    std::cout << "ConditionalLogging" << std::endl;
    logger<loglevel::error> logger;
    std::ostringstream output;
    logger.set_output( output );
    logger( true ) << "a b c";
    logger( false ) << "1 2 3";

    CHECK_EQUAL( "a b c", output.str() );
}

TEST_FIXTURE( qlog_resetter, Numbers )
{
    std::cout << "Numbers" << std::endl;
    logger<loglevel::error> logger;
    std::ostringstream output;
    logger.set_output( output );
    logger << 1L << 2 << 3UL;

    CHECK_EQUAL( "123", output.str() );
}

TEST_FIXTURE( qlog_resetter, Loglevel )
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

TEST_FIXTURE( qlog_resetter, LoglevelWithStdEndl )
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

TEST_FIXTURE( qlog_resetter, MultipleWritingToSingleOutput )
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

    CHECK_EQUAL( output.str(), "79" );
}

TEST_FIXTURE( qlog_resetter, WritingToAFile )
{
    std::cout << "WritingToAFile" << std::endl;
    const std::string filename( "tmptestfile" );
    std::remove( filename.c_str() );

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
        CHECK( inputFile.good() );
        int x;
        inputFile >> x;
        CHECK_EQUAL( 79,x );

        std::remove( filename.c_str() );
    }
}

TEST_FIXTURE( qlog_resetter, AppendOneString )
{
    std::cout << "AppendOneString" << std::endl;

    // setting every output to our test stream
    std::ostringstream output;
    set_output( output );

    set_loglevel( loglevel::debug );
    qlog::debug.append() << "a";

    qlog::debug << "1";
    qlog::trace << "2";
    qlog::info << "3";
    qlog::warning << "4";
    qlog::error << "5";

    CHECK_EQUAL( "1a2345", output.str() );

    qlog::debug.append() << "";
}

TEST_FIXTURE( qlog_resetter, AppendTwoStrings )
{
    std::cout << "AppendTwoStrings" << std::endl;
    // setting every output to our test stream
    std::ostringstream output;
    set_output( output );

    set_loglevel( loglevel::debug );
    qlog::debug.append() << "a";

    qlog::debug << "1" << "2";
    qlog::trace << "2";
    qlog::info << "3";
    qlog::warning << "4";
    qlog::error << "5";

    CHECK_EQUAL( "12a2345", output.str() );

    qlog::debug.append() << "";
}

TEST_FIXTURE( qlog_resetter, AppendTwoStringsAndEndl )
{
    std::cout << "AppendTwoStringsAndEndl" << std::endl;

    // setting every output to our test stream
    std::ostringstream output;
    set_output( output );

    set_loglevel( loglevel::debug );
    qlog::debug.append() << "a";
    qlog::trace.append() << "b";

    qlog::debug << "1" << "2" << std::endl;
    qlog::trace << "2";
    qlog::info << "3";
    qlog::warning << "4";
    qlog::error << "5";

    CHECK_EQUAL( "12\na2b345", output.str() );

    qlog::debug.append() << "";
    qlog::trace.append() << "";
}

TEST_FIXTURE( qlog_resetter, AppendEndl )
{
    std::cout << "AppendEndl" << std::endl;
    logger<loglevel::error> logger;
    std::ostringstream output;
    logger.set_output( output );
    logger.append() << "a";

    logger << std::endl;

    CHECK_EQUAL( "\na", output.str() );
}

TEST_FIXTURE( qlog_resetter, PrependOneString )
{
    std::cout << "PrependOneString" << std::endl;
    logger<loglevel::error> logger;
    std::ostringstream output;
    logger.set_output( output );
    logger.prepend() << "a";

    logger << "1";

    CHECK_EQUAL( "a1", output.str() );
}

TEST_FIXTURE( qlog_resetter, PrependTwoStrings )
{
    std::cout << "PrependTwoStrings" << std::endl;
    logger<loglevel::error> logger;
    std::ostringstream output;
    logger.set_output( output );
    logger.prepend() << "a";

    logger << "1" << "2";

    CHECK_EQUAL( "a12", output.str() );
}
TEST_FIXTURE( qlog_resetter, PrependTwoStringsAndEndl )
{
    std::cout << "PrependTwoStringsAndEndl" << std::endl;
    logger<loglevel::error> logger;
    std::ostringstream output;
    logger.set_output( output );
    logger.prepend() << "a";

    logger << "1" << "2" << std::endl;

    CHECK_EQUAL( "a12\n", output.str() );
}

TEST_FIXTURE( qlog_resetter, PrependEndl )
{
    std::cout << "PrependOneString" << std::endl;
    logger<loglevel::error> logger;
    std::ostringstream output;
    logger.set_output( output );
    logger.prepend() << "a";
    logger << std::endl;

    CHECK_EQUAL( "a\n", output.str() );
}

TEST_FIXTURE( qlog_resetter, Colors )
{
    std::cout << "Colors" << std::endl;


    logger<loglevel::error> logger;
    logger.set_output( std::cout );
    logger << color() << "This line should not have special colors\n";
    logger << color( green ) << "This line should be green on black\n";
    logger << color( red ) << "This line should be red on black\n";
    logger << color( blue ) << "This line should be blue on black\n";
    logger << color( green, true ) << "This line should be bold green on black" << color() << '\n';
    logger << color( yellow ) << "This line should be yellow on black\n";
    logger << color( yellow, true ) << "This line should be bold yellow on black\n";
    logger << color( red, true ) << "This line should be bold red on black" << color() << '\n';
    logger << color( blue, true ) << "This line should be bold blue on black" << color() << '\n';
    logger << color( green, red ) << "This line should be green on red" << color() << '\n';
    logger << color( blue, red ) << "This line should be blue on red" << color() << '\n';
    logger << color( red, green ) << "This line should be red on green" << color() << '\n';
    logger << color( red, blue ) << "This line should be red on blue" << color() << '\n';
    logger << "This " << underline() << "word" << color() << " should be underlined" << '\n';
    logger << "This " << blink() << "word" << color() << " should be blinking" << '\n';
    logger << underline() << "These words" << color() << " should be underlined" << '\n';
    logger << blink() << "These words" << color() << " should be blinking" << '\n';
    logger << "This " << color( blue, red ) << "word" << color() << " should be blue on red\n";
    logger << "This " << color( green, blue, true ) << "word" << color() << " should be bold green on blue\n";


}

TEST_FIXTURE( qlog_resetter, ColorAppend )
{
    std::cout << "ColorAppend" << std::endl;

    set_loglevel( loglevel::warning );
    logger<loglevel::error> logger_without_append;
    logger<loglevel::warning> logger_with_append;

    std::ostringstream output_witout_append;
    std::ostringstream output_with_append;

    logger_with_append.set_output( output_with_append );
    logger_without_append.set_output( output_witout_append );

    logger_with_append.append() << "z";

    logger_with_append << "a" << color( green ) << "b";
    logger_without_append << "a" << color( green ) << "b";

    logger_without_append << "z";

    CHECK_EQUAL( output_witout_append.str(), output_with_append.str() );
    logger_without_append << color(); // hack to restore color on windows

}

TEST_FIXTURE( qlog_resetter, ColorPrepend )
{
    std::cout << "ColorPrepend" << std::endl;

    set_loglevel( loglevel::warning );
    logger<loglevel::error> logger_without_prepend;
    logger<loglevel::warning> logger_with_prepend;

    std::ostringstream output_witout_append;
    std::ostringstream output_with_append;

    logger_with_prepend.set_output( output_with_append );
    logger_without_prepend.set_output( output_witout_append );

    logger_with_prepend.prepend() << "z";

    logger_with_prepend << color( green ) << "b";
    logger_without_prepend << "z" << color( green ) << "b";

    CHECK_EQUAL( output_witout_append.str(), output_with_append.str() );
    logger_without_prepend << color(); // hack to restore color on windows
}

TEST_FIXTURE( qlog_resetter, BlinkPrepend )
{
    std::cout << "BlinkPrepend" << std::endl;

    set_loglevel( loglevel::warning );
    logger<loglevel::error> logger_without_prepend;
    logger<loglevel::warning> logger_with_prepend;

    std::ostringstream output_witout_append;
    std::ostringstream output_with_append;

    logger_with_prepend.set_output( output_with_append );
    logger_without_prepend.set_output( output_witout_append );

    logger_with_prepend.prepend() << "z";

    logger_with_prepend << blink() << "b";
    logger_without_prepend << "z" << blink() << "b";

    CHECK_EQUAL( output_witout_append.str(), output_with_append.str() );


}

TEST_FIXTURE( qlog_resetter, UnderlinePrepend )
{
    std::cout << "UnderlinePrepend" << std::endl;

    set_loglevel( loglevel::warning );
    logger<loglevel::error> logger_without_prepend;
    logger<loglevel::warning> logger_with_prepend;

    std::ostringstream output_witout_append;
    std::ostringstream output_with_append;

    logger_with_prepend.set_output( output_with_append );
    logger_without_prepend.set_output( output_witout_append );

    logger_with_prepend.prepend() << "z";

    logger_with_prepend << underline() << "b";
    logger_without_prepend << "z" << underline() << "b";

    CHECK_EQUAL( output_witout_append.str(), output_with_append.str() );


}

TEST_FIXTURE( qlog_resetter, BlinkAppend )
{
    std::cout << "BlinkAppend" << std::endl;

    set_loglevel( loglevel::warning );
    logger<loglevel::error> logger_without_append;
    logger<loglevel::warning> logger_with_append;

    std::ostringstream output_witout_append;
    std::ostringstream output_with_append;

    logger_with_append.set_output( output_with_append );
    logger_without_append.set_output( output_witout_append );

    logger_with_append.append() << "z";

    logger_with_append << "a" << blink() << "b";
    logger_without_append << "a" << blink() << "b";

    logger_without_append << "z";

    CHECK_EQUAL( output_witout_append.str(), output_with_append.str() );
    logger_without_append << color(); // hack to restore color on cmd


}

TEST_FIXTURE( qlog_resetter, UnderlineAppend )
{
    std::cout << "UnderlineAppend" << std::endl;

    set_loglevel( loglevel::warning );
    logger<loglevel::error> logger_without_append;
    logger<loglevel::warning> logger_with_append;

    std::ostringstream output_witout_append;
    std::ostringstream output_with_append;

    logger_with_append.set_output( output_with_append );
    logger_without_append.set_output( output_witout_append );

    logger_with_append.append() << "z";

    logger_with_append << "a" << underline() << "b";
    logger_without_append << "a" << underline() << "b";

    logger_without_append << "z";

    CHECK_EQUAL( output_witout_append.str(), output_with_append.str() );
    logger_without_append << color(); // hack to restore color on cmd
}

TEST_FIXTURE( qlog_resetter, LoggingDisabled )
{
    std::cout << "LoggingDisabled" << std::endl;

    set_loglevel( loglevel::disabled );

    std::ostringstream ostr ;
    set_output( ostr );

    qlog::warning << "a" << '\n' << std::endl;
    qlog::error << "a" << '\n' << std::endl;
    qlog::trace << "a" << '\n' << std::endl;
    qlog::info << "a" << '\n' << std::endl;
    qlog::debug << "a" << '\n' << std::endl;

    CHECK_EQUAL( 0, ostr.str().size() );
}

TEST_FIXTURE( qlog_resetter, LoggingDisabledWithPrefixAndSuffixes )
{
    std::cout << "LoggingDisabledWithPrefixAndSuffixes" << std::endl;

    set_loglevel( loglevel::disabled );

    std::ostringstream ostr ;
    set_output( ostr );

    qlog::warning.prepend() << "a" << color( green );
    qlog::warning.append() << "b" << color();
    qlog::warning << "a" << '\n' << std::endl;

    CHECK_EQUAL( 0, ostr.str().size() );
}

TEST_FIXTURE( qlog_resetter, CustomFlavour )
{
    std::cout << "CustomFlavour\n";
    set_output( std::cout );
    set_loglevel( loglevel::info );

    qlog::info.prepend() << "[" << color(gray) << ".." << color() << "] ";
    qlog::warning.prepend() << "[" << color( green ) << "ww" << color() << "] ";
    qlog::error.prepend() << "[" << color( red, true ) << blink() << "EE" << color() << "] " << color(true);
    qlog::error.append() << color();

    qlog::info      << "A custom flavor info line\n";
    qlog::warning   << "A custom flavor warning line\n";
    qlog::error     << "A custom flavor error line\n";
}

TEST_FIXTURE( qlog_resetter, ColorCopy )
{
    std::cout << "ColorCopy\n";
    qlog::color vert(green);
    qlog::color copy( vert );

#ifndef WIN32
    CHECK_EQUAL(vert.getBold(), copy.getBold());
    CHECK_EQUAL(vert.getForeground(), copy.getForeground());
    CHECK_EQUAL(vert.getBackground(), copy.getBackground());
#else
    CHECK_EQUAL(vert.getAttributes(), copy.getAttributes());
#endif
}

#ifdef TEST_MULTITHREADING
void multithreading_test_one(const char ch, const unsigned maxIter)
{
    unsigned i = 0;
    while (i++ < maxIter)
        qlog::info << ch << ch << ch << ch << ch << ch;
}


TEST_FIXTURE( qlog_resetter, MultithreadingTestOne )
{
    std::cout << "MultithreadingTestOne" << std::endl;

    std::ostringstream ostr;
    set_loglevel( loglevel::info );
    set_output( ostr );

    std::thread t1( multithreading_test_one, 'a', 80000);
    std::thread t2( multithreading_test_one, 'b', 80000);

    t1.join();
    t2.join();

    const std::string & result = ostr.str();

    CHECK_EQUAL( 0, result.size() % 6 );
    for ( std::string::const_iterator it = result.begin(); it != result.end(); )
    {
        const char first_char = *it++;
        CHECK_EQUAL( first_char, *it++ );
        CHECK_EQUAL( first_char, *it++ );
        CHECK_EQUAL( first_char, *it++ );
        CHECK_EQUAL( first_char, *it++ );
        CHECK_EQUAL( first_char, *it++ );
    }
}

void createSomeThreads();

TEST_FIXTURE( qlog_resetter, MultithreadingTestTwo )
{
    std::cout << "MultithreadingTestTwo" << std::endl;

    std::ostringstream ostr;
    set_loglevel( loglevel::info );
    set_output( ostr );

    std::thread t1( multithreading_test_one, 'c', 80000);
    std::thread t2( multithreading_test_one, 'd', 80000);

    createSomeThreads();

    t1.join();
    t2.join();

    const std::string & result = ostr.str();

    CHECK_EQUAL( 0, result.size() % 6 );
    for ( std::string::const_iterator it = result.begin(); it != result.end(); )
    {
        const char first_char = *it++;
        CHECK_EQUAL( first_char, *it++ );
        CHECK_EQUAL( first_char, *it++ );
        CHECK_EQUAL( first_char, *it++ );
        CHECK_EQUAL( first_char, *it++ );
        CHECK_EQUAL( first_char, *it++ );
    }
}
#endif

#ifndef WIN32
TEST_FIXTURE( qlog_resetter, ExceptionSafety )
{
    std::cout << "ExceptionSafety" << std::endl;

    std::ostringstream ostr;

    set_loglevel( loglevel::warning );
    set_output( ostr );

    struct rlimit lim;
    getrlimit( RLIMIT_AS, &lim );
    lim.rlim_cur = 0;
    const int res = setrlimit( RLIMIT_AS, &lim );
    std::cout << "limit = " << lim.rlim_cur << '\n';
    CHECK_EQUAL( 0, res );

    try
    {
        qlog::error.prepend() << color( green ) << "a" << "b" << color( gray ) << color( white ) << "c" << color( black ) << "d" << color();
        qlog::error.append() << color( green ) << "a" << "b" << color( gray ) << color( white ) << "c" << color( black ) << "d" << color();
        qlog::debug.prepend() << color( green ) << "a" << "b" << color( gray ) << color( white ) << "c" << color( black ) << "d" << color();
        qlog::debug.append() << color( green ) << "a" << "b" << color( gray ) << color( white ) << "c" << color( black ) << "d" << color();
        qlog::trace.prepend() << color( green ) << "a" << "b" << color( gray ) << color( white ) << "c" << color( black ) << "d" << color();
        qlog::trace.append() << color( green ) << "a" << "b" << color( gray ) << color( white ) << "c" << color( black ) << "d" << color();

        qlog::info.prepend() << color( green ) << "a" << "b" << color( gray ) << color( white ) << "c" << color( black ) << "d" << color();
        qlog::info.append() << color( green ) << "a" << "b" << color( gray ) << color( white ) << "c" << color( black ) << "d" << color();

        qlog::warning.prepend() << color( green ) << "a" << "b" << color( gray ) << color( white ) << "c" << color( black ) << "d" << color();
        qlog::warning.append() << color( green ) << "a" << "b" << color( gray ) << color( white ) << "c" << color( black ) << "d" << color();

        qlog::warning << "abc" << std::endl;

    }
    catch ( std::exception & )
    {
        CHECK_EQUAL( 0, 1 );
    }
}
#endif
////////////////////////////////////////
// NOTHING BEYOND THIS                //
////////////////////////////////////////
int main( int , char ** )
{
    return UnitTest::RunAllTests();
}
