/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: test_rsa.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __VBASE_TEST_RSA_H__
#define __VBASE_TEST_RSA_H__

#include "base/alarm.h"
#include "base/sigslot.h"

class test_rsa
	: public sigslot::has_slots<>
{
public:
	test_rsa();
	~test_rsa();

public:
	void test_single_rsa();
};

#endif
