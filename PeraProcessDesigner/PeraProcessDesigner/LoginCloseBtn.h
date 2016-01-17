#pragma once

// CLoginCloseBtn

class CLoginCloseBtn : public CButton
{
	DECLARE_DYNAMIC(CLoginCloseBtn)

public:
	CLoginCloseBtn();
	virtual ~CLoginCloseBtn();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam,LPARAM lParam);       
	afx_msg LRESULT OnMouseHover(WPARAM wParam,LPARAM lParam);  
	BOOL m_bIn;
};


