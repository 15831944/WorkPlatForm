#pragma once
/************************************************************************/
/*�������ò�����                                                          */
/************************************************************************/
class CPeraNetDiskSetting
{
public:
	CPeraNetDiskSetting();
	~CPeraNetDiskSetting();
	CString GetSettingToJson(); //��ȡJson��ʽ�����ý��
	CString SetSettingFromJson(CString str);//��Json��ʽ�����ý�����Ӧ��д��ini�ļ�,���ؽ��Json
	// ��ȡ�������ò���
	CString GetCacheDir();
	CString GetUploadUrl();
	CString GetDownloadUrl();
	int    GetUploadMaxTask();
	int    GetDownloadMaxTask();
	int    GetUploadSpeedLimit();
	int    GetDownloadSpeedLimit();
	int    GetDownloadConnectTimeout();
	// �������̲�����������ɺ��д��config.ini�ļ���
	void   SetCacheDir(CString);
	void   SetUploadUrl(CString);
	void   SetDownloadUrl(CString);
	void   SetUploadMaxTask(int);
	void   SetDownloadMaxTask(int);
	void   SetUploadSpeedLimit(int);
	void   SetDownloadSpeedLimit(int);
	void   SetDownloadConnectTimeout(int);
public:
	void GetSettingFromConfig(); //������ʱ���config.ini �ļ��ж�ȡ������Ϣ
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