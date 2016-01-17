#include "StdAfx.h"
#include "CxStructTreeNodeSubFlow.h"
#include <CxStructTreeNodeComponent.h>
#include "value.h"
#include "reader.h"
#include "writer.h"
#include "CxStructTree.h"
#include <CxStructTreeNodeParam.h>

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


CCxStructTreeNodeSubFlow::CCxStructTreeNodeSubFlow( CCxStructTree * pTree )
	: CCxStructTreeNode( pTree )
{
	m_bRunFlag = FALSE;
	m_bIsPopForm = TRUE;
}

CCxStructTreeNodeSubFlow::~CCxStructTreeNodeSubFlow()
{

}

void CCxStructTreeNodeSubFlow::Clear( void )
{
	m_strAuthor = "";
	m_strDescription = "";
	m_strCreated = "";
	m_strVersion = "";
	m_strPkgId = "";
	m_bIsPopForm = TRUE;
	CCxStructTreeNode::Clear();
}

BOOL CCxStructTreeNodeSubFlow::OnParsing( Json::Value& _jsonObj )
{
	if ( !CCxStructTreeNode::OnParsing( _jsonObj ) ) return FALSE;

	//Json::Value obj;
	//if ( !ConvertJsonObj( obj, lpJsonObj ) ) return FALSE;
	CString str;

	try
	{
		str = (_jsonObj["isPopForm"].isString()) ? _jsonObj["isPopForm"].asCString() : _T("TRUE"); //无isPopForm属性时默认为true
		if (str.CompareNoCase("true")==0)
			m_bIsPopForm = TRUE;
		else
			m_bIsPopForm = FALSE;

		m_strDescription = (_jsonObj["Description"].isString()) ? _jsonObj["Description"].asCString() : _T("");
		m_strAuthor = (_jsonObj["Author"].isString()) ? _jsonObj["Author"].asCString() : _T("");
		m_strCreated = (_jsonObj["Created"].isString()) ? _jsonObj["Created"].asCString() : _T("");
		m_strVersion = (_jsonObj["Version"].isString()) ? _jsonObj["Version"].asCString() : _T("");
		m_strPkgId = (_jsonObj["pkgId"].isString()) ? _jsonObj["pkgId"].asCString() : _T("");
		str = (_jsonObj["runFlag"].isString()) ? _jsonObj["runFlag"].asCString() : _T("");
		m_bRunFlag = ( !str.IsEmpty() && str.CompareNoCase( "true" ) == 0 );
		if ( m_NodeType == NT_ROOT )
		{
			m_pTree->m_strCurSelId = (_jsonObj["curSelNodeId"].isString()) ? _jsonObj["curSelNodeId"].asCString() : _T("");
		}
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}

CString CCxStructTreeNodeSubFlow::GetTooltipText( void )
{
	CString strTip;
	strTip.Format( "名称：%s\r\n作者：%s\r\n时间：%s\r\n版本：%s\r\n描述：%s"
		, m_strOriName
		, m_strAuthor
		, m_strCreated
		, m_strVersion
		, m_strDescription);
	return strTip;
}

CString CCxStructTreeNodeSubFlow::GetUpdatePropJsonStr( void )
{
	try
	{
		CString strResult;
		Json::Value obj;
		obj["id"] = Json::Value( (LPCTSTR)m_strID );
		obj["name"] = Json::Value( (LPCTSTR)m_strOriName );
		obj["Author"] = Json::Value( (LPCTSTR)m_strAuthor );
		obj["Description"] = Json::Value( (LPCTSTR)m_strDescription );
		obj["Created"] = Json::Value( (LPCTSTR)m_strCreated );
		obj["Version"] = Json::Value( (LPCTSTR)m_strVersion );

		Json::FastWriter fast_writer;
		strResult = fast_writer.write( obj ).c_str();
		return strResult;
	}
	catch (...)
	{
		return "";
	}
}

/*
如果是根节点
*/
BOOL CCxStructTreeNodeSubFlow::CanRun()
{
	if ( IsRootNode() ) return TRUE;

	CCxStructTreeNode * pNode = NULL;
	CCxStructTreeNodeParam * pParamNode = NULL;
	vector< CCxStructTreeNodeParam* > vSpecNodes;

	for ( int i=0; i<(int)m_vChildren.size(); i++ )
	{
		pNode = m_vChildren[i];
		if ( pNode->IsComponentNode() )
		{
			if ( ((CCxStructTreeNodeComponent*)pNode)->IsLoopLinker() )
			{
				return FALSE;
			}
		}
		else if ( pNode->IsGroupNode() )
		{
			if ( !((CCxStructTreeNodeGroup*)pNode)->CanRun() )
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

void CCxStructTreeNodeSubFlow::OnAfterRunCompleted( void )
{
	CCxStructTreeNode* pNode = NULL;
	CCxStructTreeNodeParam* pParamNode = NULL;

	for ( int i=0; i<(int)m_vChildren.size(); i++ )
	{
		pNode = m_vChildren[i];
		if ( pNode->IsParamNode() )
		{
			pParamNode = (CCxStructTreeNodeParam *)pNode;
			pParamNode->SetStatus( TRUE );
			pParamNode->OnAfterRunCompleted();
		}
		else if ( pNode->IsComponentNode() )
		{
			((CCxStructTreeNodeComponent*)pNode)->OnAfterRunCompleted();
		}
		else if ( pNode->IsFlowNode() )
		{
			((CCxStructTreeNodeSubFlow*)pNode)->OnAfterRunCompleted();
		}
	}
}