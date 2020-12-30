/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: thread.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __UTILITY_SPENDTIME_H__
#define __UTILITY_SPENDTIME_H__

#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include "utility.h"

class COMMON_API spendtime
{
public:
	spendtime() 
		: m_bsupported(false)
	{
#ifdef WIN32
		// Read the counter frequency (in Hz) and an initial counter value.
		if (QueryPerformanceFrequency(&mTicksPerSecond) && 
			QueryPerformanceCounter(&mCounterStartValue)) {
			m_bsupported = true;
		}
#else
		m_bsupported = true;
#endif
	}

	void start()
	{
#ifdef WIN32
		QueryPerformanceCounter(&mCounterStartValue);
#else
		gettimeofday(&t1, NULL);
#endif
	}

	void stop()
	{
#ifdef WIN32
		QueryPerformanceCounter(&mCounterEndValue);
#else
		gettimeofday(&t2, NULL);
#endif
	}

	bool get_supported() const
	{
		return m_bsupported;
	}

	float get_spendseconds() const
	{
#ifdef WIN32
		const float elapsedTicks(static_cast<float>
			(mCounterEndValue.QuadPart - mCounterStartValue.QuadPart));
		const float ticksPerSecond(static_cast<float>
			(mTicksPerSecond.QuadPart));
		return (elapsedTicks / ticksPerSecond);
#else
		return (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec) * 1e-6f;
#endif
	}

private:
	spendtime(const spendtime &other);
	spendtime &operator=(const spendtime &other);

	bool m_bsupported;

#ifdef WIN32
	LARGE_INTEGER mTicksPerSecond;
	LARGE_INTEGER mCounterStartValue;
	LARGE_INTEGER mCounterEndValue;
#else
	timeval t1, t2;
#endif

};
#endif
