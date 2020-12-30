/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: lock.cpp
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#include "lock.h"
#ifdef _MSC_VER
#include <crtdbg.h>
#else
#include <stdio.h>
#include <pthread.h>
#endif

namespace base {

mutex::mutex()
	: initialized_(false)
{
	init();
}

mutex::~mutex()
{
	exit();
}

void mutex::init()
{
#if defined(_MSC_VER)
	InitializeCriticalSection(&cs_);
#else
	// PTHREAD_MUTEX_RECURSIVE
	pthread_mutexattr_init(&cs_attr_);
	pthread_mutexattr_settype(&cs_attr_, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&cs_, &cs_attr_);
#endif

	initialized_ = true;
}

void mutex::exit()
{
	acquire();
	initialized_ = false;
	release();

#if defined(_MSC_VER)
	DeleteCriticalSection(&cs_);
#else
	pthread_mutex_destroy(&cs_);
	pthread_mutexattr_destroy(&cs_attr_);
#endif
}

void mutex::acquire()
{
	if (!initialized_) {
		return;
	}

#if defined(_MSC_VER)
	EnterCriticalSection(&cs_);
#else
	pthread_mutex_lock(&cs_);
#endif
}

int mutex::try_acquire()
{
	if (!initialized_) {
		return -1;
	}
#if defined(_MSC_VER)
	EnterCriticalSection(&cs_);
	return 0;
#else
	return pthread_mutex_trylock(&cs_);
#endif
}

void mutex::release()
{
#if defined(_MSC_VER)
	LeaveCriticalSection(&cs_);
#else
	pthread_mutex_unlock(&cs_);
#endif
}

mutex_scope::mutex_scope(mutex* m)
	: mutex_(m)
{
	assert(mutex_ != NULL);
	mutex_->acquire();
}

mutex_scope::~mutex_scope()
{
	mutex_->release();
}

/****************************************************************************/

rwlock::rwlock()
{
	init();
}

rwlock::~rwlock()
{
	release();
}

void rwlock::lock_read()
{
#ifdef _MSC_VER
	bool fNotifyReaders = false;

	EnterCriticalSection(&cs_);

	// Block readers from acquiring the lock if
	// there are any writers waiting or if a writer

	if ((numWritersWaiting_ > 0) || (HIWORD(activeWriterReaders_) > 0))
	{
		++numReadersWaiting_;

		_ASSERTE(numReadersWaiting_ > 0);

		while (true)
		{
			ResetEvent(hReadyToRead_);

			LeaveCriticalSection(&cs_);

			WaitForSingleObject(hReadyToRead_, INFINITE);

			EnterCriticalSection(&cs_);

#if defined(TRACK_READER_RACES)
			++numReaderWakeups_;
#endif

			// The reader is only allowed to read if there aren't
			// any writers waiting and if a writer doesn't own the
			// lock.
			if ((numWritersWaiting_ == 0) && (HIWORD(activeWriterReaders_) == 0))
			{
				break;
			}

#if defined(TRACK_READER_RACES)
			++numReaderRacesLost_;
#endif
		}
		// Reader is done waiting.
		--numReadersWaiting_;

		_ASSERTE(numReadersWaiting_ >= 0);

		// Reader can read.
		++activeWriterReaders_;
	}
	else
	{
		// Reader can read.
		if ((++activeWriterReaders_ == 1) && (numReadersWaiting_ != 0))
		{
			// Set flag to notify other waiting readers
			// outside of the critical section
			// so that they don't when the threads
			// are dispatched by the scheduler they
			// don't immediately block on the critical
			// section that this thread is holding.
			fNotifyReaders = true;
		}
	}

	_ASSERTE(HIWORD(activeWriterReaders_) == 0);

	LeaveCriticalSection(&cs_);

	if (fNotifyReaders)
	{
		SetEvent(hReadyToRead_);
	}
#else
	pthread_rwlock_rdlock(&rwlock_);
#endif
}

void rwlock::unlock_read()
{
#ifdef _MSC_VER
	EnterCriticalSection(&cs_);

	// Assert that the lock isn't held by a writer.
	_ASSERTE(HIWORD(activeWriterReaders_) == 0);

	// Assert that the lock is held by readers.
	_ASSERTE(LOWORD(activeWriterReaders_ > 0));

	// Decrement the number of active readers.
	if (--activeWriterReaders_ == 0)
	{
		ResetEvent(hReadyToRead_);
	}

	// if writers are waiting and this is the last reader
	// hand owneership over to a writer.
	if ((numWritersWaiting_ != 0) && (activeWriterReaders_ == 0))
	{
		// Decrement the number of waiting writers
		--numWritersWaiting_;

		// Pass ownership to a writer thread.
		activeWriterReaders_ = MAKELONG(0, 1);
		ReleaseSemaphore(hReadyToWrite_, 1, NULL);
	}

	LeaveCriticalSection(&cs_);
#else
	pthread_rwlock_unlock(&rwlock_);
#endif
}

void rwlock::lock_write()
{
#ifdef _MSC_VER
	EnterCriticalSection(&cs_);

	// Are there active readers?
	if (activeWriterReaders_ != 0)
	{
		++numWritersWaiting_;

		_ASSERTE(numWritersWaiting_ > 0);

		LeaveCriticalSection(&cs_);

		WaitForSingleObject(hReadyToWrite_, INFINITE);

		// Upon wakeup theirs no need for the writer
		// to acquire the critical section.  It
		// already has been transfered ownership of the
		// lock by the signaler.
	}
	else
	{
		_ASSERTE(activeWriterReaders_ == 0);

		// Set that the writer owns the lock.
		activeWriterReaders_ = MAKELONG(0, 1);;

		LeaveCriticalSection(&cs_);
	}
#else
	pthread_rwlock_wrlock(&rwlock_);
#endif
}

void rwlock::unlock_write()
{
#ifdef _MSC_VER
	bool fNotifyWriter = false;
	bool fNotifyReaders = false;

	EnterCriticalSection(&cs_);

	// Assert that the lock is owned by a writer.
	_ASSERTE(HIWORD(activeWriterReaders_) == 1);

	// Assert that the lock isn't owned by one or more readers
	_ASSERTE(LOWORD(activeWriterReaders_) == 0);

	if (numWritersWaiting_ != 0)
	{
		// Writers waiting, decrement the number of
		// waiting writers and release the semaphore
		// which means ownership is passed to the thread
		// that has been released.
		--numWritersWaiting_;
		fNotifyWriter = true;
	}
	else
	{
		// There aren't any writers waiting
		// Release the exclusive hold on the lock.
		activeWriterReaders_ = 0;

		// if readers are waiting set the flag
		// that will cause the readers to be notified
		// once the critical section is released.  This
		// is done so that an awakened reader won't immediately
		// block on the critical section which is still being
		// held by this thread.
		if (numReadersWaiting_ != 0)
		{
			fNotifyReaders = true;
		}
	}

	LeaveCriticalSection(&cs_);

	if (fNotifyWriter)
	{
		ReleaseSemaphore(hReadyToWrite_, 1, NULL);
	}
	else if (fNotifyReaders)
	{
		SetEvent(hReadyToRead_);
	}
#else
	pthread_rwlock_unlock(&rwlock_);
#endif
}

void rwlock::init()
{
#ifdef _MSC_VER
	numWritersWaiting_ = 0;
	numReadersWaiting_ = 0;
	activeWriterReaders_ = 0;
#if defined(TRACK_READER_RACES)
	numReaderRacesLost = 0;
	numReaderWakeups = 0;
#endif
	InitializeCriticalSection(&cs_);
	hReadyToRead_ = CreateEvent(NULL,
		TRUE,
		FALSE,
		NULL);
	if (hReadyToRead_ == NULL) {
		//throw SystemError(GetLastError());
	}

	hReadyToWrite_ = CreateSemaphore(NULL,
		0,
		1,
		NULL);

	_ASSERTE(hReadyToWrite_ != NULL);
	if (hReadyToWrite_ == NULL)
	{
		DWORD lastError = GetLastError();
		CloseHandle(hReadyToRead_);
		//throw SystemError(lastError);
	}
#else
	int res;
	res = pthread_rwlock_init(&rwlock_, NULL); 
	if (res != 0) {
		perror("rwlock initialization failed");
	}
#endif
}

void rwlock::release()
{
#ifdef _MSC_VER
	if (hReadyToRead_ != NULL)
		CloseHandle(hReadyToRead_);

	if (hReadyToWrite_ != NULL)
		CloseHandle(hReadyToWrite_);

	DeleteCriticalSection(&cs_);
#else
	pthread_rwlock_destroy(&rwlock_);
#endif
}

}
