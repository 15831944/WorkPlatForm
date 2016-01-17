#include "stdafx.h"

#include "PeraIceWs.h"

#include "Tools.h"

#ifndef CPPTL_JSON_READER_H_INCLUDED
#include "reader.h"
#endif
#ifndef JSON_WRITER_H_INCLUDED
#include "writer.h"
#endif

static CPeraIceWs s_peraIceWs;

CString InvokeWsMethod(CString sMethod, CString sJson)
{
	//CPeraIceWs peraIceWs;
	std::string strMethod = (LPCTSTR)sMethod;
	std::string strJson = (LPCTSTR)sJson;

	ZTools::WriteZToolsFormatLog("RPC >> InvokeWsMethod");
	ZTools::WriteZToolsLog(strMethod);
	ZTools::WriteZToolsLog(strJson);

	ZTools::MBToUTF8(strMethod);
	ZTools::MBToUTF8(strJson);
	std::string strResult = s_peraIceWs.InvokeWsMethodOrignal(strMethod, strJson);
	ZTools::UTF8ToMB(strResult);

	ZTools::WriteZToolsLog(strResult);
	CString sResult = strResult.c_str();
	return sResult;
}

CString InvokeWsMethodOrignal(CString sMethod, CString sJson)
{
	//CPeraIceWs peraIceWs;
	std::string strMethod = (LPCTSTR)sMethod;
	std::string strJson = (LPCTSTR)sJson;

	ZTools::WriteZToolsFormatLog("RPC >> InvokeWsMethodOrignal");
	ZTools::WriteZToolsLog(strMethod);
	ZTools::WriteZToolsLog(strJson);

	//ZTools::MBToUTF8(strMethod);
	//ZTools::MBToUTF8(strJson);
	std::string strResult = s_peraIceWs.InvokeWsMethodOrignal(strMethod, strJson);
	//ZTools::UTF8ToMB(strResult);

	ZTools::WriteZToolsLog(strResult);
	CString sResult = strResult.c_str();
	return sResult;
}

static std::string Vector2JsonArray( std::vector<std::string> vInfos )
{
	Json::Value jInfos(Json::arrayValue);
	Json::FastWriter jWriter;
	Json::Reader jReader;

	std::string strInfos;
	strInfos = jWriter.write(jInfos);

	for (std::vector<std::string>::iterator it = vInfos.begin(); it != vInfos.end(); ++it)
	{
		Json::Value jInfo;
		if (jReader.parse(*it, jInfo))
		{
			jInfos.append(jInfo);
		}
		else
		{
			ZTools::WriteZToolsFormatLog("解析JSON格式出错\n%s", it->c_str());
		}
	}
	strInfos = jWriter.write(jInfos);

	return strInfos;
}

void AmiCallWsIceRunRobotFinished(std::string taskId, std::string outputSvnPath, std::string strRunType)
{
	ZTools::WriteZToolsFormatLog("调用运行组件完成 -- %s", taskId.c_str());
	ZTools::MBToUTF8(taskId);
	ZTools::MBToUTF8(outputSvnPath);
	s_peraIceWs.amiRunRobotFinished(taskId, outputSvnPath, strRunType);
}

void CallWsIceRunRobotLog(std::vector<std::string> vLogInfo)
{
	//ZTools::WriteZToolsFormatLog("调用运行组件日志");
	std::string logInfo = Vector2JsonArray(vLogInfo);
	ZTools::MBToUTF8(logInfo);
	s_peraIceWs.runRobotLog(logInfo);
}

void CallWsIceRunRobotRate(std::vector<std::string> vRateInfo)
{
	//ZTools::WriteZToolsFormatLog("调用运行组件进度");
	std::string rateInfo = Vector2JsonArray(vRateInfo);
	ZTools::MBToUTF8(rateInfo);
	s_peraIceWs.runRobotRate(rateInfo);
}

std::string savePlanByByteXpdl(std::string strJsonInfo, std::vector<byte> vecByte)
{
	ZTools::WriteZToolsFormatLog("RPC >> savePlanByByteXpdl");
	ZTools::WriteZToolsLog(strJsonInfo);

	ZTools::MBToUTF8(strJsonInfo);
	std::string strResult = s_peraIceWs.savePlanByByteXpdl(strJsonInfo, vecByte);
	ZTools::UTF8ToMB(strResult);

	ZTools::WriteZToolsLog(strResult);
	return strResult;
}
