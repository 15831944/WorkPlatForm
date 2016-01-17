
// 601DemoServer.h : 601DemoServer Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CMy601DemoServerApp:
// �йش����ʵ�֣������ 601DemoServer.cpp
//

class CMy601DemoServerApp : public CWinAppEx
{
public:
	CMy601DemoServerApp();
public:
	CString m_strServerPort;

// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	CString GetConfigFilePath();
	BOOL GetConfigFromIni();
	void WriteSettingToIni();

// ʵ��
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();
	void TraceRunLog(CString strLogMsg);

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
private:
	CString m_strConfigFile;
};

extern CMy601DemoServerApp theApp;
