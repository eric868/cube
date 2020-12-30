/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: shareptr.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __UTILITY_SHAREPTR_H__
#define __UTILITY_SHAREPTR_H__

#include "utility.h"

namespace utility {

template<class T>
class shareptr
{
public:
    class pointer_count 
	{
	public:
		pointer_count(T*  p_obj)
			: obj_ptr(p_obj), ref_count(1)
		{
		}

		~pointer_count()
		{
			if (obj_ptr) {
				delete obj_ptr;
			}
		}

        void add_ref()
		{
			base_fetch_and_inc(&ref_count);
		}

        void release_ref()
		{
			if(refCount <= 1) {   
				delete this;   
				return;
			} else {
				base_fetch_and_dec(&ref_count);
			}
		}

        int use_count()
		{
			return ref_count;
		}

	private:
		volatile int ref_count;
		T* obj_ptr;
    };
    
	shareptr()
		: p_obj_(0)
	{
		p_count_ = new pointer_count(0);
	}

	explicit shareptr(T*  ptr)
		: p_obj_(ptr)
	{
		p_count_ = new pointer_count(ptr);
	}

	shareptr(const shareptr<T> & source)
	{
		p_count_ = source.p_count_;    
		p_count_->add_ref();
		p_obj_ = source.p_obj_;
	}

	virtual ~shareptr()
	{
		p_count_->release_ref();
	}

	shareptr<T> & operator=(const shareptr<T> & source)
	{
		if(this == &source) {
			return *this;
		}
		p_count_->release_ref();

		p_count_ = source.p_count_;
		p_obj_ = source.p_obj_;
		if(p_count_) {
			p_count_->add_ref();
		}
		return *this;
	}

    int get_ref_count() { return p_count_->use_count(); }

    T* operator->() { return p_obj_; }

	const T* operator->() const { return p_obj_; }

    T& operator*() { return *(p_obj_); }

	operator bool() const { return (p_obj_ != 0); }

	bool operator!() const { return (p_obj_ == 0); }

	bool operator<(const shareptr<T>& a) const { return this->p_obj_ < a.p_obj_; }

    inline T* get_pointer() {return p_obj_;}
	inline const T* get_pointer() const { return p_obj_;}

private:
	pointer_count* p_count_;
    T* p_obj_;
};

}
#endif
