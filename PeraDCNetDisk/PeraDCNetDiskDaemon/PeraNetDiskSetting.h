#pragma once
/************************************************************************/
/*网盘设置操作类                                                          */
/************************************************************************/
class CPeraNetDiskSetting
{
public:
	CPeraNetDiskSetting();
	~CPeraNetDiskSetting();
	CString GetSettingToJson(); //获取Json格式的设置结果
	CString SetSettingFromJson(CString str);//将Json格式的设置进行响应并写入ini文件,返回结果Json
	// 获取网盘设置参数
	CString GetCacheDir();
	CString GetUploadUrl();
	CString GetDownloadUrl();
	int    GetUploadMaxTask();
	int    GetDownloadMaxTask();
	int    GetUploadSpeedLimit();
	int    GetDownloadSpeedLimit();
	int    GetDownloadConnectTimeout();
	// 设置网盘参数（设置完成后会写入config.ini文件）
	void   SetCacheDir(CString);
	void   SetUploadUrl(CString);
	void   SetDownloadUrl(CString);
	void   SetUploadMaxTask(int);
	void   SetDownloadMaxTask(int);
	void   SetUploadSpeedLimit(int);
	void   SetDownloadSpeedLimit(int);
	void   SetDownloadConnectTimeout(int);
public:
	void GetSettingFromConfig(); //启动的时候从config.ini 文件中读取配置信息
private:
	CString m_sCacheDir;
	CString m_sUploadUrl;
	CString m_sDownloadUrl;
	int m_iUploadMaxTask;
	int m_iDownloadMaxTask;
	int m_iUploadSpeedLimit;
	int m_iDownloadSpeedLimit;
	int m_iDownloadConnectTimeout;
};
extern CPeraNetDiskSetting theSetting;