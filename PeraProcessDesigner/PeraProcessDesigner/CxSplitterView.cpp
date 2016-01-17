// CxSplitterView.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "CxSplitterView.h"
#include "DlgResLibTree.h"
#include "DlgResLibImage.h"

const int nHorzMargin = 2;
const int nVertMargin = 2;
// CCxSplitterView

IMPLEMENT_DYNCREATE(CCxSplitterView, CView)

CCxSplitterView::CCxSplitterView()
{
	m_nSpliterViewFlag = theApp.m_nSpliterViewFlag;
}

CCxSplitterView::~CCxSplitterView()
{
}

BEGIN_MESSAGE_MAP(CCxSplitterView, CView)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CCxSplitterView ��ͼ

void CCxSplitterView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	//CRect rectClient;
	//GetClientRect (rectClient);

}

// CCxSplitterView ���

#ifdef _DEBUG
void CCxSplitterView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CCxSplitterView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CCxSplitterView ��Ϣ�������


BOOL CCxSplitterView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	return TRUE;
	//return CView::OnEraseBkgnd(pDC);
}


int CCxSplitterView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	//ModifyStyle(0, WS_CLIPSIBLINGS|WS_CLIPCHILDREN);

	switch (theApp.m_nSpliterViewFlag)
	{
	case theApp.SPLVF_COMPONENT_TREE:
		if (theApp.m_pDlgResLibTree == NULL)
		{
			theApp.m_pDlgResLibTree = new CDlgResLibTree(this);
			theApp.m_pDlgResLibTree->Create(theApp.m_pDlgResLibTree->IDD, this);
			theApp.m_pDlgResLibTree->ShowWindow(SW_SHOW);
		}
		break;
	case theApp.SPLVF_COMPONENT_LIST:
		if (theApp.m_pDlgResLibImage == NULL)
		{
			theApp.m_pDlgResLibImage = new CDlgResLibImage(this);
			theApp.m_pDlgResLibImage->Create(theApp.m_pDlgResLibImage->IDD, this);
			theApp.m_pDlgResLibImage->ShowWindow(SW_SHOW);
		}
		break;

	default:
		break;
	}

	return 0;
}


void CCxSplitterView::OnDestroy()
{
	switch (m_nSpliterViewFlag)
	{
	case theApp.SPLVF_COMPONENT_TREE:
		if (theApp.m_pDlgResLibTree != NULL)
		{
			theApp.m_pDlgResLibTree->DestroyWindow();
			delete theApp.m_pDlgResLibTree;
			theApp.m_pDlgResLibTree = NULL;
		}
		break;
	case theApp.SPLVF_COMPONENT_LIST:
		if (theApp.m_pDlgResLibImage != NULL)
		{
			theApp.m_pDlgResLibImage->DestroyWindow();
			delete theApp.m_pDlgResLibImage;
			theApp.m_pDlgResLibImage = NULL;
		}
		break;

	default:
		break;
	}
	CView::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������
}


void CCxSplitterView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	CRect rcClient(0,0,cx,cy);

	switch (m_nSpliterViewFlag)
	{
	case theApp.SPLVF_COMPONENT_TREE:
		if ( theApp.m_pDlgResLibTree != NULL 
			&& IsWindow(theApp.m_pDlgResLibTree->GetSafeHwnd()) )
		{
			//rcClient.DeflateRect(0,0,1,0);
			theApp.m_pDlgResLibTree->MoveWindow(&rcClient);
		}
		break;
	case theApp.SPLVF_COMPONENT_LIST:
		if (theApp.m_pDlgResLibImage != NULL 
			&& IsWindow(theApp.m_pDlgResLibImage->GetSafeHwnd()) )
		{
			//rcClient.DeflateRect(0,0,1,0);
			theApp.m_pDlgResLibImage->MoveWindow(&rcClient);
		}
		break;

	default:
		break;
	}
}
