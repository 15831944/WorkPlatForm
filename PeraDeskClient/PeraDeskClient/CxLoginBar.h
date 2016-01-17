#pragma once
#include "CxLoginData.h"
#include "LoginBarToolTipCtrl.h"



//////////////////////////////////////////////////////////////////////////
// CCxLoginBar

class CCxLoginBar : public CBCGPToolBar
{
	friend class CCxLoginBarUserButton;
	friend class CCxLoginBarLoginButton;
	DECLARE_DYNCREATE(CCxLoginBar)

public:
	CCxLoginBar();
	virtual ~CCxLoginBar();

	enum
	{
		IMG_USER_BG = 0,
		IMG_LOGIN,
		IMG_LOGIN_PRESSED,
		IMG_LOGIN_FOCUSED,
		IMG_LOGOUT,
		IMG_LOGOUT_PRESSED,
		IMG_LOGOUT_FOCUSED
	};

	BOOL Create(CWnd* pParentWnd,
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP,
		UINT nID = AFX_IDW_TOOLBAR);
	BOOL CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle = 0,
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP,
		UINT nID = AFX_IDW_TOOLBAR);

	//void OnDrawLoginButton (CDC* pDC, CBCGPToolbarButton* pButton, CRect rect, BOOL bHighlight);
	//void OnDrawButtonsFrame (CDC* pDC);

	CSize GetLoginImageSize () const
	{
		return m_bLargeIcons ? m_BCGToolbarImages.GetImageSize () : m_BCGToolbarImages.GetImageSize ();
	}

	void OnLoadImages ();
	void ClearImages();

	virtual int CalcMaxButtonHeight ();


protected:
	virtual void AdjustLocations ();
	virtual void  OnFillBackground (CDC* /*pDC*/);
	

public:
	//CRect m_rcUser;
	//CRect m_rcLogin;

	CBCGPToolBarImages m_BCGToolbarImages;
	vector< Bitmap * > m_vImages;
	vector< HBITMAP > m_vHBitmaps;

	int m_nUserID;
	int m_nLoginID;

//	void MoveToRight();

	//virtual BCGP_CS_STATUS IsChangeState (int nOffset, CBCGPBaseControlBar** ppTargetBar) const;

	//BOOL m_bUserOP; //是否用户操作
	//BOOL IsUserOP() const { return m_bUserOP; }
	//void SetUserOP(BOOL val) { m_bUserOP = val; }

protected:
	DECLARE_MESSAGE_MAP()

	virtual BOOL CanFloat () const ;
	BOOL m_bLBDownForFloat;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg LRESULT OnMouseHover(WPARAM wp,LPARAM lp);
	afx_msg LRESULT OnMouseLeaved(WPARAM wp,LPARAM lp);
	
	BOOL m_bLoginClicked; //登陆bar上面点击登陆按钮后设置为TRUE
	CLoginBarToolTipCtrl	m_ToolTip; 
	BOOL m_bMouseTracked;
	

protected:
	virtual void DoPaint(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//virtual void OnAfterDock (CBCGPBaseControlBar* pBar, LPCRECT lpRect, BCGP_DOCK_METHOD dockMethod);

public:
	//void LoadButton( int nFirstCmd, int nFirstImg, int nCount );
	/*virtual BOOL OnUserToolTip (CBCGPToolbarButton* pButton, CString& strTTText) const;*/

	BOOL OnClicked( void );
};


