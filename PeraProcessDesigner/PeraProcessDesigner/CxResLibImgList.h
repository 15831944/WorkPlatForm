#pragma once
#include "CxListCtrl.h"
#include "CxResLibImageToolTipCtrl.h"

// CxResLibImgList

class CxResLibImgList : public CxListCtrl
{
	DECLARE_DYNAMIC(CxResLibImgList)

public:
	CxResLibImgList();
	virtual ~CxResLibImgList();
	BOOL m_bSetCapture;
	//ListCtrl默认背景色
	static const Color COLOR_LISTCTRL_BG;
	//默认图片背景色
	static const Color COLOR_IMAGE_BG;
	//Y轴方向，图片最小间距
	static const int MIN_CY_SPACE_ITEM;

	//清除所有数据，调用之后，可以重新初始化。
	virtual void Release(void);
	//清除图片数据，不删除初始化的数据
	virtual void Clear(void);
	
	//在初始化前、初始化期间调用。初始化后，不允许修改
	BOOL SetAutoDelete(BOOL bEnabled = TRUE)
	{
		if (IsInitialed() && bEnabled != m_bAutoDeleteData) return FALSE;
		m_bAutoDeleteData = bEnabled; return TRUE;
	}
	//是否自动删除图片数据
	//如果自动删除，需要调用AddImage(lpszPath ...)，指定图片路径，自动加载图片。
	//否则，在外部加载好图片后，调用AddImage(Bitmap * ...)。
	BOOL IsAutoDeleteData( void ) const { return m_bAutoDeleteData; }

	//初始化图片尺寸、间距、底色数据
	BOOL InitImageLayout(int nItemCx, int nItemCy, int nSpaceX, int nSpaceY
		, int nImgCx, int nImgCy, Color crImgBk = COLOR_IMAGE_BG);
	
	//添加图片
	BOOL AddImage(LPCTSTR lpszImgPath, LPCTSTR lpszLabel = "", LPARAM lp = NULL, int nPos = -1);
	BOOL AddImage(Bitmap * pBitmap, LPCTSTR lpszLabel = "", LPARAM lp = NULL, int nPos = -1);
	//删除图片
	BOOL DeleteIamge(int nItemIndex);

protected:
	virtual BOOL OnInit(void);
	//加载状态图片。用于绘制焦点状态和选中状态。
	virtual void OnLoadStateImage(void);
	//加载字体数据
	virtual void OnLoadTextFont();
	//取得图片与文字的间距
	virtual int GetSpaceOfIconLabel(void);
	//背景更新时
	virtual void OnEraseBkgndCustom(void);
	//焦点改变时
	virtual void OnFocusChanged(int nItem, BOOL bFocus);
	//绘制图片
	virtual BOOL OnItemPaint(LPNMLVCUSTOMDRAW lplvcd);
	//绘制图片文字
	virtual void DrawItemText(int nItem, RECT& rcLabel);
	//
	int _AddImage( Bitmap * pBitmap, LPCTSTR lpszLabel = "", LPARAM lp = NULL, int nPos = -1 );

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseHover(WPARAM wp,LPARAM lp);
	afx_msg LRESULT OnMouseLeaved(WPARAM wp,LPARAM lp);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

	void TestItem( vector<int>& vSels, int& nCurSel );

	CCxResLibImageToolTipCtrl m_ToolTip;
	CMenu m_RMenu;
	CEdit m_wndDummy;

	
protected:
	//是否自动删除数据。
	//如果是，则当 Clear、Release 被调用时，删除 m_vImg 中的数据。
	//否则，不删除
	BOOL m_bAutoDeleteData;
	vector< Image * > m_vImg; //图片对象

	//自绘对象
	Graphics * m_pGraphics;
	Bitmap * m_pBmpCanvas; //自绘期间，在这个Bitmap对象上画（双缓冲）
	Image * m_pImgFocused; //获得焦点时对应的图片
	Image * m_pImgSelected; //选中状态的图片
	
	//ListCtrl背景色
	Color m_crCanvasBk;

	CString m_strFontFamilyName; //字体名称
	Gdiplus::Font * m_pFont; //字体对象
	StringFormat m_FontFormat; //字体对齐控制
	Color m_FontColor; //字体颜色

	//以下为图片尺寸、间距等变量，详细操作参见 InitImageLayout 函数实现。
	//图片尺寸。当原始图片与设置尺寸不符时，拉伸显示。
	int m_nImgCx;
	int m_nImgCy;
	//
	int m_nItemCx;
	int m_nItemCy;
	//图片间距。
	int m_nItemCxSpace;
	int m_nItemCySpace;
	//主要用于 ListCtrl 测算图片区域。只给了初始值，并没有添加实际图片
	CImageList m_ImageList;
	//图片背景色（透明色？）
	Color m_crImgBk;

	BOOL m_bDragLayer;

	BOOL m_bMouseTracked;
private:
	//表示上次获得焦点的 Item。
	//该值只允许在 OnMouseMove 中被修改。
	int m_nLastFocusItem;

public:

	void DragLayer();
	
	afx_msg void OnCxResLibImgListDelete();
	afx_msg void OnUpdateProp();
	afx_msg void OnDownload();
};


