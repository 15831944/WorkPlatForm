
// testiceclient.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CtesticeclientApp:
// �йش����ʵ�֣������ testiceclient.cpp
//

class CtesticeclientApp : public CWinApp
{
public:
	CtesticeclientApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CtesticeclientApp theApp;