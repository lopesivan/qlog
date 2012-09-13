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
 * - Loglevel::disable
 *
 * If you call <c>setLogLevel( Loglevel::warning )</c>, only the error and warning
 * messages will be processed, the more detailed messages will be ignored.
 *
 * The special log level <c>Loglevel::disable</c> will disable all output. No
 * messages will be written after this has been set.
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
 * NAMING AND NAMESPACES
 * ---------------------
 * I normally like to ditch my objects on the global namespace but some people
 * just don't. I have created a couple preprocessor directives if you want
 * to personalize the name of the objects and retrict them into a namespace. By
 * default, 5 objects are created (log_debug, log_trace, log_info, log_warning
 * and log_error), and they are thrown into the global namespace. Now if you
 * want them to be called debug, trace, info, warning and error and to belong
 * to the namespace log, what you can do is:
 * @code{.cpp}
 * #define QDIILOG_NAMESPACE log
 * #define QDIILOG_NAME_LOGGER_DEBUG debug
 * #define QDIILOG_NAME_LOGGER_TRACE trace
 * #define QDIILOG_NAME_LOGGER_INFO info
 * #define QDIILOG_NAME_LOGGER_WARNING warning
 * #define QDIILOG_NAME_LOGGER_ERROR error
 * #include "qdiilog.hpp"
 *
 * int main()
 * {
 *   log::info << "here we go!" << std::endl;
 *   return 0;
 * }
 * @endcode
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
#include <sstream>
#include <utility> // std forward
#include <vector>
#include <algorithm> 

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
    error,

    disable
};

static const ErrorCode          OK                  = 0;

#define QDIILOG_DECL_HIDDEN __attribute__ ((visibility("hidden")))
#define QDIILOG_DECL_VISIBLE __attribute__ ((visibility("default")))

#ifndef QDIILOG_NAME_LOGGER_DEBUG
#   define QDIILOG_NAME_LOGGER_DEBUG log_debug
#endif
#ifndef QDIILOG_NAME_LOGGER_TRACE
#   define QDIILOG_NAME_LOGGER_TRACE log_trace
#endif
#ifndef QDIILOG_NAME_LOGGER_INFO
#   define QDIILOG_NAME_LOGGER_INFO log_info
#endif
#ifndef QDIILOG_NAME_LOGGER_WARNING
#   define QDIILOG_NAME_LOGGER_WARNING log_warning
#endif
#ifndef QDIILOG_NAME_LOGGER_ERROR
#   define QDIILOG_NAME_LOGGER_ERROR log_error
#endif

//------------------------------------------------------------
struct QDIILOG_DECL_HIDDEN Traits;
/**@private
 * @internal
 * @brief A class that is used as template parameter for the loggers */
struct Traits
{
    Loglevel filter_level;

    Traits()
        :filter_level( Loglevel::error )
    {
    }
};

//------------------------------------------------------------
/**@private
 * @internal
 * @brief A class that is used to prevent prepend messages from being added
 *        too often */
template <typename Traits>
struct UndecoratedLogger;

//------------------------------------------------------------
/**@private
 * @internal
 * @brief A class that is used to prevent messages from being written altogether */
template <typename Traits>
struct MutedLogger;

//------------------------------------------------------------
/**@private
 * @internal
 * @struct LoggerInterface
 * @brief Groups all the commands available to the user
 */
struct LoggerInterface
{
    LoggerInterface() {}
    virtual ~LoggerInterface() {}

    virtual ErrorCode setPrependText( const std::string & _prependText ) = 0;
    virtual void updateOutput( std::ostream & _newOutput ) = 0;

};

//------------------------------------------------------------
/**@private
 * @internal
 * @struct LoggerImplementation
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
template <typename T>
struct LoggerImplementation : public LoggerInterface
{
    /**@brief Constructs a Logger
     * @param[in] _level The level of logging. */
    explicit LoggerImplementation( Loglevel _level = Loglevel::error )
        :m_level( _level )
        ,m_output( nullptr )
        ,m_userPrepend()
        ,m_undecoratedLogger( nullptr )
        ,m_wrappers()
    {
    }

    /**@brief Constructs a Logger
     * @private
     * @param[in] _level The level of logging.
     * @param[in] _notDecorated Internal use, don’t touch. */
    LoggerImplementation( Loglevel _level, bool _notDecorated)
        :m_level( _level )
        ,m_output( nullptr )
        ,m_userPrepend()
        ,m_undecoratedLogger( nullptr )
        ,m_wrappers()
    {
        if( !_notDecorated )
            m_undecoratedLogger = new UndecoratedLogger<T>( _level );
    }

    /**@brief Destructs a Logger */
    virtual ~LoggerImplementation()
    {
        delete m_undecoratedLogger;
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
    ErrorCode setPrependText( const std::string & _prependText ) override
    {
        m_userPrepend.assign( _prependText );
        return OK;
    }

    /**@private
     * @brief Prepends _u with user data if needed, and return the string */
    template<typename U>
    std::string processLog( U&& _u )
    {
        std::ostringstream ret;

        if( m_output && canLog() )
        {
            prepend( ret );
            ret << std::forward<U>( _u );
        }

        return ret.str();
    }
    
    /**@brief Check if the level of detail of the logger is sufficient to log
     * @return true if logging will happen, else otherwise */
    bool canLog() const
    {
        bool doesLevelAllowLogging = false;

            switch( m_level )
            {
            case Loglevel::error:
                doesLevelAllowLogging |= ( g_config.filter_level == Loglevel::error );

            case Loglevel::warning:
                doesLevelAllowLogging |= ( g_config.filter_level == Loglevel::warning );

            case Loglevel::info:
                doesLevelAllowLogging |= ( g_config.filter_level == Loglevel::info );

            case Loglevel::trace:
                doesLevelAllowLogging |= ( g_config.filter_level == Loglevel::trace );

            case Loglevel::debug:
                doesLevelAllowLogging |= ( g_config.filter_level == Loglevel::debug );
                break;

            case Loglevel::disable:
                doesLevelAllowLogging = false;
                break;
            }
        return doesLevelAllowLogging;
    }

private:
    LoggerImplementation( LoggerImplementation & _logger ) = delete;
    LoggerImplementation & operator=( LoggerImplementation & _logger ) = delete;

private:
    Loglevel    m_level;
    static T    g_config;
    
    std::ostream * m_output;

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
    virtual void prepend( std::ostringstream & output_ )
    {
        if( m_userPrepend.size() && m_output )
        {
            output_ << m_userPrepend;
        }
    }

    std::string m_userPrepend;
    UndecoratedLogger<T> * m_undecoratedLogger;
    
private:
    std::vector< LoggerInterface * > m_wrappers;

public:
    /**@brief Registers a new wrapper */
    void registerWrapper( LoggerInterface & _interface)
    {
        m_wrappers.push_back( &_interface );
    }
    
    void unregisterWrapper( LoggerInterface & _interface)
    {
        m_wrappers.erase( std::remove(m_wrappers.begin(), m_wrappers.end(), &_interface), m_wrappers.end() );
    }
    
    void updateOutput( std::ostream & _newOutput ) override
    {
        m_output = &_newOutput;
        for (auto wrapper : m_wrappers)
        {
            (*wrapper).updateOutput( _newOutput );
        }
    }
};

//------------------------------------------------------------
/**@private
 * @internal
 * @brief A class that is used to prevent prepend messages from being added too often */
template <typename T>
struct UndecoratedLogger : public LoggerImplementation<T>
{
    UndecoratedLogger( Loglevel _level = Loglevel::error )
        :LoggerImplementation<T>( _level, true, false )
    {

    }
    virtual void prepend(std::ostringstream &) override final {};
    bool isDecorated() const
    {
        return false;
    }
};

//------------------------------------------------------------
template <typename T>
T LoggerImplementation<T>::g_config;

//------------------------------------------------------------
/**@struct Logger
 * @brief Provides logging
 * @implements LoggerInterface
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
template<Loglevel LEVEL>
struct Logger : public std::ostream, public LoggerInterface
{
    /**@brief Constructs a Logger
     * @private
     * @internal
     * @param[in] _muted Whether this logger should output anything
     */
    Logger(bool _muted = false)
        :m_muted(_muted)
    {
        getSingleton().registerWrapper(*this);
    }
    
    /**@brief Destruct a logger
     * @private
     * @internal
     */
    ~Logger()
    {
        getSingleton().unregisterWrapper( *this );
    }
    
    /**@brief Get the static implementation 
     * @private
     * @internal
     */
    static LoggerImplementation<Traits> & getSingleton()
    {
        static LoggerImplementation<Traits> logger( LEVEL );
        return logger;
    }

    /**@brief Indicates where the messages will be written
     * @return OK
     * @param[in] _output an std::ostream into which the messages will be written
     */
    ErrorCode setOutput( std::ostream & _output )
    {
        getSingleton().updateOutput( _output );
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
     * @public
     * @return OK if everything is fine.
     * @warning Calling setPrependText many times cancels the text previously
     *          set */
    ErrorCode setPrependText( const std::string & _prependText ) override
    {
        return getSingleton().setPrependText( _prependText );
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
     * @return *this
     * @param[in] _condition The logging will be enabled only if
     *            _condition is true */
    Logger && operator()( bool _condition )
    {
        static Logger nullLogger(true);
        
        if( _condition && getSingleton().canLog() )
            return std::move( *this );
        else
            return std::move( nullLogger );
    }
    
    /**@private
     * @internal
     * @brief Sets a new output to the parent stream object
     * @param[in] _output The output to redirect to
     */
    void updateOutput( std::ostream & _output ) override
    {
        rdbuf( _output.rdbuf() );
    }

    /**@private
     * @internal 
     * @brief Checks if the stream is muted 
     * @return true if the stream is muted, false if it is not */
    bool isMuted() const
    {
        return m_muted;
    }
    
private:
    bool m_muted;
};
//------------------------------------------------------------

template<Loglevel LEVEL, typename T>
Logger<LEVEL> & operator<<( Logger<LEVEL> && _wrapper, T && _param )
{
    if (!_wrapper.isMuted())
    {
        std::string && logString = _wrapper.getSingleton().processLog( _param );
        static_cast<std::ostream&>( _wrapper ) << logString;
    }
    return _wrapper;
}

//------------------------------------------------------------

template<Loglevel LEVEL, typename T>
Logger<LEVEL> & operator<<( Logger<LEVEL> & _wrapper, T && _param )
{
    if (!_wrapper.isMuted())
    {
        std::string && logString = _wrapper.getSingleton().processLog( _param );
        static_cast<std::ostream&>( _wrapper ) << logString;
    }
    return _wrapper;
}

//------------------------------------------------------------
static Logger<Loglevel::debug>    QDIILOG_NAME_LOGGER_DEBUG ;
static Logger<Loglevel::trace>    QDIILOG_NAME_LOGGER_TRACE ;
static Logger<Loglevel::info>     QDIILOG_NAME_LOGGER_INFO ;
static Logger<Loglevel::warning>  QDIILOG_NAME_LOGGER_WARNING ;
static Logger<Loglevel::error>    QDIILOG_NAME_LOGGER_ERROR ;

//------------------------------------------------------------
/**@brief Sets the level of detail
 * @param[in] _level is the granularity of the logging.
 * It can take 5 values:
 * - Loglevel::debug
 * - Loglevel::trace
 * - Loglevel::info
 * - Loglevel::warning
 * - Loglevel::error
 *
 * By changing the level of detail of the logging, the user prevents too
 * detailed messages from being output. For instance, if the level of
 * detail is Loglevel::warning, nothing but warning and error messages will
 * be output.
 *
 * @code{.cpp}
 * int main()
 * {
 *   setLogLevel( Loglevel::warning );
 *   log_debug << "this will not be written" << std::endl;
 *   log_warning << "but this will" << std::endl;
 *   log_error << "and that also" << std::endl;
 *
 *   return 0;
 * }
 * @endcode
 */
inline
void setLogLevel( Loglevel _level )
{
    LoggerImplementation<Traits>::setLogLevel( _level );
}

//------------------------------------------------------------

/**@brief Redirects the output of all the loggers
 * @param[in] _output The std::ostream where the loggers should
 *                    write their messages
 * @return OK if everything goes well.
 *
 * This causes all the loggers to output their messages on another ostream
 * than the previous one. This is exactly the same thing as calling the member
 * function setOutput on the 5 loggers.
 */
inline
ErrorCode setOutput( std::ostream & _output )
{
    ErrorCode ret = QDIILOG_NAME_LOGGER_DEBUG .setOutput( _output );

    if( ret == OK )
        ret = QDIILOG_NAME_LOGGER_TRACE .setOutput( _output );

    if( ret == OK )
        ret = QDIILOG_NAME_LOGGER_INFO .setOutput( _output );

    if( ret == OK )
        ret = QDIILOG_NAME_LOGGER_WARNING .setOutput( _output );

    if( ret == OK )
        ret = QDIILOG_NAME_LOGGER_ERROR .setOutput( _output );

    return ret;
}

//------------------------------------------------------------
inline
ErrorCode setPrependText( const std::string & _prependText )
{
    ErrorCode ret = QDIILOG_NAME_LOGGER_DEBUG .setPrependText( _prependText );

    if( ret == OK )
        ret = QDIILOG_NAME_LOGGER_TRACE .setPrependText( _prependText );

    if( ret == OK )
        ret = QDIILOG_NAME_LOGGER_INFO .setPrependText( _prependText );

    if( ret == OK )
        ret = QDIILOG_NAME_LOGGER_WARNING .setPrependText( _prependText );

    if( ret == OK )
        ret = QDIILOG_NAME_LOGGER_ERROR .setPrependText( _prependText );

    return ret;
}

//------------------------------------------------------------
inline
ErrorCode setPrependTextQdiiFlavour()
{
    ErrorCode ret = QDIILOG_NAME_LOGGER_DEBUG .setPrependText( "" );

    if( ret == OK )
        ret = QDIILOG_NAME_LOGGER_TRACE .setPrependText( "" );

    if( ret == OK )
        ret = QDIILOG_NAME_LOGGER_INFO .setPrependText( "[..] " );

    if( ret == OK )
        ret = QDIILOG_NAME_LOGGER_WARNING .setPrependText( "[ww] " );

    if( ret == OK )
        ret = QDIILOG_NAME_LOGGER_ERROR .setPrependText( "[EE] " );

    return ret;
}

//------------------------------------------------------------
enum BashColor
{
    NONE = 0,
    BLACK, RED, GREEN,
    YELLOW, BLUE, MAGENTA,
    CYAN, WHITE
};

//------------------------------------------------------------
static
std::string set_color( BashColor _foreground = NONE, BashColor _background = NONE )
{
    std::ostringstream transform;
    transform << "\033[";

    if( !_foreground && ! _background ) transform << "0"; // reset colors if no params

    if( _foreground != NONE )
    {
        transform << 29 + _foreground;

        if( _background ) transform << ";";
    }

    if( _background != NONE )
    {
        transform << 39 + _background;
    }

    transform << "m";
    return transform.str();
}

//------------------------------------------------------------
inline
ErrorCode setPrependedTextQdiiFlavourBashColors()
{
    ErrorCode ret = QDIILOG_NAME_LOGGER_DEBUG .setPrependText( "" );

    if( ret == OK )
        ret = QDIILOG_NAME_LOGGER_TRACE .setPrependText( "" );

    if( ret == OK )
        ret = QDIILOG_NAME_LOGGER_INFO .setPrependText( "[..] " );

    if( ret == OK )
        ret = QDIILOG_NAME_LOGGER_WARNING .setPrependText( std::string( "[" ) + set_color( GREEN ) + "ww" + set_color( NONE ) + "] " );

    if( ret == OK )
        ret = QDIILOG_NAME_LOGGER_ERROR .setPrependText( std::string( "[" ) + set_color( RED ) + "EE" + set_color( NONE ) + "] " );

    return ret;
}

#ifdef QDIILOG_NAMESPACE
}
#endif //QDIILOG_NAMESPACE

#endif //QDIILOG_H
