#pragma once
#include "ZGridCtrl.h"
/************************************************************************/
/* if���ʽ�༭������ʾ����װ�࣬���»����ߺ�˫����ʧȥ����                                      */
/************************************************************************/
class CIfEditTree :
	public CZGridCtrl
{
	DECLARE_DYNAMIC(CIfEditTree)
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
public:
	CIfEditTree(void);
	~CIfEditTree(void);
	void UpdateSelToParamName(); // ʹ��ѡ��Ԫ��Ϊѡ���еĵ�һ��
	/*
	����˫��ʱ������Ϣ�Ĵ��ھ��
	*/
	void SetMessReveiveWnd(HWND hRecvWnd);
	HWND GetMessReveiveWnd();
protected:
	virtual void DrawTreeLine(CDC* pDC, CZGridRow* pCurRow, CRect rcLeftOffset);
	virtual BOOL UpdateRowByNode( CZGridRow * pRow, CCxStructTreeNode * pNode );
private:
	HWND m_hWndRecvDbClichMsg;
};

