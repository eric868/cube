/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: reference_base.cpp
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#include "reference_base.h"
#if defined(_MSC_VER)
#include <Windows.h>
#else
#endif

namespace base {

reference_base::reference_base()
	: ref_counter_(1)
{
}

reference_base::~reference_base()
{

}

reference_base* reference_base::retain()
{
	base_fetch_and_inc(&ref_counter_);
	return this;
}

void reference_base::release()
{
	long ref_count = base_fetch_and_dec(&ref_counter_);
	if (ref_count == 1) {
		delete this;
	}
}

long reference_base::retain_count() const
{
	return ref_counter_;
}

}