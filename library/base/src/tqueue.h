/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: tqueue.h
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __VBASE_TQUEUE_H__
#define __VBASE_TQUEUE_H__

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <atomic>
#include "base.h"
#include "lock.h"

namespace base {

#define kTQueueDefaultPageIndexBits 15
#define kTQueuePageMsgCount 65536
#define kTQueuePageMsgCountBits 16
#define kTQueuePageMsgIndexMask 0xFFFF

template<class T>
class tqueue
{
public:
	struct tmsg {
		T msg;
		short flag;
	};

public:
	/* 
	 * capacity of the message queue: (2^page_bits * 65536)
	 * in 32 bits os, page_bits range is [8, 31], 15 by default (default capacity: 2,147,483,648)
	 * in 64 bits os, page_bits range is [8, 47], 15 by default (default capacity: 2,147,483,648)
	 */
	tqueue(int pages_bits = kTQueueDefaultPageIndexBits)
		: initialized_(false)
		, page_indexes_(NULL)
		, page_counter_(NULL)
		, post_index_(0)
		, get_index_(0)
	{
		page_index_count_ = bint64(1) << pages_bits;
		page_index_mask_ = page_index_count_ - 1;

		init();
	}
	~tqueue()
	{
		release();
	}

public:
	/*
	 * single writer mode
	 * it's not multithread-safe, and should not be called concurrently
	 */
	inline bool post_msg(T& msg)
	{
		assert(initialized_);

		bint64 post_index = post_index_++;
		tmsg* page = get_msg_page(int((post_index >> kTQueuePageMsgCountBits) & page_index_mask_));

		page[post_index & kTQueuePageMsgIndexMask].msg = msg;
		base_memory_fence();
		page[post_index & kTQueuePageMsgIndexMask].flag = 1;

		return true;
	}
	/*
	 * multiple writers mode
	 * it's multithread-safe, and can be called anywhere and anytime as long as you want
	 */
	inline bool post_msg_mul(T& msg)
	{
		assert(initialized_);

		bint64 post_index = base_fetch_and_inc(&post_index_);
		tmsg* page = get_msg_page(int((post_index >> kTQueuePageMsgCountBits) & page_index_mask_));

		page[post_index & kTQueuePageMsgIndexMask].msg = msg;
		base_memory_fence();
		page[post_index & kTQueuePageMsgIndexMask].flag = 1;

		return true;
	}
	/*
	 * single reader mode 
	 * it's not multithread-safe, and should not be called concurrently
	 */
	inline bool get_msg(T& msg)
	{
		assert(initialized_);

		if (get_index_ >= post_index_) {
			return false;
		}

		bint64 page_index = int((get_index_ >> kTQueuePageMsgCountBits) & page_index_mask_);
		tmsg* page = get_msg_page(page_index);

		int msg_index = (get_index_ & kTQueuePageMsgIndexMask);
		if (page[msg_index].flag != 1) {
			return false;
		}

		msg = page[msg_index].msg;
		if (msg_index == kTQueuePageMsgIndexMask) {
			recycle_msg_page(page_index);
		}
		
		get_index_++;
		return true;
	}
	/*
	 * multiple readers mode
	 * it's multithread-safe, and can be called anywhere and anytime as long as you want
	 */
	inline bool get_msg_mul(T& msg)
	{
		assert(initialized_);

		while (true)
		{
			bint64 get_index = get_index_;
			if (get_index_ >= post_index_) {
				return false;
			}

			if (base_compare_and_exchange(&get_index_, get_index, get_index + 1) == get_index)
			{
				bint64 page_index = ((get_index >> kTQueuePageMsgCountBits) & page_index_mask_);
				tmsg* page = get_msg_page(page_index);
				int msg_index = (get_index & kTQueuePageMsgIndexMask);
				do {
					if (page[msg_index].flag == 1)
					{
						msg = page[msg_index].msg;
						/* if all messages in the page have been fetched, recycle message page */
						unsigned int count = base_fetch_and_inc(&page_counter_[page_index]);
						if (count == kTQueuePageMsgIndexMask) {
							recycle_msg_page(page_index);
						}
						return true;
					}
					else {
						sleep(1);
					}
				} while (true);
			}
		}
		return false;
	}

private:
	inline bool init()
	{
		mutex_scope ms(&mutex_);

		page_indexes_ = new tmsg*[page_index_count_];
		memset(page_indexes_, 0, sizeof(tmsg*) * page_index_count_);

		page_indexes_[0] = new tmsg[kTQueuePageMsgCount];
		memset(page_indexes_[0], 0, sizeof(tmsg) * kTQueuePageMsgCount);

		page_counter_ = new volatile unsigned int[page_index_count_];
		memset((void*)page_counter_, 0, sizeof(int) * page_index_count_);

		initialized_ = true;
		return true;
	}

	inline bool release()
	{
		mutex_scope ms(&mutex_);

		initialized_ = false;

		if (page_indexes_ != NULL) {
			for (int i = 0; i < page_index_count_; i++) {
				if (page_indexes_[i]) {
					delete[]page_indexes_[i];
					page_indexes_[i] = NULL;
				}
			}
			delete[]page_indexes_;
			page_indexes_ = NULL;
		}

		if (page_counter_ != NULL) {
			delete[]page_counter_;
			page_counter_ = NULL;
		}

		return true;
	}

	inline tmsg* get_msg_page(bint64 page_index)
	{
		assert(page_index >= 0 && page_index < page_index_count_);

		if (page_indexes_[page_index] == NULL) {
			mutex_scope ms(&mutex_);
			if (page_indexes_[page_index] == NULL) 
			{
				tmsg* page = new tmsg[kTQueuePageMsgCount];
				memset(page, 0, sizeof(tmsg) * kTQueuePageMsgCount);
				base_memory_fence();
				page_indexes_[page_index] = page;
			}
		}
		return page_indexes_[page_index];
	}

	inline void recycle_msg_page(bint64 page_index)
	{
		tmsg* page = page_indexes_[page_index];
		assert(page != NULL);

		mutex_scope ms(&mutex_);

		page_indexes_[page_index] = NULL;
		page_counter_[page_index] = 0; 
		
		delete[]page;
	}

	inline void sleep(unsigned int milli_seconds)
	{
#if defined(_MSC_VER)
		Sleep(milli_seconds);
#else
		struct timespec ts;
		ts.tv_sec = milli_seconds / 1000;
		ts.tv_nsec = (milli_seconds % 1000) * 1000000;
		nanosleep(&ts, NULL);
#endif
	}

private:
	bool initialized_;

	tmsg** page_indexes_;
	volatile unsigned int* page_counter_;

	volatile bint64 post_index_;
	volatile bint64 get_index_;
	bint64 page_index_count_;
	bint64 page_index_mask_;

	base::mutex mutex_;
};

}

#endif