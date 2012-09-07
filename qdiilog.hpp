#ifndef QDIILOG_H
#define QDIILOG_H

#include <iostream>

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

#ifdef QDIILOG_AS_A_DLL
#   define QDIILOG_DECL QDIILOG_DECL_VISIBLE
#else
#   define QDIILOG_DECL QDIILOG_DECL_HIDDEN
#endif // QDIILOG_AS_A_DLL

template <typename QdiiLogParameters>
ErrorCode qdiilog_init() QDIILOG_DECL_VISIBLE;

template <typename QdiiLogParameters>
ErrorCode qdiilog_end() QDIILOG_DECL_VISIBLE;

#define QDIILOG_LOGGING_TYPE_NULL 0
#define QDIILOG_LOGGING_TYPE_STDCOUT 1
#define QDIILOG_LOGGING_TYPE_STDCERR 2
#define QDIILOG_LOGGING_TYPE_FILE 3

#ifndef QDIILOG_LOGGING_TYPE
#   define QDIILOG_LOGGING_TYPE QDIILOG_LOGGING_TYPE_STDCOUT
#endif


//------------------------------------------------------------
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
template <typename QdiilogParameters>
struct Logger
{
    typedef typename QdiilogParameters::Output Output;

    Logger( Loglevel _level = Loglevel::error )
        :m_output( nullptr )
        ,m_level( _level )
    {
    }
    virtual ~Logger() { }

    ErrorCode setOutput( Output & _output )
    {
        m_output = &_output;
        return OK;
    }
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
    static void setLogLevel( Loglevel _level )
    {
        g_config.filter_level = _level;
    }
};

//------------------------------------------------------------
template <typename QdiilogParameters>
QdiilogParameters Logger<QdiilogParameters>::g_config;

//------------------------------------------------------------
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
        if( _logger.g_config.filter_level >= _logger.m_level )
            *( _logger.m_output ) << std::forward<T>( _t );
    }

    return _logger;
}

//------------------------------------------------------------
template <typename QdiilogParameters>
Logger<QdiilogParameters> & Logger<QdiilogParameters>::operator()( bool _condition )
{
    return _condition ? *this : OutputNull<QdiilogParameters>::nullOutput;;
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
    if (ret == OK)
        ret = log_trace.setOutput( _output );
    if (ret == OK)
        ret = log_info.setOutput( _output );
    if (ret == OK)
        ret = log_warning.setOutput( _output );
    if (ret == OK)
        ret = log_error.setOutput( _output );
    
    return ret;
}

#ifdef QDIILOG_NAMESPACE
}
#endif //QDIILOG_NAMESPACE

#endif //QDIILOG_H
