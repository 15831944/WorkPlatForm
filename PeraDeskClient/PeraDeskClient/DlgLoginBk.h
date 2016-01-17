#pragma once
#include "dlgloginmain.h"
#include "LoginCloseBtn.h"

// CDlgLoginBk 对话框

class CDlgLoginBk : public CDialog
{
	DECLARE_DYNAMIC(CDlgLoginBk)

public:
	CDlgLoginBk(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgLoginBk();

	// 对话框数据
	enum { IDD = IDD_DIALOG_LOGIN_BK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	BOOL UpdateBk(BOOL bOverClose = FALSE);
	HDC m_hdcMemory;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL ImageFromIDResource(UINT nID, LPCTSTR sTR, Gdiplus::Image * &pImg);
	BLENDFUNCTION m_Blend;
	Gdiplus::Image* m_pImageBk;
	Gdiplus::Image* m_pImageCloseNor;
	Gdiplus::Image* m_pImageCloseOver;
	int m_nBkWidth;
	int m_nBkHeight;
	int m_nWndWidth;
	int m_nWndHeight;
	int m_nCloseWidth;
	int m_nCloseHeight;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	CDlgLoginMain m_DlgLoginMain;
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	void OnMyOk();
	void OnMyCancle();
protected:
	virtual void OnCancel();
	virtual void OnOK();
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	HBITMAP KCreateCompatibleBitmap(int nWidth, int nHeight);
	afx_msg void OnBnClickedButtonClose();
	CLoginCloseBtn m_btnClose;
	afx_msg LRESULT OnMouseLeaveClose(WPARAM wParam,LPARAM lParam);       
	afx_msg LRESULT OnMouseOverClose(WPARAM wParam,LPARAM lParam);  
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	void   LoginFromCopyData(LPVOID lpData);
};
