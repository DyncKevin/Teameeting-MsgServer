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

#ifndef _TEST_
#define _TEST_ 0
#endif

int main(int argc, const char * argv[]) {
    printf("Hello, MsgServerDaemon!!!\n");

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
    L_Init(0, "./logdeamon.log");
#endif
    FILE* fp;
    time_t t;

    RTConfigParser conf;
    conf.LoadFromFile(argv[2]);
    int res = daemon(2, 4);
    if (res != 0) {
        LI("DRTDeamon start failed and goto exit, res:%d\n", res);
        goto EXIT;
    }
    while (1) {
        sleep(1);
        if((fp=fopen("print_time", "a"))>=0) {
             t = time(0);
             fprintf(fp, "The time right now is :%s\n", asctime(localtime(&t)));
             fclose(fp);
        }
        //break;
    }
        sleep(1);
EXIT:
    L_Deinit();
    RTZKClient::Instance().Unin();
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
