#ifndef __TEST_NOCOPYABLE_H__
#define __TEST_NOCOPYABLE_H__

#include <stdio.h>
#include <iostream>

#include "utility/nocopyable.h"

using namespace std;

class test_nocopyable 
    : public utility::nocopyable {
public:
    test_nocopyable() {}
    ~test_nocopyable() {}

public:
    void test()
    {
        cout << "this is test_nocopyable" << endl;
    }
};

#endif
