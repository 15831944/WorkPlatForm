#include "stdafx.h"
#include "Tools.h"
#include "CommonMethod.h"
#include "FileCopy.h"
#include "InterfaceData.h"
#include "CopyManager.h"
#include "PeraCopyManager.h"
#include "UserDefine.h"
#include "UtilityFunc.h"
#include "CPeraDataPersist.h"
#include "PtrVectorHeader.h"


CPeraCopyManager * CPeraCopyManager::m_PManagetr = NULL;
CPeraCopyManager::GabageCollector CPeraCopyManager::m_gc;

CPeraCopyManager * CPeraCopyManager::Instance()
{
	if (m_PManagetr == NULL)
		m_PManagetr = new CPeraCopyManager;

	return m_PManagetr;
}
CString CPeraCopyManager::AddCopyTask(CInterfaceData_CopyFiles * pIData,bool IsLoadFinished)
{
	ZTools::WriteZToolsLog("AddCopyTask开始调用...");
	try
	{
		string  strDestDir = ConvertToAbsolutePath("",(LPCTSTR)pIData->m_sUserName,(LPCTSTR)pIData->m_sFlowId);
		MakeSureDirectoryPathExists(strDestDir.c_str());
		for (size_t i=0;i<pIData->m_vFiles.size();i++)
		{
			string strDstFileName = (LPCTSTR)pIData->m_vFiles[i].m_sNetDiskPath;
			strDstFileName = strDestDir + string("\\") + strDstFileName;
			CString strTmp = strDstFileName.c_str();
			NormalPath(strTmp);
			strDstFileName = (LPCTSTR)strTmp;

			if (!IsLoadFinished)
			{
				CTaskData * pTask = pIData->GetTaskCopied(i);
				if (pTask)
				{
					pTask->m_dwStatus = CFileCopy::COPY_WAIT_SATRT;
					thePersistMrg.DoOperation(pTask); // 拷贝任务到xml数据库中
					delete pTask;
					pTask = NULL;
				}
			}
			//构建查询索引
			CString strMapKey = pIData->m_sUserName + CString("\\") + pIData->m_sFlowId + CString("\\") + pIData->m_vFiles[i].m_sIdPath;
			strMapKey.Replace("\\\\","\\");
			if (IsLoadFinished)
			{
				CCopyManager::Instance()->LoadFinishedCopyTask((LPCTSTR)pIData->m_vFiles[i].m_sLocalPath,strDstFileName.c_str(),
					strMapKey,pIData->m_vFiles[i].m_sIdPath,pIData->m_vFiles[i].m_sNetDiskPath);
			}
			else
			{
				CCopyManager::Instance()->AddToManager((LPCTSTR)pIData->m_vFiles[i].m_sLocalPath,strDstFileName.c_str(),
					strMapKey,pIData->m_vFiles[i].m_sIdPath,pIData->m_vFiles[i].m_sNetDiskPath);
			}

		}
		ZTools::WriteZToolsFormatLog("AddCopyTask开始调用结束,增加%d个拷贝任务",pIData->m_vFiles.size());
	}
	catch (...)
	{
		ZTools::WriteZToolsLog("AddCopyTask开始调用发生异常");
	}
	return DefaultReturnJson;
}
void CPeraCopyManager::QueryCopyProgress(CInterfaceData_QueryFilesOperation & IData)
{
	for (size_t iIndex=0;iIndex<IData.m_vPath.size();iIndex++)
	{
		CString strMapKey = IData.m_sUserName + CString("\\") + IData.m_sFlowId + CString("\\") + IData.m_vPath[iIndex]->m_sIdPath;
		strMapKey.Replace("\\\\","\\");
		vector<CFileCopy *> vQueryResult  = CCopyManager::Instance()->GetCopyProgressInfo(strMapKey,IData.m_vPath[iIndex]->m_bIsFile);

		for (size_t i=0;i<vQueryResult.size();i++)
		{
			CFileCopy * CopyParam = vQueryResult[i];
			if (NULL==CopyParam)
				continue;

			CInterfaceData_QueryFilesOperationResult * operationRes = new CInterfaceData_QueryFilesOperationResult;
			operationRes->m_sOperation = "copy";
			operationRes->m_sMsg = CopyParam->GetErrMsg();
			operationRes->m_sTotalSize.Format("%lld",CopyParam->m_iTotalBytes);
			operationRes->m_sFinishedSize.Format("%lld",CopyParam->m_iCurrentBytes);
			int iProgress = 0;
			if (CopyParam->m_iTotalBytes > 0)
				iProgress = int((CopyParam->m_iCurrentBytes/10.0)/(CopyParam->m_iTotalBytes/1000.0));
		
			if (CopyParam->m_iCurrentBytes == CopyParam->m_iTotalBytes )
				iProgress = 100;

			operationRes->m_sProgress.Format("%d",iProgress);
			if (CopyParam->m_CopyState== CFileCopy::COPY_WAIT_SATRT)
				operationRes->m_sStatus = OperationStatus_Waiting;
			else if (CopyParam->m_CopyState== CFileCopy::COPY_RUNNING)
				operationRes->m_sStatus = OperationStatus_Normal;
			else if (CopyParam->m_CopyState== CFileCopy::COPY_FINISH)
				operationRes->m_sStatus = OperationStatus_Finish;
			else if (CopyParam->m_CopyState== CFileCopy::COPY_STOP)
				operationRes->m_sStatus = OperationStatus_Stop;
			else if (CopyParam->m_CopyState== CFileCopy::COPY_PAUSE)
				operationRes->m_sStatus = OperationStatus_Pause;
			else if (CopyParam->m_CopyState== CFileCopy::COPY_RESUME)
				operationRes->m_sStatus = OperationStatus_Resume;
			else if (CopyParam->m_CopyState== CFileCopy::COPY_ERROR)
				operationRes->m_sStatus = OperationStatus_Error;

			operationRes->m_sNetDiskPath = CopyParam->m_sNetDiskPath; // 修改netDiskPath为传入的netDiskPath
			operationRes->m_sIdPath = CopyParam->m_sIdPath; // 修改idPath为传入的idPath
			IData.m_vPath[iIndex]->m_vResult.push_back(operationRes);
		}
	}
}
void CPeraCopyManager::DoFilesOperation(const CInterfaceData_DoFilesOperation &IData)
{
	CString strMapKey = IData.m_sUserName + CString("\\") + IData.m_sFlowId + CString("\\") + IData.m_sIdPath;
	strMapKey.Replace("\\\\","\\");

	if (IData.m_soperationFlag=="delete")
	{
		CCopyManager::Instance()->DeleteCopy(strMapKey,IData.m_bIsFile);
	}
	else if (IData.m_soperationFlag=="stop")
	{
		CCopyManager::Instance()->StopCopy(strMapKey,IData.m_bIsFile);
	}
	else if (IData.m_soperationFlag=="pause")
	{
		CCopyManager::Instance()->PauseCopy(strMapKey,IData.m_bIsFile);
	}
	else if (IData.m_soperationFlag=="resume")
	{
		CCopyManager::Instance()->ResumeCopy(strMapKey,IData.m_bIsFile);
	}
}
CPeraCopyManager::CPeraCopyManager()
{
	ZTools::WriteZToolsLog("业务层拷贝管理器CPeraCopyManager构造函数调用");
}
CPeraCopyManager::~CPeraCopyManager()
{
	ZTools::WriteZToolsLog("业务层拷贝管理器CPeraCopyManager析构函数调用");
}
void CPeraCopyManager::LoadHistoryCopyTask()
{
	ZTools::WriteZToolsLog("加载历史拷贝任务...");
	PERSIST_DATA vCopyTasks;
	if (!thePersistMrg.GetFileCopyData(vCopyTasks))
		return;

	int iRecory = 0;
	for (size_t i=0;i<vCopyTasks.size();i++)
	{
		bool bLoadFinished =  vCopyTasks[i]->m_dwStatus == CFileCopy::COPY_FINISH ? true:false;
		CInterfaceData_CopyFiles IData;
		IData.m_sUserName = vCopyTasks[i]->m_sUserName;
		IData.m_sFlowId   = vCopyTasks[i]->m_sFlowId;
		CInterfaceData_CopyFiles::SCopyFile cf = CInterfaceData_CopyFiles::FromTaskData(vCopyTasks[i]);
		IData.m_vFiles.push_back(cf);
		AddCopyTask(&IData,bLoadFinished);
		iRecory++;
	}
	PtrVectorClear(vCopyTasks);
	ZTools::WriteZToolsFormatLog("加载历史拷贝任务完成,共恢复%d条拷贝任务...",iRecory);
}
CString CPeraCopyManager::AddCopyTask(CInterfaceData_PasteFiles * pIData)
{
	ZTools::WriteZToolsLog("AddCopyTask(粘贴任务)开始调用...");
	try
	{
		for (size_t i=0;i<pIData->m_vFiles.size();i++)
		{
			string strSrcFile = pIData->m_vFiles[i].m_sSrcNetDiskPath;
			std::replace(strSrcFile.begin(),strSrcFile.end(),'/','\\');
			if (!IsAbsolutePath(strSrcFile) || strSrcFile.empty())
				strSrcFile = ConvertToAbsolutePath(strSrcFile,(LPCTSTR)pIData->m_sUserName,(LPCTSTR)pIData->m_vFiles[i].m_sSrcFlowId);

			if (!PathFileExists(strSrcFile.c_str()))
			{
				ZTools::WriteZToolsFormatLog("粘贴源文件%s不存在\n",strSrcFile.c_str());
				continue;
			}
			string strDstFile = pIData->m_vFiles[i].m_sDstNetDiskPath;
			std::replace(strDstFile.begin(),strDstFile.end(),'/','\\');
			if (!IsAbsolutePath(strDstFile) || strDstFile.empty())
				strDstFile = ConvertToAbsolutePath(strDstFile,(LPCTSTR)pIData->m_sUserName,(LPCTSTR)pIData->m_vFiles[i].m_sDstFlowId);

			MakeSureDirectoryPathExists(strDstFile.c_str());

			//构建查询索引
			CString strMapKey = pIData->m_sUserName + CString("\\") + pIData->m_vFiles[i].m_sDstFlowId + CString("\\") + pIData->m_vFiles[i].m_sIdPath;
			strMapKey.Replace("\\\\","\\");
			
				CCopyManager::Instance()->AddToManager(strSrcFile.c_str(),strDstFile.c_str(),
					strMapKey,pIData->m_vFiles[i].m_sIdPath,pIData->m_vFiles[i].m_sDstNetDiskPath);
		}
		ZTools::WriteZToolsFormatLog("AddCopyTask开始调用结束,增加%d个粘贴任务",pIData->m_vFiles.size());
	}
	catch (...)
	{
		ZTools::WriteZToolsLog("AddCopyTask（粘贴任务）开始调用发生异常");
	}
	return DefaultReturnJson;
}