/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: reference_base.h
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __VBASE_REFERENCE_BASE_H__
#define __VBASE_REFERENCE_BASE_H__

#include <stdio.h>
#include "base.h"
namespace base {

class TEMPLATE_API reference_base
{
public:
	reference_base();
	virtual ~reference_base();

private:
    reference_base(const reference_base& x);
    reference_base& operator = (const reference_base& x);

public:
	virtual reference_base* retain();
	virtual void release();
	long retain_count() const;

private:
	volatile long ref_counter_;
};

template<class T>
class TEMPLATE_API ref_adapter
	: public reference_base
{
public:
	explicit ref_adapter(T* pointer)
		: pointer_(pointer) {
	}

	virtual ~ref_adapter() {
		if (pointer_ != NULL) {
			delete pointer_;
			pointer_ = NULL;
		}
	}

	T* get() {
		return pointer_;
	}

public:
	T* pointer_;
};

} // end of namespace base
#endif
