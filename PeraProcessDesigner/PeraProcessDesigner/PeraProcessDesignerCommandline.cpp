#include "StdAfx.h"
#include "PeraProcessDesignerCommandline.h"

#include "reader.h"
#include "writer.h"
#include "value.h"
#ifdef _DEBUG
#pragma comment(lib, "jsond.lib")
#else
#pragma comment(lib, "json.lib")
#endif

CPeraProcessDesignerCommandline::CPeraProcessDesignerCommandline(void)
{
}


CPeraProcessDesignerCommandline::~CPeraProcessDesignerCommandline(void)
{
	Clear();
}

BOOL CPeraProcessDesignerCommandline::ParseJson( LPCTSTR lpszData )
{
	CString strSrc = lpszData == NULL ? "" : lpszData;
	if (strSrc.IsEmpty()) return FALSE;

	CString sUser, sRealName, sTicket;
	Json::Reader reader;
	Json::Value obj( Json::objectValue );
	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) return FALSE;
		if ( obj.isNull() || !obj.isObject() ) return FALSE;

		sUser = obj["userName"].isString() ? obj["userName"].asCString() : _T("");
		sRealName = obj["realName"].isString() ? obj["realName"].asCString() : _T("");
		sTicket = obj["ticket-proxy"].isString() ? obj["ticket-proxy"].asCString() : _T("");
		if ( !sTicket.IsEmpty() ) sTicket = "ticket-proxy=" + sTicket;

		SetValue( "userName", sUser );
		SetValue( "realName", sRealName );
		SetValue( "ticket-proxy", sTicket );
	}
	catch (...)
	{
		Clear(); return FALSE;
	}

	if ( sTicket.IsEmpty() || sUser.IsEmpty() )
	{
		Clear(); return FALSE;
	}

	return TRUE;
}