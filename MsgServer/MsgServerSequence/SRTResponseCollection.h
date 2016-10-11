//
//  SRTResponseCollection.h
//  MsgServerSequence
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerSequence__SRTResponseCollection__
#define __MsgServerSequence__SRTResponseCollection__

#include <stdio.h>
#include <iostream>
#include <string>
#include <set>
#include <unordered_map>
#include <utility>
#include "sigslot.h"
#include "rtklog.h"

#define REQUEST_TYPE_READ (1)
#define REQUEST_TYPE_WRITE (2)

#include "ProtoCommon.h"

class SRTRedisManager;

class SRTResponseCollection {
public:
    SRTResponseCollection(SRTRedisManager* manager, int reqType, int clientNum, const pms::StorageMsg& request, int64 seqn);
    ~SRTResponseCollection();

    void AddResponse(const pms::StorageMsg& request, int64 seqn);

    // for all the sequence request
    // read request update maxseqn()
    // write request udpate sequence()
    class MsgSeqn {
        public:
        MsgSeqn(int clientNum, int64 seqn, SRTRedisManager* manager, const pms::StorageMsg& request);
        ~MsgSeqn();
        int                 cnumber;
        int                 counter;
        SRTRedisManager*       rmanager;
        std::set<int64> seqns;
        pms::StorageMsg     storage;
        //pms::SeqnMsg
        sigslot::signal1<const std::string&> WriteResponse;
        sigslot::signal1<const std::string&> ReadResponse;
        bool AddAndCheckWrite(int64 seq)
        {
			LI("MsgSeqn AddAndCheckWrite counter :%d, cnumber:%d\n", counter, cnumber);
            seqns.insert(seq);
            if ((++counter) == cnumber)
            {
				LI("MsgSeqn AddAndCheckWrite counter222 :%d, cnumber:%d\n", counter, cnumber);
                storage.set_sequence(*seqns.rbegin());
                WriteResponse(storage.SerializeAsString());
                return true;
            } else {
                return false;
            }
        }
        bool AddAndCheckRead(int64 seq)
        {
			LI("MsgSeqn AddAndCheckRead counter :%d, cnumber:%d\n", counter, cnumber);
            seqns.insert(seq);
            if ((++counter) == cnumber)
            {
				LI("MsgSeqn AddAndCheckRead counter222 :%d, cnumber:%d\n", counter, cnumber);
                storage.set_maxseqn(*seqns.rbegin());
                ReadResponse(storage.SerializeAsString());
                return true;
            } else {
                return false;
            }
        }
    };
    //<msgid, msgseqn>
    typedef std::unordered_map<std::string, SRTResponseCollection::MsgSeqn*>  SeqnResponseMap;
    typedef SeqnResponseMap::iterator                 SeqnResponseMapIt;
    typedef SeqnResponseMap::const_iterator           SeqnResponseMapCIt;

private:

    int                 m_ClientNum;
    int                 m_ReqType;
    SeqnResponseMap     m_WriteSeqnResponse;
    SeqnResponseMap     m_ReadSeqnResponse;
    SRTRedisManager*    m_pRedisManager;

};

#endif /* defined(__MsgServerSequence__SRTResponseCollection__) */
