// PeraProcessDesignerDoc.cpp : implementation of the CPeraProcessDesignerDoc class
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"

#include "PeraProcessDesignerDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPeraProcessDesignerDoc

IMPLEMENT_DYNCREATE(CPeraProcessDesignerDoc, CxDocumentExt)

BEGIN_MESSAGE_MAP(CPeraProcessDesignerDoc, CxDocumentExt)
END_MESSAGE_MAP()


// CPeraProcessDesignerDoc construction/destruction

CPeraProcessDesignerDoc::CPeraProcessDesignerDoc()
{
	// TODO: add one-time construction code here
}

CPeraProcessDesignerDoc::~CPeraProcessDesignerDoc()
{
}

BOOL CPeraProcessDesignerDoc::OnNewDocument()
{
	if (!CxDocumentExt::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	return TRUE;
}




// CPeraProcessDesignerDoc serialization

void CPeraProcessDesignerDoc::Serialize(CArchive& ar)
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


// CPeraProcessDesignerDoc diagnostics

#ifdef _DEBUG
void CPeraProcessDesignerDoc::AssertValid() const
{
	CxDocumentExt::AssertValid();
}

void CPeraProcessDesignerDoc::Dump(CDumpContext& dc) const
{
	CxDocumentExt::Dump(dc);
}
#endif //_DEBUG

void CPeraProcessDesignerDoc::OnCloseDocument()
{
	VIEW_DATA * pView = theViewMgr.Find( g_lpszViewTitleModel );
	if ( pView != NULL )
	{
		if (pView->m_pDoc == this)
		{
			if (theApp.m_processMgr.OnClose())
			{
				return;
				//return CxDocumentExt::OnCloseDocument();
			}
			else
			{
				return;
			}			
		}
	}

	if (theApp.m_processMgr.m_bProcessStarting)
	{
		CString sTitle = this->GetTitle();
		CString sLoadingFlowTitle = "ÔËÐÐÊµÀý:" + theApp.m_processMgr.m_sFlowId;
		if (sTitle.CompareNoCase(sLoadingFlowTitle) == 0)
		{
			VIEW_DATA * pViewFlow = theViewMgr.Find( sTitle );
			if ( pViewFlow != NULL )
			{
				if (pViewFlow->m_hwndInner != NULL && IsWindow(pViewFlow->m_hwndInner))
				{
					return;
				}
				else
				{
					theApp.m_processMgr.ClearRunVar();
				}
			}
		}
	}

	return CxDocumentExt::OnCloseDocument();
}

BOOL CPeraProcessDesignerDoc::DoFileSave()
{
	VIEW_DATA * pView = theViewMgr.Find( g_lpszViewTitleModel );
	if ( pView != NULL )
	{
		if (pView->m_pDoc == this)
		{
			theApp.m_processMgr.OnSave();
			return TRUE;
		}
	}

	return TRUE;
}





