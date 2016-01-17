// PeraDeskClient.h : main header file for the PeraDeskClient application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "UserDefine.h"
#include "CxViewMgr.h"
//#include "BarStructTree.h"
//#include "BarResLib.h"
//#include "DlgResLibTree.h"
//#include "DlgResLibImage.h"
#include "GDIPlusInit.h"
//#include "DlgDragLayer.h"
//#include "SharedMemoryOnlineEdit.h"
//#include "ProcessMgr.h"
#include "CxLoginData.h"
#include "PeraWebPageMgrHost.h"
#include "LicenseMgr.h"
#include "CommandParser.h"
#include "MsgDlg.h"
//#include "CFlexNetLicMgr.h"
// CPeraDeskClientApp:
// See PeraDeskClient.cpp for the implementation of this class
//
//typedef BOOL (_stdcall* LPFNDLLFUNC)(char*);

class CPeraDeskClientApp : public CWinApp, public CBCGPWorkspace, CGDIPlusInit
{
public:
	CPeraDeskClientApp();

	// Override from CBCGPWorkspace
	virtual void PreLoadState ();

public:
	VIEW_DATA * m_pCurViewData; // 临时指针，不需要外部删除、创建
//	CBarStructTree * m_pBarJianmoTree; //CChildFrame 中创建、删除
//	CBarResLib * m_pBarJianmoComponentLib; //CChildFrame 中创建、删除
//	CDlgResLibTree * m_pDlgResLibTree; //CCxSplitterView 中创建、删除
//	CDlgResLibImage * m_pDlgResLibImage; //CCxSplitterView 中创建、删除
//	CDlgDragLayer * m_pDlgDragLayer; //CMainFrame 中创建、删除

	CString m_strXpdlWorkPath;
	CString m_strXpdlName;
	//int m_nFlagFillBarBackground;

	//当前项目标识
	CString m_strProject; 

	int m_nSpliterViewFlag;

	HANDLE m_hMutex;
	BOOL m_bAppExisted;

	enum {
		SPLVF_COMPONENT_TREE = 1 //组件树
		, SPLVF_COMPONENT_LIST //组件列表
	};

	CFont m_font;

	//FlexNet认证
	//CFlexNetLicMgr m_FlexNetMgr;
	//LPFNDLLFUNC lpFnDLLfunc;
	//HMODULE hModule;
	//////////////////////////////////////////////////////////////////////////
	//在线编辑用到的共享内存
	//SharedMemoryOnlineEdit* m_pSharedMemoryOnlineEdit;
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//当前设计过程编辑信息
//	CProcessMgr m_processMgr;
	//
	CCxLoginData m_LoginData;
	//
	BOOL m_bCloseByLogin; //登陆对话框导致关闭
	//
	CCxPeraDCData m_cxPeraDCData;
	//
	BOOL m_bInitMDI; //第一次加载程序时，隐藏menubar、toolbar等，避免闪烁。
	CRect m_rcLastWindowPlacement;

	//运行子流程的模态窗口句柄
	//当弹出模态窗口时，赋值，当收到copydata消息时，将消息转发至模态窗口中。
	HWND m_hwndRunFlow;
	HWND m_hwndViewRunFlow;
	CMsgDlg *m_pMsgDlg;
//	CFlexNetLicMgr m_FlexNetMgr;


	BOOL m_bCreate; //LoadFrame标志

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	void LoadDefaultFont();
	DECLARE_MESSAGE_MAP()


protected:
	virtual BOOL ReloadWindowPlacement (CFrameWnd* pFrame);
};

extern CPeraDeskClientApp theApp;

extern HWND GetMainWnd( void );