#include <stdio.h>
#include <iostream>
#include <memory>
#include <evpp/libevent.h>
#include <evpp/event_watcher.h>
#include <evpp/event_loop.h>
#include <evpp/timestamp.h>
#include <evpp/tcp_server.h>

#define H_TEST_ASSERT assert

#ifdef WIN32
struct OnApp {
    OnApp() {
        // Initialize net work.
        WSADATA wsaData;
        // Initialize Winsock 2.2
        int nError = WSAStartup(MAKEWORD(2, 2), &wsaData);

        if (nError) {
            std::cout << "WSAStartup() failed with error: %d" << nError;
        }

    }
    ~OnApp() {
        WSACleanup();
    }
} __s_onexit_pause;
#endif

static std::shared_ptr<evpp::EventLoop> loop;
static evpp::Duration delay(1.0);
static bool event_handler_called = false;

static void MyEventThread() {
    LOG_INFO << "EventLoop is running ...";
    loop = std::shared_ptr<evpp::EventLoop>(new evpp::EventLoop);
    loop->Run();

    // Make sure the loop object is delete in its own thread.
    loop.reset();
}

static int periodic_run_count = 0;
static void PeriodicFunc() {
    periodic_run_count++;
    LOG_INFO << "PeriodicFunc is called , periodic_run_count=" << periodic_run_count;
    LOG_INFO << "thread_id=" << std::this_thread::get_id();
}

static void Handle(evpp::InvokeTimerPtr t) {
    event_handler_called = true;
    t->Cancel();
    loop->Stop();
}

void TestEventLoop(int argc, char* argv[])
{
    std::thread th(MyEventThread);
    usleep(delay.Microseconds());
    evpp::Timestamp start = evpp::Timestamp::Now();
    evpp::InvokeTimerPtr t = loop->RunEvery(evpp::Duration(0.3), &PeriodicFunc);
    loop->RunAfter(delay, std::bind(&Handle, t));
    th.join();
    t.reset();
    evpp::Duration cost = evpp::Timestamp::Now() - start;
    H_TEST_ASSERT(delay <= cost);
    H_TEST_ASSERT(event_handler_called);
    H_TEST_ASSERT(periodic_run_count == 3);
    H_TEST_ASSERT(evpp::GetActiveEventCount() == 0);

    getchar();
}

void test_singleton_func(int argc, char* argv[])
{
    getchar();
}

void test_tcpserver_func(int argc, char* argv[])
{
    std::unique_ptr<evpp::TCPServer> tsrv(
        new evpp::TCPServer(loop.get(), "116.236.247.243:20504", "tcp_server", 2));

    getchar();
}

void test_tcpclient_func(int argc, char* argv[])
{

    getchar();
}

int main(int argc, char* argv[])
{
    int ch = '1';
    do{
        printf("the function list: \n");
        printf("0: exit \n");
        printf("1: TestEventLoop \n");
        printf("2: test singleton \n");
        printf("3: test tcpserver \n");
        printf("4: test tcpclient \n");
        printf("please select your decide: ");
        ch = getchar();
        switch (ch) {
        case '0':
            printf("exit OK ~~\n");
            break;
        case '1':
            TestEventLoop(argc, argv);
            break;
        case '2':
            test_singleton_func(argc, argv);
            break;
        case '3':
            test_tcpserver_func(argc, argv);
            break;
        case '4':
            test_tcpclient_func(argc, argv);
            break;
        default:
            printf("please input right decide~~\n");
            break;
        }
    } while (ch != '0');

    getchar();
    return 0;
}
