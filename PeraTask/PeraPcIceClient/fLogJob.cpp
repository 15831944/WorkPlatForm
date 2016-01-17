#include "StdAfx.h"
#include "fLogJob.h"

CfLogJob::CfLogJob(void)
{
	
}

CfLogJob::CfLogJob(void* pMyJobParam)
:CFJobBase<void*>(pMyJobParam)
{
	
}

CfLogJob::~CfLogJob(void)
{
}

BOOL CfLogJob::Run()
{
//	AmiCallWsIceRunRobotLog(((LogJobParam*)m_JobParam)->m_strLogInfo);
	
	return TRUE;
}

VOID CfLogJob::OnFinalize( IN BOOL isWaiting )
{
	SAFE_DELETE(m_JobParam);
	delete this;
}

