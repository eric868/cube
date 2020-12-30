/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: test_aes.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __VBASE_TEST_AES_H__
#define __VBASE_TEST_AES_H__

#include "base/sigslot.h"

class test_aes
	: public sigslot::has_slots<>
{
public:
	test_aes();
	~test_aes();

public:
	void test_single_aes();
};

#endif
