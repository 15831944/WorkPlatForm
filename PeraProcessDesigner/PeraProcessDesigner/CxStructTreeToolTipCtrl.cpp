#include "StdAfx.h"
#include "CxStructTreeToolTipCtrl.h"
#include <CxBCGPVisualManager2007.h>

#ifndef _BCGSUITE_
//#include "CxResLibImageToolTipCtrl.h"
#define visualManagerMFC	CCxBCGPVisualManager2007::GetInstance ()
#else
#define visualManagerMFC	CCxBCGPVisualManager2007::GetInstance ()
#endif

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif

CString CCxStructTreeToolTipCtrl::m_strClassName;

BEGIN_MESSAGE_MAP(CCxStructTreeToolTipCtrl, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_MESSAGE(WM_SETFONT, OnSetFont)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CCxStructTreeToolTipCtrl, CWnd)

CCxStructTreeToolTipCtrl::CCxStructTreeToolTipCtrl(void)
{
	m_rectLast.SetRectEmpty ();
	m_nTextMargin = 10;
	m_pFont	= NULL;
	m_pWndParent = NULL;
}


CCxStructTreeToolTipCtrl::~CCxStructTreeToolTipCtrl(void)
{
}


BOOL CCxStructTreeToolTipCtrl::Create (CWnd* pWndParent) 
{
	ASSERT_VALID (pWndParent);
	m_pWndParent = pWndParent;

	if (m_strClassName.IsEmpty ())
	{
		m_strClassName = ::AfxRegisterWndClass (
			CS_SAVEBITS,
			::LoadCursor(NULL, IDC_ARROW),
			(HBRUSH)(COLOR_BTNFACE + 1));
	}  

	return CreateEx (0,
					m_strClassName, _T (""), WS_POPUP,
					0, 0, 0, 0,
					pWndParent->GetSafeHwnd (), (HMENU) NULL);
}

BOOL CCxStructTreeToolTipCtrl::OnEraseBkgnd(CDC* /*pDC*/) 
{
	return TRUE;
}

void CCxStructTreeToolTipCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rect;
	GetClientRect (rect);

	CBCGPToolTipParams params;
	visualManagerMFC->GetToolTipParams (params);

	if (params.m_clrFill == (COLORREF)-1)
	{
		::FillRect (dc.GetSafeHdc (), rect, ::GetSysColorBrush (COLOR_INFOBK));
	}
	else
	{
		if (params.m_clrFillGradient == (COLORREF)-1)
		{
			CBrush br (params.m_clrFill);
			dc.FillRect (rect, &br);
		}
		else
		{
			CBCGPDrawManager dm (dc);

			dm.FillGradient2 (rect, 
				params.m_clrFillGradient, params.m_clrFill,
				params.m_nGradientAngle == -1 ? 90 : params.m_nGradientAngle);
		}
	}

	COLORREF clrLine = params.m_clrBorder == (COLORREF)-1 ?
		::GetSysColor (COLOR_INFOTEXT) : params.m_clrBorder;

	COLORREF clrText = params.m_clrText == (COLORREF)-1 ?
		::GetSysColor (COLOR_INFOTEXT) : params.m_clrText;

	dc.Draw3dRect (rect, clrLine, clrLine);

	CFont* pPrevFont = m_pFont == NULL ?
		(CFont*) dc.SelectStockObject (DEFAULT_GUI_FONT) :
		dc.SelectObject (m_pFont);

	dc.SetBkMode (TRANSPARENT);
	dc.SetTextColor(clrText);

	if (m_strText.FindOneOf (_T("\n")) != -1)	// multi-line tooltip
	{
		rect.DeflateRect (m_nTextMargin, m_nTextMargin);
		if (rect.Height () < m_rectLast.Height ())
		{
			// center tooltip vertically
			rect.top += (m_rectLast.Height () - rect.Height ()) / 2;
		}

		//dc.DrawText (m_strText, rect, DT_LEFT | DT_WORDBREAK);
		DrawTextML(dc, m_strText, rect);
	}
	else // single line tooltip
	{
		rect.DeflateRect (m_nTextMargin, 0);
		dc.DrawText (m_strText, rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
	}

	if (pPrevFont != NULL)
	{
		dc.SelectObject (pPrevFont);
	}
}

CString * CCxStructTreeToolTipCtrl::SplitString(CString str, char split, int& iSubStrs)
{
	int iPos = 0; //分割符位置
	int iNums = 0; //分割符的总数
	CString strTemp = str;
	CString strRight;
	//先计算子字符串的数量
	while (iPos != -1)
	{
		iPos = strTemp.Find(split);
		if (iPos == -1)
		{
			break;
		}
		strRight = strTemp.Mid(iPos + 1, str.GetLength());
		strTemp = strRight;
		iNums++;
	}
	if (iNums == 0) //没有找到分割符
	{
		//子字符串数就是字符串本身
		iSubStrs = 1; 
		return NULL;
	}
	//子字符串数组
	iSubStrs = iNums + 1; //子串的数量 = 分割符数量 + 1
	CString* pStrSplit;
	pStrSplit = new CString[iSubStrs];
	strTemp = str;
	CString strLeft;
	for (int i = 0; i < iNums; i++)
	{
		iPos = strTemp.Find(split);
		//左子串
		strLeft = strTemp.Left(iPos);
		//右子串
		strRight = strTemp.Mid(iPos + 1, strTemp.GetLength());
		strTemp = strRight;
		pStrSplit[i] = strLeft;
	}
	pStrSplit[iNums] = strTemp;
	return pStrSplit;
}


//绘制多行文本
void CCxStructTreeToolTipCtrl::DrawTextML(CDC& dc, CString& str, CRect& rect)
{
	CRect rectDraw;
	CString strLine;

	TEXTMETRIC tm;
	dc.GetTextMetrics (&tm);
	ASSERT(tm.tmHeight >= 0);

	rectDraw = rect;
	rectDraw.bottom = rectDraw.top + tm.tmHeight;

	CString* pStr;
	int iSubStrs;
	pStr = SplitString(str, '\n', iSubStrs);
	//如果子字符串的数量为1
	if (iSubStrs == 1)
	{
		//Convert CString to char
		char* pCh = (LPSTR)(LPCTSTR)str;
		dc.DrawText(pCh, rectDraw, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);	
	}
	else
	{	
		//输出所有子字符串
		for (int i = 0; i < iSubStrs; i++)
		{
			//Convert CString to char
			char* pCh = (LPSTR)(LPCTSTR)pStr[i];
			dc.DrawText(pCh, rectDraw, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
			rectDraw.OffsetRect(0, tm.tmHeight + m_nTextMargin);
		}
		delete []pStr;
	}

}

//获取文本的行数
int CCxStructTreeToolTipCtrl::GetLineCount(CString str)
{
	return count_if( (LPCTSTR)str, (LPCTSTR)str+str.GetLength(), bind2nd(equal_to<TCHAR>(),_T('\n')) ) + 1 ;
}

//*******************************************************************************************
void CCxStructTreeToolTipCtrl::Track (CRect rect, const CString& strText)
{
	if (GetSafeHwnd () == NULL)
	{
		return;
	}

	if (m_rectLast == rect && m_strText == strText)
	{
		//static int  a= 0;
		//if (a++>1000)
		//{
		//	Sleep(0);
		//}
		return;
	}

	ASSERT_VALID (m_pWndParent);

	m_rectLast = rect;
	m_strText = strText;

	CClientDC dc (this);

    //ASSERT_VALID(m_pFont);
	CFont* pPrevFont = m_pFont == NULL ?
		(CFont*) dc.SelectStockObject (DEFAULT_GUI_FONT) :
		dc.SelectObject (m_pFont);
	ASSERT (pPrevFont != NULL);

	int nTextHeight = rect.Height ();
	int nTextWidth = rect.Width ();
	if (m_strText.FindOneOf (_T("\n")) != -1)	// multi-line tooltip
	{
		const int nDefaultHeight = globalData.GetTextHeight ();
		const int nDefaultWidth = 200;
		CRect rectText (0, 0, nDefaultWidth, nDefaultHeight);
 
		nTextHeight = dc.DrawText (m_strText, rectText, DT_LEFT | DT_CALCRECT | DT_NOPREFIX );
		nTextWidth = rectText.Width ();
		nTextHeight += 2 * m_nTextMargin;
		nTextWidth += 2 * m_nTextMargin;
		nTextHeight += (GetLineCount(m_strText) - 1) * m_nTextMargin; //加上行间距
	}
	else
	{
		nTextWidth = dc.GetTextExtent (m_strText).cx + 2 * m_nTextMargin;
	}

	dc.SelectObject (pPrevFont);

	if (m_pWndParent->GetExStyle () & WS_EX_LAYOUTRTL)
	{
		rect.left = rect.right - nTextWidth;
	}
	else
	{
		rect.right = rect.left + nTextWidth;
	}
	rect.bottom = rect.top + nTextHeight;
	if (rect.Height () < m_rectLast.Height ())
	{
		rect.top = m_rectLast.top;
		rect.bottom = m_rectLast.bottom;
	}

	MONITORINFO mi;
	mi.cbSize = sizeof (MONITORINFO);

	CRect rectScreen;

	//if (GetMonitorInfo (MonitorFromPoint (rect.TopLeft (), MONITOR_DEFAULTTONEAREST), &mi))
	//{
	//	rectScreen = mi.rcWork;
	//}
	//else
	//{
		::SystemParametersInfo (SPI_GETWORKAREA, 0, &rectScreen, 0);
	//}

	if (rect.Width () > rectScreen.Width ())
	{
		rect.left = rectScreen.left;
		rect.right = rectScreen.right;
	}
	else if (rect.right > rectScreen.right)
	{
		rect.right = rectScreen.right;
		rect.left = rect.right - nTextWidth;
	}
	else if (rect.left < rectScreen.left)
	{
		rect.left = rectScreen.left;
		rect.right = rect.left + nTextWidth;
	}

	if (rect.Height () > rectScreen.Height ())
	{
		rect.top = rectScreen.top;
		rect.bottom = rectScreen.bottom;
	}
	else if (rect.bottom > rectScreen.bottom)
	{
		rect.bottom = rectScreen.bottom;
		rect.top = rect.bottom - nTextHeight;
	}
	else if (rect.top < rectScreen.top)
	{
		rect.top = rectScreen.top;
		rect.bottom = rect.bottom + nTextHeight;
	}

	SetWindowPos (&wndTop, rect.left, rect.top, 
		rect.Width (), rect.Height (), SWP_NOACTIVATE | SWP_NOOWNERZORDER);
  
    ShowWindow (SW_SHOWNOACTIVATE);
	Invalidate ();
	UpdateWindow ();

	SetCursor (AfxGetApp ()->LoadStandardCursor (IDC_ARROW));
}
//*******************************************************************************************
void CCxStructTreeToolTipCtrl::Hide ()
{
	if (GetSafeHwnd () != NULL)
	{
		ShowWindow (SW_HIDE);
	}
}
//*******************************************************************************************
void CCxStructTreeToolTipCtrl::Deactivate ()
{
	m_strText.Empty ();
	m_rectLast.SetRectEmpty ();

	Hide ();
}
//*****************************************************************************
LRESULT CCxStructTreeToolTipCtrl::OnSetFont (WPARAM wParam, LPARAM lParam)
{
	BOOL bRedraw = (BOOL) LOWORD (lParam);

    m_pFont = CFont::FromHandle((HFONT) wParam);

	if (bRedraw)
	{
		Invalidate ();
		UpdateWindow ();
	}

	return 0;
}
//***************************************************************************
BOOL CCxStructTreeToolTipCtrl::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message >= WM_MOUSEFIRST &&
		pMsg->message <= WM_MOUSELAST)
	{
		if (pMsg->message != WM_MOUSEMOVE)
		{
			Deactivate ();
		}

		ASSERT_VALID (m_pWndParent);

		// the parent should receive the mouse message in its client coordinates
		CPoint pt(BCG_GET_X_LPARAM(pMsg->lParam), BCG_GET_Y_LPARAM(pMsg->lParam));
		MapWindowPoints (m_pWndParent, &pt, 1);
		LPARAM lParam = MAKELPARAM (pt.x, pt.y);

		m_pWndParent->SendMessage (pMsg->message, pMsg->wParam, lParam);
		return TRUE;
	}
	
	return CWnd::PreTranslateMessage(pMsg);
}
