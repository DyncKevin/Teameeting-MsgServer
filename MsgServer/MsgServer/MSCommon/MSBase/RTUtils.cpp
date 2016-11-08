//
//  RTUtils.cpp
//  MsgServer
//
//  Created by hp on 1/29/16.
//  Copyright © 2016 DYNC. All rights reserved.
//

#include "RTUtils.hpp"
#include "StrPtrLen.h"
#include "md5.h"
#include "md5digest.h"
#include "OS.h"
#include "atomic.h"

static unsigned int g_trans_id = 0;

void GenericSessionId(std::string& strId)
{

    SInt64 curTime = 0;
    MD5_CTX context;
    StrPtrLen hashStr;
    char          curMicroSecStr[33] = {0};
    unsigned char digest[16] = {0};
    {
        curTime = OS::Milliseconds();
        qtss_sprintf(curMicroSecStr, "%lld", curTime);
        MD5_Init(&context);
        MD5_Update(&context, (unsigned char*)curMicroSecStr, (unsigned int)strlen((const char*)curMicroSecStr));
        MD5_Final(digest, &context);
        HashToString(digest, &hashStr);
        strId = hashStr.Ptr;
        hashStr.Delete();
    }
}

unsigned int GenericTransSeq()
{
    return atomic_add(&g_trans_id, 1);
}

int Split(const std::string& delim, const std::string& srcStr, std::vector<std::string>& dstVec)
{
    if (delim.c_str()==nullptr || delim.length()==0 || srcStr.c_str()==nullptr || srcStr.length()==0) return -1;
    size_t last = 0;
    size_t index = srcStr.find_first_of(delim, last);
    while(index!=std::string::npos)
    {
        dstVec.push_back(srcStr.substr(last, index-last));
        last = index + 1;
        index = srcStr.find_first_of(delim, last);
    }
    if (index-last>0)
    {
        dstVec.push_back(srcStr.substr(last, srcStr.length()));
    }
    return 0;
}

