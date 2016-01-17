// LoginCloseBtn.cpp : 实现文件
//

#include "stdafx.h"
//#include "IMClient.h"
#include "LoginCloseBtn.h"


// CLoginCloseBtn

IMPLEMENT_DYNAMIC(CLoginCloseBtn, CButton)

CLoginCloseBtn::CLoginCloseBtn()
{
	m_bIn = FALSE;
}

CLoginCloseBtn::~CLoginCloseBtn()
{
}


BEGIN_MESSAGE_MAP(CLoginCloseBtn, CButton)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER,OnMouseHover)
END_MESSAGE_MAP()



// CLoginCloseBtn 消息处理程序



void CLoginCloseBtn::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CButton::OnMouseMove(nFlags, point);
	TRACKMOUSEEVENT tme;       
	memset(&tme, NULL, sizeof(tme));
	tme.cbSize = sizeof(TRACKMOUSEEVENT);       
	tme.dwFlags = TME_HOVER | TME_LEAVE;       
	tme.dwHoverTime = 1;     
	tme.hwndTrack = m_hWnd;   
	_TrackMouseEvent( &tme ); 
}

LRESULT CLoginCloseBtn::OnMouseLeave(WPARAM /*wParam*/,LPARAM /*lParam*/)       
{     
	if (m_bIn == TRUE)
	{
		m_bIn = FALSE;
		::SendMessage(GetParent()->GetSafeHwnd(), WM_LOGIN_CLOSEBUTTON_MOUSELEAVE, 0, 0);
	}
	return 0;       
}       

LRESULT CLoginCloseBtn::OnMouseHover(WPARAM /*wParam*/,LPARAM /*lParam*/)       
{      
	if (m_bIn == FALSE)
	{
		m_bIn = TRUE;
		::SendMessage(GetParent()->GetSafeHwnd(), WM_LOGIN_CLOSEBUTTON_MOUSEOVER, 0, 0);
	}

	return 0;       
}       
