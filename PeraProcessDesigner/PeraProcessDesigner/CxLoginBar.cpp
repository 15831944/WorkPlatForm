// CxLoginBar.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "CxLoginBar.h"
#include "BCGPToolbarButton.h"
#include "PtrVectorHeader.h"
#include "CxBCGPToolbarButton.h"
#include "UserDefine.h"
#include "DlgLoginBk.h"
#include <BCGPDockBar.h>
#include "MainFrm.h"
#include "ConvertData.h"
#include "PeraProcessDesignerUtility.h"
#include "UtilityFunc.h"

static const int nHorzMargin = 2;
static const int g_nUserWidth = 138;
static const int g_nLoginWidth = 38 + 20;

//////////////////////////////////////////////////////////////////////////

class CCxLoginBarUserButton : public CBCGPToolbarButton
{
	DECLARE_DYNCREATE (CCxLoginBarUserButton)

public:
	CCxLoginBarUserButton() {}
	CCxLoginBarUserButton (UINT uiID, LPCTSTR lpszText = NULL) :
		CBCGPToolbarButton (0, -1, lpszText) {}
	virtual ~CCxLoginBarUserButton() {}

	virtual void CopyFrom (const CBCGPToolbarButton& src);
	virtual void OnDraw (CDC* pDC, const CRect& rect, CBCGPToolBarImages* pImages,
		BOOL bHorz = TRUE, BOOL bCustomizeMode = FALSE,
		BOOL bHighlight = FALSE,
		BOOL bDrawBorder = TRUE,
		BOOL bGrayDisabledButtons = TRUE);
	virtual SIZE OnCalculateSize (CDC* pDC, const CSize& sizeDefault, BOOL bHorz);
};

IMPLEMENT_DYNCREATE(CCxLoginBarUserButton, CBCGPToolbarButton)

void CCxLoginBarUserButton::CopyFrom (const CBCGPToolbarButton& s)
{
	CBCGPToolbarButton::CopyFrom (s);
	const CCxLoginBarUserButton& src = (const CCxLoginBarUserButton&) s;
}

void CCxLoginBarUserButton::OnDraw (CDC* pDC, const CRect& rect, CBCGPToolBarImages* /*pImages*/,
	BOOL /*bHorz*/, BOOL /*bCustomizeMode*/,
	BOOL bHighlight,
	BOOL /*bDrawBorder*/,
	BOOL /*bGrayDisabledButtons*/)
{
	ASSERT_VALID (this);

	CCxLoginBar* pBar = DYNAMIC_DOWNCAST(CCxLoginBar, m_pWndParent);
	if (pBar == NULL)
	{
		ASSERT(FALSE);
		return;
	}

	CRect rc = rect;
	if ( pBar->m_vImages[ CCxLoginBar::IMG_USER_BG ]->GetHeight() < (UINT)rc.Height() )
	{
		rc.DeflateRect( 0, (rc.Height() - pBar->m_vImages[ CCxLoginBar::IMG_USER_BG ]->GetHeight()) / 2 );
	}
	
	//Graphics g(pDC->GetSafeHdc());
	//g.DrawImage(pBar->m_vImages[ CCxLoginBar::IMG_USER_BG ]
	//	, (float)rc.left, (float)rc.top );
	//g.ReleaseHDC( pDC->GetSafeHdc() );

	CString str;
	if (theApp.m_LoginData.m_strTicket.IsEmpty())
	{
		str = "用户名：单机版";
	}
	else
	{
// 		if ( theApp.m_LoginData.m_strRealName.GetLength() > 12 )
// 		{
// 			str = "用户名：" + theApp.m_LoginData.m_strRealName.Mid( 0, 12 );
// 			str += "...";
// 		}
// 		else
// 		{
			str = "用户名：" + theApp.m_LoginData.m_strRealName;
//		}
	}
	rc = Rect();
	int nFontHeight = pDC->GetTextExtent (str).cy;
	if ( nFontHeight < rc.Height() )
	{
		rc.DeflateRect( 0, (rc.Height() - nFontHeight) / 2 );
	}
	CFont * pOldFont = pDC->SelectObject(&theApp.m_font);
	pDC->DrawText( str, &rc, DT_RIGHT|DT_VCENTER|DT_WORD_ELLIPSIS );
	pDC->SelectObject(pOldFont);
}


SIZE CCxLoginBarUserButton::OnCalculateSize (CDC* /*pDC*/, const CSize& /*sizeDefault*/, BOOL /*bHorz*/)
{
	ASSERT_VALID (this);
	return CSize (g_nUserWidth, 16);
}



//////////////////////////////////////////////////////////////////////////

class CCxLoginBarLoginButton : public CBCGPToolbarButton
{
	DECLARE_DYNCREATE (CCxLoginBarLoginButton)

public:
	CCxLoginBarLoginButton() {}
	CCxLoginBarLoginButton (UINT uiID, LPCTSTR lpszText = NULL) :
	CBCGPToolbarButton (0, -1, lpszText) {}
	virtual ~CCxLoginBarLoginButton() {}

	virtual void CopyFrom (const CBCGPToolbarButton& src);
	virtual void OnDraw (CDC* pDC, const CRect& rect, CBCGPToolBarImages* pImages,
		BOOL bHorz = TRUE, BOOL bCustomizeMode = FALSE,
		BOOL bHighlight = FALSE,
		BOOL bDrawBorder = TRUE,
		BOOL bGrayDisabledButtons = TRUE);
	virtual SIZE OnCalculateSize (CDC* pDC, const CSize& sizeDefault, BOOL bHorz);
	virtual BOOL CanBeStretched () const { return TRUE; }
	virtual BOOL OnClick (CWnd* pWnd, BOOL bDelay = TRUE);
};

IMPLEMENT_DYNCREATE(CCxLoginBarLoginButton, CBCGPToolbarButton)

void CCxLoginBarLoginButton::CopyFrom (const CBCGPToolbarButton& s)
{
	CBCGPToolbarButton::CopyFrom (s);
	const CCxLoginBarLoginButton& src = (const CCxLoginBarLoginButton&) s;
}

void CCxLoginBarLoginButton::OnDraw (CDC* pDC, const CRect& rect, CBCGPToolBarImages* /*pImages*/,
	BOOL /*bHorz*/, BOOL /*bCustomizeMode*/,
	BOOL bHighlight,
	BOOL /*bDrawBorder*/,
	BOOL /*bGrayDisabledButtons*/)
{
	ASSERT_VALID (this);

	CCxLoginBar* pBar = DYNAMIC_DOWNCAST(CCxLoginBar, m_pWndParent);
	if (pBar == NULL)
	{
		ASSERT(FALSE);
		return;
	}
	CStringW strText;
	int nIndex = CCxLoginBar::IMG_LOGIN;
	if ( theApp.m_LoginData.m_strTicket.IsEmpty() ) //如果没有ticket
	{
		//登录
		strText = L"登录";
/*		if (m_nStyle & TBBS_PRESSED)
		{
			nIndex = CCxLoginBar::IMG_LOGIN_PRESSED;
		}
		else*/ if (bHighlight)
		{
			nIndex = CCxLoginBar::IMG_LOGIN_FOCUSED;
		}
		else
		{
			nIndex = CCxLoginBar::IMG_LOGIN;
		}
	}
	else
	{
		//登出
		strText = L"注销";
/*		if (m_nStyle & TBBS_PRESSED)
		{
			nIndex = CCxLoginBar::IMG_LOGOUT_PRESSED;
		}
		else*/ if (bHighlight)
		{
			nIndex = CCxLoginBar::IMG_LOGOUT_FOCUSED;
		}
		else
		{
			nIndex = CCxLoginBar::IMG_LOGOUT;
		}
	}

	CRect rc = Rect();
	if ( pBar->m_vImages[ nIndex ]->GetHeight() < (UINT)rc.Height() )
	{
		rc.DeflateRect( 0, (rc.Height() - pBar->m_vImages[ nIndex ]->GetHeight()) / 2 );
	}

	CRect rcText = rc;
	CRect rcIcon = rc;
	rcIcon.left = rcIcon.right - rcIcon.Height();
	rcText.right = rcIcon.left - 2;

	RectF rcfText ((float)rcText.left, (float)rcText.top
		, (float)rcText.Width(), (float)rcText.Height());

	Graphics g(pDC->GetSafeHdc());

	CStringW strW = L"宋体";
	FontFamily family((WCHAR *)(LPCWSTR)strW);
	Gdiplus::Font ft (&family, 12, FontStyleRegular, UnitPixel);
	
	StringFormat ff;
	ff.SetAlignment(StringAlignmentFar); //水平方向
	ff.SetLineAlignment(StringAlignmentCenter); //垂直方向

	SolidBrush brushText(Color::Blue);

	g.DrawString((WCHAR*)(LPCWSTR)strText
		, strText.GetLength()
		, &ft
		, rcfText
		, &ff
		, &brushText);

	g.DrawImage(pBar->m_vImages[ nIndex ]
		, (float)rcIcon.left, (float)rcIcon.top );

	g.ReleaseHDC( pDC->GetSafeHdc() );
}

SIZE CCxLoginBarLoginButton::OnCalculateSize (CDC* /*pDC*/, const CSize& /*sizeDefault*/, BOOL /*bHorz*/)
{
	ASSERT_VALID (this);
	return CSize (g_nLoginWidth, 16);
}

BOOL CCxLoginBarLoginButton::OnClick (CWnd* pWnd, BOOL bDelay)
{
	CCxLoginBar* pBar = DYNAMIC_DOWNCAST(CCxLoginBar, m_pWndParent);
	if (pBar == NULL)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	if ( theApp.m_LoginData.m_strTicket.IsEmpty() )
	{
		//登录
		CDlgLoginBk dlgLogin;
		dlgLogin.DoModal();

		pBar->m_iButtonCapture = -1;
 		pBar->m_iHighlighted = -1;
		pBar->RedrawWindow();

		return TRUE;
	}
	else
	{
		//登出
		//pBar->m_LoginData.m_strTicket = "";
		//LoginOut();
		::PostMessage( theApp.m_pMainWnd->GetSafeHwnd(), WM_LOGINBAR_CLICKED_LOGOUT, 0, 0 );
	}

	pBar->RedrawWindow();
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
// CCxLoginBar

BEGIN_MESSAGE_MAP(CCxLoginBar, CBCGPToolBar)
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeaved)
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CCxLoginBar, CBCGPToolBar)

CCxLoginBar::CCxLoginBar()
{
	m_nUserID = IDC_LOGINBAR_USER;
	m_nLoginID = IDC_LOGINBAR_LOGIN;
	m_pMiniFrameRTC = RUNTIME_CLASS (CBCGPMiniFrameWnd);
	m_bUserOP = TRUE;
	m_bMouseTracked = FALSE;
}

CCxLoginBar::~CCxLoginBar()
{
}

void CCxLoginBar::OnAfterDock (CBCGPBaseControlBar* pBar, LPCRECT lpRect, BCGP_DOCK_METHOD dockMethod) 
{
	m_bFloating = FALSE;
	CBCGPBaseToolBar::OnAfterDock  (pBar, lpRect, dockMethod);

	if(m_pParentDockBar && dockMethod == BCGP_DM_DBL_CLICK || dockMethod == BCGP_DM_RECT)
	{
		MoveToRight();
	}
	
	if (GetParent() != NULL)
	{
		CRect rect;
		GetWindowRect (rect);
		GetParent()->ScreenToClient (rect);

		GetParent()->RedrawWindow (rect);
	}
}

void CCxLoginBar::MoveToRight()
{
	if(theApp.m_bCreate == FALSE) return;
	CRect rectRobot;
	CRect rectTool;
	CMainFrame* pMain = (CMainFrame*)theApp.GetMainWnd();
	if(!IsWindow(pMain->GetSafeHwnd()) || !IsWindow(pMain->m_wndRobotBar.GetSafeHwnd()) ||  !IsWindow(pMain->m_wndToolBar.GetSafeHwnd())) return;
	pMain->m_wndRobotBar.GetWindowRect(rectRobot);
	pMain->m_wndToolBar.GetWindowRect(rectTool);
	if(m_pParentDockBar && ((IsLeftOf(rectRobot) && IsLeftOf(rectTool) && pMain->m_wndRobotBar.GetDockBarRow() == GetDockBarRow()) || pMain->m_wndRobotBar.GetDockBarRow() != GetDockBarRow() ))
	{
		SetUserOP(FALSE);
		m_pParentDockBar->MoveControlBar(this, 0, CPoint(9999, 0));
		m_pDockBarRow->FixupVirtualRects (false);
		SetUserOP(TRUE);
	}
}

//***********************************************************************************
BCGP_CS_STATUS CCxLoginBar::IsChangeState (int nOffset, 
											 CBCGPBaseControlBar** ppTargetBar) const
{
	if(!IsUserOP()) return BCGP_CS_NOTHING;

	ASSERT_VALID (this);
	ASSERT (ppTargetBar != NULL);
	

	CPoint	ptMousePos;
	CRect	rectBarWnd;
	CRect	rectDockBarWnd;
	CRect	rectIntersect;

	CRect	rectVirtual;

	CPoint  ptDelta;

	GetCursorPos (&ptMousePos);

	GetWindowRect  (&rectBarWnd);
	GetVirtualRect (rectVirtual);

	// check whether the mouse is around a dock bar
	CBCGPBaseControlBar* pBaseBar = ControlBarFromPoint (ptMousePos, nOffset, FALSE,
											RUNTIME_CLASS (CBCGPDockBar)); 

	*ppTargetBar = DYNAMIC_DOWNCAST (CBCGPDockBar, pBaseBar);

	if (m_pParentDockBar != NULL)
	{
		// the mouse is around the dock bar, check the virtual rect
		m_pParentDockBar->GetWindowRect (&rectDockBarWnd);
		if (!rectIntersect.IntersectRect (rectDockBarWnd, rectVirtual))
		{
			return BCGP_CS_DOCK_IMMEDIATELY;
		}

		// there is some intersection of the virtual rectangle an the dock bar. 
		// special processing when horizontal bar is about to float in horizontal direction
		bool bTreatMouse = false; 
		if (m_pParentDockBar->IsHorizontal ())
		{

			if (rectVirtual.left < rectDockBarWnd.left && 
				rectDockBarWnd.left - rectVirtual.left > nOffset * 2 ||
				rectVirtual.right > rectDockBarWnd.right && 
				rectVirtual.right - rectDockBarWnd.right > nOffset * 2)
			{
				bTreatMouse = true;	
			}
		}
		else
		{
			if (rectVirtual.top < rectDockBarWnd.top && 
				rectDockBarWnd.top - rectVirtual.top > nOffset * 2 ||
				rectVirtual.bottom > rectDockBarWnd.bottom && 
				rectVirtual.bottom - rectDockBarWnd.bottom > nOffset * 2)
			{
				bTreatMouse = true;	
			}
			
		}

		if (bTreatMouse && !rectDockBarWnd.PtInRect (ptMousePos))
		{
			return BCGP_CS_DOCK_IMMEDIATELY;
		}
	}
	else
	{
		if (*ppTargetBar == NULL)
		{
			// the mouse is out of dock bar in either direction - keep the bar floating
			return BCGP_CS_NOTHING;
		}

		if (!CanBeDocked (*ppTargetBar))
		{
			// bar's style does not allow to dock the bar to this dock bar
			return BCGP_CS_NOTHING;
		}
		// the mouse is getting closer to a dock bar
		(*ppTargetBar)->GetWindowRect (&rectDockBarWnd);

		if (rectDockBarWnd.PtInRect (ptMousePos))
		{
			// the mouse is over the dock bar, the bar must be docked
			return BCGP_CS_DOCK_IMMEDIATELY;
		}

		// check on which side the mouse is relatively to the dock bar
		bool bMouseLeft = ptMousePos.x < rectDockBarWnd.left;
		bool bMouseRight = ptMousePos.x > rectDockBarWnd.right;
		bool bMouseTop  = ptMousePos.y < rectDockBarWnd.top;
		bool bMouseBottom = ptMousePos.y > rectDockBarWnd.bottom;

		double	dPixelsOnDock = nOffset;
		int		nMouseOffset  = 0;
		if (bMouseLeft)
		{
			dPixelsOnDock = ((rectBarWnd.right - ptMousePos.x) * 100. / 
								rectBarWnd.Width ()) / 100. * nOffset;
			nMouseOffset = rectDockBarWnd.left - ptMousePos.x;
			
		}
		else if (bMouseRight)
		{
			dPixelsOnDock = ((ptMousePos.x - rectBarWnd.left) * 100. / 
								rectBarWnd.Width ()) / 100. * nOffset;
			nMouseOffset = ptMousePos.x - rectDockBarWnd.right;
		}
		else if (bMouseTop)
		{
			dPixelsOnDock = ((rectBarWnd.bottom - ptMousePos.y) * 100. / 
								rectBarWnd.Height ()) / 100. * nOffset;
			nMouseOffset = rectDockBarWnd.top - ptMousePos.y;
		}
		else if (bMouseBottom)
		{
			dPixelsOnDock = ((ptMousePos.y - rectBarWnd.top) * 100. / 
								rectBarWnd.Height ()) / 100. * nOffset;
			nMouseOffset = ptMousePos.y - rectDockBarWnd.bottom;
		}

		if (nMouseOffset <= dPixelsOnDock)
		{
			return BCGP_CS_DOCK_IMMEDIATELY;
		}
	}

	return BCGP_CS_NOTHING;
}

//********************************************************************************
BOOL CCxLoginBar::Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID)
{
	return CreateEx(pParentWnd, 0, dwStyle, nID);
}
//********************************************************************************
BOOL CCxLoginBar::CreateEx (CWnd* pParentWnd, DWORD /*dwCtrlStyle*/, DWORD dwStyle, 
							 UINT nID)
{
	ASSERT_VALID(pParentWnd);   // must have a parent

	m_dwStyle |= CBRS_HIDE_INPLACE;

	//dwStyle &= ~CBRS_GRIPPER;

	m_dwStyle = (dwStyle & CBRS_ALL);
	// save the style
	SetBarAlignment (dwStyle & CBRS_ALL);

	SetBorders (CRect(1, 0, 1, 0));

	// create the HWND
	CRect rect;
	rect.SetRectEmpty();

#ifndef _BCGSUITE_
	m_dwBCGStyle = 0; // can't float, resize, close, slide
#else
	m_dwControlBarStyle = 0; // can't float, resize, close, slide
#endif

	if (!CBCGPBaseToolBar::Create (
		globalData.RegisterWindowClass (_T("CCxLoginBar")), dwStyle, rect, pParentWnd, nID, 0))
	{
		return FALSE;
	}

	////if (!CWnd::Create (globalData.RegisterWindowClass (_T("CCxLoginBar")),
	//if (!CWnd::Create (globalData.RegisterWindowClass (_T("CCxLoginBar")),
	//	NULL, dwStyle | WS_CLIPSIBLINGS, rect, pParentWnd, nID))
	//{
	//	return FALSE;
	//}

	//if (pParentWnd->IsKindOf (RUNTIME_CLASS (CBCGPFrameWnd)))
	//{
	//	((CBCGPFrameWnd*) pParentWnd)->AddControlBar (this);
	//}
	//else if (pParentWnd->IsKindOf (RUNTIME_CLASS (CBCGPMDIFrameWnd)))
	//{
	//	((CBCGPMDIFrameWnd*) pParentWnd)->AddControlBar (this);
	//}
	//else
	//{
	//	ASSERT (FALSE);
	//	return FALSE;
	//}

	return TRUE;
}

void CCxLoginBar::OnLoadImages ()
{
	//static const int cxImg = 16;
	//static const int cyImg = 16;

	static const LPCTSTR arrImageName[] = {
		"User.png",
		"Login.png", "login_Pressed.png", "login_Focused.png",
		"Logout.png", "logout_Pressed.png", "logout_Focused.png"
	};

	CString strDir = GetExecDir() + "\\Res\\Login\\";
	Bitmap * pBitmap = NULL;
	HBITMAP hbmp = NULL;

	//ResetAll();
	ClearImages();

	Color crImgBg;
	crImgBg.SetFromCOLORREF(g_crToolbarBg);

	//m_BCGToolbarImages.SetImageSize( CSize(cxImg, cyImg) );
	m_BCGToolbarImages.SetTransparentColor (g_crToolbarBg);

	int nCount = sizeof( arrImageName ) / sizeof( LPCTSTR );
	for (int i=0; i<nCount; i++)
	{
		pBitmap = GetNewBitmap( strDir + arrImageName[i] );
		if ( pBitmap == NULL )
		{
			CString strLog;
			strLog.Format( "图片资源丢失，%s", arrImageName[i] );
			MessageBox( strLog, g_lpszAppTitle );

			ClearImages();
			return;
		}
		m_vImages.push_back( pBitmap );
		pBitmap->GetHBITMAP( crImgBg, &hbmp);
		if ( hbmp == NULL )
		{
			ClearImages();
			return;
		}
		m_vHBitmaps.push_back( hbmp );
		m_BCGToolbarImages.AddImage( hbmp );
	}

	if ( m_BCGToolbarImages.GetCount() == 0 ) return;
}

void CCxLoginBar::ClearImages()
{
	m_BCGToolbarImages.Clear();
	for (int i=0; i<(int)m_vHBitmaps.size(); i++)
	{
		DeleteObject ( m_vHBitmaps[i] );
	}
	m_vHBitmaps.clear();
	PtrVectorClear( m_vImages );
}

void CCxLoginBar::DoPaint( CDC* pDC )
{
	CBCGPToolBar::DoPaint(pDC);
}

//**********************************************************************************
void CCxLoginBar::AdjustLocations ()
{
	ASSERT_VALID(this);
	
	if (m_Buttons.IsEmpty () || GetSafeHwnd () == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect (rectClient);
	if ( rectClient.IsRectEmpty() || rectClient.IsRectNull() )
	{
		return;
	}

	CRect rc;

	for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL;)
	{
		CBCGPToolbarButton* pButton = (CBCGPToolbarButton*) m_Buttons.GetNext (pos);
		if (pButton == NULL || pButton == (CBCGPToolbarButton*)m_pCustomizeBtn)
		{
			break;
		}

		ASSERT_VALID (pButton);

		if (pButton->IsVisible ())
		{
			if ( pButton->IsKindOf( RUNTIME_CLASS(CCxLoginBarUserButton) ) )
			{
				rc = rectClient;
				rc.right = rectClient.right - g_nLoginWidth - 1;
				pButton->SetRect (rc);
			}
			else if ( pButton->IsKindOf( RUNTIME_CLASS(CCxLoginBarLoginButton) ) )
			{
				rc = rectClient;
				rc.left = rectClient.right - g_nLoginWidth;
				pButton->SetRect (rc);
			}
		}
	}
}

//**********************************************************************************
void CCxLoginBar::OnFillBackground (CDC* pDC)
{
	ASSERT_VALID(this);
	ASSERT_VALID (pDC);
	
	//CBCGPToolBar::OnFillBackground (pDC);

	CRect rc;
	GetClientRect(&rc);

	CBCGPDrawManager dm(*pDC);
	dm.DrawRect(&rc, g_crToolbarBg, g_crToolbarBg);
}

//**********************************************************************************
int CCxLoginBar::CalcMaxButtonHeight ()
{
	int nNextIndex = 0;

	if (m_Buttons.GetCount() == 0)
	{
		InsertButton (CCxLoginBarUserButton (m_nUserID), 0);
		InsertButton (CCxLoginBarLoginButton (m_nLoginID), 1);
		nNextIndex = 2;
	}

	return CBCGPToolBar::CalcMaxButtonHeight ();
}

int CCxLoginBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGPToolBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	OnLoadImages ();
	m_ToolTip.Create (this);
	return 0;
}
LRESULT CCxLoginBar::OnMouseHover(WPARAM wp,LPARAM lp)
{
	//当此消息产生时，盘旋跟踪停止。如果需要进一步的鼠标盘旋跟踪，应用程序应当再次调用TrackMouseEvent。
	m_bMouseTracked = FALSE;
	SetMouseTrack( m_bMouseTracked, GetSafeHwnd(), g_dwToolTipDelay );
	CPoint ptGlobal;
	::GetCursorPos(&ptGlobal);
	CPoint ptClient = ptGlobal;

	ScreenToClient(&ptClient);
	int iBtn = HitTest(ptClient); // 根据光标位置获取按钮ID
	CString strTip = "";
	if (iBtn==0)
	{
		CString str;
		if (theApp.m_LoginData.m_strTicket.IsEmpty())
			strTip = "用户名：单机版";
		else
			strTip = "用户名：" + theApp.m_LoginData.m_strRealName;
	}
	else if (iBtn==1)
	{
		if (theApp.m_LoginData.m_strTicket.IsEmpty())
			strTip = "登录";
		else
			strTip = "注销";
	}
	else
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
LRESULT CCxLoginBar::OnMouseLeaved(WPARAM wp,LPARAM lp)
{
	m_ToolTip.Deactivate();
	Invalidate();
	m_bMouseTracked = FALSE;
	return 0;
}
void CCxLoginBar::OnMouseMove(UINT nFlags, CPoint point)
{
	SetMouseTrack( m_bMouseTracked, GetSafeHwnd(), g_dwToolTipDelay );
	CBCGPToolBar::OnMouseMove(nFlags, point);
}