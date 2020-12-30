#include <stdio.h>
#include <assert.h>
#include <signal.h>
#include <string.h>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <memory>
#include <vector>
#include <set>
#include <queue>
#include <stack>
#include <deque>
#include <map>
#include <unordered_map>
#include <functional>
#include <numeric>
#include <thread>
#include <chrono>
#include <mutex>
#include <exception>
#include <iomanip>
#include <algorithm>
#include <utility>
#include <tuple>
#include <chrono>
#include <random>
#include <time.h>
#include <type_traits>
#include <utility>
#include <stdexcept>
#include <typeindex>
#include <exception>
#include <future>

#include "datacore/esunny_datatype_core.h"

using namespace std;
struct EsunnyTypeWatchPosition
{
    EsunnyTypeWatchPosition()
    {
    }

    esunnydatatype::EsunnyTypeRiskInvestorID InvestorID;
    esunnydatatype::EsunnyTypeInstrumentID InstrumentID;
    esunnydatatype::EsunnyEnumDirection Direction;

    esunnydatatype::EsunnyTypeVolume YestodayVolume;
    esunnydatatype::EsunnyTypeVolume CloseYestodayVolume;
    esunnydatatype::EsunnyTypeVolume TodayOpenVolume;
    esunnydatatype::EsunnyTypeVolume CloseTodayVolume;
    esunnydatatype::EsunnyTypeVolume OldYestodayVolume;
    esunnydatatype::EsunnyTypeVolume OldCloseYestodayVolume;
    esunnydatatype::EsunnyTypeVolume OldTodayOpenVolume;
    esunnydatatype::EsunnyTypeVolume OldCloseTodayVolume;

    typedef std::tuple<esunnydatatype::EsunnyTypeRiskInvestorID, esunnydatatype::EsunnyTypeInstrumentID,
        esunnydatatype::EsunnyEnumDirection> key_type;
    key_type GetKey() const
    {
        return std::make_tuple(InvestorID, InstrumentID, Direction);
    }
};


template <typename T1, typename T2, 
    template<typename T, typename = std::allocator<T> > class Container>
class Relation
{
private:
    Container<T1> dom1;
    Container<T2> dom2;
};

class MyClass
{
public:
    MyClass(int id, const std::string& name) 
        : id_(id),
        name_(name)
    {}

    void test()
    {
        std::cout << "id: " << id_ 
            << " name: " << name_ 
            << std::endl;
    }
public:
    int id_;
    std::string name_;
};

void test(char num[100] = "hello")
{
    std::cout << sizeof(num) << std::endl;
    std::cout << __FUNCTION__ << std::endl;
}

std::string GetFutureByProduct(const std::string& str)
{
	std::string tmp = str;
	std::string productid = str.substr(0, 2);
	if (productid == "IO") {
		tmp.replace(0, 2, "IF");
	}
	else if (productid == "HO"){
		tmp.replace(0, 2, "IH");
	}

	return tmp;
}

struct MyStruct
{
    char a;
    char e;
    char f;
    short d;
    int b;
    short c;
};

//删除字符串中空格，制表符tab等无效字符
std::string Trim(std::string& str)
{
    //str.find_first_not_of(" \t\r\n"),在字符串str中从索引0开始，返回首次不匹配"\t\r\n"的位置
    str.erase(0, str.find_first_not_of(" \t\r\n"));
    str.erase(str.find_last_not_of(" \t\r\n") + 1);
    return str;
}

std::map<int, int> map_tmp;
void ref_func(int &i)
{
    auto it = map_tmp.find(1);
    it->second = 300;
}

class Base
{
public:
    virtual void test()
    {
        std::cout << "id: " << id_ << " name: " << name_ << std::endl;
    }
public:
    int id_;
    std::string name_;
};

class Child : public Base
{
public:
    Child(int id, const std::string& name)
        : child_id_(id),
        child_name_(name) {}

    virtual void test()
    {
        std::cout << "child_id_: " << child_id_ << " child_name_: " << child_name_ << std::endl;
    }
private:
    int child_id_;
    std::string child_name_;
};

void rfunc(int&& num)
{
    cout << num << endl;
}

bool IsOption(const std::string& instrument)
{
    std::size_t i = 0;
    for (i = 0; i < instrument.size(); i++) {
        if (isdigit(instrument[i])) break;
    }
    std::size_t j = 0;
    for (j = i; j < instrument.size(); j++) {
        if (!isdigit(instrument[j])) break;
    }
    if (j == instrument.size()) return false;

    std::size_t x = 0;
    for (x = j; x < instrument.size(); x++) {
        if (isalpha(instrument[x])) break;
    }
    char option_type = instrument[x];
    if (option_type != 'C' && option_type != 'P') return false;

    return true;
}

int main(int argc, char *argv[])
{
    std::tuple<std::string, std::string, int> tup = std::make_tuple("10000049", "SR901", 2);

    auto investorid = std::get<0>(tup);
    auto future = std::get<1>(tup);
    auto productclass = std::get<2>(tup);


    std::vector<int> vec = { 0, 1, 2 };
    std::cout << vec[2];

    std::ifstream fin("t_user.csv"); //打开文件流操作
    std::string line;
    while (getline(fin, line))   //整行读取，换行符“\n”区分，遇到文件尾标志eof终止读取
    {
        std::cout << "原始字符串：" << line << std::endl; //整行输出
        if (line[line.size() - 1] == ',') {
            line.push_back(' ');
        }
        std::istringstream sin(line); //将整行字符串line读入到字符串流istringstream中
        std::vector<std::string> fields; //声明一个字符串向量
        std::string field;
        while (std::getline(sin, field, ',')) //将字符串流sin中的字符读入到field字符串中，以逗号为分隔符
        {
            fields.push_back(field); //将刚刚读取的字符串添加到向量fields中
        }
        std::string name = Trim(fields[2]); //清除掉向量fields中第一个元素的无效字符，并赋值给变量name
        std::string age = Trim(fields[3]); //清除掉向量fields中第二个元素的无效字符，并赋值给变量age
        std::string birthday = Trim(fields[4]); //清除掉向量fields中第三个元素的无效字符，并赋值给变量birthday
        std::cout << "处理之后的字符串：" << name << "\t" << age << "\t" << birthday << std::endl;
    }

    std::vector<char> sql;
    sql.resize(1024);
    snprintf(sql.data(), 1024, "hello world");
    std::cout << sql.data() << std::endl;

    std::chrono::time_point<std::chrono::system_clock> begin_clock = std::chrono::system_clock::now();

    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now +
        std::chrono::hours(24 * 5));
    struct tm ti = *std::localtime(&time);

    char date_time_buf[32];
    sprintf(date_time_buf, "%04d-%02d-%02d %02d:%02d:%02d", 1900 + ti.tm_year,
        ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec);

    std::cout << date_time_buf << std::endl;

    auto end_clock = std::chrono::system_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>
        (end_clock - begin_clock).count() << std::endl;

    getchar();
    return 0;
}