#include "stdafx.h"
#include "JsonData.h"

CString CDataRpcInvokeServer::GetJsonStr()
{
	Json::Value JsonOut;
	JsonOut["clientIp"] = Json::Value((LPCTSTR)m_strIp);
	JsonOut["param"]    = Json::Value((LPCTSTR)m_strParam);
	JsonOut["clientPort"]    = Json::Value((LPCTSTR)m_strPort);
	return JsonOut.toStyledString().c_str();
}
void CDataRpcInvokeServer::Clear()
{
	m_strIp.Empty();
	m_strParam.Empty();
	m_strPort.Empty();
}
BOOL CDataRpcInvokeServer::Parse(LPCTSTR strIn)
{
	Clear();
	if (strIn ==NULL)
		return FALSE;

	if(strcmp(strIn,"")==0)
		return FALSE;

	Json::Reader reader;
	Json::Value root;
	try
	{
		if (reader.parse((LPCTSTR)strIn, root))
		{
			m_strIp    = root["clientIp"].isString() ? root["clientIp"].asCString():"";
			m_strParam = root["param"].isString() ? root["param"].asCString():"";
			m_strPort = root["clientPort"].isString() ? root["clientPort"].asCString():"";
		}
		else
			return  FALSE;
	}
	catch (...)
	{
		Clear();
		return FALSE;
	}
	
	return TRUE;
}

CString CDataAddFunParam::GetJsonStr()
{
	Json::Value JsonOut;
	JsonOut["a"] = Json::Value(m_da);
	JsonOut["b"]    = Json::Value(m_db);
	return JsonOut.toStyledString().c_str();
}
BOOL CDataAddFunParam::Parse(LPCTSTR strIn)
{
	Clear();
	if (strIn ==NULL)
		return FALSE;

	if(strcmp(strIn,"")==0)
		return FALSE;

	Json::Reader reader;
	Json::Value root;
	try
	{
		if (reader.parse((LPCTSTR)strIn, root))
		{
			m_da = root["a"].isNumeric() ? root["a"].asDouble():0;
			m_db = root["b"].isNumeric() ? root["b"].asDouble():0;
		}
		else
			return  FALSE;
	}
	catch (...)
	{
		Clear();
		return FALSE;
	}

	return TRUE;
}
void CDataAddFunParam::Clear()
{
	m_da = 0;
	m_db = 0;
}