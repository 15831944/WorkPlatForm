#include "StdAfx.h"
#include "EditBranch.h"

#ifndef _BCGSUITE_
#define visualManagerMFC	CBCGPVisualManager::GetInstance ()
#else
#define visualManagerMFC	CMFCVisualManager::GetInstance ()
#define CBCGPDropDownList	CMFCDropDownListBox
#endif

CEditBranch::CEditBranch(void)
{
}


CEditBranch::~CEditBranch(void)
{
}
BEGIN_MESSAGE_MAP(CEditBranch, CBCGPEdit)
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_NCPAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


void CEditBranch::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//CenterTextV();
	KillTimer(nIDEvent);
	CBCGPEdit::OnTimer(nIDEvent);
}


BOOL CEditBranch::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO: 在此添加专用代码和/或调用基类
	BOOL bRet = CBCGPEdit::Create(dwStyle, rect, pParentWnd, nID);
	//SetTimer( TIMER_EDITBRANCH_UNKNOW, TIMER_EDITBRANCH_UNKNOW_TIMEOUT, 0);
	return bRet;
}

void CEditBranch::CenterTextV()
{
	//return;
	CRect rc = CRect(0,0,0,0);
	GetClientRect(&rc);

	CDC* pDC = GetDC();
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	int nFontHeight = tm.tmHeight + tm.tmExternalLeading;
	int nMargin = (rc.Height() - nFontHeight) / 2;

	rc.DeflateRect(0,nMargin);
	rc.OffsetRect(3, 0);
	rc.right = rc.right - 2;
	SetRectNP(&rc);
	ReleaseDC(pDC);
}


void CEditBranch::OnSize(UINT nType, int cx, int cy)
{
	CBCGPEdit::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	CenterTextV();
}


BOOL CEditBranch::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CenterTextV();
	return CBCGPEdit::OnEraseBkgnd(pDC);
}
void CEditBranch::OnNcPaint()
{
	const BOOL bHasScrollBars = ((GetStyle () & WS_VSCROLL) == WS_VSCROLL) || ((GetStyle () & WS_HSCROLL) == WS_HSCROLL);
	const BOOL bHasBorder = (GetExStyle () & WS_EX_STATICEDGE) || GetExStyle () & WS_EX_CLIENTEDGE|| (GetStyle () & WS_BORDER);

	if (bHasScrollBars || (!m_bVisualManagerStyle && !m_bOnGlass))
	{
		CEdit::OnNcPaint ();
	}

	if (bHasBorder && (m_bVisualManagerStyle || m_bOnGlass))
	{
		CBCGPDrawOnGlass dog (m_bOnGlass);
		visualManagerMFC->OnDrawControlBorder (this);
	}

	if (m_Mode == BrowseMode_None)
	{
		return;
	}

	CWindowDC dc(this);
	DoNcPaint(&dc, FALSE);

}