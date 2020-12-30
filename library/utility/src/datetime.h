/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: datetime.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __UTILITY_DATETIME_H__
#define __UTILITY_DATETIME_H__ 

#include <stdio.h>
#include <string.h>
#include <string>
#include <sstream>
#include <iomanip>

#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#else
#include <sys/time.h>
#endif

namespace utility {

#pragma pack(1)
struct date {
	int year;
	int month;
	int day;

	date() : year(1900), month(1), day(1) {}

	date(int y, int m, int d) : year(y), month(m), day(d) {}

	date(char* date_str) {
		if (strlen(date_str) != 8) {
			year = 1900;
			month = 1;
			day = 1;
			return;
		}

		sscanf(date_str, "%4d%2d%2d", &year, &month, &day);
	}

	date(const date& date) : year(date.year), month(date.month), day(date.day) {}

	date& operator=(const date& date) {
		if (this != &date) {
			year = date.year;
			month = date.month;
			day = date.day;
		}

		return *this;
	}

	bool operator==(const date& date) const
	{
		if(year == date.year && 
			month == date.month && 
			day == date.day) 
		{ return true; }

		return false;
	}

	bool operator>(const date& date) const
	{
		if(year > date.year || 
			(year == date.year && month > date.month) || 
			(year == date.year && month == date.month && day > date.day)) {
			return true;
		}
			
		return false;
	}

	bool operator<(const date& date) const
	{
		if(year < date.year || 
			(year == date.year && month < date.month) || 
			(year == date.year && month == date.month && day < date.day)) {
			return true;
		}

		return false;
	}

	bool operator<=(const date& date) const
	{
		return !(*this > date);
	}

	bool operator>=(const date& date) const
	{
		return !(*this < date);
	}

	bool operator!=(const date& date) const
	{
		return !(*this == date);
	}

	std::string str() const {
		std::stringstream ss;
		ss<<year<<"-"
			<<std::setfill('0')<<std::setw(2)<<month<<"-"
			<<std::setfill('0')<<std::setw(2)<<day;
		return ss.str();
	}
};

struct time {
	int hour;
	int minute;
	int sec;
	int milsec;

	time() : hour(0), minute(0), sec(0), milsec(0) {}

	time(int h, int m, int s, int ms = 0) 
		: hour(h), minute(m), sec(s), milsec(ms) {}

	time(const time& t) 
		: hour(t.hour), minute(t.minute), sec(t.sec), milsec(t.milsec) {}

	time(char* time_str, int mil) {
		milsec = mil;
		if (strlen(time_str) != 8) {
			hour = 0;
			minute = 0;
			sec = 0;
			return;
		}

		sscanf(time_str, "%2d:%2d:%2d", &hour, &minute, &sec);
	}

	time& operator=(const time& t) {
		if (this != &t) {
			hour = t.hour;
			minute = t.minute;
			sec = t.sec;
			milsec = t.milsec;
		}

		return *this;
	}

	bool operator==(const time& t) const
	{
		if(hour == t.hour && minute == t.minute && 
			sec == t.sec && milsec == t.milsec) 
		{ return true; }

		return false;
	}

	bool operator>(const time& t) const
	{
		if(hour > t.hour || 
			(hour == t.hour && minute > t.minute) || 
			(hour == t.hour && minute == t.minute && sec > t.sec) || 
			(hour == t.hour && minute == t.minute && sec == t.sec && 
			milsec > t.milsec)) {
			return true;
		}

		return false;
	}

	bool operator<(const time& t) const
	{
		if(hour < t.hour || 
			(hour == t.hour && minute < t.minute) || 
			(hour == t.hour && minute == t.minute && sec < t.sec) || 
			(hour == t.hour && minute == t.minute && sec == t.sec && 
			milsec < t.milsec)) {
			return true;
		}

		return false;
	}

	bool operator<=(const time& t) const
	{
		return !(*this > t);
	}

	bool operator>=(const time& t) const
	{
		return !(*this < t);
	}

	bool operator!=(const time& t) const
	{
		return !(*this == t);
	}

	friend time operator+(const time& t1, const time& t2) {
		long t1_milsecs = ((t1.hour * 60 + t1.minute) * 60 + t1.sec) * 1000 + t1.milsec;
		long t2_milsecs = ((t2.hour * 60 + t2.minute) * 60 + t2.sec) * 1000 + t2.milsec;
		long ret_milsecs = t1_milsecs + t2_milsecs;
		int ret_hour = ret_milsecs / (60 * 60 * 1000);
		int ret_min = (ret_milsecs % (60 * 60 * 1000)) / (60 * 1000);
		int ret_sec = (ret_milsecs % (60 * 1000)) / 1000;
		int ret_milsec = ret_milsecs % 1000;

		return time(ret_hour, ret_min, ret_sec, ret_milsec);
	}

	friend time operator-(const time& t1, const time& t2) {
		long t1_milsecs = ((t1.hour * 60 + t1.minute) * 60 + t1.sec) * 1000 + t1.milsec;
		long t2_milsecs = ((t2.hour * 60 + t2.minute) * 60 + t2.sec) * 1000 + t2.milsec;
		long ret_milsecs = t1_milsecs - t2_milsecs;
		int ret_hour = ret_milsecs / (60 * 60 * 1000);
		int ret_min = (ret_milsecs % (60 * 60 * 1000)) / (60 * 1000);
		int ret_sec = (ret_milsecs % (60 * 1000)) / 1000;
		int ret_milsec = ret_milsecs % 1000;

		return time(ret_hour, ret_min, ret_sec, ret_milsec);
	}

	std::string str() const {
		std::stringstream ss;
		ss<<std::setfill('0')<<std::setw(2)<<hour<<":"
			<<std::setfill('0')<<std::setw(2)<<minute<<":"
			<<std::setfill('0')<<std::setw(2)<<sec<<"."
			<<std::setfill('0')<<std::setw(3)
			<<std::setiosflags(std::ios::right)<<milsec;
		return ss.str();
	}
};

struct date_time {
	date m_date;
	time m_time;

	date_time() {}

	//获取当前时间
	date_time(void* p) {
#ifdef WIN32
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		m_date = date(sys.wYear,sys.wMonth, sys.wDay);
		m_time = time(sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
#else
		struct timeval t_time;
		gettimeofday(&t_time, NULL);
		time_t t_date;
		time(&t_date);
		tm* local_t = localtime(&t_date);
		m_date = date(local_t->tm_year+1900, local_t->tm_mon+1, local_t->tm_mday);
		m_time = time(local_t->tm_hour, local_t->tm_min, local_t->tm_sec, t_time.tv_usec/1000);
#endif
	}

	date_time(const date& d, const time& t) : m_date(d), m_time(t) {}

	date_time(const date_time& date_time) 
		: m_date(date_time.m_date), m_time(date_time.m_time) {}

	date_time& operator=(const date_time& date_time) {
		if (this != &date_time) {
			m_date = date_time.m_date;
			m_time = date_time.m_time;
		}

		return *this;
	}

	bool operator==(const date_time& date_time) const
	{
		if(m_date == date_time.m_date && m_time == date_time.m_time) 
		{ return true; }

		return false;
	}

	bool operator>(const date_time& date_time) const
	{
		if(m_date > date_time.m_date || 
			(m_date == date_time.m_date && m_time > date_time.m_time)) 
		{ return true; }

		return false;
	}

	bool operator<(const date_time& date_time) const
	{
		if(m_date < date_time.m_date || 
			(m_date == date_time.m_date && m_time < date_time.m_time)) 
		{ return true; }

		return false;
	}

	bool operator!=(const date_time& date_time) const {
		return !(*this == date_time);
	}

	bool operator>=(const date_time& date_time) const {
		return !(*this < date_time);
	}

	bool operator<=(const date_time& date_time) const {
		return !(*this > date_time);
	}

	friend date_time operator+(const date_time& t1, const time& t2) {
		time t3 = t1.m_time + t2;
		return date_time(t1.m_date, t3);
	}

	friend date_time operator+(const time& t1, const date_time& t2) {
		return t2 + t1;
	}

	friend date_time operator-(const date_time& t1, const time& t2) {
		time t3 = t1.m_time - t2;
		return date_time(t1.m_date, t3);
	}

	std::string str() const {
		std::stringstream ss;
		ss<<m_date.str()<<" "<<m_time.str();
		return ss.str();
	}
};
#pragma pack()

}
#endif
