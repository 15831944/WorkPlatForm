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
	strMsg.Format("�յ��������˵�Զ�̵���,��������[%s],���ò���[%s]",strMethod.c_str(),strJson.c_str());
	theApp.TraceRunLog(strMsg);

	theApp.TraceRunLog("������Զ�̵��ÿͻ��˽���...");
	return "Success"; // �ڴ˴�������ͻ��˵ĵ���
}