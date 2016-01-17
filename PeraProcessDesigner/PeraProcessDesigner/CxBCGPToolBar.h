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
// 		IMG_NEW_PROCESS,
// 		IMG_OPEN_PROCESS,
// 		IMG_RUN_PROCESS,
// 		//------------
// 		IMG_PREVIEW,
// 		IMG_SAVE,
// 		IMG_DATALINKER,
// 		IMG_JOBMGR,
// 		//---------------
// 		IMG_HELP,
// 		IMG_ABOUT
// 	};

	enum {
		IMG_NEW_PROCESS,
		IMG_OPEN_PROCESS,
		IMG_SAVE,
		IMG_SAVEAS,
		IMG_INFO,
		IMG_PUBLISH,
		IMG_DATALINKER,
		//------------
		IMG_RUN_PROCESS,
		//------------
		IMG_JOBMGR,
		IMG_PREVIEW,
		//---------------
		IMG_HELP,
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

protected:
	virtual void DoPaint(CDC* pDC);
	virtual BOOL OnUserToolTip (CBCGPToolbarButton* pButton, CString& strTTText) const;
protected:
	DECLARE_MESSAGE_MAP()
};


