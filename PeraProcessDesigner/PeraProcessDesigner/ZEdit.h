#pragma once
#include "CxResLibImageToolTipCtrl.h"

typedef CCxResLibImageToolTipCtrl CEditToolTipCtrl;
// CZEdit

class CZEdit : public CBCGPEdit
{
	DECLARE_DYNAMIC(CZEdit)

public:
	CZEdit();
	virtual ~CZEdit();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnEnKillfocus();
	// 光标位置
	int m_nPos;
	// 光标位置结束
	int m_nPosEnd;
	// 文本
	CString m_strText;

	int m_index; //索引

	static int m_staCount;

	afx_msg void OnEnSetfocus();
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	afx_msg void OnPaint();
	afx_msg void OnNcPaint();
	void SetBkGrndColor();

private:
	CString m_Text;
	COLORREF m_BackColor;
	CBrush m_Brush;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	void CenterTextV();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseHover(WPARAM wp,LPARAM lp);
	afx_msg LRESULT OnMouseLeaved(WPARAM wp,LPARAM lp);

	CEditToolTipCtrl m_ToolTip;
	BOOL m_bMouseTracked;
};


