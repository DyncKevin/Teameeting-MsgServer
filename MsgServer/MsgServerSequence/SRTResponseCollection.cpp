//
//  SRTResponseCollection.cpp
//  dyncRTSequence
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "SRTResponseCollection.h"
#include "SRTRedisManager.h"


SRTResponseCollection::SRTResponseCollection(SRTRedisManager* manager, int reqType, int clientNum, const pms::StorageMsg& request, int64 seqn)
: m_pRedisManager(manager)
, m_ClientNum(clientNum)
, m_ReqType(reqType)
{
    AddResponse(request, seqn);
}

SRTResponseCollection::~SRTResponseCollection()
{

}

void SRTResponseCollection::AddResponse(const pms::StorageMsg& request, int64 seqn)
{
	LI("SRTResponseCollection::AddResponse storeid:%s, ruserid:%s, seqn:%lld, msgid:%s, m_ReqType:%d\n"\
			, request.storeid().c_str()\
			, request.ruserid().c_str()\
			, seqn\
			, request.msgid().c_str()\
			, m_ReqType);
    if (m_ReqType==REQUEST_TYPE_READ)
    {
		LI("SRTResponseCollection::AddResponse 1\n");
		SeqnResponseMapCIt cit = m_ReadSeqnResponse.find(request.msgid());
		if (cit != m_ReadSeqnResponse.end())
		{
			LI("SRTResponseCollection::AddResponse 2\n");
			if (cit->second->AddAndCheckRead(seqn))
			{
				LI("SRTResponseCollection::AddResponse 3\n");
				delete cit->second;
				m_ReadSeqnResponse.erase(cit);
			}
		} else {
			LI("SRTResponseCollection::AddResponse 4\n");
			m_ReadSeqnResponse.insert(make_pair(request.msgid(), new SRTResponseCollection::MsgSeqn(m_ClientNum, seqn, m_pRedisManager, request)));
        }
    } else if (m_ReqType==REQUEST_TYPE_WRITE)
	{
		LI("SRTResponseCollection::AddResponse 5\n");
		SeqnResponseMapCIt cit = m_WriteSeqnResponse.find(request.msgid());
		if (cit != m_WriteSeqnResponse.end())
		{
			LI("SRTResponseCollection::AddResponse 6\n");
			if (cit->second->AddAndCheckWrite(seqn))
			{
				LI("SRTResponseCollection::AddResponse 7\n");
				delete cit->second;
				m_WriteSeqnResponse.erase(cit);
			}
		} else {
			LI("SRTResponseCollection::AddResponse 8\n");
			m_WriteSeqnResponse.insert(make_pair(request.msgid(), new SRTResponseCollection::MsgSeqn(m_ClientNum, seqn, m_pRedisManager, request)));
        }
    }
}


///////////////////inner class MsgSeqn//////////////////////////////////
///////////////////move here because//////////////////////////////////
///////////////////connect & disconnect//////////////////////////////////
///////////////////cannot be used in *.h//////////////////////////////////


SRTResponseCollection::MsgSeqn::MsgSeqn(int clientNum, int64 seqn, SRTRedisManager* manager, const pms::StorageMsg& request)
: cnumber(clientNum)
, counter(1)
, rmanager(manager)
, storage(request)
{
    seqns.insert(seqn);
    WriteResponse.connect(rmanager, &SRTRedisManager::OnAddAndCheckWrite);
    ReadResponse.connect(rmanager, &SRTRedisManager::OnAddAndCheckRead);
}
SRTResponseCollection::MsgSeqn::~MsgSeqn()
{
    ReadResponse.disconnect(rmanager);
    WriteResponse.disconnect(rmanager);
    seqns.clear();
}

