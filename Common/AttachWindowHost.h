#pragma once
#include "AttachWindow.h"
#include <afxwin.h>
#include <atlstr.h>

/*
���¶����������ʶ���磺F_WEB_SVG

������Ҫ��UserDifine.h�У�����ָ��������Ϣֵ��
CPMSG_ATTACH_PROCESS_WND_RESPONSE
WM_ATTACH_PROCESS_WND

�����ڵ�copydata��Ӧ�����У���� ClientProcessCreatedByCopydata

��������� WM_ATTACH_PROCESS_WND����Ӧ���������ں����е��� OnAttachProcessWnd��

�������̴������� CreateProcessForAttach
*/

enum{
	//�������Ľ��̱�־��һ���ڹ���UserDefine.h��ͳһ����
	F_EXE_WEB_CONTAINER								= 1
};

enum{
	//CMainFrame::OnCopyData�У��������Ľ��̴����ɹ��󣬻ᷢ��copydata֪ͨCMainFrame��
	CPMSG_ATTACH_PROCESS_WND_RESPONSE		= 1
};

enum{
	//CMainFrame::OnCopyData�У��յ� CPMSG_ATTACH_PROCESS_WND_RESPONSE ��
	//��������������Id�ʹ��ڣ�Ȼ����Լ��ύ����Ϣ���첽������
	WM_ATTACH_PROCESS_WND = (WM_USER + 1)
};

//�� CAttachWindow::AttachClient �Ĳ��� nProcessWndAttachedMsgCode ��Ӧ��
//֪ͨ���������̣�CMainFrame��������������Խ��к�������
extern UINT WM_PROCESS_WND_ATTACHED;

/*
����׼�������Ľ���
������
nExeFlag��				�������
hHostWnd��				�����ھ�������ڽ���copydata��Ϣ��
lpszClientExecPath��	���������̵�ȫ·����
����ֵ��
	0��ʧ�ܣ����򷵻� CAttachWindow����� m_nId.
*/
CString CreateProcessForAttach( int nExeFlag, HWND hHostWnd, LPCTSTR lpszClientExecPath );

//
BOOL ClientProcessCreatedByCopydata( CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct, HWND hHostWnd );

extern BOOL OnAttachProcessWnd( CAttachWindow * pAttachWindow, HWND hMainframeWnd, BOOL bTheFilled = TRUE );

extern vector< CAttachWindow* > theAttachWnds;