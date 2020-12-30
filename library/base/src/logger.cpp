/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: logger.cpp
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#include "logger.h"
#include "log4cplus/socketappender.h"
#include "log4cplus/loggingmacros.h"
#include "log4cplus/tstring.h"
#include "log4cplus/fileappender.h"
#include "trace.h"
#include "file.h"
#include "base.h"
#include <limits.h>

using namespace log4cplus;

namespace base {

logger::logger(struct log4config& config)
{
	string abs_path = config.locset.log_path + string("/") + config.locset.log_date;
#if defined(_MSC_VER)
	int pos = 0;
	abs_path += "\\";
	while ((pos = abs_path.find("/", pos)) != std::string::npos)
	{
		abs_path.replace(pos, strlen("/"), "\\");
		pos++;
	}
#endif
	if(!file::make_sure_directory_exist(abs_path.c_str())) {
		printf("create error: %s\n", abs_path.c_str());
	}

#ifndef WIN32
    abs_path += string("/") + config.locset.log_name;
#else
    abs_path += config.locset.log_name;
#endif

#if defined(_MSC_VER)
	pos = 0;
	while ((pos = abs_path.find("/", pos)) != std::string::npos)
	{
		abs_path.replace(pos, strlen("/"), "\\");
		pos++;
	}
#endif
	if (config.locset.max_file_size == -1) {
		config.locset.max_file_size = kMaxInt64 ;
	}
	else {
		config.locset.max_file_size *= 1024*1024;//MB
	}

	if(config.locset.max_backup_index == -1) {
		config.locset.max_backup_index = 31998; //unix
	}
	switch(config.store_flag) {
	case LOG_STORE_FLAG_LOCAL:
		initlocstore(abs_path.c_str(), config.locset.max_file_size, config.locset.max_backup_index, config.format);
		break;
	case LOG_STORE_FLAG_REMOTE:
		initserver(config.addr.ip.c_str(), config.addr.port, config.addr.server_name.c_str());
		break;
	case LOG_STORE_FLAG_LOCAL_AND_REMOTE:
		initlocstore(abs_path.c_str(), config.locset.max_file_size, config.locset.max_backup_index, config.format);
		initserver(config.addr.ip.c_str(), config.addr.port, config.addr.server_name.c_str());
		break;
	default:
		break;
	}
	Logger::getRoot().setLogLevel(config.level);
}

logger::~logger()
{
}

void logger::initserver(const char* ip, short port, const char* server_name)
{
	if (ip == NULL || (port < 0 || port > 65535)) {
		printf("loggingout has a error, ip or port invalid!!!\n");
		return;
	}

	tstring tsservername = server_name ? LOG4CPLUS_C_STR_TO_TSTRING(server_name) : LOG4CPLUS_C_STR_TO_TSTRING("");
	//tstring tshost = LOG4CPLUS_TEXT(ip);
	tstring tshost = LOG4CPLUS_C_STR_TO_TSTRING(ip);
	SharedAppenderPtr append_1(new SocketAppender(tshost, port, tsservername));
	append_1->setName(LOG4CPLUS_TEXT("server"));
	Logger::getRoot().addAppender(append_1);
}

void logger::initlocstore(const char* filename, long maxsize, int maxindex, int format)
{
	/*限制文件大小个数存储*/
	// SharedAppenderPtr append_daily(new RollingFileAppender(filename, maxsize, maxindex, true));
	SharedAppenderPtr append_daily(new DailyRollingFileAppender(LOG4CPLUS_C_STR_TO_TSTRING(filename), DAILY, true, maxindex));
	append_daily->setName(LOG4CPLUS_TEXT("client"));
	std::string pattern;
	if (format == LOG_FORMAT_STANDARD) {
		pattern = "|%D:%d{%Q}|%p|%t|%l|%m|%n";
	}
	else {
		pattern = "%m";
	}
	std::auto_ptr<Layout> pat(new PatternLayout(LOG4CPLUS_C_STR_TO_TSTRING(pattern)));
	//std::auto_ptr<Layout> ttc(new TTCCLayout());
	append_daily->setLayout(pat);
	Logger::getRoot().addAppender(append_daily);
}

void logger::inputlogmsg(int level, const char* msg)
{
	Logger root_ = Logger::getRoot();
	switch (level) {
	case LOG_LEVEL_TRACE:
		LOG4CPLUS_TRACE(root_, msg);
		break;
	case LOG_LEVEL_DEBUG:
		LOG4CPLUS_DEBUG(root_, msg);
		break;
	case LOG_LEVEL_INFO:
		LOG4CPLUS_INFO(root_, msg);
		break;
	case LOG_LEVEL_WARN:
		LOG4CPLUS_WARN(root_, msg);
		break;
	case LOG_LEVEL_ERROR:
		LOG4CPLUS_ERROR(root_, msg);
		break;
	case LOG_LEVEL_FATAL:
		LOG4CPLUS_FATAL(root_, msg);
		break;
	default:
		assert(false);
		break;
	}
}

}
