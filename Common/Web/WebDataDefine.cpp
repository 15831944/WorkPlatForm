#include "StdAfx.h"
#include "WebDataDefine.h"
#include "PtrVectorHeader.h"
#include "Tools.h"

#include "reader.h"
#include "writer.h"
#include "value.h"

UINT WM_WEBEVENT_ASYNC = ::RegisterWindowMessage ( _T("WM_WEBEVENT_ASYNC") );
UINT WM_WEB_LOADED = ::RegisterWindowMessage ( _T("WM_WEB_LOADED") );

CWebMethodParamData::CWebMethodParamData()
{

}

CWebMethodParamData::~CWebMethodParamData()
{
	Clear();
}

void CWebMethodParamData::Clear( void )
{
	m_sName = "";
	m_sValue = "";
	m_sDataType = "";
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


CWebMethodData::CWebMethodData()
{

}

CWebMethodData::~CWebMethodData()
{
	Clear();
}

void CWebMethodData::Clear( void )
{
	m_sMethodName = "";
	PtrVectorClear( m_vParams );
	m_sResult = "";
	m_sInfo = "";
}

CString CWebMethodData::GetJsonStr( void )
{
	return "";
}

BOOL CWebMethodData::Parse( LPCTSTR lpszJsonStr )
{
	return TRUE;
}



BOOL CWebEventData::Parse( LPCTSTR lpData )
{
	Clear();
	CString strSrc = lpData == NULL ? "" : lpData;
	if ( strSrc.IsEmpty() ) return FALSE;

	Json::Reader reader;
	Json::Value obj( Json::objectValue );
	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) return FALSE;
		if ( obj.isNull() || !obj.isObject() ) return FALSE;
		m_sId = obj["id"].isString() ? obj["id"].asCString() : "";
		m_sWebId = obj["webId"].isString() ? obj["webId"].asCString() : "";
		m_dwWebType = obj["webType"].isInt() ? obj["webType"].asInt() : 0;
		m_dwWebEvent = obj["webEvent"].isInt() ? obj["webEvent"].asInt() : 0;
		m_sData = obj["data"].isString() ? obj["data"].asCString() : "";

		if ( !ParseData( m_sData ) ) return FALSE;
	}
	catch (...)
	{
		Clear(); return FALSE;
	}
	return TRUE;
}

CString CWebEventData::GetJsonStr()
{
	try
	{
		Json::Value jsonObj( Json::objectValue );
		jsonObj["id"] = Json::Value( m_sId );
		jsonObj["webId"] = Json::Value( m_sWebId );
		jsonObj["webType"] = Json::Value( (int)m_dwWebType );
		jsonObj["webEvent"] = Json::Value( (int)m_dwWebEvent );
		CString sData = GetDataStr();
		jsonObj["data"] = Json::Value( sData );

		Json::FastWriter writer;
		CString sStr = writer.write(jsonObj).c_str();

		return sStr;
	}
	catch (...)
	{
		return "";
	}
}

BOOL CWebEventData::ParseData( LPCTSTR lpData )
{
	return TRUE;
}

CString CWebEventData::GetDataStr()
{
	return "";
}

void CWebEventData::Clear()
{
	ClearData();
	m_sId.Empty();
	m_sWebId.Empty();
	m_dwWebType = 0;
	m_dwWebEvent = 0;
	m_sData.Empty();
	
}

CWebEventData::CWebEventData()
{
	m_dwWebEvent = 0;
	m_dwWebType = 0;
}

CWebEventData::~CWebEventData()
{
	Clear();
}

BOOL CWebEventData::CopyEventHeaderFrom( CWebEventData* pSrc )
{
	if( pSrc == NULL ) return FALSE;
	m_sId = pSrc->m_sId;
	m_sWebId = pSrc->m_sWebId;
	m_dwWebType = pSrc->m_dwWebType;
	m_dwWebEvent = pSrc->m_dwWebEvent;
	return TRUE;
}

void CWebEventData::ClearData()
{

}
