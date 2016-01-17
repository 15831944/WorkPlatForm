// PeraDeskClientDoc.cpp : implementation of the CPeraDeskClientDoc class
//

#include "stdafx.h"
#include "PeraDeskClient.h"

#include "PeraDeskClientDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPeraDeskClientDoc

IMPLEMENT_DYNCREATE(CPeraDeskClientDoc, CxDocumentExt)

BEGIN_MESSAGE_MAP(CPeraDeskClientDoc, CxDocumentExt)
END_MESSAGE_MAP()


// CPeraDeskClientDoc construction/destruction

CPeraDeskClientDoc::CPeraDeskClientDoc()
{
	// TODO: add one-time construction code here
}

CPeraDeskClientDoc::~CPeraDeskClientDoc()
{
}

BOOL CPeraDeskClientDoc::OnNewDocument()
{
	if (!CxDocumentExt::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	return TRUE;
}




// CPeraDeskClientDoc serialization

void CPeraDeskClientDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CPeraDeskClientDoc diagnostics

#ifdef _DEBUG
void CPeraDeskClientDoc::AssertValid() const
{
	CxDocumentExt::AssertValid();
}

void CPeraDeskClientDoc::Dump(CDumpContext& dc) const
{
	CxDocumentExt::Dump(dc);
}
#endif //_DEBUG

void CPeraDeskClientDoc::OnCloseDocument()
{
	VIEW_DATA * pView = theViewMgr.Find( g_lpszViewTitleTaskModel );
	if ( pView != NULL )
	{
		if (pView->m_pDoc == this)
		{
			//if (theApp.m_processMgr.OnClose())
			//{
			//	return;
			//	//return CxDocumentExt::OnCloseDocument();
			//}
			//else
			//{
			//	return;
			//}			
		}
	}

	//if (theApp.m_processMgr.m_bProcessStarting)
	//{
	//	CString sTitle = this->GetTitle();
	//	CString sLoadingFlowTitle = "ÔËÐÐÊµÀý:" + theApp.m_processMgr.m_sFlowId;
	//	if (sTitle.CompareNoCase(sLoadingFlowTitle) == 0)
	//	{
	//		VIEW_DATA * pViewFlow = theViewMgr.Find( sTitle );
	//		if ( pViewFlow != NULL )
	//		{
	//			if (pViewFlow->m_hwndInner != NULL && IsWindow(pViewFlow->m_hwndInner))
	//			{
	//				return;
	//			}
	//			else
	//			{
	//				theApp.m_processMgr.m_bProcessStarting = FALSE;
	//				theApp.m_processMgr.ClearRunVar();
	//			}
	//		}
	//	}
	//}

	return CxDocumentExt::OnCloseDocument();
}

BOOL CPeraDeskClientDoc::DoFileSave()
{
	VIEW_DATA * pView = theViewMgr.Find( g_lpszViewTitleTaskModel );
	//if ( pView != NULL )
	//{
	//	if (pView->m_pDoc == this)
	//	{
	//		theApp.m_processMgr.OnSave();
	//		return TRUE;
	//	}
	//}

	return TRUE;
}





