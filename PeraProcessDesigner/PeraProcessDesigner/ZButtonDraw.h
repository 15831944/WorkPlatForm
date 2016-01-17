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
	CCxStaticRes* m_pImg; //��ʾ
	CCxStaticRes* m_pImgDisabled; //������ʾ
	int m_nState; 

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	void SetImageDisabled(CCxStaticRes * pImg);
	BOOL EnableWindow(BOOL bEnable  = TRUE );


protected:
	int          m_Style;     //��ť��״��0-������1-��ǰ��2-���£�3-������
	BOOL         b_InRect;            //�������־
	CString      m_strText;           //��ť����
	COLORREF     m_ForeColor;         //�ı���ɫ
	COLORREF     m_BackColor;         //����ɫ

	COLORREF     m_LockForeColor;     //������ť��������ɫ
	CRect        m_ButRect;           //��ť�ߴ�
	CFont*       p_Font;              //����


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


