#pragma once
#include "AttachWindow.h"
#include <afxwin.h>
#include <atlstr.h>

/*
重新定义进程类别标识，如：F_WEB_SVG

根据需要在UserDifine.h中，重新指定下列消息值：
CPMSG_ATTACH_PROCESS_WND_RESPONSE
WM_ATTACH_PROCESS_WND

主窗口的copydata响应函数中，添加 ClientProcessCreatedByCopydata

主窗口添加 WM_ATTACH_PROCESS_WND的响应函数，并在函数中调用 OnAttachProcessWnd。

创建进程处，调用 CreateProcessForAttach
*/

enum{
	//被吸附的进程标志，一般在工程UserDefine.h中统一定义
	F_EXE_WEB_CONTAINER								= 1
};

enum{
	//CMainFrame::OnCopyData中，被吸附的进程创建成功后，会发送copydata通知CMainFrame。
	CPMSG_ATTACH_PROCESS_WND_RESPONSE		= 1
};

enum{
	//CMainFrame::OnCopyData中，收到 CPMSG_ATTACH_PROCESS_WND_RESPONSE 后，
	//解析被吸附进程Id和窗口，然后给自己提交本消息，异步吸附。
	WM_ATTACH_PROCESS_WND = (WM_USER + 1)
};

//与 CAttachWindow::AttachClient 的参数 nProcessWndAttachedMsgCode 对应，
//通知被吸附进程，CMainFrame已完成吸附，可以进行后续处理。
extern UINT WM_PROCESS_WND_ATTACHED;

/*
创建准备吸附的进程
参数：
nExeFlag：				进程类别
hHostWnd：				主窗口句柄，用于接收copydata消息。
lpszClientExecPath：	被吸附进程的全路径。
返回值：
	0：失败，否则返回 CAttachWindow对象的 m_nId.
*/
CString CreateProcessForAttach( int nExeFlag, HWND hHostWnd, LPCTSTR lpszClientExecPath );

//
BOOL ClientProcessCreatedByCopydata( CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct, HWND hHostWnd );

extern BOOL OnAttachProcessWnd( CAttachWindow * pAttachWindow, HWND hMainframeWnd, BOOL bTheFilled = TRUE );

extern vector< CAttachWindow* > theAttachWnds;