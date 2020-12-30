/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: spinlock.cpp
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#include "spinlock.h"

namespace utility {

	spinlock::spinlock(int spin_count/* = 10000*/)
	{
#if defined(WIN32)  
		//��תspin_count��ʱ�����ں��������
		InitializeCriticalSectionAndSpinCount(&m_critical_section, spin_count);
#else  
		//PTHREAD_PROCESS_PRIVATE:
		//����ʼ�������������߳����ڵĽ����ڵ��̲߳��ܹ�ʹ�ø���������
		int rs = ::pthread_spin_init(&m_spinlock, PTHREAD_PROCESS_PRIVATE);  
		assert(0 == rs);  
#endif  
	}

	spinlock::~spinlock()
	{
#if defined(WIN32)  
		DeleteCriticalSection(&m_critical_section);
#else  
		int rs = ::pthread_spin_destroy(&m_spinlock);  
		assert(0 == rs);  
#endif  
	}

}
