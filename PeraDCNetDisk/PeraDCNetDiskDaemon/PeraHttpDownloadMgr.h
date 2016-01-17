#pragma once
#include "HttpDownloadMgr.h"
#include "InterfaceData.h"
#include "PeraDownloadThread.h"


class CPeraHttpDownloadMgr : public CHttpDownloadMgr
{
public:
	CPeraHttpDownloadMgr(void);
	virtual ~CPeraHttpDownloadMgr(void);

	BOOL Add( CInterfaceData_DownloadFiles* pDownLoadFiles );					//ɾ��һ���ļ�
	BOOL DoFileOperation( CInterfaceData_DoFilesOperation* pCancelFile );		//ȡ������
	BOOL Query( CInterfaceData_QueryFilesOperation& pQueryFiles );
	BOOL SetSysConfig( int nMaxThreadCount, int nConnectTimeOut = -1 );
	BOOL RecoverDownTask();
	BOOL GetSysConfig();							//�������ļ���ȡ��������������


protected:
	BOOL Delete( LPCTSTR lpIdPath );								//ɾ����������
	BOOL Pause( LPCTSTR lpszIdPath );								//��ͣΨһ�ļ�
	BOOL Stop( LPCTSTR lpIdPath );									//ֹͣ��������
	BOOL Resume( LPCTSTR lpIdPath );								//�ָ���������
	BOOL _Add( CInterfaceData_DownloadFile* pDownloadFile );			//ɾ��һ���ļ�
	virtual CHttpDownloadThread* NewThreadObj(void);
	CPeraDownloadThread* Find( LPCTSTR lpszLocalPath, LPCTSTR lpszIdPath );
	BOOL	IsParentPath( const char* pszParent, const char* pszUnkown );
	BOOL	Replace( char** pszChar );
	BOOL	DelRunVector( LPCTSTR lpIdPath );
};

extern CPeraHttpDownloadMgr theDownloadMgr;