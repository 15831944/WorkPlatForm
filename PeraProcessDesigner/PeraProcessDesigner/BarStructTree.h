#pragma once
#include "CxFloatBar.h"
//#include "CxStructTreeGrid.h"
#include "CxStructTreeGridT.h"


// CCxFloatDlgBar

class CBarStructTree : public CCxFloatBar
{
	DECLARE_DYNAMIC(CBarStructTree)

public:
	CBarStructTree();
	virtual ~CBarStructTree();

protected:
	DECLARE_MESSAGE_MAP()

public:
	CCxStructTreeGridT		m_wndGridTree;
	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//afx_msg LRESULT OnGridTreeSelChanged (WPARAM wp, LPARAM lp);
	//afx_msg LRESULT OnGridTreeItemChanged (WPARAM wp, LPARAM lp);
	//afx_msg LRESULT OnGridTreeBeginInplaceEdit (WPARAM wp, LPARAM lp);
	//afx_msg LRESULT OnGridTreeEndInplaceEdit (WPARAM wp, LPARAM lp);

	void UpdateTree();

	virtual int GetCaptionHeight () const;
	virtual void OnEraseNCBackground (CDC* pDC, CRect rectBar);
	//CStructTreeNodeData * m_pRoot;

protected:
	//virtual void DrawCaption (CDC* pDC, CRect rectCaption);
	afx_msg void OnPaint();
};


