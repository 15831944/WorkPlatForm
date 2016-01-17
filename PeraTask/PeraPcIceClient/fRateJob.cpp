#include "StdAfx.h"
#include "fRateJob.h"

CfRateJob::CfRateJob(void)
{
	
}

CfRateJob::CfRateJob(void* pMyJobParam)
:CFJobBase<void*>(pMyJobParam)
{
	
}

CfRateJob::~CfRateJob(void)
{
}

BOOL CfRateJob::Run()
{
//	AmiCallWsIceRunRobotRate(((RateJobParam*)m_JobParam)->m_strRateInfo);
	
	return TRUE;
}

VOID CfRateJob::OnFinalize( IN BOOL isWaiting )
{
	SAFE_DELETE(m_JobParam);
	delete this;
}

