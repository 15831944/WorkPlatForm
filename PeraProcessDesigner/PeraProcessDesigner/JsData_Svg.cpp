#include "StdAfx.h"
#include "JsData_Svg.h"
#include "PtrVectorHeader.h"

#include "value.h"
#include "reader.h"
#include "writer.h"

CJsData_Svg_UpdateRunFlagData::CJsData_Svg_UpdateRunFlagData(void)
{
}


CJsData_Svg_UpdateRunFlagData::~CJsData_Svg_UpdateRunFlagData(void)
{
	Clear();
}

void CJsData_Svg_UpdateRunFlagData::Clear(void)
{
	m_bRan = FALSE;
	m_strId = "";
}

CJsData_Svg_UpdateRunFlag::CJsData_Svg_UpdateRunFlag(void)
{

}

CJsData_Svg_UpdateRunFlag::~CJsData_Svg_UpdateRunFlag(void)
{
	Clear();
}

void CJsData_Svg_UpdateRunFlag::Clear(void)
{
	PtrVectorClear( m_vData );
}

CString CJsData_Svg_UpdateRunFlag::GetJsonStr(void)
{
	CString strResult;
	try
	{
		Json::Value arrResult( Json::arrayValue );
		for ( int i=0; i<(int)m_vData.size(); i++ )
		{
			Json::Value obj;
			obj["id"] = Json::Value( (LPCTSTR)m_vData[i]->m_strId );
			obj["runFlag"] = Json::Value( m_vData[i]->m_bRan ? "true" : "false" );
			arrResult.append( obj );
		}

		Json::FastWriter fast_writer;
		strResult = fast_writer.write( arrResult ).c_str();
		return strResult;
	}
	catch (...)
	{
		return "";
	}
}

//////////////////////////////////////////////////////////////////////////

CJsData_Svg_UpdateParamStatusData::CJsData_Svg_UpdateParamStatusData( void )
{
	m_dwStatus = 0;
}

CJsData_Svg_UpdateParamStatusData::~CJsData_Svg_UpdateParamStatusData( void )
{
	Clear();
}

void CJsData_Svg_UpdateParamStatusData::Clear( void )
{
	m_dwStatus = 0;
	m_strId = "";
}

CJsData_Svg_UpdateParamStatus::CJsData_Svg_UpdateParamStatus( void )
{

}

CJsData_Svg_UpdateParamStatus::~CJsData_Svg_UpdateParamStatus( void )
{
	Clear();
}

void CJsData_Svg_UpdateParamStatus::Clear( void )
{
	PtrVectorClear( m_vData );
}

CString CJsData_Svg_UpdateParamStatus::GetJsonStr( void )
{
	CString strResult, str;
	try
	{
		Json::Value arrResult( Json::arrayValue );
		for ( int i=0; i<(int)m_vData.size(); i++ )
		{
			Json::Value obj;
			obj["id"] = Json::Value( (LPCTSTR)m_vData[i]->m_strId );
			str.Format( "%d", m_vData[i]->m_dwStatus );
			obj["status"] = Json::Value( (LPCTSTR)str );
			arrResult.append( obj );
		}

		Json::FastWriter fast_writer;
		strResult = fast_writer.write( arrResult ).c_str();
		return strResult;
	}
	catch (...)
	{
		return "";
	}
}
