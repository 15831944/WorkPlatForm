#include "StdAfx.h"
#include "CxStructTreeNodeArrayElement.h"
#include <CxStructTreeNodeParam.h>
#include <ConvertData.h>


CCxStructTreeNodeArrayElement::CCxStructTreeNodeArrayElement( CCxStructTree * pTree )
	: CCxStructTreeNode( pTree )
{

}

CCxStructTreeNodeArrayElement::~CCxStructTreeNodeArrayElement()
{
	Clear();
}

void CCxStructTreeNodeArrayElement::Clear( void )
{
	m_strValue = "";
	CCxStructTreeNode::Clear();
}

CString CCxStructTreeNodeArrayElement::GetTooltipText( void )
{
	CCxStructTreeNodeParam * pParamNode = GetParamNode();
	if ( pParamNode == NULL ) return "";

	vector<int> vPos;
	BOOL bElem = TRUE;
	if ( !GetPos( vPos, bElem ) ) return FALSE;

	CxArrayND * pArray = (CxArrayND *)pParamNode->m_lpValueData;
	if ( pArray == NULL ) return FALSE;

	CString strValue;
	CString strDataType = ConvertParamDataType( pParamNode->GetElementDataType() );
	if ( vPos.size() > 0 && bElem )
	{
		strValue = pArray->GetDataAsCString( vPos );
	}

	CString strTip;

	if ( bElem )
	{
		strTip.Format( "���ƣ�%s\r\n�������ͣ�%s\r\nֵ��%s"
			, m_strText
			, strDataType
			, strValue
			);
	}
	else
	{
		strTip.Format( "���ƣ�%s"
			, m_strText
			);
	}

	return strTip;
}

BOOL CCxStructTreeNodeArrayElement::GetPos( vector<int>& vPos, BOOL& bElem )
{
	vPos.clear();
	bElem = TRUE;
	CCxStructTreeNodeParam * pParamNode = GetParamNode();
	if ( pParamNode == NULL ) return FALSE;
	if ( !pParamNode->IsArray() ) return FALSE;

	CxArrayND * pArray = (CxArrayND *)pParamNode->m_lpValueData;
	if ( pArray == NULL ) return FALSE;

	int nPos = m_strText.Find( pParamNode->m_strOriName );
	if ( nPos != 0 ) return FALSE;

	CString strDims = m_strText.Mid( pParamNode->m_strOriName.GetLength()
		, m_strText.GetLength() - pParamNode->m_strOriName.GetLength() );
	if ( strDims.IsEmpty() ) return FALSE;

	CString str;
	int nLeft, nRight;
	nRight = strDims.ReverseFind(']');
	nLeft = strDims.ReverseFind('[');
	if (nRight <= -1 || nLeft <= -1 || nRight <= nLeft + 1) return FALSE;
	if (nRight + 1 != strDims.GetLength()) return FALSE;

	//ȡ�����ݸ���
	str = strDims.Mid(nLeft + 1, nRight - nLeft - 1);
	if ( str.IsEmpty() ) return FALSE;
	bElem = ( str.Find("*") == -1 );

	//ά��
	vector< CString > vStr;
	int nDimCnt = CString2Vector(vStr, str);
	vector<int> vTempPos;
	//ÿһά���������ݸ�����
	vTempPos.resize( nDimCnt );
	for ( int i=0; i<nDimCnt; i++ )
	{
		if ( vStr[i] == "*" )
			vTempPos.push_back(-1);
		else
			vTempPos[i] = strtol((char *)(LPCTSTR)vStr[i], NULL, 10);
	}
	vPos = vTempPos;
	return TRUE;
}

CCxStructTreeNodeParam * CCxStructTreeNodeArrayElement::GetParamNode( void )
{
	CCxStructTreeNodeParam * pParamNode = NULL;
	CCxStructTreeNode * pParent = m_pParent;
	while ( pParent != NULL )
	{
		if ( pParent->IsParamNode() )
		{
			pParamNode = (CCxStructTreeNodeParam*)pParent;
			break;
		}
		pParent = pParent->m_pParent;
	}
	return pParamNode;
}

