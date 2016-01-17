#pragma once
#include "ResLibImageSet.h"
#include <BCGProMigrate.h>


// CZButtonDraw

class CZButtonDraw : public CBCGButton
{
	DECLARE_DYNAMIC(CZButtonDraw)

public:
	CZButtonDraw();
	virtual ~CZButtonDraw();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	void SetImage( CCxStaticRes * pImg);
	void DrawImg(CCxStaticRes* pImage);
	virtual void DrawImg(CCxStaticRes* pImage, CDC* pDC);
	void DrawImg(CDC* pDC);
	CCxStaticRes* m_pImg; //显示
	CCxStaticRes* m_pImgDisabled; //禁用显示
	int m_nState; 

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	void SetImageDisabled(CCxStaticRes * pImg);
	BOOL EnableWindow(BOOL bEnable  = TRUE );


protected:
	int          m_Style;     //按钮形状（0-正常，1-当前，2-按下，3-锁定）
	BOOL         b_InRect;            //鼠标进入标志
	CString      m_strText;           //按钮文字
	COLORREF     m_ForeColor;         //文本颜色
	COLORREF     m_BackColor;         //背景色

	COLORREF     m_LockForeColor;     //锁定按钮的文字颜色
	CRect        m_ButRect;           //按钮尺寸
	CFont*       p_Font;              //字体


	virtual void PreSubclassWindow();
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual void DrawButton(CDC *pDC);
	void SetText(CString str);
	void SetForeColor(COLORREF color);
	void SetBkColor(COLORREF color);
	void SetTextFont(int FontHight,LPCTSTR FontName);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	COLORREF BackColor() const { return m_BackColor; }
	void BackColor(COLORREF val) { m_BackColor = val; }
};


