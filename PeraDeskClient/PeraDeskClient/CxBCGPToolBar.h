#pragma once
#include <BCGPToolBar.h>

// CCxBCGPToolBar

class CCxBCGPToolBar : public CBCGPToolBar
{
	DECLARE_SERIAL(CCxBCGPToolBar)

public:
	CCxBCGPToolBar();
	virtual ~CCxBCGPToolBar();

public:
// 	enum {
// 		IMG_TOOLBAR_MESSAGE,
// 		IMG_TOOLBAR_FLEXWARE,
// 		IMG_RUN_PROCESS,
// 		//------------
// 		IMG_PREVIEW,
// 		IMG_TOOLBAR_PORTAL,
// 		IMG_DATALINKER,
// 		IMG_HISTORY_MONITOR,
// 		//---------------
// 		IMG_HELP,
// 		IMG_ABOUT
// 	};

	enum {
		IMG_TOOLBAR_LOGOUT,
		IMG_TOOLBAR_REFRESH,
		IMG_TOOLBAR_MESSAGE,
		IMG_TOOLBAR_PORTAL,
		IMG_TOOLBAR_FLEXWARE,
		IMG_TOOLBAR_SETTING,
		IMG_TOOLBAR_ABOUT

		//IMG_SAVEAS,
		//IMG_INFO,
		//IMG_PUBLISH,
		//IMG_DATALINKER,
		////------------
		//IMG_RUN_PROCESS,
		////------------
		//IMG_HISTORY_MONITOR,
		//IMG_PREVIEW,
		////---------------
		//IMG_HELP,
		//IMG_ABOUT
	};


	void LoadButton( int nFirstCmd, int nFirstImg, int nCount );

	void LoadData();
	void ClearData();

	CBCGPToolBarImages m_BCGToolbarImages;
	vector< Bitmap * > m_vImages;
	vector< HBITMAP > m_vHBitmaps;

	void AdjustLocations ();
	virtual CSize CalcSize (BOOL bVertDock);

	virtual BOOL CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle = TBSTYLE_FLAT,
		DWORD dwStyle = dwDefaultToolbarStyle,
		CRect rcBorders = CRect(1, 1, 1, 1),
		UINT nID = AFX_IDW_TOOLBAR);

	BOOL m_bUserOP; //是否用户操作
	BOOL IsUserOP() const { return m_bUserOP; }
	void SetUserOP(BOOL val) { m_bUserOP = val; }
	//
	void MoveToRight();

	virtual BOOL CanFloat () const ;
	BOOL m_bLBDownForFloat;

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

protected:
	virtual void DoPaint(CDC* pDC);
	virtual BOOL OnUserToolTip (CBCGPToolbarButton* pButton, CString& strTTText) const;

	virtual void OnAfterDock (CBCGPBaseControlBar* pBar, LPCRECT lpRect, BCGP_DOCK_METHOD dockMethod);
	virtual BCGP_CS_STATUS IsChangeState (int nOffset, CBCGPBaseControlBar** ppTargetBar) const;
protected:
	DECLARE_MESSAGE_MAP()
};


