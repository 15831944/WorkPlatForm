//���Ʒ�װ��ICE����
#pragma once

#include "IceHeader.h"

#include <Ice/Ice.h>

Ice::CommunicatorPtr InitPeraIce();
//�˳�����֮ǰ����һ�Σ���ֹ�ڴ�й¶
void ClearPeraIce();
BOOL SetMessageSizeMax(int nSize = 1024);

class CPeraIceBase
{
public:
	CPeraIceBase(void);
	~CPeraIceBase(void);

protected:
	Ice::CommunicatorPtr m_pCommunicator;
	void InitCommunicator();

	virtual std::string GetIceEndPoints() = 0;
	virtual BOOL InitProxy() = 0;
	virtual void ClearProxy() = 0;

public:
	void SetCommunicator(Ice::CommunicatorPtr pCommunicator);
};

