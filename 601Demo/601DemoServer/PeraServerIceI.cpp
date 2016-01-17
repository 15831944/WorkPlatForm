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
	strMsg.Format("�յ��ͻ��˵�Զ�̵���,��������[%s],���ò���[%s]",strMethod.c_str(),strJson.c_str());
	theApp.TraceRunLog(strMsg);
	std::string strReturn = "Success!";
	CDataRpcInvokeServer DataInvoke;
	if (!DataInvoke.Parse(strJson.c_str()))
	{
		theApp.TraceRunLog("����jons���ִ���...");
		return "Failed";
	}
	if (strMethod == "add")
	{
		CDataAddFunParam DataAdd;
		if (!DataAdd.Parse(DataInvoke.m_strParam))
		{
			theApp.TraceRunLog("����Add��������jons���ִ���...");
			return "Failed";
		}
		theApp.TraceRunLog("Զ�̵��üӷ�����...");
		double d = DataAdd.m_da + DataAdd.m_db;
		CString str;
		str.Format("�ӷ����㷵�أ�%f",d);
		strReturn = str;
	}
	InvokeClientMethod InvokeObj;
	std::string strErrInvoke;
	std::string strClinetReturn =  InvokeObj.InvokeMethod("TestCallClient","���Բ���1,���Բ���2",(LPCTSTR)DataInvoke.m_strIp,(LPCTSTR)DataInvoke.m_strPort,strErrInvoke);
	if (strClinetReturn.empty())
	{
		std::string strErr = "error,�������ص��ͻ���TestCallClient����ʧ��!";
		theApp.TraceRunLog(strErr.c_str());
		strReturn = strErr + strReturn;
		strErrInvoke = strErr + " " + strErrInvoke;
		return strErrInvoke.c_str();
	}
	std::string strInfo = "�ͻ��˵��÷���������������...";
	strReturn = strInfo + strReturn;
	theApp.TraceRunLog(strInfo.c_str());
	theApp.TraceRunLog("----------------------------------------------------------------------------------------");
	return "Success"; // �ڴ˴�������ͻ��˵ĵ���
}