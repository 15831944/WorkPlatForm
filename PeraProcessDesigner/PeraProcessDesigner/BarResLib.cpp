// CxFloatDlgBar.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "BarResLib.h"

// CCxFloatDlgBar

BEGIN_MESSAGE_MAP(CBarResLib, CCxFloatBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CBarResLib, CCxFloatBar)

CBarResLib::CBarResLib()
{
	m_pDlgInner = NULL;
}

CBarResLib::~CBarResLib()
{
	if (m_pDlgInner != NULL)
	{
		if ( IsWindow( m_pDlgInner->GetSafeHwnd() ) )
			m_pDlgInner->DestroyWindow();

		delete m_pDlgInner; m_pDlgInner = NULL;
	}
}






int CBarResLib::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CCxFloatBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rcClient;
	GetClientRect (&rcClient);

	m_pDlgInner = new CDlgResLib(this);
	m_pDlgInner->Create(m_pDlgInner->IDD, this);
	m_pDlgInner->ShowWindow(SW_SHOW);
	return 0;
}


void CBarResLib::OnSize(UINT nType, int cx, int cy)
{
	CCxFloatBar::OnSize(nType, cx, cy);
	if ( m_pDlgInner != NULL && IsWindow(m_pDlgInner->GetSafeHwnd()) )
		m_pDlgInner->SetWindowPos (NULL, 1, -1, cx - 2, cy
			, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CBarResLib::UpdateContent()
{

}

