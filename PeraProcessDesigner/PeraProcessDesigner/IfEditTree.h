#pragma once
#include "ZGridCtrl.h"
/************************************************************************/
/* if表达式编辑下拉显示树封装类，重新画竖线和双击及失去焦点                                      */
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
	void UpdateSelToParamName(); // 使得选中元素为选中行的第一列
	/*
	设置双击时接受消息的窗口句柄
	*/
	void SetMessReveiveWnd(HWND hRecvWnd);
	HWND GetMessReveiveWnd();
protected:
	virtual void DrawTreeLine(CDC* pDC, CZGridRow* pCurRow, CRect rcLeftOffset);
	virtual BOOL UpdateRowByNode( CZGridRow * pRow, CCxStructTreeNode * pNode );
private:
	HWND m_hWndRecvDbClichMsg;
};

