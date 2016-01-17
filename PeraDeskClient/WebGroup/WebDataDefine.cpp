#include "StdAfx.h"
#include "WebDataDefine.h"
#include "PtrVectorHeader.h"


CWebMethodParamData::CWebMethodParamData()
{

}

CWebMethodParamData::~CWebMethodParamData()
{
	Clear();
}

void CWebMethodParamData::Clear( void )
{
	m_strName = "";
	m_strValue = "";
	m_strDataType = "";
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


CWebMethodData::CWebMethodData()
{

}

CWebMethodData::~CWebMethodData()
{
	Clear();
}

void CWebMethodData::Clear( void )
{
	m_strWebID = "";
	m_strMethodName = "";
	PtrVectorClear( m_vParams );
	m_strResult = "";
	m_strInfo = "";
}

CString CWebMethodData::GetJsonStr( void )
{
	return "";
}

BOOL CWebMethodData::Parse( LPCTSTR lpszJsonStr )
{
	return TRUE;
}


