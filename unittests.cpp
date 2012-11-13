#include <UnitTest++.h>
#include <fstream>
#include <iomanip>
#if 0
#include <thread>
#endif
//#include "test_with_different_preprocessor_directives.h"
#include "qdiilog.hpp"

TEST( TestVectorGeneral )
{
    /*
    log_debug << "hey!" << std::endl;
    std::vector<Logger<Loglevel::debug>*> & debugLoggers
        = Logger<Loglevel::debug>::m_allLoggers;
    CHECK( std::find( debugLoggers.begin(), debugLoggers.end(), &log_debug) != debugLoggers.end() );
     */
}

TEST( Test0 )
{
    std::cout << "Test 0" << std::endl;
    Logger<Loglevel::error> logger;
    std::ostringstream output;
    logger.setOutput( output );

    logger << "bla";
    logger << std::endl;
}

TEST( Test1 )
{
    std::cout << "Test 1" << std::endl;
    const std::string message( "bla" );
    Logger<Loglevel::error> logger;

    {
        std::ostringstream output;
        logger.setOutput( output );

        logger << "bla" << "blu";
        CHECK_EQUAL( "blablu", output.str() );
    }

    {
        std::ostringstream output;
        logger.setOutput( output );

        logger << message.c_str();
        CHECK_EQUAL( message, output.str() );
    }

    {
        std::ostringstream output2;
        logger.setOutput( output2 );

        logger << message;
        CHECK_EQUAL( message, output2.str() );
    }

}

TEST( Test2 )
{
    const std::string message( "bla1" );
    std::ostringstream output;
    Logger<Loglevel::error> logger;
    logger.setOutput( output );

    logger( true ) << "bla";
    CHECK_EQUAL( "bla", output.str() );

    logger( false ) << "blu" << std::endl;
    logger( true ) << 1;

    CHECK_EQUAL( message, output.str() );
}

TEST( Test3 )
{
    const std::string message( "bla" );
    std::ostringstream output;
    Logger<Loglevel::error> logger;
    logger.setOutput( output );

    logger( true ) << "bla" << ' ' << "34";
    logger( false ) << "blu";

    CHECK_EQUAL( message + ' ' + "34" , output.str() );
}

TEST( Test4 )
{
    std::ostringstream output;
    Logger<Loglevel::error> logger;
    logger.setOutput( output );

    logger( true ) << 1UL;
    logger( false ) << 2UL;

    CHECK_EQUAL( "1" , output.str() );
}

TEST( Test5  )
{
    std::ostringstream output;
    Logger<Loglevel::error> logger;
    logger.setOutput( output );
    logger.prepend( "a" );

    logger << "1" << "2" << 3;

    logger.prepend("");
    CHECK_EQUAL( "a123" , output.str() );
}

TEST( TestEndl )
{
    std::cout << "TestEndl\n";
    Logger<Loglevel::warning> logger;
    setLogLevel(Loglevel::warning);
    {
        std::ostringstream output;
        logger.setOutput( output );
        logger << std::endl;
        CHECK_EQUAL( "\n", output.str() );
    }

    setLogLevel(Loglevel::error);
    {
        std::ostringstream output;
        logger.setOutput( output );
        logger << std::endl;
        logger << "something" << std::endl;

        // logger is in warning mode, so nothing should be output
        CHECK_EQUAL (0UL, output.str().size() );
    }
}

TEST( TestIntermediaire )
{
    std::ostringstream output;
    CHECK( output.rdbuf() != nullptr );
    setOutput( output );
    CHECK( log_debug.rdbuf() != nullptr );
    setLogLevel( Loglevel::debug ); // in debug mode, everything is logged
    auto && loggerIntermediaire = log_debug(true);
    CHECK_EQUAL(loggerIntermediaire.rdbuf(), log_debug.rdbuf());
    loggerIntermediaire << 1UL;

    CHECK_EQUAL( "1" , output.str() );
}

TEST( TestFilterDebug )
{
    // setting every output to our test stream
    std::ostringstream output;
    setOutput( output );
    setLogLevel( Loglevel::debug ); // in debug mode, everything is logged
    log_debug( true ) << 1UL;
    log_debug( false ) << 2UL;
    CHECK_EQUAL( "1" , output.str() );

    log_trace( true ) << 1UL;
    log_trace( false ) << 2UL;
    CHECK_EQUAL( "11" , output.str() );

    log_warning( true ) << 1UL;
    log_warning( false ) << 2UL;
    CHECK_EQUAL( "111" , output.str() );

    log_error( true ) << 1UL;
    log_error( false ) << 2UL;
    CHECK_EQUAL( "1111" , output.str() );

    log_info( true ) << 1UL;
    log_info( false ) << 2UL;
    CHECK_EQUAL( "11111" , output.str() );
}

TEST( TestFilterTrace )
{
    // setting every output to our test stream
    std::ostringstream output;
    setOutput( output );

    // in trace mode, everything but debug is logged
    setLogLevel( Loglevel::trace );

    log_debug( true ) << 1UL;
    log_debug( false ) << 2UL;
    CHECK_EQUAL( "" , output.str() );

    log_trace( true ) << 1UL;
    log_trace( false ) << 2UL;
    CHECK_EQUAL( "1" , output.str() );

    log_warning( true ) << 1UL;
    log_warning( false ) << 2UL;
    CHECK_EQUAL( "11" , output.str() );

    log_error( true ) << 1UL;
    log_error( false ) << 2UL;
    CHECK_EQUAL( "111" , output.str() );

    log_info( true ) << 1UL;
    log_info( false ) << 2UL;
    CHECK_EQUAL( "1111" , output.str() );
}

TEST( TestFilterInfo )
{
    // setting every output to our test stream
    std::ostringstream output;
    setOutput( output );

    // in info mode, everything but debug and trace is logged
    setLogLevel( Loglevel::info );

    log_debug( true ) << 1UL;
    log_debug( false ) << 2UL;
    CHECK_EQUAL( "" , output.str() );

    log_trace( true ) << 1UL;
    log_trace( false ) << 2UL;
    CHECK_EQUAL( "" , output.str() );

    log_info( true ) << 1UL;
    log_info( false ) << 2UL;
    CHECK_EQUAL( "1" , output.str() );

    log_warning( true ) << 1UL;
    log_warning( false ) << 2UL;
    CHECK_EQUAL( "11" , output.str() );

    log_error( true ) << 1UL;
    log_error( false ) << 2UL;
    CHECK_EQUAL( "111" , output.str() );
}

TEST( TestFilterWarning )
{
    // setting every output to our test stream
    std::ostringstream output;
    setOutput( output );

    // in warning mode, only warning and debug are logged
    setLogLevel( Loglevel::warning );

    log_debug( true ) << 1UL;
    log_debug( false ) << 2UL;
    CHECK_EQUAL( "" , output.str() );

    log_trace( true ) << 1UL;
    log_trace( false ) << 2UL;
    CHECK_EQUAL( "" , output.str() );

    log_info( true ) << 1UL;
    log_info( false ) << 2UL;
    CHECK_EQUAL( "" , output.str() );

    log_warning( true ) << 1UL;
    log_warning( false ) << 2UL;
    CHECK_EQUAL( "1" , output.str() );

    log_error( true ) << 1UL;
    log_error( false ) << 2UL;
    CHECK_EQUAL( "11" , output.str() );
}

TEST( TestFilterError )
{
    // setting every output to our test stream
    std::ostringstream output;
    setOutput( output );

    // in error mode, only errors are logged
    setLogLevel( Loglevel::error );

    log_debug( true ) << 1UL;
    log_debug( false ) << 2UL;
    CHECK_EQUAL( "" , output.str() );

    log_trace( true ) << 1UL;
    log_trace( false ) << 2UL;
    CHECK_EQUAL( "" , output.str() );

    log_info( true ) << 1UL;
    log_info( false ) << 2UL;
    CHECK_EQUAL( "" , output.str() );

    log_warning( true ) << 1UL;
    log_warning( false ) << 2UL;
    CHECK_EQUAL( "" , output.str() );

    log_error( true ) << 1UL;
    log_error( false ) << 2UL;
    CHECK_EQUAL( "1" , output.str() );
}

TEST( TestOutputFile )
{
    const std::string filename("tmptestfile");
    std::remove(filename.c_str());

    {
        std::ofstream outputFile(filename);
        setOutput( outputFile );

        // in error mode, only errors are logged
        setLogLevel( Loglevel::warning );

        log_debug( true ) << 1UL;
        log_debug( false ) << 2UL;
        log_trace( true ) << 1UL;
        log_trace( false ) << 2UL;
        log_info( true ) << 1UL;
        log_info( false ) << 2UL;
        log_warning( true ) << 1UL;
        log_warning( false ) << 2UL;
        log_error( true ) << 1UL;
        log_error( false ) << 2UL;
    }

    {
        std::ifstream inputFile(filename);
        CHECK(inputFile.good());
        int x;
        inputFile >> x;
        CHECK_EQUAL(11,x);

        std::remove(filename.c_str());
    }
}

TEST( TestFilterInfoDecorate )
{
    // setting every output to our test stream
    std::ostringstream output;
    setOutput( output );

    // in info mode, everything but debug and trace is logged
    setLogLevel( Loglevel::info );

    log_warning.prepend( "aaa" );

    log_debug( true ) << 1UL;
    log_debug( false ) << 2UL;
    CHECK_EQUAL( "" , output.str() );

    log_trace( true ) << 1UL;
    log_trace( false ) << 2UL;
    CHECK_EQUAL( "" , output.str() );

    log_info( true ) << 1UL;
    log_info( false ) << 2UL;
    CHECK_EQUAL( "1" , output.str() );

    log_warning( true ) << 1UL;
    log_warning( false ) << 2UL;
    CHECK_EQUAL( "1aaa1" , output.str() );

    log_error( true ) << 1UL;
    log_error( false ) << 2UL;
    CHECK_EQUAL( "1aaa11" , output.str() );
}



TEST( TestGeneralSetOutput )
{
    // setting every output to our test stream
    std::ostringstream output;
    setOutput( output );

    setLogLevel( Loglevel::debug );

    log_debug.prepend("");
    log_trace.prepend("");
    log_info.prepend("");
    log_warning.prepend("");
    log_error.prepend("");

    log_debug << "1";
    log_trace << "2";
    log_info << "3";
    log_warning << "4";
    log_error << "5";

    CHECK_EQUAL( "12345", output.str() );
}

TEST( TestDisableOutput )
{
    // setting every output to our test stream
    std::ostringstream output;
    setOutput( output );

    setLogLevel( Loglevel::debug );

    log_debug << "1";
    log_trace << "2";
    log_info << "3";
    log_warning << "4";
    log_error << "5";

    CHECK_EQUAL( "12345", output.str() );

    setLogLevel( Loglevel::disable );
    log_debug << "1";
    log_trace << "2";
    log_info << "3";
    log_warning << "4";
    log_error << "5";

    // output should be unchanged
    CHECK_EQUAL( "12345", output.str() );
}

TEST( TestAppend )
{
    // setting every output to our test stream
    std::ostringstream output;
    setOutput( output );

    setLogLevel( Loglevel::debug );
    log_debug.append("a");

    log_debug << "1";
    log_trace << "2";
    log_info << "3";
    log_warning << "4";
    log_error << "5";

    CHECK_EQUAL( "1a2345", output.str() );
}

TEST( TestAppend1 )
{
    // setting every output to our test stream
    std::ostringstream output;
    setOutput( output );

    setLogLevel( Loglevel::debug );
    log_debug.append("a");

    log_debug << "1" << "2";
    log_trace << "2";
    log_info << "3";
    log_warning << "4";
    log_error << "5";

    CHECK_EQUAL( "12a2345", output.str() );
}

TEST( TestAppend2 )
{
    // setting every output to our test stream
    std::ostringstream output;
    setOutput( output );

    setLogLevel( Loglevel::debug );
    log_debug.append("a");
    log_trace.append("b");

    log_debug << "1" << "2" << std::endl;
    log_trace << "2";
    log_info << "3";
    log_warning << "4";
    log_error << "5";

    CHECK_EQUAL( "12\na2b345", output.str() );
}

TEST( TestQdiiFlavour )
{
    // setting every output to our test stream
    std::ostringstream output;
    setOutput( output );

    setLogLevel( Loglevel::debug ); // in debug mode, everything is logged
    setPrependTextQdiiFlavour();

    log_debug( true ) << 1UL;
    log_debug( false ) << 2UL;
    CHECK_EQUAL( "1" , output.str() );

    log_trace( true ) << 1UL;
    log_trace( false ) << 2UL;
    CHECK_EQUAL( "11" , output.str() );

    log_info( true ) << 1UL;
    log_info( false ) << 2UL;
    CHECK_EQUAL( "11[..] 1" , output.str() );

    log_warning( true ) << 1UL;
    log_warning( false ) << 2UL;
    CHECK_EQUAL( "11[..] 1[ww] 1" , output.str() );

    log_error( true ) << 1UL;
    log_error( false ) << 2UL;
    CHECK_EQUAL( "11[..] 1[ww] 1[EE] 1" , output.str() );
}

TEST( TestQdiiFlavourBashColour )
{
    // setting every output to our test stream
    std::ostringstream output;
    setOutput( output );
    setLogLevel( Loglevel::error ); // in debug mode, everything is logged
    setPrependedTextQdiiFlavourBashColors();

    log_error << "a";

    const std::string res = std::string( "[" ) + setBashColor( RED ) + "EE" +
        setBashColor( NONE ) + "]" + setBashColor( NONE, NONE, true ) + " a"
        + setBashColor( NONE, NONE, false );

    CHECK_EQUAL( res, output.str() );
}

#if 0
TEST( TestMultiThreading )
{
    setLogLevel(Loglevel::error);
    std::ostringstream output;
    setOutput(output);

    static int threadNumber = 0;
    struct ThreadOutput
    {
        ThreadOutput()
            :m_number(threadNumber++)
            {
            }
        void operator()()
        {
            int i = 0;
            while (i++<100)
                log_debug << m_number << m_number << m_number
                          << m_number << m_number << m_number
                          << m_number << m_number << m_number
                          << m_number << m_number << m_number
                          << m_number << m_number << m_number
                          << m_number << m_number << m_number
                          << m_number << m_number << m_number << '\n';
        }
    private:
        int m_number;
    } thread1, thread2;

    std::thread thr1(thread1);
    std::thread thr2(thread2);

    thr1.join();
    thr2.join();
}
#endif
int main( int , char ** )
{
    return UnitTest::RunAllTests();
}
