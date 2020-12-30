#ifndef LOG_H_
#define LOG_H_

#include "logger.h"
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/attributes/named_scope.hpp>


BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(logger_mt,
        boost::log::sources::severity_logger_mt<SeverityLevel>)

//stlog define
#define BOOST_SLOG(stlog_lvl) \
    BOOST_LOG_FUNCTION();\
    BOOST_LOG_SEV(logger_mt::get(), stlog_lvl)

#define STLOG_TRACE     BOOST_SLOG(trace)
#define STLOG_DEBUG     BOOST_SLOG(debug)
#define STLOG_INFO      BOOST_SLOG(info)
#define STLOG_WARN      BOOST_SLOG(warn)
#define STLOG_ERROR     BOOST_SLOG(error)
#define STLOG_FATAL     BOOST_SLOG(fatal)

#endif
