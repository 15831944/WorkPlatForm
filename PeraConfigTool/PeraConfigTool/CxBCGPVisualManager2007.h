#pragma once
// CCxBCGPVisualManager2007 命令目标

class CCxBCGPVisualManager2007 : public CBCGPVisualManager2007
{
	DECLARE_DYNCREATE(CCxBCGPVisualManager2007)
public:
	CCxBCGPVisualManager2007();
	virtual ~CCxBCGPVisualManager2007();

	virtual void OnDrawSplitterBorder (CDC* pDC, CBCGPSplitterWnd* pSplitterWnd, CRect rect);
	virtual void OnDrawSplitterBox (CDC* pDC, CBCGPSplitterWnd* pSplitterWnd, CRect& rect);
	virtual void OnFillSplitterBackground (CDC* pDC, CBCGPSplitterWnd* pSplitterWnd, CRect rect);

	virtual void DrawNcCaption (CDC* pDC, CRect rectCaption, 
		DWORD dwStyle, DWORD dwStyleEx,
		const CString& strTitle, const CString& strDocument,
		HICON hIcon, BOOL bPrefix, BOOL bActive, BOOL bTextCenter,
		const CObList& lstSysButtons);

	virtual BOOL OnNcPaint (CWnd* pWnd, const CObList& lstSysButtons, CRect rectRedraw);
	
	virtual void DrawNcText (CDC* pDC, CRect& rect, const CString& strTitle, 
		const CString& strDocument, BOOL bPrefix, 
		BOOL bActive, BOOL bIsRTL, BOOL bTextCenter,
		BOOL bGlass = FALSE, int nGlassGlowSize = 0, 
		COLORREF clrGlassText = (COLORREF)-1);

	virtual void OnFillBarBackground (CDC* pDC, CBCGPBaseControlBar* pBar,
		CRect rectClient, CRect rectClip,
		BOOL bNCArea = FALSE);
	virtual void OnDrawBarBorder (CDC* pDC, CBCGPBaseControlBar* pBar, CRect& rect);

	virtual void OnDrawBarGripper (CDC* pDC, CRect rectGripper, BOOL bHorz, CBCGPBaseControlBar* pBar);
	//绘制FloatBar标题栏
	virtual COLORREF OnDrawControlBarCaption (CDC* pDC, CBCGPDockingControlBar* pBar, 
		BOOL bActive, CRect rectCaption, CRect rectButtons);

	virtual BOOL OnDrawGridHeaderItemBorder (CBCGPGridCtrl* pCtrl, CDC* pDC, CRect rect, BOOL bPressed);

	//virtual void OnDrawCaptionBarInfoArea (CDC* pDC, CBCGPCaptionBar* pBar, CRect rect);

	//virtual void OnDrawMiniFrameBorder (CDC* pDC, CBCGPMiniFrameWnd* pFrameWnd,
	//	CRect rectBorder, CRect rectBorderSize);

	//virtual void OnDrawFloatingToolbarBorder (	CDC* pDC, CBCGPBaseToolBar* pToolBar, 
	//	CRect rectBorder, CRect rectBorderSize);

	//停靠bar标题栏扩展高度（实际高度 = gripper + 这个返回值）
	virtual int GetDockingBarCaptionExtraHeight () const
	{
		return 6;
	}

	virtual COLORREF OnFillGridGroupByBoxBackground (CDC* pDC, CRect rect);	
	virtual COLORREF OnFillGridGroupByBoxTitleBackground (CDC* pDC, CRect rect);
	virtual void OnDrawGridGroupByBoxItemBorder (CBCGPGridCtrl* pCtrl, CDC* pDC, CRect rect);
	virtual COLORREF GetGridLeftOffsetColor (CBCGPGridCtrl* pCtrl);

	virtual COLORREF OnFillGridItem (CBCGPGridCtrl* pCtrl, CDC* pDC, CRect rectFill,
		BOOL bSelected, BOOL bActiveItem, BOOL bSortedColumn);

	virtual BOOL ParseStyleXMLMainWnd (const CString& strItem);
public:
	CSize      m_szNcCloseBtnSize;

	virtual void OnDrawTab (CDC* pDC, CRect rectTab,
		int iTab, BOOL bIsActive, const CBCGPBaseTabWnd* pTabWnd);

	virtual void OnEraseTabsArea (CDC* pDC, CRect rect, const CBCGPBaseTabWnd* pTabWnd);

	virtual void OnEraseTabsButton (CDC* pDC, CRect rect, CBCGPButton* pButton,
		CBCGPBaseTabWnd* pWndTab);

	virtual BOOL OnEraseTabsFrame (CDC* pDC, CRect rect, const CBCGPBaseTabWnd* pTabWnd);

	virtual void OnFillTab (CDC* pDC, CRect rectFill, CBrush* pbrFill, int iTab, BOOL bIsActive, const CBCGPBaseTabWnd* pTabWnd);

	virtual void OnDrawTabsButtonBorder (CDC* pDC, CRect& rect, 
		CBCGPButton* pButton, UINT uiState, CBCGPBaseTabWnd* pWndTab);

	virtual void OnDrawTabContent (CDC* pDC, CRect rectTab,
		int iTab, BOOL bIsActive, const CBCGPBaseTabWnd* pTabWnd,
		COLORREF clrText);

	virtual int GetTabHorzMargin (const CBCGPBaseTabWnd* pTabWnd);

	virtual void OnDrawTabBorder(CDC* pDC, CBCGPTabWnd* pTabWnd, CRect rectBorder, COLORREF clrBorder, CPen& penLine);

	virtual BOOL IsMDITabsTopEdge ()
	{
		return TRUE;
	}

	virtual int GetMDITabsBordersSize ()
	{
		return 1;
	}

	//第一个tab页签左边的余白区域
	virtual int GetTabsMargin (const CBCGPTabWnd* pTabWnd)
	{
		return 4;
	}

	virtual int GetTabExtraHeight(const CBCGPTabWnd* /*pTab*/)
	{
		return 11;
	}

	virtual void OnDrawTabCloseButton (CDC* pDC, CRect rect, 
											   const CBCGPBaseTabWnd* pTabWnd, 
											   BOOL bIsHighlighted,
											   BOOL bIsPressed,
											   BOOL bIsDisabled);

	virtual BOOL OnDrawGridSelectAllAreaBorder (CBCGPGridCtrl* pCtrl, CDC* pDC, CRect rect, BOOL bPressed);

	virtual BOOL OnUpdateNcButtons( CWnd* pWnd, const CObList& lstSysButtons, CRect rectCaption );

	virtual void GetTabOffectInTabsArea_LocationTop( int & x, int & y )
	{
		x = 0;
		y = 1;
	}
	//取得每个Tab页签矩形的水平间距
	virtual int GetTabSpaceInTabsArea( void )
	{
		return 0; //ori: 1
	}

};


