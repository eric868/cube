/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: util.h
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __VBASE_UTIL_H__
#define __VBASE_UTIL_H__

#include <string>
#include <vector>
#include "base.h"

namespace base {

class COMMON_API util
{
public:
	static bool is_equal_ignore_case(const char* s1, const char* s2);
	static bool has_prefix_ignore_case(const char* s1, const char* prefix);
	static bool is_nonnegative_integer(const char* s);
	static bool is_positive_integer(const char* s);
	static bool is_integer(const char* s);
	static bool is_number(const char* s);

	static std::string to_lower_case(const char* s);
	static std::string to_upper_case(const char* s);

	static void sleep(unsigned int milli_seconds);
    static void usleep(unsigned long micro_seconds);
    /*
     * get current clock tick, in milliseconds.
     */
    static long clock();
    /*
     * get local utc timestamp (unit:second)
     */
    static base::int64 local_timestamp();
	/*
	 * get local utc timestamp (unit:millisecond)
	 */
	static base::int64 local_mtimestamp();
    /*
     * get local date string, format: YYYY-MM-DD
     */
    static std::string local_date_string();
    /*
     * get local time string, format: HH:MM:SS
     */
    static std::string local_time_string();
    /*
     * get local date time string, format: YYYY-MM-DD HH:MM:SS
     */
    static std::string local_date_time_string();
    /*
     * translate timestamp into date string, format: YYYY-MM-DD
     */
    static std::string date_string(base::int64 timestamp);
    /*
     * translate timestamp into time string, format: HH:MM:SS
     */
    static std::string time_string(base::int64 timestamp);
    /*
     * translate timestamp into date time string, format: YYYY-MM-DD HH:MM:SS
     */
    static std::string date_time_string(base::int64 timestamp);
    /*
     * translate date time string to timestamp, format:
	 *	1. YYYY-MM-DD HH:MM:SS
	 *  2. YYYY/MM/DD HH:MM:SS
     */
    static base::int64 string_to_timestamp(const char* str, int format = 1);
    /*
     * calculate the hash value of the string
     */
    static int hash_key(const char* key);
    /*
     * parse server address, which may like 127.0.0.1:9008
     * @param server[in] server address
     * @param domain[out] the doman/ip of the server
     * @param port[out] the port of the server
     * @return true if parse successfully, false if failed
     */
    static bool parse_server_addr(const std::string& server_addr, std::string& domain, int& port);
    /*
     * compare version string, which may like 1.0.2.3
     * @param v1 first version string
     * @param v2 second version string
     * @return 1 if v1 > v2; 0 if equal; -1 if v1 < v2;
     */
    static int compare_version(const char* v1, const char* v2);
    /*
     * calculate crc
     */
    static int calculate_crc(void const* content, size_t content_length, int crc);

    /*
     * calculate 函数设计到位，节约了空间，当然我们也可以用bitset
     */
    static unsigned int calculate_hash(const char* a);

    /*
     * 按分隔符解析字符串
     */
    static void splitstrbychar(const char* buf, char ch, std::vector<std::string>& arout,long size);
	
	/*
     * 获取当日时间戳（不含时分秒）
     */
	static long local_datestamp();

	/*
	 * 将yyyy-mm-dd和yyyy-mm-dd hh:mm:ss的格式的时间转换为时间戳
	 */
	static long string_to_datestamp(char* str);

    /*
    * 加密
    */
    static void encode(char *pstr);

    /*
    * 解密
    */
    static void decode(char *pstr);
};

}

#endif
