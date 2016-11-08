//
//  main.cpp
//  MsgServerConnector
//
//  Created by hp on 1/16/16.
//  Copyright © 2016 DYNC. All rights reserved.
//

#include <iostream>
#include "rtklog.h"
#include "CRTConnector.h"
#include "RTZKClient.hpp"
#include <google/protobuf/message.h>

#ifndef _TEST_
#define _TEST_ 1
#endif

int main(int argc, const char * argv[]) {
    printf("Hello, Connector!!!");
    CRTConnector::PrintVersion();

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
    std::string strLogPath = conf.GetValue("log", "path", "./logconnector.log");
    if (nLogLevel < 0 || nLogLevel > 5)
    {
        std::cout << "Error: Log level=" << nLogLevel << " extend range(0 - 5)!" << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }
    L_Init(nLogLevel, strLogPath.c_str());
    printf("connector log path:%s\n", strLogPath.c_str());

    CRTConnector::Initialize(1024);
    CRTConnector* pConnector = CRTConnector::Inst();
    //int test = 0;
    int res = pConnector->Start(conf);
    if (res != 0) {
        LI("CRTConnector start failed and goto exit, res:%d\n", res);
        goto EXIT;
    }
    //while (test++ < 30) {
    while (1) {
        pConnector->DoTick();
        sleep(1);
        //break;
    }
        sleep(1);
EXIT:
    pConnector->Stop();
    CRTConnector::DeInitialize();
    L_Deinit();
    RTZKClient::Instance().Unin();
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
