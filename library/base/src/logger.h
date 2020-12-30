/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: logger.h
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __LOGGER_CLIENT_H__
#define __LOGGER_CLIENT_H__

#include "log4cplus/logger.h"
#include "log4cplus/loglevel.h"
#include <string>
#include <time.h>
#include <stdio.h>
#include <memory.h>
#include "base.h"

using log4cplus::Logger;
using std::string;

namespace base {

enum LOG_LEVEL
{
	LOG_LEVEL_ALL = 0,
	LOG_LEVEL_TRACE = 0,
	LOG_LEVEL_DEBUG = 10000,
	LOG_LEVEL_INFO = 20000,
	LOG_LEVEL_WARN = 30000,
	LOG_LEVEL_ERROR = 40000,
	LOG_LEVEL_FATAL = 50000,
	LOG_LEVEL_OFF = 60000
};

enum LOG_FORMAT
{
	LOG_FORMAT_MESSAGE_ONLY = 0,
	LOG_FORMAT_STANDARD = 1
};

enum LOG_STORE_FLAG
{
	LOG_STORE_FLAG_NONE = 0,
	LOG_STORE_FLAG_LOCAL = 1,
	LOG_STORE_FLAG_REMOTE = 2,
	LOG_STORE_FLAG_LOCAL_AND_REMOTE = 3
};

struct log4config
{
	/* 1:本地存储，2:远程转发，3:both */
	int store_flag;
	/*
	 * TRACE < DEBUG < INFO < WARN < ERROR < FATAL
	 * 上述各种级别,从左到右，重要性依次递增。而在配置文件中的级别会屏蔽掉比它重要性更低的日志输出
	 * NOT_SET_LOG_LEVEL = -1,
	 * ALL_LOG_LEVEL = 0,
	 * TRACE_LOG_LEVEL = 0,
	 * DEBUG_LOG_LEVEL = 10000,
	 * INFO_LOG_LEVEL = 20000,
	 * WARN_LOG_LEVEL = 30000,
	 * ERROR_LOG_LEVEL = 40000,
	 * FATAL_LOG_LEVEL = 50000,
	 * OFF_LOG_LEVEL = 60000
	 */
	int level;

	/* 0: 默认格式只输出msg; 1:输出标准格式 */
	int format;

	/* remote server info */
	struct serveraddr {
		string ip; 				/* 服务器IP地址："127.0.0.1" */
		short port; 	 		/* 服务器端口号 */
		string server_name; 	/* Host name of event's origin prepended to each event */
		serveraddr()
			: ip("")
			, port(0)
			, server_name("")
		{}
	} addr;

	/* local store info */
	struct localstore {
		string log_name;		/* "system.log" */
		string log_path;    	/* "/a/b/c/log/" */
		string log_date;     	/* "20150916" */
		long max_file_size;		/* log file size limit, 100(M) by default, if -1,  */
		int max_backup_index;	/* log file count limit, 10 by default */

		localstore()
			: log_name("system.log")
			, log_path("./log")
			, max_file_size(100*1024*1024)
			, max_backup_index(10) {
				time_t now = time(NULL);
				tm* t = localtime(&now);
				char buff[20];
				memset(buff, 0, sizeof(buff));
				sprintf(buff, "%d%02d%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
				log_date = buff;
			}
	} locset;

	log4config()
		: store_flag(LOG_STORE_FLAG_LOCAL)
		, level(LOG_LEVEL_ALL)
		, format(LOG_FORMAT_MESSAGE_ONLY)
	{}
};

class COMMON_API logger
{
public:
	logger(struct log4config& config);
	~logger();
	/*
	 * 需要输出的log信息
	 * @param level 输出log等级，参照trace中的TRACE_LEVEL
	 * @param msg 需要输出的打印信息
	 */
	void inputlogmsg(int level, const char* msg);

protected:
	/* 初始化远程服务器 */
	void initserver(const char* ip, short port, const char* servername);

	/* 初始化本地存储*/
	void initlocstore(const char* filename, long maxsize,
			int maxindex, int format = LOG_FORMAT_MESSAGE_ONLY);
};

}
#endif
