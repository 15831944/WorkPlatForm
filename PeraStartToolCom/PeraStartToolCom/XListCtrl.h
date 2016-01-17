#pragma once


// CXListCtrl

class CXListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CXListCtrl)

public:
	CXListCtrl();
	virtual ~CXListCtrl();
protected:
	virtual void DrawItem(_In_ LPDRAWITEMSTRUCT lpDrawItemStruct);
protected:
	DECLARE_MESSAGE_MAP()
};


