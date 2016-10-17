//
//  main.cpp
//  MsgServerClient
//
//  Created by hp on 1/16/16.
//  Copyright © 2016 DYNC. All rights reserved.
//

#include <iostream>
#include <unistd.h>
#include "rtklog.h"
#include "MsgServerClient.h"
#include <google/protobuf/message.h>
#include <time.h>
#include "webrtc/base/logging.h"
#include "webrtc/base/logsinks.h"
#include "webrtc/base/stream.h"
#include "webrtc/base/fileutils.h"

#ifndef _TEST_
#define _TEST_ 1
#endif

template <typename Base>
class LogSinkImpl
    : public rtc::LogSink,
      public Base {
 public:
  LogSinkImpl() {}

  template<typename P>
  explicit LogSinkImpl(P* p) : Base(p) {}

 private:
  void OnLogMessage(const std::string& message) override {
    static_cast<Base*>(this)->WriteAll(
        message.data(), message.size(), nullptr, nullptr);
  }
};



int main(int argc, const char * argv[]) {
    LI("Hello, SClient!!!");
    MsgServerClient::PrintVersion();

    if (argc <= 1) {
        std::cout << "Error: Please usage:$0 {userid} " << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }


    std::string logpath("./client_log");
    logpath.append(argv[1]);
    //L_Init(2, logpath.c_str());
    L_Init(2, NULL);

    // write to file
    int log_level = 0;
    std::string log_path("./client_logs/Log_");
    log_path.append(argv[1]);
    LogSinkImpl<rtc::FileStream> logStream;
    if (logStream.Open(log_path, "w+", NULL))
    {
        logStream.DisableBuffering();
        rtc::LogMessage::AddLogToStream(&logStream, (rtc::LoggingSeverity)log_level);
    } else {
        logStream.Close();
        rtc::LogMessage::LogToDebug((rtc::LoggingSeverity)log_level);
    }

    MsgServerClient::Initialize(1024);
    MsgServerClient* pSClient = MsgServerClient::Inst();
    int res = pSClient->Start(argv[1], "192.168.7.207", 6630);
    //int test = 0;
    if (res != 0) {
        LI("MsgServerClient start failed and goto exit, res:%d\n", res);
        goto EXIT;
    }
    //while (test++ < 30*1000) {
    //while (test++ < 3000) {
    //while (test++ < 700) {
    while (1) {
        pSClient->DoTick();
        //usleep(1000);
        sleep(1);
        //break;
    }
        sleep(1);
EXIT:
    pSClient->Stop();
    MsgServerClient::DeInitialize();
    L_Deinit();
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
