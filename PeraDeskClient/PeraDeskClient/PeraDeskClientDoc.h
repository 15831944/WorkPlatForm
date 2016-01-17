// PeraDeskClientDoc.h : interface of the CPeraDeskClientDoc class
//


#pragma once
#include "CxViewMgr.h"


class CPeraDeskClientDoc : public CxDocumentExt
{
protected: // create from serialization only
	CPeraDeskClientDoc();
	DECLARE_DYNCREATE(CPeraDeskClientDoc)

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
	virtual ~CPeraDeskClientDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


