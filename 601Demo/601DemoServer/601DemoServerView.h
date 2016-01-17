
// 601DemoServerView.h : CMy601DemoServerView ��Ľӿ�
//

#pragma once

class CIceService;
class CMy601DemoServerView : public CListView
{
protected: // �������л�����
	CMy601DemoServerView();
	DECLARE_DYNCREATE(CMy601DemoServerView)

// ����
public:
	CMy601DemoServerDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // ������һ�ε���

// ʵ��
public:
	virtual ~CMy601DemoServerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	CIceService *m_pIceServiceServer;
	BOOL    m_bServerConfigChanged;

// ���ɵ���Ϣӳ�亯��
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

#ifndef _DEBUG  // 601DemoServerView.cpp �еĵ��԰汾
inline CMy601DemoServerDoc* CMy601DemoServerView::GetDocument() const
   { return reinterpret_cast<CMy601DemoServerDoc*>(m_pDocument); }
#endif

