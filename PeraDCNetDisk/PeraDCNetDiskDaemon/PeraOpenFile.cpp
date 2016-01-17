#include "StdAfx.h"
#include "FilePro.h"
#include "PeraNetDiskIceI.h"
#include "InterfaceData.h"
#include "CommonMethod.h"
#include "PeraNetDiskSetting.h"
#include "PeraHttpDownloadMgr.h"
#include "PeraUploadManager.h"
#include "UploadManager.h"
#include "PeraOpenFile.h"
#include "DownUploadCommonFunc.h"
#include "CallHprose.h"
#include "Tools.h"
#include "UtilityFunc.h"
#include "FileFunc.h"
/*
 文件监控模式：当打开文件的时候，程序会监控该文件，并存储打开文件的进程句柄，该句柄可能是新创建的，也可能是之前打开文件已启动的
 例如word、excel、notepad等可以在多个进程中打开同一个文档，当用户修改了文档（保存操作或者退出之前保存），程序会检测到文件修改操作，
 并上传更新后的文件，当打开文件的进程句柄关闭后，该程序会检测到进程的关闭，并将该文件从监控列表中去除。

 存在的不足：当word打开多个文件的时候，关闭被监控的文档时，由于多个文件被同一个进程打开，进程没有退出，
 监控列表不能及时清除已被关闭的文件，导致多余监控，但不影响监控结果。
*/

map<CString,COpenData> COpenFilePro::mapOpenFile;
CRITICAL_SECTION  COpenFilePro::g_cs;

BOOL FileIsUsed(CString strFile)
{
	if (!PathFileExists(strFile))
		return FALSE;

	FILE *pFile = NULL;
	fopen_s(&pFile,strFile,"w");
	if (pFile==NULL)
		return TRUE;

	fclose(pFile);
	return FALSE;
}
DWORD WINAPI COpenFilePro::ProcessChangeThreadPro(LPVOID param)
{
	map<CString,COpenData>::iterator iter;
	vector<CString> vSaveFiles;
	while(1)
	{
		vSaveFiles.clear();
		EnterCriticalSection(&g_cs);
		iter = mapOpenFile.begin();
		ULONGLONG uTick = GetTickCount(); 
		// 找出需要保存的文件
		//DWORD dExitCode = 0;
		//BOOL bOK = FALSE;
		for (iter;iter!=mapOpenFile.end();iter++)
		{
			if ((iter->second.m_uLastModifyTime !=0) && (uTick - iter->second.m_uLastModifyTime> 500))
				vSaveFiles.push_back(iter->first);

			//if (iter->second.m_handleOpenProcess !=NULL) // OpenAs 模式打开的进程句柄未知
			//{
			//	bOK = GetExitCodeProcess((HANDLE)iter->second.m_handleOpenProcess,&dExitCode);
			//	if (bOK && dExitCode != STILL_ACTIVE) // 监测到进程退出
			//	{
			//		if (iter->second.m_uLastModifyTime!=0)
			//		{
			//			ZTools::WriteZToolsFormatLog("检测到打开文件【%s】的进程关闭，且文件已经修改，将执行上传操作...",iter->first);
			//			vSaveFiles.push_back(iter->first); //关闭进程时监测到文件已经修改
			//		}

			//		iter->second.m_bExitOpenProcess = TRUE; // 监测到打开文件的进程已经关闭
			//	}
			//}
			
		}
		// 保存文件，并清空保存列表
		for (size_t i = 0; i< vSaveFiles.size();i++)
		{
			CString str = vSaveFiles[i];
			UploadModifiedFile(str,mapOpenFile[str].m_IData);
			iter = mapOpenFile.find(str);
			iter->second.m_uLastModifyTime = 0;
			//判断进程是否结束，则去下对该文件的监控
		}
		// 清除进程已关闭的文件记录
		//for (iter = mapOpenFile.begin();iter!=mapOpenFile.end();)
		//{
		//	if (iter->second.m_bExitOpenProcess) // 检测到进程退出，则删除该监控
		//	{
		//		ZTools::WriteZToolsFormatLog("打开文件%s的进程已退出，结束对该文件的监控...",iter->first);
		//		mapOpenFile.erase(iter++);
		//	}
		//	else
		//		iter++;
		//}
		
		LeaveCriticalSection(&g_cs);
		Sleep(100);
	}
	return 0;
}
DWORD WINAPI COpenFilePro::NotifyThreadPro(LPVOID param)
{
	// 获取要监控的目录的句柄
	MonitorThreadParam Threadparam = *(MonitorThreadParam*)param;
	delete (MonitorThreadParam*)param;
	param = NULL;

	CString strDir  = Threadparam.m_strDir;
	strDir.Replace("/","\\");
	if (strDir.Right(1)!="\\")
		strDir.AppendChar('\\');

	MakeSureDirectoryPathExists(strDir); //确保监控的目录存在
	HANDLE hDir = CreateFile( strDir, FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | 
		FILE_SHARE_WRITE /*|*/ 
		/*FILE_SHARE_DELETE*/, NULL, 
		OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | 
		FILE_FLAG_OVERLAPPED, NULL); 
	if (hDir == INVALID_HANDLE_VALUE) 
	{ 

		printf("hDir:INVALID_HANDLE_VALUE\r\n");
		return 1;
	} 
	char notifyMsg[1024] = {0};
	wchar_t wstrFileName[1024]={0};
	CString strFileName;
	DWORD dRetBytes = 0;

	FILE_NOTIFY_INFORMATION *pnotify=(FILE_NOTIFY_INFORMATION *)notifyMsg; 
	FILE_NOTIFY_INFORMATION *tmp; 

	while (TRUE) 
	{ 
		_strset_s(notifyMsg,0);
		if(!ReadDirectoryChangesW(hDir, &notifyMsg, sizeof(notifyMsg),
			TRUE, FILE_NOTIFY_CHANGE_ATTRIBUTES,
			      /* FILE_NOTIFY_CHANGE_FILE_NAME| 
				  FILE_NOTIFY_CHANGE_DIR_NAME| 
				  FILE_NOTIFY_CHANGE_ATTRIBUTES| 
				  FILE_NOTIFY_CHANGE_SIZE| 
				  FILE_NOTIFY_CHANGE_LAST_WRITE| 
				  FILE_NOTIFY_CHANGE_LAST_ACCESS| 
				  FILE_NOTIFY_CHANGE_CREATION| 
				  FILE_NOTIFY_CHANGE_SECURITY,  */
			&dRetBytes, NULL, NULL)) 
			continue;

		tmp = pnotify;
		while(1) 
		{
			StrCpyNW(wstrFileName,tmp->FileName,tmp->FileNameLength/2 + 1);
			strFileName = (LPCTSTR)_bstr_t(wstrFileName);
			strFileName = strDir + CString("\\") + strFileName;
			strFileName.MakeLower();
			strFileName.Replace("\\\\","\\");

			EnterCriticalSection(&g_cs);
			bool bNotFind = mapOpenFile.find(strFileName)== mapOpenFile.end();
			LeaveCriticalSection(&g_cs);
			if (bNotFind) // 不在监控列表中
			{
				if (tmp->NextEntryOffset > 0)
				{
					tmp = (FILE_NOTIFY_INFORMATION*)((char*)tmp + tmp->NextEntryOffset);
					continue; 
				}
				else
					break;
			}

			if (tmp->Action == FILE_ACTION_MODIFIED) // 文件内容发生变化
			{
				EnterCriticalSection(&g_cs);
				mapOpenFile[strFileName].m_uLastModifyTime = GetTickCount();//记录修改进行的时间
				LeaveCriticalSection(&g_cs);
			}
			if (tmp->NextEntryOffset > 0)
			{
				tmp = (FILE_NOTIFY_INFORMATION*)((char*)tmp + tmp->NextEntryOffset);
			}
			else
				break;
		}

	}

	return 0;
}
CString COpenFilePro::DownloadToLocal(CInterfaceData_OpenFile &IData)
{
	CString strLocalTemp = theSetting.GetCacheDir();
	strLocalTemp = strLocalTemp + CString("\\") + IData.m_sUserName + CString("\\") + IData.m_sFlowId +  CString("\\") + IData.m_sNetDiskpath;
	strLocalTemp.Replace("\\\\","\\");
	ZTools::WriteZToolsFormatLog("下载fileId:【%s】,目标地址localpath:【%s】",IData.m_sFileId,strLocalTemp);
	if (PathFileExists(strLocalTemp))
	{
		ZTools::WriteZToolsFormatLog("下载目标文件【%s】已经存在，开始比较文件大小和md5...",strLocalTemp);
		LONGLONG fsize = FileSize(strLocalTemp);
		string strFileSize = ZTools::FormatString("%lld",fsize);
		ZTools::WriteZToolsFormatLog("本地缓存文件大小为[%s]",strFileSize.c_str());
		BOOL bDeleteLocal = FALSE;
		if (strFileSize.compare(IData.m_sFileSize)==0)
		{
			ZTools::WriteZToolsLog("文件大小一致，开始比较MD5值...");
			string strMd5 = GetFileMd5((LPCTSTR)strLocalTemp);
			ZTools::WriteZToolsFormatLog("本地缓存中文件MD5值为[%s]",strMd5.c_str());
			if (IData.m_sFileMd5.CompareNoCase(strMd5.c_str())==0)
			{
				ZTools::WriteZToolsLog("文件大小和MD5均相同,直接打开本地该文件...");
				return strLocalTemp;
			}
			else
			{
				ZTools::WriteZToolsLog("文件大小一致，但MD5不同,删除本地该文件，重新下载...");
				bDeleteLocal = TRUE;
			}
		}
		else
		{
			ZTools::WriteZToolsFormatLog("下载目标文件【%s】已经存在，md5或文件大小不一致，删除本地文件，重新下载...",strLocalTemp);
			bDeleteLocal = TRUE;
		}

		if (bDeleteLocal)
		{
			if (!FileIsUsed(strLocalTemp))
				remove((LPCTSTR)strLocalTemp);
			else
			{
				ZTools::WriteZToolsFormatLog("下载前检测到缓存文件【%s】被占用，无法替换，下载文件将重命名",strLocalTemp);
			}
		}

	}

	CInterfaceData_DownloadFiles *pIData = IData.ConvertToDownloadObj(strLocalTemp);
	ZTools::WriteZToolsFormatLog("向下载管理器增加下载任务:\n fileId = %s,\n md5 = %s,\n localPath = %s,\n netDiskPath = %s,\n idPath = %s,\n fileSize = %s\n",
		pIData->m_vecFiles[0]->m_sFileId,pIData->m_vecFiles[0]->m_sFileMd5,pIData->m_vecFiles[0]->m_sLocalPath,
		pIData->m_vecFiles[0]->m_sNetDiskPath,pIData->m_vecFiles[0]->m_sIdPath,pIData->m_vecFiles[0]->m_sFileSize);

	theDownloadMgr.Add(pIData); // 添加到下载管理器中
	delete pIData; 

	CInterfaceData_QueryFilesOperation *pIDataQuery = IData.ConvertToQueryFilesOperationObj();
	CString strDownStatus = "";
	//查询任务状态，看是否下载完成
	while(1)
	{
		pIDataQuery->m_vPath[0]->m_vResult.clear();
		if (!theDownloadMgr.Query(*pIDataQuery))
		{
			Sleep(100);
			continue;
		}
		if (pIDataQuery->m_vPath[0]->m_vResult.empty())
		{
			strDownStatus == "Error";
			ZTools::WriteZToolsFormatLog("无法查询到idPath = %s的下载任务\n",(LPCTSTR)pIDataQuery->m_vPath[0]->m_sIdPath);
			{
				if (PathFileExists(strLocalTemp)) // 界面定时器抢先删除了已完成的下载任务,此时判断本地文件是否是正确
				{

					string strFileSize = GetFileSize((LPCTSTR)strLocalTemp);
					if (strFileSize.compare(IData.m_sFileSize)==0)
					{
						ZTools::WriteZToolsLog("文件大小一致，开始比较MD5值...");
						string strMd5 = GetFileMd5((LPCTSTR)strLocalTemp);
						if (IData.m_sFileMd5.CompareNoCase(strMd5.c_str())==0)
						{
							ZTools::WriteZToolsLog("文件大小和MD5均相同,下载已经完成");
							strDownStatus = "Finished";
						}
					}
						
				}
				
			}
			break;
		}
		
		strDownStatus = pIDataQuery->m_vPath[0]->m_vResult[0]->m_sStatus;
		if(strDownStatus == "Finished" || strDownStatus == "Error") //完成或者出错
			break;
		else
			Sleep(100);
	}

	if (strDownStatus == "Finished")
	{
		CString strFilePath  = pIDataQuery->m_vPath[0]->m_vResult[0]->m_sRealPath;
		strLocalTemp = strFilePath;
		ZTools::WriteZToolsFormatLog("下载fileId为%s的文件成功，存储为%s",IData.m_sFileId,strFilePath);
		//IData.m_sFileMd5     = GetFileMd5((LPCTSTR)strFilePath).c_str();
		//IData.m_sFileSize    = GetFileSize((LPCTSTR)strFilePath).c_str();
	}
	else
	{
		strLocalTemp = "";// 下载失败
		ZTools::WriteZToolsFormatLog("下载fileId为%s的文件失败!",(LPCTSTR)IData.m_sFileId);

	}

	delete pIDataQuery;

	CInterfaceData_DoFilesOperation IDataFop;
	IDataFop.m_soperationFlag = "delete";
	IDataFop.m_sIdPath = IData.m_sIdPath;
	theDownloadMgr.DoFileOperation(&IDataFop); //下载任务结束后删除记录

	return strLocalTemp;
}
CString COpenFilePro::UploadModifiedFile(CString strLocal,CInterfaceData_OpenFile &IData)
{
	//上传之前需要删除该DataId的上传任务，否则上传失败
	ZTools::WriteZToolsFormatLog("检测到文件%s更新，开始执行上传...",(LPCTSTR)strLocal);
	CString strUploadUrl = theSetting.GetUploadUrl();
	//CallHprose call((LPCTSTR)strUploadUrl,"","",false);
	//call.deltask((LPCTSTR)IData.m_sDataId);

	// 执行上传
	CString strDataId = GetNewGUID();
	CUploadInputParam upParam(strUploadUrl,strLocal,IData.m_sDataId);
	CString strMapKey = IData.m_sUserName + CString("\\") + IData.m_sFlowId + CString("\\") + IData.m_sIdPath;
	strMapKey.Replace("\\\\","\\");
	CUploadManager::Instance()->AddToManager(strMapKey,upParam);

	CString strUploadStatus;
	CInterfaceData_QueryFilesOperation *pQueryData = IData.ConvertToQueryFilesOperationObj();
	while(1)
	{
		pQueryData->m_vPath[0]->m_vResult.clear();
		CPeraUploadManager::Instance()->QueryUploadProgress(*pQueryData);
		

		strUploadStatus = pQueryData->m_vPath[0]->m_vResult[0]->m_sStatus;
		if(strUploadStatus == "finish" || strUploadStatus == "error") //完成或者出错
			break;
		else
			Sleep(100);
	}
	if (strUploadStatus=="finish")
	{
		IData.m_sFileId = pQueryData->m_vPath[0]->m_vResult[0]->m_sFileId;
		ZTools::WriteZToolsFormatLog("文件%s更新后上传成功，返回fileId为:%s",(LPCTSTR)strLocal,(LPCTSTR)IData.m_sFileId);
	}
	else
	{
		ZTools::WriteZToolsFormatLog("文件%s更新后上传失败，错误信息为:%s",(LPCTSTR)strLocal,pQueryData->m_vPath[0]->m_vResult[0]->m_sMsg);
	}
	
	delete pQueryData;
	return "";
}
void COpenFilePro::OpenFileLocal(CInterfaceData_OpenFile &IData)
{
	CString strAbsPath = IData.m_sNetDiskpath;
	strAbsPath = ConvertToAbsolutePath((LPCTSTR)IData.m_sNetDiskpath,(LPCTSTR)IData.m_sUserName,(LPCTSTR)IData.m_sFlowId).c_str();
	CFilePro pro;
	if (!pro.OpenFile(strAbsPath))
	{
		IData.m_sFlag = "false";
		IData.m_sMsg = pro.GetErrMsg();
		MessageBox(NULL,IData.m_sMsg,"提示",MB_OK|MB_ICONERROR);
	}
}
void COpenFilePro::OpenFileShared(CInterfaceData_OpenFile &IData)
{
	CString strFileDwon = DownloadToLocal(IData);
	CFilePro pro;
	if (!strFileDwon.IsEmpty())
	{
		if (!pro.OpenFile(strFileDwon))
		{
			IData.m_sFlag = "false";
			IData.m_sMsg = pro.GetErrMsg();
			MessageBox(NULL,IData.m_sMsg,"提示",MB_OK|MB_ICONERROR);
		}
	}
	else
	{
		IData.m_sFlag = "false";
		IData.m_sMsg = pro.GetErrMsg();
		//MessageBox(NULL,"下载文件失败","提示",MB_OK|MB_ICONERROR);
	}
}
void COpenFilePro::OpenFileNetDisk(CInterfaceData_OpenFile &IData)
{
	CString strFileDwon = DownloadToLocal(IData);
	CFilePro pro;
	HANDLE h;
	if (!strFileDwon.IsEmpty())
	{
		if (!pro.OpenFile(strFileDwon,&h))
		{
			IData.m_sFlag = "false";
			IData.m_sMsg = pro.GetErrMsg();
			MessageBox(NULL,IData.m_sMsg,"提示",MB_OK|MB_ICONERROR);
			return;
		}
	}
	else
	{
		IData.m_sFlag = "false";
		IData.m_sMsg = pro.GetErrMsg();
		//MessageBox(NULL,"下载文件失败","提示",MB_OK|MB_ICONERROR);
		return;
	}


	IData.m_sFlag = "true";
	IData.m_sMsg = "";
	static bool bCreated = false;
	if (!bCreated)
	{
		//开始监听文件的修改，并且在修改后上传
		ZTools::WriteZToolsLog("启动文件监控线程和文件更新处理线程...");
		MonitorThreadParam *param = new MonitorThreadParam;
		param->m_strDir = theSetting.GetCacheDir(); // 监控Cache目录
		HANDLE handle1  = CreateThread(NULL,0,NotifyThreadPro,param,0,NULL);
		HANDLE handle2  = CreateThread(NULL,0,ProcessChangeThreadPro,NULL,0,NULL);
		bCreated = true;
	}

	strFileDwon.MakeLower();
	strFileDwon.Replace("\\\\","\\");
	COpenData oData;
	oData.m_IData = IData;
	oData.m_sFilePath = strFileDwon;
	oData.m_uLastModifyTime = 0;
	oData.m_handleOpenProcess = h;
	oData.m_bExitOpenProcess = FALSE;

	EnterCriticalSection(&g_cs);
	ZTools::WriteZToolsFormatLog("向文件监控列表中增加监控对象：%s",strFileDwon);
	mapOpenFile[strFileDwon] = oData;
	LeaveCriticalSection(&g_cs);
}
DWORD WINAPI COpenFilePro::OpenFileThreadPro(LPVOID param)
{
	CInterfaceData_OpenFile *pDataCopy  = (CInterfaceData_OpenFile*)param;
	//打开文件前取消监控
	CString strLocalTemp = theSetting.GetCacheDir();
	strLocalTemp = strLocalTemp + CString("\\") + pDataCopy->m_sUserName + CString("\\") + pDataCopy->m_sFlowId +  CString("\\") + pDataCopy->m_sNetDiskpath;
	strLocalTemp.MakeLower();
	strLocalTemp.Replace("\\\\","\\");
	EnterCriticalSection(&g_cs);
	map<CString,COpenData>::iterator iterFind = mapOpenFile.find(strLocalTemp);
	if (iterFind != mapOpenFile.end())
	{
		ZTools::WriteZToolsFormatLog("下载前移除对【%s】文件的监控...",strLocalTemp);
		mapOpenFile.erase(iterFind); // 下载过程中监控需要移除，否则导致误上传
	}
	LeaveCriticalSection(&g_cs);

	if (pDataCopy->m_sOpenMode.Compare("local")==0)
	{
		 OpenFileLocal(*pDataCopy);
	}
	else if (pDataCopy->m_sOpenMode.Compare("netDisk")==0)
	{
		 OpenFileNetDisk(*pDataCopy);
	}
	else if (pDataCopy->m_sOpenMode.Compare("shared")==0)
	{
		 OpenFileShared(*pDataCopy);
	}
	else
	{
		pDataCopy->m_sMsg = "打开模式参数不正确";
		pDataCopy->m_sFlag = "false";
		ZTools::WriteZToolsFormatLog("打开文件模式参数%s错误!",pDataCopy->m_sOpenMode);
	}
	delete pDataCopy;
	return 0;
}
void COpenFilePro::OnOpenFile(CInterfaceData_OpenFile &IData)
{
	static bool bInitialized = false;
	if (!bInitialized)
	{
		InitializeCriticalSection(&g_cs);
		bInitialized = true;
	}
	
	if(IData.m_sOpenMode.Compare("local")==0)
	{
		CString strAbsPath = ConvertToAbsolutePath((LPCTSTR)IData.m_sNetDiskpath,
			(LPCTSTR)IData.m_sUserName,(LPCTSTR)IData.m_sFlowId).c_str();
		if (!PathFileExists(strAbsPath))
		{
			IData.m_sFlag = "false";
			IData.m_sMsg.Format("文件%s不存在!",strAbsPath);
			return;
		}
	}

	{
		// 在一个线程中执行打开操作
		CInterfaceData_OpenFile *pDataCopy  = new CInterfaceData_OpenFile(IData);
		HANDLE hOpen =  CreateThread(NULL,0,OpenFileThreadPro,pDataCopy,0,NULL);
		IData.m_sFlag = "true";
		IData.m_sMsg  = "";
	}
}