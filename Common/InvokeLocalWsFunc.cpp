#include "stdafx.h"
#include "InvokeLocalWsFunc.h"

#include "PeraIceLocalWs.h"

#include "Tools.h"

static CPeraIceLocalWs s_peraIceLocalWs;

CString InvokeLocalWsMethod(CString sMethod, CString sJson)
{
	//CPeraIceLocalWs peraIceLocalWs;
	std::string strMethod = (LPCTSTR)sMethod;
	std::string strJson = (LPCTSTR)sJson;

	ZTools::MBToUTF8(strMethod);
	ZTools::MBToUTF8(strJson);
	std::string strResult = s_peraIceLocalWs.InvokeLocalWsMethodOrignal(strMethod, strJson);
	ZTools::UTF8ToMB(strResult);

	CString sResult = strResult.c_str();
	return sResult;
}

CString InvokeLocalWsMethodOrignal(CString sMethod, CString sJson)
{
	//CPeraIceLocalWs peraIceLocalWs;
	std::string strMethod = (LPCTSTR)sMethod;
	std::string strJson = (LPCTSTR)sJson;

	//ZTools::MBToUTF8(strMethod);
	//ZTools::MBToUTF8(strJson);
	std::string strResult = s_peraIceLocalWs.InvokeLocalWsMethodOrignal(strMethod, strJson);
	//ZTools::UTF8ToMB(strResult);

	CString sResult = strResult.c_str();
	return sResult;
}

BOOL InvokeLocalWsMethod_Async( CString sMethod, CString sJson, cbInvokeLocalWsMethod cbFunc )
{
	//CPeraIceLocalWs peraIceLocalWs;
	std::string strMethod = (LPCTSTR)sMethod;
	std::string strJson = (LPCTSTR)sJson;

	BOOL bResult = s_peraIceLocalWs.InvokeLocalWsMethod_Async(strMethod, strJson, new AMI_WsICEService_invokeLocalWsMethod_I(cbFunc));

	return bResult;
}

BOOL InvokeLocalWsMethodOrignal_Async(CString sMethod, CString sJson, cbInvokeLocalWsMethodOrignal cbFunc)
{
	//CPeraIceLocalWs peraIceLocalWs;
	std::string strMethod = (LPCTSTR)sMethod;
	std::string strJson = (LPCTSTR)sJson;

	BOOL bResult = s_peraIceLocalWs.InvokeLocalWsMethodOrignal_Async(strMethod, strJson, new AMI_WsICEService_invokeLocalWsMethodOrignal_I(cbFunc));

	return bResult;
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

void AmiCallWsIceRunLocalRobotFinished(std::string taskId, std::string outputSvnPath, std::string strRunType)
{
	ZTools::WriteZToolsFormatLog("调用运行组件完成 -- %s", taskId.c_str());
	ZTools::MBToUTF8(taskId);
	ZTools::MBToUTF8(outputSvnPath);
	s_peraIceLocalWs.amiRunLocalRobotFinished(taskId, outputSvnPath, strRunType);
}

void CallWsIceRunLocalRobotLog(std::vector<std::string> vLogInfo)
{
	//ZTools::WriteZToolsFormatLog("调用运行组件日志");
	std::string logInfo = Vector2JsonArray(vLogInfo);
	ZTools::MBToUTF8(logInfo);
	s_peraIceLocalWs.runLocalRobotLog(logInfo);
}

void CallWsIceRunLocalRobotRate(std::vector<std::string> vRateInfo)
{
	//ZTools::WriteZToolsFormatLog("调用运行组件进度");
	std::string rateInfo = Vector2JsonArray(vRateInfo);
	ZTools::MBToUTF8(rateInfo);
	s_peraIceLocalWs.runLocalRobotRate(rateInfo);
}
