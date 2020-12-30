#ifndef __TEST_SINGLETON_H__
#define __TEST_SINGLETON_H__

#include <stdio.h>
#include <iostream>

#include "utility/singleton.h"

using namespace std;

class test_singleton {
    SINGLETON_ONCE(test_singleton);
public:
	void test()
	{
		cout << "this is singleton_test" << endl;
	}
};
SINGLETON_GET(test_singleton);
#endif