
// PeraTaskService.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CPeraTaskServiceApp:
// �йش����ʵ�֣������ PeraTaskService.cpp
//

class CPeraTaskServiceApp : public CWinApp
{
public:
	CPeraTaskServiceApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CPeraTaskServiceApp theApp;