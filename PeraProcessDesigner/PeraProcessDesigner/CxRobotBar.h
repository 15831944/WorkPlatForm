#pragma once
#include <BCGPToolBar.h>
#include <ResLibData.h>

// CCxBCGPToolBar

class CCxRobotBar : public CBCGPToolBar
{
	DECLARE_SERIAL(CCxRobotBar)

public:
	CCxRobotBar();
	virtual ~CCxRobotBar();

public:

	void LoadButton( int nFirstCmd, int nFirstImg, int nCount );

	void LoadData();
	void ClearData();

	virtual BOOL DrawButton (CDC* pDC, CBCGPToolbarButton* pButton, 
		CBCGPToolBarImages* pImages, BOOL bHighlighted,
		BOOL bDrawDisabledImages);

	CBCGPToolBarImages m_BCGToolbarImages;
	vector< Bitmap * > m_vImages;
	vector< HBITMAP > m_vHBitmaps;

	//list< CResLibData* > m_BaseComponentList;

	vector< CResLibData * > m_vResLibData;

	BOOL m_bLButtonDown;

	void UpdateResData( list< CResLibData* >& BaseComponentList );

	void ClearButtonStatus();

protected:
	virtual void DoPaint(CDC* pDC);
	//virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	virtual BOOL OnUserToolTip (CBCGPToolbarButton* pButton, CString& strTTText) const;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	void DragLayer( int nPos );
	void AdjustLocations ();
	virtual CSize CalcSize (BOOL bVertDock);
	BOOL m_bDragMe;
	//���϶�Toolbarʱ���������ƶ��ٶȹ��죬��mousemove�¼��У�
	//�����Robotͼ�걻�϶��Ĵ����жϣ��˱�־���������Ƿ�Ϊͼ�걻�϶���
	//
	virtual BOOL CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle = TBSTYLE_FLAT,
		DWORD dwStyle = dwDefaultToolbarStyle,
		CRect rcBorders = CRect(1, 1, 1, 1),
		UINT nID = AFX_IDW_TOOLBAR);
};


