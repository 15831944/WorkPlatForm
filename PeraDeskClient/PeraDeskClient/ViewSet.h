#pragma once
#include "StlHeader.h"

class CChildFrame;
class CDocument;
class CView;

typedef struct _VIEW_DATA
{
	struct _VIEW_DATA();
	~_VIEW_DATA();
	void Clear(void);

	CChildFrame * m_pChildFrame;
	CDocument * m_pDoc;
	CView * m_pView;
	CString m_strId;
	CString m_strTitle;
	DWORD_PTR m_dwExData;
	CString m_strFlag;
	CString m_strErrInfo;
	HWND m_hwndInner;
} VIEW_DATA;

class CViewSet
{
public:
	CViewSet(void);
	~CViewSet(void);
	void Clear(void);
	BOOL Add(VIEW_DATA * pData);
	BOOL Remove(LPCTSTR lpszId);
	VIEW_DATA * Get(LPCTSTR lpszId);
	VIEW_DATA * Find(LPCTSTR lpszTitle);

	vector<VIEW_DATA *> m_vViews;
};

