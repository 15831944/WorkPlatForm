#pragma once
#include "HttpDownloadThread.h"
#include "InterfaceData.h"

class CPeraDownloadThread :
	public CHttpDownloadThread
{
public:
	CPeraDownloadThread( CHttpDownloadMgr* pMgr );
	virtual ~CPeraDownloadThread(void);

	virtual CString GetUrlForDownload( void );
	virtual CString GetLocalPathForDownload( void );
	virtual CString GetFileTotalSize(void);
	virtual BOOL	IsCopy( void );
	virtual CString GetCopySourcePath( void );
	virtual BOOL	ReNameExistFile();
	virtual bool	CheckFileType( const  curl_slist* m_pHeader );
	virtual CString GetFileId( void );
	virtual void	OnStatusChanged(void);
	virtual CString	GetidPath();
	CPeraDownloadThread* GetCopied();
	void			ClearAllData();
	void			SetBreakPointDownload();

public:
	CInterfaceData_DownloadFile * m_pData;
};

