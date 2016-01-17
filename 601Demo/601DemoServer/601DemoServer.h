
// 601DemoServer.h : 601DemoServer 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号


// CMy601DemoServerApp:
// 有关此类的实现，请参阅 601DemoServer.cpp
//

class CMy601DemoServerApp : public CWinAppEx
{
public:
	CMy601DemoServerApp();
public:
	CString m_strServerPort;

// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	CString GetConfigFilePath();
	BOOL GetConfigFromIni();
	void WriteSettingToIni();

// 实现
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
