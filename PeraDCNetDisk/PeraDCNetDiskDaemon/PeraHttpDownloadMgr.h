#pragma once
#include "HttpDownloadMgr.h"
#include "InterfaceData.h"
#include "PeraDownloadThread.h"


class CPeraHttpDownloadMgr : public CHttpDownloadMgr
{
public:
	CPeraHttpDownloadMgr(void);
	virtual ~CPeraHttpDownloadMgr(void);

	BOOL Add( CInterfaceData_DownloadFiles* pDownLoadFiles );					//删除一组文件
	BOOL DoFileOperation( CInterfaceData_DoFilesOperation* pCancelFile );		//取消下载
	BOOL Query( CInterfaceData_QueryFilesOperation& pQueryFiles );
	BOOL SetSysConfig( int nMaxThreadCount, int nConnectTimeOut = -1 );
	BOOL RecoverDownTask();
	BOOL GetSysConfig();							//从配置文件获取做大下载任务数


protected:
	BOOL Delete( LPCTSTR lpIdPath );								//删除下载任务
	BOOL Pause( LPCTSTR lpszIdPath );								//暂停唯一文件
	BOOL Stop( LPCTSTR lpIdPath );									//停止下载任务
	BOOL Resume( LPCTSTR lpIdPath );								//恢复下载任务
	BOOL _Add( CInterfaceData_DownloadFile* pDownloadFile );			//删除一个文件
	virtual CHttpDownloadThread* NewThreadObj(void);
	CPeraDownloadThread* Find( LPCTSTR lpszLocalPath, LPCTSTR lpszIdPath );
	BOOL	IsParentPath( const char* pszParent, const char* pszUnkown );
	BOOL	Replace( char** pszChar );
	BOOL	DelRunVector( LPCTSTR lpIdPath );
};

extern CPeraHttpDownloadMgr theDownloadMgr;