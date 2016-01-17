#include "StdAfx.h"
#include "CxBCGPDialog.h"
#include "resource.h"
//#include <dwmapi.h>

IMPLEMENT_DYNAMIC(CCxBCGPDialog, CBCGPDialog)
	
BEGIN_MESSAGE_MAP(CCxBCGPDialog, CBCGPDialog)
END_MESSAGE_MAP()

CCxBCGPDialog::CCxBCGPDialog() :
	CBCGPDialog()
{
}
//*************************************************************************************
CCxBCGPDialog::CCxBCGPDialog (UINT nIDTemplate, CWnd *pParent/*= NULL*/) : 
	CBCGPDialog (nIDTemplate, pParent)
{
}
//*************************************************************************************
CCxBCGPDialog::CCxBCGPDialog (LPCTSTR lpszTemplateName, CWnd *pParentWnd/*= NULL*/) : 
	CBCGPDialog(lpszTemplateName, pParentWnd)
{
}

BOOL CCxBCGPDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ�����ר�ô����/����û���
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_NCLBUTTONDOWN:
	case WM_NCRBUTTONDOWN:
	case WM_NCMBUTTONDOWN:
	case WM_NCLBUTTONUP:
	case WM_NCRBUTTONUP:
	case WM_NCMBUTTONUP:
	case WM_MOUSEMOVE:
		if (m_pToolTip->GetSafeHwnd () != NULL)
		{
			m_pToolTip->RelayEvent(pMsg);
		}
		break;
	}
	return __super::PreTranslateMessage(pMsg);
}


BOOL CCxBCGPDialog::OnInitDialog()
{
	__super::OnInitDialog();

	// TODO:  �ڴ����Ӷ���ĳ�ʼ��
	EnableVisualManagerStyle (TRUE, TRUE);
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME),TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}