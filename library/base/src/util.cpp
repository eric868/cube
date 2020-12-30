/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: util.cpp
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#include "util.h"
#ifdef _MSC_VER
#include <Windows.h>
#include <time.h>
#include <assert.h>
#else
#include <sys/time.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "base.h"
#include <stdlib.h>
#endif
#include <stdio.h>
#include <iostream>
namespace base {

bool util::is_equal_ignore_case(const char* s1, const char* s2)
{
    assert(s1 != NULL && s2 != NULL);

    do {
        char c1 = *s1, c2 = *s2;
        if (c1 >= 'A' && c1 <='Z') {
            c1 -= ('A' - 'a');
        }
        if (c2 >= 'A' && c2 <= 'Z') {
            c2 -= ('A' - 'a');
        }
        if (c1 != c2) {
            return false;
        }
        s1++, s2++;
    } while (*s1 != 0 || *s2 != 0);

    return true;
}

bool util::has_prefix_ignore_case(const char* s1, const char* prefix)
{
    assert(s1 != NULL && prefix != NULL);

    do {
        char c1 = *s1, c2 = *prefix;
        if (c1 >= 'A' && c1 <='Z') {
            c1 -= ('A' - 'a');
        }
        if (c2 >= 'A' && c2 <= 'Z') {
            c2 -= ('A' - 'a');
        }
        if (c1 != c2) {
            return false;
        }
        s1++, prefix++;
    } while (*prefix != 0);

    return true;
}

bool util::is_nonnegative_integer(const char* s)
{
    if (*s == '0' && *(s+1) != 0x00) {
        return false;
    }

    do {
        if (*s < '0' || *s > '9') {
            return false;
        }
    } while (*(++s));

    return true;
}

bool util::is_positive_integer(const char* s)
{
    if (*s <= '0' || *s > '9') {
        return false;
    }

    while (*(++s)) {
        if (*s < '0' || *s > '9') {
            return false;
        }
    }
    return true;
}

bool util::is_integer(const char* s)
{
    if (*s == '+' || *s == '-') {
        ++s;
    }

    return is_nonnegative_integer(s);
}

bool util::is_number(const char* s)
{
    if (is_integer(s)) {
        return true;
    }

    static const int yy_accept[21] =
    {   0,
        0,    0,    3,    2,    2,    2,    2,    1,    0,    0,
        0,    1,    1,    1,    1,    0,    1,    0,    1,    0
    } ;

    static const int yy_ec[256] =
    {   0,
        1,    1,    1,    1,    1,    1,    1,    1,    2,    2,
        2,    2,    2,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    2,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    3,    1,    3,    4,    1,    5,    5,    5,
        5,    5,    5,    5,    5,    5,    5,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    6,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,

        6,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,

        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1
    } ;

    static const int yy_meta[7] =
    {   0,
        1,    2,    1,    1,    3,    1
    } ;

    static const int yy_base[24] =
    {   0,
        0,    0,   27,   28,    5,    7,    0,   11,    0,    0,
        0,    0,   16,    0,   21,   20,    0,    0,   19,   28,
        17,   17,   11
    } ;

    static const int yy_def[24] =
    {   0,
        20,    1,   20,   20,   20,   20,   21,   20,    5,    6,
        21,    8,   20,   22,   13,   20,   15,   23,   15,    0,
        20,   20,   20
    } ;

    static const int yy_nxt[35] =
    {   0,
        4,    5,    6,    7,    8,    4,    9,   10,   11,   12,
        11,   12,   14,   19,   15,   12,   16,   14,   14,   13,
        13,   16,   18,   19,   19,   17,   20,    3,   20,   20,
        20,   20,   20,   20
    } ;

    static const int yy_chk[35] =
    {   0,
        1,    1,    1,    1,    1,    1,    5,    5,    5,    5,
        6,    6,    8,   23,    8,    8,    8,   13,   22,   21,
        13,   13,   16,   19,   16,   15,    3,   20,   20,   20,
        20,   20,   20,   20
    } ;

    enum { last_dfa = 19, jambase = 28 };

    int curr = 1, c ;
    do {
        if (0 == *s) {
            break;
        }

        c = yy_ec[*(unsigned char*)s];
        if (1 == yy_accept[curr] && 0 == s[1]) {
            return true;
        }
        while (yy_chk[yy_base[curr] + c] != curr) {
            if ((curr=yy_def[curr]) >= last_dfa + 2) {
                c = yy_meta[c];
            }
        }
        curr = yy_nxt[yy_base[curr] + c];
        ++s;
    } while(yy_base[curr] != jambase);

    return false;
}

std::string util::to_lower_case(const char* s)
{
	std::string ret = s;
	for (int i = 0; i < (int)ret.length(); i++) {
		if (ret[i] >= 'A' && ret[i] <= 'Z') {
			ret[i] += ('a' - 'A');
		}
	}
	return ret;
}

std::string util::to_upper_case(const char* s)
{
	std::string ret = s;
	for (int i = 0; i < (int)ret.length(); i++) {
		if (ret[i] >= 'a' && ret[i] <= 'z') {
			ret[i] += ('A' - 'a');
		}
	}
	return ret;
}

void util::sleep(unsigned int milli_seconds)
{
#if defined(_MSC_VER)
	Sleep(milli_seconds);
#else
    ::usleep(milli_seconds * 1000);
#endif
}
    
void util::usleep(unsigned long micro_seconds)
{
#if defined(_MSC_VER)
#else
    ::usleep((unsigned int)micro_seconds);
#endif
}

long util::clock()
{
#if defined(_MSC_VER)
    return ::clock();
#else
    struct timeval t;
    gettimeofday(&t, NULL);
    long long end = t.tv_sec * 1000000 + t.tv_usec;
    return (long)(end / 1000);
#endif
}

base::int64 util::local_timestamp()
{
	return (base::int64)time(NULL);
}

base::int64 util::local_mtimestamp()
{
#if defined(_MSC_VER)
	union {
		long long ns100;
		FILETIME ft;
	} now;

	GetSystemTimeAsFileTime(&now.ft);
	base::int64 usec = (base::int64)((now.ns100 / 10LL) % 1000000LL);
	base::int64 sec = (base::int64)((now.ns100 - 116444736000000000LL) / 10000000LL);

	return (sec * 1000 + usec / 1000);
#else
	struct timeval t;
	gettimeofday(&t, NULL);
	long long end = t.tv_sec * 1000 + t.tv_usec / 1000;
	return (long)end;
#endif
}

std::string util::local_date_string()
{
	struct tm ti = {0};
	time_t now = time(NULL);
#ifdef _MSC_VER
	localtime_s(&ti, &now);
#else
	localtime_r(&now, &ti);
#endif

	char date_buf[20];
	sprintf(date_buf, "%04d-%02d-%02d", 1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday);
	return date_buf;
}

std::string util::local_time_string()
{
	struct tm ti = {0};
	time_t now = time(NULL);
#ifdef _MSC_VER
	localtime_s(&ti, &now);
#else
	localtime_r(&now, &ti);
#endif

	char time_buf[20];
	sprintf(time_buf, "%02d:%02d:%02d", ti.tm_hour, ti.tm_min, ti.tm_sec);
	return time_buf;
}

std::string util::local_date_time_string()
{
	struct tm ti = {0};
	time_t now = time(NULL);
#ifdef _MSC_VER
	localtime_s(&ti, &now);
#else
	localtime_r(&now, &ti);
#endif

	char date_time_buf[32];
	sprintf(date_time_buf, "%04d-%02d-%02d %02d:%02d:%02d", 1900 + ti.tm_year,
			ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec);
	return date_time_buf;
}


std::string util::date_string(base::int64 timestamp)
{
#ifdef _MSC_VER
	struct tm ti = { 0 };
	localtime_s(&ti, &timestamp);

	char date_buf[20];
	sprintf(date_buf, "%04d-%02d-%02d", 1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday);
	return date_buf;
#else
	struct tm ti = {0};
	localtime_r(&timestamp, &ti);

	char date_buf[20];
	sprintf(date_buf, "%04d-%02d-%02d", 1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday);
	return date_buf;
#endif
}

std::string util::time_string(base::int64 timestamp)
{
#ifdef _MSC_VER
	struct tm ti = { 0 };
	localtime_s(&ti, &timestamp);

	char time_buf[20];
	sprintf(time_buf, "%02d:%02d:%02d", ti.tm_hour, ti.tm_min, ti.tm_sec);
	return time_buf;
#else
	struct tm ti = {0};
	localtime_r(&timestamp, &ti);

	char time_buf[20];
	sprintf(time_buf, "%02d:%02d:%02d", ti.tm_hour, ti.tm_min, ti.tm_sec);
	return time_buf;
#endif
}

std::string util::date_time_string(base::int64 timestamp)
{
#ifdef _MSC_VER
	struct tm ti = { 0 };
	localtime_s(&ti, &timestamp);

	char date_time_buf[32];
	sprintf(date_time_buf, "%04d-%02d-%02d %02d:%02d:%02d", 1900 + ti.tm_year,
		ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec);
	return date_time_buf;
#else
	struct tm ti = {0};
	localtime_r(&timestamp, &ti);

	char date_time_buf[32];
	sprintf(date_time_buf, "%04d-%02d-%02d %02d:%02d:%02d", 1900 + ti.tm_year,
			ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec);
	return date_time_buf;
#endif
}

base::int64 util::string_to_timestamp(const char* str, int format)
{
#ifdef _MSC_VER
	struct tm t;
	int year, month, day, hour, minute, second;
	int ret = 0;
	if (format == 1) {
		ret = sscanf(str, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
	}
	else if (format == 2) {
		ret = sscanf(str, "%d/%d/%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
	}
	if (ret > 0) {
		t.tm_year = year - 1900;
		t.tm_mon = month - 1;
		t.tm_mday = day;
		t.tm_hour = hour;
		t.tm_min = minute;
		t.tm_sec = second;
		t.tm_isdst = 0;

		return mktime(&t);
	}
	return 0;
#else
	char time_str[64];
	strcpy(time_str, str);
	char* tmp = time_str;
	while (*tmp) {
		if (*tmp == ' ') {
			*tmp = '-';
		}
		tmp++;
	}

	std::string fmt = "%Y-%m-%d-%H:%M:%S";
	if (format == 2) {
		fmt = "%Y/%m/%d-%H:%M:%S";
	}
	struct tm t;
	if (strptime(time_str, fmt.c_str(), &t) != NULL) {
		return mktime(&t);
	}
	return 0;
#endif
}


int util::hash_key(const char* key)
{
	int hash = 1315423911;
	while (*key) {
		hash ^= ((hash << 5) + (*key++) + (hash >> 2));
	}
	return hash & 0x7FFF;
}

bool util::parse_server_addr(const std::string& server_addr, std::string& domain, int& port)
{
	int pos = server_addr.find(":");
	if (pos != (int)std::string::npos) {
		domain = server_addr.substr(0, pos);
		port = atoi(server_addr.substr(pos + 1, server_addr.length() - pos - 1).c_str());
		return true;
	}
	return false;
}

int util::compare_version(const char* v1, const char* v2)
{
	assert(v1 != NULL);
	assert(v2 != NULL);

	char t1[64], t2[64];
	do {
		int p1 = 0, p2 = 0;
		t1[0] = t2[0] = 0x00;
		while (*v1 != 0 && *v1 != '.') {
			t1[p1++] = *v1++;
		}
		while (*v2 != 0 && *v2 != '.') {
			t2[p2++] = *v2++;
		}

		int tv1 = atoi(t1);
		int tv2 = atoi(t2);
		if (tv1 > tv2) {
			return 1;
		}
		else if (tv1 < tv2) {
			return -1;
		}

		if (*v1 == '.') {
			v1++;
		}
		if (*v2 == '.') {
			v2++;
		}
	} while(*v1 != 0x00 || *v2 != 0x00);
	return 0;
}

int util::calculate_crc(void const* content, size_t content_length, int crc)
{
	static const unsigned int table[] = {
		0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
		0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
		0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
		0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,

		0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
		0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
		0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
		0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,

		0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
		0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
		0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
		0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,

		0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
		0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
		0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
		0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,

		0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
		0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
		0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
		0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,

		0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
		0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
		0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
		0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,

		0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
		0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
		0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
		0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,

		0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
		0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
		0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
		0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,

		0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
		0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
		0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
		0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,

		0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
		0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
		0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
		0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,

		0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
		0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
		0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
		0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,

		0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
		0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
		0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
		0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,

		0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
		0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
		0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
		0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,

		0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
		0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
		0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
		0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,

		0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
		0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
		0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
		0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,

		0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
		0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
		0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
		0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
	};

	unsigned char* buffer = (unsigned char*)content;
	while (content_length-- != 0) {
		crc = (crc >> 8) ^ table[(crc & 0xFF) ^ *buffer++];
	}
	return crc;
}

unsigned int util::calculate_hash(const char* a)
{
    unsigned int tmp=0;
    while(*a)
    {
        tmp|=1<<(*a);
        a++;
    }
    return tmp;
}

void util::splitstrbychar(const char* buf, char ch, std::vector<std::string>& arout,long size)
{
	std::string sValue = "";

	if (size == -1)
		size = strlen(buf);

	for (int i = 0; i < size; i++)
	{
		if (buf[i] == ch)
		{
			arout.push_back(sValue);
			sValue = "";
		}
		else
		{
			if (buf[i] == 0)
				return;
			sValue += buf[i];
		}
	}

	if (sValue.size() > 0)
	{
		arout.push_back(sValue);
	}
	return;
}

long util::local_datestamp()
{
	struct tm timeinfo = {0}, timeinfo1 = {0};
	time_t tmnow = time(NULL);
#ifndef _MSC_VER
	localtime_r(&tmnow,&timeinfo);
#else
    localtime_s(&timeinfo, &tmnow);
#endif
	if(timeinfo.tm_year == 0)
	{
		return 0;
	}
	
	timeinfo1.tm_year = timeinfo.tm_year;
	timeinfo1.tm_mon = timeinfo.tm_mon;
	timeinfo1.tm_mday = timeinfo.tm_mday;
	return mktime(&timeinfo1);
}

long util::string_to_datestamp(char* str)
{
	struct tm timeinfo1 = {0};

	if(sscanf(str,"%d-%d-%d %02d:%02d:%02d",&(timeinfo1.tm_year),&(timeinfo1.tm_mon),
			&(timeinfo1.tm_mday),&(timeinfo1.tm_hour),&(timeinfo1.tm_min),&(timeinfo1.tm_sec)) >= 3)
	{
		timeinfo1.tm_year -= 1900;
		timeinfo1.tm_mon -= 1;

		return mktime(&timeinfo1);
	}

	return 0;
}

void util::encode(char *pstr)
{
    int len = strlen(pstr);
    for (int i = 0; i < len; i++)
        *(pstr + i) = *(pstr + i) ^ i;
}

void util::decode(char *pstr)
{
    int len = strlen(pstr);
    for (int i = 0; i < len; i++)
        *(pstr + i) = *(pstr + i) ^ i;
}

}
