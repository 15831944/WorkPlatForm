#include "StdAfx.h"
#include "WebContainerCmdLine.h"
#include "AttachWindowClient.h"


CWebContainerCmdLine::CWebContainerCmdLine(void)
{
}


CWebContainerCmdLine::~CWebContainerCmdLine(void)
{
}

BOOL CWebContainerCmdLine::ParseJson( LPCTSTR lpszData )
{
	return g_aw.Deserialize( lpszData );
}

CWebContainerCmdLine theCmd;