#ifndef __MsgServerDispatcher_DRTMEETING_H__
#define __MsgServerDispatcher_DRTMEETING_H__
#include "TCPListenerSocket.h"
#include "DRTModuleListener.h"
#include "DRTTransferSession.h"
#include "RTConfigParser.h"

#include <iostream>

class DRTMsgQueue
{
protected:
	DRTMsgQueue(void);
public:
	virtual ~DRTMsgQueue(void);

	/* ��ӡ����汾��
	 * ����:	��
	 */
	static void PrintVersion();
	/* ϵͳ��ʼ��
	 * ����:	evTbSize - ϵͳ��Hash���С������ϵͳ������������仯
	 */
	static void Initialize(int evTbSize);
	/* ϵͳע��
	 * ����:	��
	 */
	static void DeInitialize();

	/*
	 *
	 */
	static DRTMsgQueue* Inst();


public:
	int		Start(const RTConfigParser& conf);
	void	DoTick();
	void	Stop();

private:
    DRTModuleListener                *m_pModuleListener;
    DRTTransferSession               *m_pTransferSession;

};

#endif	// __MsgServerDispatcher_DRTMEETING_H__
