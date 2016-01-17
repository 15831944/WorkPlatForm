//封装环境提供的ICE接口的封装类
#pragma once
#include "peraicebase.h"

#include "RunTimeICEService.h"
using namespace FlexwareRuntimeICEService;

class CPeraIceRunRobot :
	public CPeraIceBase
{
public:
	CPeraIceRunRobot(void);
	~CPeraIceRunRobot(void);

protected:
	virtual BOOL InitProxy();
	virtual void ClearProxy();

	virtual std::string GetIceEndPoints();

	RunTimeICEServicePrx m_pPrx;
	CCriticalSection m_critical_section_Prx;

public:
	BOOL RunRobot(std::string strArgs, int nProcessId);
};
