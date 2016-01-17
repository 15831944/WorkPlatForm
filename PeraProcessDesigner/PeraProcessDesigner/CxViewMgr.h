#pragma once
#include "ViewSet.h"

/*
ʹ�÷�����
�� theApp �У���ӳ�Ա������VIEW_DATA * m_pCurViewData;
ע�͵� theApp �е� CCommandLineInfo cmdInfo; ��صĴ���Ŀ��������EXEʱ���Զ��½��ĵ���
������д theApp ��������Ϣ����������Ĭ�Ϲ��ܣ�
afx_msg void OnFileNew();
afx_msg void OnFileOpen();
��

�� MDI �� View �ĸ����޸�Ϊ��CxViewExt��
�� MDI �� ChildFrame �ĸ����޸�Ϊ��CxChildFrameExt��
�� MDI �� Document �ĸ����޸�Ϊ��CxDocumentExt��

�� CMainFrame ��ʼ�������캯��֮ǰ����� friend class CxViewMgr;

�� theApp ��ͷ�ļ��У���� #include "CxViewMgr.h"

������ҳ�棬��Ҫ�޸� CxViewExt ��ļ���ʵ�ֺ�����ָ����ʾ

-------------------

ͨ������ theViewMgr.NewView( "��ģ����" ); �򿪽�ģ����ҳ�档

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
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
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
protected: // �������л�����
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