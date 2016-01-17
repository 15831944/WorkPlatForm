#pragma once
#include "bcgpedit.h"
class CEditBranch :
	public CBCGPEdit
{
public:
	CEditBranch(void);
	virtual ~CEditBranch(void);
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	void CenterTextV();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNcPaint();
};

