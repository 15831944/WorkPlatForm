#include "StdAfx.h"
#include "PeraWebPageEventExData.h"

#include "value.h"
#include "reader.h"
#include "writer.h"

const LPCTSTR CWPERunFlowCompleted::S_STOP = "4";
const LPCTSTR CWPERunFlowCompleted::S_COMPLETED = "8";

//4:"已停止",
//   		8:"已完成",
//           1:"执行中",
//           0:"未启动",
//           '':"未启动", h黄圆圆（7851） 13:58:22


//////////////////////////////////////////////////////////////////////////

CWPERunFlowStart::CWPERunFlowStart( void )
{
}

CWPERunFlowStart::~CWPERunFlowStart( void )
{
	Clear();
}

void CWPERunFlowStart::Clear( void )
{
	m_strUrl = "";
	m_strFlowId = "";
}

BOOL CWPERunFlowStart::_OnGetCopy( CCxWebPageEventExData *& pDest )
{
	CWPERunFlowStart * pOther = ( CWPERunFlowStart* )pDest;
	pOther->m_strUrl = m_strUrl;
	pOther->m_strFlowId = m_strFlowId;
	return TRUE;
}

BOOL CWPERunFlowStart::OnParseJsonObj( LPVOID lpJsonObj )
{
	if ( !CanParseJsonObjPtr( lpJsonObj ) ) return FALSE;
	Json::Value& obj = *reinterpret_cast< Json::Value* >( lpJsonObj );
	try
	{
		m_strUrl = ( obj["url"].isString()) ? obj["url"].asCString() : _T("");
		m_strFlowId = ( obj["flowId"].isString()) ? obj["flowId"].asCString() : _T("");
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CWPERunFlowStart::OnGetJsonStr( LPVOID lpJsonObj )
{
	if ( !CanParseJsonObjPtr( lpJsonObj ) ) return FALSE;
	Json::Value& obj = *reinterpret_cast< Json::Value* >( lpJsonObj );
	Json::Value objData( Json::objectValue );
	objData["url"] = Json::Value( (LPCTSTR)m_strUrl );
	objData["flowId"] = Json::Value( (LPCTSTR)m_strFlowId );
	obj["data"] = objData;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CWPERunFlowCompleted::CWPERunFlowCompleted()
{
}

CWPERunFlowCompleted::~CWPERunFlowCompleted()
{
	Clear();
}

void CWPERunFlowCompleted::Clear()
{
	m_strStatus = "";
}

BOOL CWPERunFlowCompleted::_OnGetCopy( CCxWebPageEventExData *& pDest )
{
	CWPERunFlowCompleted * pOther = ( CWPERunFlowCompleted* )pDest;
	pOther->m_strStatus = m_strStatus;
	return TRUE;
}

BOOL CWPERunFlowCompleted::OnParseJsonObj( LPVOID lpJsonObj )
{
	if ( !CanParseJsonObjPtr( lpJsonObj ) ) return FALSE;
	Json::Value& obj = *reinterpret_cast< Json::Value* >( lpJsonObj );
	try
	{
		m_strStatus = (obj["status"].isString()) ? obj["status"].asCString() : _T("");
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CWPERunFlowCompleted::OnGetJsonStr( LPVOID lpJsonObj )
{
	if ( !CanParseJsonObjPtr( lpJsonObj ) ) return FALSE;
	Json::Value& obj = *reinterpret_cast< Json::Value* >( lpJsonObj );
	Json::Value objData( Json::objectValue );
	objData["status"] = Json::Value( (LPCTSTR)m_strStatus );
	obj["data"] = objData;
	return TRUE;
}


