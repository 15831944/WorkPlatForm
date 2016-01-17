#include "StdAfx.h"
#include "InterfaceData.h"

#include "ConvertData.h"
#include "PtrVectorHeader.h"

#include "reader.h"
#include "writer.h"
#include "CommonMethod.h"

#include "..\PeraDCNetDiskDaemon\PeraNetDiskSetting.h"

extern CPeraNetDiskSetting theSetting;


///////////////////////////////////////////////////////////////////////////////////////

CInterfaceData_DownloadFiles::CInterfaceData_DownloadFiles()
{

}

CInterfaceData_DownloadFiles::~CInterfaceData_DownloadFiles()
{
	Clear();
}

void CInterfaceData_DownloadFiles::Clear()
{
	PtrVectorClear(m_vecFiles);
}

BOOL CInterfaceData_DownloadFiles::Parse( LPCTSTR lpszData )
{
	Clear();

	CString strSrc = lpszData == NULL ? "" : lpszData;
	if (strSrc.IsEmpty()) return FALSE;

	Json::Reader reader;
	Json::Value obj;
	CString str;

	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) return FALSE;
		if (!obj.isObject()) return FALSE;

		if ( obj["paths"].isArray())
		{
			int iCount = obj["files"].size();
			for (int i=0;i< iCount;i++)
			{
				CInterfaceData_DownloadFile* pDownLoadPath = new CInterfaceData_DownloadFile;
				pDownLoadPath->Clear();
				pDownLoadPath->m_sUserName = (obj["userName"].isString()) ? obj["userName"].asCString() : _T("");
				pDownLoadPath->m_sFlowId = (obj["flowId"].isString()) ? obj["flowId"].asCString() : _T("");
				pDownLoadPath->m_sFlowId = ReplaceFlowSc(pDownLoadPath->m_sFlowId);
				pDownLoadPath->m_sFileId = obj["files"][i]["fileId"].isString() ?  obj["files"][i]["fileId"].asCString():"";
				pDownLoadPath->m_sFileMd5 = obj["files"][i]["fileMd5"].isString() ?  obj["files"][i]["fileMd5"].asCString():"";
				pDownLoadPath->m_sFileSize = obj["files"][i]["fileSize"].isString() ?  obj["files"][i]["fileSize"].asCString():"";
				pDownLoadPath->m_sIdPath = obj["files"][i]["idPath"].isString() ?  obj["files"][i]["idPath"].asCString():"";
				pDownLoadPath->m_sLocalPath = obj["files"][i]["localPath"].isString() ?  obj["files"][i]["localPath"].asCString():"";
				NormalPath(pDownLoadPath->m_sLocalPath);
				pDownLoadPath->m_sNetDiskPath = obj["files"][i]["netDiskPath"].isString() ?  obj["files"][i]["netDiskPath"].asCString():"";
				NormalPath(pDownLoadPath->m_sNetDiskPath);
				m_vecFiles.push_back(pDownLoadPath);
			}
		}
	}
	catch(...)
	{
		Clear(); return FALSE;
	}
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


CInterfaceData_DownloadFile::CInterfaceData_DownloadFile()
{

}

CInterfaceData_DownloadFile::~CInterfaceData_DownloadFile()
{
	Clear();
}

void CInterfaceData_DownloadFile::Clear( void )
{
	m_sUserName.Empty();
	m_sFlowId.Empty();
	m_sNetDiskPath.Empty();
	m_sIdPath.Empty();
	m_sFileId.Empty();
	m_sFileSize.Empty();
	m_sFileMd5.Empty();
	m_sLocalPath.Empty();
}

BOOL CInterfaceData_DownloadFile::Parse( LPCTSTR lpszData )
{
	Clear();

	CString strSrc = lpszData == NULL ? "" : lpszData;
	if (strSrc.IsEmpty()) return FALSE;

	Json::Reader reader;
	Json::Value obj;
	CString str;

	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) return FALSE;
		if (!obj.isObject()) return FALSE;

		m_sUserName = (obj["userName"].isString()) ? obj["userName"].asCString() : _T("");
		m_sFlowId = (obj["flowId"].isString()) ? obj["flowId"].asCString() : _T("");
		m_sFlowId = ReplaceFlowSc(m_sFlowId);
		m_sNetDiskPath = (obj["path"].isString()) ? obj["path"].asCString() : _T("");
		NormalPath(m_sNetDiskPath);
		m_sIdPath = (obj["idPath"].isString()) ? obj["idPath"].asCString() : _T("");
		m_sFileId = (obj["fileId"].isString()) ? obj["fileId"].asCString() : _T("");
		m_sFileSize = (obj["fileSize"].isString()) ? obj["fileSize"].asCString() : _T("");
		m_sFileMd5 = (obj["fileMd5"].isString()) ? obj["fileMd5"].asCString() : _T("");
		//m_sDstDir = (obj["dstDir"].isString()) ? obj["dstDir"].asCString() : _T("");
	}
	catch (...)
	{
		Clear(); return FALSE;
	}
	return TRUE;
}

CString CInterfaceData_DownloadFile::GetJsonStr( void )
{
	try
	{
		CString strResult;
		Json::Value obj(Json::objectValue);
		obj["userName"] = Json::Value( (LPCTSTR)m_sUserName );
		obj["flowId"] = Json::Value( (LPCTSTR)m_sFlowId );
		obj["path"] = Json::Value( (LPCTSTR)m_sNetDiskPath );
		obj["idPath"] = Json::Value( (LPCTSTR)m_sIdPath );
		obj["fileId"] = Json::Value( (LPCTSTR)m_sFileId );
		obj["fileSize"] = Json::Value( (LPCTSTR)m_sFileSize );
		obj["fileMd5"] = Json::Value( (LPCTSTR)m_sFileMd5 );
		obj["localPath"] = Json::Value( (LPCTSTR)m_sLocalPath );

		Json::FastWriter fast_writer;
		strResult = fast_writer.write( obj ).c_str();
		return strResult;
	}
	catch (...)
	{
		return "";
	}
}

CInterfaceData_DownloadFile* CInterfaceData_DownloadFile::GetCopied(void)
{
	CInterfaceData_DownloadFile* pNew = new CInterfaceData_DownloadFile();
	pNew->m_sUserName = m_sUserName;
	pNew->m_sFlowId = m_sFlowId;
	pNew->m_sNetDiskPath = m_sNetDiskPath;
	pNew->m_sIdPath = m_sIdPath;
	pNew->m_sFileId = m_sFileId;
	pNew->m_sFileSize = m_sFileSize;
	pNew->m_sFileMd5 = m_sFileMd5;
	//pNew->m_sDstDir = m_sDstDir;
	pNew->m_sLocalPath = m_sLocalPath;
	return pNew;
}


BOOL CInterfaceData_DownloadFile::CopyFromTaskData( CTaskData* pTaskData )
{
	m_sFileId = pTaskData->m_sFileId;
	m_sFileMd5 = pTaskData->m_sFileMd5;
	m_sFileSize = pTaskData->m_sTotalSize;
	m_sFlowId = pTaskData->m_sFlowId;
	m_sIdPath = pTaskData->m_sIdPath;
	m_sLocalPath = pTaskData->m_sLocalPath;
	m_sNetDiskPath = pTaskData->m_sNetDiskPath;
	m_sUserName = pTaskData->m_sUserName;
	return TRUE;
}

CString CInterfaceData_DownloadFile::GetLocalPath( )
{
	/*
	if ( bReal )
	{
		return m_sRealLocalPath;
	}

	CString sLocalPath, sFilename, str;
	str = m_sPath;
	str.Replace('\\', '/');
	int nPos = str.ReverseFind( '/' );
	sFilename = str.Mid( nPos + 1, str.GetLength() - (nPos + 1) );
	sLocalPath = m_sDstDir + "\\";
	sLocalPath += sFilename;
	return sLocalPath;
	*/
	return m_sLocalPath;
}

CString CInterfaceData_DownloadFile::GetUrl( void )
{
#if 1
	CString sUrl = theSetting.GetDownloadUrl();

	if ( sUrl.IsEmpty() )
	{
		sUrl = "http://192.168.50.165/dms/file/down?id=";
	}
#else
	CString sUrl = "http://192.168.112.36:8080/dms/netDisk/down?id=";
#endif

	sUrl += m_sFileId;

	return sUrl;
}


//////////////////////////////////////////////////////////////////////////

CInterfaceData_DownloadDir::CInterfaceData_DownloadDir()
{

}

CInterfaceData_DownloadDir::~CInterfaceData_DownloadDir()
{
	Clear();
}

void CInterfaceData_DownloadDir::Clear( void )
{
}

BOOL CInterfaceData_DownloadDir::Parse( LPCTSTR lpszData )
{
	return TRUE;
}

CString CInterfaceData_DownloadDir::GetJsonStr( void )
{
	return "";
}

/************************************************************************/
/*  查询文件基本信息接口数据解析实现                                                       */
/************************************************************************/
CInterfaceData_QueryFilesInfo::CInterfaceData_QueryFilesInfo()
{

}
CInterfaceData_QueryFilesInfo::~CInterfaceData_QueryFilesInfo()
{
	Clear();
}
void CInterfaceData_QueryFilesInfo::Clear(void)
{
	PtrVectorClear(m_vFiles);
	m_strErrMsg.Empty();
}
BOOL CInterfaceData_QueryFilesInfo::Parse( LPCTSTR lpszData )
{
	Clear();

	CString strSrc = lpszData == NULL ? "" : lpszData;
	if (strSrc.IsEmpty()) return FALSE;

	Json::Reader reader;
	Json::Value obj;
	CString str;

	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) return FALSE;
		if (!obj.isObject()) return FALSE;
		if (obj["files"].isArray())
		{
			int iSize = obj["files"].size();
			for (int i=0; i< iSize;i++)
			{
				CInterfaceData_QueryFilesInfoFile *fobj = new CInterfaceData_QueryFilesInfoFile;
				Json::Value valItem = obj["files"][i]["path"]; 
				fobj->m_sPath = valItem.isString() ? valItem.asCString():"";
				NormalPath(fobj->m_sPath);

				Json::Value valName = obj["files"][i]["userName"];
				fobj->m_sUserName = valName.isString()? valName.asCString():"";

				Json::Value valFlowId = obj["files"][i]["flowId"];
				fobj->m_sFlowId = valFlowId.isString() ? valFlowId.asCString():"";
				fobj->m_sFlowId = ReplaceFlowSc(fobj->m_sFlowId);

				if (!IsAbsolutePath((LPCTSTR)fobj->m_sPath)) // 如果是相对路径，必须要有用户名和工作流ID字段
				{
					if (fobj->m_sUserName.IsEmpty() || 	fobj->m_sFlowId.IsEmpty())
					{
						m_strErrMsg = "输入路径为相对路径,但用户名和工作流ID字段不完整";
						return FALSE;
					}
				}


				m_vFiles.push_back(fobj);
			}

		}

	}
	catch (...)
	{
		Clear(); return FALSE;
	}
	return TRUE;
}
CString CInterfaceData_QueryFilesInfo::GetJsonStrResult(void)
{
	try
	{
		CString strResult;
		 Json::Value vArryInfo(Json::arrayValue);
		 for (size_t iIndex =0;iIndex <m_vFiles.size();iIndex++)
		 {
			 Json::Value Objinfo;
			 Objinfo["path"]     = Json::Value((LPCTSTR)m_vFiles[iIndex]->m_Result.m_sPath);
			 Objinfo["fullPath"] = Json::Value((LPCTSTR)m_vFiles[iIndex]->m_Result.m_sFullPath);
			 Objinfo["flag"]     = Json::Value((LPCTSTR)m_vFiles[iIndex]->m_Result.m_sFlag);
			 Objinfo["msg"]      = Json::Value((LPCTSTR)m_vFiles[iIndex]->m_Result.m_sMsg);
			 Objinfo["fileName"] = Json::Value((LPCTSTR)m_vFiles[iIndex]->m_Result.m_sFileName);
			 Objinfo["fileSize"] = Json::Value((LPCTSTR)m_vFiles[iIndex]->m_Result.m_sFileSize);
			 Objinfo["createTime"] = Json::Value((LPCTSTR)m_vFiles[iIndex]->m_Result.m_sCreateTime);
			 Objinfo["modifyTime"] = Json::Value((LPCTSTR)m_vFiles[iIndex]->m_Result.m_sModitifyTime);
			 Objinfo["fileType"] = Json::Value((LPCTSTR)m_vFiles[iIndex]->m_Result.m_sFileType);
			 Objinfo["openWithType"] = Json::Value((LPCTSTR)m_vFiles[iIndex]->m_Result.m_sOpenWithType);
			 Objinfo["fileIcon"] = Json::Value((LPCTSTR)m_vFiles[iIndex]->m_Result.m_sFileIcon);
			 vArryInfo.append(Objinfo);
		 }
		 Json::Value objOut;
		 objOut["flag"] = Json::Value("true");
		 objOut["msg"]  = Json::Value("");
		 objOut["infos"] = vArryInfo;
		 strResult = objOut.toStyledString().c_str();
		 return strResult;
	}
	catch (...)
	{
		return "";
	}
}
/************************************************************************/
/* 新建本地文件接口数据解析实现                                                          */
/************************************************************************/
CInterfaceData_CreateFile::CInterfaceData_CreateFile()
{

}
CInterfaceData_CreateFile::~CInterfaceData_CreateFile()
{
	Clear();
}
void CInterfaceData_CreateFile::Clear(void)
{
	m_sUserName.Empty();
	m_sFlowId.Empty();
	m_sPath.Empty();
	m_sFileName.Empty();
	m_sType.Empty();
	m_strErrMsg.Empty();
}
BOOL CInterfaceData_CreateFile::Parse( LPCTSTR lpszData )
{
	Clear();

	CString strSrc = lpszData == NULL ? "" : lpszData;
	if (strSrc.IsEmpty()) return FALSE;

	Json::Reader reader;
	Json::Value obj;
	CString str;

	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) return FALSE;
		if (!obj.isObject()) return FALSE;

		m_sUserName = (obj["userName"].isString()) ? obj["userName"].asCString() : _T("");
		m_sFlowId = (obj["flowId"].isString()) ? obj["flowId"].asCString() : _T("");
		m_sFlowId = ReplaceFlowSc(m_sFlowId);
		m_sPath = (obj["path"].isString()) ? obj["path"].asCString() : _T("");
		NormalPath(m_sPath);
		m_sType = (obj["type"].isString()) ? obj["type"].asCString() : _T("");
		m_sFileName = (obj["fileName"].isString()) ? obj["fileName"].asCString() : _T("");
	}
	catch (...)
	{
		Clear(); return FALSE;
	}
	if (!IsAbsolutePath((LPCTSTR)m_sPath)) // 如果是相对路径，必须要有用户名和工作流ID字段
	{
		if (m_sUserName.IsEmpty() || m_sFlowId.IsEmpty() )
		{
			m_strErrMsg = "输入路径为相对路径,但用户名和工作流ID字段不完整";
			return FALSE;
		}
	}
	return TRUE;
}
CString CInterfaceData_CreateFile::GetJsonStrResult(void)
{
	try
	{
		CString strResult;
		Json::Value objOut;
		objOut["flag"] = Json::Value((LPCTSTR)m_Result.m_sflag);
		objOut["msg"] = Json::Value((LPCTSTR)m_Result.m_sMsg);
		objOut["errorCode"] = Json::Value(m_Result.m_iErrCode);

		Json::Value Objinfo;
		Objinfo["path"] = Json::Value((LPCTSTR)m_Result.m_sPath);
		Objinfo["fullPath"] = Json::Value((LPCTSTR)m_Result.m_sFullPath);
		Objinfo["fileName"] = Json::Value( (LPCTSTR)m_Result.m_sFileName);
		Objinfo["fileSize"] = Json::Value( (LPCTSTR)m_Result.m_sFileSize );
		Objinfo["createTime"] = Json::Value( (LPCTSTR)m_Result.m_sCreatetime);
		Objinfo["modifyTime"] = Json::Value( (LPCTSTR)m_Result.m_sModifyTime);

		objOut["info"] = Objinfo;

		strResult = objOut.toStyledString().c_str();
		return strResult;
	}
	catch (...)
	{
		return "";
	}
}
/************************************************************************/
/* 复制本地文件接口数据解析实现                                                */
/************************************************************************/
CInterfaceData_CopyFiles::CInterfaceData_CopyFiles()
{

}
CInterfaceData_CopyFiles::~CInterfaceData_CopyFiles()
{
	Clear();
}
void CInterfaceData_CopyFiles::Clear(void)
{
	m_sUserName.Empty();
	m_sFlowId.Empty();
	m_vFiles.clear();
}
BOOL CInterfaceData_CopyFiles::Parse( LPCTSTR lpszData )
{
	Clear();

	CString strSrc = lpszData == NULL ? "" : lpszData;
	if (strSrc.IsEmpty()) return FALSE;

	Json::Reader reader;
	Json::Value obj;
	CString str;

	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) return FALSE;
		if (!obj.isObject()) return FALSE;
		m_sUserName  = (obj["userName"].isString()) ? obj["userName"].asCString() : _T("");
		m_sFlowId    = (obj["flowId"].isString()) ? obj["flowId"].asCString() : _T("");
		m_sFlowId = ReplaceFlowSc(m_sFlowId);

		if ( obj["files"].isArray())
		{
			int iCount = obj["files"].size();
			for (int i=0;i< iCount;i++)
			{
				SCopyFile Sfile;
				Sfile.m_sNetDiskPath  = obj["files"][i]["netDiskPath"].isString() ?  obj["files"][i]["netDiskPath"].asCString():"";
				NormalPath(Sfile.m_sNetDiskPath);
				Sfile.m_sLocalPath  = obj["files"][i]["localPath"].isString() ?  obj["files"][i]["localPath"].asCString():"";
				NormalPath(Sfile.m_sLocalPath);
				Sfile.m_sIdPath  = obj["files"][i]["idPath"].isString() ?  obj["files"][i]["idPath"].asCString():"";
				m_vFiles.push_back(Sfile);
			}
		}
	}
	catch (...)
	{
		Clear(); return FALSE;
	}
	return TRUE;
}
CTaskData * CInterfaceData_CopyFiles::GetTaskCopied(int iIndex)
{
	if (iIndex <0 || iIndex >= (int)m_vFiles.size())
		return NULL;

	CTaskData * pTask = new CTaskData;
	if (pTask)
	{
		pTask->m_sUserName = this->m_sUserName;
		pTask->m_sFlowId = this->m_sFlowId;
		pTask->m_sLocalPath = m_vFiles[iIndex].m_sLocalPath;
		pTask->m_sNetDiskPath = m_vFiles[iIndex].m_sNetDiskPath;
		pTask->m_sIdPath = m_vFiles[iIndex].m_sIdPath;
		pTask->m_sOperatorFlag ="copy";
		pTask->m_bDelete = FALSE;
	}
	return pTask;
}
CInterfaceData_CopyFiles::SCopyFile CInterfaceData_CopyFiles::FromTaskData(CTaskData * pTask)
{
	CInterfaceData_CopyFiles::SCopyFile cf;
	if (pTask)
	{
		cf.m_sIdPath = pTask->m_sIdPath;
		cf.m_sLocalPath = pTask->m_sLocalPath;
		cf.m_sNetDiskPath = pTask->m_sNetDiskPath;
	}
	return cf;
}
/************************************************************************/
/* 查询文件操作进度接口数据解析实现                                             */
/************************************************************************/
CInterfaceData_QueryFilesOperation::CInterfaceData_QueryFilesOperation()
{
	m_bFlag = FALSE;
}
CInterfaceData_QueryFilesOperation::~CInterfaceData_QueryFilesOperation()
{
	Clear();
}
void CInterfaceData_QueryFilesOperation::Clear(void)
{
	m_bFlag = FALSE;
	m_sMsg.Empty();
	m_sUserName.Empty();
	m_sFlowId.Empty();
	PtrVectorClear( m_vPath );
}
BOOL CInterfaceData_QueryFilesOperation::Parse( LPCTSTR lpszData )
{
	Clear();

	CString strSrc = lpszData == NULL ? "" : lpszData;
	if (strSrc.IsEmpty()) return FALSE;

	Json::Reader reader;
	Json::Value obj;
	CString str;

	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) return FALSE;
		if (!obj.isObject()) return FALSE;
		m_sUserName  = (obj["userName"].isString()) ? obj["userName"].asCString() : _T("");
		m_sFlowId    = (obj["flowId"].isString()) ? obj["flowId"].asCString() : _T("");
		m_sFlowId = ReplaceFlowSc(m_sFlowId);

		if ( obj["paths"].isArray())
		{
			int iCount = obj["paths"].size();
			for (int i=0;i< iCount;i++)
			{
				CInterfaceData_QueryFilesOperationPath * QueryPath = new CInterfaceData_QueryFilesOperationPath;
				QueryPath->m_sNetdiskPath = obj["paths"][i]["netDiskPath"].isString() ?  obj["paths"][i]["netDiskPath"].asCString():"";
				NormalPath(QueryPath->m_sNetdiskPath);
				QueryPath->m_sIdPath = obj["paths"][i]["idPath"].isString() ?  obj["paths"][i]["idPath"].asCString():"";
				CString strTmp    = obj["paths"][i]["isFile"].isString() ?  obj["paths"][i]["isFile"].asCString():"";
				strTmp.MakeLower();
				QueryPath->m_bIsFile = strTmp.Compare("true")==0 ? true:false;
				m_vPath.push_back(QueryPath);
			}
		}
	}
	catch (...)
	{
		Clear(); return FALSE;
	}
	return TRUE;

}

//////////////////////////////////////////////////////////////////////////


CInterfaceData_QueryFilesOperationResult::CInterfaceData_QueryFilesOperationResult()
{

}

CInterfaceData_QueryFilesOperationResult::~CInterfaceData_QueryFilesOperationResult()
{
	Clear();
}

void CInterfaceData_QueryFilesOperationResult::Clear( void )
{
	m_sNetDiskPath.Empty();
	m_sIdPath.Empty();
	m_sOperation.Empty();
	m_sTotalSize.Empty();
	m_sFinishedSize.Empty();
	m_sProgress.Empty();
	m_sStatus.Empty();
	m_sMsg.Empty();
	m_sRealPath.Empty();
}

CInterfaceData_QueryFilesOperationPath::CInterfaceData_QueryFilesOperationPath()
{

}

CInterfaceData_QueryFilesOperationPath::~CInterfaceData_QueryFilesOperationPath()
{
	Clear();
}

void CInterfaceData_QueryFilesOperationPath::Clear( void )
{
	m_sMsg.Empty();
	m_sNetdiskPath.Empty();
	m_sIdPath.Empty();
	m_bIsFile = FALSE;
	PtrVectorClear( m_vResult );
}

CString CInterfaceData_QueryFilesOperation::GetJsonStrForResult(void)
{
	try
	{
		Json::Value objOut(Json::objectValue);
		objOut["flag"]  = Json::Value("true");
		objOut["msg"]   = Json::Value("");

		Json::Value paths(Json::arrayValue);
		for (size_t iIndex = 0; iIndex<m_vPath.size();iIndex++)
		{
			Json::Value OnePath (Json::objectValue);
			OnePath["netDiskPath"]   = Json::Value((LPCTSTR)m_vPath[iIndex]->m_sNetdiskPath);
			OnePath["idPath"] = Json::Value((LPCTSTR)m_vPath[iIndex]->m_sIdPath);
			OnePath["flag"]   = Json::Value("true");
			OnePath["msg"]    = Json::Value("");
			Json::Value operatios(Json::arrayValue);
			for (size_t i=0; i<m_vPath[iIndex]->m_vResult.size();i++)
			{
				Json::Value OneOperation (Json::objectValue);
				OneOperation["netDiskPath"] = Json::Value((LPCTSTR)m_vPath[iIndex]->m_vResult[i]->m_sNetDiskPath);
				OneOperation["idPath"] = Json::Value((LPCTSTR)m_vPath[iIndex]->m_vResult[i]->m_sIdPath);
				OneOperation["operation"] = Json::Value((LPCTSTR)m_vPath[iIndex]->m_vResult[i]->m_sOperation);
				OneOperation["totalSize"] = Json::Value((LPCTSTR)m_vPath[iIndex]->m_vResult[i]->m_sTotalSize);
				OneOperation["finishedSize"] = Json::Value((LPCTSTR)m_vPath[iIndex]->m_vResult[i]->m_sFinishedSize);
				OneOperation["progress"] = Json::Value((LPCTSTR)m_vPath[iIndex]->m_vResult[i]->m_sProgress);
				OneOperation["status"] = Json::Value((LPCTSTR)m_vPath[iIndex]->m_vResult[i]->m_sStatus);
				OneOperation["msg"] = Json::Value((LPCTSTR)m_vPath[iIndex]->m_vResult[i]->m_sMsg);
				//OneOperation["realPath"] = Json::Value((LPCTSTR)m_vPath[iIndex]->m_vResult[i]->m_sRealPath);
				if (m_vPath[iIndex]->m_vResult[i]->m_sOperation.Compare("upload")==0)
					OneOperation["fileId"] = Json::Value((LPCTSTR)m_vPath[iIndex]->m_vResult[i]->m_sFileId);

				operatios.append(OneOperation);
			}
			OnePath["operations"] = operatios;
			paths.append(OnePath);
		}
		objOut["paths"] = paths;

		CString strResult = objOut.toStyledString().c_str();
		return strResult;
	}
	catch (...)
	{
		return "";
	}
}
/************************************************************************/
/*取消文件操作进度接口数据解析实现                                                  */
/************************************************************************/
CInterfaceData_DoFilesOperation::CInterfaceData_DoFilesOperation()
{

}
CInterfaceData_DoFilesOperation::~CInterfaceData_DoFilesOperation()
{
	Clear();
}
void CInterfaceData_DoFilesOperation::Clear(void)
{
	m_sUserName.Empty();
	m_sFlowId.Empty();
	m_sNetdiskPath.Empty();
	m_sIdPath.Empty();
	m_soperationFlag.Empty();
	m_bIsFile = TRUE;
}
BOOL CInterfaceData_DoFilesOperation::Parse( LPCTSTR lpszData )
{
	Clear();

	CString strSrc = lpszData == NULL ? "" : lpszData;
	if (strSrc.IsEmpty()) return FALSE;

	Json::Reader reader;
	Json::Value obj;
	CString str;

	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) return FALSE;
		if (!obj.isObject()) return FALSE;

		m_sUserName = (obj["userName"].isString()) ? obj["userName"].asCString() : _T("");
		m_sFlowId = (obj["flowId"].isString()) ? obj["flowId"].asCString() : _T("");
		m_sFlowId = ReplaceFlowSc(m_sFlowId);

		m_sNetdiskPath = (obj["netDiskPath"].isString()) ? obj["netDiskPath"].asCString() : _T("");
		NormalPath(m_sNetdiskPath);
		m_sIdPath = (obj["idPath"].isString()) ? obj["idPath"].asCString() : _T("");
		CString strTmp = (obj["isFile"].isString()) ? obj["isFile"].asCString() : _T("");
		strTmp.MakeUpper();
		m_bIsFile = strTmp.Compare("TRUE")==0 ? TRUE:FALSE;
		m_soperationFlag = (obj["operationFlag"].isString()) ? obj["operationFlag"].asCString() : _T("");
	}
	catch (...)
	{
		Clear(); return FALSE;
	}
	return TRUE;
}
/************************************************************************/
/* 设置缓存路径接口数据解析实现                                                                     */
/************************************************************************/
CInterfaceData_SetCacheInputData::CInterfaceData_SetCacheInputData()
{

}
CInterfaceData_SetCacheInputData::~CInterfaceData_SetCacheInputData()
{
	Clear();
}
void CInterfaceData_SetCacheInputData::Clear(void)
{
	m_sCacheDir.Empty();
}
BOOL CInterfaceData_SetCacheInputData::Parse( LPCTSTR lpszData )
{
	Clear();

	CString strSrc = lpszData == NULL ? "" : lpszData;
	if (strSrc.IsEmpty()) return FALSE;

	Json::Reader reader;
	Json::Value obj;
	CString str;

	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) return FALSE;
		if (!obj.isObject()) return FALSE;

		m_sCacheDir = (obj["CacheDir"].isString()) ? obj["CacheDir"].asCString() : _T("");
	}
	catch (...)
	{
		Clear(); return FALSE;
	}
	if (m_sCacheDir.IsEmpty())
		return FALSE;

	return TRUE;
}
CString CInterfaceData_SetCacheInputData::GetJsonStr(void)
{
	try
	{
		CString strResult;
		Json::Value obj(Json::objectValue);
		obj["CacheDir"] = Json::Value( (LPCTSTR)m_sCacheDir);

		Json::FastWriter fast_writer;
		strResult = fast_writer.write( obj ).c_str();
		return strResult;
	}
	catch (...)
	{
		return "";
	}
}
/************************************************************************/
/* 上传文件接口数据解析实现                                                                     */
/************************************************************************/
CInterfaceData_UploadFile::CInterfaceData_UploadFile()
{

}
CInterfaceData_UploadFile::~CInterfaceData_UploadFile()
{
	Clear();
}
void CInterfaceData_UploadFile::Clear(void)
{
	m_sUserName.Empty();
	m_sNetdiskPath.Empty();
	m_sIdPath.Empty();
	m_sDataId.Empty();
	m_sFlowId.Empty();
}
BOOL CInterfaceData_UploadFile::Parse( LPCTSTR lpszData )
{
	Clear();

	CString strSrc = lpszData == NULL ? "" : lpszData;
	if (strSrc.IsEmpty()) return FALSE;

	Json::Reader reader;
	Json::Value obj;
	CString str;

	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) return FALSE;
		if (!obj.isObject()) return FALSE;

		m_sUserName = (obj["userName"].isString()) ? obj["userName"].asCString() : _T("");
		m_sFlowId = (obj["flowId"].isString()) ? obj["flowId"].asCString() : _T("");
		m_sFlowId = ReplaceFlowSc(m_sFlowId);
		m_sNetdiskPath = (obj["netDiskPath"].isString()) ? obj["netDiskPath"].asCString() : _T("");
		NormalPath(m_sNetdiskPath);
		m_sIdPath = (obj["idPath"].isString()) ? obj["idPath"].asCString() : _T("");
		m_sDataId = (obj["dataId"].isString()) ? obj["dataId"].asCString() : _T("");
	}
	catch (...)
	{
		m_strErrMsg = "解析Json输入出现异常";
		Clear(); return FALSE;
	}

	if (!IsAbsolutePath((LPCTSTR)m_sNetdiskPath))
	{
		if (m_sUserName.IsEmpty() || m_sFlowId.IsEmpty())
		{
			m_strErrMsg = "输入路径为相对路径,但用户名和工作流ID字段不完整";
			return FALSE;
		}
	}

	return TRUE;
}
CTaskData * CInterfaceData_UploadFile::GetTaskCopied()
{
	CTaskData *pUploadTask = new CTaskData;
	pUploadTask->m_sUserName = this->m_sUserName;
	pUploadTask->m_sFlowId   = this->m_sFlowId;
	pUploadTask->m_sIdPath   = this->m_sIdPath;
	pUploadTask->m_sNetDiskPath  = this->m_sNetdiskPath;
	pUploadTask->m_sDataId    = this->m_sDataId;
	pUploadTask->m_sOperatorFlag = "upload";
	pUploadTask->m_bDelete = FALSE;
	return pUploadTask;
}
CInterfaceData_UploadFile::CInterfaceData_UploadFile(CTaskData * pTask)
{
	Clear();
	if (pTask)
	{
		m_sUserName = pTask->m_sUserName;
		m_sFlowId = pTask->m_sFlowId;
		m_sIdPath = pTask->m_sIdPath;
		m_sNetdiskPath = pTask->m_sNetDiskPath;
		m_sDataId = pTask->m_sDataId;
	}
}
//////////////////////////////////////////////////////////////////////////
CInterfaceData_QueryLocalFileType::CInterfaceData_QueryLocalFileType()
{

}
CInterfaceData_QueryLocalFileType::~CInterfaceData_QueryLocalFileType()
{
	Clear();
}
void CInterfaceData_QueryLocalFileType::Clear(void)
{
	PtrVectorClear(m_vItemList);

}
CString CInterfaceData_QueryLocalFileType::GetJsonStrResult(void)
{
	try
	{
		Json::Value objOut;
		objOut["flag"]=  Json::Value(m_vItemList.size()>0 ? "true":"false");
		objOut["msg"] =  Json::Value(m_vItemList.size()>0 ? "":"获取新建文件类型错误!");

		Json::Value jsonTypeItems(Json::arrayValue);

		vector<NewType *>::iterator iter = m_vItemList.begin();
		for (iter;iter!=m_vItemList.end();iter++)
		{
			Json::Value jnt;
			jnt["ext"] = Json::Value((*iter)->m_strExt.c_str());
			jnt["desc"] = Json::Value((*iter)->m_strDesrciption.c_str());
			jsonTypeItems.append(jnt);

		}
		objOut["types"] = jsonTypeItems;
		return objOut.toStyledString().c_str();
	}
	catch(...)
	{
		return "";
	}
}
BOOL CInterfaceData_OpenFile::Parse( LPCTSTR lpszData )
{
	CString strSrc = lpszData == NULL ? "" : lpszData;
	if (strSrc.IsEmpty()) return FALSE;

	Json::Reader reader;
	Json::Value obj;

	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) return FALSE;
		if (!obj.isObject()) return FALSE;

		m_sUserName = obj["userName"].isString() ? obj["userName"].asCString() : _T("");
		m_sFlowId   = obj["flowId"].isString()	? obj["flowId"].asCString() : _T("");
		m_sFlowId = ReplaceFlowSc(m_sFlowId);
		m_sFileId   = obj["fileId"].isString()	?  obj["fileId"].asCString():"";
		m_sFileMd5  = obj["fileMd5"].isString() ?  obj["fileMd5"].asCString():"";
		m_sFileSize = obj["fileSize"].isString() ?  obj["fileSize"].asCString():"";
		m_sIdPath   = obj["idPath"].isString()	 ?  obj["idPath"].asCString():"";
		m_sNetDiskpath = obj["netDiskPath"].isString() ?  obj["netDiskPath"].asCString():"";
		NormalPath(m_sNetDiskpath);
		m_sOpenMode    = obj["openMode"].isString()	?  obj["openMode"].asCString():"";
		m_sDataId      = obj["dataId"].isString()	?  obj["dataId"].asCString():"";
	}
	catch(...)
	{
		return FALSE;
	}
	return TRUE;
}
CString CInterfaceData_OpenFile::GetResultJson()
{
	try
	{
		Json::Value objOut;
		objOut["flag"]=  Json::Value((LPCTSTR)m_sFlag);
		objOut["msg"] =  Json::Value((LPCTSTR)m_sMsg);
		CString str = objOut.toStyledString().c_str();
		return str;
	}
	catch(...)
	{
		return "";
	}
}
CInterfaceData_DownloadFiles * CInterfaceData_OpenFile::ConvertToDownloadObj(CString strLocalPath)
{
	CInterfaceData_DownloadFiles *pIData = new CInterfaceData_DownloadFiles;
	CInterfaceData_DownloadFile *pDownLoadPath = new CInterfaceData_DownloadFile;
	pDownLoadPath->m_sUserName = m_sUserName;
	pDownLoadPath->m_sFlowId   = m_sFlowId; 
	pDownLoadPath->m_sFileId   = m_sFileId;
	pDownLoadPath->m_sFileMd5  = m_sFileMd5; 
	pDownLoadPath->m_sFileSize = m_sFileSize;
	pDownLoadPath->m_sIdPath   = m_sIdPath; 
	pDownLoadPath->m_sLocalPath   = strLocalPath;
	pDownLoadPath->m_sNetDiskPath = m_sNetDiskpath;
	pIData->m_vecFiles.push_back(pDownLoadPath);
	return pIData;
}
CInterfaceData_UploadFile * CInterfaceData_OpenFile::ConvertToUploadObj()
{
	CInterfaceData_UploadFile *pIData = new CInterfaceData_UploadFile;
	pIData->m_sUserName = m_sUserName;
	pIData->m_sFlowId   = m_sFlowId;
	pIData->m_sIdPath   = m_sIdPath;
	pIData->m_sNetdiskPath = m_sNetDiskpath;
	pIData->m_sDataId = m_sDataId;

	return pIData;
}
CInterfaceData_QueryFilesOperation *CInterfaceData_OpenFile::ConvertToQueryFilesOperationObj()
{
	CInterfaceData_QueryFilesOperation *pIData = new CInterfaceData_QueryFilesOperation;
	pIData->m_sUserName = m_sUserName;
	pIData->m_sFlowId = m_sFlowId;

	CInterfaceData_QueryFilesOperationPath * QueryPath = new CInterfaceData_QueryFilesOperationPath;
	QueryPath->m_bIsFile = TRUE;
	QueryPath->m_sIdPath = m_sIdPath;
	QueryPath->m_sNetdiskPath = m_sNetDiskpath;
	pIData->m_vPath.push_back(QueryPath);
	return pIData;
}
void CInterfaceData_QueryDir::Clear(void)
{
	m_sMsg.Empty();
	m_sFlag.Empty();
	m_Dirs.clear();
	m_bRecursion = false;
}
BOOL CInterfaceData_QueryDir::Parse( LPCTSTR lpszData )
{
	Clear();
	CString strSrc = lpszData == NULL ? "" : lpszData;
	if (strSrc.IsEmpty()) return FALSE;

	Json::Reader reader;
	Json::Value obj;

	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) return FALSE;
		if (!obj.isObject()) return FALSE;

		CString strRecursion = obj["isRecursion"].isString() ? obj["isRecursion"].asCString() : _T("FALSE");
		if(strRecursion.CompareNoCase("TRUE") == 0)
			m_bRecursion  = true;
	    else m_bRecursion = false;

		if (!obj["dirs"].isArray())
			return FALSE;

		Json::Value jDirs = obj["dirs"];
		int iCount = jDirs.size();
		for (int i=0;i<iCount;i++)
		{
			if (!jDirs[i].isString())
			{
				Clear();
				return FALSE;
			}
			m_Dirs.push_back(jDirs[i].asCString());
		}
	}
	catch(...)
	{
		return FALSE;
	}
	m_sFlag = "true";
	return TRUE;
}

static void GetFolderItem(const struct _Folder_Item & fItem,Json::Value & jItem,const bool & bRecursion)
{
	jItem["name"] = Json::Value((LPCTSTR)(fItem.m_strPath));
	if (fItem.m_bIsFile)
	{
		jItem["type"] = "file";
	}
	else
	{
		jItem["type"] = "dir";
		if (bRecursion)
		{
			Json::Value jChildren(Json::arrayValue);
			for (size_t i=0;i<fItem.m_SubItem.size();i++)
			{
				Json::Value jChild;
				GetFolderItem(fItem.m_SubItem[i],jChild,bRecursion);
				jChildren.append(jChild);
			}
			jItem["children"] = jChildren;
		}
	}
}

CString CInterfaceData_QueryDir::GetQueryDirResultJson(const struct _Folder_Item & fItem)
{
	try
	{
		CString strResult;
		Json::Value obj(Json::objectValue);
		obj["msg"] = Json::Value( (LPCTSTR)m_sMsg);
		obj["flag"] = Json::Value((LPCTSTR)m_sFlag);

		Json::Value jDirs(Json::arrayValue);
		for(size_t i=0;i<m_Dirs.size();i++)
		{
			Json::Value jDir(Json::objectValue);
			jDir["dir"] =  Json::Value(LPCTSTR(m_Dirs[i]));
			Json::Value jItems(Json::arrayValue);

			const _Folder_Item & OneQueryFolder = fItem.m_SubItem[i];
			for (size_t j=0;j<OneQueryFolder.m_SubItem.size();j++)
			{
				Json::Value jItem;
				GetFolderItem(OneQueryFolder.m_SubItem[j],jItem,m_bRecursion);
				jItems.append(jItem);
			}
			
			jDir["items"] = jItems;
			jDirs.append(jDir);
		}
		obj["dirs"] = jDirs;
		strResult = obj.toStyledString().c_str();
		return strResult;
	}
	catch (...)
	{
		return "";
	}
}
CString CInterfaceData_QueryDir::GetQueryDirSubCountResultJson(const struct _Folder_Item & fItem)
{
	try
	{
		CString strResult;
		Json::Value obj(Json::objectValue);
		obj["msg"] = Json::Value( (LPCTSTR)m_sMsg);
		obj["flag"] = Json::Value((LPCTSTR)m_sFlag);
		Json::Value jDirs(Json::arrayValue);
		for(size_t i=0;i<m_Dirs.size();i++)
		{
			Json::Value jDir(Json::objectValue);
			jDir["dir"] =  Json::Value(LPCTSTR(m_Dirs[i]));

			int iFileCount   = 0;
			int iFolderCount = 0;
			const _Folder_Item & OneQueryFolder = fItem.m_SubItem[i];
			CFilePro::GetFolderItemCount(OneQueryFolder,iFolderCount,iFileCount,m_bRecursion);
			CString strTmp;
			strTmp.Format("%d",iFolderCount);
			jDir["folderCount"] =  Json::Value((LPCTSTR)strTmp);
			strTmp.Format("%d",iFileCount);
			jDir["fileCount"]   =  Json::Value((LPCTSTR)strTmp);
			jDirs.append(jDir);
		}
		obj["dirs"] = jDirs;
		strResult = obj.toStyledString().c_str();
		return strResult;
	}
	catch (...)
	{
		return "";
	}
}

void CInterfaceData_ReName::Clear(void)
{
	m_sUserName.Empty();
	m_sFlowId.Empty();
	m_sPath.Empty();
	m_sNewName.Empty();
	m_sMsg.Empty();
	m_sFlag.Empty();
}
BOOL CInterfaceData_ReName::Parse( LPCTSTR lpszData )
{
	if (lpszData==NULL) lpszData = "";
	CString strSrc = lpszData;
	if (strSrc.IsEmpty()) return FALSE;

	Json::Reader reader;
	Json::Value obj;

	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) return FALSE;
		if (!obj.isObject()) return FALSE;

		m_sPath = obj["path"].isString() ? obj["path"].asCString() : _T("");
		m_sUserName = obj["userName"].isString() ? obj["userName"].asCString() : _T("");
		m_sFlowId = obj["flowId"].isString() ? obj["flowId"].asCString() : _T("");
		m_sFlowId = ReplaceFlowSc(m_sFlowId);
		m_sNewName = obj["newname"].isString() ? obj["newname"].asCString() : _T("");
	}
	catch (...)
	{
		Clear();
		return FALSE;
	}

	return TRUE;
}

void CInterfaceData_DeleteItem::Clear(void)
{
	m_sUserName.Empty();
	m_sFlowId.Empty();
	m_sMsg.Empty();
	m_sFlag.Empty();
	m_vItems.clear();
}
BOOL CInterfaceData_DeleteItem::Parse( LPCTSTR lpszData )
{
	if (lpszData==NULL) lpszData = "";
	CString strSrc = lpszData;
	if (strSrc.IsEmpty()) return FALSE;

	Json::Reader reader;
	Json::Value obj;

	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) return FALSE;
		if (!obj.isObject()) return FALSE;

		m_sUserName = obj["userName"].isString() ? obj["userName"].asCString() : _T("");
		m_sFlowId = obj["flowId"].isString() ? obj["flowId"].asCString() : _T("");
		m_sFlowId = ReplaceFlowSc(m_sFlowId);

		Json::Value objItems = obj["items"];

		if (!objItems.isArray()) 
			{
				Clear();
				return FALSE;
			}

		size_t iSize = objItems.size();
		CString strType = "";
		for (size_t i=0;i<iSize;i++)
		{
			SDeleteItem SItem;
			SItem.m_strPath = objItems[i]["path"].isString()? objItems[i]["path"].asCString():_T("");
			strType = objItems[i]["type"].isString()? objItems[i]["type"].asCString():_T("");
			if (strType.CompareNoCase("file")==0) SItem.m_bIsFile = TRUE;
			else if (strType.CompareNoCase("folder")==0) SItem.m_bIsFile = FALSE;
			else SItem.m_bIsFile = FALSE;

			m_vItems.push_back(SItem);
		}
	
	}
	catch (...)
	{
		Clear();
		return FALSE;
	}

	return TRUE;
}
void CInterfaceData_PasteFiles::Clear(void)
{
	m_sUserName.Empty();
	m_vFiles.clear();
}
BOOL CInterfaceData_PasteFiles::Parse( LPCTSTR lpszData )
{
	if (lpszData==NULL) lpszData = "";
	CString strSrc = lpszData;
	if (strSrc.IsEmpty()) return FALSE;

	Json::Reader reader;
	Json::Value obj;

	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) return FALSE;
		if (!obj.isObject()) return FALSE;

		m_sUserName = obj["userName"].isString() ? obj["userName"].asCString() : _T("");
		Json::Value & objFiles = obj["files"];
		if (!objFiles.isArray()) 
		{
			Clear();
			return FALSE;
		}

		size_t iSize = objFiles.size();
		CString strType = "";
		for (size_t i=0;i<iSize;i++)
		{
			Json::Value & srcObj = objFiles[i]["src"];
			Json::Value & dstObj = objFiles[i]["dst"];
			if (!srcObj.isObject() || !dstObj.isObject())
			{
				Clear();
				return FALSE;
			}

			SPasteFile PasteObj;
			PasteObj.m_sSrcFlowId = srcObj["flowId"].isString() ? srcObj["flowId"].asCString() : _T("");
			PasteObj.m_sSrcNetDiskPath = srcObj["netDiskPath"].isString() ? srcObj["netDiskPath"].asCString() : _T("");

			PasteObj.m_sDstFlowId = dstObj["flowId"].isString() ? dstObj["flowId"].asCString() : _T("");
			PasteObj.m_sDstNetDiskPath = dstObj["netDiskPath"].isString() ? dstObj["netDiskPath"].asCString() : _T("");

			PasteObj.m_sIdPath = dstObj["idPath"].isString() ? dstObj["idPath"].asCString() : _T("");
			m_vFiles.push_back(PasteObj);
		}

	}
	catch (...)
	{
		Clear();
		return FALSE;
	}
	return TRUE;
}