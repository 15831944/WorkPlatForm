// DlgResLibImage.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgResLibImage.h"
#include "afxdialogex.h"

#include <ResLibData.h>
#include <ResLibProcessData.h>
#include <ResLibRobotData.h>

#include "ResLibImageSet.h"


// CDlgResLibImage 对话框
BEGIN_MESSAGE_MAP(CDlgResLibImage, CDialogEx)
	ON_WM_SIZE()
	ON_MESSAGE(WM_RESLIB_NOTIFY_SELCHANGED, &CDlgResLibImage::OnNotifySelchanged)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CDlgResLibImage, CDialogEx)

CDlgResLibImage::CDlgResLibImage(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgResLibImage::IDD, pParent)
{

}

CDlgResLibImage::~CDlgResLibImage()
{
}

void CDlgResLibImage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMPONENT_LIST_CTRL, m_wndList);
}


void CDlgResLibImage::UpdateContent( void )
{
	m_wndList.SetRedraw(FALSE);

	m_wndList.Clear();

	CString str;
	for (int i=0; i<(int)m_vImgPath.size(); i++)
	{
		m_wndList.AddImage(m_vImgPath[i], m_vComponentName[i], m_vComponentParam[i]) ;
	}

	//if ( m_wndList.GetItemCount() > 0 )
	//	m_wndList.RedrawItems(0, m_wndList.GetItemCount() -1);

	//m_wndList.Invalidate();

//	m_wndList.ModifyStyle(WS_VISIBLE, 0);
	m_wndList.SetRedraw(TRUE);
//	m_wndList.ModifyStyle(0, WS_VISIBLE);
//	m_wndList.UpdateWindow();

	Invalidate();
}


BOOL CDlgResLibImage::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	m_wndList.SetAutoDelete(TRUE);
	m_wndList.Init ();

	ModifyStyle(DS_3DLOOK, 0);
	ModifyStyleEx(WS_EX_CLIENTEDGE, 0);

	SetBackgroundColor( g_crResLibImageBg );

	return TRUE;
}


void CDlgResLibImage::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	CRect rcClient(0,0,cx,cy);
	if ( IsWindow(m_wndList.GetSafeHwnd()) )
	{
		m_wndList.MoveWindow(&rcClient);
	}
}



afx_msg LRESULT CDlgResLibImage::OnNotifySelchanged(WPARAM wParam, LPARAM lParam)
{
	m_vImgPath.clear();
	m_vComponentName.clear();
	m_vComponentParam.clear();

	CCxStaticRes * pRobotImage = NULL;
	DWORD_PTR * pArray = reinterpret_cast< DWORD_PTR * > (wParam);
	int nCount = (int)(lParam);
	CResLibData * pData = NULL;
	CResLibProcessData * pProcess = NULL;
	CResLibRobotData * pRobot = NULL;
	for (int i=0; i<nCount; i++)
	{
		pData = reinterpret_cast< CResLibData * > ( pArray[i] );

		switch (pData->m_nResType)
		{
		case CResLibData::RT_ROBOT:
			pRobot = (CResLibRobotData *)pData;
			if ( pRobot != NULL )
			{
				pRobotImage = GetRobotImageData( pRobot->m_RobotType.c_str(), 42 );
				if ( pRobotImage != NULL 
					&& IsAccessibleFile( pRobotImage->m_strPath ) )
				{
					CString str = pRobot->m_sName.c_str();
					m_vComponentName.push_back(str);
					m_vImgPath.push_back( pRobotImage->m_strPath );
					m_vComponentParam.push_back( reinterpret_cast<LPARAM> ( pData ) );
				}
			}
			break;
		case CResLibData::RT_PROCESS:
			pProcess = (CResLibProcessData *)pData;
			if ( pProcess != NULL )
			{
				pRobotImage = GetStaticResImageData( "StaticResId_ResLib_Flow_42" );
				if ( pRobotImage != NULL 
					&& IsAccessibleFile( pRobotImage->m_strPath ) )
				{
					CString str = pProcess->m_sName.c_str();
					m_vComponentName.push_back(str);
					m_vImgPath.push_back( pRobotImage->m_strPath );
					m_vComponentParam.push_back( reinterpret_cast<LPARAM> ( pData ) );
				}
			}
			break;
		default:
			break;
		}

	}

	UpdateContent();

	if ( m_wndList.m_bSetCapture )
	{
		m_wndList.SetCapture();
		m_wndList.m_bSetCapture = FALSE;
	}

	Invalidate();

	return 0;

}


BOOL CDlgResLibImage::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
// 	switch (pMsg->message)
// 	{
// 	case WM_LBUTTONDOWN:
// 	case WM_RBUTTONDOWN:
// 	case WM_RBUTTONUP:
// 	case WM_MBUTTONDOWN:
// 	case WM_MBUTTONUP:
// 		CBCGPPopupMenu* pActivePopupMenu = CBCGPPopupMenu::GetSafeActivePopupMenu();
// 		if(pActivePopupMenu)
// 		{
// 			pActivePopupMenu->SendMessage (WM_CLOSE);	
// 			pActivePopupMenu = NULL;
// 		}
// 		break;
// 	}

	ASSERT_VALID(this);
	ASSERT(m_hWnd != NULL);

	CWnd* pOwner = GetOwner();

	// don't translate dialog messages when in Shift+F1 help mode
	CFrameWnd* pFrameWnd = GetTopLevelFrame();
	if (pFrameWnd != NULL && pFrameWnd->m_bHelpMode)
		return FALSE;

	// since 'IsDialogMessage' will eat frame window accelerators,
	//   we call all frame windows' PreTranslateMessage first
	while (pOwner != NULL)
	{
		// allow owner & frames to translate before IsDialogMessage does
		if (pOwner->PreTranslateMessage(pMsg))
			return TRUE;

		// try parent frames until there are no parent frames
		if (IsWindow (pOwner->GetSafeHwnd ()))
		{
			pOwner = pOwner->GetParentFrame();
		}
		else
		{
			break;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
