/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: test_aes.cpp
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#include "test_md5.h"
#include <stdio.h>
#include "base/md5.h"

using namespace base;

test_md5::test_md5()
{

}

test_md5::~test_md5()
{

}

void test_md5::test_single_md5()
{
	std::string eostr = "123456abcdEFG";
	std::string ecipher = md5(eostr.c_str());
	printf("eostr:   %s, %d\n", eostr.c_str(), eostr.length());
	printf("ecipher: %s, %d\n", ecipher.c_str(), ecipher.length());
}
