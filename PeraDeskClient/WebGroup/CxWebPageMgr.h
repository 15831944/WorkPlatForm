#pragma once
#include "CxWebPageEvent.h"

typedef map< CString, CCxWebPageEvent* > WEBPAGE_EVENT_MAP;

class CCxWebPageMgr
{
	friend class CCxWebPageEvent;

public:
	//事件代码---------------------
	enum{
		FE_START = 1, //启动WebPage
		FE_ATTACH, //绑定到主进程
		FE_NAVIGATE,
		FE_CUSTOM
	};

public:
	CCxWebPageMgr(void);
	virtual ~CCxWebPageMgr(void);

protected:
	//
	void ClearEventMap(void);

	//创建Event对象
public:
	CCxWebPageEvent * NewEvent( int nModuleFlag, int nEventFlag, BOOL bMgr = FALSE );
	CCxWebPageEvent * NewEvent( int nModuleFlag, int nEventFlag
		, CCxWebPageEventExData * pExData, BOOL bMgr = FALSE );
	CCxWebPageEvent * NewEvent( LPCTSTR lpszData );
protected:
	//new 一个新的 事件对象
	virtual CCxWebPageEvent * _GetNew( int nModuleFlag, int nEventFlag );
	//new 事件对象的扩展数据
	virtual CCxWebPageEventExData* _OnNewEventExData( int nModuleFlag, int nEventFlag );
	//创建事件对象的后续处理
	virtual BOOL _OnNewEvent( CCxWebPageEvent * pEvent );

public:
	//删除
	BOOL DeleteEvent( LPCTSTR lpszEventId );
	BOOL DeleteEvent( CCxWebPageEvent*& pEvent );
	//查找
	CCxWebPageEvent * Find( LPCTSTR lpszEventId );

	//处理其他进程发送给本进程的WebPage事件
	virtual BOOL OnWebPageEvent( LPCTSTR lpszData );
	//
	virtual CCxWebPageEvent * Require( int nModuleFlag, int nEventFlag
		, CCxWebPageEventExData * pExData = NULL );

protected:
	//查找
	WEBPAGE_EVENT_MAP::iterator _Find( LPCTSTR lpszEventId );
	WEBPAGE_EVENT_MAP::iterator _Find( CCxWebPageEvent * pEvent );

protected:
	WEBPAGE_EVENT_MAP m_mapEvent;
};

