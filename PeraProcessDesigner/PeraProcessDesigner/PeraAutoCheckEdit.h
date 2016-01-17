#pragma once

#include "CxResLibImageToolTipCtrl.h"

typedef CCxResLibImageToolTipCtrl CEditToolTipCtrl;
// CPeraAutoCheckEdit

class CPeraAutoCheckEdit : public CEdit
{
	DECLARE_DYNAMIC(CPeraAutoCheckEdit)

public:
	CPeraAutoCheckEdit();
	virtual ~CPeraAutoCheckEdit();
	void    SetMsgProWnd(HWND);
	void    SetErrBkColor(COLORREF cl) { m_ErrBkColor =cl;}
	void    ChangeBkColor(bool bErr); // 不是键盘输入的情况，强制设置为错误标示
	void    ShowBalloonTip(LPCWSTR strTitile,LPCWSTR strtext,UINT uStyle);
	void    HideBalloonTip();
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseHover(WPARAM wp,LPARAM lp);
	afx_msg LRESULT OnMouseLeaved(WPARAM wp,LPARAM lp);

	CEditToolTipCtrl m_ToolTip;
	BOOL m_bMouseTracked;
private:
	HWND m_WndPro;
	COLORREF m_ErrBkColor;
	bool     m_bErr;
	HBRUSH   m_pOldBrush;
public:
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	virtual void PreSubclassWindow();
};


