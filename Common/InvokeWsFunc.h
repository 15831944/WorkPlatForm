#pragma once
#include <string>
#include <vector>

CString InvokeWsMethod(CString sMethod, CString sJson);
CString InvokeWsMethodOrignal(CString sMethod, CString sJson);

//���淽�����棬JS���ͻ����м�����м����ͨ���ýӿڵ���̨����ɱ��淽������
std::string savePlanByByteXpdl(std::string strJsonInfo, std::vector<byte> vecByte);

void AmiCallWsIceRunRobotFinished(std::string taskId, std::string outputSvnPath, std::string strRunType);
void CallWsIceRunRobotLog(std::vector<std::string> vLogInfo);
void CallWsIceRunRobotRate(std::vector<std::string> vRateInfo);