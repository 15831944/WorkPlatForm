
// 601DemoClientView.h : CMy601DemoClientView ��Ľӿ�
//

#pragma once
#include "InvokeServerMethod.h"

class CIceService;
class CMy601DemoClientView : public CListView
{
protected: // �������л�����
	CMy601DemoClientView();
	DECLARE_DYNCREATE(CMy601DemoClientView)

// ����
public:
	CMy601DemoClientDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // ������һ�ε���

// ʵ��
public:
	virtual ~CMy601DemoClientView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	InvokeServerMethod m_ServerObj;
	CIceService *m_pIceServiceClient;

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTestinvoke();
	afx_msg void OnConnectionSet();
	afx_msg void OnQuit();
	afx_msg LRESULT OnMsgLogMsg(WPARAM wp,LPARAM lp);
	void AddLogMsg(CString strMsg);
	afx_msg void OnClearLog();
	afx_msg void OnTestinvokeuserdefine();
	afx_msg LRESULT OnMsgStartLocalService(WPARAM wp,LPARAM lp);
	CString GetConfigIniFile();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTestinvokeByIp();
	afx_msg void OnTestInvokeByDomain();
};

#ifndef _DEBUG  // 601DemoClientView.cpp �еĵ��԰汾
inline CMy601DemoClientDoc* CMy601DemoClientView::GetDocument() const
   { return reinterpret_cast<CMy601DemoClientDoc*>(m_pDocument); }
#endif

