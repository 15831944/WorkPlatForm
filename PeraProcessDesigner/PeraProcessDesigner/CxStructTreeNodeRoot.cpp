#include "StdAfx.h"
#include "CxStructTreeNodeRoot.h"

#include "value.h"
#include "reader.h"
#include "writer.h"

//static BOOL ConvertJsonObj( Json::Value& obj, LPVOID lpJsonObj )
//{
//	Json::Value NewObj;
//	obj = NewObj;
//	Json::Value * pObj = reinterpret_cast< Json::Value * > ( lpJsonObj );
//	if ( pObj == NULL ) return FALSE;
//	obj = *pObj;
//	if ( !obj.isObject() ) return FALSE;
//	return TRUE;
//}


CCxStructTreeNodeRoot::CCxStructTreeNodeRoot( CCxStructTree * pTree )
	: CCxStructTreeNodeFlow( pTree )
{

}

CCxStructTreeNodeRoot::~CCxStructTreeNodeRoot()
{

}

void CCxStructTreeNodeRoot::Clear( void )
{
	m_strSelNodeId = "";
	CCxStructTreeNodeFlow::Clear();
}

BOOL CCxStructTreeNodeRoot::OnParsing( Json::Value& _jsonObj )
{
	if ( !CCxStructTreeNodeFlow::OnParsing( _jsonObj ) ) return FALSE;

//	Json::Value obj;
//	if ( !ConvertJsonObj( obj, lpJsonObj ) ) return FALSE;
	CString str;

	try
	{
		m_strSelNodeId = (_jsonObj["curSelNodeId"].isString()) ? _jsonObj["curSelNodeId"].asCString() : _T("");
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
