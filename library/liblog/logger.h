#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>
#include <iostream>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/noncopyable.hpp>

//@ afw severity level.
enum SeverityLevel
{
    trace = 0,
    debug,
    info,
    warn,
    error,
    fatal
};

//@ outputs stringized representation of the severity level to the stream
template< typename CharT, typename TraitsT >
inline std::basic_ostream< CharT, TraitsT >& operator<< (
    std::basic_ostream< CharT, TraitsT >& strm,
    SeverityLevel lvl)
{
    static const char* const str[] =
    {
        // log use.
        "trace",
        "debug",
        "info_",
        "warn_",
        "error",
        "fatal"
    };
    //const char* str = ToString(lvl);
    if (static_cast<size_t>(lvl) < (sizeof(str) / sizeof(*str))) {
        strm << str[lvl];
    }
    else {
        strm << static_cast<int>(lvl);
    }

    return strm;
}
namespace light
{

    class Logger
    {
    private:
        Logger();

    public:
        ~Logger();

        static Logger& Instance()
        {
            static Logger instance_;
            return instance_;
        }

        void InitLevelLog();

        // init console sink.
        void InitConsoleLog();

        // init persist logging.
        void InitPersistLog(
            bool is_sync,
            bool is_auto_flush);

        /*@ filter log by serverity.*/
        void Filter(SeverityLevel sev = debug);

        // flush logs to backend output.
        void Flush();

        /*@ open logging.*/
        void Enable(bool is_enabled = true);

    private:
        class impl;
        impl* m_impl;
    };
}
#endif
