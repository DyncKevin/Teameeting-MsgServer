#ifndef __MsgServerSequence_SRTSEQUENCE_H__
#define __MsgServerSequence_SRTSEQUENCE_H__
#include "TCPListenerSocket.h"
#include "SRTSequenceListener.h"
#include "RTConfigParser.h"

#include <iostream>
#include <map>
#include <vector>

class SRTSequence
{
protected:
	SRTSequence(void);
public:

	virtual ~SRTSequence(void);

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
	static SRTSequence* Inst();

public:
	int		Start(const RTConfigParser& conf);
	void	DoTick();
	void	Stop();

private:

    SRTSequenceListener    *m_pSequenceListener;

};

#endif	// __MsgServerSequence_SRTSEQUENCE_H__
