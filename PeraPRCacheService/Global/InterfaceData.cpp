#include "stdafx.h"
#include "InterfaceData.h"

CInterfaceData_CacheServer::CInterfaceData_CacheServer()
{

}

CInterfaceData_CacheServer::~CInterfaceData_CacheServer()
{
	Clear();
}

BOOL CInterfaceData_CacheServer::Parse( LPCTSTR lpszData )
{
	Clear();
	CString strSrc = lpszData == NULL ? "" : lpszData;
	if ( strSrc.IsEmpty() ) return FALSE;

	Json::Reader reader;
	Json::Value obj( Json::objectValue );
	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) return FALSE;
		if ( obj.isNull() || !obj.isObject() ) return FALSE;
		m_sRunId = obj["runId"].isString() ? obj["runId"].asCString() : "";
		m_sWsPath = obj["wsPath"].isString() ? obj["wsPath"].asCString() : "";
		m_sXpdlStream = obj["xpdlStream"].isString() ? obj["xpdlStream"].asCString() : "";
		m_sUnzipDir = obj["unzipDir"].isString() ? obj["unzipDir"].asCString() : "";
		m_sCacheDirId = obj["cacheDirId"].isString() ? obj["cacheDirId"].asCString() : "";
		m_sErrMsg = obj["errMsg"].isString() ? obj["errMsg"].asCString() : "";
	}
	catch (...)
	{
		Clear(); return FALSE;
	}
	return TRUE;
}

CString CInterfaceData_CacheServer::GetJsonStr( int nInterfaceFlag, BOOL bCreateReturnObj )
{
	try
	{
		Json::Value jsonParams( Json::objectValue );
		jsonParams["runId"] = Json::Value( m_sRunId );
		if ( bCreateReturnObj == FALSE )//生成传入参数json对象
		{
			if ( nInterfaceFlag == CACHESERVER_INTERFACE_CREATECACHEBYWS )
			{
				jsonParams["unzipDir"] = Json::Value( m_sUnzipDir );
				jsonParams["wsPath"] = Json::Value( m_sWsPath );
			}
			else if ( nInterfaceFlag == CACHESERVER_INTERFACE_CREATEWSBYCACHE )
			{
				jsonParams["wsPath"] = Json::Value( m_sWsPath );
			}
			else if ( nInterfaceFlag == CACHESERVER_INTERFACE_COPYCACHE )
			{
				jsonParams["cacheDirId"] = Json::Value( m_sCacheDirId );
			}
			else if ( nInterfaceFlag == CACHESERVER_INTERFACE_CREATECACHEBYXPDLSTREAM )
			{
				jsonParams["xpdlStream"] = Json::Value( m_sXpdlStream );
			}
			else if ( nInterfaceFlag == CACHESERVER_INTERFACE_CREATEWSBYXPSLSTREAM )
			{
				jsonParams["wsPath"] = Json::Value( m_sWsPath );
				jsonParams["xpdlStream"] = Json::Value( m_sXpdlStream );
			}
		}
		else
		{
			jsonParams["errMsg"] = Json::Value( m_sErrMsg );
		}

		Json::FastWriter writer;
		CString sStr = writer.write(jsonParams).c_str();

		ZTools::WriteZToolsFormatLog("------CInterfaceData_CacheServer::GetJsonStr Json[%s]---------------\n", sStr );

		return sStr;
	}
	catch (...)
	{
		return "";
	}
}

void CInterfaceData_CacheServer::Clear()
{
	m_sRunId.Empty();
	m_sWsPath.Empty();
	m_sXpdlStream.Empty();
	m_sUnzipDir.Empty();
	m_sCacheDirId.Empty();
	m_sErrMsg.Empty();
}
