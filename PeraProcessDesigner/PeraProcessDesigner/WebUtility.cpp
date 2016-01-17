#include "StdAfx.h"
#include "WebUtility.h"
#include "reader.h"
#include "value.h"
#include "writer.h"
#include "PeraProcessDesigner.h"

CString WebGetInfo( LPCTSTR lpszJsonSrc )
{
	CString strSrc = lpszJsonSrc == NULL ? "" : lpszJsonSrc;
	if (strSrc.IsEmpty()) return "";

	Json::Reader reader;
	Json::Value arrSrc;
	Json::Value objResult;
	CString strKey, strValue, strResult;

	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, arrSrc, false)) return FALSE;
		if (!arrSrc.isArray()) return FALSE;

		for ( Json::Value::iterator itrKey = arrSrc.begin(); itrKey != arrSrc.end(); ++itrKey)
		{
			strKey = (*itrKey).isString() ? (*itrKey).asCString() : "";
			
			if ( strKey.CollateNoCase("workPath") )
			{
				strKey = "workPath";
				strValue = theApp.m_processMgr.m_sWorkPath;
				objResult[ (LPCTSTR)strKey ] = Json::Value((LPCTSTR)strValue);
			}
			else if ( strKey.CollateNoCase("project") )
			{
				strKey = "project";
				strValue = theApp.m_strProject;
				objResult[ (LPCTSTR)strKey ] = Json::Value((LPCTSTR)strValue);
			}
			else
			{
				objResult[ (LPCTSTR)strKey ] = Json::Value("");
			}
		}

		Json::FastWriter fast_writer;
		strResult = fast_writer.write(objResult).c_str();
		return strResult;
	}
	catch (...)
	{
		return "";
	}
}