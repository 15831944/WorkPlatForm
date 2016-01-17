#pragma once
#include "PeraWebPageMgr.h"
class CPeraWebPageMgrHost :
	public CPeraWebPageMgr
{
public:
	CPeraWebPageMgrHost(void);
	virtual ~CPeraWebPageMgrHost(void);

	virtual BOOL OnWebPageEvent( LPVOID lpData );
	virtual CCxWebPageEvent * Require( int nModuleFlag, int nEventFlag
		, CCxWebPageEventExData * pExData = NULL );

	BOOL Navigate( int nModuleFlag, LPCTSTR lpszUrl );

	BOOL SendCopyDataLoginOut(HWND hWndSend);

	BOOL SendCopyDataRefresh(HWND hWndSend);

protected:
	virtual BOOL _OnNewEvent( CCxWebPageEvent * pEvent );

	BOOL StartWebPage( CCxWebPageEvent * pEvent );


};

extern CPeraWebPageMgrHost theWebPageMgr;