//按引擎提供的接口描述ice文件定制封装的WS接口基类
#pragma once

#include "peraicebase.h"

#include "WsICEService.h"
using namespace com::pera::base::runtime::remote;

class CPeraIceWsBase :
	public CPeraIceBase
{
public:
	CPeraIceWsBase(void);
	~CPeraIceWsBase(void);

protected:
	virtual BOOL InitProxy();
	virtual void ClearProxy();

	WsICEServicePrx m_pPrx;
	CCriticalSection m_critical_section_Prx;

	virtual BOOL StartThreadCheckIp();
};


