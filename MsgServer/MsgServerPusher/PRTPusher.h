#ifndef __MsgServerPusher_PRTPUSHER_H__
#define __MsgServerPusher_PRTPUSHER_H__
#include "TCPListenerSocket.h"
#include "PRTPusherListener.h"
#include "RTConfigParser.h"
#include "PRTApnsPusher.h"

#include <iostream>
#include <map>
#include <vector>

class PRTPusher
{
protected:
	PRTPusher(void);
public:

	virtual ~PRTPusher(void);

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
	static PRTPusher* Inst();

public:
	int		Start(const RTConfigParser& conf);
	void	DoTick();
	void	Stop();

private:

    PRTPusherListener    *m_pModuleListener;
    PRTApnsPusher        *m_apnsPusher;

};

#endif	// __MsgServerPusher_PRTPUSHER_H__
