#pragma once
#include "CxCmdLineBase.h"

class CWebContainerCmdLine : public CCxCmdlineBase
{
public:
	CWebContainerCmdLine(void);
	virtual ~CWebContainerCmdLine(void);
	virtual BOOL ParseJson( LPCTSTR lpszData );
};

extern CWebContainerCmdLine theCmd;