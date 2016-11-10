//
//  main.cpp
//  MsgServerDaemon
//
//  Created by hp on 1/16/16.
//  Copyright © 2016 DYNC. All rights reserved.
//

#include <unistd.h>
#include <iostream>
#include "rtklog.h"
#include "RTConfigParser.h"
#include "RTZKClient.hpp"
#include <google/protobuf/message.h>

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>

#include "DRTDaemon.h"
#include "DRTServerManager.h"

#ifndef _TEST_
#define _TEST_ 0
#endif

void CreateDaemon(void)
{
    int i;
    int fd0;
    pid_t pid;
    struct sigaction sa;
    umask(0);
    if (pid=fork()<0)
    {

    } else if (pid!=0) {
        exit(0);
    }
    setsid();
    sa.sa_handler=SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags=0;

    if (sigaction(SIGCHLD, &sa, NULL)<0)
    {
         return;
    }
    if (pid=fork()<0)
    {
        return;
    } else if (pid!=0) {
         exit(0);
    }
    if (chdir("/")<0)
    {
        printf("child dir error\n");
        return;
    }
    close(0);
    fd0=open("/dev/null", O_RDWR);
    dup2(fd0, 1);
    dup2(fd0, 2);
}

int main(int argc, const char * argv[]) {
    printf("Hello, MsgServerDaemon!!!\n");

    if (argc <= 2) {
        std::cout << "Error: Please usage:$0 {conf_path} " << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }

#if _TEST_
    if (RTZKClient::Instance().InitOnly(argv[1])!=0) {
#else
    if (RTZKClient::Instance().InitZKClient(argv[1])!=0) {
#endif
        std::cout << "Please check the config file ..." << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }

    RTConfigParser conf;
    conf.LoadFromFile(argv[2]);

    int nLogLevel = conf.GetIntVal("log", "level", 0);
    std::string strLogPath = conf.GetValue("log", "path", "./logdaemon.log");
    if (nLogLevel < 0 || nLogLevel > 5)
    {
        std::cout << "Error: Log level=" << nLogLevel << " extend range(0 - 5)!" << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }
    L_Init(nLogLevel, strLogPath.c_str());

    int count = 0, res = 0;
    DRTDaemon* pDaemon = nullptr;

    LI("DRTDaemon start argv1:%s, argv2:%s\n", argv[1], argv[2]);

    DRTDaemon::Initialize(1024);
    pDaemon = DRTDaemon::Inst();
    res = pDaemon->Start(conf);
    if (res != 0) {
        LI("DRTDaemon start failed and goto exit, res:%d\n", res);
        goto EXIT;
    }

    while (1) {
        sleep(1);
        pDaemon->DoTick();
        //if (count++ >120)
        //    break;
    }
        sleep(1);
EXIT:
    pDaemon->Stop();
    L_Deinit();
    RTZKClient::Instance().Unin();
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
