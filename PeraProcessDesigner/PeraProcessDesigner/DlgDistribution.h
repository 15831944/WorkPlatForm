#pragma once
#include "dlgJobMgr.h"
class CDlgDistribution :
	public CDlgJobMgr
{
public:
	CDlgDistribution(void);
	virtual ~CDlgDistribution(void);
	CString LoadUrl(void);
};

