/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: trace.cpp
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#include "trace.h"
#include <stdarg.h>
#include <stdio.h>
#include <string>
#include <string.h>
#ifdef _MSC_VER
#include <time.h>
#else
#include <sys/time.h>
#include <errno.h>
#endif
#include "thread.h"

std::string default_error_string_translator(int error)
{
	return "NOT DEFINED";
}

namespace base {

trace_callback trace::callback_ = NULL;
trace_callback trace::mo_callback_ = NULL;
int trace::level_filter_ = TRACE_LEVEL_ALL;
error_string_translator trace::error_string_translator_ = default_error_string_translator;
bool trace::enable_std_output_ = true;
bool trace::use_formated_message_ = true;

#define MESSAGE_BUFFER 4096
#define FORMATED_MESSAGE_BUFFER 5120

const char* parseFileName(const char* filename)
{
	const char* ptr = filename;
	for (ptr += strlen(filename) - 1; ptr > filename; --ptr) {
		if (*ptr == '/' || *ptr == '\\') {
			ptr++;
			break;
		}
	}
	return ptr;
}

void trace::add(const TRACE_LEVEL level,
		const char* module, int code, const char* message, ...)
{
	if (level_filter_ > level) {
		return;
	}

	if (!enable_std_output_ && callback_ == NULL && mo_callback_ == NULL) {
		return;
	}

	va_list args;
	va_start(args, message);

	char buffer[MESSAGE_BUFFER];
	int ret = vsnprintf(buffer, MESSAGE_BUFFER, message, args);
	if (ret < 0) {
		fprintf(stderr, "trace buffer out of memory, module:'%s', message:'%s'\n", module, message);
		return;
	}

	va_end(args);

	if (enable_std_output_ || use_formated_message_)
	{
		char formated_buffer[FORMATED_MESSAGE_BUFFER];

		static const size_t buffer_size = 128;
		char time_buffer[buffer_size];

#if defined(_MSC_VER)
		union {
			long long ns100;
			FILETIME ft;
		} now;

		GetSystemTimeAsFileTime(&now.ft);
		__int64 usec = (__int64)((now.ns100 / 10LL) % 1000000LL);
		__int64 sec = (__int64)((now.ns100 - 116444736000000000LL) / 10000000LL);

		time_t now_t = sec;

		struct tm ltime;
		localtime_s(&ltime, &now_t);

		size_t write_size = strftime(time_buffer, buffer_size, "## %Y-%m-%d %H:%M:%S", &ltime);
		time_buffer[write_size] = '\0';

		char milli_seconds[12];
		sprintf(milli_seconds, ".%03d", (int)usec / 1000);
		strcat(time_buffer, milli_seconds);
#else
		struct timeval tv;
		gettimeofday(&tv, NULL);

		struct tm* ltime = localtime(&tv.tv_sec);
		size_t write_size = strftime(time_buffer, buffer_size, "## %F %T", ltime);
		time_buffer[write_size] = '\0';

		/* add milliseconds */
		char milli_seconds[12];
		sprintf(milli_seconds, ".%03d", (int)tv.tv_usec / 1000);
		strcat(time_buffer, milli_seconds);
#endif

		if (level == TRACE_LEVEL_ERROR) {
#if defined(_MSC_VER) || defined(__APPLE__)
			sprintf(formated_buffer, "%s (%ld) [error] %s(%d:%s): %s\n", time_buffer,
					thread::curr_thread_id(), module, code, error_string_translator_(code).c_str(), buffer);
#else
			if (errno != 0) {
				sprintf(formated_buffer, "%s (%ld) [error] %s(%d:%s, %d:%s): %s\n", time_buffer,
						thread::curr_thread_id(), module, code, error_string_translator_(code).c_str(), errno, strerror(errno), buffer);
			}
			else {
				sprintf(formated_buffer, "%s (%ld) [error] %s(%d:%s): %s\n", time_buffer,
						thread::curr_thread_id(), module, code, error_string_translator_(code).c_str(), buffer);
			}
#endif
		}
		else {
			const char* level_string = NULL;
			switch (level) {
			case TRACE_LEVEL_VERBOSE:
				level_string = "verbose";
				break;
			case TRACE_LEVEL_DEBUG:
				level_string = "debug";
				break;
			case TRACE_LEVEL_INFO:
				level_string = "info";
				break;
			case TRACE_LEVEL_SYSTEM:
				level_string = "system";
				break;
			case TRACE_LEVEL_WARNING:
				level_string = "warning";
				break;
			default:
				level_string = "info";
				break;
			}
			sprintf(formated_buffer, "%s (%ld) [%s] %s(%d): %s\n", time_buffer,
					thread::curr_thread_id(), level_string, module, code, buffer);
		}

		if (enable_std_output_) {
			if (level == TRACE_LEVEL_ERROR) {
				fprintf(stderr, formated_buffer);
			}
			else {
				fprintf(stdout, formated_buffer);
			}
		}

		if (callback_ != NULL) {
			callback_(level, module, code, formated_buffer);
		}

		if (mo_callback_ != NULL) {
			mo_callback_(level, module, code, formated_buffer);
		}
	}
	else {
		if (callback_ != NULL) {
			callback_(level, module, code, buffer);
		}

		if (mo_callback_ != NULL) {
			mo_callback_(level, module, code, buffer);
		}
	}
}

void trace::set_trace_callback(trace_callback callback)
{
	callback_ = callback;
}

void trace::set_trace_mo_callback(trace_callback callback)
{
	mo_callback_ = callback;
}

void trace::set_error_string_translator(error_string_translator translator)
{
	if (translator != NULL) {
		error_string_translator_ = translator;
	}
}

void trace::set_trace_level_filter(int level)
{
	level_filter_ = level;
}

void trace::enable_std_output(bool enable)
{
	enable_std_output_ = enable;
}

void trace::use_formated_message(bool use)
{
	use_formated_message_ = use;
}

} // end of namespace base
