#include "StdAfx.h"
#include "WebEventData.h"
#include "Tools.h"

#include "reader.h"
#include "writer.h"
#include "value.h"


//-------------------------------------------------------------------

CAttachWindowExData::CAttachWindowExData()
{

}

CAttachWindowExData::~CAttachWindowExData()
{
	Clear();
}

void CAttachWindowExData::Clear()
{
	m_sUrl.Empty();
}

BOOL CAttachWindowExData::Parse( LPCTSTR lpData )
{
	CString strSrc = lpData == NULL ? "" : lpData;
	if ( strSrc.IsEmpty() ) return FALSE;

	Json::Reader reader;
	Json::Value obj( Json::objectValue );
	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) return FALSE;
		if ( obj.isNull() || !obj.isObject() ) return FALSE;
		m_sUrl = obj["url"].isString() ? obj["url"].asCString() : "";
	}
	catch (...)
	{
		Clear(); return FALSE;
	}
	return TRUE;
}

CString CAttachWindowExData::GetJsonStr()
{
	try
	{
		Json::Value jsonObj( Json::objectValue );
		jsonObj["url"] = Json::Value( m_sUrl );

		Json::FastWriter writer;
		CString sStr = writer.write(jsonObj).c_str();

		return sStr;
	}
	catch (...)
	{
		return "";
	}
}

//////////////////////////////////////////////////////////////////////////

CWERunFlowCompleted::CWERunFlowCompleted()
{

}

CWERunFlowCompleted::~CWERunFlowCompleted()
{
	Clear();
}

BOOL CWERunFlowCompleted::ParseData( LPCTSTR lpData )
{
	CString strSrc = lpData == NULL ? "" : lpData;
	if ( strSrc.IsEmpty() ) return FALSE;

	Json::Reader reader;
	Json::Value obj( Json::objectValue );
	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) return FALSE;
		if ( obj.isNull() || !obj.isObject() ) return FALSE;
		m_sStatus = obj["status"].isString() ? obj["status"].asCString() : "";
	}
	catch (...)
	{
		ClearData(); return FALSE;
	}
	return TRUE;
}

CString CWERunFlowCompleted::GetDataStr()
{
	try
	{
		Json::Value jsonObj( Json::objectValue );
		jsonObj["status"] = Json::Value( m_sStatus );

		Json::FastWriter writer;
		CString sStr = writer.write(jsonObj).c_str();

		return sStr;
	}
	catch (...)
	{
		return "";
	}
}

void CWERunFlowCompleted::ClearData()
{
	m_sStatus.Empty();
}


//---------------------------------------------------------------------------------



CWERunFlowSubFlowId::CWERunFlowSubFlowId()
{

}

CWERunFlowSubFlowId::~CWERunFlowSubFlowId()
{
	Clear();
}

BOOL CWERunFlowSubFlowId::ParseData( LPCTSTR lpData )
{
	CString strSrc = lpData == NULL ? "" : lpData;
	if ( strSrc.IsEmpty() ) return FALSE;

	Json::Reader reader;
	Json::Value obj( Json::objectValue );
	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) return FALSE;
		if ( obj.isNull() || !obj.isObject() ) return FALSE;
		m_sSubFlowId = obj["subFlowId"].isString() ? obj["subFlowId"].asCString() : "";
	}
	catch (...)
	{
		ClearData(); return FALSE;
	}
	return TRUE;
}

CString CWERunFlowSubFlowId::GetDataStr()
{
	try
	{
		Json::Value jsonObj( Json::objectValue );
		jsonObj["subFlowId"] = Json::Value( m_sSubFlowId );

		Json::FastWriter writer;
		CString sStr = writer.write(jsonObj).c_str();

		return sStr;
	}
	catch (...)
	{
		return "";
	}
}

void CWERunFlowSubFlowId::ClearData()
{
	m_sSubFlowId.Empty();
}

CWERunFlowCancel::CWERunFlowCancel()
{

}

CWERunFlowCancel::~CWERunFlowCancel()
{

}

BOOL CWERunFlowCancel::ParseData( LPCTSTR lpData )
{
	return TRUE;
}

CString CWERunFlowCancel::GetDataStr()
{
	return "";
}

void CWERunFlowCancel::ClearData()
{

}

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
