/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: main.cpp
Version: 1.0
Date: 2016.1.13

History:
eric     2018.4.27   1.0     Create
******************************************************************************/
#include <iostream>
#include <sstream>
#include <string>
#include <set>
#include <functional>
#include <chrono>
#include <thread>
#include <ctime>
#include <regex>
#include <tuple>
#include "libeco/project.h"
#include "libeco/dispatch_server.h"
#include "libeco/monitor.h"
#include "libeco/event.h"
#include "libeco/memory_pool.h"
#include "libeco/file.h"
#include "libeco/lexical_cast.h"
#include "libeco/any.h"
#include "libeco/util.h"
#include "libeco/md5.h"
#include "libeco/memoryex.h"

using namespace std;

using protocal_struct = std::map<std::string, eco::Any>;

class Test
{
    ECO_VALUE_API(Test)
public:
    Test& set(IN const char* str);
    const char* get_str() const;
};

class Test::Impl
{
    ECO_IMPL_INIT(Test)
public:
    std::string data_;
public:
    void set_data(IN const char* str)
    {
        data_ = str;
    }
};

ECO_VALUE_IMPL(Test);
Test& Test::set(IN const char* str)
{
    m_impl->set_data(str);
    return *this;
}

const char* Test::get_str() const 
{
    cout << __FUNCTION__ << endl;
    return m_impl->data_.c_str();
}

void fun_default(IN protocal_struct& msg)
{
    std::cout << __FUNCTION__
        << " message_type:" << msg["type"].AnyCast<int>()
        << " sessionid:" << msg["sessionid"].AnyCast<char>()
        << " price:" << msg["price"].AnyCast<double>()
        << " data:" << msg["data"].AnyCast<std::string>()
        << std::endl;
}

void fun_eco(IN protocal_struct& msg)
{
    std::cout << __FUNCTION__
        << " message_type:" << msg["type"].AnyCast<int>()
        << " sessionid:" << msg["sessionid"].AnyCast<char>()
        << " price:" << msg["price"].AnyCast<double>()
        << " data:" << msg["data"].AnyCast<std::string>()
        << std::endl;
}

template <typename T, typename... argv>
int print(T first, argv... rest)
{
    return first + print(std::forward<argv>(rest)...);
}

template <typename T>
int print(T value)
{
    return value;
}

class Single
{
ECO_SINGLETON(Single)
public:
    void print()
    {
        cout << "hello singleton" << endl;
    }
};
ECO_SINGLETON_GET(Single)

struct ProtocalStructComp
{
    bool operator() (const protocal_struct& lhs,const protocal_struct& rhs) const
    {
        auto it_l = lhs.find("type");
        auto it_r = rhs.find("type");
        assert(it_l != lhs.end() && it_r != rhs.end());
        eco::Any anyl= it_l->second;
        eco::Any anyr = it_r->second;
        if (anyl.AnyCast<int>() < anyr.AnyCast<int>()) {
            return true;
        }
        else {
            return false;
        }
    }
};

bool GetContractInfo(IN const std::string& str, OUT protocal_struct& msg)
{
    auto it = str.find('&');
    if (it == std::string::npos) return false;
    std::size_t i = 0;
    for (i = 0; i < str.size(); ++i) {
        if (isdigit(str[i])) break;
    }
    msg["commodityno"] = str.substr(0, i);
    msg["contractno1"] = str.substr(i, it - i);
    std::size_t j = 0;
    for (j = it+1; j < str.size(); ++j) {
        if (isdigit(str[j])) break;
    }
    msg["contractno2"] = str.substr(j);

    return true;
}


int main(int argc, char **argv)
{
    std::set<std::string> holidays;
    holidays.insert("2020-01-04");
    holidays.insert("2018-08-08");
    holidays.insert("2019-01-01");
    holidays.insert("2030-05-05");
    for (const auto val : holidays) {
        cout << val << endl;
    }

    auto it = holidays.lower_bound("2040-01-01");
    cout << *it << endl;

    std::chrono::steady_clock::now().time_since_epoch();

    std::time_t t = eco::util::string_to_datestamp("2019-08-06");

    std::string definiteday = eco::util::date_string(t);

    std::string desc = "<**>ddd<**>aaa<**>";
    eco::StringReplace(desc, "<**>", "#");
    std::vector<std::string> ret;
    eco::StringSplit(desc, "#", 3, ret);

    std::string updatetime = eco::util::local_date_time_string();

    string str = "181123084338.475739";
    std::string date, time;
    if (str.size() > 12) {
        char year[2], month[2], day[2], hour[2], minute[2], second[2];
        sscanf(str.c_str(), "%2s%2s%2s%2s%2s%2s", year, month, day, hour, minute, second);
        date = std::string("20") + year + std::string("-") + month + std::string("-") + day;
        time = hour + std::string(":") + minute + std::string(":") + second;
    }

    GetSingle().print();

    eco::DispatchServer<int64_t, protocal_struct> dispatch;
    dispatch.SetDefault(fun_default);
    dispatch.SetDispatch(10001, fun_eco);
    dispatch.Start(1);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    for (int i = 0; i < 5; ++i)
    {
        protocal_struct msg;
        msg["type"] = 10000 + i;
        msg["sessionid"] = 'c';
        msg["price"] = 99.999;
        msg["data"] = std::string("hello world");
        dispatch.Post(msg);
    }

    getchar();
    return 0;
}
