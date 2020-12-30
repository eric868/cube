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

//ɾ���ַ����пո��Ʊ��tab����Ч�ַ�
std::string Trim(std::string& str)
{
    //str.find_first_not_of(" \t\r\n"),���ַ���str�д�����0��ʼ�������״β�ƥ��"\t\r\n"��λ��
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

    std::ifstream fin("t_user.csv"); //���ļ�������
    std::string line;
    while (getline(fin, line))   //���ж�ȡ�����з���\n�����֣������ļ�β��־eof��ֹ��ȡ
    {
        std::cout << "ԭʼ�ַ�����" << line << std::endl; //�������
        if (line[line.size() - 1] == ',') {
            line.push_back(' ');
        }
        std::istringstream sin(line); //�������ַ���line���뵽�ַ�����istringstream��
        std::vector<std::string> fields; //����һ���ַ�������
        std::string field;
        while (std::getline(sin, field, ',')) //���ַ�����sin�е��ַ����뵽field�ַ����У��Զ���Ϊ�ָ���
        {
            fields.push_back(field); //���ոն�ȡ���ַ�����ӵ�����fields��
        }
        std::string name = Trim(fields[2]); //���������fields�е�һ��Ԫ�ص���Ч�ַ�������ֵ������name
        std::string age = Trim(fields[3]); //���������fields�еڶ���Ԫ�ص���Ч�ַ�������ֵ������age
        std::string birthday = Trim(fields[4]); //���������fields�е�����Ԫ�ص���Ч�ַ�������ֵ������birthday
        std::cout << "����֮����ַ�����" << name << "\t" << age << "\t" << birthday << std::endl;
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