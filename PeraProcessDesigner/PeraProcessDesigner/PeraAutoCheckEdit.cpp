// PeraAutoCheckEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "PeraAutoCheckEdit.h"
#include "UserDefine.h"
#include "UtilityFunc.h"

// CPeraAutoCheckEdit

IMPLEMENT_DYNAMIC(CPeraAutoCheckEdit, CEdit)

CPeraAutoCheckEdit::CPeraAutoCheckEdit()
{
	m_WndPro = NULL;
	m_ErrBkColor = g_crEditErrBkColor;
	m_bErr = false;
	m_pOldBrush = NULL;
	m_bMouseTracked = FALSE;
}

CPeraAutoCheckEdit::~CPeraAutoCheckEdit()
{
}


BEGIN_MESSAGE_MAP(CPeraAutoCheckEdit, CEdit)
	ON_WM_CHAR()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeaved)
END_MESSAGE_MAP()



// CPeraAutoCheckEdit 消息处理程序
void CPeraAutoCheckEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	 CEdit::OnChar(nChar, nRepCnt, nFlags);
	 if (m_WndPro)
	 {
		 BOOL bBkChanged = FALSE;
		LRESULT lR =  ::SendMessage(m_WndPro,WM_EDIT_AUTOCHECK,nChar,(LPARAM)this);
		if (lR==S_OK)
		{
			if (m_bErr)
			{
				bBkChanged = TRUE;
				HideBalloonTip();
			}
			m_bErr = false;
		}
		else
		{
			if (!m_bErr)
				bBkChanged = TRUE;

			m_bErr = true;
		}
		
		if (bBkChanged)
		   Invalidate();
	 }
}
void CPeraAutoCheckEdit::SetMsgProWnd(HWND hWnd)
{
	if (::IsWindow(hWnd))
	{
		m_WndPro = hWnd;
	}
}

HBRUSH CPeraAutoCheckEdit::CtlColor(CDC* pDC, UINT nCtlColor)
{
	// TODO:  在此更改 DC 的任何特性
    // 返回红色出错误的颜色的刷子
	if (m_bErr)
	{
		pDC->SetBkColor(m_ErrBkColor);
	   return  CreateSolidBrush(m_ErrBkColor);
	}
	else
	{
		pDC->SetBkColor(RGB(255,255,255));
		return NULL;
	}
}
void CPeraAutoCheckEdit::ChangeBkColor(bool bErr)
{
	m_bErr = bErr;
	Invalidate(TRUE);
}
void CPeraAutoCheckEdit::ShowBalloonTip(LPCWSTR strTitile,LPCWSTR strtext,UINT uStyle)
{
	EDITBALLOONTIP ebt;
	ebt.cbStruct = sizeof(EDITBALLOONTIP);
	ebt.pszText = strtext;
	ebt.pszTitle = strTitile;
	ebt.ttiIcon = uStyle;
	Edit_ShowBalloonTip(GetSafeHwnd(),&ebt);
}
void CPeraAutoCheckEdit::HideBalloonTip()
{
	ASSERT(::IsWindow(m_hWnd));
	Edit_HideBalloonTip(m_hWnd); // EM_HIDEBALLOONTIP
}
LRESULT CPeraAutoCheckEdit::OnMouseHover(WPARAM wp,LPARAM lp)
{
	m_bMouseTracked = FALSE;
	SetMouseTrack( m_bMouseTracked, GetSafeHwnd(), g_dwToolTipDelay );
	CPoint ptGlobal;
	::GetCursorPos(&ptGlobal);

	CString strTip = "";
	GetWindowText(strTip);
	if (strTip.IsEmpty())
		return 0;

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
LRESULT CPeraAutoCheckEdit::OnMouseLeaved(WPARAM wp,LPARAM lp)
{
	m_ToolTip.Deactivate();
	Invalidate();
	m_bMouseTracked = FALSE;
	return 0;
}
void CPeraAutoCheckEdit::OnMouseMove(UINT nFlags, CPoint point)
{
	SetMouseTrack( m_bMouseTracked, GetSafeHwnd(), g_dwToolTipDelay );
	CEdit::OnMouseMove(nFlags, point);
}

void CPeraAutoCheckEdit::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	if (m_ToolTip.m_hWnd == NULL)
		m_ToolTip.Create(this);

	CEdit::PreSubclassWindow();
}
