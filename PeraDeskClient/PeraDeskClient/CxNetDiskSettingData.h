#pragma once
class CCxNetDiskSettingData
{
public:
	CCxNetDiskSettingData(void);
	~CCxNetDiskSettingData(void);
	void Clear( BOOL bMsg = TRUE );
	BOOL Parse( LPCTSTR lpszData );
	CString GetJsonStr_SetSetting(void);
public:
	CString m_sCacheDir;
	CString m_sUploadMaxTask;
	CString m_sDownloadMaxTask;
	CString m_sUploadSpeedLimit;
	CString m_sDownloadSpeedLimit;
	CString m_sDownloadConnectTimeout;
	CString m_sMsg;
};

BOOL ParseResult(LPCTSTR strRes,CString & strErrMsg); 

