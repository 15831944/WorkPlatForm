#include "StdAfx.h"
#include "CxArrayND.h"
#include "ConvertData.h"
#include "value.h"
#include "reader.h"
#include "writer.h"
#include "PeraProcessDesignerUtility.h"
#include "PeraProcessDesigner.h"

/*
参数：
    vDimSize：每维的数据个数。
	nDimIndex：0起始的维度索引号。
	nDataType：数据类型。
*/
CxArrayDim::CxArrayDim( vector<int>& vDimSize, CxArrayDim * pParent, int& nDataType
	, const BOOL bCreateDefaultData )
{
	m_pParent = pParent;
	if ( m_pParent == NULL )
	{
		m_nDimIndex = 0;
	}
	else
	{
		m_nDimIndex = m_pParent->m_nDimIndex + 1;
	}
	if ( m_nDimIndex < (int)vDimSize.size() )
	{
		m_vData.resize( vDimSize[ m_nDimIndex ] );
	}
	else
	{
		return;
	}

	if ( (int)vDimSize.size() == m_nDimIndex + 1 ) //最后一个维度
	{
		m_nRealDataType = nDataType;
		if ( bCreateDefaultData ) SetDefaultData();
	}
	else
	{
		m_nRealDataType = DT_DIM;
		for ( int i=0; i<(int)m_vData.size(); i++ )
		{
			CxArrayDim * pDim = new CxArrayDim( vDimSize, this, nDataType, bCreateDefaultData );
			m_vData[i] = reinterpret_cast< LPARAM > ( pDim );
		}
	}
}

CxArrayDim::~CxArrayDim()
{
	Clear();
}


void CxArrayDim::Clear( void )
{
	ClearElement();
	m_vData.clear();
	m_pParent = NULL;
	m_nRealDataType = -1;
	m_nDimIndex = -1;
}


void CxArrayDim::ClearElement( void )
{
	switch ( m_nRealDataType )
	{
	case DT_BOOLEAN:
		for (int i=0; i<(int)m_vData.size(); i++)
		{
			CString * pData = reinterpret_cast< CString * > ( m_vData[i] );
			delete pData; pData = NULL;
			m_vData[i] = NULL;
		}
		break;
	case DT_DOUBLE:
		for (int i=0; i<(int)m_vData.size(); i++)
		{
			CString * pData = reinterpret_cast< CString * > ( m_vData[i] );
			delete pData; pData = NULL;
			m_vData[i] = NULL;
		}
		break;
	case DT_INT:
		for (int i=0; i<(int)m_vData.size(); i++)
		{
			CString * pData = reinterpret_cast< CString * > ( m_vData[i] );
			delete pData; pData = NULL;
			m_vData[i] = NULL;
		}
		break;
	case DT_STRING:
		for (int i=0; i<(int)m_vData.size(); i++)
		{
			CString * pData = reinterpret_cast< CString * > ( m_vData[i] );
			delete pData; pData = NULL;
			m_vData[i] = NULL;
		}
		break;
	case DT_DIM:
		for (int i=0; i<(int)m_vData.size(); i++)
		{
			CxArrayDim * pData = reinterpret_cast< CxArrayDim * > ( m_vData[i] );
			delete pData; pData = NULL;
			m_vData[i] = NULL;
		}
		break;
	default:
		break;
	}
}


void CxArrayDim::Parse( vector<CString>& vSrc, int& nPos )
{
	if ( m_nRealDataType == DT_DIM )
	{
		for (int i=0; i<(int)m_vData.size(); i++)
		{
			CxArrayDim * pDim = reinterpret_cast< CxArrayDim * > ( m_vData[i] );
			pDim->Parse( vSrc, nPos );
		}
	}
	else
	{
		for (int i=0; i<(int)m_vData.size(); i++)
		{
			if ( nPos >= (int)vSrc.size() ) return;

			switch ( m_nRealDataType )
			{
			case DT_BOOLEAN:
				{
					//BOOL * pData = new BOOL();
					//if ( 0 == vSrc[nPos].CompareNoCase("true") )
					//	*pData = TRUE;
					//else if ( 0 == vSrc[nPos].CompareNoCase("false") )
					//	*pData = FALSE;
					CString * pData = new CString();
					*pData = vSrc[nPos];
					m_vData[i] = reinterpret_cast< LPARAM > ( pData );
				}
				break;
			case DT_INT:
				{
					//int * pData = new int();
					//*pData = strtol((char *)(LPCTSTR)vSrc[nPos], NULL, 10);
					CString * pData = new CString();
					*pData = vSrc[nPos];
					m_vData[i] = reinterpret_cast< LPARAM > ( pData );
				}
				break;
			case DT_DOUBLE:
				{
					CString * pData = new CString();
					*pData = vSrc[nPos];
					m_vData[i] = reinterpret_cast< LPARAM > ( pData );
				}
				break;
			case DT_STRING:
				{
					CString * pData = new CString();
					*pData = vSrc[nPos];
					m_vData[i] = reinterpret_cast< LPARAM > ( pData );
				}
				break;
			default:
				break;
			}
			nPos++;
		}
	}
}


LPARAM CxArrayDim::GetDataPtr( vector< int >& vPos )
{
	if ( m_nRealDataType == DT_DIM )
	{
		if ( m_nDimIndex + 1 >= (int)vPos.size() ) return NULL;
		if ( vPos[m_nDimIndex] >= (int)m_vData.size() ) return NULL;
		
		CxArrayDim * pDim = reinterpret_cast < CxArrayDim * > ( m_vData[ vPos[m_nDimIndex] ] );
		return pDim->GetDataPtr( vPos );
	}
	else
	{
		if ( m_nDimIndex >= (int)vPos.size() ) return NULL;
		if ( vPos[m_nDimIndex] >= (int)m_vData.size() ) return NULL;
		return m_vData[ vPos[m_nDimIndex] ];
	}
}

int CxArrayDim::GetDataCount( int nDim )
{
	if ( nDim == -1 )
	{
		if ( m_nRealDataType == DT_DIM )
		{
			if ( m_vData.size() == 0 ) return 0;
			CxArrayDim * pDim = reinterpret_cast< CxArrayDim * > ( m_vData[0] );
			return pDim->GetDataCount() * (int)m_vData.size();
		}
		return (int)m_vData.size();
	}
	else
	{
		if ( m_nDimIndex == nDim )
		{
				return (int)m_vData.size();
		}
		else
		{
			if ( m_vData.size() == 0 ) return 0;
			if ( m_nRealDataType != DT_DIM ) return 0;
			CxArrayDim * pDim = reinterpret_cast< CxArrayDim * > ( m_vData[0] );
			return pDim->GetDataCount( nDim );
		}
	}
}

CString CxArrayDim::GetArrayStr( void )
{
	CString strData, str;
	CxArrayDim * pDim = NULL;

	if ( m_nRealDataType == DT_DIM )
	{
		for (int i=0; i<(int)m_vData.size(); i++)
		{
			pDim = reinterpret_cast < CxArrayDim * > ( m_vData[ i ] );
			str = pDim->GetArrayStr();
			if ( str.IsEmpty() ) return "";
			if ( strData.IsEmpty() ) strData += str;
			else strData += "," + str;
		}
	}
	else
	{
		switch ( m_nRealDataType )
		{
		case DT_BOOLEAN:
			for (int i=0; i<(int)m_vData.size(); i++)
			{
				str = * reinterpret_cast < CString * > ( m_vData[ i ] );
				if ( str.IsEmpty() ) str = "false";
				strData += str;
				if ( i + 1 < (int)m_vData.size() )  strData += ",";
			}
			break;
		case DT_INT:
			for (int i=0; i<(int)m_vData.size(); i++)
			{
				str = * reinterpret_cast < CString * > ( m_vData[ i ] );
				if ( str.IsEmpty() ) str = "0";
				strData += str;
				if ( i + 1 < (int)m_vData.size() )  strData += ",";
			}
			break;
		case DT_DOUBLE:
			for (int i=0; i<(int)m_vData.size(); i++)
			{
				str = * reinterpret_cast < CString * > ( m_vData[ i ] );
				if ( str.IsEmpty() ) str = "0";
				strData += str;
				if ( i + 1 < (int)m_vData.size() )  strData += ",";
			}
			break;
		case DT_STRING:
			{
				Json::Value jstr( Json::arrayValue );
				Json::FastWriter fw;

				for (int i=0; i<(int)m_vData.size(); i++)
				{
					str = * reinterpret_cast < CString * > ( m_vData[ i ] );

					jstr.append( (LPCTSTR)str );
		
					//strData += "\"" + str + "\"";
					//if ( i + 1 < (int)m_vData.size() )  strData += ",";
				}

				str = fw.write( jstr ).c_str();
				str.Trim( 0x0a );
				str.TrimLeft('[');
				str.TrimRight(']');
				strData = str;
			}


			break;
		default:
			return "";
		}
	}
	return strData;
}

BOOL CxArrayDim::Resize( vector<int>& vDimSize, int nDataType )
{
	if ( m_nDimIndex < 0 || m_nDimIndex >= (int)vDimSize.size() ) return FALSE;
	int nNewSize = vDimSize[m_nDimIndex];
	int nOldSize = (int)m_vData.size();
	CString str;

	if ( nNewSize > nOldSize )
	{
		for ( int i=0; i < nNewSize - nOldSize; i++ )
		{
			CxArrayDim * pDim = new CxArrayDim(vDimSize, this, nDataType, TRUE );
			m_vData.push_back( reinterpret_cast< LPARAM > ( pDim ) );
		}
	}
	else if ( nNewSize < nOldSize )
	{
		for ( vector<LPARAM>::iterator itr = m_vData.begin() + nNewSize; 
			itr != m_vData.end(); itr = m_vData.begin() + nNewSize )
		{
			switch ( m_nRealDataType )
			{
				case DT_DIM:
				{
					CxArrayDim * pDim = reinterpret_cast< CxArrayDim * > ( *itr );
					delete pDim;
				}
				break;
			case DT_BOOLEAN:
				{
					CString * pStr = reinterpret_cast< CString * > ( *itr );
					delete pStr;
				}
				break;
			case DT_INT:
				{
					CString * pStr = reinterpret_cast< CString * > ( *itr );
					delete pStr;
				}
				break;
			case DT_DOUBLE:
				{
					CString * pStr = reinterpret_cast< CString * > ( *itr );
					delete pStr;
				}
				break;
			case DT_STRING:
				{
					CString * pStr = reinterpret_cast< CString * > ( *itr );
					delete pStr;
				}
				break;
			default:
				return FALSE;
			}
			m_vData.erase(itr);
		}
	}

	int nDimBound = min( nNewSize, nOldSize );
	if ( m_nRealDataType == DT_DIM )
	{
		for (int i=0; i<nDimBound; i++)
		{
			CxArrayDim * pDim = reinterpret_cast< CxArrayDim * > ( m_vData[i] );
			pDim->Resize( vDimSize, nDataType );
		}
	}	

	//if ( m_nDimIndex + 1 == (int)vDimSize.size() )
	//{
	//	//最后一维数据
	//	m_vData.resize( nNewSize );

	//	if ( (int)m_vData.size() < nNewSize )
	//	{

	//		switch ( m_nRealDataType )
	//		{
	//		case DT_BOOLEAN:
	//			for (int i = nOldSize; i<(int)m_vData.size(); i++)
	//			{
	//				CString * pData = new CString();
	//				*pData = "";
	//				m_vData[i] = reinterpret_cast< LPARAM > ( pData );
	//			}
	//			break;
	//		case DT_INT:
	//			for (int i=0; i<(int)m_vData.size(); i++)
	//			{
	//				CString * pData = new CString();
	//				*pData = "";
	//				m_vData[i] = reinterpret_cast< LPARAM > ( pData );
	//			}
	//			break;
	//		case DT_DOUBLE:
	//			for (int i=0; i<(int)m_vData.size(); i++)
	//			{
	//				CString * pData = new CString();
	//				*pData = "";
	//				m_vData[i] = reinterpret_cast< LPARAM > ( pData );
	//			}
	//			break;
	//		case DT_STRING:
	//			for (int i=0; i<(int)m_vData.size(); i++)
	//			{
	//				CString * pData = new CString();
	//				*pData = "";
	//				m_vData[i] = reinterpret_cast< LPARAM > ( pData );
	//			}
	//			break;
	//		default:
	//			break;
	//		}
	//	}
	//}

	//
	//
	//if ( (int)m_vData.size() == nNewSize )
	//{
	//	return TRUE;
	//}



	//else if ( (int)m_vData.size() > nNewSize )
	//{
	//	//扩容
	//	m_vData.resize( nNewSize );
	//	return TRUE;
	//}
	//else if ( (int)m_vData.size() < nNewSize )
	//{
	//	//削减
	//	for ()
	//	return TRUE;
	//}
	//
	//for (int i=0; i<(int)m_vData.size(); i++)
	//{

	//}
	////是否为最后一维数据
	//if ( m_nDimIndex + 1 == (int)vDimSize.size() )
	//{

	//	for (int i=0; i<(int)m_vData.size(); i++)
	//	{
	//		CxArrayDim * pDim = reinterpret_cast< CxArrayDim * > ( m_vData[i] );
	//		pDim->Parse( vSrc, nPos );
	//	}
	//}
	//else

	return TRUE;
}

BOOL CxArrayDim::SetDefaultData( void )
{
	if ( m_vData.size() == 0 ) return FALSE;

	switch ( m_nRealDataType )
	{
	case DT_BOOLEAN:
		ClearElement();
		for (int i=0; i<(int)m_vData.size(); i++)
		{
			CString * pData = new CString();
			*pData = "false";
			m_vData[i] = reinterpret_cast<LPARAM> ( pData );
		}
		break;

	case DT_DOUBLE:
		ClearElement();
		for (int i=0; i<(int)m_vData.size(); i++)
		{
			CString * pData = new CString();
			*pData = "0.0";
			m_vData[i] = reinterpret_cast<LPARAM> ( pData );
		}
		break;

	case DT_INT:
		ClearElement();
		for (int i=0; i<(int)m_vData.size(); i++)
		{
			CString * pData = new CString();
			*pData = "0";
			m_vData[i] = reinterpret_cast<LPARAM> ( pData );
		}
		break;

	case DT_STRING:
		ClearElement();
		for (int i=0; i<(int)m_vData.size(); i++)
		{
			CString * pData = new CString();
			*pData = "";
			m_vData[i] = reinterpret_cast<LPARAM> ( pData );
		}
		break;

	default:
		return FALSE;
	}
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CxArrayND::CxArrayND( int nDataType )
	: CCxArrayBase(nDataType)
{
	m_nDimCount = 0;
	m_pData = NULL;
	m_bIsDynamicArray = FALSE;
	m_nDir = PARAM_DIR_NULL;
	m_pNode = NULL;
	m_bResized = FALSE;
}


CxArrayND::~CxArrayND(void)
{
	Clear();
}


void CxArrayND::Clear( void )
{
	if ( m_pData != NULL )
	{
		delete m_pData; m_pData = NULL;
	}
	m_nDimCount = 0;
	//m_bIsDynamicArray = FALSE;
	//m_nDir = PARAM_DIR_NULL;
	m_bResized = FALSE;
	
}

/*
实现说明:
从原始字符串中，分离出“数据”、“维度”2部分字符串。
解析维度字符串，获得“维数”和每一维包含的数据个数。
将第一维数据保存在 m_pData 中，在 CxArrayDim 的构造函数中，会自动创建子维度数据。
*/
BOOL CxArrayND::Parse( LPCTSTR lpszSrc )
{
	Clear();

	CString strSrc = lpszSrc == NULL ? "" : lpszSrc;
	if (strSrc.IsEmpty()) return FALSE;
	//维度字符串
	CString str;
	if (!ExtractDimStr(str, strSrc)) return FALSE;
	//维数
	vector< CString > vStr;
	m_nDimCount = CString2Vector(vStr, str);
	//每一维包含的数据个数。
	vector< int > vDimSize;
	vDimSize.resize( m_nDimCount );
	for ( int i=0; i<m_nDimCount; i++ )
	{
		vDimSize[i] = strtol((char *)(LPCTSTR)vStr[i], NULL, 10);
	}
	int nDataType = GetRealDataType();
	m_pData = new CxArrayDim( vDimSize, NULL, nDataType );

	//取得数据
	CString strData;
	if (!ExtractDataStr(strData, strSrc)) return FALSE;

	switch (nDataType)
	{
	case DT_DOUBLE:
	case DT_INT:
	case DT_BOOLEAN:
		if ( m_pData->GetDataCount() != CString2Vector(vStr, strData) ) return FALSE;
		break;
	case DT_STRING:
		ParseDataVector( vStr, strData );
		if ( m_pData->GetDataCount() != (int)vStr.size() ) return FALSE;
		break;
	default:
		return FALSE;
	}
	
	int nPos = 0;
	m_pData->Parse( vStr, nPos );

	return TRUE;
}

int CxArrayND::GetDimCount( void )
{
	return m_nDimCount;
}

LPARAM CxArrayND::GetDataPtr( vector<int>& nPos )
{
	if ( m_pData == NULL ) return NULL;
	return m_pData->GetDataPtr( nPos );
}

BOOL CxArrayND::ParseDataVector( vector< CString >& vData, LPCTSTR lpszSrc )
{
	vData.clear();

	CString strSrc = lpszSrc == NULL ? "" : lpszSrc;
	if (strSrc.IsEmpty()) return FALSE;
	strSrc = "[" + strSrc + "]";

	//strSrc = "{\"id\":\"1\"}";

	Json::Reader reader;
	Json::Value arr;
	CString str;
	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, arr, false)) return FALSE;
		if (!arr.isArray()) return FALSE;
		
		for ( Json::Value::iterator itr = arr.begin(); itr != arr.end(); ++itr)
		{
			if ( (*itr).isString() )
				str = (*itr).asCString();
			else if (  (*itr).isBool() )
				str = (*itr).asBool() ? "true" : "false";
			else if (  (*itr).isDouble() )
				str.Format( "%f", (*itr).asDouble() );
			else if (  (*itr).isInt() )
				str.Format( "%d", (*itr).asInt() ); 

			vData.push_back( str );
		}
	}
	catch (...)
	{
		Clear();
		return FALSE;
	}
	return TRUE;
}

int CxArrayND::GetDataCount( int nDim /*= -1 */ )
{
	if ( m_pData == 0 ) return 0;
	return m_pData->GetDataCount( nDim );
}

CString CxArrayND::GetDataAsCString( vector<int>& nPos )
{
	LPARAM lp = GetDataPtr( nPos );
	if ( lp == NULL ) return "";
	int nDataType = GetRealDataType();
	switch ( nDataType )
	{
	case DT_BOOLEAN:
	case DT_INT:
	case DT_DOUBLE:
	case DT_STRING:
		{
			CString * pStr = reinterpret_cast< CString * > ( lp );
			return *pStr;
		}
	default:
		return "";
	}
}

CString CxArrayND::GetArrayStr( void )
{
	if ( m_pData == NULL ) return "";

	CString strDimStr = GetDimStr();
	if ( strDimStr.IsEmpty() ) return "";

	CString strData = m_pData->GetArrayStr();
	if ( strData.IsEmpty() ) return "";

	CString str;
	str = "[" + strData + "]" + "(" + strDimStr + ")";
	return str;
}

CString CxArrayND::GetDimStr( void )
{
	if ( m_pData == NULL ) return "";

	int nDimCount = GetDimCount();
	CString strDimStr;
	switch ( nDimCount )
	{
	case 0:
		return "";
	case 1:
		strDimStr.Format("%d", GetDataCount(0)); break;
	case 2:
		strDimStr.Format("%d,%d", GetDataCount(0), GetDataCount(1)); break;
	case 3:
		strDimStr.Format("%d,%d,%d", GetDataCount(0), GetDataCount(1), GetDataCount(2)); break;
	default:
		return "";
	}
	return strDimStr;
}

BOOL CxArrayND::Resize( vector<int>& vDimSize )
{
	//检查维度是否为空
	if ( vDimSize.size() == 0 )
	{
		Clear();
		return TRUE;
	}
	
	//检查边界
	for (int i=0; i<(int)vDimSize.size(); i++)
	{
		if ( vDimSize[i] <= 0 || vDimSize[i] >= _MaxDimSize )
		{
			::MessageBox( GetMainWnd(),"参数越界，无法调整数组尺寸", g_lpszAppTitle, MB_OK|MB_ICONERROR);
			return FALSE;
		}
	}

	//检查元素类型
	int nRealDataType = GetRealDataType();
	if ( nRealDataType == 0 )
	{
		::MessageBox( GetMainWnd(), "指定的元素类型不存在，无法调整数组尺寸", g_lpszAppTitle, MB_OK|MB_ICONERROR);
		return FALSE;
	}

	//如果需要改变维度，则清空数据
	if ( GetDimCount() != (int)vDimSize.size() )
	{
		Clear();
		m_nDimCount = (int)vDimSize.size();
	}

	//如果数组原来是空的，则创建
	if ( m_pData == NULL )
	{
		m_pData = new CxArrayDim( vDimSize, NULL, nRealDataType, TRUE );
		return TRUE;
	}

	//m_pData->Resize( vDimSize, nRealDataType );

	CxArrayDim * pNewCache = new CxArrayDim( vDimSize, NULL, nRealDataType, TRUE );

	//保留原有数据，调整尺寸
	vector<int> vPos;
	CString * pOld;
	CString * pNew;
	int nDim1R, nDim2R, nDim3R;
	switch (m_nDimCount)
	{
	case 1:
		nDim1R = min( (int)vDimSize[0], GetDataCount(0) );
		for (int i = 0; i < nDim1R; i++)
		{
			vPos.clear(); vPos.push_back(i);
			pOld = reinterpret_cast<CString *> ( GetDataPtr( vPos ) );
			pNew = reinterpret_cast<CString *> ( pNewCache->GetDataPtr( vPos ) );
			*pNew = *pOld;
		}
		break;
	case 2:
		nDim1R = min( (int)vDimSize[0], GetDataCount(0) );
		nDim2R = min( (int)vDimSize[1], GetDataCount(1) );
		for (int i = 0; i < nDim1R; i++)
		{
			for (int j = 0; j < nDim2R; j++)
			{
				vPos.clear(); vPos.push_back(i); vPos.push_back(j);
				pOld = reinterpret_cast<CString *> ( GetDataPtr( vPos ) );
				pNew = reinterpret_cast<CString *> ( pNewCache->GetDataPtr( vPos ) );
				*pNew = *pOld;
			}

		}
		break;
	case 3:
		nDim1R = min( (int)vDimSize[0], GetDataCount(0) );
		nDim2R = min( (int)vDimSize[1], GetDataCount(1) );
		nDim3R = min( (int)vDimSize[2], GetDataCount(2) );
		for (int i = 0; i < nDim1R; i++)
		{
			for (int j = 0; j < nDim2R; j++)
			{
				for (int k = 0; k < nDim3R; k++)
				{
					vPos.clear(); vPos.push_back(i); vPos.push_back(j); vPos.push_back(k);
					pOld = reinterpret_cast<CString *> ( GetDataPtr( vPos ) );
					pNew = reinterpret_cast<CString *> ( pNewCache->GetDataPtr( vPos ) );
					*pNew = *pOld;
				}
			}
		}
		break;

	default:
		break;
	}

	Clear();
	m_nDimCount = (int)vDimSize.size();
	m_pData = pNewCache;
	pNewCache = NULL;

	m_bResized = TRUE;

	return TRUE;
}
CxArrayND *CxArrayND::GetCopied()
{
	CxArrayND * pCopiedArry = new CxArrayND(*this);
	return pCopiedArry;
}
CxArrayND::CxArrayND(CxArrayND & other)
: CCxArrayBase(other.m_DataType)
{
	m_nDimCount = other.m_nDimCount;
	m_bIsDynamicArray = other.m_bIsDynamicArray;
	m_nDir =  other.m_nDir;
	m_pNode = other.m_pNode;
	m_bResized = other.m_bResized;
	m_pData = NULL;
	CString strArryStr = other.GetArrayStr();
	this->Parse( (LPCTSTR)strArryStr);
}