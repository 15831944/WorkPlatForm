#include "StdAfx.h"
#include "CxNetDiskSettingData.h"

#include "reader.h"
#include "writer.h"

CCxNetDiskSettingData::CCxNetDiskSettingData(void)
{
}


CCxNetDiskSettingData::~CCxNetDiskSettingData(void)
{
	Clear();
}

void CCxNetDiskSettingData::Clear( BOOL bMsg )
{
	m_sCacheDir.Empty();
	m_sUploadMaxTask.Empty();
	m_sDownloadMaxTask.Empty();
	m_sUploadSpeedLimit.Empty();
	m_sDownloadSpeedLimit.Empty();
	m_sDownloadConnectTimeout.Empty();
	if ( bMsg )
	{
		m_sMsg.Empty();
	}
}

BOOL CCxNetDiskSettingData::Parse( LPCTSTR lpszData )
{
	Clear();

	CString strSrc = lpszData == NULL ? "" : lpszData;
	if (strSrc.IsEmpty()) return FALSE;

	Json::Reader reader;
	Json::Value obj;
	CString str;

	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) return FALSE;
		if (!obj.isObject()) return FALSE;

		m_sCacheDir = (obj["CacheDir"].isString()) ? obj["CacheDir"].asCString() : _T("");
		m_sUploadMaxTask = (obj["UploadMaxTask"].isString()) ? obj["UploadMaxTask"].asCString() : _T("");
		m_sDownloadMaxTask = (obj["DownloadMaxTask"].isString()) ? obj["DownloadMaxTask"].asCString() : _T("");
		m_sUploadSpeedLimit = (obj["UploadSpeedLimit"].isString()) ? obj["UploadSpeedLimit"].asCString() : _T("");
		m_sDownloadSpeedLimit = (obj["DownloadSpeedLimit"].isString()) ? obj["DownloadSpeedLimit"].asCString() : _T("");
		m_sDownloadConnectTimeout = (obj["DownloadConnectTimeout"].isString()) ? obj["DownloadConnectTimeout"].asCString() : _T("");
	}
	catch (...)
	{
		m_sMsg = "解析失败，数据异常";
		Clear( FALSE ); return FALSE;
	}

	return TRUE;
}

CString CCxNetDiskSettingData::GetJsonStr_SetSetting(void)
{
	try
	{
		Json::Value objOut;
		objOut["CacheDir"]=  Json::Value( (LPCTSTR)m_sCacheDir );
		/*objOut["UploadMaxTask"]=  Json::Value( (LPCTSTR)m_sUploadMaxTask );
		objOut["DownloadMaxTask"]=  Json::Value( (LPCTSTR)m_sDownloadMaxTask );
		objOut["UploadSpeedLimit"]=  Json::Value( (LPCTSTR)m_sUploadSpeedLimit );
		objOut["DownloadSpeedLimit"]=  Json::Value( (LPCTSTR)m_sDownloadSpeedLimit );
		objOut["DownloadConnectTimeout"] = Json::Value( (LPCTSTR)m_sDownloadConnectTimeout );*/
		return objOut.toStyledString().c_str();
	}
	catch(...)
	{
		return "";
	}
}

BOOL ParseResult(LPCTSTR strRes,CString & strErrMsg)
{
	CString strSrc = strRes == NULL ? "" : strRes;
	if (strSrc.IsEmpty())
	{
		strErrMsg = "解析失败,返回值出现空字符串差异！";
		return FALSE;
	}

	Json::Reader reader;
	Json::Value obj;

	CString strFlag;
	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) return FALSE;
		if (!obj.isObject()) return FALSE;

		strFlag = (obj["flag"].isString()) ? obj["flag"].asCString() : _T("");
		strErrMsg = (obj["msg"].isString()) ? obj["msg"].asCString() : _T("");
	}
	catch (...)
	{
		strErrMsg = "解析失败，返回值数据异常！";
		return FALSE;
	}

	if (strFlag.CompareNoCase("true")==0)
	{
		strErrMsg = "";
		return TRUE;
	}
	else 
		return FALSE;
}