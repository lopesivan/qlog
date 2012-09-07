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
struct OutputNull : public std::ostream
{
};

//------------------------------------------------------------
template <typename T>
std::ostream & operator<<( OutputNull & _ostr , T )
{
    return _ostr;
}

//------------------------------------------------------------
struct QdiilogOstream
{
    typedef std::ostream Output;
};

//------------------------------------------------------------
template <typename QdiilogParameters>
struct Logger : public std::ostream
{
    typedef typename QdiilogParameters::Output Output;

    Logger(): m_output( nullptr ) { }

    ErrorCode setOutput( Output & _output )
    {
        m_output = &_output;
        return OK;
    }
    std::ostream & operator()( bool _condition );

private:
    Logger(Logger&) = delete;
    Logger& operator=(Logger&) = delete;
    
    std::ostream  * m_output;
    template<typename T>
    friend std::ostream & operator<<( Logger & _logger, T & _t );

};

//------------------------------------------------------------
static OutputNull nullOutput;

//------------------------------------------------------------
template <typename QdiilogParameters, typename T>
std::ostream & operator<<( Logger<QdiilogParameters> & _logger, T & _t )
{
    std::ostream & output =
        _logger.m_output ? *( _logger.m_output ) : nullOutput;
    output << _t;
    return output;
}

//------------------------------------------------------------
template <typename QdiilogParameters>
std::ostream & Logger<QdiilogParameters>::operator()( bool _condition )
{
    return _condition ? *this : nullOutput;
}

#ifdef QDIILOG_NAMESPACE
}
#endif //QDIILOG_NAMESPACE

#endif //QDIILOG_H
