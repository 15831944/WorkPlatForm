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
 �ļ����ģʽ�������ļ���ʱ�򣬳�����ظ��ļ������洢���ļ��Ľ��̾�����þ���������´����ģ�Ҳ������֮ǰ���ļ���������
 ����word��excel��notepad�ȿ����ڶ�������д�ͬһ���ĵ������û��޸����ĵ���������������˳�֮ǰ���棩��������⵽�ļ��޸Ĳ�����
 ���ϴ����º���ļ��������ļ��Ľ��̾���رպ󣬸ó�����⵽���̵Ĺرգ��������ļ��Ӽ���б���ȥ����

 ���ڵĲ��㣺��word�򿪶���ļ���ʱ�򣬹رձ���ص��ĵ�ʱ�����ڶ���ļ���ͬһ�����̴򿪣�����û���˳���
 ����б��ܼ�ʱ����ѱ��رյ��ļ������¶����أ�����Ӱ���ؽ����
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
		// �ҳ���Ҫ������ļ�
		//DWORD dExitCode = 0;
		//BOOL bOK = FALSE;
		for (iter;iter!=mapOpenFile.end();iter++)
		{
			if ((iter->second.m_uLastModifyTime !=0) && (uTick - iter->second.m_uLastModifyTime> 500))
				vSaveFiles.push_back(iter->first);

			//if (iter->second.m_handleOpenProcess !=NULL) // OpenAs ģʽ�򿪵Ľ��̾��δ֪
			//{
			//	bOK = GetExitCodeProcess((HANDLE)iter->second.m_handleOpenProcess,&dExitCode);
			//	if (bOK && dExitCode != STILL_ACTIVE) // ��⵽�����˳�
			//	{
			//		if (iter->second.m_uLastModifyTime!=0)
			//		{
			//			ZTools::WriteZToolsFormatLog("��⵽���ļ���%s���Ľ��̹رգ����ļ��Ѿ��޸ģ���ִ���ϴ�����...",iter->first);
			//			vSaveFiles.push_back(iter->first); //�رս���ʱ��⵽�ļ��Ѿ��޸�
			//		}

			//		iter->second.m_bExitOpenProcess = TRUE; // ��⵽���ļ��Ľ����Ѿ��ر�
			//	}
			//}
			
		}
		// �����ļ�������ձ����б�
		for (size_t i = 0; i< vSaveFiles.size();i++)
		{
			CString str = vSaveFiles[i];
			UploadModifiedFile(str,mapOpenFile[str].m_IData);
			iter = mapOpenFile.find(str);
			iter->second.m_uLastModifyTime = 0;
			//�жϽ����Ƿ��������ȥ�¶Ը��ļ��ļ��
		}
		// ��������ѹرյ��ļ���¼
		//for (iter = mapOpenFile.begin();iter!=mapOpenFile.end();)
		//{
		//	if (iter->second.m_bExitOpenProcess) // ��⵽�����˳�����ɾ���ü��
		//	{
		//		ZTools::WriteZToolsFormatLog("���ļ�%s�Ľ������˳��������Ը��ļ��ļ��...",iter->first);
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
	// ��ȡҪ��ص�Ŀ¼�ľ��
	MonitorThreadParam Threadparam = *(MonitorThreadParam*)param;
	delete (MonitorThreadParam*)param;
	param = NULL;

	CString strDir  = Threadparam.m_strDir;
	strDir.Replace("/","\\");
	if (strDir.Right(1)!="\\")
		strDir.AppendChar('\\');

	MakeSureDirectoryPathExists(strDir); //ȷ����ص�Ŀ¼����
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
			if (bNotFind) // ���ڼ���б���
			{
				if (tmp->NextEntryOffset > 0)
				{
					tmp = (FILE_NOTIFY_INFORMATION*)((char*)tmp + tmp->NextEntryOffset);
					continue; 
				}
				else
					break;
			}

			if (tmp->Action == FILE_ACTION_MODIFIED) // �ļ����ݷ����仯
			{
				EnterCriticalSection(&g_cs);
				mapOpenFile[strFileName].m_uLastModifyTime = GetTickCount();//��¼�޸Ľ��е�ʱ��
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
	ZTools::WriteZToolsFormatLog("����fileId:��%s��,Ŀ���ַlocalpath:��%s��",IData.m_sFileId,strLocalTemp);
	if (PathFileExists(strLocalTemp))
	{
		ZTools::WriteZToolsFormatLog("����Ŀ���ļ���%s���Ѿ����ڣ���ʼ�Ƚ��ļ���С��md5...",strLocalTemp);
		LONGLONG fsize = FileSize(strLocalTemp);
		string strFileSize = ZTools::FormatString("%lld",fsize);
		ZTools::WriteZToolsFormatLog("���ػ����ļ���СΪ[%s]",strFileSize.c_str());
		BOOL bDeleteLocal = FALSE;
		if (strFileSize.compare(IData.m_sFileSize)==0)
		{
			ZTools::WriteZToolsLog("�ļ���Сһ�£���ʼ�Ƚ�MD5ֵ...");
			string strMd5 = GetFileMd5((LPCTSTR)strLocalTemp);
			ZTools::WriteZToolsFormatLog("���ػ������ļ�MD5ֵΪ[%s]",strMd5.c_str());
			if (IData.m_sFileMd5.CompareNoCase(strMd5.c_str())==0)
			{
				ZTools::WriteZToolsLog("�ļ���С��MD5����ͬ,ֱ�Ӵ򿪱��ظ��ļ�...");
				return strLocalTemp;
			}
			else
			{
				ZTools::WriteZToolsLog("�ļ���Сһ�£���MD5��ͬ,ɾ�����ظ��ļ�����������...");
				bDeleteLocal = TRUE;
			}
		}
		else
		{
			ZTools::WriteZToolsFormatLog("����Ŀ���ļ���%s���Ѿ����ڣ�md5���ļ���С��һ�£�ɾ�������ļ�����������...",strLocalTemp);
			bDeleteLocal = TRUE;
		}

		if (bDeleteLocal)
		{
			if (!FileIsUsed(strLocalTemp))
				remove((LPCTSTR)strLocalTemp);
			else
			{
				ZTools::WriteZToolsFormatLog("����ǰ��⵽�����ļ���%s����ռ�ã��޷��滻�������ļ���������",strLocalTemp);
			}
		}

	}

	CInterfaceData_DownloadFiles *pIData = IData.ConvertToDownloadObj(strLocalTemp);
	ZTools::WriteZToolsFormatLog("�����ع�����������������:\n fileId = %s,\n md5 = %s,\n localPath = %s,\n netDiskPath = %s,\n idPath = %s,\n fileSize = %s\n",
		pIData->m_vecFiles[0]->m_sFileId,pIData->m_vecFiles[0]->m_sFileMd5,pIData->m_vecFiles[0]->m_sLocalPath,
		pIData->m_vecFiles[0]->m_sNetDiskPath,pIData->m_vecFiles[0]->m_sIdPath,pIData->m_vecFiles[0]->m_sFileSize);

	theDownloadMgr.Add(pIData); // ��ӵ����ع�������
	delete pIData; 

	CInterfaceData_QueryFilesOperation *pIDataQuery = IData.ConvertToQueryFilesOperationObj();
	CString strDownStatus = "";
	//��ѯ����״̬�����Ƿ��������
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
			ZTools::WriteZToolsFormatLog("�޷���ѯ��idPath = %s����������\n",(LPCTSTR)pIDataQuery->m_vPath[0]->m_sIdPath);
			{
				if (PathFileExists(strLocalTemp)) // ���涨ʱ������ɾ��������ɵ���������,��ʱ�жϱ����ļ��Ƿ�����ȷ
				{

					string strFileSize = GetFileSize((LPCTSTR)strLocalTemp);
					if (strFileSize.compare(IData.m_sFileSize)==0)
					{
						ZTools::WriteZToolsLog("�ļ���Сһ�£���ʼ�Ƚ�MD5ֵ...");
						string strMd5 = GetFileMd5((LPCTSTR)strLocalTemp);
						if (IData.m_sFileMd5.CompareNoCase(strMd5.c_str())==0)
						{
							ZTools::WriteZToolsLog("�ļ���С��MD5����ͬ,�����Ѿ����");
							strDownStatus = "Finished";
						}
					}
						
				}
				
			}
			break;
		}
		
		strDownStatus = pIDataQuery->m_vPath[0]->m_vResult[0]->m_sStatus;
		if(strDownStatus == "Finished" || strDownStatus == "Error") //��ɻ��߳���
			break;
		else
			Sleep(100);
	}

	if (strDownStatus == "Finished")
	{
		CString strFilePath  = pIDataQuery->m_vPath[0]->m_vResult[0]->m_sRealPath;
		strLocalTemp = strFilePath;
		ZTools::WriteZToolsFormatLog("����fileIdΪ%s���ļ��ɹ����洢Ϊ%s",IData.m_sFileId,strFilePath);
		//IData.m_sFileMd5     = GetFileMd5((LPCTSTR)strFilePath).c_str();
		//IData.m_sFileSize    = GetFileSize((LPCTSTR)strFilePath).c_str();
	}
	else
	{
		strLocalTemp = "";// ����ʧ��
		ZTools::WriteZToolsFormatLog("����fileIdΪ%s���ļ�ʧ��!",(LPCTSTR)IData.m_sFileId);

	}

	delete pIDataQuery;

	CInterfaceData_DoFilesOperation IDataFop;
	IDataFop.m_soperationFlag = "delete";
	IDataFop.m_sIdPath = IData.m_sIdPath;
	theDownloadMgr.DoFileOperation(&IDataFop); //�������������ɾ����¼

	return strLocalTemp;
}
CString COpenFilePro::UploadModifiedFile(CString strLocal,CInterfaceData_OpenFile &IData)
{
	//�ϴ�֮ǰ��Ҫɾ����DataId���ϴ����񣬷����ϴ�ʧ��
	ZTools::WriteZToolsFormatLog("��⵽�ļ�%s���£���ʼִ���ϴ�...",(LPCTSTR)strLocal);
	CString strUploadUrl = theSetting.GetUploadUrl();
	//CallHprose call((LPCTSTR)strUploadUrl,"","",false);
	//call.deltask((LPCTSTR)IData.m_sDataId);

	// ִ���ϴ�
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
		if(strUploadStatus == "finish" || strUploadStatus == "error") //��ɻ��߳���
			break;
		else
			Sleep(100);
	}
	if (strUploadStatus=="finish")
	{
		IData.m_sFileId = pQueryData->m_vPath[0]->m_vResult[0]->m_sFileId;
		ZTools::WriteZToolsFormatLog("�ļ�%s���º��ϴ��ɹ�������fileIdΪ:%s",(LPCTSTR)strLocal,(LPCTSTR)IData.m_sFileId);
	}
	else
	{
		ZTools::WriteZToolsFormatLog("�ļ�%s���º��ϴ�ʧ�ܣ�������ϢΪ:%s",(LPCTSTR)strLocal,pQueryData->m_vPath[0]->m_vResult[0]->m_sMsg);
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
		MessageBox(NULL,IData.m_sMsg,"��ʾ",MB_OK|MB_ICONERROR);
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
			MessageBox(NULL,IData.m_sMsg,"��ʾ",MB_OK|MB_ICONERROR);
		}
	}
	else
	{
		IData.m_sFlag = "false";
		IData.m_sMsg = pro.GetErrMsg();
		//MessageBox(NULL,"�����ļ�ʧ��","��ʾ",MB_OK|MB_ICONERROR);
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
			MessageBox(NULL,IData.m_sMsg,"��ʾ",MB_OK|MB_ICONERROR);
			return;
		}
	}
	else
	{
		IData.m_sFlag = "false";
		IData.m_sMsg = pro.GetErrMsg();
		//MessageBox(NULL,"�����ļ�ʧ��","��ʾ",MB_OK|MB_ICONERROR);
		return;
	}


	IData.m_sFlag = "true";
	IData.m_sMsg = "";
	static bool bCreated = false;
	if (!bCreated)
	{
		//��ʼ�����ļ����޸ģ��������޸ĺ��ϴ�
		ZTools::WriteZToolsLog("�����ļ�����̺߳��ļ����´����߳�...");
		MonitorThreadParam *param = new MonitorThreadParam;
		param->m_strDir = theSetting.GetCacheDir(); // ���CacheĿ¼
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
	ZTools::WriteZToolsFormatLog("���ļ�����б������Ӽ�ض���%s",strFileDwon);
	mapOpenFile[strFileDwon] = oData;
	LeaveCriticalSection(&g_cs);
}
DWORD WINAPI COpenFilePro::OpenFileThreadPro(LPVOID param)
{
	CInterfaceData_OpenFile *pDataCopy  = (CInterfaceData_OpenFile*)param;
	//���ļ�ǰȡ�����
	CString strLocalTemp = theSetting.GetCacheDir();
	strLocalTemp = strLocalTemp + CString("\\") + pDataCopy->m_sUserName + CString("\\") + pDataCopy->m_sFlowId +  CString("\\") + pDataCopy->m_sNetDiskpath;
	strLocalTemp.MakeLower();
	strLocalTemp.Replace("\\\\","\\");
	EnterCriticalSection(&g_cs);
	map<CString,COpenData>::iterator iterFind = mapOpenFile.find(strLocalTemp);
	if (iterFind != mapOpenFile.end())
	{
		ZTools::WriteZToolsFormatLog("����ǰ�Ƴ��ԡ�%s���ļ��ļ��...",strLocalTemp);
		mapOpenFile.erase(iterFind); // ���ع����м����Ҫ�Ƴ������������ϴ�
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
		pDataCopy->m_sMsg = "��ģʽ��������ȷ";
		pDataCopy->m_sFlag = "false";
		ZTools::WriteZToolsFormatLog("���ļ�ģʽ����%s����!",pDataCopy->m_sOpenMode);
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
			IData.m_sMsg.Format("�ļ�%s������!",strAbsPath);
			return;
		}
	}

	{
		// ��һ���߳���ִ�д򿪲���
		CInterfaceData_OpenFile *pDataCopy  = new CInterfaceData_OpenFile(IData);
		HANDLE hOpen =  CreateThread(NULL,0,OpenFileThreadPro,pDataCopy,0,NULL);
		IData.m_sFlag = "true";
		IData.m_sMsg  = "";
	}
}