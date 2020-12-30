/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: test_rsa.cpp
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#include "test_rsa.h"
#include <stdio.h>
#include "base/rsa.h"

using namespace base;

test_rsa::test_rsa()
{

}

test_rsa::~test_rsa()
{

}

void test_rsa::test_single_rsa()
{
	base::rsa r;
	r.init_key(256);

	const char* plain = "1234567ABCabc";
	std::string cipher = "bc2f64e9d709cd30140b0e4e835bfe99421b8d036b4a87c6217c7696f8d4188a";
	std::string cipher1;
	std::string oplain;
	std::string oplain1;

	r.encrypt(plain, cipher);
	r.encrypt(plain, cipher1);
	r.decrypt(cipher.c_str(), oplain);
	r.decrypt(cipher1.c_str(), oplain1);

	std::string public_key;
	r.export_public_key(public_key);

	printf("plain:  %s\n", plain);
	printf("cipher: %s\n", cipher.c_str());
	printf("oplain: %s\n", oplain.c_str());
	printf("cipher1: %s\n", cipher1.c_str());
	printf("oplain1: %s\n", oplain1.c_str());
	printf("public-key: %s\n", public_key.c_str());
}
