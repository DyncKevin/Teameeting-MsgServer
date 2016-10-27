#ifndef __MsgServerGrouper_GRTGROUPER_H__
#define __MsgServerGrouper_GRTGROUPER_H__
#include "TCPListenerSocket.h"
#include "GRTGrouperListener.h"
#include "RTConfigParser.h"

#include <iostream>
#include <map>
#include <vector>

class GRTGrouper
{
protected:
	GRTGrouper(void);
public:

	virtual ~GRTGrouper(void);

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
	static GRTGrouper* Inst();

public:
	int		Start(const RTConfigParser& conf);
	void	DoTick();
	void	Stop();

private:
    GRTGrouperListener    *m_pGrouperListener;
};

#endif	// __MsgServerGrouper_GRTGROUPER_H__
