#pragma once
#include "PeraWebPageMgr.h"

class CPeraWebPageMgrClient :
	public CPeraWebPageMgr
{
public:
	CPeraWebPageMgrClient(void);
	virtual ~CPeraWebPageMgrClient(void);

	//virtual BOOL OnWebPageEvent( LPVOID lpData );

	//virtual CCxWebPageEvent * Require( int nModuleFlag, int nEventFlag
	//	, CCxWebPageEventExData * pExData = NULL );

protected:
	virtual BOOL _OnNewEvent( CCxWebPageEvent * pEvent );
};

extern CPeraWebPageMgrClient theWebPageMgr;