
// 601DemoClient.h : 601DemoClient Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CMy601DemoClientApp:
// �йش����ʵ�֣������ 601DemoClient.cpp
//

class CMy601DemoClientApp : public CWinAppEx
{
public:
	CMy601DemoClientApp();


// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	CString GetConfigFilePath();
	BOOL GetConfigFromIni();
	void WriteSettingToIni();
// ʵ��
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;
	BOOL  m_bUseIp;
	CString m_strServerPort;
	CString m_strServerIp;
	CString m_strServerDomain;
	CString m_strLocalServicePort;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();
	void TraceRunLog(CString strLogMsg);
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
private:
	CString m_strConfigFile;
};

extern CMy601DemoClientApp theApp;
