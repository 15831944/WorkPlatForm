
// 601DemoClientView.h : CMy601DemoClientView 类的接口
//

#pragma once
#include "InvokeServerMethod.h"

class CIceService;
class CMy601DemoClientView : public CListView
{
protected: // 仅从序列化创建
	CMy601DemoClientView();
	DECLARE_DYNCREATE(CMy601DemoClientView)

// 特性
public:
	CMy601DemoClientDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用

// 实现
public:
	virtual ~CMy601DemoClientView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	InvokeServerMethod m_ServerObj;
	CIceService *m_pIceServiceClient;

// 生成的消息映射函数
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

#ifndef _DEBUG  // 601DemoClientView.cpp 中的调试版本
inline CMy601DemoClientDoc* CMy601DemoClientView::GetDocument() const
   { return reinterpret_cast<CMy601DemoClientDoc*>(m_pDocument); }
#endif

