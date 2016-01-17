#include "stdafx.h"
#include "JsonData.h"
CDataUploadIn::CDataUploadIn(CString strInWsFile)
{
	m_strWsFile = strInWsFile;
}
CString CDataUploadIn::GetJsonStr()
{
	Json::Value JsonInUp;
	JsonInUp["filePath"] = Json::Value((LPCTSTR)m_strWsFile);
	return JsonInUp.toStyledString().c_str();
}
void CDataUpLoadResult::FromJson(CString strJsonStr)
{
	Json::Reader reader;
	Json::Value root;
	if (reader.parse((LPCTSTR)strJsonStr, root))
	{
		m_strIndex = root["index"].isString() ? root["index"].asCString():"";
	}
	else
		m_strIndex = "";
}
CString CDataUpLoadResult::GetIndex()
{
	return m_strIndex;
}
CDataQueryUpProgressIn::CDataQueryUpProgressIn(CString strIndex)
{
	m_strIndex = strIndex;
}
CString CDataQueryUpProgressIn::GetJsonStr()
{
	Json::Value JsonQueryUp;
	JsonQueryUp["index"] = Json::Value((LPCTSTR)m_strIndex);
	CString strQueryInJson = JsonQueryUp.toStyledString().c_str();
	return strQueryInJson;
}
void CDataQueryUpProgressResult::FromJson(CString strJsonStr)
{
	Json::Reader reader;
	Json::Value root;
	CString strTmp = "0";
	if (reader.parse((LPCTSTR)strJsonStr, root))
	{
		strTmp = root["rate"].isString() ? root["rate"].asCString():"0";
		m_strFlag = root["flag"].isString() ? root["flag"].asCString():"";
		m_strMsg = root["msg"].isString() ? root["msg"].asCString():"";
	}
	else
		strTmp = "0";

	m_iRate = atoi(strTmp);
}
int CDataQueryUpProgressResult::GetUpRate()
{
	return m_iRate;
}
CString CDataQueryUpProgressResult::GetFlag()
{
	return m_strFlag;
}
CString CDataQueryUpProgressResult::GetMsg()
{
	return m_strMsg;
}
CDataCallStartWs::CDataCallStartWs(CString strUpResult,CString strLocalIp,CString strUserId,CString strUsrName,CString strTicket)
{
	m_strJsPart = strUpResult;
	m_strLocalIp = strLocalIp;
	m_strUsrId = strUserId;
	m_strUsrName = strUsrName;
	m_strTicket = strTicket;
}
CString CDataCallStartWs::GetJsonStr()
{
	Json::Reader reader;
	Json::Value PartJs;
	if (!reader.parse((LPCTSTR)m_strJsPart, PartJs))
		return "";

	/*Json::Value JsOut;
	JsOut["json"]    = PartJs;
	JsOut["localIp"] = Json::Value(LPCTSTR(m_strLocalIp));
	JsOut["userId"]  = Json::Value(LPCTSTR(m_strUsrId));
	JsOut["userName"] = Json::Value(LPCTSTR(m_strUsrName));
	JsOut["ticket"]  = Json::Value(LPCTSTR(m_strTicket));*/

	Json::Value jArry(Json::arrayValue);
	jArry.append(Json::Value(LPCTSTR(m_strJsPart)));
	jArry.append(Json::Value(LPCTSTR(m_strLocalIp)));
	jArry.append(Json::Value(LPCTSTR(m_strUsrId)));
	jArry.append(Json::Value(LPCTSTR(m_strUsrName)));
	jArry.append(Json::Value(LPCTSTR(m_strTicket)));

	CString strOut = jArry.toStyledString().c_str();
	return strOut;
}

void CDataCallStartWsResult::FromJson(CString strJsonStr)
{
	Json::Reader reader;
	Json::Value root;
	if (reader.parse((LPCTSTR)strJsonStr, root))
	{
		m_strOptMsg = root["optMsg"].isString() ? root["optMsg"].asCString():"";
		m_strOptUrl = root["optUrl"].isString() ? root["optUrl"].asCString():"";
	}
	else
		m_strOptMsg = "结果Json解析出错";
}
CString CDataCallStartWsResult::GetOptMsg()
{
	return m_strOptMsg;
}
CString CDataCallStartWsResult::GetOptUrl()
{
	return m_strOptUrl;
}