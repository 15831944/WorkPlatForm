
// PeraPRCacheService.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CPeraPRCacheServiceApp:
// �йش����ʵ�֣������ PeraPRCacheService.cpp
//

class CPeraPRCacheServiceApp : public CWinApp
{
public:
	CPeraPRCacheServiceApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CPeraPRCacheServiceApp theApp;