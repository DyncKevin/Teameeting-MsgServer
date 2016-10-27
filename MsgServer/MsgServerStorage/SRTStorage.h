#ifndef __MsgServerStorage_SRTSTORAGE_H__
#define __MsgServerStorage_SRTSTORAGE_H__
#include "TCPListenerSocket.h"
#include "SRTStorageListener.h"
#include "RTConfigParser.h"

#include <iostream>
#include <map>
#include <vector>

class SRTStorage
{
protected:
	SRTStorage(void);
public:

	virtual ~SRTStorage(void);

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
	static SRTStorage* Inst();

public:
	int		Start(const RTConfigParser& conf);
	void	DoTick();
	void	Stop();

private:

    SRTStorageListener    *m_pStorageListener;

};

#endif	// __MsgServerStorage_SRTSTORAGE_H__
