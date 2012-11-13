#ifndef QLOG_HPP
#define QLOG_HPP

#include <ostream>
#include <vector>
#include <assert.h>

// let the user defines his own namespace
#ifndef QLOG_NAMESPACE
#   define QLOG_NAMESPACE qlog
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

// -------------------------------------------------------------------------- //
// hack to catch std::endl;
typedef std::basic_ostream<char, std::char_traits<char> > cout_type;
typedef cout_type & ( *standard_endline )( cout_type & );

// -------------------------------------------------------------------------- //
template<typename T>
struct user_global_settings
{
    static unsigned loglevel;
};

// -------------------------------------------------------------------------- //
template<>
unsigned user_global_settings<int>::loglevel = loglevel::error;

static
void set_loglevel( unsigned level )
{
    user_global_settings<int>::loglevel = level;
}

static
unsigned get_loglevel()
{
    return user_global_settings<int>::loglevel;
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

namespace bash
{
static const unsigned black = 1;
static const unsigned red = 2;
static const unsigned green = 3;
static const unsigned yellow = 4;
static const unsigned blue = 5;
static const unsigned magenta = 6;
static const unsigned cyan = 7;
static const unsigned white = 8;

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
        ,m_bold( _bold ? "\033[1m" : "\033[0m" )
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
        ,m_bold( _bold ? "\033[1m" : "\033[0m" )
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
        case black: m_background = "\033[40m"; break;
        case red: m_background = "\033[41m"; break;
        case green: m_background = "\033[42m"; break;
        case yellow: m_background = "\033[43m"; break;
        case blue: m_background = "\033[44m"; break;
        case magenta: m_background = "\033[45m"; break;
        case cyan: m_background = "\033[46m"; break;
        case white: m_background = "\033[47m"; break;
        }
    }

    const char * getBold() const { return m_bold; }
    const char * getBackground() const { return m_background; }
    const char * getForeground() const { return m_foreground; }

private:
    const char * m_bold;
    const char * m_foreground;
    const char * m_background;
};

} // namespace bash

template<unsigned level>
receiver<level> operator <<( const logger<level> & _logger, const bash::color & _color )
{
    return _logger << _color.getBold() << _color.getForeground() << _color.getBackground();
}

template<unsigned level>
receiver<level> operator <<( const receiver<level> & _recv, const bash::color & _color )
{
    return _recv << _color.getBold() << _color.getForeground() << _color.getBackground();
}

} // namespace
#endif // QLOG_HPP
