//定制封装的ICE基类
#pragma once

#include "IceHeader.h"

#include <Ice/Ice.h>

Ice::CommunicatorPtr InitPeraIce();
//退出程序之前调用一次，防止内存泄露
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

