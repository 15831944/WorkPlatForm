
// 601DemoServerView.h : CMy601DemoServerView 类的接口
//

#pragma once

class CIceService;
class CMy601DemoServerView : public CListView
{
protected: // 仅从序列化创建
	CMy601DemoServerView();
	DECLARE_DYNCREATE(CMy601DemoServerView)

// 特性
public:
	CMy601DemoServerDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用

// 实现
public:
	virtual ~CMy601DemoServerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	CIceService *m_pIceServiceServer;
	BOOL    m_bServerConfigChanged;

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnServerPortSet();
	afx_msg void OnQuitServer();
	void AddLogMsg(CString strMsg);
	afx_msg void OnClearLog();
	afx_msg void OnStartservice();
	afx_msg LRESULT OnMsgLogMsg(WPARAM wp,LPARAM lp);

	afx_msg void OnUpdateStartservice(CCmdUI *pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

#ifndef _DEBUG  // 601DemoServerView.cpp 中的调试版本
inline CMy601DemoServerDoc* CMy601DemoServerView::GetDocument() const
   { return reinterpret_cast<CMy601DemoServerDoc*>(m_pDocument); }
#endif

