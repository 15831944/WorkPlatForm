// PeraProcessDesigner.h : main header file for the PeraProcessDesigner application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "UserDefine.h"
#include "CxViewMgr.h"
#include "BarStructTree.h"
#include "BarResLib.h"
#include "DlgResLibTree.h"
#include "DlgResLibImage.h"
#include "GDIPlusInit.h"
#include "DlgDragLayer.h"
#include "SharedMemoryOnlineEdit.h"
#include "ProcessMgr.h"
#include "CxLoginData.h"

//#include "CFlexNetLicMgr.h"

#ifdef USE_DLGLOG
#include "DlgLog.h"
#endif // USE_DLGLOG
#include "PeraProcessDesignerCommandline.h"
#include "AttachWindow.h"

//#include "CFlexNetLicMgr.h"
// CPeraProcessDesignerApp:
// See PeraProcessDesigner.cpp for the implementation of this class
//
//typedef BOOL (_stdcall* LPFNDLLFUNC)(char*);

class CPeraProcessDesignerApp : public CWinApp, public CBCGPWorkspace, CGDIPlusInit
{
public:
	CPeraProcessDesignerApp();

	// Override from CBCGPWorkspace
	virtual void PreLoadState ();

public:
	VIEW_DATA * m_pCurViewData; // ��ʱָ�룬����Ҫ�ⲿɾ��������
	CBarStructTree * m_pBarJianmoTree; //CChildFrame �д�����ɾ��
	CBarResLib * m_pBarJianmoComponentLib; //CChildFrame �д�����ɾ��
	CDlgResLibTree * m_pDlgResLibTree; //CCxSplitterView �д�����ɾ��
	CDlgResLibImage * m_pDlgResLibImage; //CCxSplitterView �д�����ɾ��
	CDlgDragLayer * m_pDlgDragLayer; //CMainFrame �д�����ɾ��

#ifdef USE_DLGLOG
	CDlgLog m_dlgLog; //��־����
#endif // USE_DLGLOG

	CString m_strXpdlWorkPath;
	CString m_strXpdlName;
	//int m_nFlagFillBarBackground;

	//��ǰ��Ŀ��ʶ
	CString m_strProject; 

	int m_nSpliterViewFlag;

	BOOL m_bAloneVersionWs;


	enum {
		SPLVF_COMPONENT_TREE = 1 //�����
		, SPLVF_COMPONENT_LIST //����б�
	};

	CFont m_font;

	//FlexNet��֤
	//CFlexNetLicMgr m_FlexNetMgr;
	//LPFNDLLFUNC lpFnDLLfunc;
	//HMODULE hModule;
	//////////////////////////////////////////////////////////////////////////
	//���߱༭�õ��Ĺ����ڴ�
	SharedMemoryOnlineEdit* m_pSharedMemoryOnlineEdit;
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//��ǰ��ƹ��̱༭��Ϣ
	CProcessMgr m_processMgr;
	//
	CCxLoginData m_LoginData;
	//
	BOOL m_bInitMDI; //��һ�μ��س���ʱ������menubar��toolbar�ȣ�������˸��
	CRect m_rcLastWindowPlacement;

	//���������̵�ģ̬���ھ��
	//������ģ̬����ʱ����ֵ�����յ�copydata��Ϣʱ������Ϣת����ģ̬�����С�
	HWND m_hwndRunFlow;
	HWND m_hwndViewRunFlow;
	//CFlexNetLicMgr m_FlexNetMgr;


	BOOL m_bCreate; //LoadFrame��־

	BOOL m_bGetClientSubFlowId; //

	CPeraProcessDesignerCommandline m_CmdLine;

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	void LoadDefaultFont();
	DECLARE_MESSAGE_MAP()

private:
	HANDLE m_hMetux;

protected:
	virtual BOOL ReloadWindowPlacement (CFrameWnd* pFrame);
};

extern CPeraProcessDesignerApp theApp;

extern HWND GetMainWnd( void );

extern CAttachWindow* g_pawRunProcess;