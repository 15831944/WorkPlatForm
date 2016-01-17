// ZEdit.cpp : ʵ���ļ�
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



// CZEdit ��Ϣ�������




void CZEdit::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CBCGPEdit::OnLButtonDown(nFlags, point);
}


void CZEdit::OnEnKillfocus()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_nPos = GetSel() & 0xFFFF;
	m_nPosEnd = GetSel() >> 16 & 0xFFFF;
	GetWindowText(m_strText);
	//CWndIf* pWndParent = ((CWndIf*)GetParent());
	//((CWndIf*)GetParent())->m_vBCs[pWndParent->m_nBranchSelect]->m_staArrow.ShowWindow(SW_HIDE);
}


void CZEdit::OnEnSetfocus()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	{//��һ��
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
	{//������1��
		pBtnDown->EnableWindow(FALSE);
	}
	else
	{
		pBtnDown->EnableWindow(TRUE);
	}
}


HBRUSH CZEdit::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	// TODO:  �ڴ˸��� DC ���κ�����

	// TODO:  �����Ӧ���ø����Ĵ�������򷵻ط� null ����
	CBrush      m_brushedit;   
	m_brushedit.CreateSolidBrush      (RGB      (      255,      0,      0      )      );   

	pDC->SetTextColor(RGB(255,0,0));          //������ɫ   
	//pDC->SetBkMode(OPAQUE);
	//pDC->SetBkMode(TRANSPARENT);
	pDC->SetBkColor(RGB(0,255,0));      //���ֱ�����ɫ   

	//return NULL;
	return (HBRUSH)m_brushedit.GetSafeHandle()      ;        //edit�����ɫ   
}


void CZEdit::SetBkGrndColor()
{
	// ɾ���ɵ�ˢ�� 
	m_Brush.DeleteObject();
	//����һ��m_BackColor��ɫ��ˢ�� 
	m_Brush.CreateSolidBrush(m_BackColor); 
	CDC* pDC = GetDC();
	//����������ʾģʽ��OPAQUEΪ��ռģʽ 
	pDC->SetBkMode(OPAQUE);
	//�������ֵı�����ɫ 
	pDC->SetBkColor(m_BackColor);
	//ѡ��ˢ�� 
	CBrush* pBrushT = pDC->SelectObject(&m_Brush); 
	CRect rc;
	GetClientRect(&rc);
	//ScreenToClient(&rc);
	//����һ������ 
	pDC->Rectangle(0, 0, rc.Width(), rc.Height());
	//�����ı���ɫ 
	pDC->SetTextColor(RGB(255, 255, 255));
	//����ı� 
	GetWindowText(m_strText);
	pDC->TextOut(2, 2, m_strText);
	pDC->SelectObject(pBrushT); 
	ReleaseDC(pDC);

}

void CZEdit::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	SetBkGrndColor();

	// ��Ϊ��ͼ��Ϣ���� 
	//CEdit::OnPaint();
}


int CZEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CenterTextV();

	if (CBCGPEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	CenterTextV();
	// TODO:  �ڴ������ר�õĴ�������
	CRect rect;
	GetWindowRect(rect);
	OnSize(0, rect.Width(), rect.Height());
	return 0;
}


void CZEdit::OnSize(UINT nType, int cx, int cy)
{
	CBCGPEdit::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
	CenterTextV();
}


BOOL CZEdit::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO: �ڴ����ר�ô����/����û���
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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
	// TODO: �ڴ˴������Ϣ����������
	CenterTextV();
}


void CZEdit::OnMove(int x, int y)
{
	CBCGPEdit::OnMove(x, y);
	CenterTextV();
	// TODO: �ڴ˴������Ϣ����������
}



void CZEdit::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//CenterTextV();
	KillTimer( nIDEvent );
	CBCGPEdit::OnTimer(nIDEvent);
}


BOOL CZEdit::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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