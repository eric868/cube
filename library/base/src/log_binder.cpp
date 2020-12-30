/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: log_binder.cpp
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#include "log_binder.h"
#include "base.h"
#include "pugixml.hpp"
#include <string.h>

namespace base
{

logger* default_log_binder::logger_ = NULL;

bool default_log_binder::bind_trace(const char* config_file)
{
	int ret = 0;

	LABEL_SCOPE_START;

	pugi::xml_document doc;

	if (!doc.load_file(config_file)) {
		TRACE_ERROR("base", 0, "trace-binder log config file is not exist or invalid '%s'", config_file);
		ASSIGN_AND_CHECK_LABEL(ret, -1, end);
	}

	log4config log_config;

	pugi::xml_node xlog_local = doc.child("log").child("local");
	pugi::xml_node xlog_remote = doc.child("log").child("remote");
	if (xlog_local.empty() && xlog_remote.empty()) {
		TRACE_ERROR("base", 0, "local or remote log options be configured");
		ASSIGN_AND_CHECK_LABEL(ret, -1, end);
	}

	log_config.store_flag = LOG_STORE_FLAG_NONE;
	std::string tmp;

	if (!xlog_local.empty())
	{
		tmp = xlog_local.attribute("enable").as_string();
		if (tmp == "true") {
			tmp = xlog_local.child("root-path").text().as_string();
			if (tmp.length() > 0) {
				log_config.locset.log_path = tmp;
			}

			tmp = xlog_local.child("date").text().as_string();
			if (tmp.length() > 0) {
				log_config.locset.log_date = tmp;
			}

			tmp = xlog_local.child("file-size-limit").text().as_string();
			if (tmp.length() > 0) {
				log_config.locset.max_file_size = atoi(tmp.c_str());
			}

			tmp = xlog_local.child("file-count-limit").text().as_string();
			if (tmp.length() > 0) {
				log_config.locset.max_backup_index = atoi(tmp.c_str());
			}

			log_config.store_flag |= LOG_STORE_FLAG_LOCAL;
		}
	}

	if (!xlog_remote.empty())
	{
		tmp = xlog_remote.attribute("enable").as_string();
		if (tmp == "true") {
			tmp = xlog_remote.child("server-name").text().as_string();
			if (tmp.length() > 0) {
				log_config.addr.server_name = tmp;
			}

			tmp = xlog_remote.child("host").text().as_string();
			if (tmp.length() > 0) {
				log_config.addr.ip = tmp;
			}

			tmp = xlog_remote.child("port").text().as_string();
			if (tmp.length() > 0) {
				log_config.addr.port = atoi(tmp.c_str());
			}

			log_config.store_flag |= LOG_STORE_FLAG_REMOTE;
		}
	}

	if (log_config.store_flag != LOG_STORE_FLAG_NONE) {
		ret = bind_trace(log_config) ? 0 : -1;
	}

	LABEL_SCOPE_END;

end:
	return ret == 0;
}

bool default_log_binder::bind_trace(log4config& log_config)
{
	if (logger_ != NULL) {
		return true;
	}
	base::trace::enable_std_output(true);
	base::trace::set_trace_callback(default_log_binder::trace_callback);
	base::trace::use_formated_message(true);

	logger_ = new base::logger(log_config);
	return true;
}

bool default_log_binder::bind_trace_local(const char* log_path,
		const char* log_date, int log_file_size_limit, int log_file_count_limit)
{
	base::log4config log_config;
	log_config.format = base::LOG_FORMAT_MESSAGE_ONLY;
	log_config.level = base::LOG_LEVEL_ALL;
	if (strlen(log_path) > 0) {
		log_config.locset.log_path = log_path;
	}
	if (strlen(log_date) > 0) {
		log_config.locset.log_date = log_date;
	}
	log_config.locset.max_file_size = log_file_size_limit;
	log_config.locset.max_backup_index = log_file_count_limit;

	return bind_trace(log_config);
}

void default_log_binder::trace_callback(base::TRACE_LEVEL level,
		const char* module, int code, const char* message)
{
	if (logger_ != NULL) {
		int log_level = base::LOG_LEVEL_TRACE;
		switch (level) {
		case base::TRACE_LEVEL_VERBOSE:
			log_level = base::LOG_LEVEL_TRACE;
			break;
		case base::TRACE_LEVEL_DEBUG:
			log_level = base::LOG_LEVEL_DEBUG;
			break;
		case base::TRACE_LEVEL_INFO:
		case base::TRACE_LEVEL_SYSTEM:
			log_level = base::LOG_LEVEL_INFO;
			break;
		case base::TRACE_LEVEL_WARNING:
			log_level = base::LOG_LEVEL_WARN;
			break;
		case base::TRACE_LEVEL_ERROR:
			log_level = base::LOG_LEVEL_ERROR;
			break;
		default:
			log_level = base::LOG_LEVEL_INFO;
			break;
		}
		logger_->inputlogmsg(log_level, message);
	}
}

void default_log_binder::unbind_trace()
{
	if (logger_) {
		delete logger_;
		logger_ = NULL;
	}
}

}
