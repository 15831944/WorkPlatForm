#include "StdAfx.h"
#include "AttachWindowClient.h"


void ResponseAttachProcessWnd( HWND hClientWnd )
{
	g_aw.ResponseHost( GetCurrentProcessId(), hClientWnd );
}


CAttachWindow g_aw;