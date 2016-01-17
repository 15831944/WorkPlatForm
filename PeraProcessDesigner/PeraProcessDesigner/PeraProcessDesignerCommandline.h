#pragma once
#include "cxcmdlinebase.h"
class CPeraProcessDesignerCommandline :
	public CCxCmdlineBase
{
public:
	CPeraProcessDesignerCommandline(void);
	virtual ~CPeraProcessDesignerCommandline(void);
	virtual BOOL ParseJson( LPCTSTR lpszData );
};

