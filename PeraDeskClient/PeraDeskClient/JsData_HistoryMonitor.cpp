#include "StdAfx.h"
#include "JsData_HistoryMonitor.h"

#include "value.h"
#include "reader.h"
#include "writer.h"

//////////////////////////////////////////////////////////////////////////
CJsData_ShowViewRunFlow::CJsData_ShowViewRunFlow()
{
	m_nWidth = 0;
	m_nHeight = 0;
}
CJsData_ShowViewRunFlow::~CJsData_ShowViewRunFlow()
{
	Clear();
}

void CJsData_ShowViewRunFlow::Clear()
{
	m_strUrl = "";
	m_nWidth = 0;
	m_nHeight = 0;
}

BOOL CJsData_ShowViewRunFlow::Parse( LPCTSTR lpszData )
{
	Clear();
	CString strSrc = lpszData == NULL ? "" : lpszData;
	if (strSrc.IsEmpty()) return FALSE;

	Json::Reader reader;
	Json::Value obj( Json::objectValue );
	CString str;
	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) return FALSE;
		if ( obj.isNull() || !obj.isObject() ) return FALSE;

		m_strUrl = ( obj["url"].isString()) ? obj["url"].asCString() : _T("");
		
		str = ( obj["width"].isString()) ? obj["width"].asCString() : _T("");
		if ( !str.IsEmpty() )
			m_nWidth = strtol( (char*)(LPCTSTR)str, NULL, 10 );
		
		str = ( obj["height"].isString()) ? obj["height"].asCString() : _T("");
		if ( !str.IsEmpty() )
			m_nHeight = strtol( (char*)(LPCTSTR)str, NULL, 10 );
	}
	catch (...)
	{
		Clear(); return FALSE;
	}
	return TRUE;
}
