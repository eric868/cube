#include "logger.h"
#include <boost/log/sinks.hpp>
#include <boost/log/common.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;
namespace attrs = boost::log::attributes;
namespace sinks = boost::log::sinks;

/*@ const attribute names.*/
namespace log_attribute_names {

    inline const char* line_id()
    {
        return "lid";
    }

    inline const char* timestamp()
    {
        return "tsp";
    }

    inline const char* thread_id()
    {
        return "tid";
    }

    inline const char* severity()
    {
        return "sev";
    }

    inline const char* scope()
    {
        return "scp";
    }

    inline const char* message()
    {
        return "msg";
    }

    // inner name, cann't change.
    inline const char* inner_severity()
    {
        return "Severity";
    }
}// ns::log_attr

 /*@ extract attribute placeholder.*/
BOOST_LOG_ATTRIBUTE_KEYWORD(attr_line_id,
    log_attribute_names::line_id(), unsigned int);
BOOST_LOG_ATTRIBUTE_KEYWORD(attr_timestamp,
    log_attribute_names::timestamp(),
    attrs::local_clock::value_type);
BOOST_LOG_ATTRIBUTE_KEYWORD(attr_thread_id,
    log_attribute_names::thread_id(),
    attrs::current_thread_id::value_type);
BOOST_LOG_ATTRIBUTE_KEYWORD(attr_severity,
    log_attribute_names::inner_severity(),
    SeverityLevel);
BOOST_LOG_ATTRIBUTE_KEYWORD(attr_scope,
    log_attribute_names::scope(),
    attrs::named_scope::value_type);

namespace light
{

    class Logger::impl
    {
    public:
        // console sink.
        typedef sinks::asynchronous_sink<sinks::text_ostream_backend>
            async_console_sink;
        typedef sinks::synchronous_sink<sinks::text_ostream_backend>
            sync_console_sink;
        // file sink.
        typedef sinks::asynchronous_sink<sinks::text_file_backend>
            async_file_sink;
        typedef sinks::synchronous_sink<sinks::text_file_backend>
            sync_file_sink;

    public:
        impl();
        ~impl();

        // init attributes.
        void InitAttributes();

        void InitLevelSink();

        // init console sink.
        template<typename sink_type>
        void InitConsoleSink();

        template<typename sink_type>
        void InitPersistSink(bool is_auto_flush);
    };

    Logger::impl::impl()
    {
    }

    Logger::impl::~impl()
    {
        logging::core::get()->remove_all_sinks();
    }

    void Logger::impl::InitAttributes()
    {
        // add "LineID"\"TimeStamp"\"ThreadID";
        // logging::add_common_attributes();
        logging::core::get()->add_global_attribute(
            log_attribute_names::line_id(),
            attrs::counter<unsigned int>(1));
        logging::core::get()->add_global_attribute(
            log_attribute_names::timestamp(),
            attrs::local_clock());
        logging::core::get()->add_global_attribute(
            log_attribute_names::thread_id(),
            attrs::current_thread_id());
        logging::core::get()->add_global_attribute(
            log_attribute_names::scope(),
            attrs::named_scope());
    }

    void Logger::impl::InitLevelSink()
    {
        for (int i = trace; i <= fatal; ++i) {
            std::stringstream allfilename;
            allfilename << "logs/level_" << (SeverityLevel)i << "_%Y%m%d_%N.log";

            // Create a backend and initialize it with a stream
            boost::shared_ptr<sinks::text_file_backend> file_backend
                = boost::make_shared<sinks::text_file_backend>(
                    keywords::file_name = allfilename.str(),
                    keywords::rotation_size = 500 * 1024 * 1024,
                    keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
                    keywords::open_mode = std::ios::app
                    );
            bool is_auto_flush = ((i >= error) ? true : false);
            file_backend->auto_flush(is_auto_flush);

            boost::shared_ptr<async_file_sink> sink(new async_file_sink(file_backend));
            sink->set_filter(attr_severity == (SeverityLevel)i);
            sink->set_formatter(expr::format(
                "## %1% (%2%) [%3%]: %4%")
                % expr::format_date_time(attr_timestamp, "%Y-%m-%d %H:%M:%S.%f")
                % attr_thread_id
                % expr::format_named_scope(attr_scope,
                    keywords::format = "%f:%l, %n", keywords::depth = 1)
                % expr::smessage
            );
            logging::core::get()->add_sink(sink);
        }
    }


    template<typename sink_type>
    void Logger::impl::InitConsoleSink()
    {
        // synchronous sink to display on console.
        boost::shared_ptr<sink_type> console_sink;
        console_sink = boost::make_shared<sink_type>();

        console_sink->locked_backend()->add_stream(
            boost::shared_ptr<std::ostream>(
                &std::clog, boost::null_deleter()));

        console_sink->set_formatter(expr::format("## %1% (%2%) [%3%] [%4%]: %5%")
            % expr::format_date_time(attr_timestamp, "%Y-%m-%d %H:%M:%S.%f")
            % attr_thread_id
            % attr_severity
            % expr::format_named_scope(attr_scope,
                keywords::format = "%f:%l, %n", keywords::depth = 1)
            % expr::smessage);
        logging::core::get()->add_sink(console_sink);
    }

    template<typename sink_type>
    void Logger::impl::InitPersistSink(bool is_auto_flush)
    {
        /*@ asynchronous sink to file.*/
        // construct backend.
        boost::shared_ptr<sinks::text_file_backend> backend =
            boost::make_shared<sinks::text_file_backend>(
                keywords::open_mode = std::ios::app,
                keywords::file_name = "./logs/%Y%m%d_%N.log",
                keywords::rotation_size = 500 * 1024 * 1024,        // 500M
                keywords::time_based_rotation =
                sinks::file::rotation_at_time_point(0, 0, 0));
        backend->auto_flush(is_auto_flush);

        // construct sink.
        boost::shared_ptr<sink_type> file_sink(new sink_type(backend));
        file_sink->set_formatter(expr::format(
            "## %1% (%2%) [%3%] [%4%]: %5%")
            % expr::format_date_time(attr_timestamp, "%Y-%m-%d %H:%M:%S.%f")
            % attr_thread_id
            % attr_severity
            % expr::format_named_scope(attr_scope,
                keywords::format = "%f:%l, %n", keywords::depth = 1)
            % expr::smessage
        );
        logging::core::get()->add_sink(file_sink);
    }

    Logger::Logger()
    {
        m_impl = new impl();

        m_impl->InitAttributes();
        Filter(debug);
    }

    Logger::~Logger()
    {
        delete m_impl;
        m_impl = nullptr;
    }

    void Logger::InitLevelLog()
    {
        m_impl->InitLevelSink();
    }

    void Logger::InitConsoleLog()
    {
        m_impl->InitConsoleSink<impl::sync_console_sink>();
    }

    void Logger::InitPersistLog(bool is_sync, bool is_auto_flush)
    {
        if (is_sync)
        {
            m_impl->InitPersistSink<impl::sync_file_sink>(is_auto_flush);
        }
        else
        {
            m_impl->InitPersistSink<impl::async_file_sink>(is_auto_flush);
        }
    }

    void Logger::Filter(SeverityLevel sev)
    {
        logging::core::get()->set_filter(attr_severity >= sev);
    }

    void Logger::Flush()
    {
        logging::core::get()->flush();
    }

    void Logger::Enable(bool is_enabled)
    {
        logging::core::get()->set_logging_enabled(is_enabled);
    }
}
