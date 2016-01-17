// PeraProcessDesignerDoc.h : interface of the CPeraProcessDesignerDoc class
//


#pragma once
#include "CxViewMgr.h"


class CPeraProcessDesignerDoc : public CxDocumentExt
{
protected: // create from serialization only
	CPeraProcessDesignerDoc();
	DECLARE_DYNCREATE(CPeraProcessDesignerDoc)

// Attributes
public:

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void OnCloseDocument();
	virtual BOOL DoFileSave();

// Implementation
public:
	virtual ~CPeraProcessDesignerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


