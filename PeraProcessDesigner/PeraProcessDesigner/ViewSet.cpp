#include "StdAfx.h"
#include "ViewSet.h"
#include "PeraProcessDesigner.h"

/*
文档元素的创建顺序是：m_pDoc -- m_pChildFrame -- m_pView
*/
_VIEW_DATA::_VIEW_DATA()
{
	m_pChildFrame = NULL;
	m_pDoc = NULL;
	m_pView = NULL;
	m_strId = "";
	m_strTitle = "";
	m_dwExData = 0;
	m_strFlag = "";
	m_strErrInfo = "";
	m_hwndInner = NULL;
}

_VIEW_DATA::~_VIEW_DATA()
{
	Clear();
}

void _VIEW_DATA::Clear( void )
{
	m_pChildFrame = NULL;
	m_pDoc = NULL;
	m_pView = NULL;
	m_strId = "";
	m_strTitle = "";
	m_dwExData = 0;
	m_strFlag = "";
	m_strErrInfo = "";
	m_hwndInner = NULL;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CViewSet::CViewSet(void)
{
}


CViewSet::~CViewSet(void)
{
}

void CViewSet::Clear( void )
{

}


BOOL CViewSet::Add( VIEW_DATA * pData )
{
	vector<VIEW_DATA *>::iterator itr;

	if (pData == NULL) return FALSE;
	if (pData->m_strId.IsEmpty()
		|| pData->m_strTitle.IsEmpty()
		|| pData->m_pView == NULL) return FALSE;
	for (itr = m_vViews.begin(); itr != m_vViews.end(); ++itr)
	{
		if ((*itr)->m_pView == pData->m_pView) return FALSE;
		if ((*itr)->m_strId == pData->m_strId) return FALSE;
	}
	m_vViews.push_back(pData);
	theApp.GetMainWnd()->SetTimer( TIMER_MAINFRAME_UPDATEWINDOW, TIMER_MAINFRAME_UPDATEWINDOW_TIMEOUT, 0);//更新菜单栏
	return TRUE;
}

BOOL CViewSet::Remove( LPCTSTR lpszId )
{
	vector<VIEW_DATA *>::iterator itr;
	CString strId = lpszId == NULL ? "" : lpszId;

	if (strId.IsEmpty()) return FALSE;
	
	for (itr = m_vViews.begin(); itr != m_vViews.end(); ++itr)
	{
		if ((*itr)->m_strId != strId) continue;

		delete ((VIEW_DATA *)(*itr));
		(*itr) = NULL;
		m_vViews.erase(itr);
		theApp.GetMainWnd()->SetTimer( TIMER_MAINFRAME_UPDATEWINDOW, TIMER_MAINFRAME_UPDATEWINDOW_TIMEOUT, 0);//更新菜单栏
		return TRUE;
	}
	return FALSE;
}

VIEW_DATA * CViewSet::Get( LPCTSTR lpszId )
{
	vector<VIEW_DATA *>::iterator itr;
	CString strId = lpszId == NULL ? "" : lpszId;

	if (strId.IsEmpty()) return NULL;

	for (itr = m_vViews.begin(); itr != m_vViews.end(); ++itr)
	{
		if ((*itr)->m_strId != strId) continue;
		return (*itr);
	}
	return NULL;
}

VIEW_DATA * CViewSet::Find( LPCTSTR lpszTitle )
{
	CString strTitle = lpszTitle == NULL ? "" : lpszTitle;
	if (strTitle.IsEmpty()) return NULL;

	for (int i=0; i<(int)m_vViews.size(); i++)
	{
		if ( m_vViews[i]->m_strTitle.CompareNoCase((LPCTSTR)strTitle) != 0 )
			continue;
		return m_vViews[i];
	}
	return NULL;
}
