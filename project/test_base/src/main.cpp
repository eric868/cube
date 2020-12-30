/*****************************************************************************
VBase Copyright (c) 2015. All Rights Reserved.

FileName: main.cpp
Version: 1.0
Date: 2016.11.25

History:
ericsheng     2016.11.25   1.0     Create
******************************************************************************/
#include <map>
#include <stdio.h>

#include "base/util.h"
#include "test_sigslot.h"
#include "test_dictionary.h"
#include "test_alarm.h"
#include "test_log_binder.h"
#include "test_zip.h"
#include "test_aes.h"
#include "test_rsa.h"
#include "test_timer.h"
#include "test_dispatch.h"
#include "test_tqueue.h"
#include "base/pugixml.hpp"
#include "test_util.h"
#include "test_log4cplus.h"
#include "test_md5.h"
#include "test_xml.h"
using namespace std;
void test_timer1(int argc, char* argv[])
{
    test_timer tt;
    tt.test_settimer_single();
    //tt.test_settimer_multi();
    getchar(); //换行符
    getchar(); //等待超时
}

void test_dispatch1(int argc, char* argv[])
{
    test_dispatch t;
    t.test_timer();
}


void test_rsa1(int argc, char* argv[])
{
    test_rsa t;
    t.test_single_rsa();
}

void test_aes1(int argc, char* argv[])
{
    test_aes t;
    t.test_single_aes();
}

void test_md51(int argc, char* argv[])
{
    test_md5 t;
    t.test_single_md5();
}

void test_zlib(int argc, char* argv[])
{
    test_zip t;
    t.test_flate(argc, argv);
}

void test_log_binder1(int argc, char* argv[])
{
    test_log_binder t;
    t.test();
}

void test_sigslot(int argc, char* argv[])
{
    alarm_clock alarm_clock;
    student midschool_stu;
    alarm_clock.tick.connect(&midschool_stu, &student::weak_up);
    for (int i = 0; i < 10; i++) {
        alarm_clock.send_msg(i);
        base::util::sleep(1000);
    }
    alarm_clock.tick.disconnect(&midschool_stu);
}

void test_tqueue1(int argc, char* argv[])
{
    test_tqueue t;
    t.mult_thread_test();
}

void test_alarm1(int argc, char* argv[])
{
    test_alarm t;
    t.set_alarm(15, 23, 0);
    getchar(); //换行符
    getchar(); //等待超时
}

void test_dictionarys(int argc, char* argv[])
{
    test_dictionay d;
    d.test();
}

void test_pugixml(int argc, char* argv[])
{
    test_xml t;
    t.test();
}

void test_log4cplus1(int argc, char* argv[])
{
    test_log4cplus tmp;
    tmp.test_file_appender();
}

void test_util1(int argc, char* argv[])
{
    test_util tmp;
    tmp.test();
}

int main(int argc, char* argv[])
{
    int ch = '1';
    do{
        printf("the list: \n");
        printf("0: exit \n");
        printf("1: test base::timer \n");
        printf("2: test base::dispatch \n");
        printf("3: test base::log_binder \n");
        printf("4: test base::event \n");
        printf("5: test base::tqueue \n");
        printf("6: test base::alarm \n");
        printf("7: test base::rsa \n");
        printf("8: test base::aes \n");
        printf("9: test base::trace \n");
        printf("a: test std::map\n");
        printf("b: test base::dictonary\n");
        printf("c: test base::sigslot\n");
        printf("d: test base::zlib\n");
        printf("e: test base::xml\n");
        printf("f: test log4cplus\n");
        printf("g: test base::util\n");
        printf("h: test base::md5\n");
        printf("please select your decide: ");
        ch = getchar();
        switch (ch) {
        case '0':
            printf("exit OK ~~\n");
            break;
        case '1':
            test_timer1(argc, argv);
            break;
        case '2':
            test_dispatch1(argc, argv);
            break;
        case '3':
            test_log_binder1(argc, argv);
            break;
        case '5':
            test_tqueue1(argc, argv);
            break;
        case '6':
            test_alarm1(argc, argv);
            break;
        case '7':
            test_rsa1(argc, argv);
            break;
        case '8':
            test_aes1(argc, argv);
            break;
        case 'b':
            test_dictionarys(argc, argv);
            break;
        case 'c':
            test_sigslot(argc, argv);
            break;
        case 'd':
            test_zlib(argc, argv);
            break;
        case 'e':
            test_pugixml(argc, argv);
            break;
        case 'f':
            test_log4cplus1(argc, argv);
            break;
        case 'g':
            test_util1(argc, argv);
            break;
        case 'h':
            test_md51(argc, argv);
            break;
        default:
            printf("please input right decide~~\n");
            break;
        }
    } while (0);
    //} while (ch != '0');

    getchar();
    getchar();
    return 0;
}
