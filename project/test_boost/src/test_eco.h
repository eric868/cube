/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: test_eco.h
Version: 1.0
Date: 2016.1.13

History:
eric     2016.1.13   1.0     Create
******************************************************************************/

#ifndef TEST_ECO_H_
#define TEST_ECO_H_

#include <chrono>
#include <thread>
#include "eco/filesystem/Operations.h"

class Context
{
public:
    inline Context()
    {}

    inline const uint32_t get_type() const
    {
        return type;
    }
private:
    uint32_t type;
    std::string data;
};

class TestEco
{
public:
    TestEco(){}
    ~TestEco(){}

    void TestFileSystem()
    {
        std::string file = "./logs/20180424_0.log";
        eco::filesystem::ReadFile rfile(file, "a+" );
        std::cout << rfile.data() << std::endl;

        eco::filesystem::File wfile("./logs/backup.log", "w+");
        wfile.write(rfile.data().c_str(), rfile.data().size());
        wfile.flush();
    }

    void TestDispatchServer()
    {
    }
};

#endif
