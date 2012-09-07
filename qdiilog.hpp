#ifndef QDIILOG_H
#define QDIILOG_H

#include <iostream>

#ifdef QDIILOG_NAMESPACE
namespace QDIILOG_NAMESPACE
{
#endif

typedef int_fast32_t ix;
typedef ix ErrorCode;

static const ErrorCode          OK          = 0;


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

//------------------------------------------------------------
inline
ErrorCode qdiilog_init()
{
    return OK;
}

//------------------------------------------------------------
inline
ErrorCode qdiilog_end()
{
    return OK;
}


//------------------------------------------------------------
struct QdiilogOstream
{
    typedef std::ostream Output;
};

//------------------------------------------------------------
template <typename QdiilogParameters>
struct Logger
{
    typedef typename QdiilogParameters::Output Output;

    Logger(): m_output( nullptr ) { }
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

};


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
        *( _logger.m_output ) << std::forward<T>(_t);

    return _logger.m_output ? _logger : OutputNull<QdiilogParameters>::nullOutput;
}

//------------------------------------------------------------
template <typename QdiilogParameters>
Logger<QdiilogParameters> & Logger<QdiilogParameters>::operator()( bool _condition )
{
    return _condition ? *this : OutputNull<QdiilogParameters>::nullOutput;;
}

#ifdef QDIILOG_NAMESPACE
}
#endif //QDIILOG_NAMESPACE

#endif //QDIILOG_H
