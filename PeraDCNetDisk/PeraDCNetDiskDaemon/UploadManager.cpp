#include "stdafx.h"
#include "UploadManager.h"
#include "PeraNetDiskSetting.h"
#include "CPeraDataPersist.h"

CRITICAL_SECTION g_UpCriticalSection; // �����ٽ���
CUploadManager * CUploadManager::m_PManagetr = NULL;
CUploadManager::GabageCollector CUploadManager::m_gc;

CUploadManager * CUploadManager::Instance()
{
	if (m_PManagetr == NULL)
		m_PManagetr = new CUploadManager;

	return m_PManagetr;
}
CUploadManager::CUploadManager()
{
	ZTools::WriteZToolsLog("�ϴ�������CUploadManager���캯������...");
	m_iCurrentRunningThreadCount = 0;
	GetSystemConfig();
	InitializeCriticalSection(&g_UpCriticalSection);
	StartManagerThread();
}
void CUploadManager::StartManagerThread()
{
	//�ڴ�����һ���໤���̣��������̵߳�״̬
	HANDLE hThread =CreateThread(NULL,0,UploadManagerThreadPro,NULL,0,NULL);

}
void CUploadManager::GetSystemConfig()
{
	m_MaxThreadCount = theSetting.GetUploadMaxTask();
}
CUploadManager::~CUploadManager()
{
	ZTools::WriteZToolsLog("�ϴ�������CUploadManager������������...");
	DestoyrUploadProgressInfo();
	DeleteCriticalSection(&g_UpCriticalSection); 
}
void CUploadManager::SetMaxThreadCount(int iThreadCount)
{
	EnterCriticalSection(&g_UpCriticalSection);  // �߳�ͬ����
	if (iThreadCount > 20 || iThreadCount < 1)
		iThreadCount = 5;

	m_MaxThreadCount = iThreadCount;
	LeaveCriticalSection(&g_UpCriticalSection); // �ͷ��߳���
}
void CUploadManager::LoadFinishedUploadTask(CString str ,const CUploadInputParam & param)
{
	ZTools::WriteZToolsFormatLog("CUploadManager::LoadFinishedUploadTask����,����ֵΪ:%s",(LPCTSTR)str);

	EnterCriticalSection(&g_UpCriticalSection); 
	map<CString,CallHprose *>::iterator  iterFind = m_UploadTaskMap.find(str);
	if (iterFind != m_UploadTaskMap.end())
		return; // ���ظ�������

	CTaskData * pTask = thePersistMrg.GetTaskDataCopied(param.m_strIdPath);
	CallHprose *phprose = new CallHprose((LPCTSTR)param.m_strHproseRpc,
		(LPCTSTR)param.m_strFilePath,(LPCTSTR)param.m_strIndex);

	phprose->m_DataPathObj.m_sPath   = (LPCTSTR)param.m_strPath;
	phprose->m_DataPathObj.m_sIdPath = (LPCTSTR)param.m_strIdPath;
	phprose->m_TaskStatus = CallHprose::Upload_Finish;
	if (pTask)
	{
		phprose->dataFileObject.m_strFileId = (LPCTSTR)pTask->m_sFileId;
		phprose->dataFileObject.m_strMd5  = (LPCTSTR)pTask->m_sFileMd5;
		phprose->dataFileObject.m_strHproseRpc = (LPCTSTR)pTask->m_sUrl;
		phprose->m_strFileSize = (LPCTSTR)pTask->m_sTotalSize;
		phprose->dataFileObject.m_nFileSizeUploaded = phprose->dataFileObject.m_nFileSize;
		delete pTask;
		pTask = NULL;
	}

	m_UploadTaskMap[str] = phprose; // ���ӵ������¼����

	LeaveCriticalSection(&g_UpCriticalSection); 
}
void CUploadManager::AddToManager(CString str,const CUploadInputParam & param)
{
	ZTools::WriteZToolsFormatLog("CUploadManager::AddToManager����,����ֵΪ:%s",(LPCTSTR)str);
	CallHprose *phprose = new CallHprose((LPCTSTR)param.m_strHproseRpc,
		(LPCTSTR)param.m_strFilePath,(LPCTSTR)param.m_strIndex);

	phprose->m_DataPathObj.m_sPath   = (LPCTSTR)param.m_strPath;
	phprose->m_DataPathObj.m_sIdPath = (LPCTSTR)param.m_strIdPath;
	phprose->m_bCreateIndexForFileInfo = param.m_bCreateIndex;

	ZTools::WriteZToolsFormatLog("CUploadManager::AddToManager��ʼ��ȡ�߳���...");
	EnterCriticalSection(&g_UpCriticalSection); 
	ZTools::WriteZToolsFormatLog("CUploadManager::AddToManager��ȡ�߳����ɹ�...");
	map<CString,CallHprose *>::iterator  iterFind = m_UploadTaskMap.find(str);
	if (iterFind == m_UploadTaskMap.end())
		m_UploadTaskMap[str] = phprose; // ���ӵ������¼����
	else //�ظ���ӵ����������ʱ��ִ���滻����
	{
		//return; // ���������ڹ���������
		ZTools::WriteZToolsFormatLog("AddToManager����ֵ:%s�ظ��������Ǿ�����",(LPCTSTR)str);
		iterFind->second->m_TaskStatus = CallHprose::Upload_DELET; //����ȡ����־��ʹ���̰߳�ȫ����
		map<CallHprose *,CWinThread *>::iterator iterThread = m_MapUpThread.find(iterFind->second);
		if (iterThread!=m_MapUpThread.end())
		{
			HANDLE hThread = iterThread->second->m_hThread;
			ZTools::WriteZToolsLog("�ȴ��������̰߳�ȫ����...");
			WaitForSingleObject(hThread,INFINITE);
			DWORD dExitCode = 0;
			GetExitCodeThread(hThread,&dExitCode);
			if (dExitCode!=STILL_ACTIVE)
				ZTools::WriteZToolsLog("�������߳��Ѱ�ȫ����...");

			delete iterThread->second;
			m_iCurrentRunningThreadCount--;
		}
		delete iterFind->second;
		m_UploadTaskMap.erase(iterFind);
		m_UploadTaskMap[str] = phprose; // ���ӵ������¼����
	}


	if (m_iCurrentRunningThreadCount >= m_MaxThreadCount) //�Ѿ��ﵽ�߳�����
	{
		ZTools::WriteZToolsFormatLog("CUploadManager::AddToManager �Ѿ��ﵽ�߳�����,����Ⱥ���У��ͷ��߳���...");
		UniquePushWaitTask(str);// ��ȴ�����������һ������
		LeaveCriticalSection(&g_UpCriticalSection);  //�ǵý�������������߳̽�����
		return;
	}
	StartOneUploadThread(phprose); // �����ϴ�����
	ZTools::WriteZToolsFormatLog("CUploadManager::AddToManager�ͷ��߳���...");
	LeaveCriticalSection(&g_UpCriticalSection); 
	ZTools::WriteZToolsFormatLog("CUploadManager::AddToManager���ý���,����ֵΪ:%s",(LPCTSTR)str);
}
void CUploadManager::StartOneUploadThread(CallHprose *pHpose)
{
	ZTools::WriteZToolsFormatLog("CUploadManager::StartOneUploadThread��ʼ����...");
	if(NULL==pHpose)
		return;

	m_iCurrentRunningThreadCount++;
	m_MapUpThread[pHpose] = AfxBeginThread(CUploadManager::UploadThreadPro, pHpose, 0, 0, 0, 0);
	m_MapUpThread[pHpose]->m_bAutoDelete = FALSE;
	ZTools::WriteZToolsFormatLog("CUploadManager::StartOneUploadThread���ý������ϴ��߳�IDΪ:0x%x ...",m_MapUpThread[pHpose]->m_nThreadID);
}
UINT CUploadManager::UploadThreadPro(LPVOID lpParamter)
{
	CallHprose *phprose = (CallHprose*)lpParamter;
	if (!phprose)
		return 1;

	CTaskData * pTask = thePersistMrg.GetTaskDataCopied(phprose->m_DataPathObj.m_sIdPath.c_str());
	if (pTask)
	{
		pTask->m_dwStatus = CallHprose::Upload_Runing;
		if (!pTask->m_sUrl.IsEmpty())
			phprose->dataFileObject.m_strHproseRpc = (LPCTSTR)(pTask->m_sUrl);

		phprose->dataFileObject.m_strMd5 = (LPCTSTR)(pTask->m_sFileMd5);
		thePersistMrg.DoOperation(pTask); //����xml�м�¼״̬
		delete pTask;
		pTask = NULL;
	}

	phprose->UploadDefault(); //�ڸ��ϴ��̺߳����е����ϴ�
	pTask = thePersistMrg.GetTaskDataCopied(phprose->m_DataPathObj.m_sIdPath.c_str());
	if (pTask)
	{
		pTask->m_dwStatus = phprose->m_TaskStatus;
		pTask->m_sFileId   = phprose->dataFileObject.m_strFileId.c_str();
		pTask->m_sTotalSize = phprose->m_strFileSize.c_str();
		pTask->m_sFileMd5  = phprose->dataFileObject.m_strMd5.c_str();
		pTask->m_sUrl = phprose->dataFileObject.m_strHproseRpc.c_str();

		thePersistMrg.DoOperation(pTask); //����xml�м�¼״̬
		delete pTask;
		pTask = NULL;
	}
	return 0;
}
void CUploadManager::DeleteCanceledFromQueue()
{
	queue<CString> queTmp;
	map<CString,CallHprose*>::iterator iter;
	while(m_UploadQueue.size()>0)
	{
		CString str = m_UploadQueue.front();
		iter = m_UploadTaskMap.find(str);
		if (iter!=m_UploadTaskMap.end())
		{
			if (iter->second->m_TaskStatus != CallHprose::Upload_DELET)
			{
				queTmp.push(str);
			}
		}
		m_UploadQueue.pop();
	}

	m_UploadQueue = queTmp;
}
DWORD WINAPI CUploadManager::UploadManagerThreadPro(LPVOID lpParamter)
{
	CUploadManager * pManager = CUploadManager::Instance();
	while(1)
	{
		int & iRuningCount = pManager->m_iCurrentRunningThreadCount;
		if (!pManager->m_UploadTaskMap.empty())
		{
			//�ҳ���ɵģ���ȡ�������񣬽���������б���ɾ��
			EnterCriticalSection(&g_UpCriticalSection);
			if (pManager->m_UploadQueue.size()>0)
				pManager->DeleteCanceledFromQueue();

			map<CString,CallHprose*>::iterator iter = pManager->m_UploadTaskMap.begin();
			for (iter;iter!=pManager->m_UploadTaskMap.end();)
			{
				// �ڴ˴���delete,��֤���������е�ʱ��Ҳ�ܴ���ʷ��¼��ɾ��
				if (iter->second->m_TaskStatus== CallHprose::Upload_DELET)
				{
					map<CallHprose *, CWinThread *>::iterator iterFind = pManager->m_MapUpThread.find(iter->second);
					if (iterFind!=pManager->m_MapUpThread.end()) // �߳����ݻ�����
					{
						DWORD dExitCode = 0;
						CWinThread *pFinishedThread = pManager->m_MapUpThread[iter->second];

						GetExitCodeThread(pFinishedThread->m_hThread,&dExitCode);
						if (dExitCode != STILL_ACTIVE)
						{
							pManager->m_MapUpThread.erase(iterFind); //���߳�ӳ�����ɾ����ӳ��
							delete pFinishedThread; // �߳̽���
							iRuningCount--; // �������е��ϴ��߳������٣��˳����������߳�
						}
						else
						{
							iter++;
							Sleep(100);
							continue; // �̻߳�δ��ȫ�˳������������б���ɾ��
						}
					}

					delete iter->second;
					ZTools::WriteZToolsFormatLog("ɾ������Ϊ��%s�����ϴ�����ɹ����ϴ��߳��Ѿ���ȫ�˳�...",iter->first);
					pManager->m_UploadTaskMap.erase(iter++); //�����ȡ��������
				}
				else
					iter++;
			}

			while( pManager->m_UploadQueue.size()>0 && iRuningCount < pManager->m_MaxThreadCount )
			{
				pManager->StartOneTaskFromQueue();
			}
			LeaveCriticalSection(&g_UpCriticalSection); 
		}

		if (iRuningCount==0)
		{
			Sleep(100);
			continue;
		}

		EnterCriticalSection(&g_UpCriticalSection);
		map<CString,CallHprose*>::iterator iter = pManager->m_UploadTaskMap.begin();
		for (iter;iter!=pManager->m_UploadTaskMap.end();iter++)
		{
			if ( iter->second->m_TaskStatus == CallHprose::Upload_Finish 
				|| iter->second->m_TaskStatus == CallHprose::Upload_Error
				|| iter->second->m_TaskStatus== CallHprose::Upload_STOP 
				|| iter->second->m_TaskStatus== CallHprose::Upload_PAUSE)
			{
				//������ɣ��ɹ����߳�����ֹ��������ʱ��ɾ���̣߳����Ӷ�����ȡ���Ŷ�����ִ��
				map<CallHprose *, CWinThread *>::iterator iterFind = pManager->m_MapUpThread.find(iter->second);
				if (iterFind!=pManager->m_MapUpThread.end())
				{
					CWinThread *pFinishedThread = pManager->m_MapUpThread[iter->second];
					WaitForSingleObject(pFinishedThread->m_hThread,INFINITE); //�ȴ��߳��˳�����ɾ���̶߳��󣬷�������
					pManager->m_MapUpThread.erase(iterFind); //���߳�ӳ�����ɾ����ӳ��
					delete pFinishedThread; // �߳̽���
					iRuningCount--; // �������е��ϴ��߳������٣��˳����������߳�
				}
			}
		}
		while( pManager->m_UploadQueue.size()>0 && iRuningCount < pManager->m_MaxThreadCount )
		{
			pManager->StartOneTaskFromQueue();
		}
		LeaveCriticalSection(&g_UpCriticalSection); 
		Sleep(100);

	}
	return 0;
}
void CUploadManager::StartOneTaskFromQueue()
{
	// ȡ�������еĵ�һ������
	CString strTop = m_UploadQueue.front();
	map<CString,CallHprose*>::iterator iter = m_UploadTaskMap.find(strTop);
	if (iter==m_UploadTaskMap.end())
		return;

	StartOneUploadThread(iter->second);
	m_UploadQueue.pop(); // ɾ�������еĵ�һ��Ԫ��
}
void CUploadManager::DestoyrUploadProgressInfo()
{
	ZTools::WriteZToolsLog("����ϴ�������CUploadManager��ʷ��¼...");
	EnterCriticalSection(&g_UpCriticalSection); 
	map<CString,CallHprose *>::iterator iter = m_UploadTaskMap.begin();
	for (iter;iter!=m_UploadTaskMap.end();iter++)
		delete iter->second;
	// �˴�����ر��߳�
	m_UploadTaskMap.clear();
	LeaveCriticalSection(&g_UpCriticalSection); 
}
vector<CallHprose *> CUploadManager::GetUploadProgressInfo(CString strIndex,BOOL IsFile)
{
	vector<CallHprose *> vQueryResult;
	EnterCriticalSection(&g_UpCriticalSection); 
	if (IsFile) // �����ѯ�����ļ�
	{
		map<CString,CallHprose*>::iterator iter = m_UploadTaskMap.find(strIndex);
		if (iter!=m_UploadTaskMap.end())
			vQueryResult.push_back(m_UploadTaskMap[strIndex]);

		LeaveCriticalSection(&g_UpCriticalSection); 
		return vQueryResult;
	}

	//�����ѯ�����ļ���
	map<CString,CallHprose*>::iterator iter = m_UploadTaskMap.begin();
	for (iter;iter!=m_UploadTaskMap.end();iter++)
	{
		CString strKey = iter->first;
		if (strKey.Find(strIndex,0)==0) //�õ�������ǰ������Ҫ���ҵ�����ǰ�벿��ƥ��
			vQueryResult.push_back(iter->second);
	}
	LeaveCriticalSection(&g_UpCriticalSection); 
	return vQueryResult;
}
void CUploadManager::PauseUpload(CString strIndex,BOOL bIsFile)
{
	EnterCriticalSection(&g_UpCriticalSection); 
	if (bIsFile)
	{
		map<CString,CallHprose*>::iterator iter = m_UploadTaskMap.find(strIndex);
		if (iter==m_UploadTaskMap.end())
		{
			LeaveCriticalSection(&g_UpCriticalSection);
			return;
		}
		if ( iter->second->m_TaskStatus==CallHprose::Upload_Waiting 
			|| iter->second->m_TaskStatus==CallHprose::Upload_Runing)
		{
			iter->second->m_TaskStatus = CallHprose::Upload_PAUSE;
			ZTools::WriteZToolsFormatLog("CUploadManager::StopUpload �յ���ͣ����Ϊ��%s��������֪ͨ�����ڵȴ��̰߳�ȫ����...",iter->first);
		}
	
	}
	else
	{
		//�����ѯ�����ļ���
		map<CString,CallHprose*>::iterator iter = m_UploadTaskMap.begin();
		for (iter;iter!=m_UploadTaskMap.end();iter++)
		{
			CString strKey = iter->first;
			if (strKey.Find(strIndex,0)==0) //�õ�������ǰ������Ҫ���ҵ�����ǰ�벿��ƥ��
			{
				if ( iter->second->m_TaskStatus==CallHprose::Upload_Waiting 
					|| iter->second->m_TaskStatus==CallHprose::Upload_Runing)
				{
					iter->second->m_TaskStatus = CallHprose::Upload_PAUSE;
					ZTools::WriteZToolsFormatLog("CUploadManager::StopUpload �յ���ͣ����Ϊ��%s��������֪ͨ�����ڵȴ��̰߳�ȫ����...",iter->first);
				}
			}
		}
	}

	LeaveCriticalSection(&g_UpCriticalSection); 
}
void CUploadManager::ResumeUpload(CString strIndex,BOOL bIsFile)
{
	EnterCriticalSection(&g_UpCriticalSection); 
	if (bIsFile)
	{
		map<CString,CallHprose*>::iterator iter = m_UploadTaskMap.find(strIndex);
		if (iter==m_UploadTaskMap.end())
		{
			LeaveCriticalSection(&g_UpCriticalSection); 
			return; // δ�ҵ�������
		}
		if (iter->second->m_TaskStatus == CallHprose::Upload_STOP
			|| iter->second->m_TaskStatus == CallHprose::Upload_PAUSE
			|| iter->second->m_TaskStatus == CallHprose::Upload_Error)
		{
			map<CallHprose *,CWinThread *>::iterator iterThread = m_MapUpThread.find(iter->second);
			if (iterThread == m_MapUpThread.end())  // �ϴ��߳��Ѿ������ģ�����ȴ��������¿�ʼ������ֱ������״̬���ɼ���ִ��
			{
				iter->second->m_TaskStatus = CallHprose::Upload_Waiting;
				iter->second->dataFileObject.m_strMsg = "";
				iter->second->ResetErrFlag();
				m_UploadQueue.push(strIndex); //����ȴ����� �ȴ����¿�ʼִ��
			}
			else
				iter->second->m_TaskStatus = CallHprose::Upload_Runing;

		}
	}
	else
	{
		//�����ѯ�����ļ���
		map<CString,CallHprose*>::iterator iter = m_UploadTaskMap.begin();
		for (iter;iter!=m_UploadTaskMap.end();iter++)
		{
			CString strKey = iter->first;
			if (strKey.Find(strIndex,0)!=0) //�õ�������ǰ������Ҫ���ҵ�����ǰ�벿��ƥ��
				continue;

			if (iter->second->m_TaskStatus == CallHprose::Upload_STOP 
				|| iter->second->m_TaskStatus == CallHprose::Upload_PAUSE
				|| iter->second->m_TaskStatus == CallHprose::Upload_Error)
			{
				map<CallHprose *,CWinThread *>::iterator iterThread = m_MapUpThread.find(iter->second);
				if (iterThread==m_MapUpThread.end()) // �ϴ��߳��Ѿ������ģ�����ȴ��������¿�ʼ������ֱ������״̬���ɼ���ִ��
				{
					iter->second->m_TaskStatus = CallHprose::Upload_Waiting;
					iter->second->dataFileObject.m_strMsg = "";
					iter->second->ResetErrFlag();
					m_UploadQueue.push(strKey); //����ȴ����� �ȴ����¿�ʼִ��
				}
				else
					iter->second->m_TaskStatus = CallHprose::Upload_Runing;
			}
		}

	}

	LeaveCriticalSection(&g_UpCriticalSection); 
}
void CUploadManager::StopUpload(CString strIndex,BOOL bIsFile)
{
	EnterCriticalSection(&g_UpCriticalSection); 
	if (bIsFile)
	{
		map<CString,CallHprose*>::iterator iter = m_UploadTaskMap.find(strIndex);
		if (iter==m_UploadTaskMap.end())
		{
			LeaveCriticalSection(&g_UpCriticalSection);
			return;
		}
		if ( iter->second->m_TaskStatus==CallHprose::Upload_Waiting 
			|| iter->second->m_TaskStatus==CallHprose::Upload_Runing)
		{
			iter->second->m_TaskStatus = CallHprose::Upload_STOP;
			ZTools::WriteZToolsFormatLog("CUploadManager::StopUpload �յ�ֹͣ����Ϊ��%s��������֪ͨ�����ڵȴ��̰߳�ȫ����...",iter->first);
		}
	}
	else
	{
		//�����ѯ�����ļ���
		map<CString,CallHprose*>::iterator iter = m_UploadTaskMap.begin();
		for (iter;iter!=m_UploadTaskMap.end();iter++)
		{
			CString strKey = iter->first;
			if (strKey.Find(strIndex,0)==0) //�õ�������ǰ������Ҫ���ҵ�����ǰ�벿��ƥ��
			{
				if (  iter->second->m_TaskStatus==CallHprose::Upload_Waiting 
					|| iter->second->m_TaskStatus==CallHprose::Upload_Runing)
				{
					iter->second->m_TaskStatus = CallHprose::Upload_STOP;
					ZTools::WriteZToolsFormatLog("CUploadManager::StopUpload �յ�ֹͣ����Ϊ��%s��������֪ͨ�����ڵȴ��̰߳�ȫ����...",iter->first);
				}
			}
		}
	}

	LeaveCriticalSection(&g_UpCriticalSection); 
}
void CUploadManager::DeleteUpload(CString strIndex,BOOL bIsFile)
{
	EnterCriticalSection(&g_UpCriticalSection); 
	if (bIsFile)
	{
		map<CString,CallHprose*>::iterator iter = m_UploadTaskMap.find(strIndex);
		if (iter==m_UploadTaskMap.end())
		{
			LeaveCriticalSection(&g_UpCriticalSection);
			return;
		}
		iter->second->m_TaskStatus = CallHprose::Upload_DELET;
		//iter->second->deltask(iter->second->m_sIndex);
		CTaskData * pTask = thePersistMrg.GetTaskDataCopied(iter->second->m_DataPathObj.m_sIdPath.c_str());
		if (pTask)
		{
			pTask->m_bDelete = TRUE;
			thePersistMrg.DoOperation(pTask); //��xml��ɾ���ü�¼
			delete pTask;
			pTask = NULL;
		}

		ZTools::WriteZToolsFormatLog("CUploadManager::DeleteUpload �յ�ɾ������Ϊ��%s��������֪ͨ�����ڵȴ��̰߳�ȫ����...",iter->first);
	}
	else
	{
		//�����ѯ�����ļ���
		map<CString,CallHprose*>::iterator iter = m_UploadTaskMap.begin();
		for (iter;iter!=m_UploadTaskMap.end();iter++)
		{
			CString strKey = iter->first;
			if (strKey.Find(strIndex,0)==0) //�õ�������ǰ������Ҫ���ҵ�����ǰ�벿��ƥ��
			{
				iter->second->m_TaskStatus = CallHprose::Upload_DELET;
				//iter->second->deltask(iter->second->m_sIndex);
				CTaskData * pTask = thePersistMrg.GetTaskDataCopied(iter->second->m_DataPathObj.m_sIdPath.c_str());
				if (pTask)
				{
					pTask->m_bDelete = TRUE;
					thePersistMrg.DoOperation(pTask); //��xml��ɾ���ü�¼
					delete pTask;
					pTask = NULL;
				}
				ZTools::WriteZToolsFormatLog("CUploadManager::DeleteUpload �յ�ɾ������Ϊ��%s��������֪ͨ�����ڵȴ��̰߳�ȫ����...",iter->first);
			}
		}
	}

	LeaveCriticalSection(&g_UpCriticalSection); 
}
void CUploadManager::UniquePushWaitTask(CString strMapKey)
{
	queue<CString> queTmp;
	bool bHave = false;
	while(m_UploadQueue.size()>0)
	{
		CString str = m_UploadQueue.front();
		if (str == strMapKey)
			bHave = true;  // ��֤����Ԫ�ز��ظ����Ҳ��޸�ԭʼ����Ԫ��˳��

		queTmp.push(str); 
		m_UploadQueue.pop();
	}

	m_UploadQueue = queTmp;
	if (!bHave)
		m_UploadQueue.push(strMapKey); // ��ȴ�����������һ������

}