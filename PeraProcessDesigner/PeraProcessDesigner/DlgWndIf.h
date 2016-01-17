#pragma once
#include <BranchCondition.h>

//#define USE_SCROLLBAR_H //ʹ�ú��������

// CDlgWndIf �Ի���

class CDlgWndIf : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgWndIf)

public:
	CDlgWndIf(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgWndIf();

// �Ի�������
	enum { IDD = IDD_WND_IF };
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

	bool m_bScrollBarIsVisible; //��������״̬ʾ

	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void SetBranchPos(float fScaleX, float fScaleY);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	void ReSetScrollBar();
	void RefreshBranchPos();
	void SetBranchMode(BranchCondition::MODE mode);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
