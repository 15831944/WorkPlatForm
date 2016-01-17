#pragma once
#include "CxWebPageEvent.h"

typedef map< CString, CCxWebPageEvent* > WEBPAGE_EVENT_MAP;

class CCxWebPageMgr
{
	friend class CCxWebPageEvent;

public:
	//�¼�����---------------------
	enum{
		FE_START = 1, //����WebPage
		FE_ATTACH, //�󶨵�������
		FE_NAVIGATE,
		FE_CUSTOM
	};

public:
	CCxWebPageMgr(void);
	virtual ~CCxWebPageMgr(void);

protected:
	//
	void ClearEventMap(void);

	//����Event����
public:
	CCxWebPageEvent * NewEvent( int nModuleFlag, int nEventFlag, BOOL bMgr = FALSE );
	CCxWebPageEvent * NewEvent( int nModuleFlag, int nEventFlag
		, CCxWebPageEventExData * pExData, BOOL bMgr = FALSE );
	CCxWebPageEvent * NewEvent( LPCTSTR lpszData );
protected:
	//new һ���µ� �¼�����
	virtual CCxWebPageEvent * _GetNew( int nModuleFlag, int nEventFlag );
	//new �¼��������չ����
	virtual CCxWebPageEventExData* _OnNewEventExData( int nModuleFlag, int nEventFlag );
	//�����¼�����ĺ�������
	virtual BOOL _OnNewEvent( CCxWebPageEvent * pEvent );

public:
	//ɾ��
	BOOL DeleteEvent( LPCTSTR lpszEventId );
	BOOL DeleteEvent( CCxWebPageEvent*& pEvent );
	//����
	CCxWebPageEvent * Find( LPCTSTR lpszEventId );

	//�����������̷��͸������̵�WebPage�¼�
	virtual BOOL OnWebPageEvent( LPCTSTR lpszData );
	//
	virtual CCxWebPageEvent * Require( int nModuleFlag, int nEventFlag
		, CCxWebPageEventExData * pExData = NULL );

protected:
	//����
	WEBPAGE_EVENT_MAP::iterator _Find( LPCTSTR lpszEventId );
	WEBPAGE_EVENT_MAP::iterator _Find( CCxWebPageEvent * pEvent );

protected:
	WEBPAGE_EVENT_MAP m_mapEvent;
};

