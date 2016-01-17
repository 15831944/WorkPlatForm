#include "StdAfx.h"
#include "CxStructTreeNodeGroup.h"
#include <CxStructTreeNodeComponent.h>
#include "value.h"
#include "reader.h"
#include "writer.h"
#include "CxStructTree.h"
#include <CxStructTreeNodeParam.h>


CCxStructTreeNodeGroup::CCxStructTreeNodeGroup( CCxStructTree * pTree )
	: CCxStructTreeNode( pTree )
{
	m_NodeType = NT_GROUP;
}

CCxStructTreeNodeGroup::~CCxStructTreeNodeGroup()
{

}

void CCxStructTreeNodeGroup::Clear( void )
{
	CCxStructTreeNode::Clear();
}

BOOL CCxStructTreeNodeGroup::OnParsing( Json::Value& _jsonObj )
{
	if ( !CCxStructTreeNode::OnParsing( _jsonObj ) ) return FALSE;

	//Json::Value obj;
	//if ( !ConvertJsonObj( obj, lpJsonObj ) ) return FALSE;
	CString str;

	try
	{
		//if ( m_NodeType == NT_ROOT )
		//{
		//	m_pTree->m_strCurSelId = (_jsonObj["curSelNodeId"].isString()) ? _jsonObj["curSelNodeId"].asCString() : _T("");
		//}
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}

CString CCxStructTreeNodeGroup::GetTooltipText( void )
{
	CString strTip;
	//strTip.Format( "名称：%s\r\n作者：%s\r\n时间：%s\r\n版本：%s\r\n描述：%s"
	//	, m_strOriName
	//	, m_strAuthor
	//	, m_strCreated
	//	, m_strVersion
	//	, m_strDescription);
	return strTip;
}

int CCxStructTreeNodeGroup::GetParamNodes( vector< CCxStructTreeNodeParam* >& vParams )
{
	for (int i=0; i<(int)m_vChildren.size(); i++)
	{
		if ( m_vChildren[i]->IsGroupNode() )
		{
			((CCxStructTreeNodeGroup*)m_vChildren[i])->GetParamNodes( vParams );
		}
		else if ( m_vChildren[i]->IsParamNode() )
		{
			vParams.push_back( (CCxStructTreeNodeParam*)m_vChildren[i] );
		}
		else
		{

		}
	}
	return (int)vParams.size();
}

BOOL CCxStructTreeNodeGroup::CanRun( void )
{
	if ( m_pParent == NULL )
	{
		return FALSE;
	}
	if ( !m_pParent->IsComponentNode() &&
		!m_pParent->IsRootNode() &&
		!m_pParent->IsSubFlowNode() )
	{
		return FALSE;
	}
	CCxStructTreeNode * pNode = NULL;
	CCxStructTreeNodeParam * pParamNode = NULL;
	//
	for ( int i=0; i<(int)m_vChildren.size(); i++ )
	{
		pNode = m_vChildren[i];
		if ( !pNode->IsParamNode() ) continue;
		pParamNode = (CCxStructTreeNodeParam *)pNode;
		if ( !pParamNode->CanRun() )
		{
			return FALSE;
		}
		if ( pParamNode->IsLoopLinker() )
		{
			return FALSE;
		}
	}
	return TRUE;
}
