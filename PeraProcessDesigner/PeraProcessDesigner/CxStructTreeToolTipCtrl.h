#pragma once
class CCxStructTreeToolTipCtrl : public CWnd
{
	friend class CCxStructTreeGrid;
	friend class CCxStructTreeGridT;
	DECLARE_DYNAMIC(CCxStructTreeToolTipCtrl)
public:
	CCxStructTreeToolTipCtrl(void);
	virtual ~CCxStructTreeToolTipCtrl(void);

	void SetTextMargin (int nTextMargin)
	{
		m_nTextMargin = nTextMargin;
	}

	void GetLastRect (CRect& rect) const
	{
		rect = m_rectLast;
	}

protected:
	static CString	m_strClassName;
	CString			m_strText;
	CRect			m_rectLast;
	int				m_nTextMargin;
	CFont*			m_pFont;
	CWnd*			m_pWndParent;

public:
	void Track (CRect rect, const CString& strText);
	void Hide ();
	void Deactivate ();

public:
	virtual BOOL Create(CWnd* pWndParent = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg LRESULT OnSetFont (WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
	int GetLineCount(CString str);
	void DrawTextML(CDC& dc, CString& str, CRect& rect);
	CString * SplitString(CString str, char split, int& iSubStrs);
};

