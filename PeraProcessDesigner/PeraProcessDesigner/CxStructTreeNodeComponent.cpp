#include "StdAfx.h"
#include "CxStructTreeNodeComponent.h"

#include "value.h"
#include "reader.h"
#include "writer.h"
#include <CxStructTreeNodeParam.h>
#include <CxStructTree.h>
#include "PeraProcessDesigner.h"

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

CCxStructTreeNodeComponent::CCxStructTreeNodeComponent( CCxStructTree * pTree )
	: CCxStructTreeNode( pTree )
{
	m_NodeType = NT_COMPONENT;
	m_bRunFlag = FALSE;
	m_bIsPopForm = TRUE;
}

CCxStructTreeNodeComponent::~CCxStructTreeNodeComponent()
{

}

void CCxStructTreeNodeComponent::Clear( void )
{
	m_strAuthor = "";
	m_strDescription = "";
	m_strCreated = "";
	m_strVersion = "";

	m_strRobotType = "";
	m_strLocalPath = "";
	m_bIsPopForm = TRUE;
	CCxStructTreeNode::Clear();
}

BOOL CCxStructTreeNodeComponent::OnParsing( Json::Value& _jsonObj )
{
	if ( !CCxStructTreeNode::OnParsing( _jsonObj ) ) return FALSE;

//	Json::Value obj;
//	if ( !ConvertJsonObj( obj, lpJsonObj ) ) return FALSE;
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

		m_strLocalPath = (_jsonObj["localPath"].isString()) ? _jsonObj["localPath"].asCString() : _T("");
		m_strRobotType = (_jsonObj["robotType"].isString()) ? _jsonObj["robotType"].asCString() : _T("");

		str = (_jsonObj["runFlag"].isString()) ? _jsonObj["runFlag"].asCString() : _T("");
		m_bRunFlag = ( !str.IsEmpty() && str.CompareNoCase( "true" ) == 0 );

		if ( !_jsonObj["linkToParamIds"].isNull() && _jsonObj["linkToParamIds"].isArray() )
		{
			Json::Value arrLinkToIds = _jsonObj["linkToParamIds"];
			Json::FastWriter fast_writer;
			CString strLinkToIds = fast_writer.write( arrLinkToIds ).c_str();
			CString * pStrLinkToIds = new CString();
			*pStrLinkToIds = strLinkToIds;
			theApp.m_pMainWnd->PostMessage( WM_UPDATE_COMPONENT_BY_LINKER, 0, (LPARAM)pStrLinkToIds );
		}
	}
	catch (...)
	{
		theApp.m_pMainWnd->PostMessage( WM_UPDATE_COMPONENT_BY_LINKER, 0, (LPARAM)NULL );
		return FALSE;
	}
	return TRUE;
}

CString CCxStructTreeNodeComponent::GetTooltipText( void )
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

CString CCxStructTreeNodeComponent::GetUpdatePropJsonStr( void )
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

BOOL CCxStructTreeNodeComponent::CanRun( void )
{
	CCxStructTreeNode * pNode = NULL;
	CCxStructTreeNodeParam * pParamNode = NULL;

	for ( int i=0; i<(int)m_vChildren.size(); i++ )
	{
		pNode = m_vChildren[i];
		if ( pNode->IsGroupNode() )
		{
			if ( !((CCxStructTreeNodeGroup*)pNode)->CanRun() )
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

BOOL CCxStructTreeNodeComponent::IsLoopLinker( void )
{
	CCxStructTreeNode * pNode = NULL;
	CCxStructTreeNodeParam * pParamNode = NULL;
	CCxStructTreeNodeGroup * pGroupNode = NULL;
	for (int i=0; i<(int)m_vChildren.size(); i++)
	{
		pNode = m_vChildren[i];
		if ( !pNode->IsGroupNode() ) continue;
		pGroupNode = (CCxStructTreeNodeGroup *)pNode;

		for (int i1=0; i1<(int)pGroupNode->m_vChildren.size(); i1++)
		{
			pNode = pGroupNode->m_vChildren[i1];
			if ( !pNode->IsParamNode() ) continue;
			pParamNode = (CCxStructTreeNodeParam *)pNode;
			if ( pParamNode->IsLoopLinker() )
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

void CCxStructTreeNodeComponent::OnAfterRunCompleted( void )
{
	if ( m_vChildren.size() < 1 ) return;
	if ( !m_vChildren[0]->IsGroupNode() ) return;

	vector< CCxStructTreeNodeParam* > vParams;
	((CCxStructTreeNodeGroup*)m_vChildren[0])->GetParamNodes( vParams );
	if ( vParams.size() == 0 ) return;

	CCxStructTreeNode * pNode = NULL;
	CCxStructTreeNodeParam * pParamNode = NULL;

	for (int i=0; i<(int)vParams.size(); i++)
	{
		pNode = vParams[i];
		if ( !pNode->IsParamNode() ) continue;
		pParamNode = (CCxStructTreeNodeParam *)pNode;
		pParamNode->SetStatus( TRUE, TRUE );
		pParamNode->OnAfterRunCompleted();
	}

	//CCxStructTreeNode * pNode = NULL;
	//CCxStructTreeNodeParam * pParamNode = NULL;
	//for (int i=0; i<(int)m_vChildren.size(); i++)
	//{
	//	pNode = m_vChildren[i];
	//	if ( !pNode->IsParamNode() ) continue;
	//	pParamNode = (CCxStructTreeNodeParam *)pNode;
	//	pParamNode->SetStatus( TRUE, TRUE );
	//	pParamNode->OnAfterRunCompleted();
	//}
}