/*****************************************************************************
 Nautilus Database Copyright (c) 2015. All Rights Reserved.

 FileName: main.cpp
 Version: 1.0
 Date: 2015.9.6

 History:
 jeffguo     2015.9.6   1.0     Create
 ******************************************************************************/

#include "test_unidb.h"

int main(int argc, char* argv[])
{
	test_unidb t;
	// t.start_test();
	// t.test_query();
	// t.start_multi_thread();
//	 t.insert_db_test();
	t.start_redis_test();
	return 0;
}



