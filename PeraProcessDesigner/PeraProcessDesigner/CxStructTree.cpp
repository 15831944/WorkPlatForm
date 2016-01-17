#include "StdAfx.h"
#include "CxStructTree.h"
#include "PeraProcessDesigner.h"

#include "ConvertData.h"
#include "PtrVectorHeader.h"

#include "value.h"
#include "reader.h"
#include "writer.h"

#include "CxStructTreeGridT.h"
#include "DlgSvg.h"
#include "PeraProcessDesignerUtility.h"
#include "WebDataDefine.h"
#include "DlgNDArrayEditor.h"
#include "DlgStructTreeFileParamEditor.h"
#include "DlgStructTreePropComponent.h"
#include "DlgStructTreePropProcess.h"
#include "DlgStructTreePropFlow.h"
#include <Tools.h>
#include <afxwin.h>
#include "CxStructTreeNodeSubFlow.h"

/*
MC����ӳ�����

�½�2�������
���1���������a1���������a2��
���2���������b1���������b2��

��ʼʱ��a1��b1��Ч��a2��b2��Ч��

�ֱ�����2�������a2��b2��Ϊ��Ч��

����a2��b1������ӳ���b1��b2��Ϊ��Ч��

��ʱ��������1���й��������2����ֱ�����У���֮���������С�

������1�޸��������������a2��b1��b2����Ϊ��Ч״̬��

�������1��b1��Ϊ����״̬��

���������2��b2��Ϊ��Ч��
*/

CCxStructTree::CCxStructTree(void)
{
	m_pRoot = NULL;
	m_pGrid = NULL;
	m_bNodeParseing = FALSE;
}


CCxStructTree::~CCxStructTree(void)
{
	Clear();
}

void CCxStructTree::Clear( void )
{
	ClearDataLinker();
	m_mapRefIn.clear();
	m_mapRefOut.clear();
	m_mapNode.clear();
	//m_mapParseNodeCache.clear();
	m_bNodeParseing = FALSE;
	delete m_pRoot; m_pRoot = NULL;
}


CCxStructTreeNode * CCxStructTree::ParseNode( LPCTSTR lpszData )
{
	CString strSrc = lpszData == NULL ? "" : lpszData;
	if (strSrc.IsEmpty()) return NULL;

	Json::Reader reader;
	Json::Value obj;
	CString str;
	CCxStructTreeNode * pNode = NULL;
	int nNodeType = CCxStructTreeNode::NT_NULL;
	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) return NULL;
		if (!obj.isObject()) return NULL;

		m_bNodeParseing = TRUE;
		m_mapParseNodeCache.clear();

		str = (obj["nodeType"].isString()) ? obj["nodeType"].asCString() : _T("");
		nNodeType = CCxStructTreeNode::ConvertNodeType( (LPCTSTR)str );
		pNode = CCxStructTreeNode::GetNewNodeByType( nNodeType, this );
		if ( pNode == NULL )
		{
			m_bNodeParseing = FALSE;
			return NULL;
		}

		if ( !pNode->Parse( obj/*reinterpret_cast< LPVOID > ( (Json::Value *)&obj )*/
			, pNode, nNodeType, NULL ) )
		{
			m_bNodeParseing = FALSE; delete pNode; pNode = NULL; return NULL;
		}

	}
	catch (...)
	{
		m_bNodeParseing = FALSE; delete pNode; pNode = NULL; return NULL;
	}

	m_bNodeParseing = FALSE; 
	return pNode;
}

CString CCxStructTree::ParseNodeId( LPCTSTR lpszData )
{
	CString strSrc = lpszData == NULL ? "" : lpszData;
	if (strSrc.IsEmpty()) return NULL;

	Json::Reader reader;
	Json::Value obj;
	CString strId;
	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) return "";
		if (!obj.isObject()) return "";
		strId = (obj["id"].isString()) ? obj["id"].asCString() : _T("");
	}
	catch (...)
	{
		return "";
	}
	return strId;
}

CCxStructTreeNode * CCxStructTree::Find( const CString& strId )
{
	if ( m_bNodeParseing )
	{
		STRUCTTREEMAP_NODE::iterator itrNode = m_mapParseNodeCache.find( strId );
		if ( itrNode == m_mapParseNodeCache.end() ) return NULL;
		return itrNode->second;
	}
	else
	{
		STRUCTTREEMAP_NODE::iterator itrNode = m_mapNode.find( strId );
		if ( itrNode == m_mapNode.end() ) return NULL;
		return itrNode->second;
	}
}

BOOL CCxStructTree::ParseDataLinker( LPCTSTR lpszData )
{
	ClearDataLinker();

	CString strSrc = lpszData == NULL ? "" : lpszData;
	if (strSrc.IsEmpty()) return FALSE;

	Json::Reader reader;
	Json::Value arr;
	CCxStructTreeDataLinker * pLinker = NULL;
	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, arr, false)) return FALSE;
		if ( arr.isNull() || !arr.isArray() ) return FALSE;

		for ( Json::Value::iterator itr = arr.begin(); itr != arr.end(); ++itr)
		{
			if ( !(*itr).isObject() ) continue;
			pLinker = new CCxStructTreeDataLinker();

			pLinker->m_strOwnerId = ( (*itr)["id"].isString()) ? (*itr)["id"].asCString() : _T("");
			pLinker->m_strFrom = ( (*itr)["from"].isString()) ? (*itr)["from"].asCString() : _T("");
			pLinker->m_strTo = ( (*itr)["to"].isString()) ? (*itr)["to"].asCString() : _T("");
			pLinker->m_strFromIndex = ( (*itr)["fromIndex"].isString()) ? (*itr)["fromIndex"].asCString() : _T("");
			pLinker->m_strToIndex = ( (*itr)["toIndex"].isString()) ? (*itr)["toIndex"].asCString() : _T("");
			pLinker->m_strFormula = ( (*itr)["expr"].isString()) ? (*itr)["expr"].asCString() : _T("");
			if ( pLinker->m_strFrom.IsEmpty() || pLinker->m_strTo.IsEmpty() )
			{
				delete pLinker; pLinker = NULL;
				PtrVectorClear( m_vDataLinker );
				return FALSE;
			}
			
			m_vDataLinker.push_back( pLinker );
		}
	}
	catch (...)
	{
		PtrVectorClear( m_vDataLinker );
		return FALSE;
	}

	return TRUE;
}

void CCxStructTree::ClearDataLinker( void )
{
	m_mapLinkerTo.clear();
	m_mapLinkerFrom.clear();
	PtrVectorClear( m_vDataLinker );
}


BOOL CCxStructTree::GetDataLinker( vector< CCxStructTreeDataLinker* >& vLinker
	, CCxStructTreeNode * pFrom, CCxStructTreeNode * pTo )
{
	vLinker.clear();
	if ( pFrom == NULL || pTo == NULL ) return FALSE;
	for ( int i=0; i<(int)m_vDataLinker.size(); i++ )
	{
		if ( 
			m_vDataLinker[i]->m_strTo != pTo->m_strID 
			|| m_vDataLinker[i]->m_strFrom != pFrom->m_strID
			) continue;

		vLinker.push_back( m_vDataLinker[i] );
	}

	return TRUE;
}


BOOL CCxStructTree::AddNode( CCxStructTreeNode * pNewNode, CCxStructTreeNode * pParent, int nIndex /*= -1 */ )
{
	if ( pNewNode == NULL ) return FALSE;
	if ( pParent == NULL )
	{
		if ( pNewNode->m_NodeType != CCxStructTreeNode::NT_ROOT )
		{
			return FALSE;
		}
		SetRoot( pNewNode );
		return TRUE;
	}
	if ( nIndex == -1 )
	{
		 pParent->m_vChildren.push_back( pNewNode );
	}
	else
	{
		if ( nIndex < -1 || nIndex >= (int)pParent->m_vChildren.size() + 1 )
		{
			::MessageBox(GetMainWnd(), "�쳣�������½ڵ�ʱ��ָ����������Խ��", g_lpszAppTitle, MB_OK|MB_ICONERROR);
			return FALSE;
		}

		pParent->m_vChildren.insert( pParent->m_vChildren.begin() + nIndex, pNewNode );
	}
	pNewNode->m_strParentID = pParent->m_strID;
	pNewNode->m_pParent = pParent;

	return TRUE;
}

BOOL CCxStructTree::DeleteNode( CCxStructTreeNode *& pNode )
{
	if ( pNode == NULL ) return FALSE;

	CCxStructTreeNode * pParent = pNode->m_pParent;
	if ( pParent == NULL )
	{
		if ( pNode->m_NodeType != CCxStructTreeNode::NT_ROOT )
		{
			return FALSE;
		}

		Clear();
		return TRUE;
	}
	vector< CCxStructTreeNode* >::iterator itr = find( pParent->m_vChildren.begin()
		, pParent->m_vChildren.end(), pNode );
	if ( itr == pParent->m_vChildren.end() ) return FALSE;

	//RemoveNodeFromRefMap( pNode );

	pParent->m_vChildren.erase( itr );
	delete pNode; pNode = NULL;

	return TRUE;
}

BOOL CCxStructTree::MoveNode( CCxStructTreeMoveEventData * pMoveEventData )
{
	if ( pMoveEventData == NULL ) return FALSE;

	//�ӵ�ǰλ��ɾ��
	pMoveEventData->m_pSrcParent->m_vChildren.erase( pMoveEventData->m_pSrcParent->m_vChildren.begin()
		+ pMoveEventData->m_nSrcIndex );
	//��ӵ���λ��
	if ( pMoveEventData->m_nDestIndex == -1 )
	{
		pMoveEventData->m_pDestParent->m_vChildren.push_back( pMoveEventData->m_pSrcNode );
	}
	else
	{
		pMoveEventData->m_pDestParent->m_vChildren.insert( pMoveEventData->m_pDestParent->m_vChildren.begin()
			+ pMoveEventData->m_nDestIndex, pMoveEventData->m_pSrcNode );
	}

	pMoveEventData->m_pSrcNode->m_strParentID = pMoveEventData->m_pDestParent->m_strID;
	pMoveEventData->m_pSrcNode->m_pParent = pMoveEventData->m_pDestParent;
	return TRUE;
}

BOOL CCxStructTree::SetSelNode( CCxStructTreeNode * pSelNode )
{
	if ( m_pGrid == NULL ) return FALSE;

	CBCGPGridRange* pSelRange = NULL;
	
	//�����ʷѡ��״̬
	pSelRange = new CBCGPGridRange (CCxStructTreeGridT::COL_NAME, 0
		, CCxStructTreeGridT::COL_UNITS, m_pGrid->GetRowCount());
	m_pGrid->SelectRange (*pSelRange, FALSE, TRUE);
	delete pSelRange;

	//CList<CBCGPGridRange*, CBCGPGridRange*>	lstSel;
	//lstSel.AddTail (pSelRange);
	//while (!lstSel.IsEmpty ())
	//{
	//	CBCGPGridRange* pRange = lstSel.RemoveTail ();
	//	//ASSERT (pRange != NULL);

	//	m_pGrid->SelectRange (*pRange, FALSE, TRUE);
	//	delete pRange;
	//}

	while (!m_pGrid->m_lstSel.IsEmpty ())
	{
		pSelRange = m_pGrid->m_lstSel.RemoveTail ();
		ASSERT (pSelRange != NULL);

		m_pGrid->SelectRange (*pSelRange, FALSE, TRUE);
		delete pSelRange;
	}

	m_pGrid->m_idRealSelItem.SetNull();
	m_pGrid->m_idLastSel.SetNull();
	m_pGrid->m_idActive.SetNull();
	m_pGrid->m_pSelRow = NULL;
	m_pGrid->m_pSelItem = NULL;
	m_pGrid->m_pLastSelRow = NULL;
	m_pGrid->m_pLastSelItem = NULL;

	if ( pSelNode != NULL && pSelNode->m_pRow != NULL )
	{
		if ( !pSelNode->IsRowMatched() )
		{
			return FALSE;
		}

		m_pGrid->m_idRealSelItem.m_nColumn = CCxStructTreeGridT::COL_NAME;
		m_pGrid->m_idRealSelItem.m_nRow = pSelNode->m_pRow->GetRowId();

		m_pGrid->m_idLastSel = m_pGrid->m_idRealSelItem;
		m_pGrid->m_idActive = m_pGrid->m_idRealSelItem;
		m_pGrid->m_pSelRow = pSelNode->m_pRow;
		m_pGrid->m_pSelItem = pSelNode->m_pRow->GetItem( CCxStructTreeGridT::COL_NAME );
		m_pGrid->m_pLastSelRow = pSelNode->m_pRow;
		m_pGrid->m_pLastSelItem = m_pGrid->m_pSelItem;

		pSelRange = new CBCGPGridRange ( m_pGrid->m_idRealSelItem );
		m_pGrid->m_lstSel.AddTail (pSelRange);
		m_pGrid->SelectRange (*pSelRange, TRUE, TRUE);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

CCxStructTreeNode * CCxStructTree::GetRoot( void )
{
	return m_pRoot;
}


/*
Svgҳ�淢���ṹ���ؼ��Ľ�����Ϣ��
ע�⣺����Ϣ����ǰ������Ӧ���ڵ���svg�е��κη�������ֹ��ͻ��
�����Ҫ����svg�еķ��������� PostMessage ��ʽ������һ����Ϣ�д���
Ŀǰ֧�ֵ��¼��б����£�

refresh                ˢ�����ýṹ����
add                    �ڽṹ�������һ���ڵ㣬�������ӽڵ㡣
Remove                 ɾ��һ���ڵ㣬�������ӽڵ㡣
update                 ����һ���ڵ㣬�������ӽڵ㡣
move                   �ƶ�һ���Ѵ��ڵĽڵ㵽ָ��λ�á�
Select                 svgͼ��ѡ��һ���ڵ�󣬽ṹ����ͬ��ѡ��״̬��
updateDataMapping      svg֪ͨ�ṹ������������ӳ���ϵ��
updateProp             svg֪ͨ�ṹ���������ڵ����Ա༭���档
showToolTip            svg֪ͨ�ṹ�����Ƿ���ʾ��������ĳ���ڵ��tooltip��
runComponentCompleted  svg֪ͨ�ṹ�������������ɡ�
publish                �������������
editIfProcess          �༭���̽ڵ�
editRootParams          �༭���ڵ����
editDataMappingExp     �༭ӳ����ʽ
runProcess             ������ƹ���
saveSubProcessAs       ����������

*/
BOOL CCxStructTree::SvgEvent( CString& strFlag, CString& strData )
{
	if ( m_pGrid == NULL ) return FALSE;
	if ( ! ::IsWindow ( m_pGrid->GetSafeHwnd() ) ) return FALSE;
	CCxStructTreeNode * pParseNode = NULL;

	ZTools::WriteZToolsFormatLog( "CCxStructTree::SvgEvent Flag��%s Data��\r\n%s\r\n", strFlag, strData );

	//����ˢ��------------------------------------------
	if ( strFlag.CompareNoCase("refresh") == 0 )
	{
		CWaitCursor wait;

		ZTools::WriteZToolsFormatLog( "CCxStructTree::SvgEvent refresh %s\r\n", "����Ч�� - ParseNode ֮ǰ" );
		pParseNode = ParseNode( (LPCTSTR)strData );
		ZTools::WriteZToolsFormatLog( "CCxStructTree::SvgEvent refresh %s\r\n", "����Ч�� - ParseNode �������" );
		if ( pParseNode == NULL )
		{
			ZTools::WriteZToolsFormatLog( "CCxStructTree::SvgEvent refresh ���ݽ���ʧ��\r\n%s\r\n", strData );
			Clear();
		}
		else
		{
			SetRoot( pParseNode );
		}

		CCxStructTreeNode * pNode = Find( m_strCurSelId );
		if ( pNode == NULL ) pNode = m_pRoot;

		theApp.m_pMainWnd->PostMessage( WM_STRUCTTREE_REFRESH, (WPARAM)pParseNode );
		return ( pParseNode != NULL );
	}

	//����µĽڵ�--------------------------------------
	else if ( strFlag.CompareNoCase("add") == 0 )
	{
		CWaitCursor wait;

		//�����½ڵ�
		pParseNode = ParseNode( (LPCTSTR)strData );
		if ( pParseNode == NULL )
		{
			ZTools::WriteZToolsFormatLog( "CCxStructTree::SvgEvent add ���ݽ���ʧ��\r\n%s\r\n", strData );
			return FALSE;
		}
		//�Ҹ��ڵ�
		CCxStructTreeNode * pParentNode = Find( pParseNode->m_strParentID );
		if ( pParentNode == NULL )
		{
			::MessageBox(GetMainWnd(),"�쳣���޷��ҵ���ӽڵ�ĸ��ڵ�", g_lpszAppTitle, MB_OK|MB_ICONERROR);
			return FALSE;
		}
		//�����½ڵ�
		if ( !AddNode( pParseNode, pParentNode, pParseNode->m_nIndex ) )
		{
			delete pParseNode; pParseNode = NULL; return FALSE;
		}
		pParseNode->AddToNodeMap();

		//���½ṹ��
		if ( m_pGrid->CanAddNewRow( pParseNode ) )
		{
			if ( !m_pGrid->AddNode( pParseNode, (CCxStructTreeRowT *)pParentNode->m_pRow, pParseNode->m_nIndex, FALSE ) )
			{
				::MessageBox(GetMainWnd(), "�½ڵ�����ɹ�������ӵ��ṹ��ʧ��", g_lpszAppTitle, MB_OK|MB_ICONERROR);
				return FALSE;
			}
			m_pGrid->RebuildIndexes();
		}

		//m_pGrid->UpdateParamValueBg();
		//m_pGrid->AdjustLayout ();
		m_pGrid->m_bModified = TRUE;

		//SetSelNode( pParseNode );

		theApp.m_pMainWnd->PostMessage( WM_UPDATE_DATAMAPPING, (WPARAM)pParseNode );
		return TRUE;
	}
	
	//ɾ���ڵ�----------------------------------
	else if (strFlag.CompareNoCase("Remove") == 0)
	{
		CWaitCursor wait;

		CString strId = ParseNodeId( (LPCTSTR)strData );
		if ( strId.IsEmpty() ) return FALSE;

		pParseNode = Find( strId );
		if ( pParseNode == NULL ) return FALSE;

		if ( m_pGrid->CanRemoveRow( pParseNode ) )
		{
			if ( !m_pGrid->DeleteNode( pParseNode ) )
			{
				::MessageBox(GetMainWnd(), "ɾ���ڵ�ʧ��", g_lpszAppTitle, MB_OK|MB_ICONERROR);
				return FALSE;
			}
		}

		//ɾ�����ڵ�ʱ�������ڵ�ĸ��ڵ�ĵ���������ΪTRUE
		if (pParseNode->m_NodeType == CCxStructTreeNode::NT_FRONT_FORM || pParseNode->m_NodeType == CCxStructTreeNode::NT_REAR_FORM)
		{
			ASSERT(pParseNode->m_pParent!=NULL);
			CCxStructTreeNodeComponent *pComponent = NULL;
			CCxStructTreeNodeFlow *pFlow = NULL;
			CCxStructTreeNodeSubFlow *pSubFlow = NULL;
			if (pParseNode->m_pParent->IsComponentNode())
			{
				pComponent = static_cast<CCxStructTreeNodeComponent*>(pParseNode->m_pParent);
				ASSERT(pComponent!=NULL);
				pComponent->m_bIsPopForm = TRUE;
			}
			else if (pParseNode->m_pParent->m_NodeType == CCxStructTreeNode::NT_SUBFLOW)
			{
				pSubFlow = static_cast<CCxStructTreeNodeSubFlow*>(pParseNode->m_pParent);
				ASSERT(pSubFlow!=NULL);
				pSubFlow->m_bIsPopForm = TRUE;
			}
			else if (pParseNode->m_pParent->IsFlowNode()|| pParseNode->m_pParent->IsRootNode())
			{
				pFlow = static_cast<CCxStructTreeNodeFlow*>(pParseNode->m_pParent);
				ASSERT(pFlow!=NULL);
				pFlow->m_bIsPopForm = TRUE;
			}			
		}
		
		if ( !DeleteNode( pParseNode ) )
		{
			::MessageBox(GetMainWnd(), "ɾ���ڵ����ݽṹʧ��", g_lpszAppTitle, MB_OK|MB_ICONERROR);
			return FALSE;
		}

		//SetSelNode( m_pRoot );

		theApp.m_pMainWnd->PostMessage( WM_UPDATE_DATAMAPPING, (WPARAM)m_pRoot );
		return TRUE;
	}

	//�༭�ڵ㣬�༭�󣬱���֮ǰ��ѡ��״̬-------------------------------------------
	else if ( strFlag.CompareNoCase("update") == 0 )
	{
		CWaitCursor wait;

		//�����½ڵ�
		ZTools::WriteZToolsFormatLog( "CCxStructTree::SvgEvent update %s\r\n", "����Ч�� - ParseNode ֮ǰ" );
		pParseNode = ParseNode( (LPCTSTR)strData );
		ZTools::WriteZToolsFormatLog( "CCxStructTree::SvgEvent update %s\r\n", "����Ч�� - ParseNode �������" );
		if ( pParseNode == NULL )
		{
			ZTools::WriteZToolsFormatLog( "CCxStructTree::SvgEvent update ���ݽ���ʧ��\r\n%s\r\n", strData );
			return FALSE;
		}
		CCxStructTreeNode * pOldNode = Find( pParseNode->m_strID );
		if ( pOldNode == NULL )
		{
			::MessageBox(GetMainWnd(), "�쳣��Ҫ���µĽڵ�Id������", g_lpszAppTitle, MB_OK||MB_ICONERROR);
			return FALSE;
		}
		///>>>>>>>>>>>>>>>>>>>
		CCxStructTreeNode * pSelNode = NULL;
		CString strSelNodeId;
		if ( m_pGrid != NULL )
		{
			//CCxStructTreeRowT * pSelRow = (CCxStructTreeRowT *)m_pGrid->GetCurSel();
			//if ( pSelRow != NULL )
			//{
			//	pSelNode = (CCxStructTreeNode *)pSelRow->GetData();
			//}
			pSelNode = m_pGrid->GetCurSelNodeData();
		}
		if ( pSelNode == NULL )
		{
			strSelNodeId = pOldNode->m_strID;
		}
		else
		{
			strSelNodeId = pSelNode->m_strID;
		}
		//<<<<<<<<<<<<<<<<

		int nNewPos = pOldNode->GetSiblingPos();
		CCxStructTreeNode * pParentNode = pOldNode->m_pParent;
		CCxStructTreeRowT * pParentRow = NULL;
		if ( pParentNode != NULL )
		{
			pParentRow = (CCxStructTreeRowT*)pParentNode->m_pRow;
		}

		if ( m_pGrid->CanRemoveRow( pOldNode ) )
		{
			if ( !m_pGrid->DeleteNode( pOldNode ) )
			{
				return FALSE;
			}
		}

		if ( !DeleteNode( pOldNode ) )
		{
			return FALSE;
		}
		pParseNode->AddToNodeMap();

		
		if ( !AddNode( pParseNode, pParentNode, nNewPos ) )
		{
			return FALSE;
		}
		ZTools::WriteZToolsFormatLog( "CCxStructTree::SvgEvent update %s\r\n", "����Ч�� - m_pGrid->AddNode ֮ǰ" );
		if ( m_pGrid->CanAddNewRow( pParentNode ) )
		{
			if ( !m_pGrid->AddNode( pParseNode, pParentRow, nNewPos, FALSE ) )
			{
				return FALSE;
			}
			ZTools::WriteZToolsFormatLog( "CCxStructTree::SvgEvent update %s\r\n", "����Ч�� - m_pGrid->AddNode �������" );
			m_pGrid->RebuildIndexes();
			ZTools::WriteZToolsFormatLog( "CCxStructTree::SvgEvent update %s\r\n", "����Ч�� - m_pGrid->m_pGrid->RebuildIndexes() �������" );
		}
		//SetSelNode( pParseNode );

		pSelNode = Find( strSelNodeId );
		if ( pSelNode == NULL ) pSelNode = pParseNode;
		theApp.m_pMainWnd->PostMessage( WM_UPDATE_DATAMAPPING, (WPARAM)pSelNode );
		return TRUE;
	}

	//�ƶ��ڵ�---------------------------------------------
	else if ( strFlag.CompareNoCase("move") == 0 )
	{
		CWaitCursor wait;

		CCxStructTreeMoveEventData MoveData;
		if ( ! MoveData.Parse( (LPCTSTR)strData, this ) ) return FALSE;

		if ( !MoveData.CanMove() ) return FALSE;

		if ( m_pGrid->CanRemoveRow( MoveData.m_pSrcNode ) )
		{
			if ( !m_pGrid->DeleteNode( MoveData.m_pSrcNode ) )
			{
				return FALSE;
			}
		}

		if ( !MoveNode( &MoveData ) )
		{
			::MessageBox(GetMainWnd(),"�ƶ��ڵ����ݽṹʧ��", g_lpszAppTitle, MB_OK|MB_ICONERROR);
			return FALSE;
		}

		if ( m_pGrid->CanAddNewRow( MoveData.m_pSrcNode ) )
		{
			if ( !m_pGrid->AddNode( MoveData.m_pSrcNode, 
				(CCxStructTreeRowT*)MoveData.m_pDestParent->m_pRow, MoveData.m_nDestIndex, FALSE ) )
			{
				return FALSE;
			}
			m_pGrid->RebuildIndexes();
		}

		//SetSelNode( MoveData.m_pSrcNode );

		theApp.m_pMainWnd->PostMessage( WM_UPDATE_DATAMAPPING, (WPARAM)MoveData.m_pSrcNode );
		return TRUE;

		return TRUE;
	}

	//ѡ�нڵ�---------------------------------------
	else if ( strFlag.CompareNoCase("Select") == 0 )
	{
		CString strId = ParseNodeId( (LPCTSTR)strData );
		if ( strId.IsEmpty() ) return FALSE;

		pParseNode = Find( strId );
		if ( pParseNode == NULL ) return FALSE;

		m_strCurSelId = strId;

		SetSelNode( pParseNode );

		return TRUE;
	}

	//��������ӳ��----------------------------------------------
	else if ( strFlag.CompareNoCase("updateDataMapping") == 0 )
	{
		CString* pToId = new CString();
		*pToId = strData;

		if (  theApp.m_pMainWnd != NULL && IsWindow(  theApp.m_pMainWnd->GetSafeHwnd() ) )
		{
			theApp.m_pMainWnd->PostMessage( WM_UPDATE_DATAMAPPING, (WPARAM)NULL, (LPARAM)pToId );
		}
		theApp.m_pBarJianmoTree->m_wndGridTree.m_bModified = TRUE;
		return TRUE;
	}

	//�������ԣ�svg��֪ͨ��Ҫ�������ԣ��ɿͻ��������༭ҳ�棩------------------------------
	else if ( strFlag.CompareNoCase("updateProp") == 0 )
	{
		CString strId = ParseNodeId( (LPCTSTR)strData );
		if ( strId.IsEmpty() ) return FALSE;

		pParseNode = Find( strId );
		if ( pParseNode == NULL ) return FALSE;

		//SetSelNode( pParseNode );

		theApp.m_pMainWnd->PostMessage( WM_STRUCTTREE_UPDATE_PROP
			, reinterpret_cast<WPARAM>(pParseNode) );
		return TRUE;
	}

	//��ʾtooltip��svg��֪ͨ��Ҫtooltip���ɿͻ�����ʾ������tooltip��------------------------------
	else if ( strFlag.CompareNoCase("showToolTip") == 0 )
	{
		CCxStructTreeTooltipData ttd;
		if ( !ttd.Parse( (LPCTSTR)strData, this ) ) return FALSE;

		theApp.m_pMainWnd->PostMessage( WM_STRUCTTREE_SHOW_TOOLTIP
			, reinterpret_cast<WPARAM>(ttd.m_pNode), (LPARAM)ttd.m_bShow );
		return TRUE;
	}

	//������н��-------------------------------------------
	//{ paramList:[{"id":"����id1","value":"��ֵ1"},{"id":"����id2","value":"��ֵ2"}], "status":"�ɹ���ʧ�ܡ�ȡ���ȱ�ʶ����" }
	else if ( strFlag.CompareNoCase("runComponentCompleted") == 0 )
	{
		CWaitCursor wait;

		CCxStructTreeRunResult rr;
		if ( !rr.Parse( strData, this ) )
		{
			ZTools::WriteZToolsFormatLog( "CCxStructTree::SvgEvent runComponentCompleted ���ݽ���ʧ��\r\n%s\r\n", strData );
			return FALSE;
		}
		
		//////////////////////////////////////////////////////////////////////////
		//���������ʧ��ʱ����������������״̬ΪFail����ʱ���û���ʾʧ�ܣ�ͬʱ���в���ִ�н��������
		BOOL bRunSuccess = TRUE;
		BOOL bRunCancel = FALSE;
		for ( int i=0; i<(int)rr.m_vFlows.size(); i++ )
		{
			if (rr.m_vFlows[i]->m_strStatus.CompareNoCase("Fail") == 0)
			{
				bRunSuccess = FALSE;
				break;
			}
			else if (rr.m_vFlows[i]->m_strStatus.CompareNoCase("Cancel") == 0)
			{
				bRunCancel = TRUE;
				break;
			}
		}
		for ( int i=0; i<(int)rr.m_vComponents.size() && bRunSuccess; i++ )
		{
			if (rr.m_vComponents[i]->m_strStatus.CompareNoCase("Fail") == 0)
			{
				bRunSuccess = FALSE;
				break;
			}
			else if (rr.m_vComponents[i]->m_strStatus.CompareNoCase("Cancel") == 0)
			{
				bRunCancel = TRUE;
				break;
			}
		}

		if (!bRunSuccess)
		{
			::MessageBox(GetMainWnd(),"�����г����쳣��", g_lpszAppTitle, MB_OK|MB_ICONERROR);
			return FALSE;
		}
		if(bRunCancel) //ȡ��������Ϣ�򣬲����½ṹ������
		{
			ZTools::WriteZToolsFormatLog("runstate = Cancel,ȡ���������!");
			return FALSE;
		}

		CCxStructTreeNode * pNode = NULL;
		CCxStructTreeNodeParam * pParamNode = NULL;
		vector<CCxStructTreeNodeParam*> vNodeCacheForSetStatus;

		//�������̡�������б�־
		for ( int i=0; i<(int)rr.m_vFlows.size(); i++ )
		{
			if ( rr.m_vFlows[i]->m_pNode != NULL )
				rr.m_vFlows[i]->m_pNode->m_bRunFlag = TRUE;
		}
		for ( int i=0; i<(int)rr.m_vComponents.size(); i++ )
		{
			if ( rr.m_vComponents[i]->m_pNode != NULL )
				rr.m_vComponents[i]->m_pNode->m_bRunFlag = TRUE;
		}

		//����ÿ������ֵ
		for ( STRUCTTREEMAP_PARAM_VALUE::iterator itr = rr.m_mapRunReturn.begin();
			 itr != rr.m_mapRunReturn.end(); ++itr )
		{
			pNode = itr->second->m_pNode;
			if ( pNode == NULL ) continue;
			if ( !pNode->IsParamNode() ) continue;
			pParamNode = (CCxStructTreeNodeParam *)pNode;
			pParamNode->UpdateValue( (LPCTSTR)itr->second->m_strValue );
		}

		//���¼�������ӳ��
		ReloadRefMap();

		//����һ������ӳ��
		for ( STRUCTTREEMAP_PARAM_VALUE::iterator itr = rr.m_mapRunReturn.begin();
			itr != rr.m_mapRunReturn.end(); ++itr )
		{
			pNode = itr->second->m_pNode;
			if ( pNode == NULL ) continue;
			if ( !pNode->IsParamNode() ) continue;
			pParamNode = (CCxStructTreeNodeParam *)pNode;
			pParamNode->RunDataLinker( vNodeCacheForSetStatus );
		}

		for ( int i=0; i<(int)vNodeCacheForSetStatus.size(); i++ )
		{
			pParamNode = vNodeCacheForSetStatus[i];
			pParamNode->OnAfterRunCompleted();
		}

		//���½ڵ�״̬
		vNodeCacheForSetStatus.clear();
		for ( int i=0; i<(int)rr.m_vFlows.size(); i++ )
		{
			if ( rr.m_vFlows[i]->m_pNode == NULL ) continue;
			((CCxStructTreeNodeFlow*)rr.m_vFlows[i]->m_pNode)->OnAfterRunCompleted();
		}
		for ( int i=0; i<(int)rr.m_vComponents.size(); i++ )
		{
			if ( rr.m_vComponents[i]->m_pNode == NULL ) continue;
			((CCxStructTreeNodeComponent*)rr.m_vComponents[i]->m_pNode)->OnAfterRunCompleted();
		}
		for ( STRUCTTREEMAP_PARAM_VALUE::iterator itr = rr.m_mapRunReturn.begin();
			itr != rr.m_mapRunReturn.end(); ++itr )
		{
			pNode = itr->second->m_pNode;
			if ( pNode == NULL ) continue;
			if ( !pNode->IsParamNode() ) continue;
			pParamNode = (CCxStructTreeNodeParam *)pNode;
			pParamNode->OnAfterRunCompleted();
		}
		m_pGrid->AdjustLayout ();
		return TRUE;
	}

	//������svg��֪ͨ��Ҫ�������ɿͻ������������߼���------------------------------
	else if ( strFlag.CompareNoCase("publish") == 0 )
	{
		pParseNode = Find( strData );
		if ( pParseNode == NULL ) return FALSE;

		if ( pParseNode->IsComponentNode() )
			theApp.m_pBarJianmoTree->m_wndGridTree.PublishComponent(( CCxStructTreeNodeComponent* )pParseNode);
		else if ( pParseNode->IsRootNode() )
			theApp.m_pBarJianmoTree->m_wndGridTree.OnMenuStructTreePublishProcess();
		else if ( pParseNode->IsFlowNode() )
			theApp.m_pBarJianmoTree->m_wndGridTree.OnMenuStructTreePublishFlow();
		return TRUE;
	}

	//�༭��֧�ڵ㣨svg�����ͻ��˰������ʽ��json�����ͻ���ͼ�λ���ʾ��������ģ̬��------------------------------
	else if ( strFlag.CompareNoCase("editIfProcess") == 0 )
	{
		CString* pstr = new CString(strData);
		theApp.m_pMainWnd->PostMessage( WM_STRUCTTREE_EDIT_IF
			, reinterpret_cast<WPARAM>(pstr) );
		return TRUE;
	}

	//�༭���ڵ������svg�����ͻ��˰������ʽ��json�����ͻ���ͼ�λ���ʾ��������ģ̬��------------------------------
	else if ( strFlag.CompareNoCase("editProcessParams") == 0 )
	{
		theApp.m_pMainWnd->PostMessage( WM_STRUCTTREE_EDIT_ROOTPARAMS
			 );
		return TRUE;
	}

	//�༭����ӳ����ʽ��svg�����ͻ��˰������ʽ��json�����ͻ���ͼ�λ���ʾ��������ģ̬��------------------------------
	else if ( strFlag.CompareNoCase("editDataMappingExp") == 0 )
	{
		CString* pstr = new CString(strData);
		theApp.m_pMainWnd->PostMessage( WM_STRUCTTREE_EDIT_DATAMAPPING_EXP
			, reinterpret_cast<WPARAM>(pstr) );
		return TRUE;
	}

	//�������̣����ڵ�������̽ڵ㡢if��for�����У�
	else if ( strFlag.CompareNoCase("runProcess") == 0 )
	{
		pParseNode = Find( strData );
		if ( pParseNode == NULL || pParseNode->IsRootNode() )
		{
			theApp.m_pMainWnd->PostMessage( WM_STRUCTTREE_RUN_PROCESS );
		}
		else if ( pParseNode->IsFlowNode() )
		{
			theApp.m_pMainWnd->PostMessage( WM_STRUCTTREE_RUN_SUB_PROCESS );
		}
		return TRUE;
	}
	//�������
	else if ( strFlag.CompareNoCase("runComponent") == 0 )
	{
		theApp.m_pMainWnd->PostMessage( WM_STRUCTTREE_RUN_COMPNENT );
		return TRUE;
	}

	//���������Ϊ
	else if (strFlag.CompareNoCase("saveSubProcessAs") == 0)
	{
		pParseNode = Find( strData );
		theApp.m_pMainWnd->PostMessage( WM_STRUCTTREE_SAVEAS_SUB_PROCESS );
		return TRUE;
	}

	//���������µ����в���
	else if ( strFlag.CompareNoCase("updateParams") == 0 )
	{
		CCxStructTreeNodeParam * pParamNode = NULL;
		CCxStructTreeNode * pNode = NULL;
	//	CCxStructTreeNode * pFlowNode = NULL;
		CCxStructTreeUpdateParamsData pd;
		//����ԭʼ����
		if ( !pd.Parse( strData, this ) )
		{
			//m_bNodeParseing = TRUE;
			//PtrVectorClear( pd.m_vParams );
			m_bNodeParseing = FALSE;
			ZTools::WriteZToolsFormatLog( "CCxStructTree::SvgEvent updateParams ���ݽ���ʧ��\r\n%s\r\n", strData );
			return FALSE;
		}

		//ɾ���ڵ�
		for ( int i=0; i<(int)pd.m_vOperators.size(); i++ )
		{
			CCxStructTreeUpdateParamsOperatorData * pOperate = pd.m_vOperators[i];
			if ( pOperate->m_strFlag.CompareNoCase( "delete" ) != 0 ) continue;

			CCxStructTreeNode * pExistNode = Find( pOperate->m_strId );
			if ( pExistNode == NULL ) continue;

			CCxStructTreeNode * pParentNode = pExistNode->m_pParent;
			if ( pParentNode == NULL ) continue;

			if ( !pExistNode->IsParamNode() && !pExistNode->IsGroupNode() ) continue;

			if ( m_pGrid->CanRemoveRow( pExistNode ) )
			{
				m_pGrid->DeleteNode( pExistNode );
			}
			DeleteNode( pExistNode );
			m_pGrid->m_bModified = TRUE;
		}

		//ɾ���������������С�delete������
		for ( vector< CCxStructTreeUpdateParamsOperatorData* >::iterator itrDel = pd.m_vOperators.begin();
			itrDel != pd.m_vOperators.end(); )
		{
			if ( (*itrDel)->m_strFlag.CompareNoCase( "delete" ) != 0 )
			{
				++itrDel;
				continue;
			}

			delete (*itrDel);
			pd.m_vOperators.erase( itrDel );
			itrDel = pd.m_vOperators.begin();
			continue;
		}

		//�����ڵ�
		for ( vector< CCxStructTreeNodeGroup* >::iterator itrNode = pd.m_vGroups.begin();
			itrNode != pd.m_vGroups.end(); ++itrNode )
		{
			CCxStructTreeNode * pCacheNode = (*itrNode);
			if ( pCacheNode == NULL ) continue;

			CCxStructTreeNode * pExistNode = Find( pCacheNode->m_strID );
			if ( pExistNode == NULL )
			{
				//�����ڣ���ӣ��� operatorList�еġ�add����Ӧ

				CCxStructTreeNode * pParentNode = Find( pCacheNode->m_strParentID );
				if ( pParentNode == NULL ) continue;

				pExistNode = pCacheNode->GetCopied();

				if ( !AddNode( pExistNode, pParentNode, pExistNode->m_nIndex ) )
				{
					delete pExistNode; pExistNode = NULL;
					return FALSE;
				}
				pExistNode->AddToNodeMap();

				if ( m_pGrid->CanAddNewRow( pExistNode ) )
				{
					if ( !m_pGrid->AddNode( pExistNode, (CCxStructTreeRowT *)pParentNode->m_pRow, pExistNode->m_nIndex, FALSE ) )
					{
						m_bNodeParseing = TRUE;
						PtrVectorClear( pd.m_vParams );
						m_bNodeParseing = FALSE;
						::MessageBox(GetMainWnd(), "�½ڵ�����ɹ�������ӵ��ṹ��ʧ��", g_lpszAppTitle, MB_OK|MB_ICONERROR);
						return FALSE;
					}
					m_pGrid->RebuildIndexes();
				}
				m_pGrid->m_bModified = TRUE;
			} // end of if �����ڣ����
			else
			{
				//�Ѵ��ڣ��޸ģ��� operatorList�еġ�modify����Ӧ

				CCxStructTreeNode * pParentNode = pExistNode->m_pParent;
				if ( pParentNode == NULL ) continue;

				pCacheNode->m_dwStatus = pExistNode->m_dwStatus;

				//��ɾ��ԭ�е�
				if ( m_pGrid->CanRemoveRow( pExistNode ) )
				{
					m_pGrid->DeleteNode( pExistNode );
				}
				DeleteNode( pExistNode );

				//������µ�
				pExistNode = pCacheNode->GetCopied();

				if ( !AddNode( pExistNode, pParentNode, pExistNode->m_nIndex ) )
				{
					delete pExistNode; pExistNode = NULL;
					return FALSE;
				}
				pExistNode->AddToNodeMap();

				if ( m_pGrid->CanAddNewRow( pExistNode ) )
				{
					if ( !m_pGrid->AddNode( pExistNode, (CCxStructTreeRowT *)pParentNode->m_pRow, pExistNode->m_nIndex, FALSE ) )
					{
						m_bNodeParseing = TRUE;
						PtrVectorClear( pd.m_vParams );
						m_bNodeParseing = FALSE;
						::MessageBox(GetMainWnd(), "�½ڵ�����ɹ�������ӵ��ṹ��ʧ��", g_lpszAppTitle, MB_OK|MB_ICONERROR);
						return FALSE;
					}
					m_pGrid->RebuildIndexes();
				}
				m_pGrid->m_bModified = TRUE;
			} // end of else �Ѵ��ڣ��޸�
		} // end for ��ڵ�

		//��ӡ��޸��µĽڵ�
		for ( vector< CCxStructTreeNodeParam* >::iterator itrNode = pd.m_vParams.begin();
			itrNode != pd.m_vParams.end(); ++itrNode )
		{
			CCxStructTreeNode * pCacheNode = (*itrNode);
			if ( pCacheNode == NULL ) continue;

			CString sOldValue, sNewValue; 

			CCxStructTreeNode * pExistNode = Find( pCacheNode->m_strID );
			if ( pExistNode == NULL )
			{
				//�����ڣ���ӣ��� operatorList�еġ�add����Ӧ

				CCxStructTreeNode * pParentNode = Find( pCacheNode->m_strParentID );
				if ( pParentNode == NULL ) continue;

				pExistNode = pCacheNode->GetCopied();
				if ( pCacheNode->IsParamNode() )
				{
					CCxStructTreeNodeParam* pExistParamNode = (CCxStructTreeNodeParam*)pExistNode;
					//��������£����������Ұָ������
					if ( pExistParamNode->IsArray() )
					{
						pExistParamNode->ParseValue();
					}
				}

				if ( !AddNode( pExistNode, pParentNode, pExistNode->m_nIndex ) )
				{
					delete pExistNode; pExistNode = NULL;
					return FALSE;
				}
				pExistNode->AddToNodeMap();

				if ( m_pGrid->CanAddNewRow( pExistNode ) )
				{
					if ( !m_pGrid->AddNode( pExistNode, (CCxStructTreeRowT *)pParentNode->m_pRow, pExistNode->m_nIndex, FALSE ) )
					{
						m_bNodeParseing = TRUE;
						PtrVectorClear( pd.m_vParams );
						m_bNodeParseing = FALSE;
						::MessageBox(GetMainWnd(), "�½ڵ�����ɹ�������ӵ��ṹ��ʧ��", g_lpszAppTitle, MB_OK|MB_ICONERROR);
						return FALSE;
					}
					m_pGrid->RebuildIndexes();
				}
				m_pGrid->m_bModified = TRUE;

			} // end of if �����ڣ����
			else
			{
				//�Ѵ��ڣ��޸ģ��� operatorList�еġ�modify����Ӧ

				CCxStructTreeNode * pParentNode = pExistNode->m_pParent;
				if ( pParentNode == NULL ) continue;

				//if (
				//	( pExistNode->m_strParentID != pCacheNode->m_strParentID )
				//	|| ( pExistNode->GetSiblingPos() != pCacheNode->m_nIndex )
				//	)
				{
					pCacheNode->m_dwStatus = pExistNode->m_dwStatus;

					//��ɾ��ԭ�е�
					if ( m_pGrid->CanRemoveRow( pExistNode ) )
					{
						m_pGrid->DeleteNode( pExistNode );
					}
					DeleteNode( pExistNode );

					//������µ�
					pExistNode = pCacheNode->GetCopied();
					if ( pCacheNode->IsParamNode() )
					{
						CCxStructTreeNodeParam* pExistParamNode = (CCxStructTreeNodeParam*)pExistNode;
						//��������£����������Ұָ������
						if ( pExistParamNode->IsArray() )
						{
							pExistParamNode->ParseValue();
						}
					}

					if ( !AddNode( pExistNode, pParentNode, pExistNode->m_nIndex ) )
					{
						delete pExistNode; pExistNode = NULL;
						return FALSE;
					}
					pExistNode->AddToNodeMap();

					if ( m_pGrid->CanAddNewRow( pExistNode ) )
					{
						if ( !m_pGrid->AddNode( pExistNode, (CCxStructTreeRowT *)pParentNode->m_pRow, pExistNode->m_nIndex, FALSE ) )
						{
							m_bNodeParseing = TRUE;
							PtrVectorClear( pd.m_vParams );
							m_bNodeParseing = FALSE;
							::MessageBox(GetMainWnd(), "�½ڵ�����ɹ�������ӵ��ṹ��ʧ��", g_lpszAppTitle, MB_OK|MB_ICONERROR);
							return FALSE;
						}
						m_pGrid->RebuildIndexes();
					}
					m_pGrid->m_bModified = TRUE;
				}
				//else
				//{
				//	pCacheNode->m_dwStatus = pExistNode->m_dwStatus;
				//	//�޸�Ϊ�µ�����
				//	pCacheNode->m_pRow = pExistNode->m_pRow;
				//	pCacheNode->m_pParent = pExistNode->m_pParent;
				//	pCacheNode->m_strParentID = pExistNode->m_strParentID;
				//	pExistNode->Copy( pCacheNode );

				//	if ( pCacheNode->IsParamNode() )
				//	{
				//		CCxStructTreeNodeParam* pExistParamNode = (CCxStructTreeNodeParam*)pExistNode;
				//		//��������£����������Ұָ������
				//		if ( pExistParamNode->IsArray() )
				//		{
				//			pExistParamNode->ParseValue();
				//		}
				//	}

				//	m_pGrid->RebuildIndexes();
				//}
			} // end of else �Ѵ��ڣ��޸�
		} // end for

		BOOL bakNodeParseing = m_bNodeParseing;
		m_bNodeParseing = TRUE;
		PtrVectorClear( pd.m_vParams );
		PtrVectorClear( pd.m_vGroups );
		m_bNodeParseing = bakNodeParseing;
		m_bNodeParseing = FALSE;

		theApp.m_pMainWnd->PostMessage( WM_UPDATE_DATAMAPPING, (WPARAM)NULL );

		return TRUE;
	}	

	//�����Ҽ��˵�/////////////////////////////////////////////////////////
	else if ( strFlag.CompareNoCase("popMenu") == 0 )
	{
		CCxStructTreePopMenuData* popd = new CCxStructTreePopMenuData;
		if (!popd->Parse( strData, this ))
		{
			delete popd;
			ZTools::WriteZToolsFormatLog( "CCxStructTree::SvgEvent popMenu ���ݽ���ʧ��\r\n%s\r\n", strData );
			return FALSE;
		}

		theApp.m_pMainWnd->PostMessage( WM_POP_MENU, (WPARAM)popd );
		return TRUE;
	}

	//�༭���Ȳ�����֪ͨ�ṹ���������޸ı��/////////////////////////
	else if ( strFlag.CompareNoCase("setModified") == 0 )
	{
		if ( theApp.m_pBarJianmoTree != NULL )
		{
			theApp.m_pBarJianmoTree->m_wndGridTree.m_bModified = TRUE;
		}
		return TRUE;
	}

	else
	{
		::MessageBox(GetMainWnd(), "�쳣�����½ṹ��������Ч������־", g_lpszAppTitle, MB_OK|MB_ICONERROR);
		return FALSE;
	}
}

//���������⣬����
//BOOL CCxStructTree::ReplaceNode( CCxStructTreeNode *& pNewNode, CCxStructTreeNode *& pOldNode )
//{
//	if ( pNewNode == NULL || pOldNode == NULL ) return FALSE;
//	CCxStructTreeNode * pParent = pOldNode->m_pParent;
//	if ( pParent == NULL )
//	{
//		if ( pOldNode->m_NodeType != CCxStructTreeNode::NT_ROOT 
//			|| pNewNode->m_NodeType != CCxStructTreeNode::NT_ROOT )
//			return FALSE;
//
//		delete m_pRoot; m_pRoot = pNewNode; return TRUE;
//	}
//
//	vector< CCxStructTreeNode* >::iterator itr = find( pParent->m_vChildren.begin()
//		, pParent->m_vChildren.end(), pOldNode );
//	if ( itr == pParent->m_vChildren.end() ) return FALSE;
//
//	CBCGPGridRow * pRow = pOldNode->m_pRow;
//
//	delete (*itr);
//	(*itr) = pNewNode;
//
//	pNewNode->m_pParent = pParent;
//	pNewNode->m_strParentID = pParent->m_strID;
//	pNewNode->m_pRow = (CCxStructTreeRowT*)pRow;
//
//	return TRUE;
//}

void CCxStructTree::OnNodeSelected( BOOL bNotifySvg )
{
	if ( m_pGrid == NULL ) return;
	//if ( !m_pGrid->m_bTreeClicked ) return;
	if ( m_pGrid->m_idRealSelItem.m_nRow < 0 /*|| m_pGrid->m_idRealSelItem.m_nColumn < 0*/ ) return;

	short nCheck = GetKeyState(VK_LBUTTON);
	BOOL bLButtonDown = (nCheck < 0);
	nCheck = GetKeyState(VK_RBUTTON);
	BOOL bRButtonDown = (nCheck < 0);

	CRect rcGrid;
	m_pGrid->GetWindowRect(&rcGrid);
	CPoint ptMouse;
	::GetCursorPos(&ptMouse);
	if ( ptMouse.x > rcGrid.left && ptMouse.x < rcGrid.right
		&& ptMouse.y > rcGrid.top && ptMouse.y < rcGrid.bottom )
	{
		//�������������
	}
	else
	{
		bLButtonDown = bRButtonDown = FALSE;
	}

	if ( !bLButtonDown && !bRButtonDown && !bNotifySvg ) return;

	CCxStructTreeRowT* pSelRow = ( CCxStructTreeRowT* )m_pGrid->GetRow( m_pGrid->m_idRealSelItem.m_nRow );
	if ( pSelRow == NULL ) return;
	if ( m_pGrid->m_idRealSelItem.m_nColumn == -1 ) m_pGrid->m_idRealSelItem.m_nColumn = CCxStructTreeGridT::COL_NAME;
	CCxStructTreeItemT * pSelItem = ( CCxStructTreeItemT* )pSelRow->GetItem( m_pGrid->m_idRealSelItem.m_nColumn );
	if ( pSelItem == NULL ) return;
	CCxStructTreeNode * pSelNode = reinterpret_cast < CCxStructTreeNode * > ( pSelRow->GetData() );
	if ( pSelNode == NULL ) return;

	if ( pSelNode->m_pRow != pSelRow )
	{
		::MessageBox(GetMainWnd(),"�쳣����ǰѡ���к������ݲ�ƥ��", g_lpszAppTitle, MB_OK|MB_ICONERROR); return;
	}

	CRect rcAera = pSelItem->GetRect();
	CPoint ptTemp = rcAera.TopLeft();
	::ClientToScreen( m_pGrid->GetSafeHwnd(), &ptTemp );
	rcAera.MoveToXY(ptTemp);
	rcAera.right = rcAera.left + 60;
	BOOL bInEditAera = rcAera.PtInRect( ptMouse );

	CCxStructTreeNode * pCanSelectNode = pSelNode->GetCanSelectedNode();
	CDlgSvg * pSvg = GetSvgDilogPtr();
	CCxStructTreeNodeParam * pParamNode = NULL;

	if ( pCanSelectNode != NULL && pSvg != NULL ) 
	{
		pSvg->JsMethod_xpdlNodeSelect( pCanSelectNode );
	}

	if (
		pSelNode->m_NodeType == CCxStructTreeNode::NT_PARAM 
		&& bLButtonDown 
		&& m_pGrid->m_idRealSelItem.m_nColumn == CCxStructTreeGridT::COL_VALUE 
		&& (pSelNode->m_dwStatus & CCxStructTreeNodeParam::NS_VALUE_READONLY) != CCxStructTreeNodeParam::NS_VALUE_READONLY
		)
	{
		CCxStructTreeNodeParam* pParamNode = (CCxStructTreeNodeParam*)pSelNode;
		if ( pParamNode->m_nDataType == DT_BOOLEAN && !pParamNode->IsOut() )
		{
			if ( pParamNode->m_strValue.CompareNoCase("true") == 0 )
				pParamNode->m_strValue = "false";
			else
				pParamNode->m_strValue = "true";
			pSelItem->SetValue( (LPCTSTR)pParamNode->m_strValue, TRUE );

			CDlgSvg * pSvg = GetSvgDilogPtr();
			if ( pSvg != NULL )
			{
				pSvg->JsMethod_paramUpdate( pParamNode );
			}

//			pParamNode->m_bModified = TRUE;
		//	pParamNode->UpdateStatus( TRUE );
		//	vector<CCxStructTreeNode*> vCacheSetStatus;
		//	pParamNode->SetStatus( vCacheSetStatus );
			pParamNode->OnAfterEditValue();
		}
	}

	if ( pSelItem->GetColumnId() == CCxStructTreeGridT::COL_VALUE && bLButtonDown )
	{
		switch ( pSelNode->m_NodeType )
		{
		case CCxStructTreeNode::NT_COMPONENT:
			//�༭���
			break;

		case CCxStructTreeNode::NT_PARAM:
			pParamNode = (CCxStructTreeNodeParam*)pSelNode;
			if ( pParamNode->IsArray() && bInEditAera )
			{
				CxArrayND * pArrayND = reinterpret_cast < CxArrayND * > ( pParamNode->m_lpValueData );
				if ( pArrayND == NULL )
				{
					::MessageBox(GetMainWnd(), "�쳣������Ϊ����Ĳ����ڵ㣬�������������", g_lpszAppTitle, MB_OK|MB_ICONERROR); return;
				}
				CString strLastDim = pArrayND->GetDimStr();
				if ( strLastDim.IsEmpty() )
				{
					::MessageBox(GetMainWnd(),"�쳣������Ϊ����Ĳ����ڵ㣬�޷�ȡ��ά��", g_lpszAppTitle, MB_OK|MB_ICONERROR); return;
				}

				CDlgNDArrayEditor dlg;
				dlg.m_pArrayData = pArrayND;
				if ( dlg.DoModal() == IDOK )
				{
					CString strNewDim = pArrayND->GetDimStr();
					CString * pStr = new CString();
					*pStr = pArrayND->GetArrayStr();
					/*if ( !OnUpdateParamValue( pParamNode, pArrayND->GetArrayStr() ) )
					{
						AfxMessageBox("�쳣��������������ڵ�ʧ��");
						return;
					}*/

					//if ( strNewDim != strLastDim )
					//{
						//if ( !pParamNode->ParseValue( TRUE ) )
						//{
						//	AfxMessageBox("�쳣����������ʧ��");
						//	return;
						//}

						theApp.m_pMainWnd->PostMessage( WM_STRUCTTREE_UPDATE_PARAM_ARRAY
							, reinterpret_cast<WPARAM>(pParamNode)
							, reinterpret_cast<LPARAM>(pStr));
					//}
					//else
					//{
					//	if ( !pParamNode->ParseValue( FALSE ) )
					//	{
					//		AfxMessageBox("�쳣����������ʧ��");
					//		return;
					//	}
					//	m_pGrid->AdjustLayout();
					//}
				}
			}
			else if ( pParamNode->IsFile() && bInEditAera )
			{
				CDlgStructTreeFileParamEditor dlg;
				dlg.m_pNode = (CCxStructTreeNodeParam*)pSelNode;
				if ( dlg.DoModal() )
				{
					//����
					//m_wndGridTree.UpdateParamValueBg();
					//m_pGrid->UpdateParamValue( (LPVOID)(LPCTSTR)pParamNode->m_strValue, pSelRow );
					if ( !OnUpdateParamValue( pParamNode, pParamNode->m_strValue ) )
					{
						::MessageBox(GetMainWnd(), "�쳣��������������ڵ�ʧ��", g_lpszAppTitle, MB_OK|MB_ICONERROR);
						return;
					}
					m_pGrid->m_bModified = TRUE;
				}
			}
			break; // NT_PARAM

		default:
			break;
		}
	} // end of COL_VALUE
}

BOOL CCxStructTree::OnUpdateParamValue( CCxStructTreeNodeParam * pParamNode, LPCTSTR lpszNewValue )
{
	if ( pParamNode == NULL ) return FALSE;
	if ( !pParamNode->UpdateValue( lpszNewValue ) ) return FALSE;
	pParamNode->OnAfterEditValue();

	//pParamNode->m_strValue = lpszNewValue == NULL ? "" : lpszNewValue;

	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return FALSE;

	pSvg->JsMethod_paramUpdate( pParamNode );

	//if ( !IsWindow( pSvg->GetSafeHwnd() ) ) return FALSE;

	//CWebMethodData wmd;
	//CWebMethodParamData * pParam = NULL;
	////��������
	//wmd.m_strMethodName = "paramUpdate";
	////��1������
	//pParam = new CWebMethodParamData();
	//pParam->m_strName = "jsonObj";
	//pParam->m_strValue = pParamNode->GetUpdateParamJsonStr();
	//wmd.m_vParams.push_back(pParam);
	//pSvg->InvokeJsMethod( &wmd );

	//pParamNode->SetModified();

	return TRUE;
}

BOOL CCxStructTree::RemoveNodeFromMap( CCxStructTreeNode * pNode )
{
	if ( pNode == NULL ) return FALSE;
	return RemoveNodeFromMap( pNode->m_strID );
}

BOOL CCxStructTree::RemoveNodeFromMap( CString& strId )
{
	if ( strId.IsEmpty() ) return FALSE;
	if ( m_bNodeParseing )
	{
		STRUCTTREEMAP_NODE::iterator itrNode = m_mapParseNodeCache.find( strId );
		if ( itrNode == m_mapParseNodeCache.end() ) return FALSE;
		m_mapParseNodeCache.erase( itrNode );
	}
	else
	{
		STRUCTTREEMAP_NODE::iterator itrNode = m_mapNode.find( strId );
		if ( itrNode == m_mapNode.end() ) return FALSE;
		m_mapNode.erase( itrNode );
	}
	return TRUE;
}

void CCxStructTree::ReplaceNodeInRefMap( STRUCTTREEMAP_REF& mapRef
	, CCxStructTreeNode * pNewNode, CCxStructTreeNode * pOldNode )
{
	if ( pNewNode == NULL || pOldNode == NULL || mapRef.size() == 0 ) return;
	if ( pNewNode == pOldNode ) return;

	//itr->first: ĳ������ڵ�A
	//itr->second: ������ڵ��Ӧ����������ڵ�mapB
	//itrPair->first: mapB�е���������ڵ��е�һ��
	//itrPair->first: ֵ���� itr->first
	for (STRUCTTREEMAP_REF::iterator itr = mapRef.begin(); itr != mapRef.end(); ++itr )
	{
		for (STRUCTTREEMAP_REFPAIR::iterator itrPair = itr->second.begin(); 
			itrPair != itr->second.end(); /*++itrPair*/ )
		{
			if ( itrPair->first == pOldNode )
			{
				CCxStructTreeNode * pBakNode = itrPair->second;
				itr->second.erase( itrPair );
				itr->second.insert( make_pair( pNewNode, pBakNode ) );
				itrPair = itr->second.begin();
			}
			else
			{
				++itrPair;
			}
		}
	}

	STRUCTTREEMAP_REF::iterator itrRef = mapRef.find( pOldNode );
	if ( itrRef != mapRef.end() )
	{
		mapRef.insert( make_pair( pNewNode, itrRef->second ) );
		mapRef.erase( itrRef );
	}
}

void CCxStructTree::RemoveNodeFromRefMap( STRUCTTREEMAP_REF& mapRef, CCxStructTreeNode * pNode )
{
	if ( pNode == NULL || mapRef.size() == 0 ) return;

	for (STRUCTTREEMAP_REF::iterator itr = mapRef.begin(); itr != mapRef.end(); ++itr )
	{
		for (STRUCTTREEMAP_REFPAIR::iterator itrPair = itr->second.begin(); 
			itrPair != itr->second.end(); /*++itrPair*/ )
		{
			if ( itrPair->first == pNode )
			{
				itr->second.erase( itrPair );
				itrPair = itr->second.begin();
			}
			else
			{
				++itrPair;
			}
		}
	}

	STRUCTTREEMAP_REF::iterator itrRef = mapRef.find( pNode );
	if ( itrRef != mapRef.end() )
	{
		mapRef.erase( itrRef );
	}
}

void CCxStructTree::RemoveNodeFromRefMap( CCxStructTreeNode * pNode )
{
	RemoveNodeFromRefMap( m_mapRefOut, pNode );
	RemoveNodeFromRefMap( m_mapRefIn, pNode );
	RemoveNodeFromRefMap( m_mapLinkerTo, pNode );
	RemoveNodeFromRefMap( m_mapLinkerFrom, pNode );
}

void CCxStructTree::ReplaceNodeInRefMap( CCxStructTreeNode * pNewNode, CCxStructTreeNode * pOldNode )
{
	ReplaceNodeInRefMap( m_mapRefOut, pNewNode, pOldNode );
	ReplaceNodeInRefMap( m_mapRefIn, pNewNode, pOldNode );
	ReplaceNodeInRefMap( m_mapLinkerTo, pNewNode, pOldNode );
	ReplaceNodeInRefMap( m_mapLinkerFrom, pNewNode, pOldNode );
}

BOOL CCxStructTree::AddNodeToMap( CCxStructTreeNode * pNode )
{
	if ( pNode == NULL ) return FALSE;
	if ( pNode->m_strID.IsEmpty() ) return FALSE;
	if ( m_bNodeParseing )
	{
		STRUCTTREEMAP_NODE::iterator itrNode = m_mapParseNodeCache.find( pNode->m_strID );
		if ( itrNode != m_mapParseNodeCache.end() ) FALSE;
		m_mapParseNodeCache.insert( make_pair( pNode->m_strID, pNode ) );
	}
	else
	{
		STRUCTTREEMAP_NODE::iterator itrNode = m_mapNode.find( pNode->m_strID );
		if ( itrNode != m_mapNode.end() ) FALSE;
		m_mapNode.insert( make_pair( pNode->m_strID, pNode ) );
	}
	return TRUE;
}

BOOL CCxStructTree::ReloadRefMap( void )
{
	m_mapRefOut.clear();
	m_mapRefIn.clear();
	m_mapLinkerTo.clear();
	m_mapLinkerFrom.clear();

	STRUCTTREEMAP_NODE::iterator itrNode;
	CCxStructTreeNode * pNode = NULL;
	CCxStructTreeNodeParam * pParamNode = NULL;
	
	vector< CCxStructTreeNodeParam * > vIn;
	vector< CCxStructTreeNodeParam * > vOut;
	STRUCTTREEMAP_REFPAIR::iterator itrRefPair;
	STRUCTTREEMAP_REF::iterator itrRef;

	CCxStructTreeNode * pFromNode = NULL;
	CCxStructTreeNode * pToNode = NULL;

	//�������� �� ���
	for ( itrNode = m_mapNode.begin(); itrNode != m_mapNode.end(); ++itrNode )
	{
		switch ( itrNode->second->m_NodeType )
		{
		case CCxStructTreeNode::NT_ROOT:
		case CCxStructTreeNode::NT_PARALLEL:
		case CCxStructTreeNode::NT_SEQUENCE:
		case CCxStructTreeNode::NT_FOR:
		case CCxStructTreeNode::NT_IF:
		case CCxStructTreeNode::NT_COMPONENT:
		case CCxStructTreeNode::NT_GROUP:
		case CCxStructTreeNode::NT_SUBFLOW:
			//��������ڵ���ӽڵ㣬����ǲ����ڵ㣬ȡ������in��inout���͵Ĳ���
			//����out��inout���Ͳ���������
			vIn.clear();
			vOut.clear();
			for (int i=0; i<(int)itrNode->second->m_vChildren.size(); i++)
			{
				pNode = itrNode->second->m_vChildren[i];
				if ( !pNode->IsParamNode() ) continue;
				//����µ�ÿ�������ڵ�
				pParamNode = (CCxStructTreeNodeParam *)pNode;
				if ( pParamNode->IsIn() || pParamNode->IsInOut() )
					vIn.push_back(pParamNode);
				if ( pParamNode->IsOut() || pParamNode->IsInOut() )
					vOut.push_back(pParamNode);
			}
			//���� ����--��� �Ĺ���
			for (int i=0; i<(int)vOut.size(); i++)
			{
				//out��inout
				itrRef = InsertRefMap( m_mapRefOut, vOut[i] );
				if ( itrRef == m_mapRefOut.end() )
				{
					m_mapRefOut.clear();
					return FALSE;
				}
				//in��inout
				for (int j=0; j<(int)vIn.size(); j++)
				{
					itrRef->second[ vIn[j] ] = vOut[i];
				}
			}
			//���� ���--����Ĺ���
			for (int i=0; i<(int)vIn.size(); i++)
			{
				itrRef = InsertRefMap( m_mapRefIn, vIn[i] );
				if ( itrRef == m_mapRefIn.end() )
				{
					m_mapRefIn.clear();
					return FALSE;
				}
				//���� ����ڵ�(in��inout)
				for (int j=0; j<(int)vOut.size(); j++)
				{
					itrRef->second[ vOut[j] ] = vIn[i];
				}
			}
		default:
			break;
		}
	}

	//��������ӳ��////////////////////////////////////////////
	for ( int i=0; i<(int)m_vDataLinker.size(); i++ )
	{
		pFromNode = Find( m_vDataLinker[i]->m_strFrom );
		pToNode = Find( m_vDataLinker[i]->m_strTo );
		if ( pFromNode == NULL || pToNode == NULL )
		{
			//Ӱ�����ݲ�����
			::MessageBox(GetMainWnd(), "�쳣������ӳ��ڵ㲻����", g_lpszAppTitle, MB_OK|MB_ICONERROR);
			m_mapLinkerTo.clear(); m_mapLinkerFrom.clear();
			return FALSE;
		}
		if ( !pFromNode->IsParamNode() || !pToNode->IsParamNode() )
		{
			//ӳ��ڵ㲻�ǲ����ڵ�
			::MessageBox(GetMainWnd(), "�쳣��ӳ��ڵ㲻�ǲ����ڵ�", g_lpszAppTitle, MB_OK|MB_ICONERROR);
			continue;
		}

		//�������� to - from
		itrRef = InsertRefMap( m_mapLinkerTo, pToNode );
		if ( itrRef == m_mapLinkerTo.end() )
		{
			m_mapLinkerTo.clear(); m_mapLinkerFrom.clear(); return FALSE;
		}
		itrRef->second[ pFromNode ] = pToNode;

		//�������� from - to
		itrRef = InsertRefMap( m_mapLinkerFrom, pFromNode );
		if ( itrRef == m_mapLinkerFrom.end() )
		{
			m_mapLinkerTo.clear(); m_mapLinkerFrom.clear(); return FALSE;
		}
		itrRef->second[ pToNode ] = pFromNode;
	}
	return TRUE;
}

STRUCTTREEMAP_REF::iterator CCxStructTree::InsertRefMap( STRUCTTREEMAP_REF& mapRef,  CCxStructTreeNode * pNode )
{
	STRUCTTREEMAP_REF::iterator itrRef = mapRef.find( pNode );
	if ( itrRef != mapRef.end() ) return itrRef;

	STRUCTTREEMAP_REFPAIR mapRefPair;
	mapRef.insert( make_pair( pNode, mapRefPair ) );
	itrRef = mapRef.find( pNode );
	if ( itrRef == mapRef.end() )
	{
		itrRef = mapRef.end();
	}
	return itrRef;
}

void CCxStructTree::UpdateGridAllRowStatus( void )
{
	if ( m_pGrid == NULL ) return;
	
	STRUCTTREEMAP_NODE::iterator itrNode;
	CCxStructTreeNodeParam * pParamNode = NULL;

	//for ( itrNode = m_mapNode.begin(); itrNode != m_mapNode.end(); ++itrNode )
	//{
	//	if ( !itrNode->second->IsParamNode() ) continue;
	//	pParamNode = (CCxStructTreeNodeParam *)itrNode->second;
	//	pParamNode->m_dwStatus = 0;
	//}

	//����ֵ��Ԫ��������ɫ��״̬
	for ( itrNode = m_mapNode.begin(); itrNode != m_mapNode.end(); ++itrNode )
	{
		if ( !itrNode->second->IsParamNode() ) continue;
		pParamNode = (CCxStructTreeNodeParam *)itrNode->second;
		BOOL bValid = !( (pParamNode->m_dwStatus & CCxStructTreeNode::NS_NAME_INVALID) == 
			CCxStructTreeNode::NS_NAME_INVALID );
		pParamNode->SetStatus( bValid, TRUE );

	}
}
//
//BOOL CCxStructTree::CheckInvalidStatusByRef( CCxStructTreeNode * pNode )
//{
//	if ( pNode == NULL ) return FALSE;
//
//	STRUCTTREEMAP_REFPAIR::iterator itrRefPair;
//	STRUCTTREEMAP_REF::iterator itrRef;
//	CCxStructTreeNodeParam * pParamNode = NULL;
//
//	itrRef = m_mapRefOut.find( pNode );
//	if ( itrRef == m_mapRefOut.end() ) return FALSE;
//
//	for ( itrRefPair = itrRef->second.begin(); itrRefPair != itrRef->second.end(); ++itrRefPair )
//	{
//		if ( !itrRefPair->first->IsParamNode() ) continue;
//		pParamNode = (CCxStructTreeNodeParam*)itrRefPair->first;
//		if ( pParamNode->m_bSetInvalidStatus || pParamNode->m_bModified )
//		{
//			return TRUE;
//		}
//	}
//	return FALSE;
//}
//
//BOOL CCxStructTree::CheckInvalidStatusByLinker( CCxStructTreeNode * pNode )
//{
//	if ( pNode == NULL ) return FALSE;
//
//	STRUCTTREEMAP_REFPAIR::iterator itrRefPair;
//	STRUCTTREEMAP_REF::iterator itrRef;
//	CCxStructTreeNodeParam * pParamNode = NULL;
//
//	itrRef = m_mapLinkerTo.find( pNode );
//	if ( itrRef == m_mapLinkerTo.end() ) return FALSE;
//
//	for ( itrRefPair = itrRef->second.begin(); itrRefPair != itrRef->second.end(); ++itrRefPair )
//	{
//		if ( !itrRefPair->first->IsParamNode() ) continue;
//		pParamNode = (CCxStructTreeNodeParam*)itrRefPair->first;
//		if ( pParamNode->m_bSetInvalidStatus || pParamNode->m_bModified )
//		{
//			return TRUE;
//		}
//	}
//	return FALSE;
//}

BOOL CCxStructTree::SetRoot( CCxStructTreeNode * pRoot )
{
	Clear();
	if ( pRoot == NULL ) return FALSE;
	if ( pRoot->m_NodeType != CCxStructTreeNode::NT_ROOT ) return FALSE;
	m_pRoot = pRoot;
	m_bNodeParseing = FALSE;
	m_pRoot->AddToNodeMap();
	return TRUE;
}

BOOL CCxStructTree::UpdateArrayParamNode( CCxStructTreeNode * pParamNode )
{
	if ( pParamNode == NULL ) return FALSE;
	if ( m_pGrid == NULL ) return FALSE;

	int nRowPos = pParamNode->GetSiblingPos();
	CCxStructTreeRowT * pParentRow = NULL;
	if ( pParamNode->m_pParent != NULL )
	{
		pParentRow = (CCxStructTreeRowT*)pParamNode->m_pParent->m_pRow;
	}
	if ( pParentRow == NULL )
	{
		::MessageBox(GetMainWnd(), "�޷�ȡ�ò����ڵ�ĸ��ڵ�", g_lpszAppTitle, MB_OK|MB_ICONERROR);
		return FALSE;
	}

	if ( m_pGrid->CanRemoveRow( pParamNode ) )
	{
		if ( !m_pGrid->DeleteNode( pParamNode ) )
		{
			::MessageBox(GetMainWnd(), "�쳣��ɾ���ṹ�������ڵ�ʧ��", g_lpszAppTitle, MB_OK|MB_ICONERROR);
			return FALSE;
		}
	}

	if ( !((CCxStructTreeNodeParam*)pParamNode)->ParseValue( TRUE ) )
	{
		::MessageBox(GetMainWnd(), "�쳣����������ʧ��", g_lpszAppTitle, MB_OK|MB_ICONERROR);
		return FALSE;
	}

	if ( m_pGrid->CanAddNewRow( pParamNode ) )
	{
		if ( !m_pGrid->AddNode( pParamNode, pParentRow, nRowPos ) )
		{
			::MessageBox(GetMainWnd(), "�쳣����Ӳ����ڵ�ʧ��", g_lpszAppTitle, MB_OK|MB_ICONERROR);
			return FALSE;
		}
	}
	
	//pParamNode->UpdateStatus(TRUE);
//	vector<CCxStructTreeNode*> vCacheSetStatus;
//	((CCxStructTreeNodeParam*)pParamNode)->SetStatus( vCacheSetStatus );

	if (  theApp.m_pMainWnd != NULL && IsWindow(  theApp.m_pMainWnd->GetSafeHwnd() ) )
	{
		theApp.m_pMainWnd->PostMessage( WM_UPDATE_DATAMAPPING, (WPARAM)pParamNode );
	}

	return TRUE;
}

BOOL CCxStructTree::RetriveDataMapping( void )
{
	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return FALSE;

	CWebMethodData wmd;
	wmd.m_sMethodName = "getDataMapping";
	pSvg->InvokeJsMethod( &wmd );
	if (wmd.m_sResult.IsEmpty()) return FALSE;

	if ( !ParseDataLinker( wmd.m_sResult ) )
	{
		return FALSE;
	}

	if ( m_pGrid != NULL )
		m_pGrid->UpdateDataMapping();

	return TRUE;
}

CString CCxStructTree::GetDataLinkerFormula( CCxStructTreeNode* pFromNode
	, CCxStructTreeNode* pToNode )
{
	if ( pFromNode == NULL || pToNode == NULL ) return "";
	for ( int i=0; i<(int)m_vDataLinker.size(); i++ )
	{
		if ( 
			m_vDataLinker[i]->m_strTo == pToNode->m_strID 
			&& m_vDataLinker[i]->m_strFrom == pFromNode->m_strID
			)
			return m_vDataLinker[i]->m_strFormula;
	}
	return "";
}


BOOL CCxStructTree::UpdateParams( LPCTSTR lpszData )
{
	CString strFlag = "runComponentCompleted";
	CString strData = lpszData == NULL ? "" : lpszData;
	SvgEvent( strFlag, strData );
	return TRUE;
}

BOOL CCxStructTree::UpdateNodeProp( CCxStructTreeNode * pNode )
{
	if ( pNode == NULL ) return FALSE;

	if ( pNode->IsComponentNode() )
	{
		CDlgStructTreePropComponent dlg;
		dlg.m_pNode = (CCxStructTreeNodeComponent*)pNode;
		if (!dlg.DoModal())
		{
			return FALSE;
		}
	}
	else if ( pNode->IsRootNode() )
	{
		CDlgStructTreePropProcess dlg;
		dlg.m_pNode = (CCxStructTreeNodeFlow*)pNode;
		if (!dlg.DoModal())
		{
			return FALSE;
		}
		theApp.m_pMainWnd->PostMessage( WM_UPDATE_MAIN_TITLE);
	}
	else if ( pNode->IsFlowNode() )
	{
		CDlgStructTreePropFlow dlg;
		dlg.m_pNode = (CCxStructTreeNodeFlow*)pNode;
		if (!dlg.DoModal())
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}

	if ( pNode->IsRowMatched() )
	{
		pNode->m_pRow->GetItem(CCxStructTreeGridT::COL_NAME)->SetValue(
			(LPCTSTR)pNode->m_strText );
	}

	theApp.m_pBarJianmoTree->m_wndGridTree.m_bModified = TRUE;
	return TRUE;
}
//
//CCxStructTreeNode * CCxStructTree::ReBuildArrayNode( CCxStructTreeNode* pOldNode )
//{
//	if ( pOldNode == NULL ) return NULL;
//	if ( !pOldNode->IsParamNode() ) return NULL;
//	if ( !((CCxStructTreeNodeParam*)pOldNode)->IsArray() ) return NULL;
//
//	CCxStructTreeNodeParam* pNewNode = (CCxStructTreeNodeParam*)pOldNode->GetCopied();
//	if ( pNewNode == NULL ) return NULL;
//
////	pNewNode->m_bModified = FALSE;
//	pNewNode->m_dwStatus = 0;
//	pNewNode->m_lpValueData = NULL;
//	pNewNode->m_pRow = NULL;
//	pNewNode->m_nIndex = -1;
//	pNewNode->m_pParent = NULL;
//	pNewNode->m_vChildren.clear();
//	pNewNode->ParseValue();
//
//	int nNewPos = pOldNode->GetSiblingPos();
//	CCxStructTreeNode * pParentNode = pOldNode->m_pParent;
//	CCxStructTreeRowT * pParentRow = NULL;
//	if ( pParentNode != NULL )
//	{
//		pParentRow = (CCxStructTreeRowT*)pParentNode->m_pRow;
//	}
//	//���¼���ӳ�仺��
//	ReplaceNodeInRefMap( pNewNode, pOldNode );
//	//���������ֵ�����д��ڣ����滻Ϊ�µ�
//	vector< CCxStructTreeNode* >::iterator itrCache = find( m_vCacheSetValue.begin()
//		, m_vCacheSetValue.end(), pOldNode );
//	if ( itrCache != m_vCacheSetValue.end() )
//	{
//		m_vCacheSetValue.erase( itrCache );
//		m_vCacheSetValue.push_back( pNewNode );
//	}
//	//ɾ��Grid�У�ɾ���ڵ㣬��ӽڵ㣬���Grid��
//	if ( !m_pGrid->DeleteNode( pOldNode ) )
//	{
//		AfxMessageBox("ɾ��Grid�ڵ�ʧ��");
//		delete pNewNode; pNewNode = NULL; return NULL;
//	}
//
//	if ( !DeleteNode( pOldNode ) )
//	{
//		AfxMessageBox("ɾ��StructTree�ڵ�ʧ��");
//		delete pNewNode; pNewNode = NULL; return NULL;
//	}
//	pNewNode->AddToNodeMap();
//	if ( !AddNode( pNewNode, pParentNode, nNewPos ) )
//	{
//		AfxMessageBox("���StructTree�ڵ�ʧ��");
//		delete pNewNode; pNewNode = NULL; return NULL;
//	}
//	if ( !m_pGrid->AddNode( pNewNode, pParentRow, nNewPos ) )
//	{
//		AfxMessageBox("���Grid�ڵ�ʧ��");
//		delete pNewNode; pNewNode = NULL; return NULL;
//	}
//	
//	//pNewNode->UpdateStatus(TRUE);
//	vector<CCxStructTreeNode*> vCacheSetStatus;
//	pNewNode->SetStatus( vCacheSetStatus );
//	m_pGrid->RebuildIndexes();
//
//	return pNewNode;
//}

//BOOL CCxStructTree::CheckNodeUpdateStatusInquire( CCxStructTreeNode * pNode, BOOL bAdd )
//{
//	if ( pNode == NULL ) return FALSE;
//	if ( find( m_vCacheUpdateStatusInquire.begin(), m_vCacheUpdateStatusInquire.end()
//		, pNode ) != m_vCacheUpdateStatusInquire.end() ) return TRUE;
//	if ( bAdd ) m_vCacheUpdateStatusInquire.push_back( pNode );
//	return FALSE;
//}

//BOOL CCxStructTree::CheckNodeUpdateStatus( CCxStructTreeNode * pNode, BOOL bAdd )
//{
//	if ( pNode == NULL ) return FALSE;
//	if ( find( m_vCacheUpdateStatus.begin(), m_vCacheUpdateStatus.end()
//		, pNode ) != m_vCacheUpdateStatus.end() ) return TRUE;
//	if ( bAdd ) m_vCacheUpdateStatus.push_back( pNode );
//	return FALSE;
//}

//void CCxStructTree::BeforeRunning( CCxStructTreeNode * pNode )
//{
//	if ( m_pRoot == NULL || pNode == NULL ) return;
//	for ( int i=0; i<(int)m_pRoot->m_vChildren.size(); i++ )
//	{
//		if ( m_pRoot->m_vChildren[i]->IsComponentNode() )
//		{
//			(( CCxStructTreeNodeComponent* )m_pRoot->m_vChildren[i])->m_bRunFlag = FALSE;
//		}
//		else if ( m_pRoot->m_vChildren[i]->IsFlowNode() )
//		{
//			(( CCxStructTreeNodeFlow* )m_pRoot->m_vChildren[i])->SetRunFlag( FALSE );
//		}
//	}
//	if ( pNode->IsComponentNode() )
//	{
//		(( CCxStructTreeNodeComponent* )pNode)->m_bRunFlag = TRUE;
//		//(( CCxStructTreeNodeComponent* )pNode)->ClearModifiedFlag();
//	}
//	else if ( pNode->IsFlowNode() )
//	{
//		(( CCxStructTreeNodeFlow* )pNode)->SetRunFlag( TRUE );
//		//(( CCxStructTreeNodeFlow* )pNode)->ClearModifiedFlag();
//	}
//}

BOOL CCxStructTree::GetParamNodesWithLinker( vector< CCxStructTreeNodeParam* >& vParamNodes )
{
	CCxStructTreeNode* pNode = NULL;
	CCxStructTreeNodeParam* pParamNode = NULL;
	CCxStructTreeDataLinker* pLinker = NULL;
	STRUCTTREEMAP_NODE::iterator itrNode;
	vParamNodes.clear();
	vector<CString> vNodeIds;
	for ( int i=0; i<(int)m_vDataLinker.size(); i++ )
	{
		pLinker = m_vDataLinker[i];
		vNodeIds.clear();
		if ( !pLinker->m_strFrom.IsEmpty() )
			vNodeIds.push_back( pLinker->m_strFrom );
		if ( !pLinker->m_strTo.IsEmpty() )
			vNodeIds.push_back( pLinker->m_strTo );

		for ( int j=0; j<(int)vNodeIds.size(); j++ )
		{
			itrNode = m_mapNode.find( vNodeIds[j] );
			if ( itrNode == m_mapNode.end() )
			{
				::MessageBox(GetMainWnd(), "�쳣������ӳ��ڵ㲻����\r\nCCxStructTree::GetParamNodesWithLinker", g_lpszAppTitle, MB_OK|MB_ICONERROR);
				return FALSE;
			}
			pNode = itrNode->second;
			if ( pNode == NULL ) return FALSE;
			if ( !pNode->IsParamNode() ) return FALSE;
			pParamNode = (CCxStructTreeNodeParam*)pNode;
			if ( find( vParamNodes.begin(), vParamNodes.end(), pParamNode )
				== vParamNodes.end() )
			{
				vParamNodes.push_back( pParamNode );
			}
		}
	}

	return TRUE;
}

void CCxStructTree::ResetRunFlag()
{
	CCxStructTreeNodeParam* pParamNode = NULL;
	CCxStructTreeNodeParam* pFromNode = NULL;
	vector< CCxStructTreeNodeParam* > vParamNodes;
	vector< CCxStructTreeNodeParam* > vLinkerFromNodes;
	GetParamNodesWithLinker( vParamNodes );
	for (int i=0; i<(int)vParamNodes.size(); i++)
	{
		pParamNode = vParamNodes[i];
		vLinkerFromNodes.clear();
		pParamNode->GetSiblingLinkerFrom( vLinkerFromNodes );
		for (int j=0; j<(int)vLinkerFromNodes.size(); j++)
		{
			pFromNode = vLinkerFromNodes[j];
			if ( pFromNode->m_pParent == NULL )
			{
				::MessageBox(GetMainWnd(), "�쳣���������ڵ�Ϊ�ա�\r\nCCxStructTree::ResetRunFlag", g_lpszAppTitle, MB_OK|MB_ICONERROR);
				return;
			}
			if ( !pFromNode->CheckRunFlag() )
				pParamNode->SetRunFlag( FALSE );
		}
	}
}