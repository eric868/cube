/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: mqueue.h
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __VBASE_MQUEUE_H__
#define __VBASE_MQUEUE_H__

#include "lock.h"
#include "base.h"
#include <assert.h>

namespace base {
    
#define kDefaultPageMsgCountBits 16
#define kDefaultPageIndexBits 15

#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif

/* 
 * single read thread queue 
 * we can post message to the queue anytime and in any threads,
 * while we have to pop message from the queue in the same thread, as it's not multithread safe.
 */
template<class T>
class srt_queue
{
public:
    struct srt_message {
        T msg;
        short flag;
    };
    
public:
    /*
     * capacity of the message queue: (2^page_bits * 2^page_msg_bits)
     * in both 32 bits and 64 bits os, page_msg_bits is at most 31.
     * in 32 bits os, range of the page_index_bits is [8, 31], 15 by default (default capacity: 2,147,483,648)
     * in 64 bits os, range of the page_index_bits is [8, 47], 15 by default (default capacity: 2,147,483,648)
     */
    srt_queue(int page_msg_bits = kDefaultPageMsgCountBits,
              int page_index_bits = kDefaultPageIndexBits)
        : initialized_(false)
        , page_indexes_(NULL)
        , post_index_(0)
        , get_index_(0)
    {
        page_msg_bits_ = page_msg_bits;
        page_msg_count_ = int(1) << page_msg_bits_;
        page_msg_mask_ = page_msg_count_ - 1;
        
        page_index_count_ = bint64(1) << page_index_bits;
        page_index_mask_ = page_index_count_ - 1;
    }
    ~srt_queue() {
        release();
    }
    
public:
    inline bool init()
    {
        base::mutex_scope ms(&mutex_);
        
        if (initialized_) {
            return true;
        }
        
        page_indexes_ = new srt_message*[page_index_count_];
        memset(page_indexes_, 0, sizeof(srt_message *)* page_index_count_);
        
        page_indexes_[0] = new srt_message[page_msg_count_];
        memset(page_indexes_[0], 0, sizeof(srt_message) * page_msg_count_);
        
        initialized_ = true;
        return true;
    }
    /**
     * push a message, multithread safe
     * @param msg
     */
    inline bool push(const T& msg)
    {
        assert(initialized_);
        
        /* drop msg if the queue is full */
        if (((post_index_ - get_index_) >> page_msg_bits_) >= page_index_count_ - 1) {
            return false;
        }
        
		bint64 post_index = base_fetch_and_inc(&post_index_);
        srt_message* page = get_msg_page((post_index >> page_msg_bits_) & page_index_mask_);
        page[post_index & page_msg_mask_].msg = msg;
        base_memory_fence();
        page[post_index & page_msg_mask_].flag = 1;
        
        return true;
    }
    /**
     * pop a message, not multithread safe
     * @param msg
     */
    inline bool pop(T& msg)
    {
        assert(initialized_);
        
        if (get_index_ >= post_index_) {
            return false;
        }
        
        bint64 page_index = ((get_index_ >> page_msg_bits_) & page_index_mask_);
        srt_message* page = get_msg_page(page_index);
        
        if (page[get_index_ & page_msg_mask_].flag == 1) {
            msg = page[get_index_ & page_msg_mask_].msg;
            if ((get_index_ & page_msg_mask_) == page_msg_mask_) {
                delete[]page;
                page_indexes_[page_index] = NULL;
            }
            get_index_++;
            return true;
        }
        
        return false;
    }
    
    inline bint64 size() {
        return post_index_ - get_index_;
    }
    
protected:
    inline bool release()
    {
        base::mutex_scope ms(&mutex_);
        
        if (page_indexes_ != NULL) {
            for (int i = 0; i < page_index_count_; i++) {
                if (page_indexes_[i]) {
                    delete []page_indexes_[i];
                    page_indexes_[i] = NULL;
                }
            }
            delete []page_indexes_;
        }
        initialized_ = false;
        return true;
    }
    
    inline srt_message* get_msg_page(bint64 page_index)
    {
        assert(initialized_);
        assert(page_index >= 0 && page_index < page_index_count_);
        
        if (page_indexes_[page_index] == NULL) {
            base::mutex_scope ms(&mutex_);
            if (page_indexes_[page_index] == NULL) {
                srt_message* page = new srt_message[page_msg_count_];
                memset(page, 0, sizeof(srt_message) * page_msg_count_);
                page_indexes_[page_index] = page;
            }
        }
        return page_indexes_[page_index];
    }
    
private:
    bool initialized_;
    
    srt_message** page_indexes_;
    volatile bint64 post_index_;
    volatile bint64 get_index_;
    
    bint64 page_index_count_;
    bint64 page_index_mask_;
    
    int page_msg_count_;
    int page_msg_mask_;
    int page_msg_bits_;
    
    base::mutex mutex_;
};
    
/* 
 * durable queue
 * it would never remove message from queue
 */
template<class T>
class drb_queue
{
public:
    struct drb_message {
        T msg;
        short flag;
    };
    
public:
    /*
     * initial capacity of the queue is (page_msg_count * init_page_msg_count),
     * and will be extended dynamically if necessary.
     */
    drb_queue(int page_msg_count = 8,
              int init_page_msg_count = 2)
        : initialized_(false)
        , page_indexes_(NULL)
        , post_index_(0)
        , write_index_(0)
    {
        page_msg_bits_ = 1;
        while ((int(1) << page_msg_bits_) < page_msg_count) {
            page_msg_bits_++;
        }
        page_msg_count_ = int(1) << page_msg_bits_;
        page_msg_mask_ = page_msg_count_ - 1;
        
        page_index_count_ = init_page_msg_count;
    }
    ~drb_queue()
    {
        release();
    }
    
public:
    inline bool init()
    {
        base::mutex_scope ms(&mutex_);
        
        if (initialized_) {
            return true;
        }
        
        page_indexes_ = new drb_message * [page_index_count_];
        memset(page_indexes_, 0, sizeof(drb_message*) * page_index_count_);
        
        page_indexes_[0] = new drb_message[page_msg_count_];
        memset(page_indexes_[0], 0, sizeof(drb_message) * page_msg_count_);
        
        initialized_ = true;
        return true;
    }
    /**
     * push a message, which is multithread safe
     * @param msg
     */
    inline bool push(const T& msg)
    {
        assert(initialized_);
        
        bint64 post_index = base_fetch_and_inc(&post_index_);
        drb_message* page = get_msg_page(post_index >> page_msg_bits_);
        page[post_index & page_msg_mask_].msg = msg;
        base_memory_fence();
        page[post_index & page_msg_mask_].flag = 1;
        
        base_fetch_and_inc(&write_index_);
        
        return true;
    }
    /**
     * get the message from desired position, which isn't multithread safe
     * @param pos
     * @param msg[out]
     */
    inline bool get(bint64 pos, T& msg)
    {
        assert(initialized_);
        
        if (write_index_ <= pos) {
            return false;
        }
        
        bint64 page_index = (pos >> page_msg_bits_);
        drb_message* page = get_msg_page(page_index);
        
        if (page[pos & page_msg_mask_].flag == 1) {
            msg = page[pos & page_msg_mask_].msg;
            return true;
        }
        return false;
    }
    
    inline bint64 size() {
        return write_index_;
    }
    
protected:
    inline bool release()
    {
        base::mutex_scope ms(&mutex_);
        
        if (page_indexes_ != NULL) {
            for (int i = 0; i < page_index_count_; i++) {
                if (page_indexes_[i]) {
                    delete []page_indexes_[i];
                    page_indexes_[i] = NULL;
                }
            }
            delete []page_indexes_;
        }
        initialized_ = false;
        return true;
    }
    
    inline drb_message* get_msg_page(bint64 page_index)
    {
        assert(initialized_);
        
        if (page_index >= page_index_count_ || page_indexes_[page_index] == NULL)
        {
            base::mutex_scope ms(&mutex_);
            
            /* extend index pages */
            if (page_index >= page_index_count_)
            {
                bint64 extend_size = (page_index_count_ << 1);
                while (page_index > extend_size) {
                    extend_size <<= 1;
                }
                drb_message** new_page_indexes = new drb_message*[extend_size];
                memset(new_page_indexes, 0, sizeof(drb_message*) * extend_size);
                memcpy(new_page_indexes, page_indexes_, sizeof(drb_message*) * page_index_count_);
                base_memory_fence();
                page_index_count_ = extend_size;
                page_indexes_ = new_page_indexes;
            }
            if (page_indexes_[page_index] == NULL) {
                drb_message* page = new drb_message[page_msg_count_];
                memset(page, 0, sizeof(drb_message) * page_msg_count_);
                page_indexes_[page_index] = page;
            }
        }
        return page_indexes_[page_index];
    }
    
private:
    bool initialized_;
    
    drb_message** page_indexes_;
    volatile bint64 post_index_;
    volatile bint64 write_index_;
    
    bint64 page_index_count_;
    int page_msg_count_;
    int page_msg_mask_;
    int page_msg_bits_;
    
    base::mutex mutex_;
};

}

#endif
