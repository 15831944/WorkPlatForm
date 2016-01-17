#include "stdafx.h"
#include "CxFloatBar.h"
#include "CxBCGExtHeader.h"

static int g_nCaptionHorzMargin = 5;

IMPLEMENT_DYNCREATE(CCxFloatBar,CBCGPDockingControlBar)

BEGIN_MESSAGE_MAP(CCxFloatBar, CBCGPDockingControlBar)
	ON_WM_SIZE()
	ON_WM_NCPAINT()
END_MESSAGE_MAP()

CCxFloatBar::CCxFloatBar(void)
{
}

CCxFloatBar::~CCxFloatBar(void)
{
}


void CCxFloatBar::OnSize(UINT nType, int cx, int cy) 
{
	CBCGPDockingControlBar::OnSize(nType, cx, cy);
}


void CCxFloatBar::SetCaptionButtons ()
{
	CBCGPDockingControlBar::SetCaptionButtons ();
	//RemoveCaptionButtons ();

	//m_arrButtons.Add (new CBCGPCaptionButton (HTCLOSE_BCGEXT));
	//m_arrButtons.Add (new CBCGPCaptionButton (HTMAXBUTTON));
}

void CCxFloatBar::OnPressButtons( UINT nHit )
{

}

//***********************************************************************
void CCxFloatBar::OnPressCloseButton ()
{
	CFrameWnd* pParentFrame = DYNAMIC_DOWNCAST (CFrameWnd, BCGPGetParentFrame (this));
	ASSERT_VALID (pParentFrame);

	if (pParentFrame != NULL)
	{
		if (pParentFrame->SendMessage (BCGM_ON_PRESS_CLOSE_BUTTON, NULL, (LPARAM) (LPVOID) this))
		{
			return;
		}
	}

	if (IsAutoHideMode ())
	{
		SetAutoHideMode (FALSE, GetCurrentAlignment ());
	}
	ShowControlBar (FALSE, FALSE, FALSE);
	AdjustDockingLayout ();
}

//***********************************************************************
void CCxFloatBar::OnNcPaint() 
{
	if (m_bMultiThreaded)
	{
		g_cs.Lock ();
	}

	ASSERT_VALID (this);
	
	// get window DC that is clipped to the non-client area
    CWindowDC dcPaint (this);

	CRect rectUpd;
	GetUpdateRect (rectUpd);

    CRect rcClient, rcBar;
    GetClientRect(rcClient);
    ClientToScreen(rcClient);
    GetWindowRect(rcBar);

    rcClient.OffsetRect(-rcBar.TopLeft());
    rcBar.OffsetRect(-rcBar.TopLeft());

	CDC*		pDC = &dcPaint;
	BOOL		m_bMemDC = FALSE;
	CDC			dcMem;
	CBitmap		bmp;
	CBitmap*	pOldBmp = NULL;

	if (dcMem.CreateCompatibleDC (&dcPaint) &&
		bmp.CreateCompatibleBitmap (&dcPaint, rcBar.Width (),
								  rcBar.Height ()))
	{
		//-------------------------------------------------------------
		// Off-screen DC successfully created. Better paint to it then!
		//-------------------------------------------------------------
		m_bMemDC = TRUE;
		pOldBmp = dcMem.SelectObject (&bmp);
		pDC = &dcMem;
	}

    // client area is not our bussiness :)
	//解决bar切换焦点时，矩形区域上边线有时不被重绘的问题。
	CRect rc1 = rcClient;
	rc1.DeflateRect(1,0,1,1); 
    dcPaint.ExcludeClipRect(rc1);
	//
	OnEraseNCBackground (pDC, rc1);

	CRgn rgn;
	if (!m_rectRedraw.IsRectEmpty ())
	{
		rgn.CreateRectRgnIndirect (m_rectRedraw);
		dcPaint.SelectClipRgn (&rgn);
	}

    // erase parts not drawn
    dcPaint.IntersectClipRect(rcBar);

    // erase NC background the hard way
	OnEraseNCBackground (pDC, rcBar);

	int cyGripper = GetCaptionHeight ();

    if (cyGripper > 0)
	{
		// Paint caption and buttons:
		CRect rectCaption;
		
		GetWindowRect (&rectCaption);
		ScreenToClient (&rectCaption);

		rectCaption.OffsetRect (-rectCaption.left, -rectCaption.top);
		rectCaption.DeflateRect (0, 1);

		rectCaption.left = rcClient.left - m_arScrollButtons[2]->GetRect().Width();
		rectCaption.top --;
		rectCaption.bottom = rectCaption.top + cyGripper - 2;

		DrawCaption (pDC, rectCaption);

		for (int i = 0; i < m_arrButtons.GetSize (); i ++)
		{
			CBCGPCaptionButton* pbtn = m_arrButtons [i];
			ASSERT_VALID (pbtn);

			BOOL bIsMax = FALSE;

			switch (pbtn->GetHit ())
			{
			case HTMAXBUTTON:
				bIsMax = m_bPinState;
				break;

			case HTMINBUTTON:
				bIsMax = TRUE;
				break;
			}

			pbtn->OnDraw (pDC, m_bActive, IsHorizontal (), bIsMax);
			pbtn->m_clrForeground = (COLORREF)-1;
		}
	}

	for (int nStep = 0; nStep < 2; nStep++)
	{
		for (int i = 0; i < m_arScrollButtons.GetSize(); i++)
		{
			BOOL bIsActive = m_arScrollButtons[i]->m_bFocused;

			if ((bIsActive && nStep == 1) || (!bIsActive && nStep == 0))
			{
				m_arScrollButtons[i]->OnDraw(pDC, FALSE, FALSE, FALSE);
			}
		}
	}

	if (m_bMemDC)
	{
		//--------------------------------------
		// Copy the results to the on-screen DC:
		//-------------------------------------- 
		dcPaint.BitBlt (rcBar.left, rcBar.top, rcBar.Width(), rcBar.Height(),
					   &dcMem, rcBar.left, rcBar.top, SRCCOPY);

		dcMem.SelectObject(pOldBmp);
	}

	dcPaint.SelectClipRgn (NULL);

	if (m_bMultiThreaded)
	{
		g_cs.Unlock ();
	}
}


//*****************************************************************************************
void CCxFloatBar::DrawCaption (CDC* pDC, CRect rectCaption)
{
	ASSERT_VALID (pDC);

	CRect rcbtnRight = CRect (rectCaption.BottomRight (), CSize (0, 0));
	int i = 0;

	for (i = (int) m_arrButtons.GetUpperBound (); i >= 0; i --)
	{
		if (!m_arrButtons [i]->m_bLeftAlign && !m_arrButtons [i]->m_bHidden)
		{
			rcbtnRight = m_arrButtons [i]->GetRect();
			break;
		}
	}

	CRect rcbtnLeft = CRect (rectCaption.TopLeft (), CSize (0, 0));
	for (i = (int) m_arrButtons.GetUpperBound (); i >= 0; i --)
	{
		if (m_arrButtons [i]->m_bLeftAlign && !m_arrButtons [i]->m_bHidden)
		{
			rcbtnLeft = m_arrButtons [i]->GetRect();
			break;
		}
	}

	COLORREF clrCptnText = CBCGPVisualManager::GetInstance ()->OnDrawControlBarCaption (
		pDC, this, m_bActive, rectCaption, rcbtnRight);

	for (i = 0; i < m_arrButtons.GetSize (); i ++)
	{
		CBCGPCaptionButton* pbtn = m_arrButtons [i];
		ASSERT_VALID (pbtn);

		pbtn->m_clrForeground = clrCptnText;
	}

    int nOldBkMode = pDC->SetBkMode(TRANSPARENT);
    COLORREF clrOldText = pDC->SetTextColor (clrCptnText);

    CFont* pOldFont = pDC->SelectObject (&globalData.fontRegular);
	ASSERT (pOldFont != NULL);

    CString strTitle;
    GetWindowText (strTitle);

	rectCaption.right = rcbtnRight.left;
	rectCaption.left = rcbtnLeft.right;
	rectCaption.top++;
	rectCaption.DeflateRect (g_nCaptionHorzMargin * 2, 0);

	pDC->DrawText (strTitle, rectCaption, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);

    pDC->SelectObject(pOldFont);
    pDC->SetBkMode(nOldBkMode);
    pDC->SetTextColor(clrOldText);
}