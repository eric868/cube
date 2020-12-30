/*****************************************************************************
 Nautilus Database Copyright (c) 2015. All Rights Reserved.

 FileName: test_unidb.h
 Version: 1.0
 Date: 2015.9.6

 History:
 jeffguo     2015.9.6   1.0     Create
 ******************************************************************************/

#ifndef __NAUT_DATABASE_TEST_UNIDB_H_
#define __NAUT_DATABASE_TEST_UNIDB_H_


#include "iconv/iconv.h"
#include "base/thread.h"
#include "database/unidb.h"
#include "base/dictionary.h"
#include "base/dtrans.h"
#include <stdio.h>


using namespace base;

class test_unidb
{
public:
	test_unidb();
	virtual ~test_unidb();

public:
	void start_multi_thread();
	void start_test();
	void insert_db_test();
	static void test_query(void* args);

public:
	void start_redis_test();

private:
	void check_charset();
	void print_hex(const char* text);

private:
	naut::unidb* db_;
	thread* t1;
	thread* t2;

};

#endif /* TESTUNIDB_H_ */
