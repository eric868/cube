/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: trace.h
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __BASE_TRACE_H__
#define __BASE_TRACE_H__

#include <string>
#include "base.h"
namespace base {

enum TRACE_LEVEL
{
	TRACE_LEVEL_ALL = 0x0000,
	TRACE_LEVEL_VERBOSE = 0x0001,
	TRACE_LEVEL_DEBUG = 0x0002,
	TRACE_LEVEL_INFO = 0x0004,
	TRACE_LEVEL_SYSTEM = 0x0008,
	TRACE_LEVEL_WARNING = 0x0010,
	TRACE_LEVEL_ERROR = 0x0020,

	TRACE_LEVEL_OFF = 0x7FFF,
};

COMMON_API const char* parseFileName(const char* filename);

#if defined(DEBUG) || defined(_DEBUG)
#	define TRACE_DEBUG(module, message, ...) \
	base::trace::add(base::TRACE_LEVEL_DEBUG, module, 0, ("[%s:%d, %s] " message), base::parseFileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)

#	define TRACE_INFO(module, message, ...) \
	base::trace::add(base::TRACE_LEVEL_INFO, module, 0, ("[%s:%d, %s] " message), base::parseFileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
#	define TRACE_DEBUG(module, message, ...)
#	define TRACE_INFO(module, message, ...)
#endif

#define TRACE_SYSTEM(module, message, ...) \
	base::trace::add(base::TRACE_LEVEL_SYSTEM, module, 0, ("[%s:%d, %s] " message), base::parseFileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)

#define TRACE_WARNING(module, message, ...) \
	base::trace::add(base::TRACE_LEVEL_WARNING, module, 0, ("[%s:%d, %s] " message), base::parseFileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)

#define TRACE_ERROR(module, code, message, ...) \
	base::trace::add(base::TRACE_LEVEL_ERROR, module, code, ("[%s:%d, %s] " message), base::parseFileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)

typedef void (*trace_callback)(base::TRACE_LEVEL level, const char* module, int code, const char* message);
typedef std::string (*error_string_translator)(int);

class COMMON_API trace
{
public:
	static void add(const TRACE_LEVEL level,
			const char* module,
			int code,
			const char* message, ...);

	static void set_trace_callback(trace_callback callback);
	static void set_trace_mo_callback(trace_callback callback);
	static void set_trace_level_filter(int level);
	static void set_error_string_translator(error_string_translator translator);
	static void enable_std_output(bool enable);
	static void use_formated_message(bool use);

public:
	static trace_callback callback_;
	static trace_callback mo_callback_;
	static int level_filter_;
	static error_string_translator error_string_translator_;
	static bool enable_std_output_;
	static bool use_formated_message_;
};

} // end of namespace base

#endif
