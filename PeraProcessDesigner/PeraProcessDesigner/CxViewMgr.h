#pragma once
#include "ViewSet.h"

/*
使用方法：
在 theApp 中，天加成员变量：VIEW_DATA * m_pCurViewData;
注释掉 theApp 中的 CCommandLineInfo cmdInfo; 相关的处理，目的是启动EXE时不自动新建文档。
可以重写 theApp 的以下消息，便于屏蔽默认功能：
afx_msg void OnFileNew();
afx_msg void OnFileOpen();
等

将 MDI 的 View 的父类修改为：CxViewExt。
将 MDI 的 ChildFrame 的父类修改为：CxChildFrameExt。
将 MDI 的 Document 的父类修改为：CxDocumentExt。

在 CMainFrame 开始处，构造函数之前，添加 friend class CxViewMgr;

在 theApp 的头文件中，添加 #include "CxViewMgr.h"

新增加页面，需要修改 CxViewExt 类的几个实现函数。指定标示

-------------------

通过调用 theViewMgr.NewView( "建模环境" ); 打开建模环境页面。

*/

//////////////////////////////////////////////////////////////////////////
class CxViewExt : public CView
{
protected:
	DECLARE_DYNCREATE(CxViewExt)
	CxViewExt();
public:
	virtual ~CxViewExt(void);
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()

public:
	VIEW_DATA * m_pViewData;
};

//////////////////////////////////////////////////////////////////////////
class CxChildFrameExt : public CBCGPMDIChildWnd
{
	DECLARE_DYNCREATE(CxChildFrameExt)
public:
	CxChildFrameExt(void);
	virtual ~CxChildFrameExt(void);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	VIEW_DATA * m_pViewData;
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
class CxDocumentExt : public CDocument
{
protected: // 仅从序列化创建
	CxDocumentExt(void);
	DECLARE_DYNCREATE(CxDocumentExt)
public:
	virtual ~CxDocumentExt(void);
public:
	virtual BOOL OnNewDocument();
public:
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
class CxViewMgr : public CViewSet
{
public:
	CxViewMgr(void);
	virtual ~CxViewMgr(void);

	VIEW_DATA * NewView(LPCTSTR lpszViewName, LPCTSTR lpszFlag);
	void ActiveView( VIEW_DATA * pView );
	void CloseView( VIEW_DATA * pView );
	LRESULT OnCustomHandler( VIEW_DATA * pViewData, UINT uMsg, WPARAM wp = 0, LPARAM lp = 0 );
	CBCGPTabWnd * GetViewTab( LPCTSTR lpszViewName );
	VIEW_DATA * NewView(LPCTSTR lpszViewName, HWND hwndApp);
};

extern CxViewMgr theViewMgr;