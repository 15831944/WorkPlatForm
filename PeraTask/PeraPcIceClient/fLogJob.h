#pragma once
#include "ftlBase.h"
#include "ftlThreadPool.h"
using namespace FTL;

struct LogJobParam
{
	std::string m_strEndPoints;
	std::string m_strLogInfo;
};


class CfLogJob : public FTL::CFJobBase<void*>
{
	DISABLE_COPY_AND_ASSIGNMENT(CfLogJob);
public:
	CfLogJob(void* pMyJobParam);
	CfLogJob();
	~CfLogJob();
	//��Ҫ���ص�Job����

	virtual BOOL Run();
	virtual VOID OnFinalize(IN BOOL isWaiting);
};
