#include "stdafx.h"
#include "PtrVectorHeader.h"
#include "InterfaceData.h"
#include "CommonMethod.h"
#include "ConfigFunc.h"
#include "PeraUploadManager.h"
#include "UploadManager.h"
#include "Tools.h"
#include "UserDefine.h"
#include "PeraNetDiskSetting.h"
#include "CPeraDataPersist.h"

extern CPeraDataPersist thePersistMrg;
CPeraUploadManager * CPeraUploadManager::m_PManagetr = NULL;
CPeraUploadManager::GabageCollector CPeraUploadManager::m_gc;

CPeraUploadManager * CPeraUploadManager::Instance()
{
	if (NULL == m_PManagetr)
		m_PManagetr = new CPeraUploadManager;

	return m_PManagetr;
}
CPeraUploadManager::CPeraUploadManager()
{
	ZTools::WriteZToolsLog("ҵ����ϴ����������캯��CPeraUploadManager����...");
}
CPeraUploadManager::~CPeraUploadManager()
{
	ZTools::WriteZToolsLog("ҵ����ϴ���������������~CPeraUploadManager����...");
}
CString CPeraUploadManager::AddUploadTask(CInterfaceData_UploadFile * pIData,bool bLoadFinished)
{
	ZTools::WriteZToolsLog("CPeraUploadManager::AddUploadTask��ʼ����...");
	string strAbsPath = (LPCTSTR)pIData->m_sNetdiskPath;
	if (!IsAbsolutePath(strAbsPath))
		strAbsPath = ConvertToAbsolutePath(strAbsPath,(LPCTSTR)pIData->m_sUserName,(LPCTSTR)pIData->m_sFlowId);

	//������ѯ����
	CString strMapKey = pIData->m_sUserName + CString("\\") + pIData->m_sFlowId + CString("\\") + pIData->m_sIdPath;
	strMapKey.Replace("\\\\","\\");
	CString strHprose = theSetting.GetUploadUrl(); //��ȡ�ϴ�URL��ַ
	CUploadInputParam param(strHprose,strAbsPath.c_str(),(LPCTSTR)pIData->m_sDataId);
	param.SetDataPath(pIData->m_sNetdiskPath,pIData->m_sIdPath);

	//���ϴ��������ӵ�xml���ݿ���
	
	if (!bLoadFinished)
	{
		//�ȴ���ʷ��¼��鿴��û�иü�¼������ʹ�����¼�����ݣ���md5ֵ��
		CTaskData *pTask = thePersistMrg.GetTaskDataCopied(pIData->m_sIdPath);
		if (NULL == pTask)
		   pTask = pIData->GetTaskCopied();

		if (pTask)
		{
			pTask->m_dwStatus = CallHprose::Upload_Waiting;
			thePersistMrg.DoOperation(pTask);
			delete pTask;
			pTask = NULL;
		}
		
	}

	if (bLoadFinished)
		CUploadManager::Instance()->LoadFinishedUploadTask(strMapKey,param);
	else
		CUploadManager::Instance()->AddToManager(strMapKey,param);
	
	ZTools::WriteZToolsLog("CPeraUploadManager::AddUploadTask���ý���");
	return DefaultReturnJson;
}
void CPeraUploadManager::QueryUploadProgress(CInterfaceData_QueryFilesOperation & IData)
{
	for (size_t iIndex=0;iIndex<IData.m_vPath.size();iIndex++)
	{
		//������ѯ����
		CString strMapKey = IData.m_sUserName + CString("\\") + IData.m_sFlowId + CString("\\") + IData.m_vPath[iIndex]->m_sIdPath;
		strMapKey.Replace("\\\\","\\");
		BOOL bIsFile = IData.m_vPath[iIndex]->m_bIsFile;
		vector<CallHprose *> vQueryResult = CUploadManager::Instance()->GetUploadProgressInfo(strMapKey,bIsFile);

		for (size_t i=0;i<vQueryResult.size();i++)
		{
			CallHprose * pHprose = vQueryResult[i];
			if (NULL==pHprose)
				continue;

			CInterfaceData_QueryFilesOperationResult * operationRes = new CInterfaceData_QueryFilesOperationResult;
			operationRes->m_sOperation = "upload";
			operationRes->m_sMsg = pHprose->dataFileObject.m_strMsg.c_str();
			operationRes->m_sTotalSize.Format("%lld",pHprose->dataFileObject.m_nFileSize);
			operationRes->m_sFinishedSize.Format("%lld",pHprose->dataFileObject.m_nFileSizeUploaded);
			int iProgress = 0;
			if (pHprose->dataFileObject.m_nFileSize>0)
				iProgress = int((pHprose->dataFileObject.m_nFileSizeUploaded/10.0)/(pHprose->dataFileObject.m_nFileSize/1000.0));
			if (pHprose->dataFileObject.m_nFileSizeUploaded == pHprose->dataFileObject.m_nFileSize)
				iProgress = 100;

			operationRes->m_sProgress.Format("%d",iProgress);

			CString strStatus = "";
			if (pHprose->m_TaskStatus == CallHprose::Upload_Waiting)
				strStatus = OperationStatus_Waiting;
			else if (pHprose->m_TaskStatus == CallHprose::Upload_Runing)
				strStatus = OperationStatus_Normal;
			else if(pHprose->m_TaskStatus == CallHprose::Upload_Error)
				strStatus = OperationStatus_Error;
			else if(pHprose->m_TaskStatus == CallHprose::Upload_STOP)
				strStatus = OperationStatus_Stop;	
			else if(pHprose->m_TaskStatus == CallHprose::Upload_PAUSE)
				strStatus = OperationStatus_Pause;	
			else if(pHprose->m_TaskStatus == CallHprose::Upload_RESUME)
				strStatus = OperationStatus_Resume;	
			else
				strStatus = OperationStatus_Finish;

			operationRes->m_sStatus = strStatus;
			operationRes->m_sNetDiskPath = pHprose->m_DataPathObj.m_sPath.c_str();
			operationRes->m_sIdPath = pHprose->m_DataPathObj.m_sIdPath.c_str();
			operationRes->m_sFileId = pHprose->dataFileObject.m_strFileId.c_str();// �����ļ�Id���ϴ�����˲���

			IData.m_vPath[iIndex]->m_vResult.push_back(operationRes);
		}
	}
}
void CPeraUploadManager::DoFilesOperation(const CInterfaceData_DoFilesOperation &IData)
{
	CString strMapKey = IData.m_sUserName + CString("\\") + IData.m_sFlowId + CString("\\") + IData.m_sIdPath;
	strMapKey.Replace("\\\\","\\");
	if (IData.m_soperationFlag=="delete")
	{
		CUploadManager::Instance()->DeleteUpload(strMapKey,IData.m_bIsFile);
	}
	else if (IData.m_soperationFlag=="stop")
	{
		CUploadManager::Instance()->StopUpload(strMapKey,IData.m_bIsFile);
	}
	else if (IData.m_soperationFlag=="pause")
	{
		CUploadManager::Instance()->PauseUpload(strMapKey,IData.m_bIsFile);
	}
	else if (IData.m_soperationFlag=="resume")
	{
		CUploadManager::Instance()->ResumeUpload(strMapKey,IData.m_bIsFile);
	}
}
void CPeraUploadManager::LoadHistoryUploadTask()
{
	ZTools::WriteZToolsLog("������ʷ�ϴ�����...");
	PERSIST_DATA vUploads;
	if (!thePersistMrg.GetUpLoadData(vUploads))
		return;

	// �������δ��ɺͳ��������
	int iRecory = 0;
	for (size_t i=0;i<vUploads.size();i++)
	{
		bool bLoadFinished =  vUploads[i]->m_dwStatus == CallHprose::Upload_Finish ? true:false;
		CInterfaceData_UploadFile IData(vUploads[i]);
		AddUploadTask(&IData,bLoadFinished);
	}
	PtrVectorClear(vUploads);
	ZTools::WriteZToolsFormatLog("������ʷ�ϴ��������,���ָ�%d���ϴ�����...",iRecory);
}
CString CPeraUploadManager::AddOutPutTask(CInterfaceData_UploadFile * pIData)
{
	ZTools::WriteZToolsLog("CPeraUploadManager::AddOutPutTask��ʼ����...");
	string strAbsPath = (LPCTSTR)pIData->m_sNetdiskPath;
	std::replace(strAbsPath.begin(),strAbsPath.end(),'/','\\');
	if (!IsAbsolutePath(strAbsPath))
		strAbsPath = ConvertToAbsolutePath(strAbsPath,(LPCTSTR)pIData->m_sUserName,(LPCTSTR)pIData->m_sFlowId);

	//������ѯ����
	CString strMapKey = pIData->m_sUserName + CString("\\") + pIData->m_sFlowId + CString("\\") + pIData->m_sIdPath;
	strMapKey.Replace("\\\\","\\");

	CString strHprose = GetConfig("hprose","hproseRpc"); //��ȡ�ύ�����Ҫ�õ��ϴ�URL��ַ

	CUploadInputParam param(strHprose,strAbsPath.c_str(),(LPCTSTR)pIData->m_sDataId);
	param.SetDataPath(pIData->m_sNetdiskPath,pIData->m_sIdPath);
	param.m_bCreateIndex = TRUE;

	CUploadManager::Instance()->AddToManager(strMapKey,param);

	ZTools::WriteZToolsLog("CPeraUploadManager::AddOutPutTask���ý���");
	return DefaultReturnJson;
}