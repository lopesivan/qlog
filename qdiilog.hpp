#ifndef QDIILOG_H
#define QDIILOG_H

/** @mainpage
 * DESCRIPTION
 * -----------  
 * qdiilog is a header-only library that provides facilities to log messages
 * for tracing, debugging, etc. The messages can be written on screen or on
 * file.
 *
 * SIMPLE USAGE
 * ------------
 * 5 objects are available to the user to log messages, they are:
 * - log_debug
 * - log_trace
 * - log_info
 * - log_warning
 * - log_error
 * 
 * They work the same way std::cout does, for instance:
 * @code{.cpp}
 * log_warning << "foo() was passed a null pointer" << std::endl;
 * @endcode
 *
 * REDIRECTING TO A FILE
 * ---------------------
 * The objects will all log to std::cout by default, but you can change this
 * behaviour by calling setOutput(). Actually, there are 2 setOutput functions.
 * One of them is a member function that you can call on any log object, like
 * log_debug.setOutput( someOutput ), and the other one is a global function
 * that will call setOutput on every available log object.
 * Note that setOutput() takes a std::ostream reference as a parameter.
 *
 * The following snippet will redirect all output to myprogram.log
 * @code{.cpp}
 * int main()
 * {
 *   std::ofstream logstream( "myprogram.log" );
 *   setOutput( logstream );
 *   
 *   int ret = foo();
 *   log_info << "foo() returned: " << ret << std::endl;
 *   return 0
 * }
 * @endcode
 *
 * Now if you want to redirect the debug level to a different file, it is also
 * possible:
 * @code{.cpp}
 * int main()
 * {
 *    // redirecting all the messages to myprogram log
 *    std::ofstream logstream( "myprogram.log" );
 *    setOutput( logstream );
 * 
 *    // redirecting the debug messages to debug.log
 *    std::ofstream debug_stream( "debug.log" );
 *    log_debug.setOutput( debug_stream );
 * 
 *    log_debug << "calling foo()" << std::endl; // this will go in debug.log
 *    int ret = foo();
 *    log_info << "foo() returned: " << ret << std::endl; // this goes to myprogram.log
 *    
 *    return 0;
 * }
 * @endcode
 *
 * FILTERING IMPORTANT MESSAGES IN
 * -------------------------------
 *
 * You can restrict the messages output to only the important ones by calling
 * the global function setLogLevel() with one of the following parameters:
 * - Loglevel::debug
 * - Loglevel::trace
 * - Loglevel::info
 * - Loglevel::warning
 * - Loglevel::error
 * 
 * If you call <c>setLogLevel( Loglevel::warning )</c>, only the error and warning
 * messages will be processed, the more detailed messages will be ignored.
 * 
 * PREPENDING YOUR MESSAGES WITH SOME CUSTOM TEXT
 * ----------------------
 * Something I like when I debug is to have every message clearly stating if 
 * it is a warning, an error, or whatever. Normally, I'd prepend the warning
 * messages with some custom text, like <b>[ww]</b>, the error messages with 
 * some other (say, <b>[EE]</b>), and the information messages with something
 * less visible, such as <b>[..]</b>, so that my log clearly displays what’s 
 * important.
 * @verbatim
   [..] Initializing connection...
   [..] OK, connection initialized
   [..] Preparing data for transfert
   [ww] Data seems to be anormally long
   [..] Sending over data
   [..] ACK has been received
   [..] Data transfer is finished
   [EE] The server has unexpectedly closed connection
   @endverbatim
 * Here is an easy way to do that:
 * @code{.cpp}
 * log_info.setPrependText("[..] ");
 * log_warning.setPrependText("[ww] ");
 * log_error.setPrependText("[EE] ");
 * @endcode
 * 
 * 
 * TIPS
 * ----
 * A handy feature is the possibility to disable the logging easily:
 * Instead of writing:
 * @code{.cpp}
 * if ( foo() != SUCCESS )
 * {
 *    log_warning << "problem!" << std::endl;
 * }
 * @endcode
 * You can write:
 * @code{.cpp}
 * log_warning( foo() != SUCCESS ) << "problem" << std::endl;
 * @endcode
 */
#include <iostream>
#include <memory> // auto_ptr

#ifdef QDIILOG_NAMESPACE
namespace QDIILOG_NAMESPACE
{
#endif

typedef int_fast32_t ix;
typedef ix ErrorCode;

enum class Loglevel
{
    debug,
    trace,
    info,
    warning,
    error
};

static const ErrorCode          OK                  = 0;
static const ErrorCode          INVALID_LOGLEVEL    = -1;

#define QDIILOG_DECL_HIDDEN __attribute__ ((visibility("hidden")))
#define QDIILOG_DECL_VISIBLE __attribute__ ((visibility("default")))

//------------------------------------------------------------
struct QDIILOG_DECL_HIDDEN QdiilogOstream;
/**@private
 * @internal
 * @brief A class that is used as template parameter for the loggers */
struct QdiilogOstream
{
    typedef std::ostream Output;
    Loglevel filter_level;

    QdiilogOstream()
        :filter_level( Loglevel::error )
    {
    }
};

//------------------------------------------------------------
/**@private
 * @internal
 * @brief A class that is used to prevent prepend messages from being added 
 *        too often */
template <typename QdiilogParameters>
struct UndecoratedLogger;

//------------------------------------------------------------
/**@struct Logger
 * @brief Logs messages
 * 
 * Normally, users should not have to instantiate this class and should rather
 * use the already existing ones:
 *  - log_debug
 *  - log_trace
 *  - log_info
 *  - log_warning
 *  - log_error
 * 
 * The normal usage of a class is similar to std::cout. For instance, if I
 * wanted to log the return value of a function, I could type.
 * @code{.cpp}
 * log_debug << "the return value of foo() is: " << ret << std::endl;
 * @endcode
 */
template <typename QdiilogParameters>
struct Logger
{
    /**@brief Output is std::ostream, really
     * @todo Simplify this
     * @internal
     * @private */
    typedef typename QdiilogParameters::Output Output;

    /**@brief Constructs a Logger
     * @param[in] _level The level of logging. */
    explicit Logger( Loglevel _level = Loglevel::error )
        :m_output( nullptr )
        ,m_level( _level )
        ,m_userPrepend()
        ,m_undecoratedLogger( nullptr )
    {
    }

    /**@brief Constructs a Logger
     * @private
     * @param[in] _level The level of logging.
     * @param[in] _notDecorated Internal use, don’t touch. */
    Logger( Loglevel _level, bool _notDecorated )
        :m_output( nullptr )
        ,m_level( _level )
        ,m_userPrepend()
        ,m_undecoratedLogger( nullptr )
    {
        if( !_notDecorated )
            m_undecoratedLogger = new UndecoratedLogger<QdiilogParameters>( _level );
    }

    /**@brief Destructs a Logger */
    virtual ~Logger()
    {
        delete m_undecoratedLogger;
    }

    /**@brief Indicates where the messages will be written */
    ErrorCode setOutput( Output & _output )
    {
        m_output = &_output;

        if( m_undecoratedLogger )
            m_undecoratedLogger->m_output = &_output;

        return OK;
    }

    /**
     * This function lets you add a custom text before any message logged.
     * For instance, if you want all warning messages to be preceded by
     * <c>WARNING: </c>, you could call
     * @code{.cpp}log_warning.setPrependText("WARNING: "); @endcode
     * This way, whenever you’ll type a warning message, it will be preceded
     * by your text. For instance,
     * @code{.cpp}log_warning << "something odd happened\n"; @endcode
     * will write <c>WARNING: something odd happened</c>
     * @brief Adds a custom text before the message to log.
     * @param[in] _prependText The text to add before the log message.
     * @warning Calling setPrependText many times cancels the text previously
     *          set */
    ErrorCode setPrependText( const std::string & _prependText )
    {
        m_userPrepend.assign( _prependText );
        return OK;
    }

    /**
     * <c>operator()</c> has been rewritten to provide a handy way to log
     * under condition. This is particularly handy when you want to warn the
     * user that something went wrong without writing if and else cases.
     * Consider this example:
     * @code{.cpp}
     * if (ret != SUCCESS)
     * {
     *    log_warning << "something odd happenned\n";
     * }
     * else
     * {
     *    log_info << "entering foo()\n";
     * }
     * @endcode
     * The previous snippet can easily be rewritten:
     * @code{.cpp}
     * log_warning(ret != SUCCESS) << "something odd happenned\n";
     * log_info(ret == SUCCESS) << "entering foo()\n";
     * @endcode
     * @brief Provides a simple way to disable/enable logging
     * @param[in] _condition The logging will be enabled only if
     *            _condition is true*/
    Logger & operator()( bool _condition );

private:
    Logger( Logger & _logger ) = delete;
    Logger & operator=( Logger & _logger ) = delete;

    std::ostream  * m_output;
    template<typename P, typename T>
    friend Logger<P> & operator<<( Logger<P> & _logger, T && _t );

private:
    Loglevel    m_level;
    static QdiilogParameters g_config;

public:
    /**@private
     * @internal
     * @brief Sets the log level under which the loggers don’t display anything
     */
    static void setLogLevel( Loglevel _level )
    {
        g_config.filter_level = _level;
    }

private:
    virtual bool isDecorated() const
    {
        return m_userPrepend.size() > 0;
    }
    virtual void prepend()
    {
        if( m_userPrepend.size() && m_output )
        {
            ( *m_output ) << m_userPrepend;
        }
    }
    std::string m_userPrepend;

    UndecoratedLogger<QdiilogParameters> * m_undecoratedLogger;
};

//------------------------------------------------------------
/**@private
 * @internal
 * @brief A class that is used to prevent prepend messages from being added too often */
template <typename QdiilogParameters>
struct UndecoratedLogger : public Logger<QdiilogParameters>
{
    UndecoratedLogger( Loglevel _level = Loglevel::error )
        :Logger<QdiilogParameters>( _level, true )
    {

    }
    virtual void prepend() override final {};
    bool isDecorated() const
    {
        return false;
    }
};

//------------------------------------------------------------
template <typename QdiilogParameters>
QdiilogParameters Logger<QdiilogParameters>::g_config;

//------------------------------------------------------------
/**@private
 * @internal
 * @brief A particular logger that outputs nothing */
template<typename QdiilogParameters>
struct OutputNull : public Logger<QdiilogParameters>
{
    static OutputNull nullOutput;
};

//------------------------------------------------------------
template <typename QdiilogParameters, typename T>
Logger<QdiilogParameters> & operator<<( OutputNull<QdiilogParameters> & _ostr , T )
{
    return _ostr;
}

//------------------------------------------------------------
template <typename QdiilogParameters>
OutputNull<QdiilogParameters> OutputNull<QdiilogParameters>::nullOutput;

//------------------------------------------------------------
template <typename QdiilogParameters, typename T>
Logger<QdiilogParameters> & operator<<( Logger<QdiilogParameters> & _logger, T&& _t )
{
    if( _logger.m_output )
    {
        bool canLog = false;

        switch( _logger.m_level )
        {
        case Loglevel::error:
            canLog |= ( _logger.g_config.filter_level == Loglevel::error );

        case Loglevel::warning:
            canLog |= ( _logger.g_config.filter_level == Loglevel::warning );

        case Loglevel::info:
            canLog |= ( _logger.g_config.filter_level == Loglevel::info );

        case Loglevel::trace:
            canLog |= ( _logger.g_config.filter_level == Loglevel::trace );

        case Loglevel::debug:
            canLog |= ( _logger.g_config.filter_level == Loglevel::debug );
            break;
        }

        if( canLog )
        {
            _logger.prepend();
            *( _logger.m_output ) << std::forward<T>( _t );
        }
    }

    return ( _logger.isDecorated() && _logger.m_undecoratedLogger ) ?
           *( _logger.m_undecoratedLogger ) : _logger;
}

//------------------------------------------------------------
template <typename QdiilogParameters>
Logger<QdiilogParameters> & Logger<QdiilogParameters>::operator()( bool _condition )
{
    return _condition ? *this : OutputNull<QdiilogParameters>::nullOutput;
}

//------------------------------------------------------------
Logger<QdiilogOstream> log_debug( Loglevel::debug );
Logger<QdiilogOstream> log_trace( Loglevel::trace );
Logger<QdiilogOstream> log_info( Loglevel::info );
Logger<QdiilogOstream> log_warning( Loglevel::warning );
Logger<QdiilogOstream> log_error( Loglevel::error );

//------------------------------------------------------------
inline
void setLogLevel( Loglevel _level )
{
    Logger<QdiilogOstream>::setLogLevel( _level );
}

//------------------------------------------------------------
inline
ErrorCode setOutput( QdiilogOstream::Output & _output )
{
    ErrorCode ret = log_debug.setOutput( _output );

    if( ret == OK )
        ret = log_trace.setOutput( _output );

    if( ret == OK )
        ret = log_info.setOutput( _output );

    if( ret == OK )
        ret = log_warning.setOutput( _output );

    if( ret == OK )
        ret = log_error.setOutput( _output );

    return ret;
}

//------------------------------------------------------------
inline
ErrorCode setPrependText( const std::string _prependText )
{
    ErrorCode ret = log_debug.setPrependText( _prependText );

    if( ret == OK )
        ret = log_trace.setPrependText( _prependText );

    if( ret == OK )
        ret = log_info.setPrependText( _prependText );

    if( ret == OK )
        ret = log_warning.setPrependText( _prependText );

    if( ret == OK )
        ret = log_error.setPrependText( _prependText );

    return ret;
}

//------------------------------------------------------------
inline
ErrorCode setPrependTextQdiiFlavour()
{
    ErrorCode ret = log_debug.setPrependText( "" );

    if( ret == OK )
        ret = log_trace.setPrependText( "" );

    if( ret == OK )
        ret = log_info.setPrependText( "[..] " );

    if( ret == OK )
        ret = log_warning.setPrependText( "[ww] " );

    if( ret == OK )
        ret = log_error.setPrependText( "[EE] " );

    return ret;
}

#ifdef QDIILOG_NAMESPACE
}
#endif //QDIILOG_NAMESPACE

#endif //QDIILOG_H
