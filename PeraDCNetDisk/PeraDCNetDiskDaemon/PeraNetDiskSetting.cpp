#include "StdAfx.h"
#include "PeraNetDiskIceI.h"
#include <string>
#include "CommonMethod.h"
#include "InterfaceData.h"
#include "UploadManager.h"
#include "CopyManager.h"
#include "PeraNetDiskSetting.h"
#include "ConfigFunc.h"
#include "FileFunc.h"
#include "PeraHttpDownloadMgr.h"
using namespace std;

 CPeraNetDiskSetting theSetting; //全局设置唯一对象

 extern   CPeraHttpDownloadMgr theDownloadMgr; 

void PeraNetDiskIceI::GetCacheDir(const ::std::string& strIn,std::string& strOut)
{
	string strCacheDir = ::GetCacheDir();
	Json::Value objOut;
	objOut["flag"]=  Json::Value(strCacheDir !="" ? "true":"false");
	objOut["msg"] =  Json::Value(LPCTSTR(""));
	objOut["CacheDir"] = Json::Value(strCacheDir.c_str());
	strOut = objOut.toStyledString().c_str();
}
void PeraNetDiskIceI::SetCacheDir(const ::std::string& strIn,std::string& strOut)
{
	CInterfaceData_SetCacheInputData IData;
	if (!IData.Parse(strIn.c_str()))
	{
		strOut = GetDefaultErrMsgJson("解析Json参数出错!");
		return;
	}
	
	BOOL bOk = ::SetCacheDir((LPCTSTR)IData.m_sCacheDir);
	Json::Value objOut;
	objOut["flag"]=  Json::Value(bOk ? "true":"false");
	objOut["msg"] =  Json::Value(LPCTSTR(""));
	strOut = objOut.toStyledString().c_str();
}
CPeraNetDiskSetting::CPeraNetDiskSetting()
{
	//GetSettingFromConfig(); //系统初始化的时候从config.ini文件中读取设置
}
CPeraNetDiskSetting::~CPeraNetDiskSetting()
{

}
CString CPeraNetDiskSetting::GetSettingToJson()
{
	try
	{
		CString strResult,strTmp;
		Json::Value obj(Json::objectValue);
		obj["CacheDir"] = Json::Value( (LPCTSTR)m_sCacheDir );
		obj["UploadUrl"] = Json::Value( (LPCTSTR)m_sUploadUrl);
		obj["DownloadUrl"] = Json::Value( (LPCTSTR)m_sDownloadUrl);

		strTmp.Format("%d",m_iUploadMaxTask);
		obj["UploadMaxTask"] = Json::Value( (LPCTSTR)strTmp );

		strTmp.Format("%d",m_iDownloadMaxTask);
		obj["DownloadMaxTask"] = Json::Value( (LPCTSTR)strTmp );

		strTmp.Format("%d",m_iUploadSpeedLimit);
		obj["UploadSpeedLimit"] = Json::Value( (LPCTSTR)strTmp );

		strTmp.Format("%d",m_iDownloadSpeedLimit);
		obj["DownloadSpeedLimit"] = Json::Value( (LPCTSTR)strTmp );

		strTmp.Format("%d",m_iDownloadConnectTimeout);
		obj["DownloadConnectTimeout"] = Json::Value( (LPCTSTR)strTmp );

		strResult = obj.toStyledString().c_str();
		return strResult;
	}
	catch (...)
	{
		return "";
	}
}
CString  CPeraNetDiskSetting:: SetSettingFromJson(CString strSrc)
{
	Json::Reader reader;
	Json::Value obj,objOut;
	CString strTemp;
	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) return FALSE;
		if (!obj.isObject()) return FALSE;

		strTemp = (obj["CacheDir"].isString()) ? obj["CacheDir"].asCString() : _T("");
		if (!strTemp.IsEmpty()) // 缓存
		{
			m_sCacheDir = strTemp;
			SetCacheDir(m_sCacheDir);
		}

		strTemp = (obj["UploadMaxTask"].isString()) ? obj["UploadMaxTask"].asCString() : _T("");
		if (!strTemp.IsEmpty()) //上传任务数限制
		{
			m_iUploadMaxTask = atoi((LPCTSTR)strTemp);
			SetUploadMaxTask(m_iUploadMaxTask);
		}

		strTemp = (obj["DownloadMaxTask"].isString()) ? obj["DownloadMaxTask"].asCString() : _T("");
		if (!strTemp.IsEmpty()) // 下载任务数限制
		{
			m_iDownloadMaxTask = atoi((LPCTSTR)strTemp);
			SetDownloadMaxTask(m_iDownloadMaxTask);
		}

		strTemp = (obj["UploadSpeedLimit"].isString()) ? obj["UploadSpeedLimit"].asCString() : _T("");
		if (!strTemp.IsEmpty()) // 上传速度限制
		{
			m_iUploadSpeedLimit = atoi((LPCTSTR)strTemp);
			SetUploadSpeedLimit(m_iUploadSpeedLimit);
		}

		strTemp = (obj["DownloadSpeedLimit"].isString()) ? obj["DownloadSpeedLimit"].asCString() : _T("");
		if (!strTemp.IsEmpty()) // 下载速度限制
		{
			m_iDownloadSpeedLimit = atoi((LPCTSTR)strTemp);
			SetDownloadSpeedLimit(m_iDownloadSpeedLimit);
		}
		
		strTemp = (obj["DownloadConnectTimeout"].isString()) ? obj["DownloadConnectTimeout"].asCString() : _T("");
		if (!strTemp.IsEmpty()) // 下载连接超时时间
		{
			m_iDownloadConnectTimeout = atoi((LPCTSTR)strTemp);
			SetDownloadConnectTimeout(m_iDownloadConnectTimeout);
		}

		strTemp = (obj["UploadUrl"].isString()) ? obj["UploadUrl"].asCString() : _T("");
		if (!strTemp.IsEmpty()) // 上传地址
			SetUploadUrl(strTemp);

		strTemp = (obj["DownloadUrl"].isString()) ? obj["DownloadUrl"].asCString() : _T("");
		if (!strTemp.IsEmpty()) // 下载地址
			SetDownloadUrl(strTemp);
		
	}
	catch (...)
	{
		objOut["flag"] = Json::Value("false");
		objOut["msg"] = Json::Value("设置网盘失败!");
		strTemp = objOut.toStyledString().c_str();
		return strTemp;
	}

	objOut["flag"] = Json::Value("true");
	objOut["msg"] = Json::Value("");

	strTemp = objOut.toStyledString().c_str();
	return strTemp;
}
CString CPeraNetDiskSetting::GetCacheDir()
{
	return m_sCacheDir;
}
CString CPeraNetDiskSetting::GetUploadUrl()
{
	return m_sUploadUrl;
}
CString CPeraNetDiskSetting::GetDownloadUrl()
{
	return m_sDownloadUrl;
}
int CPeraNetDiskSetting:: GetUploadMaxTask()
{
	return m_iUploadMaxTask;
}
int CPeraNetDiskSetting::GetDownloadMaxTask()
{
	return m_iDownloadMaxTask;
}
int CPeraNetDiskSetting::GetUploadSpeedLimit()
{
	return m_iUploadSpeedLimit;
}
int CPeraNetDiskSetting::GetDownloadSpeedLimit()
{
	return m_iDownloadSpeedLimit;
}
int CPeraNetDiskSetting::GetDownloadConnectTimeout()
{
	return m_iDownloadConnectTimeout;
}
void CPeraNetDiskSetting::SetCacheDir(CString strDir)
{
	m_sCacheDir = strDir;
	SetConfig("PeraDCNetDisk","CacheDir",m_sCacheDir);
}
void CPeraNetDiskSetting::SetUploadUrl(CString strUrl)
{
	m_sUploadUrl = strUrl;
	SetConfig("PeraDCNetDisk","UploadUrl",m_sUploadUrl);
}
void CPeraNetDiskSetting::SetDownloadUrl(CString strUrl)
{
	m_sDownloadUrl = strUrl;
	SetConfig("PeraDCNetDisk","DownloadUrl",m_sDownloadUrl);
}
void CPeraNetDiskSetting::SetUploadMaxTask(int iMax)
{
	CUploadManager::Instance()->SetMaxThreadCount(iMax); //上传管理器响应设置
	m_iUploadMaxTask = iMax;

	CString strTmp;
	strTmp.Format("%d",m_iUploadMaxTask);
	SetConfig("PeraDCNetDisk","UploadMaxTask",strTmp);
}
void CPeraNetDiskSetting::SetDownloadMaxTask(int iMax)
{
	m_iDownloadMaxTask = iMax;
	CString strTmp;
	strTmp.Format("%d",m_iDownloadMaxTask);
	SetConfig("PeraDCNetDisk","DownloadMaxTask",strTmp);
	//theDownloadMgr.SetSysConfig( iMax ); // 此处调用会崩溃，因为theDownloadMgr还没有init，导致HttpDownloadMgr.cpp 349行代码崩溃
}
void CPeraNetDiskSetting::SetUploadSpeedLimit(int iLimit)
{
	m_iUploadSpeedLimit = iLimit;
	CString strTmp;
	strTmp.Format("%d",m_iUploadSpeedLimit);
	SetConfig("PeraDCNetDisk","UploadSpeedLimit",strTmp);
}
void CPeraNetDiskSetting::SetDownloadSpeedLimit(int iLimit)
{
	m_iDownloadSpeedLimit = iLimit;
	CString strTmp;
	strTmp.Format("%d",m_iDownloadSpeedLimit);
	SetConfig("PeraDCNetDisk","DownloadSpeedLimit",strTmp);
}
void CPeraNetDiskSetting::SetDownloadConnectTimeout(int iTimeOut)
{
	m_iDownloadConnectTimeout = iTimeOut;
	CString strTmp;
	strTmp.Format("%d",m_iDownloadConnectTimeout);
	SetConfig("PeraDCNetDisk","DownloadConnectTimeout",strTmp);
	theDownloadMgr.SetSysConfig( -1, iTimeOut );
}
void CPeraNetDiskSetting::GetSettingFromConfig()
{
	m_sCacheDir = GetConfig("PeraDCNetDisk","CacheDir");
	if (m_sCacheDir.IsEmpty())
	{
		m_sCacheDir = GetExecDir() + "\\CacheDir";
		m_sCacheDir.Replace("\\\\","\\");
		SetCacheDir(m_sCacheDir);
	}
	
	m_sUploadUrl   = GetConfig("PeraDCNetDisk","UploadUrl");
	m_sDownloadUrl = GetConfig("PeraDCNetDisk","DownloadUrl");

	CString strTemp = GetConfig("PeraDCNetDisk","UploadMaxTask");
	if (strTemp.IsEmpty())
	{
		m_iUploadMaxTask = 10;
		SetUploadMaxTask(m_iUploadMaxTask);
	}
	else
	  m_iUploadMaxTask = atoi((LPCTSTR)strTemp);

	strTemp = GetConfig("PeraDCNetDisk","DownloadMaxTask");
	if (strTemp.IsEmpty())
	{
		m_iDownloadMaxTask = 10;
		SetDownloadMaxTask(m_iDownloadMaxTask);
	}
	else
	  m_iDownloadMaxTask = atoi((LPCTSTR)strTemp);

	strTemp = GetConfig("PeraDCNetDisk","UploadSpeedLimit");
	if (strTemp.IsEmpty())
	{
		m_iUploadSpeedLimit = 0;
		SetUploadSpeedLimit(m_iUploadSpeedLimit);
	}
	else
		m_iUploadSpeedLimit = atoi((LPCTSTR)strTemp);

	strTemp = GetConfig("PeraDCNetDisk","DownloadSpeedLimit");
	if (strTemp.IsEmpty())
	{
		m_iDownloadSpeedLimit = 0;
		SetDownloadSpeedLimit(m_iDownloadSpeedLimit);
	}
	else
		m_iDownloadSpeedLimit = atoi((LPCTSTR)strTemp);

	strTemp = GetConfig("PeraDCNetDisk","DownloadConnectTimeout");
	if (strTemp.IsEmpty())
	{
		m_iDownloadConnectTimeout = 1000;
		SetDownloadConnectTimeout(m_iDownloadConnectTimeout);
	}
	else
		m_iDownloadConnectTimeout = atoi((LPCTSTR)strTemp);

}