
// PeraNetDiskIceTest.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CPeraNetDiskIceTestApp:
// �йش����ʵ�֣������ PeraNetDiskIceTest.cpp
//

class CPeraNetDiskIceTestApp : public CWinApp
{
public:
	CPeraNetDiskIceTestApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CPeraNetDiskIceTestApp theApp;