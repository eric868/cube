/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: main.cpp
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.4.13   1.0     Create
******************************************************************************/
#include <iostream>
#include <sstream>
#include <string>

#ifndef WIN32
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <string.h>
#endif

#include <memory>
#include <thread>
#include "test_curl.h"
using namespace std;

#ifndef WIN32
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

bool check_single_instance()
{
    // 打开或创建一个文件
    std::string file_path = "./pid.lck";
    int fd = open(file_path.c_str(), O_RDWR | O_CREAT, 0666);
    if (fd < 0)  {
        printf("Open file failed, error : %s", strerror(errno));
        exit(1);
    }

    // 将该文件锁定
    // 锁定后的文件将不能够再次锁定
    int ret = lockf(fd, F_TLOCK, 0);
    if (ret < 0) {
        if (errno == EACCES || errno == EAGAIN) {
            printf("%s already locked, error: %s\n", file_path.c_str(), strerror(errno));
            close(fd);
            return false;
        }
    }

    // 锁定文件后，将该进程的pid写入文件
    char buf[16] = {0};
    sprintf(buf, "%d", getpid());
    ftruncate(fd, 0);
    ret = write(fd, buf, strlen(buf));
    if (ret < 0) {
        printf("Write file failed, file: %s, error: %s\n", file_path.c_str(), strerror(errno));
        close(fd);
        exit(1);
    }

    // 函数返回时不需要调用close(fd)
    // 不然文件锁将失效
    // 程序退出后kernel会自动close
    return true;
}

//Bit-mask values for 'flags' argument of become_daemon()
#define BD_NO_CHDIR           01    //Don't chdir("/")
#define BD_NO_CLOSE_FILES     02    //Don't close all open files
#define BD_NO_REOPEN_STD_FDS  04    //Don't reopen stdin, stdout, and
//stderr to /dev/null 
#define BD_NO_UMASK0         010    //Don't do a umask(0)

#define BD_MAX_CLOSE  8192          //Maximum file descriptors to close if
//sysconf(_SC_OPEN_MAX) is indeterminate
//Returns 0 on success, -1 on error
int become_daemon(int flags)
{
    int maxfd, fd;

    switch (fork())                     //Become background process
    {
    case -1: return -1;
    case 0:  break;                     //Child falls through...
    default: _exit(EXIT_SUCCESS);       //while parent terminates
    }

    if (setsid() == -1)                 //Become leader of new session
        return -1;

    switch (fork()) {                   //Ensure we are not session leader
    case -1: return -1;
    case 0:  break;
    default: _exit(EXIT_SUCCESS);
    }

    if (!(flags & BD_NO_UMASK0))
        umask(0);                       //Clear file mode creation mask

    if (!(flags & BD_NO_CHDIR))
        chdir("/");                     //Change to root directory

    if (!(flags & BD_NO_CLOSE_FILES)) { //Close all open files
        maxfd = sysconf(_SC_OPEN_MAX);
        if (maxfd == -1)                //Limit is indeterminate...
            maxfd = BD_MAX_CLOSE;       //so take a guess 

        for (fd = 0; fd < maxfd; fd++)
            close(fd);
    }

    if (!(flags & BD_NO_REOPEN_STD_FDS)) {
        close(STDIN_FILENO);            //Reopen standard fd's to /dev/null

        fd = open("/dev/null", O_RDWR);

        if (fd != STDIN_FILENO)         //'fd' should be 0
            return -1;
        if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO)
            return -1;
        if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO)
            return -1;
    }

    return 0;
}
#endif

void version()
{
    cout << "date:2017-04-12" << endl
        << "ver:1.1.0      " << endl;
}

void usage(const char *bin)
{
    cout << endl
        << "-----------------------------------------------------" << endl
        << "usage: " << bin << "[options]" << endl
        << "   -v           : get version of program" << endl
        << "   -d           : service run as deamon" << endl
        << "   -o           : output log to the console" << endl
        << "   -h           : get help info" << endl
        << "   -l           : service run with local-config-flie" << endl
        << "-----------------------------------------------------" << endl;
}

static int proc_main(std::string& param)
{
#ifndef WIN32
    if (!check_single_instance()) return -1;
#endif
    cout << "process start..." << endl;
    test_curl server;
    server.test();

    while (true) {
        Sleep(1000);
    }
    return 0;
}

int main(int argc, char **argv)
{
    std::string curr_path = argv[0];
    int pos = curr_path.rfind("/");
    curr_path = curr_path.substr(0, pos);

    std::string config_file = curr_path + 
        "/config/manager_server_config.xml";
    std::string log_config_file = curr_path + "/config/log_config.xml";
    const char * binary_name = strrchr(argv[0], '/');

    bool run_as_deamon = false;
    bool output_log_to_console = false;
#ifndef WIN32
    char c;
    while ((c = getopt(argc, argv, "lvdhos")) != -1) {
        switch (c) {
        case 'v':
            version();
            return 0;
        case 'd':
            run_as_deamon = true;
            break;
        case 'o':
            output_log_to_console = true;
            break;
        case 'h':
        case '?':
            usage(binary_name);
            return 0;
        case 'l':
            config_file = curr_path + 
                "/config/manager_server_config.xml";
            break;
        default:
            break;
        }
    }
    if (run_as_deamon) {
        /* run as deamon */
        become_daemon(BD_NO_CHDIR);
    } else {
        /* ignore broken pipe signal */
        signal(SIGPIPE, SIG_IGN);
    }

#endif
    proc_main(config_file);

    return 0;
}
