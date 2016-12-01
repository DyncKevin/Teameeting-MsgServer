//
//  RTUtils.cpp
//  MsgServer
//
//  Created by hp on 1/29/16.
//  Copyright © 2016 DYNC. All rights reserved.
//

#include "RTUtils.hpp"

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

