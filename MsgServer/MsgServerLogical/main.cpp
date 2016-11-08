//
//  main.cpp
//  MsgServerLogical
//
//  Created by hp on 1/16/16.
//  Copyright © 2016 DYNC. All rights reserved.
//

#include <iostream>
#include "rtklog.h"
#include "LRTLogical.h"
#include "RTZKClient.hpp"
#include <google/protobuf/message.h>

#ifndef _TEST_
#define _TEST_ 0
#endif

int main(int argc, const char * argv[]) {
    printf("Hello, Logical!!!\n");
    LRTLogical::PrintVersion();

    if (argc <= 1) {
        std::cout << "Error: Please usage:$0 {conf_path} " << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }

#if 1
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
#endif

    RTConfigParser conf;
    conf.LoadFromFile(argv[2]);

    int nLogLevel = conf.GetIntVal("log", "level", 0);
    std::string strLogPath = conf.GetValue("log", "path", "./loglogical.log");
    if (nLogLevel < 0 || nLogLevel > 5)
    {
        std::cout << "Error: Log level=" << nLogLevel << " extend range(0 - 5)!" << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }
    L_Init(nLogLevel, strLogPath.c_str());

    LRTLogical::Initialize(1024);
    LRTLogical* pLogical = LRTLogical::Inst();
    //int test = 0;
    int res = pLogical->Start(conf);
    if (res != 0) {
        LI("LRTLogical start failed and goto exit, res:%d\n", res);
        goto EXIT;
    }
    //while (test++ < 50) {
    while (1) {
        pLogical->DoTick();
        sleep(1);
        //break;
    }
        sleep(1);
EXIT:
    pLogical->Stop();
    LRTLogical::DeInitialize();
    L_Deinit();
    RTZKClient::Instance().Unin();
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
