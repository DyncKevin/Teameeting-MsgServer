#ifndef __MsgServerModule_MRTLOGICAL_H__
#define __MsgServerModule_MRTLOGICAL_H__
#include "TCPListenerSocket.h"
#include "MRTModuleListener.h"
#include "RTConfigParser.h"

#include <iostream>
#include <map>
#include <vector>

class MRTModule
{
protected:
	MRTModule(void);
public:

	virtual ~MRTModule(void);

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
	static MRTModule* Inst();

public:
	int		Start(const RTConfigParser& conf);
	void	DoTick();
	void	Stop();

private:

    MRTModuleListener    *m_pModuleListener;

};

#endif	// __MsgServerModule_MRTLOGICAL_H__
