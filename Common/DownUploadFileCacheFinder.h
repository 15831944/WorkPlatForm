#pragma once
#include "DownUploadDataFileObject.h"
class CDownUploadFileCacheFinder
{
public:
	CDownUploadFileCacheFinder(void);
	virtual ~CDownUploadFileCacheFinder(void);
	
	static BOOL ReadUploadCache(CDownUploadDataFileObject* pFileObject);
	static BOOL WriteUploadCache(CDownUploadDataFileObject* pFileObject);
	static BOOL ReadDownloadCache(CDownUploadDataFileObject* pFileObject);
	static BOOL WriteDownloadCache(CDownUploadDataFileObject* pFileObject);
	static BOOL WriteDownloadCacheInUpload(CDownUploadDataFileObject* pFileObject, bool bMove = false);
};

