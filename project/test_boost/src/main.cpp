#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <map>
#include <list>
#include "test_strand.h"
#include "test_deadline_timer.h"
#include "test_log.h"
#include "test_base.h"
#include "alarm.h"
#include "timer_manager.h"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp> 
#include "test_eco.h"
void test_strand1(int argc, char* argv[])
{
    test_strand tt;

    //tt.test_strands();
    tt.test_service();
    getchar(); //换行符
    getchar(); 
}

void test_deadline_timer1(int argc, char* argv[])
{
    test_deadline_timer tt;

    //tt.test_timer_syn();
    tt.test_timer_asyn();
    //tt.test_timer_asyn_loop();
    getchar(); //换行符
    getchar();
}

void test_log1(int argc, char* argv[])
{
    test_log tt;
    tt.test();
    getchar(); //换行符
    getchar();
}

void TestBase1(int argc, char* argv[])
{
    TestBase tt;
    tt.Test();
    getchar(); //换行符
    getchar();
}

void test()
{
    cout << "hello world" << endl;
}

void test_eco(int argc, char* argv[])
{
    TestEco tt;
    tt.TestFileSystem();
    getchar(); //换行符
    getchar();
}


void TestTimer(int argc, char* argv[])
{
    alarm inst(9, 14, 0);
    TimerManager::get_timer_manager().Register(5000, test);
    TimerManager::get_timer_manager().Register("11:21:00", test);
    TimerManager::get_timer_manager().start();
    getchar();
    getchar();
}

int main(int argc, char* argv[])
{
    boost::gregorian::date earlymonth = boost::gregorian::from_string("2019-02-01");
    std::string str = boost::gregorian::to_iso_extended_string(earlymonth);
    boost::posix_time::ptime time(earlymonth);
    earlymonth -= boost::gregorian::months(1);
    str = boost::gregorian::to_iso_extended_string(earlymonth);
    earlymonth += boost::gregorian::days(30);
    str = boost::gregorian::to_iso_extended_string(earlymonth);
    boost::gregorian::date month_tmp = earlymonth;
    month_tmp += boost::gregorian::days(3);
    if (month_tmp.month() == earlymonth.month()) {
        earlymonth = month_tmp;
    }
    else {
        earlymonth.end_of_month();
    }
    str = boost::gregorian::to_iso_extended_string(earlymonth);

    //simple test
    cout << "现在时间：" 
        << boost::posix_time::to_iso_string(boost::posix_time::microsec_clock::local_time()) << endl;
	std::string time_point_ = "8:30:0";
	boost::gregorian::date d1 = boost::gregorian::day_clock::local_day();
	std::string ptime_str = boost::gregorian::to_iso_extended_string(d1) + ' ' + time_point_;
	boost::posix_time::ptime p1 = boost::posix_time::time_from_string(ptime_str);
	cout << boost::posix_time::to_iso_string(p1) << endl;
    int ch = '1';
    do{
        printf("the list: \n");
        printf("0: exit \n");
        printf("1: test strand \n");
        printf("2: test deadline_timer \n");
        printf("3: test log \n");
        printf("4: test base \n");
        printf("5: test timer \n");
        printf("6: test eco \n");
        printf("please select your decide: ");
        ch = getchar();
        switch (ch) {
        case '0':
            printf("exit OK ~~\n");
            break;
        case '1':
            test_strand1(argc, argv);
            break;
        case '2':
            test_deadline_timer1(argc, argv);
            break;
        case '3':
            test_log1(argc, argv);
            break;
        case '4':
            TestBase1(argc, argv);
            break;
        case '5':
            TestTimer(argc, argv);
            break;
        case '6':
            test_eco(argc, argv);
            break;
        default:
            printf("please input right decide~~\n");
            break;
        }
    } while (ch != '0');

    getchar();
    getchar();
    return 0;
}
