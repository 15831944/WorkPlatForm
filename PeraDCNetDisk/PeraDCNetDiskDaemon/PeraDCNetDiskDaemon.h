
// PeraDCNetDiskDaemon.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include "PeraHttpDownloadMgr.h"
#include "CPeraDataPersist.h"
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")

// CPeraDCNetDiskDaemonApp:
// 有关此类的实现，请参阅 PeraDCNetDiskDaemon.cpp
//

class CPeraDCNetDiskDaemonApp : public CWinApp
{
public:
	CPeraDCNetDiskDaemonApp();

// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

private:
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR	 gdiplusToken;

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CPeraDCNetDiskDaemonApp theApp;
