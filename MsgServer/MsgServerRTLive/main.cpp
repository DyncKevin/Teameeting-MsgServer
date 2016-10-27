//
//  main.cpp
//  MsgServerRTLive
//
//  Created by hp on 1/16/16.
//  Copyright © 2016 DYNC. All rights reserved.
//

#include <iostream>
#include "rtklog.h"
#include "LRTRTLive.h"
#include "RTZKClient.hpp"
#include <google/protobuf/message.h>

#ifndef _TEST_
#define _TEST_ 0
#endif

int main(int argc, const char * argv[]) {
    LI("Hello, RTLive!!!\n");
    LRTRTLive::PrintVersion();

    if (argc <= 2) {
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

#if 0
    L_Init(0, NULL);
#else
    L_Init(0, "./logrtlive.log");
#endif

    MsConfigParser conf;
    conf.LoadFromFile(argv[2]);

    LRTRTLive::Initialize(1024);
    LRTRTLive* pRTLive = LRTRTLive::Inst();
    int res = pRTLive->Start(conf);
    //int test = 0;
    if (res != 0) {
        LI("LRTRTLive start failed and goto exit, res:%d\n", res);
        goto EXIT;
    }
    //while (test++ < 30) {
    while (1) {
        pRTLive->DoTick();
        sleep(1);
        //break;
    }
        sleep(1);
EXIT:
    pRTLive->Stop();
    LRTRTLive::DeInitialize();
    L_Deinit();
    RTZKClient::Instance().Unin();
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
