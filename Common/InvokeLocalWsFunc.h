#pragma once
#include "PeraIceLocalWs.h"

CString InvokeLocalWsMethod(CString sMethod, CString sJson);
CString InvokeLocalWsMethodOrignal(CString sMethod, CString sJson);

/*
//回调函数示例
int CALLBACK cbFunc(CString sResult)
{
	AfxMessageBox(sResult);
	return 0;
}
*/

BOOL InvokeLocalWsMethod_Async(CString sMethod, CString sJson, cbInvokeLocalWsMethod cbFunc);
BOOL InvokeLocalWsMethodOrignal_Async(CString sMethod, CString sJson, cbInvokeLocalWsMethodOrignal cbFunc);

void AmiCallWsIceRunLocalRobotFinished(std::string taskId, std::string outputSvnPath, std::string strRunType);
void CallWsIceRunLocalRobotLog(std::vector<std::string> vLogInfo);
void CallWsIceRunLocalRobotRate(std::vector<std::string> vRateInfo);