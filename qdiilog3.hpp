#ifndef QLOG_HPP
#define QLOG_HPP

/**
 * @author Victor Lavaud <victor.lavaud@gmail.com>
 * @mainpage
 * DESCRIPTION
 * -----------
 * qlog is a header-only library that provides facilities to log messages
 * on screen or to a file. The messages can be written on screen or on
 * file. It is written is C++03 and is working with both Windows and Linux,
 * even though some decorative features might be silently turned off when
 * unavailable on either platform.
 *
 * SIMPLE USAGE
 * ------------
 * 5 objects are available to the user to log messages, they are:
 * - qlog::debug
 * - qlog::trace
 * - qlog::info
 * - qlog::warning
 * - qlog::error
 *
 * They work the same way std::cout does, for instance:
 * @code{.cpp}
 * qlog::warning << "foo() was passed a null pointer" << std::endl;
 * @endcode
 *
 * REDIRECTING TO A FILE
 * ---------------------
 * The objects will all log to std::cout by default, but you can change this
 * behaviour by calling set_output(). Actually, there are 2 set_output functions.
 * One of them is a member function that you can call on any log object, like
 * qlog::debug.set_output( new_output ), and the other one is a global function
 * that will call set_output on every available logger.
 * Note that set_output() takes a std::ostream reference as a parameter.
 *
 * The following snippet will redirect all output to myprogram.log
 * @code{.cpp}
 * #include <fstream>
 * #include "qlog.hpp"
 * using namespace qlog;
 *
 * int foo()
 * {
 *     return 1;
 * }
 *
 * int main()
 * {
 *     std::ofstream logstream( "myprogram.log" );
 *     set_output( logstream );
 *     int ret = foo();
 *     qlog::error << "foo() returned: " << ret << std::endl;
 * }
 *
 * @endcode
 *
 * Now if you want to redirect <i>the debug level only</i> to a different file, it is also
 * possible:
 * @code{.cpp}
 * #include <fstream>
 * #include "qlog.hpp"
 * using namespace qlog;
 * using std::ofstream;
 *
 * int foo()
 * {
 *     return 0;
 * }
 *
 * int main()
 * {
 *     set_loglevel( loglevel::debug );
 *
 *     // redirecting all the messages to myprogram log
 *	   ofstream logstream( "myprogram.log" );
 *     set_output( logstream );
 *
 *     // redirecting the debug messages to debug.log
 *     ofstream debug_stream( "debug.log" );
 *     qlog::debug.set_output( debug_stream );
 *
 *     // logging some messages
 *     qlog::debug << "calling foo()" << std::endl; // this will go in debug.log
 *     int ret = foo();
 *     qlog::info << "foo() returned: " << ret << std::endl; // this goes to myprogram.log
 * }
 *
 * @endcode
 *
 * FILTERING IMPORTANT MESSAGES IN
 * -------------------------------
 *
 * You can restrict the messages output to only the important ones by calling
 * the global function set_loglevel() with one of the following parameters:
 * - loglevel::debug
 * - loglevel::trace
 * - loglevel::info
 * - loglevel::warning
 * - loglevel::error
 * - loglevel::disable
 *
 * If you call <c>set_loglevel( loglevel::warning )</c>, only the error and warning
 * messages will be processed, the more detailed messages will be ignored.
 *
 * The special log level <c>loglevel::disable</c> will disable all output. No
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
 * @image html client.png
 * Here is an easy way to do that:
 * @code{.cpp}
 * qlog::info.prepend("[..] ");
 * qlog::warning.prepend("[ww] ");
 * qlog::error.prepend("[EE] ");
 * @endcode
 *
 *
 * NAMING AND NAMESPACES
 * ---------------------
 * I normally like to ditch my objects on the global namespace but some people
 * don't. I have created a couple preprocessor directives if you want
 * to personalize the name of the objects and retrict them into a namespace. By
 * default, 5 objects are created (qlog::debug, qlog::trace, qlog::info, qlog::warning
 * and qlog::error), and they are thrown into the global namespace. Now if you
 * want them to be called debug, trace, info, warning and error and to belong
 * to the namespace log, what you can do is:
 * @code{.cpp}
 * #define QLOG_NAMESPACE logging
 *
 * #define QLOG_NAME_LOGGER_DEBUG deb
 * #define QLOG_NAME_LOGGER_TRACE tra
 * #define QLOG_NAME_LOGGER_INFO inf
 * #define QLOG_NAME_LOGGER_WARNING warn
 * #define QLOG_NAME_LOGGER_ERROR err
 * #include "qlog.hpp"
 *
 * int main()
 * {
 *   logging::warn << "here we go!" << std::endl;
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
 *    qlog::warning << "problem!" << std::endl;
 * }
 * @endcode
 * You can write:
 * @code{.cpp}
 * qlog::warning( foo() != SUCCESS ) << "problem" << std::endl;
 * @endcode
 *
 * WRITING FANCY MESSAGES
 * ----------------------
 *
 * <h3>Adding some colors</h3>
 *
 * On both Windows and Linux, messages can be output in different colors.
 * The library provides a class called \c color which allows the user to change
 * the font color of the message he wants to output, as well as the background
 * color.
 *
 * @code{.cpp}
 * #include "qlog.hpp"
 * using namespace qlog;
 *
 * int main()
 * {
 *     qlog::error << color(red) << "Something went wrong" << color() << '\n';
 * }
 * @endcode
 *
 * Many colors are available, but not all of them are supported by Windows.
 * qlog namespace includes:
 * - black
 * - red
 * - green
 * - yellow
 * - blue
 * - magenta
 * - cyan
 * - white
 * - gray.
 *
 * They can all be used both a font color, or to fill the background.
 * For instance, to display a yellow message over a blue background, type:
 *
 * @code{.cpp}
 * #include "qlog.hpp"
 * using namespace qlog;
 *
 * int main()
 * {
 *     qlog::error << color(yellow, blue) << "Yellow message over blue" << color() << '\n';
 * }
 * @endcode
 *
 * You might have noticed that I used the special symbol @c qlog::color() which
 * role is to restore the terminal settings to default.
 *
 * <h3>Stressing important messages</h3>
 *
 * Both Windows and Linux can use a <b>bold font</b> to draw attention to particular
 * messages. To activate this function, add "true" as a color parameter just like this:
 *
 *
 * @code{.cpp}
 * #include "qlog.hpp"
 * using namespace qlog;
 *
 * int main()
 * {
 *     qlog::error << color(red, true) << "Something went wrong" << color() << '\n';
 * }
 * @endcode
 *
 * This will output message with bright red that you cannot miss.
 *
 * There are two
 * special and very  self explanatory symbols: <b>underline</b> and <b>blink</b>. Neither of
 * them work on Windows, unfortunately (but you can file a request at Microsoft's).
 * Xterm on linux would display both, but gnome-terminal does not support blink
 * (there is a bug opened on this since 2010).
 *
 * Here is an example on how to write a blinking line and underlined one.
 *
 * @code{.cpp}
 * #include "qlog.hpp"
 * using namespace qlog;
 *
 * int main()
 * {
 *     qlog::error << "This " << underline() << "word" << color() << " should be underlined" << '\n';
 *     qlog::error << "This " << blink() << "word" << color() << " should be blinking" << '\n';
 * }
 * @endcode
 *
 *
 */

#include <ostream>
#include <vector>
#include <assert.h>

// windows color support
#ifdef WIN32
#	include <windows.h>
#endif



// hide symbols on linux
#if __GNUC__ >= 4
#   pragma GCC visibility push(hidden)
#endif

// let the user defines his own namespace
#ifdef QLOG_NAMESPACE
namespace QLOG_NAMESPACE
{
#else
namespace qlog
{
#endif
// -------------------------------------------------------------------------- //
// the different levels of logging

namespace loglevel
{
static const unsigned disabled = 0;

static const unsigned debug = 1;
static const unsigned trace = 2;
static const unsigned info = 3;
static const unsigned warning = 4;
static const unsigned error = 5;
}

#ifdef WIN32
void *get_console_function(char *name) {
   static HMODULE kernel32=(HMODULE)0xffffffff;
   if(kernel32==0)
      return NULL;
   if(kernel32==(HMODULE)0xffffffff) {
      kernel32=LoadLibraryA("kernel32.dll");
      if(kernel32==0)
         return 0;
   }
   return GetProcAddress(kernel32,name);
}

typedef BOOL (WINAPI *console_function)(HANDLE hConsoleOutput, WORD attr);
#endif

// -------------------------------------------------------------------------- //
// hack to catch std::endl;
typedef std::basic_ostream<char, std::char_traits<char> > cout_type;
typedef cout_type & ( *standard_endline )( cout_type & );

// -------------------------------------------------------------------------- //

/**
 * @cond GENERATE_INTERNAL_DOCUMENTATION
 * @struct user_global_settings
 * @brief This structure is used to store information across the different compile units.
 */
template<typename T>
struct user_global_settings
{
    static unsigned loglevel; ///< The current level of logging
	static bool initialized;

#ifdef WIN32
	static HANDLE console_handle;
	static console_function set_text_attribute;
#endif
};

/** @endcond */
// -------------------------------------------------------------------------- //

/**@private
  *@brief The current level of logging */
template<>
unsigned user_global_settings<int>::loglevel = loglevel::error;

/**@private
  *@brief Whether the library is initialized */
template<>
bool user_global_settings<int>::initialized = false;

#ifdef WIN32
/**@private
  *@brief A handle to the console */
template<>
HANDLE user_global_settings<int>::console_handle = INVALID_HANDLE_VALUE;

/**@private
  *@brief A function pointer a function that will set console attributes */
template<>
console_function user_global_settings<int>::set_text_attribute = 0;
#endif

typedef user_global_settings<int> settings;

static
void set_loglevel( unsigned level )
{
    settings::loglevel = level;
}

static
unsigned get_loglevel()
{
    return settings::loglevel;
}

// -------------------------------------------------------------------------- //
/**@brief Initializes the library
 * @note This is only useful on Windows */
inline
void init()
{
	if (!settings::initialized)
	{
#		ifdef WIN32
		settings::console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
		settings::set_text_attribute = static_cast<console_function>(get_console_function("SetConsoleTextAttribute"));
#		endif

		settings::initialized = true;
	}
}

// -------------------------------------------------------------------------- //
/**@brief Terminates the library
 * @note This is only useful on Windows */
inline
void destroy()
{
	if (settings::initialized)
	{

#		ifdef WIN32
		settings::console_handle = 0;
		settings::set_text_attribute = 0;
#		endif

		settings::initialized = false;
	}
}

// -------------------------------------------------------------------------- //
/**@struct logger
 * @brief An object that logs messages
 * @tparam level The level of detail the object logs at */
template< unsigned level >
struct logger
{
    /**@brief Constructs a logger
     * @warning This should almost never been used. 5 loggers are already created
     *          by the library in each compile unit to be used by the user.
     * @param[in] _disabled Whether this logger actually writes messages */
    explicit
    logger( bool _disabled = false )
        :m_disabled( _disabled )
        ,m_output( 0 )
        ,m_prepend( 0 )
        ,m_append( 0 )
    {
        register_me( *this );
    }

    /**@brief Copies a logger
     * @warning This should almost never been used. 5 loggers are already created
     *          by the library in each compile unit to be used by the user.
     * @param[in] _logger The logger to copy */
    logger( const logger & _logger )
        :m_disabled( _logger.m_disabled )
        ,m_output( _logger.m_output )
        ,m_prepend( _logger.m_prepend )
        ,m_append( _logger.m_append )
    {
        register_me( *this );
    }

    ~logger()
    {
        unregister_me( *this );
    }

    /**@brief Assigns a logger
     * @param[in] _copy Another logger
     * @warning Assigning a logger is not permitted
     * @return *this */
    logger & operator=( const logger & _copy );

    /**@brief Checks if the logger is able to log
     * @return True if the logger can log */
    bool isDisabled() const { return m_disabled; }

    /**@brief Checks and outputs a message.
     * @warning This is automatically be called by @c operator<<
     * @param[in] _message The message to check and output
     * @param[in] _first_part Set to true if a custom message should be appended
     *
     * Checking whether the message can be output consists in:
     * - seeing whether the detail level of the logger is sufficient to output
     *   the message: an warning logger cannot output message when the logging
     *   level is set to error.
     * - checking whether an output stream has been set.
     * - checking whether the logger has been disabled during construction
     *
     * @see logger::logger(bool _disabled)
     */
    template< typename T >
    void treat( const T & _message, bool _first_part ) const
    {
        if( can_log() )
        {
            if( _first_part && m_prepend )
                ( *m_output ) << m_prepend;

            ( *m_output ) << _message;
        }
    }

    /**@brief Sets the destination of the messages
     * @param[in] _output A std::ostream object that will receive the next messages
     *
     * Use this function if you want a certain logger to write its message to
     * a customized output stream. The most frequent use is to direct messages
     * to a file.
     *
     * @code{.cpp}
     * #include <fstream>
     * #include <string>
     * #include "qlog.hpp"
     *
     * using namespace qlog;
     *
     * int main()
     * {
     *     const std::string filename("output.log");
     *     std::ofstream output_file( filename.c_str() );
     *
     *     qlog::error.set_output( output_file );
     *
     *     qlog::error << "This message will be writtin in output.log" << std::endl;
     * }
     * @endcode
     */
    void set_output( std::ostream & _output )
    {
        logger<level>::set_all_outputs( _output );
    }

    /**@brief Adds a custom text after all logged messages.
     * @param[in] _txt The text to append */
    void append( const char * _txt )
    {
        logger<level>::append_all( _txt );
    }

    /**@brief Adds a custom text before all logged messages
     * @param[in] _txt The text to prepend */
    void prepend( const char * _txt )
    {
        logger<level>::prepend_all( _txt );
    }

    /**@brief Informs the logger that the last message was treated and that custom text can be appended
     * @cond GENERATE_INTERNAL_DOCUMENTATION
     * @private */
    void signal_end() const
    {
        if( can_log() && m_append )
            ( *m_output ) << m_append;
    }

    /**@endcond*/

    /**@brief Informs the logger that std::endl, or other io manipulators, have been passed
     * @param[in] _func The io function, such as std::endl
     * @param[in] _first_message Whether it is the first element of the << series.
     * @private */
    void signal( standard_endline _func, bool _first_message = false ) const
    {
        if( can_log() )
        {
            if( _first_message && m_prepend )
                ( *m_output ) << m_prepend;

            _func( *m_output );
        }
    }

    /**@brief Conditional logging
     * @param[in] _cond Pass true to enable logging
     * @return A disabled logger if true has been passed, an enabled logger otherwise */
    logger operator()( bool _cond )
    {
        logger ret( *this );
        ret.m_disabled = !_cond;
        return ret;
    }

private:
    bool m_disabled;
    mutable std::ostream * m_output;
    const char * m_prepend;
    const char * m_append;

private:
    /**@brief Helper function to check that the logger can output messages
     * @cond GENERATE_INTERNAL_DOCUMENTATION
     * @private */
    bool can_log() const
    {
        return ( level >= get_loglevel() ) && m_output && !isDisabled();
    }

public:
    void change_output( std::ostream & _output )
    {
        m_output = &_output;
    }
    void change_prepend( const char * _txt )
    {
        m_prepend = _txt;
    }

    void change_append( const char * _txt )
    {
        m_append = _txt;
    }

    /**@endcond */

private:
    // this vectors permits to act on loggers of the same level belonging
    // to different compile units.
    static std::vector<logger *> * m_loggers;

    /**
     * @cond GENERATE_INTERNAL_DOCUMENTATION
     * @brief Adds a logger to a list of logger
     * @param[in] _logger The logger to add to the list of loggers
     *
     * Each compile unit contains 5 loggers, one for debug, one for trace, one
     * for info, etc. This vector contains all the logger instances for the
     * debug level, or the info level, so that when any compile unit calls
     * qlog::info.set_output(), the loggers of log level info, in every compile
     * unit, direct their output to the file.
     */
    static void register_me( logger & _logger )
    {
        if( !m_loggers )
            m_loggers = new std::vector<logger *>();

        m_loggers->push_back( &_logger );
    }

    /**
     * @brief Adds a logger to a list of loggers
     * @param[in] _logger The logger to remove from the list of loggers
     * @see register_me
     */
    static void unregister_me( logger & _logger )
    {
        typename std::vector<logger *>::iterator it = m_loggers->begin();
        const typename std::vector<logger *>::iterator end = m_loggers->end();

        for( ; it != end; ++it )
        {
            if( *it == &_logger )
                break;
        }

        assert( it != end );
        m_loggers->erase( it );

        if( m_loggers->empty() )
        {
            delete m_loggers;
            m_loggers = 0;
        }
    }

public:
    /**
     * @brief Changes the output of all the loggers from all the compile units
     * @param[in] _new_output A std::ostream that will be set in all loggers of a given log level
     */
    static void set_all_outputs( std::ostream & _new_output )
    {
        const typename std::vector<logger *>::iterator end = m_loggers->end();
        for( typename std::vector<logger *>::iterator logger = m_loggers->begin();
                logger != end; ++logger )
        {
            ( *logger )->change_output( _new_output );
        }
    }

    static void prepend_all( const char * _txt )
    {
        const typename std::vector<logger *>::iterator end = m_loggers->end();
        for( typename std::vector<logger *>::iterator logger = m_loggers->begin();
                logger != end; ++logger )
        {
            ( *logger )->change_prepend( _txt );
        }
    }

    static void append_all( const char * _txt )
    {
        const typename std::vector<logger *>::iterator end = m_loggers->end();
        for( typename std::vector<logger *>::iterator logger = m_loggers->begin();
                logger != end; ++logger )
        {
            ( *logger )->change_append( _txt );
        }
    }
    /** @endcond */
};
// -------------------------------------------------------------------------- //
template< unsigned level >
std::vector<logger<level> *> * logger<level>::m_loggers;

// -------------------------------------------------------------------------- //
/**@struct receiver
 * @cond GENERATE_INTERNAL_DOCUMENTATION
 * @brief A light-weight structure that receives a stream.
 *
 * The purpose of receiver is to detect when a series of operator<< ends.
 * Let’s consider this example:
 * logger << "value = " << value;
 *
 * I want to append a custom text so that the written message
 * looks something like "[..] value = 35\n", which means that I have to add
 * "\n" at some point. receiver is a helper structure that will let us know
 * about that.
 *
 * When logger << "value = " is first proceeded, it calls
 * operator<<( const logger &, std::string & ), which returns a temporary receiver
 * object.
 *
 * Then the second part of the message is treated: "<< value". This time, the
 * function operator << (const receiver &, int) is called. It treated the value
 * and returns a new temporary receiver.
 *
 * As there is no other message to treat, this last temporary receiver dies.
 * Its destructor inform the logger object that a custom text should be appended.
 * The logger object is not called from the destructor if any operator<< has been
 * called on the temporary receiver.
 */
template< unsigned level >
struct receiver
{
    explicit
    receiver( const logger<level> * _logger, bool _muted = false )
        :m_treated( false )
        ,m_logger( _logger )
        ,m_muted( _muted )
    {}

    receiver( const receiver & _copy )
        :m_treated( false )
        ,m_logger( _copy.m_logger )
        ,m_muted( _copy.m_muted )
    {}

    ~receiver()
    {
        if( !m_treated )
            m_logger->signal_end();
    }

    bool is_muted() const { return m_muted; }
    void signal( standard_endline _func ) const
    {
        m_treated = true;

        if( !is_muted() )
        {
            m_logger->signal( _func );
        }
    }

    template< typename T >
    receiver treat( const T & _message, bool _first_part ) const
    {
        m_treated = true;

        if( !m_muted )
            m_logger->treat( _message, _first_part );

        return *this;
    }

private:
    receiver operator=( const receiver & );

private:
    mutable bool m_treated;
    const logger<level> * m_logger;
    mutable bool m_muted;
};

// -------------------------------------------------------------------------- //
template< unsigned level,typename T > inline
receiver<level> operator<<( const receiver<level> & _receiver,  const T & _message )
{
    return _receiver.treat( _message, false );
}

// -------------------------------------------------------------------------- //
template< unsigned level >
receiver<level> operator<<( const receiver<level> & _recv, standard_endline _func )
{
    _recv.signal( _func );
    return _recv;
}

// -------------------------------------------------------------------------- //
template< unsigned level, typename T > inline
receiver<level> operator << ( const logger<level> & _logger, const T & _message )
{
    return receiver<level>( &_logger ).treat( _message, true );
}

// -------------------------------------------------------------------------- //
template< unsigned level >
receiver<level> operator<<( const logger<level> & _logger, standard_endline _func )
{
    _logger.signal( _func, true );
    return receiver<level>( &_logger );
}
/** @endcond */
// -------------------------------------------------------------------------- //

#ifndef QLOG_NAME_LOGGER_DEBUG
#   define QLOG_NAME_LOGGER_DEBUG debug
#endif
#ifndef QLOG_NAME_LOGGER_TRACE
#   define QLOG_NAME_LOGGER_TRACE trace
#endif
#ifndef QLOG_NAME_LOGGER_INFO
#   define QLOG_NAME_LOGGER_INFO info
#endif
#ifndef QLOG_NAME_LOGGER_WARNING
#   define QLOG_NAME_LOGGER_WARNING warning
#endif
#ifndef QLOG_NAME_LOGGER_ERROR
#   define QLOG_NAME_LOGGER_ERROR error
#endif

static logger<loglevel::debug> QLOG_NAME_LOGGER_DEBUG ;
static logger<loglevel::trace> QLOG_NAME_LOGGER_TRACE ;
static logger<loglevel::info> QLOG_NAME_LOGGER_INFO ;
static logger<loglevel::warning> QLOG_NAME_LOGGER_WARNING ;
static logger<loglevel::error> QLOG_NAME_LOGGER_ERROR ;

inline
void set_output( std::ostream & _new_output )
{
    logger< loglevel::debug>::set_all_outputs( _new_output );
    logger< loglevel::trace>::set_all_outputs( _new_output );
    logger< loglevel::info>::set_all_outputs( _new_output );
    logger< loglevel::warning>::set_all_outputs( _new_output );
    logger< loglevel::error>::set_all_outputs( _new_output );
}

// -------------------------------------------------------------------------- //

static const unsigned black = 1;
static const unsigned red = 2;
static const unsigned green = 3;
static const unsigned yellow = 4;
static const unsigned blue = 5;
static const unsigned magenta = 6;
static const unsigned cyan = 7;
static const unsigned white = 8;
static const unsigned gray = 9;

// -------------------------------------------------------------------------- //
/**@struct underline
 * @brief A manipulator to underline text
 * @note Underlining is ignored on windows
 *
 * An underline object can be used to tell the console that the following text
 * should be underlined. Use @b color() to disable underlining. This example
 * shows a simple underlined word.
 *
 * @code{.cpp}
 * #include <iostream>
 * #include "qlog.hpp"
 * using namespace qlog;
 *
 * int main()
 * {
 *     qlog::set_output( std::cout );
 *     qlog::error << "This " << underline() << "word" << color() << " should be underlined" << '\n';
 * }
 * @endcode
 * This will output the following:
 * @image html underline.png
 *
 */
struct underline
{
};

// -------------------------------------------------------------------------- //
/**@struct blink
 * @brief A manipulator to make text blink
 * @note Blinking is ignored on windows, and not supported on all terminals
 *
 * An underline object can be used to tell the console that the following text
 * should be blinking. Use @b color() to disable blinking. This example
 * shows a simple blinking word.
 *
 * @code{.cpp}
 * #include <iostream>
 * #include "qlog.hpp"
 * using namespace qlog;
 *
 * int main()
 * {
 *     qlog::set_output( std::cout );
 *     qlog::error << "This " << blink() << "word" << color() << " should be underlined" << '\n';
 * }
 * @endcode
 *
 * This will make the word blink.
 *
 */
struct blink
{
};


#ifndef WIN32
struct color
{
    color()
        :m_foreground("\033[0m")
        ,m_background("")
        ,m_bold("")
    {
    }

    explicit
    color( unsigned _foreground, bool _bold = false )
        :m_foreground("\033[0m")
        ,m_background("")
        ,m_bold( _bold ? "\033[1m" : "" )
    {
        switch( _foreground )
        {
        case black: m_foreground = "\033[30m"; break;
        case red: m_foreground = "\033[31m"; break;
        case green: m_foreground = "\033[32m"; break;
        case yellow: m_foreground = "\033[33m"; break;
        case blue: m_foreground = "\033[34m"; break;
        case magenta: m_foreground = "\033[35m"; break;
        case cyan: m_foreground = "\033[36m"; break;
        case white: m_foreground = "\033[37m"; break;
        }
    }

    color( unsigned _foreground, unsigned _background, bool _bold = false )
        :m_foreground("")
        ,m_background("")
        ,m_bold( _bold ? "\033[1m" : "" )
    {
        switch( _foreground )
        {
        case black: m_foreground = "\033[30;"; break;
        case red: m_foreground = "\033[31;"; break;
        case green: m_foreground = "\033[32;"; break;
        case yellow: m_foreground = "\033[33;"; break;
        case blue: m_foreground = "\033[34;"; break;
        case magenta: m_foreground = "\033[35;"; break;
        case cyan: m_foreground = "\033[36;"; break;
        case white: m_foreground = "\033[37;"; break;
        }

        switch( _background )
        {
        case black: m_background = "40m"; break;
        case red: m_background = "41m"; break;
        case green: m_background = "42m"; break;
        case yellow: m_background = "43m"; break;
        case blue: m_background = "44m"; break;
        case magenta: m_background = "45m"; break;
        case cyan: m_background = "46m"; break;
        case white: m_background = "47m"; break;
        }
    }

    const char * getBold() const { return m_bold; }
    const char * getBackground() const { return m_background; }
    const char * getForeground() const { return m_foreground; }

private:
    const char * m_foreground;
    const char * m_background;
    const char * m_bold;
};

template<unsigned level>
receiver<level> operator <<( const logger<level> & _logger, const color & _color )
{
    return _logger << _color.getBold() << _color.getForeground() << _color.getBackground();
}

template<unsigned level>
receiver<level> operator <<( const receiver<level> & _recv, const color & _color )
{
    return _recv << _color.getBold() << _color.getForeground() << _color.getBackground();
}

// -------------------------------------------------------------------------- //
template<unsigned level>
receiver<level> operator <<( const logger<level> & _logger, const underline & )
{
    return _logger << "\e[4m";
}

template<unsigned level>
receiver<level> operator <<( const receiver<level> & _recv, const underline & )
{
    return _recv << "\e[4m";
}

// -------------------------------------------------------------------------- //
template<unsigned level>
receiver<level> operator <<( const logger<level> & _logger, const blink & )
{
    return _logger << "\e[5m";
}

template<unsigned level>
receiver<level> operator <<( const receiver<level> & _recv, const blink & )
{
    return _recv << "\e[5m";
}

#else // WIN32

struct color
{
	color()
		:m_attributes(0)
	{
		setForeground( white, false );
		setBackground( black );
	}

	explicit
	color(unsigned _foreground, bool _bold = false)
		:m_attributes(0)
	{
		setForeground( _foreground, _bold );
	}

	color(unsigned _foreground, unsigned _background, bool _bold = false)
		:m_attributes(0)
	{
		setForeground( _foreground, _bold );
		setBackground( _background );
	}

	WORD getAttributes() const { return m_attributes; }

private:
	void setForeground(unsigned _color, bool _bold)
	{
		switch( _color )
		{
		case black: m_attributes = 0; break;
		case blue:	m_attributes = FOREGROUND_BLUE; break;
		case green: m_attributes = FOREGROUND_GREEN; break;
		case red:	m_attributes = FOREGROUND_RED; break;
		case yellow: m_attributes = FOREGROUND_GREEN | FOREGROUND_RED; break;
		case gray: m_attributes = FOREGROUND_INTENSITY; break;
		case magenta: m_attributes = FOREGROUND_BLUE | FOREGROUND_RED; break;

		default:
		case white: m_attributes = FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN; break;
		}
		if (_bold)
			m_attributes |= FOREGROUND_INTENSITY;
	}

	void setBackground(unsigned _color)
	{
		WORD bgAttributes = 0;
		switch(_color)
		{
		case white: bgAttributes |= BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_GREEN;
		case blue:	bgAttributes |= BACKGROUND_BLUE; break;
		case red:	bgAttributes |= BACKGROUND_RED; break;
		case green: bgAttributes |= BACKGROUND_GREEN; break;
		case gray:	bgAttributes |= BACKGROUND_INTENSITY; break;
		case yellow: bgAttributes |= BACKGROUND_GREEN | BACKGROUND_RED; break;
		case magenta: bgAttributes |= BACKGROUND_BLUE | BACKGROUND_RED; break;

		default:
		case black: bgAttributes |= 0; break;
		}

		m_attributes |= bgAttributes;
	}
	WORD m_attributes;
};

template<unsigned level>
receiver<level> operator <<( const logger<level> & _logger, const color & _color )
{
	assert(settings::set_text_attribute && settings::console_handle);
	settings::set_text_attribute( settings::console_handle, _color.getAttributes() );
    return receiver<level>( &_logger ).treat("", true);
}

template<unsigned level>
receiver<level> operator <<( const receiver<level> & _recv, const color & _color )
{
    assert(settings::set_text_attribute && settings::console_handle);
	settings::set_text_attribute( settings::console_handle, _color.getAttributes() );
	return _recv.treat("", false);
}

// -------------------------------------------------------------------------- //
template<unsigned level>
receiver<level> operator <<( const logger<level> & _logger, const underline & )
{
    return receiver<level>( &_logger ).treat("", true);
}

template<unsigned level>
receiver<level> operator <<( const receiver<level> & _recv, const underline & )
{
	return _recv.treat("", false);
}
// -------------------------------------------------------------------------- //
template<unsigned level>
receiver<level> operator <<( const logger<level> & _logger, const blink & )
{
    return receiver<level>( &_logger ).treat("", true);
}

template<unsigned level>
receiver<level> operator <<( const receiver<level> & _recv, const blink & )
{
	return _recv.treat("", false);
}

#endif

} // namespace

#if __GNUC__ >= 4
#   pragma GCC visibility pop
#endif

#endif // QLOG_HPP
