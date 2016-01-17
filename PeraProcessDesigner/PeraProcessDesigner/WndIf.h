#pragma once
#include <BranchCondition.h>


// CWndIf

class CWndIf : public CWnd
{
	DECLARE_DYNAMIC(CWndIf)

public:
	CWndIf();
	virtual ~CWndIf();
	bool AddBranch(const char* strExp = NULL, const char* strbranch = NULL, BranchCondition::TYPE type = BranchCondition::TYPE_MIDDLE, BranchCondition::MODE mode = BranchCondition::MODE_EXCLUSIVE); //��ӷ�֧

	void RefreshScrollBar();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	void Resize(int nOffsetX, int nOffsetY);

	vector<BranchCondition*> m_vBCs;  //��֧����
	int m_nWidth; //���ڿ�
	int m_nHeight; //���ڸ�

	int m_nBranchSelect; //��ǰѡ��EDIT

	int m_nCurSelect; //��ǰcomboxѡ�����

	CRect m_rectCurBranch; //��ǰ��֧λ��
	static CRect m_conRectCurBranch;  
	static const int m_conBlankH;  //��֧���
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


