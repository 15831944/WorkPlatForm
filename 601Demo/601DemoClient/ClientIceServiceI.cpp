#include "StdAfx.h"
#include "ClientIceServiceI.h"
#include "601DemoClient.h"

CClientIceServiceI::CClientIceServiceI(void)
{
}


CClientIceServiceI::~CClientIceServiceI(void)
{
}
std::string CClientIceServiceI::InvokeMethod(const ::std::string& strMethod, const ::std::string& strJson, const ::Ice::Current&/*= ::Ice::Current()*/)
{
	CString strMsg;
	strMsg.Format("收到服务器端的远程调用,方法名称[%s],调用参数[%s]",strMethod.c_str(),strJson.c_str());
	theApp.TraceRunLog(strMsg);

	theApp.TraceRunLog("服务器远程调用客户端结束...");
	return "Success"; // 在此处理各个客户端的调用
}