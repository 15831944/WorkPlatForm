
// WebPage.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "CrashHandler.h"

// CWebPageApp:
// �йش����ʵ�֣������ WebPage.cpp
//

class CWebPageApp : public CWinApp
{
public:
	CWebPageApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
public:
	//BOOL ParseCustomCommandLine( void );
	//CCxWebPageEvent * m_pStartEvent ;//= CCxWebPageEvent::GetNew();

	CCrashHandler ch;
};

extern CWebPageApp theApp;

extern HWND GetMainWnd( void );


