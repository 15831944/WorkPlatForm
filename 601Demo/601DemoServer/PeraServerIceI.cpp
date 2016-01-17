#include "StdAfx.h"
#include "601DemoServer.h"
#include "PeraServerIceI.h"
#include "JsonData.h"
#include "InvokeClientMethod.h"
CPeraServerIceI::CPeraServerIceI(void)
{
}


CPeraServerIceI::~CPeraServerIceI(void)
{
}
std::string CPeraServerIceI::InvokeMethod(const ::std::string& strMethod , const ::std::string&strJson ,const ::Ice::Current&)
{
	CString strMsg;
	strMsg.Format("收到客户端的远程调用,方法名称[%s],调用参数[%s]",strMethod.c_str(),strJson.c_str());
	theApp.TraceRunLog(strMsg);
	std::string strReturn = "Success!";
	CDataRpcInvokeServer DataInvoke;
	if (!DataInvoke.Parse(strJson.c_str()))
	{
		theApp.TraceRunLog("解析jons出现错误...");
		return "Failed";
	}
	if (strMethod == "add")
	{
		CDataAddFunParam DataAdd;
		if (!DataAdd.Parse(DataInvoke.m_strParam))
		{
			theApp.TraceRunLog("解析Add方法参数jons出现错误...");
			return "Failed";
		}
		theApp.TraceRunLog("远程调用加法运算...");
		double d = DataAdd.m_da + DataAdd.m_db;
		CString str;
		str.Format("加法运算返回：%f",d);
		strReturn = str;
	}
	InvokeClientMethod InvokeObj;
	std::string strErrInvoke;
	std::string strClinetReturn =  InvokeObj.InvokeMethod("TestCallClient","测试参数1,测试参数2",(LPCTSTR)DataInvoke.m_strIp,(LPCTSTR)DataInvoke.m_strPort,strErrInvoke);
	if (strClinetReturn.empty())
	{
		std::string strErr = "error,服务器回调客户端TestCallClient方法失败!";
		theApp.TraceRunLog(strErr.c_str());
		strReturn = strErr + strReturn;
		strErrInvoke = strErr + " " + strErrInvoke;
		return strErrInvoke.c_str();
	}
	std::string strInfo = "客户端调用服务器端正常结束...";
	strReturn = strInfo + strReturn;
	theApp.TraceRunLog(strInfo.c_str());
	theApp.TraceRunLog("----------------------------------------------------------------------------------------");
	return "Success"; // 在此处理各个客户端的调用
}