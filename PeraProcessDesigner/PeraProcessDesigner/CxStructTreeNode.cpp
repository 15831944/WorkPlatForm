#include "StdAfx.h"
#include "CxStructTreeNode.h"

#include "ConvertData.h"
#include "PtrVectorHeader.h"

#include "reader.h"
#include "writer.h"

#include "CxArrayND.h"
#include "CxStructTreeGridT.h"
#include "PeraProcessDesignerUtility.h"
#include "PeraProcessDesigner.h"
#include <Tools.h>
#include "CxStructTreeNodeParam.h"
#include "CxStructTreeNodeForm.h"
#include "CxStructTreeNodeRoot.h"
#include "CxStructTreeNodeGroup.h"
#include "CxStructTreeNodeSubFlow.h"



//static UINT g_nArrayParamNewId = 0; //用于生成数组参数节点的id

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

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//vector< CCxStructTreeNode* > CCxStructTreeNode::m_vCacheSetValue;
//vector< CCxStructTreeNode* > CCxStructTreeNode::m_vCacheUpdateStatus;

CCxStructTreeNode::CCxStructTreeNode( CCxStructTree * pTree )
{
	m_pParent = NULL;
	m_NodeType = NT_NULL;
//	m_bModified = FALSE;
	m_nImageID = -1;
	m_pRow = NULL;
	m_nIndex = -1;
	m_pTree = pTree;
}

CCxStructTreeNode::~CCxStructTreeNode(void)
{
	Clear();
	m_pTree = NULL;
}

void CCxStructTreeNode::Clear( void )
{
	PtrVectorClear( m_vChildren );

	if ( m_pTree != NULL )
	{
		m_pTree->RemoveNodeFromMap( this );
	}

	m_dwStatus = 0;
	m_strID = "";
	m_strParentID = "";
	m_pParent = NULL;
	m_NodeType = NT_NULL;
	m_strOriName = "";
	m_strText = "";
//	m_bModified = FALSE;
	m_nImageID = -1;
	m_pRow = NULL;
	m_nIndex = -1;
}

CCxStructTreeNode * CCxStructTreeNode::GetCopied( void )
{
	CCxStructTreeNode * pNewNode = GetNewNodeByType( m_NodeType, m_pTree );
	if ( pNewNode == NULL ) return NULL;
	OnGetCopied( pNewNode );
	return pNewNode;
}

void CCxStructTreeNode::OnGetCopied( CCxStructTreeNode * pNewNode )
{
	if ( pNewNode == NULL ) return;

	pNewNode->m_dwStatus = m_dwStatus;
	pNewNode->m_strID = m_strID;
	pNewNode->m_strParentID = m_strParentID;
	pNewNode->m_pParent = m_pParent;
	pNewNode->m_NodeType = m_NodeType;
	pNewNode->m_strOriName = m_strOriName;
	pNewNode->m_strText = m_strText;
//	pNewNode->m_bModified = m_bModified;
	pNewNode->m_nImageID = m_nImageID;
	pNewNode->m_pRow = m_pRow;
	pNewNode->m_nIndex = m_nIndex;
//	pNewNode->m_vChildren = m_vChildren;
}

BOOL CCxStructTreeNode::Copy( CCxStructTreeNode * pSrc )
{
	if ( pSrc == NULL ) return FALSE;
	pSrc->OnGetCopied( this );
	return TRUE;
}

//BOOL CCxStructTreeNode::CheckStatusFlag( DWORD dwFlag )
//{
//	for ( int i=0; i<(int)m_vChildren.size(); i++ )
//	{
//		if ( m_vChildren[i]->CheckStatusFlag( dwFlag ) ) return TRUE;
//	}
//	return FALSE;
//}
//
//void CCxStructTreeNode::ClearStatusFlag( DWORD dwFlag )
//{
//	for ( int i=0; i<(int)m_vChildren.size(); i++ )
//	{
//		m_vChildren[i]->ClearStatusFlag( dwFlag );
//	}
//}
//
//void CCxStructTreeNode::SetStatusFlag( DWORD dwFlag )
//{
//	if ( (dwFlag & NS_RUN_COMPLETED) == NS_RUN_COMPLETED )
//	{
//		for ( int i=0; i<(int)m_vChildren.size(); i++ )
//		{
//			m_vChildren[i]->SetStatusFlag( dwFlag );
//		}
//		m_dwStatus |= NS_RUN_COMPLETED;
//	}
//}

int CCxStructTreeNode::ConvertNodeType( LPCTSTR lpszType )
{
	CString str = lpszType == NULL ? "" : lpszType;
	if ( str.CompareNoCase("Root") == 0 ) return NT_ROOT;
	else if ( str.CompareNoCase("ComponentNode") == 0 ) return NT_COMPONENT;
	else if ( str.CompareNoCase("Param") == 0 ) return NT_PARAM;
	else if ( str.CompareNoCase("ParallelNode") == 0 ) return NT_PARALLEL;
	else if ( str.CompareNoCase("SequenceNode") == 0 ) return NT_SEQUENCE;
	else if ( str.CompareNoCase("IfNode") == 0 ) return NT_IF;
	else if ( str.CompareNoCase("ForEachNode") == 0 ) return NT_FOR;
	else if ( str.CompareNoCase("PreForm") == 0 ) return NT_FRONT_FORM;
	else if ( str.CompareNoCase("PostForm") == 0 ) return NT_REAR_FORM;
	else if ( str.CompareNoCase("EmptyNode") == 0 ) return NT_EMPTY;
	else if ( str.CompareNoCase("Group") == 0 ) return NT_GROUP;
	else if ( str.CompareNoCase("SubflowNode") == 0 ) return NT_SUBFLOW;
	else return NT_NULL;
}

CString CCxStructTreeNode::ConvertNodeType( int nType )
{
	switch ( nType )
	{
	case NT_ROOT:
		return "Root";
	case NT_COMPONENT:
		return "ComponentNode";
	case NT_PARAM:
		return "Param";
	case NT_PARALLEL:
		return "ParallelNode";
	case NT_SEQUENCE:
		return "SequenceNode";
	case NT_IF:
		return "IfNode";
	case NT_FOR:
		return "ForEachNode";
	case NT_FRONT_FORM:
		return "PreForm";
	case NT_REAR_FORM:
		return "PostForm";
	case NT_GROUP:
		return "Group";
	case NT_EMPTY:
		return "EmptyNode";
	case NT_SUBFLOW:
		return "SubflowNode";
	default:
		return "";
	}
}

int CCxStructTreeNode::ConvertParamDataType( LPCTSTR lpszType )
{
	CString str = lpszType == NULL ? "" : lpszType;
	if ( str.CompareNoCase("String") == 0 ) return DT_STRING;
	else if ( str.CompareNoCase("Double") == 0 ) return DT_DOUBLE;
	else if ( str.CompareNoCase("Boolean") == 0 ) return DT_BOOLEAN;
	else if ( str.CompareNoCase("Int") == 0 ) return DT_INT;
	else if ( str.CompareNoCase("File") == 0 ) return DT_FILE;

	else if ( str.CompareNoCase("StringArray") == 0 ) return DT_ARRAY1D_STRING;
	else if ( str.CompareNoCase("String2Darray") == 0 ) return DT_ARRAY2D_STRING;
	else if ( str.CompareNoCase("String3Darray") == 0 ) return DT_ARRAY3D_STRING;
	
	else if ( str.CompareNoCase("DoubleArray") == 0 ) return DT_ARRAY1D_DOUBLE;
	else if ( str.CompareNoCase("Double2Darray") == 0 ) return DT_ARRAY2D_DOUBLE;
	else if ( str.CompareNoCase("Double3Darray") == 0 ) return DT_ARRAY3D_DOUBLE;

	else if ( str.CompareNoCase("BooleanArray") == 0 ) return DT_ARRAY1D_BOOLEAN;
	else if ( str.CompareNoCase("Boolean2Darray") == 0 ) return DT_ARRAY2D_BOOLEAN;
	else if ( str.CompareNoCase("Boolean3Darray") == 0 ) return DT_ARRAY3D_BOOLEAN;

	else if ( str.CompareNoCase("IntArray") == 0 ) return DT_ARRAY1D_INT;
	else if ( str.CompareNoCase("Int2Darray") == 0 ) return DT_ARRAY2D_INT;
	else if ( str.CompareNoCase("Int3Darray") == 0 ) return DT_ARRAY3D_INT;
	else return DT_NULL;
}

CString CCxStructTreeNode::ConvertParamDataType( int nType )
{
	switch ( nType )
	{
	case DT_STRING:
		return "String";
	case DT_DOUBLE:
		return "Double";
	case DT_BOOLEAN:
		return "Boolean";
	case DT_INT:
		return "Int";
	case DT_FILE:
		return "File";

	case DT_ARRAY1D_STRING:
		return "StringArray";
	case DT_ARRAY2D_STRING:
		return "String2Darray";
	case DT_ARRAY3D_STRING:
		return "String3Darray";

	case DT_ARRAY1D_DOUBLE:
		return "DoubleArray";
	case DT_ARRAY2D_DOUBLE:
		return "Double2Darray";
	case DT_ARRAY3D_DOUBLE:
		return "Double3Darray";

	case DT_ARRAY1D_BOOLEAN:
		return "BooleanArray";
	case DT_ARRAY2D_BOOLEAN:
		return "Boolean2Darray";
	case DT_ARRAY3D_BOOLEAN:
		return "Boolean3Darray";

	case DT_ARRAY1D_INT:
		return "IntArray";
	case DT_ARRAY2D_INT:
		return "Int2Darray";
	case DT_ARRAY3D_INT:
		return "Int3Darray";

	default:
		return "";
	}
}

int CCxStructTreeNode::ConvertParamDir( LPCTSTR lpszDir )
{
	CString str = lpszDir == NULL ? "" : lpszDir;
	if ( str.CompareNoCase("in") == 0 ) return PARAM_DIR_IN;
	else if ( str.CompareNoCase("out") == 0 ) return PARAM_DIR_OUT;
	else if ( str.CompareNoCase("inout") == 0 ) return PARAM_DIR_INOUT;
	else return PARAM_DIR_NULL;
}

CString CCxStructTreeNode::ConvertParamDir( int nDir )
{
	switch ( nDir )
	{
	case PARAM_DIR_IN:
		return "in";
	case PARAM_DIR_OUT:
		return "out";
	case PARAM_DIR_INOUT:
		return "inout";
	default:
		return "";
	}
}

CCxStructTreeNode * CCxStructTreeNode::GetNewNodeByType( int nNodeType, CCxStructTree * pTree )
{
	if ( pTree == NULL ) return NULL;
	CCxStructTreeNode * pNode = NULL;
	switch ( nNodeType )
	{
	case CCxStructTreeNode::NT_ROOT:
		pNode = new CCxStructTreeNodeRoot( pTree ); break;
	case CCxStructTreeNode::NT_PARALLEL:
	case CCxStructTreeNode::NT_SEQUENCE:
	case CCxStructTreeNode::NT_IF:
	case CCxStructTreeNode::NT_FOR:
		pNode = new CCxStructTreeNodeFlow( pTree ); break;
	case CCxStructTreeNode::NT_COMPONENT:
		pNode = new CCxStructTreeNodeComponent( pTree ); break;
	case CCxStructTreeNode::NT_GROUP:
		pNode = new CCxStructTreeNodeGroup( pTree ); break;
	case CCxStructTreeNode::NT_FRONT_FORM:
	case CCxStructTreeNode::NT_REAR_FORM:
		pNode = new CCxStructTreeNodeForm( pTree ); break;
	case CCxStructTreeNode::NT_PARAM:
		pNode = new CCxStructTreeNodeParam( pTree ); break;
	case CCxStructTreeNode::NT_EMPTY:
		pNode = new CCxStructTreeNode( pTree ); break;
	case CCxStructTreeNode::NT_SUBFLOW:
		pNode = new CCxStructTreeNodeSubFlow( pTree ); break;
	default:
		//节点类型不存在，无法创建
		return NULL;
	}

	pNode->m_NodeType = nNodeType;
	return pNode;
}

CCxStructTreeNode * CCxStructTreeNode::GetCanSelectedNode( void )
{
	CCxStructTreeNode * pParent = NULL;
	switch ( m_NodeType )
	{
	case CCxStructTreeNode::NT_PARAM_ARRAY:
		if (m_pParent == NULL) return NULL;
		else return m_pParent->GetCanSelectedNode();
	default:
		return this;
	}
}

//CCxStructTreeNode * CCxStructTreeNode::Find( LPCTSTR lpszId )
//{
//	if ( lpszId == NULL ) return NULL;
//
//	if ( m_strID.CompareNoCase( lpszId ) == 0 )
//	{
//		return this;
//	}
//
//	CCxStructTreeNode * pNode = NULL;
//	for (int i=0; i<(int)m_vChildren.size(); i++)
//	{
//		pNode = m_vChildren[i]->Find( lpszId );
//		if ( NULL != pNode ) return pNode;
//	}
//
//	return NULL;
//}

BOOL CCxStructTreeNode::ResetChildrenOrder( void )
{
	CCxStructTreeNode * pNode = NULL;
	//检查索引号是否越界
	for ( int i=0; i<(int)m_vChildren.size(); i++ )
	{
		if ( m_vChildren[i]->m_nIndex == -1 ) continue;

		if ( m_vChildren[i]->m_nIndex >= (int)m_vChildren.size() || m_vChildren[i]->m_nIndex < -1 )
		{
			::MessageBox(GetMainWnd(), "指定索引号越界", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION); return FALSE;
		}
	}
	//冒泡排序（由小到大），用 UINT -1 最大，0 最小
	for ( UINT i=0; i<(UINT)m_vChildren.size(); i++ )
	{
		for (UINT j=i; j<(UINT)m_vChildren.size() - 1; j++)
		{
			if (m_vChildren[i]->m_nIndex > m_vChildren[j + 1]->m_nIndex)
			{
				pNode = m_vChildren[j + 1];
				m_vChildren[j + 1] = m_vChildren[i];
				m_vChildren[i] = pNode;
			}
		}
	}
	return TRUE;
}

int CCxStructTreeNode::GetSiblingPos( void )
{
	if (m_pParent == NULL) return -1;

	vector< CCxStructTreeNode * >::iterator itr = find( m_pParent->m_vChildren.begin()
		, m_pParent->m_vChildren.end(), this );
	if ( itr == m_pParent->m_vChildren.end() ) return -1;
	else return ( itr - m_pParent->m_vChildren.begin() );
}

BOOL CCxStructTreeNode::Parse( Json::Value& _jsonObj, CCxStructTreeNode * pParseRoot
	, int nNodeType, CCxStructTreeNode * pParent )
{
	Clear();

	m_NodeType = nNodeType;
	m_strParentID = pParent == NULL ? "" : pParent->m_strID;
	m_pParent = pParent;

	//Json::Value obj;
	//if ( !ConvertJsonObj( obj, lpJsonObj ) ) return FALSE;
	//如果有children属性，必须为数组
	if ( !_jsonObj["children"].isNull() )
	{
		if ( !_jsonObj["children"].isArray() ) return FALSE;
	}

	if ( !OnParsing( _jsonObj ) )
	{
		Clear(); return FALSE;
	}

	if ( !_jsonObj["children"].isNull() )
	{
		Json::Value arrChildNode;
		CCxStructTreeNode * pChildNode = NULL;
		int nChildNodeType = NT_NULL;
		CString str;
	
		arrChildNode = _jsonObj["children"];
		//遍历子节点
		for ( Json::Value::iterator itrChild = arrChildNode.begin(); itrChild != arrChildNode.end(); ++itrChild)
		{
			str = ( (*itrChild)["nodeType"].isString()) ? (*itrChild)["nodeType"].asCString() : _T("");
			nChildNodeType = CCxStructTreeNode::ConvertNodeType( (LPCTSTR)str );
			pChildNode = CCxStructTreeNode::GetNewNodeByType( nChildNodeType, m_pTree );
			if ( pChildNode == NULL )
			{
				Clear(); return FALSE;
			}
			if ( !pChildNode->Parse( *itrChild
		//	if ( !pChildNode->Parse( reinterpret_cast< LPVOID > ( ( Json::Value * )&(*itrChild) )
				, pParseRoot, nChildNodeType, this ) )
			{
				delete pChildNode; pChildNode = NULL;
				Clear(); return FALSE;
			}

			m_vChildren.push_back( pChildNode );
		}

		ResetChildrenOrder();
	}

	if ( !OnParsed( pParseRoot ) )
	{
		Clear(); return FALSE;
	}

	return TRUE;
}

BOOL CCxStructTreeNode::OnParsing( Json::Value& _jsonObj )
{
	//Json::Value obj;
	//if ( !ConvertJsonObj( obj, lpJsonObj ) ) return FALSE;
	CString str;
	try
	{
		m_strID = (_jsonObj["id"].isString()) ? _jsonObj["id"].asCString() : _T("");
		m_strParentID = (_jsonObj["pid"].isString()) ? _jsonObj["pid"].asCString() : _T("");

		m_strOriName = (_jsonObj["name"].isString()) ? _jsonObj["name"].asCString() : _T("");
		m_strText = m_strOriName;

		str = (_jsonObj["index"].isString()) ? _jsonObj["index"].asCString() : _T("");
		if ( !str.IsEmpty() )
		{
			m_nIndex = strtol( (char *)(LPCTSTR)str, NULL, 10 );
		}
	}
	catch (...)
	{
		return FALSE;
	}

	if (
		m_pTree != NULL 
		&& m_NodeType != NT_NULL 
		&& m_NodeType != NT_EMPTY 
		&& m_NodeType != NT_PARAM_ARRAY 
		)
	{
		if ( !m_pTree->AddNodeToMap( this ) )
		{
			::MessageBox(GetMainWnd(), "节点已存在", g_lpszAppTitle, MB_OK|MB_ICONERROR);
			Clear(); return FALSE;
		}
	}

	return TRUE;
}


BOOL CCxStructTreeNode::OnParsed( CCxStructTreeNode * pParseRoot )
{
	return TRUE;
}

CString CCxStructTreeNode::GetTooltipText( void )
{
	return "";
}

CString CCxStructTreeNode::GetUpdatePropJsonStr( void )
{
	return "";
}

BOOL CCxStructTreeNode::IsComponentNode( void )
{
	return (m_NodeType == NT_COMPONENT);
}

BOOL CCxStructTreeNode::IsGroupNode( void )
{
	return (m_NodeType == NT_GROUP);
}

BOOL CCxStructTreeNode::IsRootNode( void )
{
	return (m_NodeType == NT_ROOT);
}

BOOL CCxStructTreeNode::IsSubFlowNode( void )
{
	return (m_NodeType == NT_SUBFLOW);
}

BOOL CCxStructTreeNode::IsFlowNode( void )
{
	return (
		m_NodeType == NT_PARALLEL
		|| m_NodeType == NT_SEQUENCE
		|| m_NodeType == NT_IF
		|| m_NodeType == NT_FOR
		|| m_NodeType == NT_SUBFLOW
		);
}

BOOL CCxStructTreeNode::IsParamNode( void )
{
	return (m_NodeType == NT_PARAM);
}

BOOL CCxStructTreeNode::IsRowMatched( void )
{
	if ( m_pRow == NULL )
	{
		return FALSE;//表格中未插入该行
	}
	CCxStructTreeNode * pRowNode = reinterpret_cast< CCxStructTreeNode* >( m_pRow->GetData() );
	if ( pRowNode != this )
	{
		::MessageBox(GetMainWnd(),"[节点数据]与对应的[行数据]不匹配", g_lpszAppTitle, MB_OK|MB_ICONERROR); return FALSE;
	}
	return TRUE;
}

void CCxStructTreeNode::AddToNodeMap( void )
{
	if ( m_pTree == NULL ) return;
	if (
		m_NodeType == NT_NULL 
		|| m_NodeType == NT_EMPTY 
		|| m_NodeType == NT_PARAM_ARRAY 
		) return;

	m_pTree->AddNodeToMap( this );

	if ( IsParamNode() )
	{
		if ( ((CCxStructTreeNodeParam*)this)->IsArray() ) return;
	}

	for (int i=0; i<(int)m_vChildren.size(); i++)
	{
		m_vChildren[i]->AddToNodeMap();
	}
}

//void CCxStructTreeNode::SetModified( BOOL bModified /*= TRUE */ )
//{
//	if ( m_pTree == NULL ) return;
////	if ( m_bModified == bModified ) return;
//	//设置修改状态
////	m_bModified = bModified;
//	
//	if ( IsParamNode() )
//	{
//		((CCxStructTreeNodeParam*)this)->UpdateStatus(TRUE);
//	}
//
//}

//void CCxStructTreeNode::UpdateStatus( BOOL bForward /*= FALSE*/ )
//{
//	for ( int i=0; i<(int)m_vChildren.size(); i++ )
//	{
//		m_vChildren[i]->UpdateStatus(bForward);
//	}
//}

CCxStructTreeNode * CCxStructTreeNode::GetChild( LPCTSTR lpszId )
{
	CString strId = lpszId == NULL ? "" : lpszId;
	for ( int i=0; i < (int)m_vChildren.size(); i++ )
	{
		if ( m_vChildren[i]->m_strID == strId )
		{
			return m_vChildren[i];
		}
	}
	return NULL;
}

//BOOL CCxStructTreeNode::CanRun()
//{
//	if ( !IsComponentNode() 
//		&& !IsFlowNode()
//		&& !IsRootNode() ) return FALSE;
//
//	return TRUE;
//}


void CCxStructTreeNode::ClearRowData( void )
{
	for (int i=0; i<(int)m_vChildren.size(); i++)
	{
		m_vChildren[i]->ClearRowData();
	}
	m_pRow = NULL;
}

BOOL CCxStructTreeNode::UpdateNode( LPCTSTR lpszData )
{
	return TRUE;
}
