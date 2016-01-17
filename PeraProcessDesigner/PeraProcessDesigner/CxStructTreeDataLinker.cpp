#include "StdAfx.h"
#include "CxStructTreeDataLinker.h"
#include <ConvertData.h>

CCxStructTreeDataLinker::CCxStructTreeDataLinker()
{
}

CCxStructTreeDataLinker::~CCxStructTreeDataLinker()
{
	Clear();
}

void CCxStructTreeDataLinker::Clear( void )
{
	m_strFrom = "";
	m_strTo = "";
	m_strOwnerId = "";
	m_strToIndex = "";
	m_strFromIndex = "";
	m_strFormula = "";
}

void CCxStructTreeDataLinker::GetElementPos( vector<int>& vFrom, vector<int>& vTo )
{
	vector<CString> vStr;
	vFrom.clear(); vTo.clear();

	if ( 0 != CString2Vector( vStr, m_strFromIndex ) )
	{
		for (int i=0; i<(int)vStr.size(); i++)
		{
			vFrom.push_back( strtol((char*)(LPCTSTR)vStr[i], NULL, 10 ) );
		}
	}
	if ( 0 != CString2Vector( vStr, m_strToIndex ) )
	{
		for (int i=0; i<(int)vStr.size(); i++)
		{
			vTo.push_back( strtol((char*)(LPCTSTR)vStr[i], NULL, 10 ) );
		}
	}
}
//
//BOOL CCxStructTreeDataLinker::Parse( LPCTSTR lpszData )
//{
//	Clear();
//	CString strSrc = lpszData == NULL ? "" : lpszData;
//	if ( strSrc.IsEmpty() ) return FALSE;
//
//	//vector< CString > vDest;
//	//if ( CString2VectorWithEmpty(vDest, strSrc, ";") != 5 )
//	//	return FALSE;
//	//m_strTo = vDest[0];
//	//m_strFrom = vDest[1];
//	//m_strToIndex = vDest[2];
//	//m_strFromIndex = vDest[3];
//	//m_strOwnerId = vDest[4];
//	//if ( m_strFrom.IsEmpty() || m_strTo.IsEmpty() )
//	//{
//	//	Clear();
//	//	return FALSE;
//	//}
//
//
//	Json::Reader reader;
//	Json::Value arr;
//	CCxStructTreeDataLinker * pLinker = NULL;
//	try
//	{
//		if (!reader.parse((char*)(LPCTSTR)strSrc, arr, false)) return FALSE;
//		if ( arr.isNull() || !arr.isArray() ) return FALSE;
//
//		for ( Json::Value::iterator itr = arr.begin(); itr != arr.end(); ++itr)
//		{
//			if ( !(*itr).isObject() ) continue;
//			pLinker = new CCxStructTreeDataLinker();
//
//			pLinker->m_strOwnerId = ( (*itr)["id"].isString()) ? (*itr)["id"].asCString() : _T("");
//			pLinker->m_strFrom = ( (*itr)["id"].isString()) ? (*itr)["id"].asCString() : _T("");
//			pLinker->m_strTo = ( (*itr)["id"].isString()) ? (*itr)["id"].asCString() : _T("");
//			pLinker->m_strFromIndex = ( (*itr)["id"].isString()) ? (*itr)["id"].asCString() : _T("");
//			pLinker->m_strToIndex = ( (*itr)["id"].isString()) ? (*itr)["id"].asCString() : _T("");
//			pLinker->m_strFormula = ( (*itr)["id"].isString()) ? (*itr)["id"].asCString() : _T("");
//			if ( pLinker->m_strFrom.IsEmpty() || pLinker->m_strTo.IsEmpty() )
//			{
//				delete pLinker; pLinker = NULL;
//				Clear();
//				return FALSE;
//			}
//			
//			m_mapRunReturn.insert( make_pair( strId, strValue ) );
//		}
//		
//	}
//	catch (...)
//	{
//		Clear();
//		return FALSE;
//	}
//
//	return TRUE;
//	return TRUE;
//}

