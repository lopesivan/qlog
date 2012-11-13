#ifndef QLOG_HPP
#define QLOG_HPP

#include <ostream>
#include <vector>
#include <assert.h>

// windows color support
#ifdef WIN32
#	include <windows.h>
#endif

// let the user defines his own namespace
#ifndef QLOG_NAMESPACE
#   define QLOG_NAMESPACE qlog
#endif

// hide symbols on linux
#if __GNUC__ >= 4
#   pragma GCC visibility push(hidden)
#endif

namespace QLOG_NAMESPACE
{
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
template<typename T>
struct user_global_settings
{
    static unsigned loglevel;
	static bool initialized;

#ifdef WIN32
	static HANDLE console_handle;
	static console_function set_text_attribute;
#endif
};

// -------------------------------------------------------------------------- //
template<>
unsigned user_global_settings<int>::loglevel = loglevel::error;

template<>
bool user_global_settings<int>::initialized = false;

#ifdef WIN32
template<>
HANDLE user_global_settings<int>::console_handle = INVALID_HANDLE_VALUE;

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
 * @brief An object that logs messages */
template< unsigned level >
struct logger
{
    explicit
    logger( bool _disabled = false )
        :m_disabled( _disabled )
        ,m_output( 0 )
        ,m_prepend( 0 )
        ,m_append( 0 )
    {
        register_me( *this );
    }

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

    logger & operator=( const logger & );

    bool isDisabled() const { return m_disabled; }

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

    void set_output( std::ostream & _output )
    {
        m_output = &_output;
    }

    void append( const char * _txt )
    {
        m_append = _txt;
    }

    void prepend( const char * _txt )
    {
        m_prepend = _txt;
    }

    void signal_end() const
    {
        if( can_log() && m_append )
            ( *m_output ) << m_append;
    }

    void signal( standard_endline _func, bool first_message = false ) const
    {
        if( can_log() )
        {
            if( first_message && m_prepend )
                ( *m_output ) << m_prepend;

            _func( *m_output );
        }
    }

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
    bool can_log() const
    {
        return ( level >= get_loglevel() ) && m_output && !isDisabled();
    }

private:
    // this vectors permits to act on loggers of the same level belonging
    // to different compile units.
    static std::vector<logger *> * m_loggers;
    static void register_me( logger & _logger )
    {
        if( !m_loggers )
            m_loggers = new std::vector<logger *>();

        m_loggers->push_back( &_logger );
    }

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
    static void set_all_outputs( std::ostream & _new_output )
    {
        const typename std::vector<logger *>::iterator end = m_loggers->end();

        for( typename std::vector<logger *>::iterator logger = m_loggers->begin();
                logger != end; ++logger )
        {
            ( *logger )->set_output( _new_output );
        }
    }

};
// -------------------------------------------------------------------------- //
template< unsigned level >
std::vector<logger<level> *> * logger<level>::m_loggers;

// -------------------------------------------------------------------------- //
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
template< unsigned level, typename T > inline
receiver<level> operator << ( const logger<level> & _logger, const T & _message )
{
    return receiver<level>( &_logger ).treat( _message, true );
}

// -------------------------------------------------------------------------- //
template< unsigned level >
receiver<level> operator<<( const receiver<level> & _recv, standard_endline _func )
{
    _recv.signal( _func );
    return _recv;
}
// -------------------------------------------------------------------------- //
template< unsigned level >
receiver<level> operator<<( const logger<level> & _logger, standard_endline _func )
{
    _logger.signal( _func, true );
    return receiver<level>( &_logger );
}

// -------------------------------------------------------------------------- //

#ifndef QDIILOG_NAME_LOGGER_DEBUG
#   define QDIILOG_NAME_LOGGER_DEBUG debug
#endif
#ifndef QDIILOG_NAME_LOGGER_TRACE
#   define QDIILOG_NAME_LOGGER_TRACE trace
#endif
#ifndef QDIILOG_NAME_LOGGER_INFO
#   define QDIILOG_NAME_LOGGER_INFO info
#endif
#ifndef QDIILOG_NAME_LOGGER_WARNING
#   define QDIILOG_NAME_LOGGER_WARNING warning
#endif
#ifndef QDIILOG_NAME_LOGGER_ERROR
#   define QDIILOG_NAME_LOGGER_ERROR error
#endif

static logger<loglevel::debug> QDIILOG_NAME_LOGGER_DEBUG ;
static logger<loglevel::trace> QDIILOG_NAME_LOGGER_TRACE ;
static logger<loglevel::info> QDIILOG_NAME_LOGGER_INFO ;
static logger<loglevel::warning> QDIILOG_NAME_LOGGER_WARNING ;
static logger<loglevel::error> QDIILOG_NAME_LOGGER_ERROR ;

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
struct underline
{
};

// -------------------------------------------------------------------------- //
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
