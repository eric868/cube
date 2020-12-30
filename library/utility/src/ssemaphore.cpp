#include "ssemaphore.h"

namespace utility {
	ssemaphore::ssemaphore(long initial/* = 0 */, const char *sem_name/*=NULL*/)
	{
#if defined(WIN32)
		sem_=CreateSemaphore(0,initial,0x7fffffff,0);
		if(sem_==NULL) {
			sem_=INVALID_HANDLE_VALUE;
		}
#else
		if(sem_name) {
			sem_name=sem_name;
		}
		sem_=sem_open(sem_name, O_CREAT, 0644, initial);
#endif
	}

#if defined(WIN32)
	bool ssemaphore::release(int counter_)
	{
		if(INVALID_HANDLE_VALUE == sem_) {
			return false;
		}
		return ReleaseSemaphore(sem_,counter_,NULL)==TRUE;
	}

	bool ssemaphore::wait(int millisecond_)
	{
		if(INVALID_HANDLE_VALUE == sem_) {
			return false;
		}
		return WAIT_OBJECT_0==WaitForSingleObject(sem_,millisecond_);
	}

#else

	bool ssemaphore::release()
	{
		return sem_post(sem_)==0;
	}


	bool ssemaphore::wait(int millisecond_)
	{
		if(-1==millisecond_) {
			return 0==sem_wait(sem_);
		} else {
			struct timespec ts;
			ts.tv_sec= millisecond_/1000;
			ts.tv_nsec=(millisecond_%1000)*1000000L;

			/* Try to lock SSemaphore */
			return 0 == sem_timedwait(sem_, &ts);
		}
	}

#endif

	bool ssemaphore::try_wait()
	{

#if defined(WIN32)
		if(INVALID_HANDLE_VALUE==sem_) {
			return false;
		}
		if(WAIT_OBJECT_0==WaitForSingleObject(sem_,0)) {
			return true;
		} else {
			return false;
		}
#else	
		if(NULL==sem_) {
			return false;
		}
		if(sem_trywait(sem_)==0) {
			return true;
		} else {
			return false;
		}
#endif
	}

	ssemaphore::~ssemaphore()
	{
#if defined(WIN32)
		if(INVALID_HANDLE_VALUE!=sem_) {
			CloseHandle(sem_);
		}			
#else	
		sem_close(sem_);
		sem_unlink(sem_name.c_str());
#endif
	}
}
