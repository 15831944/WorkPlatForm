#pragma once
#include <BranchCondition.h>


// CWndIf

class CWndIf : public CWnd
{
	DECLARE_DYNAMIC(CWndIf)

public:
	CWndIf();
	virtual ~CWndIf();
	bool AddBranch(const char* strExp = NULL, const char* strbranch = NULL, BranchCondition::TYPE type = BranchCondition::TYPE_MIDDLE, BranchCondition::MODE mode = BranchCondition::MODE_EXCLUSIVE); //添加分支

	void RefreshScrollBar();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	void Resize(int nOffsetX, int nOffsetY);

	vector<BranchCondition*> m_vBCs;  //分支容器
	int m_nWidth; //窗口宽
	int m_nHeight; //窗口高

	int m_nBranchSelect; //当前选择EDIT

	int m_nCurSelect; //当前combox选择序号

	CRect m_rectCurBranch; //当前分支位置
	static CRect m_conRectCurBranch;  
	static const int m_conBlankH;  //分支间隔
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void SetBranchPos(float fScaleX, float fScaleY);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	void ReSetScrollBar();
	void RefreshBranchPos();
	void SetBranchMode(BranchCondition::MODE mode);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


