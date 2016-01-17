#pragma once


#include "reader.h"
#include "writer.h"
#include "value.h"
#ifdef _DEBUG
#pragma comment(lib, "jsond.lib")
#else
#pragma comment(lib, "json.lib")
#endif

class CInterfaceData_CacheServer
{
public:
	CInterfaceData_CacheServer();
	~CInterfaceData_CacheServer();

	BOOL Parse( LPCTSTR lpszData );
	CString GetJsonStr( int nInterfaceFlag/* = CACHESERVER_INTERFACE_NULL*/, BOOL/* bCreateReturnObj = TRUE */);
	void Clear();

	CString m_sRunId;
	CString m_sWsPath;
	CString m_sXpdlStream;
	CString m_sUnzipDir;
	CString m_sCacheDirId;
	CString m_sErrMsg;

	enum
	{
		CACHESERVER_INTERFACE_NULL = 0,
		CACHESERVER_INTERFACE_GETRUNID,
		CACHESERVER_INTERFACE_STOP,
		CACHESERVER_INTERFACE_CREATECACHEBYWS,
		CACHESERVER_INTERFACE_CREATEWSBYCACHE,
		CACHESERVER_INTERFACE_COPYCACHE,
		CACHESERVER_INTERFACE_DELETECACHE,
		CACHESERVER_INTERFACE_CREATECACHEBYXPDLSTREAM,
		CACHESERVER_INTERFACE_CREATEWSBYXPSLSTREAM
	};
};
