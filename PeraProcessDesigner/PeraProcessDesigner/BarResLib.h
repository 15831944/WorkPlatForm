#pragma once
#include "CxFloatBar.h"
//#include "CxStructTreeGrid.h"
#include "DlgResLib.h"


// CCxFloatDlgBar

class CBarResLib : public CCxFloatBar
{
	DECLARE_DYNAMIC(CBarResLib)

public:
	CBarResLib();
	virtual ~CBarResLib();

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void UpdateContent();

	CDlgResLib * m_pDlgInner;
};


