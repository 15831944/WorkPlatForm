#pragma once

#include "MethodForDataMapIfExpress.h"
// CZComboBox

class CZComboBox : public CBCGPComboBox
{
	DECLARE_DYNAMIC(CZComboBox)

public:
	CZComboBox();
	virtual ~CZComboBox();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchange();
	static CString g_strEditText;
	static BranchExpress g_BranchExpress;
};


