//netdisk�ṩ��ICE�ӿڵķ�װ��
#pragma once
#include "peraicebase.h"

#include "PeraNetDiskIce.h"
using namespace Pera::NetDisk::Daemon;

class CPeraIceNetDisk :
	public CPeraIceBase
{
public:
	CPeraIceNetDisk(void);
	~CPeraIceNetDisk(void);

protected:
	virtual BOOL InitProxy();
	virtual void ClearProxy();

	virtual std::string GetIceEndPoints();

	PeraNetDiskDaemonServicePrx m_pPrx;
	CCriticalSection m_critical_section_Prx;

public:
	std::string InvokeMethod(std::string strMethod, std::string strJson);
};