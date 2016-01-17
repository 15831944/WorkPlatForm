#pragma once
class CCxBCGPDialog : public CBCGPDialog
{
	friend class CBCGPDlgImpl;

	DECLARE_DYNAMIC(CCxBCGPDialog)

public:
	CCxBCGPDialog();
	CCxBCGPDialog(UINT nIDTemplate, CWnd *pParent= NULL);
	CCxBCGPDialog(LPCTSTR lpszTemplateName, CWnd *pParentWnd= NULL);

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
};

