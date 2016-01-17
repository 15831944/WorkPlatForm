// ZEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "ZEdit.h"
#include "WndIf.h"
#include "DlgWndIf.h"
#include "ZButtonDraw.h"
#include "UtilityFunc.h"

#ifndef _BCGSUITE_
#define visualManagerMFC	CBCGPVisualManager::GetInstance ()
#else
#define visualManagerMFC	CMFCVisualManager::GetInstance ()
#define CBCGPDropDownList	CMFCDropDownListBox
#endif

// CZEdit
int CZEdit::m_staCount = 0;

IMPLEMENT_DYNAMIC(CZEdit, CBCGPEdit)

CZEdit::CZEdit()
: m_nPos(0)
, m_strText(_T(""))
{
	m_index = m_staCount ++;

	m_BackColor = RGB(0,0,0);
	m_bMouseTracked = FALSE;
	m_Brush.CreateSolidBrush(m_BackColor);
}

CZEdit::~CZEdit()
{
	m_Brush.DeleteObject();
}


BEGIN_MESSAGE_MAP(CZEdit, CBCGPEdit)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, &CZEdit::OnEnKillfocus)
	ON_CONTROL_REFLECT(EN_SETFOCUS, &CZEdit::OnEnSetfocus)
	//ON_WM_CTLCOLOR_REFLECT()
	//ON_WM_PAINT()
// 	ON_WM_CREATE()
	ON_WM_NCPAINT()
 	ON_WM_SIZE()
	//ON_WM_NCCALCSIZE()
// 	ON_WM_SHOWWINDOW()
// 	ON_WM_MOVE()
// 	ON_WM_UPDATEUISTATE()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeaved)
END_MESSAGE_MAP()



// CZEdit 消息处理程序




void CZEdit::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CBCGPEdit::OnLButtonDown(nFlags, point);
}


void CZEdit::OnEnKillfocus()
{
	// TODO: 在此添加控件通知处理程序代码
	m_nPos = GetSel() & 0xFFFF;
	m_nPosEnd = GetSel() >> 16 & 0xFFFF;
	GetWindowText(m_strText);
	//CWndIf* pWndParent = ((CWndIf*)GetParent());
	//((CWndIf*)GetParent())->m_vBCs[pWndParent->m_nBranchSelect]->m_staArrow.ShowWindow(SW_HIDE);
}


void CZEdit::OnEnSetfocus()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlgWndIf* pWndParent = ((CDlgWndIf*)GetParent());
	CWnd* pDlg = pWndParent->GetParent();
	if(pWndParent->m_nBranchSelect >= 0)
	{
		pWndParent->m_vBCs[pWndParent->m_nBranchSelect]->m_staArrow.ShowWindow(SW_HIDE);
	}
	pWndParent->m_nBranchSelect = m_index;
	pWndParent->m_vBCs[pWndParent->m_nBranchSelect]->m_staArrow.ShowWindow(SW_SHOW);

// 	CWnd* pBtnUp = pDlg->GetDlgItem(IDC_BUTTON_UP);
// 	CWnd* pBtnDown = pDlg->GetDlgItem(IDC_BUTTON_DOWN);

	CZButtonDraw* pBtnUp = (CZButtonDraw*)pDlg->GetDlgItem(IDC_BUTTON_UP);
	CZButtonDraw* pBtnDown = (CZButtonDraw*)pDlg->GetDlgItem(IDC_BUTTON_DOWN);

	int nCurSelect = pWndParent->m_nCurSelect;
	int nMax;

	if(nCurSelect == 0)
	{//if OR else if
		nMax = pWndParent->m_vBCs.size() - 1;
	}
	else
	{//else
		nMax = pWndParent->m_vBCs.size() - 2;
	}

	if(m_index == 0)
	{//第一个
		pBtnUp->EnableWindow(FALSE);
	}
	else
	{
		pBtnUp->EnableWindow(TRUE);
		if(nCurSelect == 1 && m_index == pWndParent->m_vBCs.size() - 1)
		{
			pBtnUp->EnableWindow(FALSE);
		}
	}

	if(m_index >= nMax)
	{//倒数第1个
		pBtnDown->EnableWindow(FALSE);
	}
	else
	{
		pBtnDown->EnableWindow(TRUE);
	}
}


HBRUSH CZEdit::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	// TODO:  在此更改 DC 的任何特性

	// TODO:  如果不应调用父级的处理程序，则返回非 null 画笔
	CBrush      m_brushedit;   
	m_brushedit.CreateSolidBrush      (RGB      (      255,      0,      0      )      );   

	pDC->SetTextColor(RGB(255,0,0));          //文字颜色   
	//pDC->SetBkMode(OPAQUE);
	//pDC->SetBkMode(TRANSPARENT);
	pDC->SetBkColor(RGB(0,255,0));      //文字背景颜色   

	//return NULL;
	return (HBRUSH)m_brushedit.GetSafeHandle()      ;        //edit框的颜色   
}


void CZEdit::SetBkGrndColor()
{
	// 删除旧的刷子 
	m_Brush.DeleteObject();
	//创建一个m_BackColor颜色的刷子 
	m_Brush.CreateSolidBrush(m_BackColor); 
	CDC* pDC = GetDC();
	//设置文字显示模式，OPAQUE为独占模式 
	pDC->SetBkMode(OPAQUE);
	//设置文字的背景颜色 
	pDC->SetBkColor(m_BackColor);
	//选择刷子 
	CBrush* pBrushT = pDC->SelectObject(&m_Brush); 
	CRect rc;
	GetClientRect(&rc);
	//ScreenToClient(&rc);
	//绘制一个矩形 
	pDC->Rectangle(0, 0, rc.Width(), rc.Height());
	//设置文本颜色 
	pDC->SetTextColor(RGB(255, 255, 255));
	//输出文本 
	GetWindowText(m_strText);
	pDC->TextOut(2, 2, m_strText);
	pDC->SelectObject(pBrushT); 
	ReleaseDC(pDC);

}

void CZEdit::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	SetBkGrndColor();

	// 不为绘图消息调用 
	//CEdit::OnPaint();
}


int CZEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CenterTextV();

	if (CBCGPEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	CenterTextV();
	// TODO:  在此添加您专用的创建代码
	CRect rect;
	GetWindowRect(rect);
	OnSize(0, rect.Width(), rect.Height());
	return 0;
}


void CZEdit::OnSize(UINT nType, int cx, int cy)
{
	CBCGPEdit::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	CenterTextV();
}


BOOL CZEdit::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO: 在此添加专用代码和/或调用基类
	BOOL bRet = CBCGPEdit::Create(dwStyle, rect, pParentWnd, nID);
	m_ToolTip.Create (this);
//	SetTimer( TIMER_ZEDIT_UNKNOW, TIMER_ZEDIT_UNKNOW_TIMEOUT, 0);
	return bRet;
}

void CZEdit::CenterTextV()
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
	SetRectNP(&rc);
	ReleaseDC(pDC);
}

void CZEdit::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int height = lpncsp->rgrc[0].bottom - lpncsp->rgrc[0].top;  
	int margin = 0;  

	CDC* pDC = GetDC();  
	TEXTMETRIC tm;  
	pDC->GetTextMetrics(&tm);  
	int nFontHeight = tm.tmHeight + tm.tmExternalLeading;  
	int nMargin = (height - nFontHeight) / 2;  

	//lpncsp->rgrc[0].left += 8;  
	lpncsp->rgrc[0].top += nMargin;  
	//lpncsp->rgrc[0].right -= 8;  
	lpncsp->rgrc[0].bottom -= (nMargin-1);  


	CBCGPEdit::OnNcCalcSize(bCalcValidRects, lpncsp);
}


void CZEdit::OnShowWindow(BOOL bShow, UINT nStatus)
{
	
	CBCGPEdit::OnShowWindow(bShow, nStatus);
	// TODO: 在此处添加消息处理程序代码
	CenterTextV();
}


void CZEdit::OnMove(int x, int y)
{
	CBCGPEdit::OnMove(x, y);
	CenterTextV();
	// TODO: 在此处添加消息处理程序代码
}



void CZEdit::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//CenterTextV();
	KillTimer( nIDEvent );
	CBCGPEdit::OnTimer(nIDEvent);
}


BOOL CZEdit::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CenterTextV();
	return CBCGPEdit::OnEraseBkgnd(pDC);
}
LRESULT CZEdit::OnMouseHover(WPARAM wp,LPARAM lp)
{
	m_bMouseTracked = FALSE;
	SetMouseTrack( m_bMouseTracked, GetSafeHwnd(), g_dwToolTipDelay );
	CPoint ptGlobal;
	::GetCursorPos(&ptGlobal);

	CString strTip = "";
	GetWindowText(strTip);

	CSize strSize = GetDC()->GetTextExtent(strTip);
	CRect rc(0,0,strSize.cx,strSize.cy);

	ptGlobal.x += g_ToolTipOffset.x;
	ptGlobal.y += g_ToolTipOffset.y;
	rc.MoveToXY(ptGlobal);

	m_ToolTip.SetTextMargin (TEXT_MARGIN + 2);
	m_ToolTip.SetFont ( &theApp.m_font );
	m_ToolTip.Track (rc, strTip);
	return 0;
}
LRESULT CZEdit::OnMouseLeaved(WPARAM wp,LPARAM lp)
{
	m_ToolTip.Deactivate();
	Invalidate();
	m_bMouseTracked = FALSE;
	return 0;
}
void CZEdit::OnMouseMove(UINT nFlags, CPoint point)
{
	SetMouseTrack( m_bMouseTracked, GetSafeHwnd(), g_dwToolTipDelay );
	CEdit::OnMouseMove(nFlags, point);
}
void CZEdit::OnNcPaint()
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