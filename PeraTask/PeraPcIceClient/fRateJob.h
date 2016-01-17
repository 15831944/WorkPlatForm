#pragma once
#include "ftlBase.h"
#include "ftlThreadPool.h"
using namespace FTL;

struct RateJobParam
{
	std::string m_strEndPoints;
	std::string m_strRateInfo;
};


class CfRateJob : public FTL::CFJobBase<void*>
{
	DISABLE_COPY_AND_ASSIGNMENT(CfRateJob);
public:
	CfRateJob(void* pMyJobParam);
	CfRateJob();
	~CfRateJob();
	//��Ҫ���ص�Job����

	virtual BOOL Run();
	virtual VOID OnFinalize(IN BOOL isWaiting);
};
