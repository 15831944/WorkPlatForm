#include "StdAfx.h"
#include "CxStructTreeNodeParam.h"

#include "value.h"
#include "reader.h"
#include "writer.h"
#include <Tools.h>
#include "PtrVectorHeader.h"
#include <CxStructTreeGridT.h>
#include <DlgSvg.h>
#include "PeraProcessDesignerUtility.h"
#include <CxStructTreeNodeFlow.h>
#include <CxStructTreeNodeComponent.h>
#include "CxStructTreeNodeRoot.h"
#include "JsData_Svg.h"
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


CCxStructTreeNodeParam::CCxStructTreeNodeParam( CCxStructTree * pTree )
	: CCxStructTreeNode( pTree ) 
{
	m_nDataType = DT_NULL;
	m_nDir = PARAM_DIR_NULL;
	m_bIsDynamicArray = FALSE;
//	m_bSetInvalidStatus = FALSE;
	m_lpValueData = NULL;
	m_bIsSaveInModel = TRUE;
}

CCxStructTreeNodeParam::~CCxStructTreeNodeParam()
{
	Clear();
}

void CCxStructTreeNodeParam::Clear( void )
{
	ClearValueData();
	m_nDataType = DT_NULL;
	m_nDir = PARAM_DIR_NULL;
	m_bIsDynamicArray = FALSE;
	m_strValue = "";
	m_strUnits = "";
	m_strCnName = "";
	m_strDescrip = "";
	m_strFormat = "";
	m_strGroup = "";
	m_bIsSaveInModel = TRUE;

	CCxStructTreeNode::Clear();
}

void CCxStructTreeNodeParam::OnGetCopied( CCxStructTreeNode * pNewNode )
{
	CCxStructTreeNode::OnGetCopied( pNewNode );
	CCxStructTreeNodeParam * pParamNode = ( CCxStructTreeNodeParam* )pNewNode;

	pParamNode->m_nDataType = m_nDataType;
	pParamNode->m_nDir = m_nDir;
	pParamNode->m_strValue = m_strValue;
	pParamNode->m_strUnits = m_strUnits;
	pParamNode->m_bIsDynamicArray = m_bIsDynamicArray;
	if (IsArray())
	{
		CxArrayND * pArrayND = reinterpret_cast< CxArrayND * > ( m_lpValueData );
		CxArrayND * pArrayNDCopied =  pArrayND->GetCopied();
		pParamNode->m_lpValueData  = pArrayNDCopied;
	}
	pParamNode->m_strCnName = m_strCnName;
	pParamNode->m_strDescrip = m_strDescrip;
	pParamNode->m_strFormat = m_strFormat;
	pParamNode->m_strGroup = m_strGroup;
	pParamNode->m_bIsSaveInModel = m_bIsSaveInModel;
}

BOOL CCxStructTreeNodeParam::OnParsing( Json::Value& _jsonObj )
{
	if ( !CCxStructTreeNode::OnParsing( _jsonObj ) ) return FALSE;

//	Json::Value obj;
//	if ( !ConvertJsonObj( obj, lpJsonObj ) ) return FALSE;
	CString str;

	try
	{
		str = (_jsonObj["issaveinmodel"].isString()) ? _jsonObj["issaveinmodel"].asCString() : _T("TRUE"); //无isPopForm属性时默认为true
		if (str.CompareNoCase("true")==0)
			m_bIsSaveInModel = TRUE;
		else
			m_bIsSaveInModel = FALSE;

		str = (_jsonObj["dataType"].isString()) ? _jsonObj["dataType"].asCString() : _T("");
		m_nDataType = ConvertParamDataType( (LPCTSTR)str );
		if ( m_nDataType == DT_NULL )
		{
			return FALSE;
		}

		str = (_jsonObj["dir"].isString()) ? _jsonObj["dir"].asCString() : _T("");
		m_nDir = ConvertParamDir( (LPCTSTR)str );

		m_strValue = (_jsonObj["value"].isString()) ? _jsonObj["value"].asCString() : _T("");
		m_strUnits = (_jsonObj["units"].isString()) ? _jsonObj["units"].asCString() : _T("");

		str = (_jsonObj["isdynamicarray"].isString()) ? _jsonObj["isdynamicarray"].asCString() : _T("");
		m_bIsDynamicArray = ( str.CompareNoCase("true") == 0 );

		m_strCnName = (_jsonObj["cnname"].isString()) ? _jsonObj["cnname"].asCString() : _T("");
		m_strDescrip = (_jsonObj["desc"].isString()) ? _jsonObj["desc"].asCString() : _T("");
		m_strFormat = (_jsonObj["format"].isString()) ? _jsonObj["format"].asCString() : _T("");
		m_strGroup = (_jsonObj["group"].isString()) ? _jsonObj["group"].asCString() : _T("");
		str = (_jsonObj["status"].isString()) ? _jsonObj["status"].asCString() : _T("");
		if ( !str.IsEmpty() ) 
		{
			m_dwStatus = strtol((char*)(LPCTSTR)str, NULL, 10);
		}
		else
		{
			//新建参数如果是输出，则无效
			if ( IsInOut() || IsOut() )
			{
				m_dwStatus |= NS_NAME_INVALID;
			}
		}
		//if ( obj["refId"].isArray() )
		//{
		//	Json::Value arrRefId;
		//	arrRefId = obj["refId"];
		//	for ( Json::Value::iterator itrRefId = arrRefId.begin(); itrRefId != arrRefId.end(); ++itrRefId)
		//	{
		//		str = (*itrRefId).isString() ? (*itrRefId).asCString() : "";
		//		m_vRefId.push_back( str );
		//	}
		//}
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CCxStructTreeNodeParam::OnParsed( CCxStructTreeNode * pParseRoot )
{
	return ParseValue();
}

BOOL CCxStructTreeNodeParam::IsArray1D( void )
{
	switch ( m_nDataType )
	{
	case DT_ARRAY1D_BOOLEAN:
	case DT_ARRAY1D_INT:
	case DT_ARRAY1D_DOUBLE:
	case DT_ARRAY1D_STRING:
		return TRUE;
	default:
		return FALSE;
	}
}

BOOL CCxStructTreeNodeParam::IsArray2D( void )
{
	switch ( m_nDataType )
	{
	case DT_ARRAY2D_BOOLEAN:
	case DT_ARRAY2D_INT:
	case DT_ARRAY2D_DOUBLE:
	case DT_ARRAY2D_STRING:
		return TRUE;
	default:
		return FALSE;
	}
}

BOOL CCxStructTreeNodeParam::IsArray3D( void )
{
	switch ( m_nDataType )
	{
	case DT_ARRAY3D_BOOLEAN:
	case DT_ARRAY3D_INT:
	case DT_ARRAY3D_DOUBLE:
	case DT_ARRAY3D_STRING:
		return TRUE;
	default:
		return FALSE;
	}
}

BOOL CCxStructTreeNodeParam::IsArray( void )
{
	if ( IsArray1D() || IsArray2D() || IsArray3D() ) return TRUE;
	return FALSE;
}

BOOL CCxStructTreeNodeParam::IsFile( void )
{
	return ( m_nDataType == DT_FILE );
}

BOOL CCxStructTreeNodeParam::IsInt( void )
{
	return ( m_nDataType == DT_INT );
}

BOOL CCxStructTreeNodeParam::IsDouble( void )
{
	return ( m_nDataType == DT_DOUBLE );
}

BOOL CCxStructTreeNodeParam::IsBoolean( void )
{
	return ( m_nDataType == DT_BOOLEAN );
}

BOOL CCxStructTreeNodeParam::IsString( void )
{
	return ( m_nDataType == DT_STRING );
}

int CCxStructTreeNodeParam::GetElementDataType( void )
{
	switch ( m_nDataType )
	{
	case DT_STRING:
	case DT_DOUBLE:
	case DT_BOOLEAN:
	case DT_INT:
	case DT_FILE:
		return m_nDataType;

	case DT_ARRAY1D_BOOLEAN:
	case DT_ARRAY2D_BOOLEAN:
	case DT_ARRAY3D_BOOLEAN:
		return DT_BOOLEAN;

	case DT_ARRAY1D_INT:
	case DT_ARRAY2D_INT:
	case DT_ARRAY3D_INT:
		return DT_INT;

	case DT_ARRAY1D_DOUBLE:
	case DT_ARRAY2D_DOUBLE:
	case DT_ARRAY3D_DOUBLE:
		return DT_DOUBLE;

	case DT_ARRAY1D_STRING:
	case DT_ARRAY2D_STRING:
	case DT_ARRAY3D_STRING:
		return DT_STRING;

	default:
		return DT_NULL;
	}
}

void CCxStructTreeNodeParam::ClearValueData( void )
{
	if ( m_lpValueData == NULL ) return;

	if ( IsArray() )
	{
		CxArrayND * pArrayND = reinterpret_cast< CxArrayND * > ( m_lpValueData );
		delete pArrayND; m_lpValueData = NULL;
	}
}

BOOL CCxStructTreeNodeParam::ParseValue( BOOL bRebuild )
{
	ClearValueData();
	if ( IsArray() )
	{
		CxArrayND * pArrayND = new CxArrayND( m_nDataType );
		if ( !pArrayND->Parse( (LPCTSTR)m_strValue ) )
		{
			ZTools::WriteZToolsFormatLog("异常，CCxStructTreeNodeParam::ParseValue 解析数组失败\r\n参数类型：%s 参数值：%s\r\n"
				, ConvertParamDataType(m_nDataType), m_strValue );
			delete pArrayND; pArrayND = NULL; return FALSE;
		}

		pArrayND->m_bIsDynamicArray = m_bIsDynamicArray;
		pArrayND->m_nDir = m_nDir;
		pArrayND->m_pNode = this;

		if ( IsArray1D() )
		{
			if ( !ParseArray1D( pArrayND, bRebuild ) )
			{
				ZTools::WriteZToolsFormatLog("异常，CCxStructTreeNodeParam::ParseValue 解析1维数组失败\r\n参数类型：%s 参数值：%s\r\n"
					, ConvertParamDataType(m_nDataType), m_strValue );
				delete pArrayND; pArrayND = NULL; return FALSE;
			}
		}
		else if ( IsArray2D() )
		{
			if ( !ParseArray2D( pArrayND, bRebuild ) )
			{
				ZTools::WriteZToolsFormatLog("异常，CCxStructTreeNodeParam::ParseValue 解析2维数组失败\r\n参数类型：%s 参数值：%s\r\n"
					, ConvertParamDataType(m_nDataType), m_strValue );
				delete pArrayND; pArrayND = NULL; return FALSE;
			}
		}
		else if ( IsArray3D() )
		{
			if ( !ParseArray3D( pArrayND, bRebuild ) )
			{
				ZTools::WriteZToolsFormatLog("异常，CCxStructTreeNodeParam::ParseValue 解析3维数组失败\r\n参数类型：%s 参数值：%s\r\n"
					, ConvertParamDataType(m_nDataType), m_strValue );
				delete pArrayND; pArrayND = NULL; return FALSE;
			}
		}
		m_lpValueData = reinterpret_cast< LPVOID > ( pArrayND );
	}
	return TRUE;
}

BOOL CCxStructTreeNodeParam::ParseArray1D( CxArrayND * pArray, BOOL bRebuild )
{
	if ( bRebuild ) PtrVectorClear(m_vChildren);

	CString str;
	str.Format("[%d]", pArray->GetDataCount());
	m_strText = m_strOriName + str;

	vector< int > vPos;
	vPos.push_back( 0 );
	CCxStructTreeNode * pNode = NULL;
	vector<CCxStructTreeNode*>::iterator itrElement;
	CCxStructTreeRowT * pRow = NULL;

	if ( !bRebuild )
	{
		for (int i=0; i<pArray->GetDataCount(); i++)
		{
			vPos[0] = i;
			itrElement = m_vChildren.begin() + i;
			if ( itrElement == m_vChildren.end() ) return FALSE;
			((CCxStructTreeNodeArrayElement*)(*itrElement))->m_strValue = pArray->GetDataAsCString( vPos );
			pRow = ((CCxStructTreeNodeArrayElement*)(*itrElement))->m_pRow;
			if ( pRow != NULL )
				pRow->GetItem( CCxStructTreeGridT::COL_VALUE )->SetValue( 
				(LPCTSTR)((CCxStructTreeNodeArrayElement*)(*itrElement))->m_strValue );
		}
	}
	else
	{
		for (int i=0; i<pArray->GetDataCount(); i++)
		{
			vPos[0] = i;
			pNode = new CCxStructTreeNodeArrayElement( NULL );
			pNode->m_NodeType = NT_PARAM_ARRAY;
			str.Format("_element_%d", i);
			pNode->m_strID = m_strID + str;
			pNode->m_strParentID = m_strID;
			pNode->m_pParent = this;
			pNode->m_strText.Format("%s[%d]", m_strOriName, i);

			((CCxStructTreeNodeArrayElement*)pNode)->m_strValue = pArray->GetDataAsCString( vPos );

			m_vChildren.push_back( pNode );
		}
	}


	return TRUE;
}

BOOL CCxStructTreeNodeParam::ParseArray2D( CxArrayND * pArray, BOOL bRebuild )
{
	if ( bRebuild ) PtrVectorClear(m_vChildren);

	CString str;
	//[row, col]
	str.Format("[%d,%d]", pArray->GetDataCount( 0 ), pArray->GetDataCount( 1 ) );
	m_strText = m_strOriName + str;

	vector< int > vPos;
	vPos.push_back(0);
	vPos.push_back(0);
	CCxStructTreeNode * pRowNode = NULL;
	CCxStructTreeNode * pColNode = NULL;
	vector<CCxStructTreeNode*>::iterator itrRow;
	vector<CCxStructTreeNode*>::iterator itrCol;
	CCxStructTreeRowT * pRow = NULL;

	if ( !bRebuild )
	{
		for (int i=0; i<pArray->GetDataCount( 0 ); i++)
		{
			vPos[0] = i;
			//row
			itrRow = m_vChildren.begin() + i;
			if ( itrRow == m_vChildren.end() ) return FALSE;

			for (int j=0; j<pArray->GetDataCount( 1 ); j++)
			{
				vPos[1] = j;
				//
				itrCol = (*itrRow)->m_vChildren.begin() + j;
				if ( itrCol == (*itrRow)->m_vChildren.end() ) return FALSE;

				((CCxStructTreeNodeArrayElement*)(*itrCol))->m_strValue = pArray->GetDataAsCString( vPos );
				pRow = ((CCxStructTreeNodeArrayElement*)(*itrCol))->m_pRow;
				if ( pRow != NULL )
					pRow->GetItem( CCxStructTreeGridT::COL_VALUE )->SetValue( 
					(LPCTSTR)((CCxStructTreeNodeArrayElement*)(*itrCol))->m_strValue );
			}
		}
	}
	else
	{
		for (int i=0; i<pArray->GetDataCount( 0 ); i++)
		{
			vPos[0] = i;
			//row
			pRowNode = new CCxStructTreeNodeArrayElement( NULL );
			pRowNode->m_NodeType = NT_PARAM_ARRAY;
			pRowNode->m_strID.Format("%s_dim1_%d", m_strID, i );
			pRowNode->m_strParentID = m_strID;
			pRowNode->m_pParent = this;
			pRowNode->m_strText.Format("%s[%d,*]", m_strOriName, i);

			for (int j=0; j<pArray->GetDataCount( 1 ); j++)
			{
				vPos[1] = j;
				pColNode = new CCxStructTreeNodeArrayElement( NULL );
				pColNode->m_NodeType = NT_PARAM_ARRAY;
				pColNode->m_strID.Format( "%s_element_%d", pRowNode->m_strID, j );
				pColNode->m_strParentID = pRowNode->m_strID;
				pColNode->m_pParent = pRowNode;
				pColNode->m_strText.Format("%s[%d,%d]", m_strOriName, i, j);

				((CCxStructTreeNodeArrayElement*)pColNode)->m_strValue = pArray->GetDataAsCString( vPos );

				pRowNode->m_vChildren.push_back( pColNode );
			}

			m_vChildren.push_back( pRowNode );
		}
	}

	return TRUE;
}

BOOL CCxStructTreeNodeParam::ParseArray3D( CxArrayND * pArray, BOOL bRebuild )
{
	if ( bRebuild ) PtrVectorClear(m_vChildren);

	CString str;
	int nDim1 = pArray->GetDataCount(0);
	int nDim2 = pArray->GetDataCount(1);
	int nDim3 = pArray->GetDataCount(2);
	//[dim1, dim2, dim3]
	str.Format("[%d,%d,%d]", nDim1, nDim2, nDim3 );
	m_strText = m_strOriName + str;

	vector < int > vPos;
	vPos.push_back(0);
	vPos.push_back(0);
	vPos.push_back(0);
	CCxStructTreeNode * pDim1Node = NULL;
	CCxStructTreeNode * pDim2Node = NULL;
	CCxStructTreeNode * pDim3Node = NULL;
	vector<CCxStructTreeNode*>::iterator itrDim1;
	vector<CCxStructTreeNode*>::iterator itrDim2;
	vector<CCxStructTreeNode*>::iterator itrDim3;
	CCxStructTreeRowT * pRow = NULL;

	if ( !bRebuild )
	{
		for (int i=0; i<nDim1; i++)
		{
			vPos[0] = i;
			//dim1
			itrDim1 = m_vChildren.begin() + i;
			if ( itrDim1 == m_vChildren.end() ) return FALSE;
			//dim2
			for (int j=0; j<nDim2; j++)
			{
				vPos[1] = j;
				itrDim2 = (*itrDim1)->m_vChildren.begin() + j;
				if ( itrDim2 == (*itrDim1)->m_vChildren.end() ) return FALSE;
				//dim3
				for (int k=0; k<nDim3; k++)
				{
					vPos[2] = k;
					itrDim3 = (*itrDim2)->m_vChildren.begin() + k;
					if ( itrDim3 == (*itrDim2)->m_vChildren.end() ) return FALSE;

					((CCxStructTreeNodeArrayElement*)(*itrDim3))->m_strValue = pArray->GetDataAsCString( vPos );
					pRow = ((CCxStructTreeNodeArrayElement*)(*itrDim3))->m_pRow;
					if ( pRow != NULL )
						pRow->GetItem( CCxStructTreeGridT::COL_VALUE )->SetValue( 
							(LPCTSTR)((CCxStructTreeNodeArrayElement*)(*itrDim3))->m_strValue );
				}
			}
		}
	}
	else
	{
		for (int i=0; i<nDim1; i++)
		{
			vPos[0] = i;
			//dim1
			pDim1Node = new CCxStructTreeNodeArrayElement( NULL );
			pDim1Node->m_NodeType = NT_PARAM_ARRAY;
			pDim1Node->m_strID.Format( "%s_dim1_%d", m_strID, i );
			pDim1Node->m_strParentID = m_strID;
			pDim1Node->m_pParent = this;
			pDim1Node->m_strText.Format("%s[%d,*,*]", m_strOriName , i);

			//dim2
			for (int j=0; j<nDim2; j++)
			{
				vPos[1] = j;
				pDim2Node = new CCxStructTreeNodeArrayElement( NULL );
				pDim2Node->m_NodeType = NT_PARAM_ARRAY;
				pDim2Node->m_strID.Format( "%s_dim2_%d", pDim1Node->m_strID, j );
				pDim2Node->m_strParentID = pDim1Node->m_strID;
				pDim2Node->m_pParent = pDim1Node;
				pDim2Node->m_strText.Format("%s[%d,%d,*]", m_strOriName, i, j);

				//dim3
				for (int k=0; k<nDim3; k++)
				{
					vPos[2] = k;
					pDim3Node = new CCxStructTreeNodeArrayElement( NULL );
					pDim3Node->m_NodeType = NT_PARAM_ARRAY;
					pDim3Node->m_strID.Format( "%s_element_%d", pDim2Node->m_strID, k );
					pDim3Node->m_strParentID = pDim2Node->m_strID;
					pDim3Node->m_pParent = pDim2Node;
					pDim3Node->m_strText.Format("%s[%d,%d,%d]", m_strOriName, i, j, k);

					((CCxStructTreeNodeArrayElement*)pDim3Node)->m_strValue = pArray->GetDataAsCString( vPos );

					pDim2Node->m_vChildren.push_back( pDim3Node );
				}

				pDim1Node->m_vChildren.push_back( pDim2Node );
			}

			m_vChildren.push_back( pDim1Node );
		}
	}

	return TRUE;
}

CString CCxStructTreeNodeParam::GetUpdateParamJsonStr( void )
{
	try
	{
		CString strResult;
		Json::Value obj;
		obj["id"] = Json::Value( (LPCTSTR)m_strID );
		obj["value"] = Json::Value( (LPCTSTR)m_strValue );

		Json::FastWriter fast_writer;
		strResult = fast_writer.write( obj ).c_str();
		return strResult;
	}
	catch (...)
	{
		return "";
	}
}

CCxStructTreeNodeArrayElement * CCxStructTreeNodeParam::GetElementNode( vector<int> vPos )
{
	vector< CCxStructTreeNode* >::iterator itrChild;
	vector< CCxStructTreeNode* >::iterator itrParent;

	if ( vPos.size() == 0 ) return NULL;

	itrChild = m_vChildren.begin() + vPos[0];
	if ( itrChild == m_vChildren.end() ) return NULL;

	if ( vPos.size() > 1 )
	{
		for (int i=1; i<(int)vPos.size(); i++)
		{
			itrParent = itrChild;
			itrChild = (*itrChild)->m_vChildren.begin() + vPos[i];
			if ( itrChild == (*itrParent)->m_vChildren.end() ) return NULL;
		}
	}
	return (CCxStructTreeNodeArrayElement*)(*itrChild);
}

CString CCxStructTreeNodeParam::GetTooltipText( void )
{
	CString strTip;
	if ( IsArray() )
	{
		strTip.Format( "参数：%s\r\n名称：%s\r\n类型：%s\r\n值：%s\r\n单位：%s\r\n方向：%s\r\n备注：%s\r\n动态数组：%s\r\n组：%s\r\n格式：%s"
			, m_strText
			, m_strCnName
			, GetDataTypeForDisplay()
			, m_strValue
			, m_strUnits
			, GetDataDirForDisplay()
			, m_strDescrip
			, m_bIsDynamicArray ? "true" : "false"
			, m_strGroup
			,m_strFormat
			);
	}
	else
	{
		strTip.Format( "参数：%s\r\n名称：%s\r\n类型：%s\r\n值：%s\r\n单位：%s\r\n方向：%s\r\n备注：%s\r\n组：%s\r\n格式：%s"
			, m_strText
			, m_strCnName
			, GetDataTypeForDisplay()
			, m_strValue
			, m_strUnits
			, GetDataDirForDisplay()
			, m_strDescrip
			, m_strGroup
			,m_strFormat
			);
	}

	return strTip;
}

CString CCxStructTreeNodeParam::GetDataTypeForDisplay( void )
{
	switch ( m_nDataType )
	{
	case DT_STRING:
		return "String";
	case DT_DOUBLE:
		return "Double";
	case DT_BOOLEAN:
		return "Boolean";
	case DT_INT:
		return "Integer";
	case DT_FILE:
		return "File";

	case DT_ARRAY1D_BOOLEAN:
	case DT_ARRAY2D_BOOLEAN:
	case DT_ARRAY3D_BOOLEAN:
		return "Boolean[]";

	case DT_ARRAY1D_INT:
	case DT_ARRAY2D_INT:
	case DT_ARRAY3D_INT:
		return "Integer[]";

	case DT_ARRAY1D_DOUBLE:
	case DT_ARRAY2D_DOUBLE:
	case DT_ARRAY3D_DOUBLE:
		return "Double[]";

	case DT_ARRAY1D_STRING:
	case DT_ARRAY2D_STRING:
	case DT_ARRAY3D_STRING:
		return "String[]";

	default:
		return "";
	}
}

CString CCxStructTreeNodeParam::GetDataDirForDisplay( void )
{
	switch ( m_nDir )
	{
	case PARAM_DIR_IN:
		return "In";
	case PARAM_DIR_OUT:
		return "Out";
	case PARAM_DIR_INOUT:
		return "InOut";
	default:
		return "";
	}
}

BOOL CCxStructTreeNodeParam::DeleteArrayData( void )
{
	if ( !IsArray() ) return FALSE;

	CCxStructTreeGridT* pGrid = NULL;
	if ( m_pTree != NULL )
	{
		pGrid = m_pTree->m_pGrid;
	}

	CCxStructTreeRowT* pParentRow = NULL;
	int nSiblingPos = -1;
	if ( m_pRow != NULL )
	{
		pParentRow = (CCxStructTreeRowT*)m_pRow->GetParent();
	}
	nSiblingPos = GetSiblingPos();

	if ( pGrid != NULL )
	{
		if ( pGrid->CanRemoveRow( this ) )
		{
			if ( !pGrid->DeleteNode( this ) )
			{
				::MessageBox(GetMainWnd(), "删除Grid节点失败", g_lpszAppTitle, MB_OK|MB_ICONERROR);
				return FALSE;
			}
		}
	}
	PtrVectorClear(m_vChildren);
	ClearValueData();

	if ( pGrid != NULL )
	{
		if ( pGrid->CanAddNewRow( this ) )
		{
			if ( !pGrid->AddNode( this, pParentRow, nSiblingPos ) )
			{
				::MessageBox(GetMainWnd(), "添加Grid节点失败", g_lpszAppTitle, MB_OK|MB_ICONERROR);
				return FALSE;
			}
			pGrid->RebuildIndexes();
		}
	}
	return TRUE;
}

BOOL CCxStructTreeNodeParam::ReBuildArray( LPCTSTR lpszArrayData )
{
	CCxStructTreeGridT* pGrid = NULL;
	if ( m_pTree != NULL )
	{
		pGrid = m_pTree->m_pGrid;
	}

	CString strArrayData = lpszArrayData == NULL ? "" : lpszArrayData;
	if ( strArrayData.IsEmpty() )
	{
		strArrayData = GetDefualtValue( m_nDataType );
	}

	int nNewPos = this->GetSiblingPos();
	CCxStructTreeNode * pParentNode = this->m_pParent;
	CCxStructTreeRowT * pParentRow = NULL;
	if ( pParentNode != NULL )
	{
		pParentRow = (CCxStructTreeRowT*)pParentNode->m_pRow;
	}

	DeleteArrayData();
	ParseValue();

	if ( pGrid->CanRemoveRow( this ) )
	{
		pGrid->DeleteNode( this );
	}

	if ( pGrid->CanAddNewRow( this ) )
	{
		if ( !pGrid->AddNode( this, pParentRow, nNewPos ) )
		{
			::MessageBox(GetMainWnd(), "添加Grid节点失败", g_lpszAppTitle, MB_OK|MB_ICONERROR);
			return FALSE;
		}

		//pNewNode->UpdateStatus(TRUE);
		pGrid->RebuildIndexes();

	}
	return TRUE;
}

DWORD CCxStructTreeNodeParam::GetStatus()
{
	if ( m_pTree == NULL ) return 0;
	if ( m_pTree->m_pGrid == NULL ) return 0;
	//if ( !IsRowMatched() ) return 0;

	STRUCTTREEMAP_REF::iterator itrRef;
	STRUCTTREEMAP_REFPAIR::iterator itrRefPair;
	CCxStructTreeNode * pNode = NULL;
	CCxStructTreeNodeParam * pParamNode = NULL;
	CCxStructTreeItemT * pItemValue = NULL;

	DWORD dwNewStatus = 0;

	//更新当前节点值的状态///////////////////////////////////

	//如果是输出，设置值为灰色，且只读
	if ( IsOut() )
	{
		dwNewStatus |= NS_VALUE_INVALID;
		dwNewStatus |= NS_VALUE_READONLY;
	}

	//如果被数据映射，设置值为灰色，且只读
	if ( m_pTree->m_mapLinkerTo.find( this ) != m_pTree->m_mapLinkerTo.end() )
	{
		dwNewStatus |= NS_VALUE_INVALID;
		dwNewStatus |= NS_VALUE_READONLY;
		dwNewStatus |= NS_DATA_LINKED;
	}

	//可编辑
	if ( IsIn() || IsInOut() )
	{
		dwNewStatus |= NS_VALUE_CANEDIT;
	}

	//更新当前节点图标的状态///////////////////////////////////

	if ( m_pParent == NULL )
	{
		dwNewStatus |= NS_NAME_INVALID;
	}

	if ( IsLoopLinker() )
	{
		dwNewStatus |= NS_NAME_INVALID;
	}

	return dwNewStatus;
}

BOOL CCxStructTreeNodeParam::SetStatus( BOOL bValid, BOOL bForce )
{
	DWORD dwNewStatus = GetStatus();

	if ( !IsLoopLinker() )
	{
		if ( bValid )
			dwNewStatus &= ~NS_NAME_INVALID;
		else
			dwNewStatus |= NS_NAME_INVALID;
	}

	if ( dwNewStatus == m_dwStatus && !bForce ) return TRUE;
	m_dwStatus = dwNewStatus;
	if ( m_pRow != NULL )
	{
		SetGridRowFormat();
	}
	return TRUE;
}

void CCxStructTreeNodeParam::SetGridRowFormat( void )
{
	if ( !IsRowMatched() ) return;

	CCxStructTreeItemT * pItemValue = NULL;
	
	pItemValue = (CCxStructTreeItemT*)m_pRow->GetItem( CCxStructTreeGridT::COL_VALUE );
	if ( pItemValue != NULL )
	{
		if ( m_dwStatus & NS_VALUE_INVALID )
		{
			pItemValue->SetTextColor( g_crStructTreeTextInvalid );
		}
		else if ( m_dwStatus & NS_VALUE_CANEDIT )
		{
			pItemValue->SetTextColor( g_crStructTreeTextCanEdit );
		}
		else
		{
			pItemValue->SetTextColor( g_crStructTreeText );
		}

		BOOL bReadOnly = ( NS_VALUE_READONLY == (m_dwStatus & NS_VALUE_READONLY) );
		pItemValue->Enable( !bReadOnly );
	}

	//更新图标//////////////////////////////
	if ( m_pTree != NULL )
	{
		if ( m_pTree->m_pGrid != NULL )
		{
			m_pTree->m_pGrid->SetParamImage( this );
		}
	}	
}

BOOL CCxStructTreeNodeParam::UpdateValue( LPCTSTR lpszNewValue, BOOL bForce )
{
	CString strNewValue = lpszNewValue == NULL ? "" : lpszNewValue;
	if ( strNewValue.IsEmpty() ) strNewValue = GetDefualtValue( m_nDataType );

	if (
		( m_strValue != strNewValue )
		|| bForce
		)
	{
		m_strValue = strNewValue;
		if ( this->IsArray() )
		{
			this->ReBuildArray( (LPCTSTR)strNewValue );
		}
		else if ( this->IsFile() )
		{

		}
		else
		{
			if ( this->IsRowMatched() )
			{
				m_pRow->GetItem( CCxStructTreeGridT::COL_VALUE )->SetValue( (LPCTSTR)m_strValue, TRUE );
			}
		}
	}

	return TRUE;
}

CString CCxStructTreeNodeParam::GetDefualtValue( int nDataType )
{
	switch ( nDataType )
	{
	case DT_STRING:
		return "";
	case DT_DOUBLE:
		return "0.0";
	case DT_BOOLEAN:
		return "false";
	case DT_INT:
		return "0";
	case DT_FILE:
		return "";

	case DT_ARRAY1D_STRING:
		return "[](0)";
	case DT_ARRAY2D_STRING:
		return "[](0,0)";
	case DT_ARRAY3D_STRING:
		return "[](0,0,0)";

	case DT_ARRAY1D_DOUBLE:
		return "[](0)";
	case DT_ARRAY2D_DOUBLE:
		return "[](0,0)";
	case DT_ARRAY3D_DOUBLE:
		return "[](0,0,0)";

	case DT_ARRAY1D_BOOLEAN:
		return "[](0)";
	case DT_ARRAY2D_BOOLEAN:
		return "[](0,0)";
	case DT_ARRAY3D_BOOLEAN:
		return "[](0,0,0)";

	case DT_ARRAY1D_INT:
		return "[](0)";
	case DT_ARRAY2D_INT:
		return "[](0,0)";
	case DT_ARRAY3D_INT:
		return "[](0,0,0)";

	default:
		return "";
	}
}

BOOL CCxStructTreeNodeParam::SetRunFlag( BOOL bRan /*= TRUE */ )
{
	CCxStructTreeNode* pParentWithoutGroup = GetParentWithoutGroup();

	if ( pParentWithoutGroup == NULL )
	{
		::MessageBox(GetMainWnd(), "异常，参数节点的父节点为空\r\nCCxStructTreeNodeParam::SetRunFlag", g_lpszAppTitle, MB_OK|MB_ICONERROR);
		return FALSE;
	}
	
	if ( pParentWithoutGroup->IsComponentNode() )
		((CCxStructTreeNodeComponent*)pParentWithoutGroup)->m_bRunFlag = bRan;
	else if ( pParentWithoutGroup->IsFlowNode() )
		((CCxStructTreeNodeFlow*)pParentWithoutGroup)->m_bRunFlag = bRan;
	else if ( pParentWithoutGroup->IsRootNode() )
		((CCxStructTreeNodeRoot*)pParentWithoutGroup)->m_bRunFlag = bRan;
	else
		return FALSE;

	CJsData_Svg_UpdateRunFlag rf;
	CJsData_Svg_UpdateRunFlagData * pData = new CJsData_Svg_UpdateRunFlagData();
	pData->m_strId = pParentWithoutGroup->m_strID;
	pData->m_bRan = bRan;
	rf.m_vData.push_back( pData );
	CString str = rf.GetJsonStr();
	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg != NULL )
	{
		pSvg->JsMethod_updateRunFlag( str );
	}

	return TRUE;
}

BOOL CCxStructTreeNodeParam::CheckRunFlag( void )
{
	CCxStructTreeNode* pParentWithoutGroup = GetParentWithoutGroup();

	if ( pParentWithoutGroup == NULL ) return FALSE;

	if ( pParentWithoutGroup->IsComponentNode() )
		return ((CCxStructTreeNodeComponent*)pParentWithoutGroup)->m_bRunFlag;
	else if ( pParentWithoutGroup->IsFlowNode() )
		return ((CCxStructTreeNodeFlow*)pParentWithoutGroup)->m_bRunFlag;
	else if ( pParentWithoutGroup->IsRootNode() )
		//不检查根节点的运行状态。
		return TRUE;
		//return ((CCxStructTreeNodeRoot*)m_pParent)->m_bRunFlag;
	else
		return FALSE;
}

int CCxStructTreeNodeParam::GetSiblingParam( vector< CCxStructTreeNodeParam* >& vSpecNodes )
{
	if ( m_pParent == NULL ) return 0;
	CCxStructTreeNode * pNode = NULL;
	CCxStructTreeNodeParam * pParamNode = NULL;
	for ( vector< CCxStructTreeNode* >::iterator itr = 
		m_pParent->m_vChildren.begin(); itr != m_pParent->m_vChildren.end(); ++itr )
	{
		pNode = (*itr);
		if ( !pNode->IsParamNode() ) continue;
		pParamNode = (CCxStructTreeNodeParam *)pNode;
		if ( find( vSpecNodes.begin(), vSpecNodes.end(), pParamNode ) 
			== vSpecNodes.end() )
		{
			vSpecNodes.push_back( pParamNode );
		}
	}
	return (int)vSpecNodes.size();
}

int CCxStructTreeNodeParam::GetSiblingOut( vector< CCxStructTreeNodeParam* >& vSpecNodes )
{
	if ( m_pParent == NULL ) return 0;
	CCxStructTreeNode * pNode = NULL;
	CCxStructTreeNodeParam * pParamNode = NULL;
	for ( vector< CCxStructTreeNode* >::iterator itr = 
		m_pParent->m_vChildren.begin(); itr != m_pParent->m_vChildren.end(); ++itr )
	{
		pNode = (*itr);
		if ( !pNode->IsParamNode() ) continue;
		pParamNode = (CCxStructTreeNodeParam *)pNode;
		if ( pParamNode->IsInOut() || pParamNode->IsOut() )
		{
			if ( find( vSpecNodes.begin(), vSpecNodes.end(), pParamNode ) 
				== vSpecNodes.end() )
			{
				vSpecNodes.push_back( pParamNode );
			}
		}
	}
	return (int)vSpecNodes.size();
}

int CCxStructTreeNodeParam::GetSiblingLinkerTo( vector< CCxStructTreeNodeParam* >& vSpecNodes )
{
	STRUCTTREEMAP_REF::iterator itrRef;
	STRUCTTREEMAP_REFPAIR::iterator itrRefPair;
	CCxStructTreeNode * pNode = NULL;
	CCxStructTreeNodeParam * pParamNode = NULL;
	if ( m_pTree == NULL ) return 0;
	//当前节点不存在To节点
	itrRef = m_pTree->m_mapLinkerFrom.find( this );
	if ( itrRef == m_pTree->m_mapLinkerFrom.end() ) return 0;
	//遍历数据映射的from节点
	for ( itrRefPair = itrRef->second.begin(); itrRefPair != itrRef->second.end();
		++itrRefPair )
	{
		pNode = itrRefPair->first;
		if ( !pNode->IsParamNode() ) continue;
		pParamNode = (CCxStructTreeNodeParam*)pNode;
		if ( find( vSpecNodes.begin(), vSpecNodes.end(), pParamNode )
			== vSpecNodes.end() )
		{
			vSpecNodes.push_back( pParamNode );
		}
	}
	return (int)vSpecNodes.size();
}

int CCxStructTreeNodeParam::GetSiblingLinkerFrom( vector< CCxStructTreeNodeParam* >& vSpecNodes )
{
	STRUCTTREEMAP_REF::iterator itrRef;
	STRUCTTREEMAP_REFPAIR::iterator itrRefPair;
	CCxStructTreeNode * pNode = NULL;
	CCxStructTreeNodeParam * pParamNode = NULL;
	if ( m_pTree == NULL ) return 0;
	//当前节点不存在To节点
	itrRef = m_pTree->m_mapLinkerTo.find( this );
	if ( itrRef == m_pTree->m_mapLinkerTo.end() ) return 0;
	//遍历数据映射的from节点
	for ( itrRefPair = itrRef->second.begin(); itrRefPair != itrRef->second.end();
		++itrRefPair )
	{
		pNode = itrRefPair->first;
		if ( !pNode->IsParamNode() ) continue;
		pParamNode = (CCxStructTreeNodeParam*)pNode;
		if ( find( vSpecNodes.begin(), vSpecNodes.end(), pParamNode )
			== vSpecNodes.end() )
		{
			vSpecNodes.push_back( pParamNode );
		}
	}
	return (int)vSpecNodes.size();
}

int CCxStructTreeNodeParam::GetLinkerToNodes( vector< CCxStructTreeNodeParam* >& vSpecNodes )
{
	CCxStructTreeNodeParam* pParamNode = NULL;
	vector< CCxStructTreeNodeParam* > vLinkerNodes;
	if ( 0 == GetSiblingLinkerTo( vLinkerNodes ) )
	{
		return (int)vSpecNodes.size();
	}
	for ( vector< CCxStructTreeNodeParam* >::iterator itr = vLinkerNodes.begin();
		itr != vLinkerNodes.end(); ++itr )
	{
		pParamNode = *itr;
		if ( find( vSpecNodes.begin(), vSpecNodes.end(), pParamNode )
			== vSpecNodes.end() )
		{
			vSpecNodes.push_back( pParamNode );
			pParamNode->GetLinkerToNodes( vSpecNodes );
		}
	}
	return (int)vSpecNodes.size();
}

int CCxStructTreeNodeParam::GetLinkerFromNodes( vector< CCxStructTreeNodeParam* >& vSpecNodes )
{
	CCxStructTreeNodeParam* pParamNode = NULL;
	vector< CCxStructTreeNodeParam* > vLinkerNodes;
	if ( 0 == GetSiblingLinkerFrom( vLinkerNodes ) )
	{
		return (int)vSpecNodes.size();
	}
	for ( vector< CCxStructTreeNodeParam* >::iterator itr = vLinkerNodes.begin();
		itr != vLinkerNodes.end(); ++itr )
	{
		pParamNode = *itr;
		if ( find( vSpecNodes.begin(), vSpecNodes.end(), pParamNode )
			== vSpecNodes.end() )
		{
			vSpecNodes.push_back( pParamNode );
			pParamNode->GetLinkerFromNodes( vSpecNodes );
		}
	}
	return (int)vSpecNodes.size();
}

int CCxStructTreeNodeParam::GetNodeForEditValue( vector< CCxStructTreeNodeParam* >& vSpecNodes )
{
	vector< CCxStructTreeNodeParam* > vOutNodes;
	vector< CCxStructTreeNodeParam* > vOutLinkerToNodes;
	vector< CCxStructTreeNodeParam* > vLinkerToNodes;

	if ( IsInOut() || IsIn() )
	{
		GetSiblingOut( vOutNodes );
		//取得被编辑节点的所有同级输出节点。
		for ( vector< CCxStructTreeNodeParam* >::iterator itrOut = vOutNodes.begin();
			itrOut != vOutNodes.end(); ++itrOut )
		{
			if ( find( vSpecNodes.begin(), vSpecNodes.end(), *itrOut ) 
				== vSpecNodes.end() )
			{
				vSpecNodes.push_back( *itrOut );
				//取得一个输出节点的下一级映射节点
				vOutLinkerToNodes.clear();
				if ( 0 == (*itrOut)->GetSiblingLinkerTo( vOutLinkerToNodes ) )
				{
					continue;
				}
				for ( vector< CCxStructTreeNodeParam* >::iterator itrTo = vOutLinkerToNodes.begin();
					itrTo != vOutLinkerToNodes.end(); ++itrTo )
				{
					if ( find( vSpecNodes.begin(), vSpecNodes.end(), *itrTo ) 
						== vSpecNodes.end() )
					{
						vSpecNodes.push_back( *itrTo );
						(*itrTo)->GetNodeForEditValue( vSpecNodes );
					}
				}
			}
		}
	}

	//被编辑节点的下一级映射节点
	vLinkerToNodes.clear();
	GetSiblingLinkerTo( vLinkerToNodes );
	for ( vector< CCxStructTreeNodeParam* >::iterator itrTo = vLinkerToNodes.begin();
		itrTo != vLinkerToNodes.end(); ++itrTo )
	{
		if ( find( vSpecNodes.begin(), vSpecNodes.end(), *itrTo ) 
			== vSpecNodes.end() )
		{
			vSpecNodes.push_back( *itrTo );
			(*itrTo)->GetNodeForEditValue( vSpecNodes );
		}
	}
	return (int)vSpecNodes.size();
}


/*
建立新的数据映射时，改变被映射的节点及其下游节点的状态。
返回的节点集合中，不包括被影射的节点本身。
*/
int CCxStructTreeNodeParam::GetNodeForNewLinker( vector< CCxStructTreeNodeParam* >& vSpecNodes )
{
	vector< CCxStructTreeNodeParam* > vLinkerToNodes;
	vector< CCxStructTreeNodeParam* > vOutNodes;
	vector< CCxStructTreeNodeParam* > vOutLinkerToNodes;

	if ( IsInOut() || IsIn() )
	{
		GetSiblingOut( vOutNodes );
		for ( vector< CCxStructTreeNodeParam* >::iterator itrOut = vOutNodes.begin();
			itrOut != vOutNodes.end(); ++itrOut )
		{
			if ( find( vSpecNodes.begin(), vSpecNodes.end(), *itrOut ) 
				== vSpecNodes.end() )
			{
				vSpecNodes.push_back( *itrOut );
				vOutLinkerToNodes.clear();
				if ( 0 == (*itrOut)->GetSiblingLinkerTo( vOutLinkerToNodes ) )
				{
					continue;
				}

				for ( vector< CCxStructTreeNodeParam* >::iterator itrTo = vOutLinkerToNodes.begin();
					itrTo != vOutLinkerToNodes.end(); ++itrTo )
				{
					if ( find( vSpecNodes.begin(), vSpecNodes.end(), *itrTo ) 
						== vSpecNodes.end() )
					{
						vSpecNodes.push_back( *itrTo );
						(*itrTo)->GetNodeForNewLinker( vSpecNodes );
					}
				}
			}
		}
	}

	//当前节点的下一级映射节点
	vLinkerToNodes.clear();
	GetSiblingLinkerTo( vLinkerToNodes );
	for ( vector< CCxStructTreeNodeParam* >::iterator itrTo = vLinkerToNodes.begin();
		itrTo != vLinkerToNodes.end(); ++itrTo )
	{
		if ( find( vSpecNodes.begin(), vSpecNodes.end(), *itrTo ) 
			== vSpecNodes.end() )
		{
			vSpecNodes.push_back( *itrTo );
			(*itrTo)->GetNodeForNewLinker( vSpecNodes );
		}
	}
	return (int)vSpecNodes.size();
}

/*
如果当前参数是输入参数，判断有没有对应的数据映射的From参数节点。
*/
BOOL CCxStructTreeNodeParam::CanRun( void )
{
	CCxStructTreeNode* pParentWithoutGroup = GetParentWithoutGroup();

	if ( IsOut() ) return TRUE;

	vector< CCxStructTreeNodeParam* > vLinkerFromNodes;
	CCxStructTreeNodeParam* pFromNode = NULL;

	vLinkerFromNodes.clear();
	if ( 0 == GetSiblingLinkerFrom( vLinkerFromNodes ) )
		return TRUE;

	for ( vector< CCxStructTreeNodeParam* >::iterator itrFrom = vLinkerFromNodes.begin();
		itrFrom != vLinkerFromNodes.end(); ++itrFrom )
	{
		pFromNode = *itrFrom;
		pParentWithoutGroup = pFromNode->GetParentWithoutGroup();
		if ( pParentWithoutGroup == NULL )
		{
			return FALSE;
		}
		if ( pParentWithoutGroup->IsRootNode() ) //如果上游节点是根节点，不检测
		{
			continue;
		}
		BOOL bInvalid = ( ( pFromNode->m_dwStatus & NS_NAME_INVALID ) == NS_NAME_INVALID );
		if ( bInvalid ) return FALSE;
	}
	return TRUE;
}

void CCxStructTreeNodeParam::OnAfterEditValue()
{
	CCxStructTreeNode* pParentWithoutGroup = GetParentWithoutGroup();
	if ( pParentWithoutGroup == NULL )
	{
		::MessageBox(GetMainWnd(), "异常，父节点为空。\r\nCCxStructTreeNodeParam::OnAfterEditValue", g_lpszAppTitle, MB_OK|MB_ICONERROR);
		return;
	}

	if ( !SetRunFlag( FALSE ) )
	{
		return;
	}

	CCxStructTreeNodeParam* pParamNode = NULL;
	vector< CCxStructTreeNodeParam* > vSpecNodes;
	//vector< CCxStructTreeNode* > vCacheForSetStatus;
	GetNodeForEditValue( vSpecNodes );

	for ( vector< CCxStructTreeNodeParam* >::iterator itr = vSpecNodes.begin();
		itr != vSpecNodes.end(); ++itr )
	{
		pParamNode = *itr;
		if ( !pParamNode->SetRunFlag( FALSE ) )
		{
			return;
		}
	}

	CJsData_Svg_UpdateParamStatus svgUpdateParamStatus;
	CJsData_Svg_UpdateParamStatusData * pUpdateParamStatusData = NULL;
	for ( vector< CCxStructTreeNodeParam* >::iterator itr = vSpecNodes.begin();
		itr != vSpecNodes.end(); ++itr )
	{
		pParamNode = *itr;
		if ( !pParamNode->SetStatus( FALSE ) )
		{
			return;
		}
		pUpdateParamStatusData = new CJsData_Svg_UpdateParamStatusData();
		pUpdateParamStatusData->m_dwStatus = pParamNode->m_dwStatus;
		pUpdateParamStatusData->m_strId = pParamNode->m_strID;
		svgUpdateParamStatus.m_vData.push_back( pUpdateParamStatusData );
	}

	CString str = svgUpdateParamStatus.GetJsonStr();
	CDlgSvg* pSvg = GetSvgDilogPtr();
	if ( pSvg != NULL )
	{
		pSvg->JsMethod_updateParamStatus( str );
	}
}

void CCxStructTreeNodeParam::OnAfterRunCompleted( void )
{
	CCxStructTreeNode* pParentWithoutGroup = GetParentWithoutGroup();
	if ( pParentWithoutGroup == NULL )
	{
		::MessageBox(GetMainWnd(), "异常，父节点为空。\r\nCCxStructTreeNodeParam::OnAfterRunCompleted", g_lpszAppTitle, MB_OK|MB_ICONERROR);
		return;
	}

	CCxStructTreeNodeParam* pParamNode = NULL;
	vector< CCxStructTreeNodeParam* > vSpecNodes;

	CJsData_Svg_UpdateParamStatus svgUpdateParamStatus;
	CJsData_Svg_UpdateParamStatusData * pUpdateParamStatusData = NULL;

	//自身
	pUpdateParamStatusData = new CJsData_Svg_UpdateParamStatusData();
	pUpdateParamStatusData->m_dwStatus = this->m_dwStatus;
	pUpdateParamStatusData->m_strId = this->m_strID;
	svgUpdateParamStatus.m_vData.push_back( pUpdateParamStatusData );

	vSpecNodes.clear();
	GetSiblingLinkerTo( vSpecNodes );
	for ( vector< CCxStructTreeNodeParam* >::iterator itr = vSpecNodes.begin();
		itr != vSpecNodes.end(); ++itr )
	{
		pParamNode = *itr;
		if ( !pParamNode->SetStatus( TRUE ) )
		{
			return;
		}
		pUpdateParamStatusData = new CJsData_Svg_UpdateParamStatusData();
		pUpdateParamStatusData->m_dwStatus = pParamNode->m_dwStatus;
		pUpdateParamStatusData->m_strId = pParamNode->m_strID;
		svgUpdateParamStatus.m_vData.push_back( pUpdateParamStatusData );
	}

	CString str = svgUpdateParamStatus.GetJsonStr();
	CDlgSvg* pSvg = GetSvgDilogPtr();
	if ( pSvg != NULL )
	{
		pSvg->JsMethod_updateParamStatus( str );
	}
}


BOOL CCxStructTreeNodeParam::UpdateNode( LPCTSTR lpszData )
{
	CString strSrc = lpszData == NULL ? "" : lpszData;
	if ( strSrc.IsEmpty() ) return FALSE;
	if ( m_pTree == NULL ) return FALSE;

	Json::Reader reader;
	Json::Value obj;
	CString str;
	CCxStructTreeNode * pNewNode = NULL;
	CCxStructTreeNodeParam * pNewParamNode = NULL;
	int nNodeType = CCxStructTreeNode::NT_NULL;
	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) return FALSE;
		if (!obj.isObject()) return FALSE;

		str = (obj["nodeType"].isString()) ? obj["nodeType"].asCString() : _T("");
		nNodeType = CCxStructTreeNode::ConvertNodeType( (LPCTSTR)str );
		if ( nNodeType != NT_PARAM ) return FALSE;
		pNewNode = CCxStructTreeNode::GetNewNodeByType( nNodeType, m_pTree );
		if ( pNewNode == NULL )
		{
			return FALSE;
		}
		pNewParamNode = (CCxStructTreeNodeParam*)pNewNode;
		m_pTree->m_bNodeParseing = TRUE;
		if ( !pNewNode->OnParsing( obj/*reinterpret_cast< LPVOID > ( (Json::Value *)&obj )*/ ) )
		{
			m_pTree->m_bNodeParseing = FALSE; delete pNewNode; pNewNode = NULL; return FALSE;
		}
	}
	catch (...)
	{
		m_pTree->m_bNodeParseing = FALSE; delete pNewNode; pNewNode = NULL; return FALSE;
	}
	m_pTree->m_bNodeParseing = FALSE; 

	//
	if (
		this->m_strID != pNewNode->m_strID
		|| this->m_NodeType || pNewNode->m_NodeType
		|| this->m_strParentID || pNewNode->m_strParentID
		)
	{
		::MessageBox(GetMainWnd(), "更新参数失败，参数Id、父Id、类型不匹配", g_lpszAppTitle, MB_OK|MB_ICONERROR);
		delete pNewNode; pNewNode = NULL; return FALSE;
	}

	pNewParamNode->m_dwStatus = this->m_dwStatus;
	pNewParamNode->m_nIndex = this->m_nIndex;
	pNewParamNode->m_pParent = this->m_pParent;
	pNewParamNode->m_pRow = this->m_pRow;
	if ( this->IsArray() )
	{
		this->DeleteArrayData();
	}

	pNewParamNode->OnGetCopied( this );
	delete pNewParamNode; pNewParamNode = NULL;

	this->ParseValue();
	if ( m_pTree->m_pGrid != NULL )
	{
		CCxStructTreeRowT* pParentRow = (CCxStructTreeRowT*)this->m_pRow->GetParent();
		int nSiblingPos = this->GetSiblingPos();
		if ( m_pTree->m_pGrid->CanRemoveRow( this ) )
		{
			m_pTree->m_pGrid->DeleteNode( this );
		}
		m_pTree->m_pGrid->RebuildIndexes();
		if ( m_pTree->m_pGrid->CanAddNewRow( this ) )
		{
			m_pTree->m_pGrid->AddNode( this, pParentRow, nSiblingPos );
		}
		m_pTree->m_pGrid->RebuildIndexes();
	}

	return TRUE;
}

BOOL CCxStructTreeNodeParam::RunDataLinker( vector< CCxStructTreeNodeParam* >& vSpecNodes )
{
	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return FALSE;
	if ( m_pTree == NULL ) return FALSE;
	vector< CCxStructTreeNodeParam* > vToNodes;
	CCxStructTreeNodeParam* pTo = NULL;
	if ( 0 == GetSiblingLinkerTo( vToNodes ) ) return FALSE;
	vector< CCxStructTreeDataLinker* > vLinker;
	CCxStructTreeDataLinker* pLinker = NULL;
	
	CString strLinkerNewValue, str;
	CCxStructTreeNodeArrayElement * pElementNode = NULL;
	BOOL bArray = FALSE;

	for (vector< CCxStructTreeNodeParam* >::iterator itrTo = vToNodes.begin();
		itrTo != vToNodes.end(); ++itrTo )
	{
		pTo = *itrTo;
		m_pTree->GetDataLinker( vLinker, this, pTo );
		for (vector< CCxStructTreeDataLinker* >::iterator itrLinker = vLinker.begin();
			itrLinker != vLinker.end(); ++itrLinker )
		{
			pLinker = *itrLinker;
			if ( pLinker->m_strFormula.IsEmpty() )
			{
				strLinkerNewValue = this->m_strValue;
			}
			else
			{
				strLinkerNewValue = pSvg->JsMethod_runDataMappingExpr( pLinker->m_strFormula );
			}
			OnRunDataLinker( this, pTo, pLinker, strLinkerNewValue, vSpecNodes );
		}
	}

	return TRUE;
}

/*
数组映射说明：
动态数组只能根动态数组做整体映射；
动态数组元素不能跟标量做映射，也不能跟其他动、静态数组元素做映射；
静态数组可以跟静态数组做整体映射；
静态数组元素可以跟标量做映射，也可以跟其他静态数组元素做映射；
*/

BOOL CCxStructTreeNodeParam::OnRunDataLinker( CCxStructTreeNodeParam* pFrom, CCxStructTreeNodeParam* pTo
	, CCxStructTreeDataLinker* pLinker, LPCTSTR lpszNewValue
	, vector< CCxStructTreeNodeParam* >& vSpecNodes )
{
	BOOL bWholeLinker = FALSE; //整体映射
	BOOL bDimLinker = FALSE; //维度映射
	//数组 -- 数组
	if ( pFrom->IsArray() && pTo->IsArray() )
	{
		bWholeLinker = ( pLinker->m_strFromIndex.IsEmpty() 
			&& pLinker->m_strToIndex.IsEmpty() );

		bDimLinker = ( pLinker->m_strFromIndex.Find('*') != -1 
			&& pLinker->m_strToIndex.Find('*') != -1 );
		
		//整体映射
		if ( bWholeLinker )
		{
			pTo->UpdateValue( lpszNewValue );
			ParamNodePushVector( vSpecNodes, pTo );
			return TRUE;
		}
		//维度映射
		if ( bDimLinker )
		{
			//维度中的有效数据
			vector< CString > vDimData;
			CString sElem;
			Json::Reader reader;
			Json::Value arr;
			try
			{
				if (!reader.parse((char*)(LPCTSTR)lpszNewValue, arr, false)) return FALSE;
				if ( arr.isNull() || !arr.isArray() ) return FALSE;
				for ( Json::Value::iterator itr = arr.begin(); itr != arr.end(); ++itr)
				{
					if ( !(*itr).isString() ) continue;
					sElem = (*itr).isString() ? (*itr).asCString() : _T("");
					vDimData.push_back( sElem );
				}
			}
			catch (...)
			{
				return FALSE;
			}

			//To数组的每个元素坐标
			vector<int> vPos;
			if ( !ConvertArrayDim( vPos, pLinker->m_strToIndex ) )
			{
				return FALSE;
			}
			vector<int>::iterator itrToElem = find( vPos.begin(), vPos.end(), -1 );
			if ( itrToElem == vPos.end() ) return FALSE;

			CString* pElementData = NULL;
			CCxStructTreeNodeArrayElement* pElementNode = NULL;
			for ( int i=0; i<(int)vDimData.size(); i++ )
			{
				*itrToElem = i;
				pElementData = (CString*)((CxArrayND*)pTo->m_lpValueData)->GetDataPtr( vPos );
				pElementNode = pTo->GetElementNode( vPos );
				if ( pElementData != NULL && pElementNode != NULL )
				{
					*pElementData = pElementNode->m_strValue = vDimData[i];

					if ( pElementNode->IsRowMatched() )
					{
						pElementNode->m_pRow->GetItem( CCxStructTreeGridT::COL_VALUE )->SetValue( 
							(LPCTSTR)pElementNode->m_strValue, TRUE );
					}
				}
			}
			pTo->m_strValue = ((CxArrayND*)pTo->m_lpValueData)->GetArrayStr();
			ParamNodePushVector( vSpecNodes, pTo );
			return TRUE;
		}
		// 元素 -- 元素
		else if ( !pLinker->m_strFromIndex.IsEmpty() 
			&& !pLinker->m_strToIndex.IsEmpty() )
		{
			vector<int> vPos;
			if ( !ConvertArrayDim( vPos, pLinker->m_strToIndex ) )
			{
				return FALSE;
			}
			CCxStructTreeNodeArrayElement* pElementNode = pTo->GetElementNode( vPos );
			if ( pElementNode == NULL )
			{
				return FALSE;
			}
			CString* pElementData = (CString*)((CxArrayND*)pTo->m_lpValueData)->GetDataPtr( vPos );
			if ( pElementData == NULL )
			{
				return FALSE;
			}

			pElementNode->m_strValue = lpszNewValue == NULL ? "" : lpszNewValue;
			*pElementData = pElementNode->m_strValue;
			pTo->m_strValue = ((CxArrayND*)pTo->m_lpValueData)->GetArrayStr();

			if ( pElementNode->IsRowMatched() )
			{
				pElementNode->m_pRow->GetItem( CCxStructTreeGridT::COL_VALUE )->SetValue( 
					(LPCTSTR)pElementNode->m_strValue, TRUE );
			}
			ParamNodePushVector( vSpecNodes, pTo );
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	//数组元素 -- 标量
	else if ( pFrom->IsArray() && !pTo->IsArray() )
	{
		pTo->UpdateValue( lpszNewValue );
		ParamNodePushVector( vSpecNodes, pTo );
		return TRUE;
	}
	//标量 -- 数组元素
	else if ( !pFrom->IsArray() && pTo->IsArray() )
	{
		vector<int> vPos;
		if ( !ConvertArrayDim( vPos, pLinker->m_strToIndex ) )
		{
			return FALSE;
		}
		CCxStructTreeNodeArrayElement* pElementNode = pTo->GetElementNode( vPos );
		if ( pElementNode == NULL )
		{
			return FALSE;
		}
		CString* pElementData = (CString*)((CxArrayND*)pTo->m_lpValueData)->GetDataPtr( vPos );
		if ( pElementData == NULL )
		{
			return FALSE;
		}

		pElementNode->m_strValue = lpszNewValue == NULL ? "" : lpszNewValue;
		*pElementData = pElementNode->m_strValue;
		pTo->m_strValue = ((CxArrayND*)pTo->m_lpValueData)->GetArrayStr();

		if ( pElementNode->IsRowMatched() )
		{
			pElementNode->m_pRow->GetItem( CCxStructTreeGridT::COL_VALUE )->SetValue( 
				(LPCTSTR)pElementNode->m_strValue, TRUE );
		}
		ParamNodePushVector( vSpecNodes, pTo );
		return TRUE;
	}

	//标量 -- 标量
	else if ( !pFrom->IsArray() && !pTo->IsArray() )
	{
		pTo->UpdateValue( lpszNewValue );
		ParamNodePushVector( vSpecNodes, pTo );
		return TRUE;
	}
	return TRUE;
}

BOOL CCxStructTreeNodeParam::IsLoopLinker( void )
{
	vector< CCxStructTreeNodeParam* > vFroms;
	vector< CCxStructTreeNodeParam* > vTos;
	GetLinkerFromNodes( vFroms );
	GetLinkerToNodes( vTos );

	if ( find( vFroms.begin(), vFroms.end(), this )
		!= vFroms.end() )
	{
		return TRUE;
	}
	if ( find( vTos.begin(), vTos.end(), this )
		!= vTos.end() )
	{
		return TRUE;
	}

	for ( int i=0; i<(int)vFroms.size(); i++ )
	{
		if ( find( vTos.begin(), vTos.end(), vFroms[i] )
			!= vTos.end() )
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CCxStructTreeNodeParam::IsInvalidFrom( void )
{ 
	vector< CCxStructTreeNodeParam* > vFroms;
	if ( 0 == GetSiblingLinkerFrom( vFroms ) ) return FALSE;
	for ( int i=0; i<(int)vFroms.size(); i++ )
	{
		if ( vFroms[i]->IsInvalidFrom() ) return TRUE;
	}
	return FALSE;
}

BOOL CCxStructTreeNodeParam::IsInvalid( void )
{
	return TRUE;
}

void CCxStructTreeNodeParam::OnAfterNewLinker( void )
{
	vector< CCxStructTreeNodeParam* > vSpecNodes;
	GetNodeForNewLinker( vSpecNodes );

	CJsData_Svg_UpdateParamStatus svgUpdateParamStatus;
	CJsData_Svg_UpdateParamStatusData * pUpdateParamStatusData = NULL;

	this->SetStatus( FALSE );
	pUpdateParamStatusData = new CJsData_Svg_UpdateParamStatusData();
	pUpdateParamStatusData->m_dwStatus = this->m_dwStatus;
	pUpdateParamStatusData->m_strId = this->m_strID;
	svgUpdateParamStatus.m_vData.push_back( pUpdateParamStatusData );
	
	for ( int i=0; i<(int)vSpecNodes.size(); i++ )
	{
		vSpecNodes[i]->SetStatus( FALSE );

		pUpdateParamStatusData = new CJsData_Svg_UpdateParamStatusData();
		pUpdateParamStatusData->m_dwStatus = vSpecNodes[i]->m_dwStatus;
		pUpdateParamStatusData->m_strId = vSpecNodes[i]->m_strID;
		svgUpdateParamStatus.m_vData.push_back( pUpdateParamStatusData );
	}

	CString str = svgUpdateParamStatus.GetJsonStr();
	CDlgSvg* pSvg = GetSvgDilogPtr();
	if ( pSvg != NULL )
	{
		pSvg->JsMethod_updateParamStatus( str );
	}
}

void CCxStructTreeNodeParam::ParamNodePushVector( vector< CCxStructTreeNodeParam* >& vSpecNodes, CCxStructTreeNodeParam*& pParam )
{
	if ( pParam == NULL ) return;
	if ( find( vSpecNodes.begin(), vSpecNodes.end(), pParam )
		!= vSpecNodes.end() ) return;
	vSpecNodes.push_back( pParam );
}

CCxStructTreeNode* CCxStructTreeNodeParam::GetParentWithoutGroup( void )
{
	CCxStructTreeNode* pParent = m_pParent;
	while ( pParent != NULL )
	{
		if ( pParent->IsGroupNode() )
		{
			pParent = pParent->m_pParent;
			continue;
		}
		else
		{
			break;
		}
	}
	return pParent;
}
