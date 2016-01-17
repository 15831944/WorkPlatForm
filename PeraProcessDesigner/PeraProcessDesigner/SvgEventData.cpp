#include "StdAfx.h"
#include "SvgEventData.h"

#include "value.h"
#include "reader.h"
#include "writer.h"

#include <CxStructTree.h>
#include "PtrVectorHeader.h"
#include <PeraProcessDesigner.h>
#include <SyncDownUploadFunc.h>
#include <CxStructTreeNodeParam.h>
#include "DlgSvg.h"
#include "PeraProcessDesignerUtility.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


CCxStructTreeMoveEventData::CCxStructTreeMoveEventData()
{
	m_nSrcIndex = -1;
	m_nDestIndex = -1;
	m_pDestParent = NULL;
	m_pSrcParent = NULL;
	m_pSrcNode = NULL;
}

CCxStructTreeMoveEventData::~CCxStructTreeMoveEventData()
{
	Clear();
}

void CCxStructTreeMoveEventData::Clear()
{
	m_strSrcId = "";
	m_strDestParentId = "";
	m_nSrcIndex = -1;
	m_nDestIndex = -1;
	m_pDestParent = NULL;
	m_pSrcParent = NULL;
	m_pSrcNode = NULL;
}

BOOL CCxStructTreeMoveEventData::Parse( LPCTSTR lpszData, CCxStructTree* pTree  )
{
	Clear();

	CString strSrc = lpszData == NULL ? "" : lpszData;
	if (strSrc.IsEmpty()) return FALSE;

	Json::Reader reader;
	Json::Value obj;
	CString str;

	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) return FALSE;
		if (!obj.isObject()) return FALSE;

		m_strNameNew = (obj["name"].isString()) ? obj["name"].asCString() : _T("");
		if (m_strNameNew.IsEmpty())
		{
			::MessageBox( GetMainWnd(),"异常，Move目标节点名称不能为空", g_lpszAppTitle, MB_OK|MB_ICONERROR );
			return FALSE;
		}
		m_strSrcId = (obj["curId"].isString()) ? obj["curId"].asCString() : _T("");
		m_strDestParentId = (obj["newParentId"].isString()) ? obj["newParentId"].asCString() : _T("");
		str = (obj["newIndex"].isString()) ? obj["newIndex"].asCString() : _T("");
		//定位当前索引号和新索引号
		if (!str.IsEmpty())
		{
			m_nDestIndex = strtol((char *)(LPCTSTR)str, FALSE, 10);
		}
	}
	catch (...)
	{
		Clear();
		return FALSE;
	}

	return OnParsed( pTree );
}

BOOL CCxStructTreeMoveEventData::OnParsed( CCxStructTree* pTree )
{
	if ( pTree == NULL ) return FALSE;

	m_pSrcNode = pTree->Find( m_strSrcId );
	if ( m_pSrcNode == NULL ) return FALSE;
	if (!m_strNameNew.IsEmpty()) // 节点改名称了
	{
		m_pSrcNode->m_strOriName = m_strNameNew;
		m_pSrcNode->m_strText	 = m_strNameNew;
	}
	//原始父节点
	m_pSrcParent = m_pSrcNode->m_pParent;
	if ( m_pSrcParent == NULL ) return FALSE;
	//目的父节点
	m_pDestParent = pTree->Find( m_strDestParentId );
	if ( m_pDestParent == NULL ) return FALSE;
	//原始索引号
	m_nSrcIndex = m_pSrcNode->GetSiblingPos();
	if ( m_nSrcIndex == -1 ) return FALSE;
	//目的索引号
	m_nDestIndex;

	return TRUE;
}

BOOL CCxStructTreeMoveEventData::CanMove( void )
{
	if ( m_pSrcParent == NULL || m_pDestParent == NULL || m_pSrcNode == NULL ) return FALSE;
	//if ( !m_pSrcNode->IsRowMatched() 
	//	|| !m_pSrcParent->IsRowMatched() 
	//	|| !m_pDestParent->IsRowMatched() ) return FALSE;
	if ( m_nSrcIndex < 0 || m_nSrcIndex > (int)m_pSrcParent->m_vChildren.size() ) return FALSE;
	if ( m_nDestIndex != -1 && m_nDestIndex > (int)m_pDestParent->m_vChildren.size() ) return FALSE;
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CCxStructTreeTooltipData::CCxStructTreeTooltipData()
{
	m_bShow = FALSE;
	m_pNode = NULL;
}

CCxStructTreeTooltipData::~CCxStructTreeTooltipData()
{
	Clear();
}

void CCxStructTreeTooltipData::Clear()
{
	m_strId = "";
	m_bShow = FALSE;
	m_pNode = NULL;
}

BOOL CCxStructTreeTooltipData::Parse( LPCTSTR lpszData, CCxStructTree * pTree )
{
	Clear();

	CString strSrc = lpszData == NULL ? "" : lpszData;
	if (strSrc.IsEmpty()) return FALSE;

	Json::Reader reader;
	Json::Value obj;
	CString str;

	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) return FALSE;
		if (!obj.isObject()) return FALSE;

		m_strId = (obj["id"].isString()) ? obj["id"].asCString() : _T("");

		str = (obj["bShow"].isString()) ? obj["bShow"].asCString() : _T("");
		m_bShow = ( str.CompareNoCase("true") == 0 );

		if ( pTree != NULL )
		{
			m_pNode = pTree->Find( m_strId );
		}
	}
	catch (...)
	{
		Clear();
		return FALSE;
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CCxStructTreeRunResultParam::CCxStructTreeRunResultParam()
{
	m_pNode = NULL;
}

CCxStructTreeRunResultParam::~CCxStructTreeRunResultParam()
{
	Clear();
}

void CCxStructTreeRunResultParam::Clear( void )
{
	m_strId = "";
	m_strValue = "";
	m_pNode = NULL;
}

CCxStructTreeRunResultComponent::CCxStructTreeRunResultComponent()
{
	m_strId = "";
	m_strStatus = "";
	m_pNode = NULL;
}

CCxStructTreeRunResultComponent::~CCxStructTreeRunResultComponent()
{
	Clear();
}

void CCxStructTreeRunResultComponent::Clear( void )
{
	m_pNode = NULL;
}

CCxStructTreeRunResultFlow::CCxStructTreeRunResultFlow()
{
	m_pNode = NULL;
}

CCxStructTreeRunResultFlow::~CCxStructTreeRunResultFlow()
{
	Clear();
}

void CCxStructTreeRunResultFlow::Clear( void )
{
	m_strId = "";
	m_strStatus = "";
	m_pNode = NULL;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CCxStructTreeRunResult::CCxStructTreeRunResult()
{

}

CCxStructTreeRunResult::~CCxStructTreeRunResult()
{
	Clear();
}

void CCxStructTreeRunResult::Clear( void )
{
	PtrMapClear( m_mapRunReturn );
	PtrVectorClear( m_vFlows );
	PtrVectorClear( m_vComponents );
	m_strStatus = "";
}

//{ paramList:[{"id":"参数id1","value":"新值1"},{"id":"参数id2","value":"新值2"}], "status":"成功、失败、取消等标识代码" }
BOOL CCxStructTreeRunResult::Parse( LPCTSTR lpszData, CCxStructTree * pTree )
{
	Clear();
	CString strSrc = lpszData == NULL ? "" : lpszData;
	if (strSrc.IsEmpty()) return FALSE;

	Json::Reader reader;
	Json::Value obj, arrValue;
	CCxStructTreeRunResultParam * pValue = NULL;
	CCxStructTreeNode * pNode = NULL;
	STRUCTTREEMAP_PARAM_VALUE::iterator itrExistNode;
	CCxStructTreeRunResultParam * pExistValue = NULL;
	CCxStructTreeRunResultComponent * pComponent = NULL;
	CCxStructTreeRunResultFlow * pFlow = NULL;
	CString str;
	CDlgSvg* pSvg = GetSvgDilogPtr();
	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) return FALSE;
		if (!obj.isObject()) return FALSE;

		m_strStatus = ( obj["status"].isString()) ? obj["status"].asCString() : _T("");

		arrValue = obj["paramList"];
		if ( !arrValue.isNull() && arrValue.isArray() )
		{
			for ( Json::Value::iterator itr = arrValue.begin(); itr != arrValue.end(); ++itr)
			{
				pValue = new CCxStructTreeRunResultParam();
				pValue->m_strId = ( (*itr)["id"].isString()) ? (*itr)["id"].asCString() : _T("");
				pValue->m_strValue = ( (*itr)["value"].isString()) ? (*itr)["value"].asCString() : _T("");
				if ( pTree != NULL )
				{
					pNode = pTree->Find( pValue->m_strId );
					if ( pNode == NULL )
					{
						::MessageBox( GetMainWnd(),"异常，返回的参数Id，在结构树中不存在", g_lpszAppTitle, MB_OK|MB_ICONERROR );
						Clear(); return FALSE;
					}
					if ( !pNode->IsParamNode() )
					{
						::MessageBox( GetMainWnd(), "异常，返回的结果不是参数", g_lpszAppTitle, MB_OK|MB_ICONERROR );
						Clear(); return FALSE;
					}
					pValue->m_pNode = (CCxStructTreeNodeParam *)pNode;
				}

				itrExistNode = m_mapRunReturn.find( pValue->m_strId );
				if ( itrExistNode != m_mapRunReturn.end() )
				{
					pExistValue = itrExistNode->second;
					m_mapRunReturn.erase( itrExistNode );
					delete pExistValue; pExistValue = NULL;
				}
				m_mapRunReturn.insert( make_pair( pValue->m_strId, pValue ) );
			}
		}

		if ( !obj["flowIdList"].isNull() && obj["flowIdList"].isArray() )
		{
			arrValue = obj["flowIdList"];
			for ( Json::Value::iterator itr = arrValue.begin(); itr != arrValue.end(); ++itr)
			{
				pFlow = new CCxStructTreeRunResultFlow();
				str = ( (*itr)["id"].isString()) ? (*itr)["id"].asCString() : _T("");
				if ( pSvg != NULL )
				{
					pFlow->m_strId = pSvg->JsMethod_ConvertFlowId( str );
				}
				pFlow->m_strStatus = ( (*itr)["status"].isString()) ? (*itr)["status"].asCString() : _T("");
				if ( pTree != NULL )
				{
					pNode = pTree->Find( pFlow->m_strId );
					if ( pNode == NULL )
					{
						::MessageBox(GetMainWnd(), "异常，返回的流程Id，在结构树中不存在", g_lpszAppTitle, MB_OK|MB_ICONERROR );
						Clear(); return FALSE;
					}
					if ( !pNode->IsFlowNode() )
					{
						::MessageBox(GetMainWnd(), "异常，返回的流程Id与节点类型不匹配", g_lpszAppTitle, MB_OK|MB_ICONERROR );
						Clear(); return FALSE;
					}
					pFlow->m_pNode = (CCxStructTreeNodeFlow *)pNode;
				}
				if ( find( m_vFlows.begin(), m_vFlows.end(), pFlow )
					== m_vFlows.end() )
				{
					m_vFlows.push_back( pFlow );
				}
			}
		}

		if ( !obj["componentIdList"].isNull() && obj["componentIdList"].isArray() )
		{
			arrValue = obj["componentIdList"];
			for ( Json::Value::iterator itr = arrValue.begin(); itr != arrValue.end(); ++itr)
			{
				pComponent = new CCxStructTreeRunResultComponent();
				pComponent->m_strId = ( (*itr)["id"].isString()) ? (*itr)["id"].asCString() : _T("");
				pComponent->m_strStatus = ( (*itr)["status"].isString()) ? (*itr)["status"].asCString() : _T("");
				if ( pTree != NULL )
				{
					pNode = pTree->Find( pComponent->m_strId );
					if ( pNode == NULL )
					{
						::MessageBox(GetMainWnd(), "异常，返回的组件Id，在结构树中不存在", g_lpszAppTitle, MB_OK|MB_ICONERROR );
						Clear(); return FALSE;
					}
					if ( !pNode->IsComponentNode() )
					{
						::MessageBox(GetMainWnd(), "异常，返回的组件Id与节点类型不匹配", g_lpszAppTitle, MB_OK|MB_ICONERROR );
						Clear(); return FALSE;
					}
					pComponent->m_pNode = (CCxStructTreeNodeComponent *)pNode;
				}
				if ( find( m_vComponents.begin(), m_vComponents.end(), pComponent )
					== m_vComponents.end() )
				{
					m_vComponents.push_back( pComponent );
				}
			}
		}
	}
	catch (...)
	{
		Clear();
		return FALSE;
	}

	return TRUE;
}
//
//CString CCxStructTreeRunReturn::ConvertSyncSvgData( CCxStructTree * pTree )
//{
//	if ( pTree == NULL ) return "";
//	try
//	{
//		CCxStructTreeNode * pNode = NULL;
//		CString strResult, strId;
//		Json::Value arr( Json::arrayValue );
//		for ( STRUCTTREEMAP_PARAM_VALUE::iterator itr = m_mapRunReturn.begin();
//			itr != m_mapRunReturn.end(); ++itr )
//		{
//			pNode = pTree->Find( itr->first );
//			if ( pNode == NULL )
//			{
//				continue;
//			}
//
//			Json::Value obj;
//			obj["id"] = Json::Value( (LPCTSTR)pNode->m_strID );
//
//			strValue = ( (*itr)["value"].isString()) ? (*itr)["value"].asCString() : _T("");
//			m_mapRunReturn.insert( make_pair( strId, strValue ) );
//		}
//
//		obj["id"] = Json::Value( (LPCTSTR)m_strID );
//		obj["name"] = Json::Value( (LPCTSTR)m_strOriName );
//		obj["Author"] = Json::Value( (LPCTSTR)m_strAuthor );
//		obj["Description"] = Json::Value( (LPCTSTR)m_strDescription );
//		obj["Created"] = Json::Value( (LPCTSTR)m_strCreated );
//		obj["Version"] = Json::Value( (LPCTSTR)m_strVersion );
//
//		Json::FastWriter fast_writer;
//		strResult = fast_writer.write( obj ).c_str();
//		return strResult;
//	}
//	catch (...)
//	{
//		return "";
//	}
//}

BOOL CCxStructTreeRunResult::UpdateFileParam( void )
{
	CCxStructTreeNodeParam * pParamNode = NULL;
	for ( STRUCTTREEMAP_PARAM_VALUE::iterator itr = m_mapRunReturn.begin();
		itr != m_mapRunReturn.end(); ++itr )
	{
		pParamNode = itr->second->m_pNode;
		if ( pParamNode == NULL ) continue;
		if ( !pParamNode->IsFile() ) continue;

		//"value": "{ \"p_file_name\" : \"新建文本文档.txt\" , \"p_fileId\" : \"5279de1f860bb0736ffe8558\"}"

		CString strSrc = itr->second->m_strValue;
		if (strSrc.IsEmpty()) continue;

		Json::Reader reader;
		Json::Value obj;
		CString strFileName, strFileId, strDir;

		try
		{
			if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) continue;
			if (!obj.isObject()) continue;

			//strFileName = (obj["p_file_name"].isString()) ? obj["p_file_name"].asCString() : _T("");
			//strFileId = (obj["p_fileId"].isString()) ? obj["p_fileId"].asCString() : _T("");

			strFileName = (obj["fileName"].isString()) ? obj["fileName"].asCString() : _T("");
			strFileId = (obj["fileID"].isString()) ? obj["fileID"].asCString() : _T("");
		}
		catch (...)
		{
			continue;
		}

		if ( strFileId.IsEmpty() )
		{
			itr->second->m_strValue = "";
			continue;
		}

		strDir.Format("%s\\%s\\"
			, theApp.m_processMgr.m_sWorkPath
			, pParamNode->m_strID);

		CString strFilePath = DownloadHprose( strFileId, strDir, strFileName );
		if ( IsAccessibleFile( strFilePath ) )
		{
			itr->second->m_strValue = strFilePath;
		}
	}
	return TRUE;
}

CString CCxStructTreeRunResult::GetSvgResultJsonStr( void )
{
	CString strResult;
	try
	{
		Json::Value objResult( Json::objectValue );
		Json::Value arrParamList( Json::arrayValue );
		Json::Value arrFlowIdList( Json::arrayValue );
		Json::Value arrComponentIdList( Json::arrayValue );
		//参数
		for ( STRUCTTREEMAP_PARAM_VALUE::iterator itr = m_mapRunReturn.begin();
			itr != m_mapRunReturn.end(); ++itr )
		{
			if ( itr->second->m_pNode == NULL )
			{
				continue;
			}

			Json::Value obj;
			obj["id"] = Json::Value( (LPCTSTR)itr->second->m_pNode->m_strID );
			obj["value"] = Json::Value( (LPCTSTR)itr->second->m_strValue );
			obj["type"] = Json::Value( (LPCTSTR)itr->second->m_pNode->ConvertParamDataType( 
				itr->second->m_pNode->m_nDataType ) );
			arrParamList.append( obj );
		}
		objResult["paramList"] = arrParamList;

		//流程
		for ( vector<CCxStructTreeRunResultFlow*>::iterator itr = m_vFlows.begin();
			itr != m_vFlows.end(); ++itr )
		{
			Json::Value obj;
			obj["id"] = Json::Value( (LPCTSTR)(*itr)->m_strId );
			obj["status"] = Json::Value( (LPCTSTR)(*itr)->m_strStatus );
			arrFlowIdList.append( obj );
		}
		objResult["flowIdList"] = arrFlowIdList;
		
		//组件
		for ( vector<CCxStructTreeRunResultComponent*>::iterator itr = m_vComponents.begin();
			itr != m_vComponents.end(); ++itr )
		{
			Json::Value obj;
			obj["id"] = Json::Value( (LPCTSTR)(*itr)->m_strId );
			obj["status"] = Json::Value( (LPCTSTR)(*itr)->m_strStatus );
			arrComponentIdList.append( obj );
		}
		objResult["componentIdList"] = arrComponentIdList;

		Json::FastWriter fast_writer;
		strResult = fast_writer.write( objResult ).c_str();
		return strResult;
	}
	catch (...)
	{
		return "";
	}
}

//////////////////////////////////////////////////////////////////////////

CCxStructTreeUpdateParamsData::CCxStructTreeUpdateParamsData()
{

}

CCxStructTreeUpdateParamsData::~CCxStructTreeUpdateParamsData()
{
	Clear();
}

void CCxStructTreeUpdateParamsData::Clear()
{
	//m_nStartIndex = -1;
	//m_strFlowNodeId = "";
	PtrVectorClear( m_vGroups );
	PtrVectorClear( m_vParams );
	PtrVectorClear( m_vOperators );
	m_mapParamNode.clear();
	m_mapGroupNode.clear();
}

BOOL CCxStructTreeUpdateParamsData::Parse( LPCTSTR lpszData, CCxStructTree * pTree )
{
	Clear();
	CString strSrc = lpszData == NULL ? "" : lpszData;
	if (strSrc.IsEmpty()) return FALSE;
	CString str;

	Json::Reader reader;
	Json::Value obj( Json::objectValue ), arrValue( Json::arrayValue );
	//CCxStructTreeRunResultParam * pValue = NULL;
	CCxStructTreeNode * pNode = NULL;
	CCxStructTreeNodeGroup * pGroupNode = NULL;
	CCxStructTreeNodeParam * pParamNode = NULL;
	CCxStructTreeUpdateParamsOperatorData * pOperatorData = NULL;

	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) return FALSE;
		if (!obj.isObject()) return FALSE;

		//m_strFlowNodeId = ( obj["flowId"].isString()) ? obj["flowId"].asCString() : _T("");
		//str = ( obj["startIndex"].isString()) ? obj["startIndex"].asCString() : _T("");
		//if ( !str.IsEmpty() )
		//{
		//	m_nStartIndex = strtol( (char*)(LPCTSTR)str, NULL, 10 );
		//}

		if ( !obj["groupList"].isNull() && obj["groupList"].isArray() )
		{
			arrValue = obj["groupList"];

			if ( pTree != NULL )
			{
				pTree->m_bNodeParseing = TRUE;
				pTree->m_mapParseNodeCache.clear();
			}

			for ( Json::Value::iterator itr = arrValue.begin(); itr != arrValue.end(); ++itr)
			{
				str = ( (*itr)["nodeType"].isString() ) ? (*itr)["nodeType"].asCString() : _T("");
				int nNodeType = CCxStructTreeNode::ConvertNodeType( (LPCTSTR)str );
				pNode = CCxStructTreeNode::GetNewNodeByType( nNodeType, pTree );
				if ( pNode == NULL )
				{
					Clear();
					if ( pTree != NULL )
					{
						pTree->m_bNodeParseing = FALSE;
						pTree->m_mapParseNodeCache.clear();
					}
					return FALSE;
				}

				if ( !pNode->Parse( *itr/*reinterpret_cast< LPVOID > ( (Json::Value *)& (*itr) )*/
					, pNode, nNodeType, NULL ) )
				{
					delete pNode; pNode = NULL; Clear();
					if ( pTree != NULL )
					{
						pTree->m_bNodeParseing = FALSE;
						pTree->m_mapParseNodeCache.clear();
					}
					return FALSE;
				}

				if ( !pNode->IsGroupNode() )
				{
					delete pNode; pNode = NULL; Clear();
					if ( pTree != NULL )
					{
						pTree->m_bNodeParseing = FALSE;
						pTree->m_mapParseNodeCache.clear();
					}
					return FALSE;
				}
				pGroupNode = (CCxStructTreeNodeGroup *)pNode;
				m_vGroups.push_back( pGroupNode );
			}
		}

		for ( int i=0; i<(int)m_vGroups.size(); i++ )
		{
			m_mapGroupNode.insert( make_pair( m_vGroups[i]->m_strID, m_vGroups[i] ) );
		}
		
		if ( !obj["paramList"].isNull() && obj["paramList"].isArray() )
		{
			arrValue = obj["paramList"];

			if ( pTree != NULL )
			{
				pTree->m_bNodeParseing = TRUE;
				pTree->m_mapParseNodeCache.clear();
			}

			for ( Json::Value::iterator itr = arrValue.begin(); itr != arrValue.end(); ++itr)
			{
				str = ( (*itr)["nodeType"].isString() ) ? (*itr)["nodeType"].asCString() : _T("");
				int nNodeType = CCxStructTreeNode::ConvertNodeType( (LPCTSTR)str );
				pNode = CCxStructTreeNode::GetNewNodeByType( nNodeType, pTree );
				if ( pNode == NULL )
				{
					Clear();
					if ( pTree != NULL )
					{
						pTree->m_bNodeParseing = FALSE;
						pTree->m_mapParseNodeCache.clear();
					}
					return FALSE;
				}

				if ( !pNode->Parse( *itr/*reinterpret_cast< LPVOID > ( (Json::Value *)& (*itr) )*/
					, pNode, nNodeType, NULL ) )
				{
					delete pNode; pNode = NULL; Clear();
					if ( pTree != NULL )
					{
						pTree->m_bNodeParseing = FALSE;
						pTree->m_mapParseNodeCache.clear();
					}
					return FALSE;
				}

				if ( !pNode->IsParamNode() )
				{
					delete pNode; pNode = NULL; Clear();
					if ( pTree != NULL )
					{
						pTree->m_bNodeParseing = FALSE;
						pTree->m_mapParseNodeCache.clear();
					}
					return FALSE;
				}
				pParamNode = (CCxStructTreeNodeParam *)pNode;
				m_vParams.push_back( pParamNode );
			}
		}

		for ( int i=0; i<(int)m_vParams.size(); i++ )
		{
			m_mapParamNode.insert( make_pair( m_vParams[i]->m_strID, m_vParams[i] ) );
		}

		if ( !obj["operateList"].isNull() && obj["operateList"].isArray() )
		{
			arrValue = obj["operateList"];

			for ( Json::Value::iterator itr = arrValue.begin(); itr != arrValue.end(); ++itr)
			{
				pOperatorData = new CCxStructTreeUpdateParamsOperatorData();
				pOperatorData->m_strId = ( (*itr)["id"].isString() ) ? (*itr)["id"].asCString() : _T("");
				pOperatorData->m_strFlag = ( (*itr)["flag"].isString() ) ? (*itr)["flag"].asCString() : _T("");
				m_vOperators.push_back( pOperatorData );
			}
		}

		if ( pTree != NULL )
		{
			pTree->m_bNodeParseing = FALSE;
			pTree->m_mapParseNodeCache.clear();
		}
	}
	catch (...)
	{
		Clear();
		if ( pTree != NULL )
		{
			pTree->m_bNodeParseing = FALSE;
			pTree->m_mapParseNodeCache.clear();
		}
		return FALSE;
	}

	return TRUE;
}

CCxStructTreeNode * CCxStructTreeUpdateParamsData::FindParam( LPCTSTR lpszId )
{
	CString strId = lpszId == NULL ? "" : lpszId;
	STRUCTTREEMAP_NODE::iterator itr = m_mapParamNode.find( strId );
	if ( itr == m_mapParamNode.end() ) return NULL;
	return (CCxStructTreeNode *)itr->second;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CCxStructTreePopMenuData::CCxStructTreePopMenuData()
{

}

CCxStructTreePopMenuData::~CCxStructTreePopMenuData()
{
	Clear();
}

void CCxStructTreePopMenuData::Clear()
{
	m_strId = "";
	m_strMenuType = "";
}

BOOL CCxStructTreePopMenuData::Parse( LPCTSTR lpszData, CCxStructTree * pTree )
{
	Clear();
	CString strSrc = lpszData == NULL ? "" : lpszData;
	if (strSrc.IsEmpty()) return FALSE;

	Json::Reader reader;
	Json::Value obj( Json::objectValue );
	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) return FALSE;
		if (!obj.isObject()) return FALSE;

		m_strId = ( obj["id"].isString() ) ? obj["id"].asCString() : _T("");
		m_strMenuType = ( obj["menuType"].isString() ) ? obj["menuType"].asCString() : _T("");
	}
	catch (...)
	{
		Clear();
		return FALSE;
	}

	return TRUE;
}

CCxStructTreeUpdateParamsOperatorData::CCxStructTreeUpdateParamsOperatorData()
{
	m_pParamNode = NULL;
}

CCxStructTreeUpdateParamsOperatorData::~CCxStructTreeUpdateParamsOperatorData()
{
	Clear();
}

void CCxStructTreeUpdateParamsOperatorData::Clear()
{
	m_strFlag = "";
	m_strId = "";
	m_pParamNode = NULL;
}
