#pragma once
#include <string>
#include <vector>

CString InvokeWsMethod(CString sMethod, CString sJson);
CString InvokeWsMethodOrignal(CString sMethod, CString sJson);

//保存方案界面，JS调客户端中间件，中间件再通过该接口调后台，完成保存方案功能
std::string savePlanByByteXpdl(std::string strJsonInfo, std::vector<byte> vecByte);

void AmiCallWsIceRunRobotFinished(std::string taskId, std::string outputSvnPath, std::string strRunType);
void CallWsIceRunRobotLog(std::vector<std::string> vLogInfo);
void CallWsIceRunRobotRate(std::vector<std::string> vRateInfo);