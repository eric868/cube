/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: log_binder.h
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __BASE_LOG_BINDER_H__
#define __BASE_LOG_BINDER_H__

#include "trace.h"
#include "logger.h"

namespace base
{

class COMMON_API default_log_binder
{
public:
	static bool bind_trace(const char* config_file);
	static bool bind_trace(log4config& log_config);
	static bool bind_trace_local(const char* log_path, const char* log_date,
			int log_file_size_limit = 100, int log_file_count_limit = 10);

	static void trace_callback(base::TRACE_LEVEL level,
			const char* module, int code, const char* message);

	static void unbind_trace();

private:
	static logger* logger_;
};

}

#endif
