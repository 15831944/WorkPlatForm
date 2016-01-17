#pragma once
#include "ZToolTipEvent.h"

class CCxBCGPDialog : public CBCGPDialog, public ZToolTipEvent
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

