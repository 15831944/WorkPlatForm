
// PeraDCNetDiskDaemon.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "PeraHttpDownloadMgr.h"
#include "CPeraDataPersist.h"
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")

// CPeraDCNetDiskDaemonApp:
// �йش����ʵ�֣������ PeraDCNetDiskDaemon.cpp
//

class CPeraDCNetDiskDaemonApp : public CWinApp
{
public:
	CPeraDCNetDiskDaemonApp();

// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

private:
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR	 gdiplusToken;

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CPeraDCNetDiskDaemonApp theApp;
