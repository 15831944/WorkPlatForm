#include "stdafx.h"
#include "CopyManager.h"
#include "FileCopy.h"
#include "Tools.h"
#include "CPeraDataPersist.h"
extern LONGLONG FileSize(CString strFile);
CCopyManager * CCopyManager::m_PManagetr = NULL;
CCopyManager::GabageCollector CCopyManager::m_gc;

CRITICAL_SECTION g_cs; // �����ٽ���

CCopyManager * CCopyManager::Instance()
{
	if (m_PManagetr == NULL)
		m_PManagetr = new CCopyManager;

	return m_PManagetr;
}
void CCopyManager::StartOneCopyThread(CFileCopy *pCp)
{
	if(NULL==pCp)
		return;

	m_iCurrentRunningThreadCount++;
	m_MapCopyThread[pCp] = AfxBeginThread(CCopyManager::CopyThreadPro,pCp, 0, 0, 0, 0);
	m_MapCopyThread[pCp]->m_bAutoDelete = FALSE;
}
UINT CCopyManager::CopyThreadPro(LPVOID lpParamter)
{
	CFileCopy *pCp = (CFileCopy*)lpParamter;
	if (NULL==pCp)
		return 1;

	CTaskData * pTask = thePersistMrg.GetTaskDataCopied(pCp->m_sIdPath);
	if (pTask)
	{
		pTask->m_dwStatus = CFileCopy::COPY_RUNNING;
		thePersistMrg.DoOperation(pTask); //����xml�м�¼״̬
		delete pTask;
		pTask = NULL;
	}

	pCp->m_CopyState = CFileCopy::COPY_RUNNING;
	pCp->ExcuteCopy(false); //�ڸÿ����̺߳����е��ÿ���
	
	if (pCp->m_CtrlState == CFileCopy::COPY_RUNNING)
		pCp->m_CopyState = CFileCopy::COPY_FINISH;
	else
		pCp->m_CopyState = pCp->m_CtrlState;

	pTask = thePersistMrg.GetTaskDataCopied(pCp->m_sIdPath);
	if (pTask)
	{
		pTask->m_dwStatus = pCp->m_CopyState;
		thePersistMrg.DoOperation(pTask); //����xml�м�¼״̬
		delete pTask;
		pTask = NULL;
	}
	return 0;
}
void CCopyManager::LoadFinishedCopyTask(CString strSrc ,CString strDest,CString strMapKey,CString strIdPath,CString strNetDiskPath)
{
	ZTools::WriteZToolsFormatLog("��������ɵ���ʷ��������%s,%s,%s,%s",strSrc,strMapKey,strIdPath,strNetDiskPath);
	EnterCriticalSection(&g_cs); 
	map<CString,CFileCopy *>::iterator  iterFind = g_CopyProgressInfo.find(strMapKey);
	if (iterFind == g_CopyProgressInfo.end())
	{
		LONGLONG iFileSize	= FileSize(strSrc);
		if (iFileSize < 0 )
		{
			ZTools::WriteZToolsFormatLog("��ȡ�ļ�[%s]��Сʧ��,FileSize����%lld",strSrc,iFileSize);
			LeaveCriticalSection(&g_cs); 
			return;
		}
		CFileCopy *pcp = new CFileCopy((LPCTSTR)strSrc,strDest);
		pcp->m_sIdPath = strIdPath;
		pcp->m_sNetDiskPath = strNetDiskPath;
		pcp->m_CopyState = CFileCopy::COPY_FINISH;
		pcp->m_iCurrentBytes = pcp->m_iTotalBytes = iFileSize;
		g_CopyProgressInfo[strMapKey] = pcp; // ���ӵ�������ʷ��¼��
	}
	LeaveCriticalSection(&g_cs); 
}
void CCopyManager::AddToManager(CString strSrc ,CString strDest,CString strMapKey,CString strIdPath,CString strNetDiskPath)
{
	ZTools::WriteZToolsFormatLog("CCopyManager::AddToManager��ʼ����...");
	EnterCriticalSection(&g_cs); 
	CFileCopy *pcp = new CFileCopy((LPCTSTR)strSrc,(LPCTSTR)strDest);
	pcp->m_sIdPath = strIdPath;
	pcp->m_sNetDiskPath = strNetDiskPath;
	//if (PathFileExists(strDest))
	//{
	//	pcp->m_CtrlState = CFileCopy::COPY_RESUM; //�ٴ�ִ��

	//	struct stat buf;
	//	int result = stat(strDest,&buf);
	//	if (result==0)
	//		pcp->m_iCurrentBytes = buf.st_size;	
	//}

	map<CString,CFileCopy *>::iterator  iterFind = g_CopyProgressInfo.find(strMapKey);
	if (iterFind == g_CopyProgressInfo.end())
		g_CopyProgressInfo[strMapKey] = pcp; // ���ӵ�������ʷ��¼��

	else //�ظ���ӵ����������ʱ��ִ���滻����
	{
		//return; // ���������ڹ���������
		iterFind->second->m_CtrlState = CFileCopy::COPY_DELET; // ����ɾ������
		map<CFileCopy *,CWinThread *>::iterator iterThread = m_MapCopyThread.find(iterFind->second);
		if (iterThread!=m_MapCopyThread.end())
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
		g_CopyProgressInfo.erase(iterFind);
		g_CopyProgressInfo[strMapKey] = pcp; // ���ӵ������¼����
	}


	if (m_iCurrentRunningThreadCount >= m_MaxThreadCount) //�Ѿ��ﵽ�߳�����
	{
		UniquePushWaitTask(strMapKey);
		LeaveCriticalSection(&g_cs);  //�ǵý�������������߳̽�����
		ZTools::WriteZToolsFormatLog("CCopyManager::AddToManager �Ѿ��ﵽ�߳�����,���ý���...");
		return;
	}

	StartOneCopyThread(pcp);
	//ZTools::WriteZToolsFormatLog("CCopyManager::AddToManager���ý���...");
	LeaveCriticalSection(&g_cs); 
}
void CCopyManager::UniquePushWaitTask(CString strMapKey)
{
	queue<CString> queTmp;
	bool bHave = false;
	while(m_CopyQueue.size()>0)
	{
		CString str = m_CopyQueue.front();
		if (str == strMapKey)
		    bHave = true;  // ��֤����Ԫ�ز��ظ����Ҳ��޸�ԭʼ����Ԫ��˳��

		queTmp.push(str); 

		m_CopyQueue.pop();
	}

	m_CopyQueue = queTmp;
	if (!bHave)
	   m_CopyQueue.push(strMapKey); // ��ȴ�����������һ������

}
void CCopyManager::DestoyrCopyProgressInfo()
{
	ZTools::WriteZToolsLog("��տ�����������ʷ��¼...");
	EnterCriticalSection(&g_cs); 
	map<CString,CFileCopy *>::iterator iter = g_CopyProgressInfo.begin();
	for (iter;iter!=g_CopyProgressInfo.end();iter++)
	{
		delete iter->second;
	}

	g_CopyProgressInfo.clear();
	LeaveCriticalSection(&g_cs); 
}
vector<CFileCopy *> CCopyManager::GetCopyProgressInfo(CString strIndex,BOOL IsFile)
{
	vector<CFileCopy *> vQueryResult;
	if (IsFile)
	{
		map<CString,CFileCopy*>::iterator iter = g_CopyProgressInfo.find(strIndex);
		if (iter==g_CopyProgressInfo.end())
			return vQueryResult;

		vQueryResult.push_back(g_CopyProgressInfo[strIndex]);
		return vQueryResult;
	}

	//�����ѯ�����ļ���
	map<CString,CFileCopy*>::iterator iter = g_CopyProgressInfo.begin();
	for (iter;iter!=g_CopyProgressInfo.end();iter++)
	{
		CString strKey = iter->first;
		if (strKey.Find(strIndex,0)==0) //�õ�������ǰ������Ҫ���ҵ�����ǰ�벿��ƥ��
		{
			vQueryResult.push_back(iter->second);
		}
	}

	return vQueryResult;
}
void CCopyManager::DeleteCopy(CString strIndex,BOOL bIsFile)
{
	EnterCriticalSection(&g_cs); 
	if (bIsFile)
	{
		map<CString,CFileCopy*>::iterator iter = g_CopyProgressInfo.find(strIndex);
		if (iter==g_CopyProgressInfo.end())
		{
			LeaveCriticalSection(&g_cs); 
			return;
		}
		g_CopyProgressInfo[strIndex]->m_CtrlState = CFileCopy::COPY_DELET;
		CTaskData * pTask = thePersistMrg.GetTaskDataCopied(iter->second->m_sIdPath);
		if (pTask)
		{
			pTask->m_bDelete = TRUE;
			thePersistMrg.DoOperation(pTask); //��xml��ɾ���ü�¼
			delete pTask;
			pTask = NULL;
		}
		ZTools::WriteZToolsFormatLog("CCopyManager::DeleteCopy �յ�ɾ������Ϊ��%s��������֪ͨ�����ڵȴ��̰߳�ȫ����...",iter->first);
	}
	else
	{
		//�����ѯ�����ļ���
		map<CString,CFileCopy*>::iterator iter = g_CopyProgressInfo.begin();
		for (iter;iter!=g_CopyProgressInfo.end();iter++)
		{
			CString strKey = iter->first;
			if (strKey.Find(strIndex,0)==0) //�õ�������ǰ������Ҫ���ҵ�����ǰ�벿��ƥ��
			{
				iter->second->m_CtrlState = CFileCopy::COPY_DELET;
				CTaskData * pTask = thePersistMrg.GetTaskDataCopied(iter->second->m_sIdPath);
				if (pTask)
				{
					pTask->m_bDelete = TRUE;
					thePersistMrg.DoOperation(pTask); //��xml��ɾ���ü�¼
					delete pTask;
					pTask = NULL;
				}
				ZTools::WriteZToolsFormatLog("CCopyManager::DeleteCopy �յ�ɾ������Ϊ��%s��������֪ͨ�����ڵȴ��̰߳�ȫ����...",iter->first);
			}
		}

	}
	
	LeaveCriticalSection(&g_cs); 
}
void CCopyManager::StopCopy(CString strIndex,BOOL bIsFile)
{
	EnterCriticalSection(&g_cs); 
	if (bIsFile)
	{
		map<CString,CFileCopy*>::iterator iter = g_CopyProgressInfo.find(strIndex);
		if (iter==g_CopyProgressInfo.end())
		{
			LeaveCriticalSection(&g_cs); 
			return;
		}
		g_CopyProgressInfo[strIndex]->m_CtrlState = CFileCopy::COPY_STOP;
		ZTools::WriteZToolsFormatLog("CCopyManager::StopCopy �յ�ֹͣ����Ϊ��%s��������֪ͨ�����ڵȴ��̰߳�ȫ����...",iter->first);
	}
	else
	{
		//�����ѯ�����ļ���
		map<CString,CFileCopy*>::iterator iter = g_CopyProgressInfo.begin();
		for (iter;iter!=g_CopyProgressInfo.end();iter++)
		{
			CString strKey = iter->first;
			if (strKey.Find(strIndex,0)==0) //�õ�������ǰ������Ҫ���ҵ�����ǰ�벿��ƥ��
			{
				iter->second->m_CtrlState = CFileCopy::COPY_STOP;
				ZTools::WriteZToolsFormatLog("CCopyManager::StopCopy �յ�ֹͣ����Ϊ��%s��������֪ͨ�����ڵȴ��̰߳�ȫ����...",iter->first);
			}
		}

	}

	LeaveCriticalSection(&g_cs); 
}
void CCopyManager::PauseCopy(CString strIndex,BOOL bIsFile)
{
	EnterCriticalSection(&g_cs); 
	if (bIsFile)
	{
		map<CString,CFileCopy*>::iterator iter = g_CopyProgressInfo.find(strIndex);
		if (iter==g_CopyProgressInfo.end())
		{
			LeaveCriticalSection(&g_cs); 
			return;
		}
		g_CopyProgressInfo[strIndex]->m_CtrlState = CFileCopy::COPY_PAUSE;
		ZTools::WriteZToolsFormatLog("CCopyManager::SuspendCopy �յ���ͣ����Ϊ��%s��������֪ͨ�����ڵȴ��̰߳�ȫ����...",iter->first);
	}
	else
	{
		//�����ѯ�����ļ���
		map<CString,CFileCopy*>::iterator iter = g_CopyProgressInfo.begin();
		for (iter;iter!=g_CopyProgressInfo.end();iter++)
		{
			CString strKey = iter->first;
			if (strKey.Find(strIndex,0)==0) //�õ�������ǰ������Ҫ���ҵ�����ǰ�벿��ƥ��
			{
				iter->second->m_CtrlState = CFileCopy::COPY_PAUSE;
				ZTools::WriteZToolsFormatLog("CCopyManager::SuspendCopy �յ���ͣ����Ϊ��%s��������֪ͨ�����ڵȴ��̰߳�ȫ����...",iter->first);
			}
		}

	}

	LeaveCriticalSection(&g_cs); 
}
void CCopyManager::ResumeCopy(CString strIndex,BOOL bIsFile)
{
	EnterCriticalSection(&g_cs);
	if (bIsFile)
	{
		map<CString,CFileCopy*>::iterator iter = g_CopyProgressInfo.find(strIndex);
		if (iter==g_CopyProgressInfo.end())
		{
			LeaveCriticalSection(&g_cs); 
			return;
		}
		if (iter->second->m_CtrlState == CFileCopy::COPY_STOP || iter->second->m_CtrlState == CFileCopy::COPY_PAUSE)
		{
			ZTools::WriteZToolsFormatLog("����Ϊ��%s��������״̬�Ѿ�����,������ִ��",strIndex);
			iter->second->m_CopyState = CFileCopy::COPY_WAIT_SATRT;
			iter->second->m_CtrlState = CFileCopy::COPY_RESUME; 
			iter->second->m_strErrMsg = "";
			m_CopyQueue.push(strIndex); //����ȴ����� �ȴ����¿�ʼִ��
		}
	}
	else
	{
		//�����ѯ�����ļ���
		map<CString,CFileCopy*>::iterator iter = g_CopyProgressInfo.begin();
		for (iter;iter!=g_CopyProgressInfo.end();iter++)
		{
			CString strKey = iter->first;
			if (strKey.Find(strIndex,0)==0) //�õ�������ǰ������Ҫ���ҵ�����ǰ�벿��ƥ��
			{
				if (iter->second->m_CtrlState == CFileCopy::COPY_STOP || iter->second->m_CtrlState == CFileCopy::COPY_PAUSE)
				{
					ZTools::WriteZToolsFormatLog("����Ϊ��%s��������״̬�Ѿ�����,������ִ��",strKey);
					iter->second->m_strErrMsg = "";
					iter->second->m_CopyState = CFileCopy::COPY_WAIT_SATRT;
					iter->second->m_CtrlState = CFileCopy::COPY_RESUME; 
					m_CopyQueue.push(strKey); //����ȴ����� �ȴ����¿�ʼִ��
				}
			}
		}

	}
	
	LeaveCriticalSection(&g_cs);
}
void CCopyManager::SetMaxThreadCount(int iThreadCount)
{
	EnterCriticalSection(&g_cs);
	if (iThreadCount > 10 || iThreadCount < 1)
		iThreadCount = 5;

	m_MaxThreadCount = iThreadCount;
	LeaveCriticalSection(&g_cs); 
}
void CCopyManager::StartManagerThread()
{
	//�ڴ�����һ���໤���̣��������̵߳�״̬
	HANDLE hThread =CreateThread(NULL,0,CopyManagerThreadPro,NULL,0,NULL);
}
CCopyManager::CCopyManager()
{	
	ZTools::WriteZToolsLog("����������CCopyManager���캯������...");
	m_iCurrentRunningThreadCount = 0;
	m_MaxThreadCount = 1; // �����߳��޶�Ϊ1��
	/// ��ʹ���ٽ���ǰ��������г�ʼ��    
	InitializeCriticalSection(&g_cs);
	StartManagerThread();
}
CCopyManager::~CCopyManager ()
{
	/// ʹ�������ʾɾ���Ա�����Դй© 
	ZTools::WriteZToolsLog("����������CCopyManager������������...");
	DestoyrCopyProgressInfo();
	DeleteCriticalSection(&g_cs); 
}
void CCopyManager::StartOneTaskFromQueue()
{
	// ȡ�������еĵ�һ������
	CString strTop = m_CopyQueue.front();
	map<CString,CFileCopy*>::iterator iter = g_CopyProgressInfo.find(strTop);
	if (iter==g_CopyProgressInfo.end())
		return;

	StartOneCopyThread(iter->second);
	m_CopyQueue.pop(); // ɾ�������еĵ�һ��Ԫ��
}
void CCopyManager::DeleteCanceledFromQueue()
{
	queue<CString> queTmp;
	map<CString,CFileCopy*>::iterator iter;
	while(m_CopyQueue.size()>0)
	{
		CString str = m_CopyQueue.front();
		iter = g_CopyProgressInfo.find(str);
		if (iter!=g_CopyProgressInfo.end())
		{
			if (iter->second->m_CtrlState!=CFileCopy::COPY_DELET) //��������ɾ��������
			{
				queTmp.push(str);
			}
		}
		m_CopyQueue.pop();
	}

	m_CopyQueue = queTmp;

}
DWORD WINAPI CCopyManager::CopyManagerThreadPro(LPVOID lpParamter)
{
	HANDLE *pHandArry = NULL;
	CCopyManager *pManager = CCopyManager::Instance();
	while(1)
	{
		int & iRuningCount = pManager->m_iCurrentRunningThreadCount;
		if (!pManager->g_CopyProgressInfo.empty())
		{
			//�ҳ���ɵģ���ȡ�������񣬽���������б���ɾ��
			EnterCriticalSection(&g_cs);
			if ( pManager->m_CopyQueue.size()>0)
				pManager->DeleteCanceledFromQueue();
			
			map<CString,CFileCopy*>::iterator iter = pManager->g_CopyProgressInfo.begin();
			for (iter;iter!=pManager->g_CopyProgressInfo.end();)
			{
				// ɾ������Ҫ����̺߳���ʷ��¼��ֹͣ�Ĺر��̣߳���ɾ����ʷ��¼
				if (iter->second->m_CtrlState == CFileCopy::COPY_DELET || iter->second->m_CtrlState == CFileCopy::COPY_STOP)
				{
					map<CFileCopy *, CWinThread *>::iterator iterFind = pManager->m_MapCopyThread.find(iter->second);
					if (iterFind!=pManager->m_MapCopyThread.end()) // �߳����ݻ�����
					{
						DWORD dExitCode = 0;
						CWinThread *pFinishedThread = pManager->m_MapCopyThread[iter->second];
						GetExitCodeThread(pFinishedThread->m_hThread,&dExitCode);

						if (dExitCode != STILL_ACTIVE)
						{
							pManager->m_MapCopyThread.erase(iterFind); //���߳�ӳ�����ɾ����ӳ��
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
					if (iter->second->m_CtrlState == CFileCopy::COPY_DELET)
					{
						delete iter->second;
						ZTools::WriteZToolsFormatLog("ɾ������Ϊ��%s���Ŀ�������ɹ��������߳��Ѿ���ȫ�˳�...",iter->first);
						pManager->g_CopyProgressInfo.erase(iter++); //�����ȡ��������
					}
					else
					{
						iter++;
						ZTools::WriteZToolsFormatLog("ֹͣ����Ϊ��%s���Ŀ�������ɹ��������߳��Ѿ���ȫ�˳�...",iter->first);
					}
				}
				else
					iter++;
			}

			while(pManager->m_CopyQueue.size()>0 && iRuningCount < pManager->m_MaxThreadCount)
			{
				pManager->StartOneTaskFromQueue();
			}

			LeaveCriticalSection(&g_cs); 
		}
		if (iRuningCount==0)
		{
			Sleep(100);
			continue;
		}

	/*	pManager->GetRunningThreads(&pHandArry);
		WaitForMultipleObjects(pManager->m_MapCopyThread.size(),pHandArry,FALSE,INFINITE);
		delete[] pHandArry;
		pHandArry = NULL;*/

		EnterCriticalSection(&g_cs); 
		map<CString,CFileCopy*>::iterator iter = pManager->g_CopyProgressInfo.begin();
		for (iter;iter!=pManager->g_CopyProgressInfo.end();iter++)
		{
			if ( iter->second->m_CopyState == CFileCopy::COPY_FINISH 
				|| iter->second->m_CopyState == CFileCopy::COPY_DELET
				|| iter->second->m_CopyState == CFileCopy::COPY_STOP) // �����߳��Ѿ�����
			{
				map<CFileCopy *, CWinThread *>::iterator iterFind = pManager->m_MapCopyThread.find(iter->second);
				if (iterFind!=pManager->m_MapCopyThread.end())
				{
					CWinThread *pFinishedThread = pManager->m_MapCopyThread[iter->second];
					WaitForSingleObject(pFinishedThread->m_hThread,INFINITE); //�ȴ��߳��˳�����ɾ���̶߳��󣬷�������
					pManager->m_MapCopyThread.erase(iterFind); //���߳�ӳ�����ɾ����ӳ��
					delete pFinishedThread; // �߳̽���
					iRuningCount--; // �������е��ϴ��߳������٣��˳����������߳�
				}

			}

		}
		while(pManager->m_CopyQueue.size()>0 && iRuningCount < pManager->m_MaxThreadCount)
		{
			pManager->StartOneTaskFromQueue();
		}
		LeaveCriticalSection(&g_cs);
	}

	return 0;
}
void CCopyManager::GetRunningThreads(HANDLE **ppHandleArry)
{
	EnterCriticalSection(&g_cs);
	*ppHandleArry = new HANDLE [m_MapCopyThread.size()];
	map<CFileCopy *, CWinThread *>::iterator iter = m_MapCopyThread.begin();
	int iIndex = 0;
	for (iter;iter!=m_MapCopyThread.end();iter++)
	{
		(*ppHandleArry)[iIndex++]  = iter->second->m_hThread;
	}
	LeaveCriticalSection(&g_cs);
}