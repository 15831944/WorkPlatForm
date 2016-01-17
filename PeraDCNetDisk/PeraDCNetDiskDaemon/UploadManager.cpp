#include "stdafx.h"
#include "UploadManager.h"
#include "PeraNetDiskSetting.h"
#include "CPeraDataPersist.h"

CRITICAL_SECTION g_UpCriticalSection; // 定义临界区
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
	ZTools::WriteZToolsLog("上传管理器CUploadManager构造函数调用...");
	m_iCurrentRunningThreadCount = 0;
	GetSystemConfig();
	InitializeCriticalSection(&g_UpCriticalSection);
	StartManagerThread();
}
void CUploadManager::StartManagerThread()
{
	//在此启动一个监护进程，管理子线程的状态
	HANDLE hThread =CreateThread(NULL,0,UploadManagerThreadPro,NULL,0,NULL);

}
void CUploadManager::GetSystemConfig()
{
	m_MaxThreadCount = theSetting.GetUploadMaxTask();
}
CUploadManager::~CUploadManager()
{
	ZTools::WriteZToolsLog("上传管理器CUploadManager析构函数调用...");
	DestoyrUploadProgressInfo();
	DeleteCriticalSection(&g_UpCriticalSection); 
}
void CUploadManager::SetMaxThreadCount(int iThreadCount)
{
	EnterCriticalSection(&g_UpCriticalSection);  // 线程同步锁
	if (iThreadCount > 20 || iThreadCount < 1)
		iThreadCount = 5;

	m_MaxThreadCount = iThreadCount;
	LeaveCriticalSection(&g_UpCriticalSection); // 释放线程锁
}
void CUploadManager::LoadFinishedUploadTask(CString str ,const CUploadInputParam & param)
{
	ZTools::WriteZToolsFormatLog("CUploadManager::LoadFinishedUploadTask调用,索引值为:%s",(LPCTSTR)str);

	EnterCriticalSection(&g_UpCriticalSection); 
	map<CString,CallHprose *>::iterator  iterFind = m_UploadTaskMap.find(str);
	if (iterFind != m_UploadTaskMap.end())
		return; // 有重复的任务

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

	m_UploadTaskMap[str] = phprose; // 增加到任务记录表中

	LeaveCriticalSection(&g_UpCriticalSection); 
}
void CUploadManager::AddToManager(CString str,const CUploadInputParam & param)
{
	ZTools::WriteZToolsFormatLog("CUploadManager::AddToManager调用,索引值为:%s",(LPCTSTR)str);
	CallHprose *phprose = new CallHprose((LPCTSTR)param.m_strHproseRpc,
		(LPCTSTR)param.m_strFilePath,(LPCTSTR)param.m_strIndex);

	phprose->m_DataPathObj.m_sPath   = (LPCTSTR)param.m_strPath;
	phprose->m_DataPathObj.m_sIdPath = (LPCTSTR)param.m_strIdPath;
	phprose->m_bCreateIndexForFileInfo = param.m_bCreateIndex;

	ZTools::WriteZToolsFormatLog("CUploadManager::AddToManager开始获取线程锁...");
	EnterCriticalSection(&g_UpCriticalSection); 
	ZTools::WriteZToolsFormatLog("CUploadManager::AddToManager获取线程锁成功...");
	map<CString,CallHprose *>::iterator  iterFind = m_UploadTaskMap.find(str);
	if (iterFind == m_UploadTaskMap.end())
		m_UploadTaskMap[str] = phprose; // 增加到任务记录表中
	else //重复添加到任务管理器时，执行替换操作
	{
		//return; // 该任务正在管理器当中
		ZTools::WriteZToolsFormatLog("AddToManager索引值:%s重复，将覆盖旧任务",(LPCTSTR)str);
		iterFind->second->m_TaskStatus = CallHprose::Upload_DELET; //设置取消标志，使得线程安全结束
		map<CallHprose *,CWinThread *>::iterator iterThread = m_MapUpThread.find(iterFind->second);
		if (iterThread!=m_MapUpThread.end())
		{
			HANDLE hThread = iterThread->second->m_hThread;
			ZTools::WriteZToolsLog("等待旧任务线程安全结束...");
			WaitForSingleObject(hThread,INFINITE);
			DWORD dExitCode = 0;
			GetExitCodeThread(hThread,&dExitCode);
			if (dExitCode!=STILL_ACTIVE)
				ZTools::WriteZToolsLog("旧任务线程已安全结束...");

			delete iterThread->second;
			m_iCurrentRunningThreadCount--;
		}
		delete iterFind->second;
		m_UploadTaskMap.erase(iterFind);
		m_UploadTaskMap[str] = phprose; // 增加到任务记录表中
	}


	if (m_iCurrentRunningThreadCount >= m_MaxThreadCount) //已经达到线程上限
	{
		ZTools::WriteZToolsFormatLog("CUploadManager::AddToManager 已经达到线程上限,加入等候队列，释放线程锁...");
		UniquePushWaitTask(str);// 向等待队列中增加一个任务
		LeaveCriticalSection(&g_UpCriticalSection);  //记得解锁，否则管理线程将挂起
		return;
	}
	StartOneUploadThread(phprose); // 启动上传任务
	ZTools::WriteZToolsFormatLog("CUploadManager::AddToManager释放线程锁...");
	LeaveCriticalSection(&g_UpCriticalSection); 
	ZTools::WriteZToolsFormatLog("CUploadManager::AddToManager调用结束,索引值为:%s",(LPCTSTR)str);
}
void CUploadManager::StartOneUploadThread(CallHprose *pHpose)
{
	ZTools::WriteZToolsFormatLog("CUploadManager::StartOneUploadThread开始调用...");
	if(NULL==pHpose)
		return;

	m_iCurrentRunningThreadCount++;
	m_MapUpThread[pHpose] = AfxBeginThread(CUploadManager::UploadThreadPro, pHpose, 0, 0, 0, 0);
	m_MapUpThread[pHpose]->m_bAutoDelete = FALSE;
	ZTools::WriteZToolsFormatLog("CUploadManager::StartOneUploadThread调用结束，上传线程ID为:0x%x ...",m_MapUpThread[pHpose]->m_nThreadID);
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
		thePersistMrg.DoOperation(pTask); //更新xml中记录状态
		delete pTask;
		pTask = NULL;
	}

	phprose->UploadDefault(); //在该上传线程函数中调用上传
	pTask = thePersistMrg.GetTaskDataCopied(phprose->m_DataPathObj.m_sIdPath.c_str());
	if (pTask)
	{
		pTask->m_dwStatus = phprose->m_TaskStatus;
		pTask->m_sFileId   = phprose->dataFileObject.m_strFileId.c_str();
		pTask->m_sTotalSize = phprose->m_strFileSize.c_str();
		pTask->m_sFileMd5  = phprose->dataFileObject.m_strMd5.c_str();
		pTask->m_sUrl = phprose->dataFileObject.m_strHproseRpc.c_str();

		thePersistMrg.DoOperation(pTask); //更新xml中记录状态
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
			//找出完成的，且取消的任务，将其从任务列表中删除
			EnterCriticalSection(&g_UpCriticalSection);
			if (pManager->m_UploadQueue.size()>0)
				pManager->DeleteCanceledFromQueue();

			map<CString,CallHprose*>::iterator iter = pManager->m_UploadTaskMap.begin();
			for (iter;iter!=pManager->m_UploadTaskMap.end();)
			{
				// 在此处理delete,保证无任务运行的时候，也能从历史记录里删除
				if (iter->second->m_TaskStatus== CallHprose::Upload_DELET)
				{
					map<CallHprose *, CWinThread *>::iterator iterFind = pManager->m_MapUpThread.find(iter->second);
					if (iterFind!=pManager->m_MapUpThread.end()) // 线程数据还存在
					{
						DWORD dExitCode = 0;
						CWinThread *pFinishedThread = pManager->m_MapUpThread[iter->second];

						GetExitCodeThread(pFinishedThread->m_hThread,&dExitCode);
						if (dExitCode != STILL_ACTIVE)
						{
							pManager->m_MapUpThread.erase(iterFind); //从线程映射表中删除该映射
							delete pFinishedThread; // 线程结束
							iRuningCount--; // 正在运行的上传线程数减少，退出完成任务的线程
						}
						else
						{
							iter++;
							Sleep(100);
							continue; // 线程还未安全退出，不从任务列表中删除
						}
					}

					delete iter->second;
					ZTools::WriteZToolsFormatLog("删除索引为【%s】的上传任务成功，上传线程已经安全退出...",iter->first);
					pManager->m_UploadTaskMap.erase(iter++); //清空已取消的任务
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
				//当有完成（成功或者出错终止）的任务时，删除线程，并从队列中取出排队任务执行
				map<CallHprose *, CWinThread *>::iterator iterFind = pManager->m_MapUpThread.find(iter->second);
				if (iterFind!=pManager->m_MapUpThread.end())
				{
					CWinThread *pFinishedThread = pManager->m_MapUpThread[iter->second];
					WaitForSingleObject(pFinishedThread->m_hThread,INFINITE); //等待线程退出后再删除线程对象，否则会崩溃
					pManager->m_MapUpThread.erase(iterFind); //从线程映射表中删除该映射
					delete pFinishedThread; // 线程结束
					iRuningCount--; // 正在运行的上传线程数减少，退出完成任务的线程
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
	// 取出队列中的第一个任务
	CString strTop = m_UploadQueue.front();
	map<CString,CallHprose*>::iterator iter = m_UploadTaskMap.find(strTop);
	if (iter==m_UploadTaskMap.end())
		return;

	StartOneUploadThread(iter->second);
	m_UploadQueue.pop(); // 删除队列中的第一个元素
}
void CUploadManager::DestoyrUploadProgressInfo()
{
	ZTools::WriteZToolsLog("清空上传管理器CUploadManager历史记录...");
	EnterCriticalSection(&g_UpCriticalSection); 
	map<CString,CallHprose *>::iterator iter = m_UploadTaskMap.begin();
	for (iter;iter!=m_UploadTaskMap.end();iter++)
		delete iter->second;
	// 此处还需关闭线程
	m_UploadTaskMap.clear();
	LeaveCriticalSection(&g_UpCriticalSection); 
}
vector<CallHprose *> CUploadManager::GetUploadProgressInfo(CString strIndex,BOOL IsFile)
{
	vector<CallHprose *> vQueryResult;
	EnterCriticalSection(&g_UpCriticalSection); 
	if (IsFile) // 如果查询的是文件
	{
		map<CString,CallHprose*>::iterator iter = m_UploadTaskMap.find(strIndex);
		if (iter!=m_UploadTaskMap.end())
			vQueryResult.push_back(m_UploadTaskMap[strIndex]);

		LeaveCriticalSection(&g_UpCriticalSection); 
		return vQueryResult;
	}

	//如果查询的是文件夹
	map<CString,CallHprose*>::iterator iter = m_UploadTaskMap.begin();
	for (iter;iter!=m_UploadTaskMap.end();iter++)
	{
		CString strKey = iter->first;
		if (strKey.Find(strIndex,0)==0) //该迭代器当前索引与要查找的索引前半部分匹配
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
			ZTools::WriteZToolsFormatLog("CUploadManager::StopUpload 收到暂停索引为【%s】的任务通知，正在等待线程安全结束...",iter->first);
		}
	
	}
	else
	{
		//如果查询的是文件夹
		map<CString,CallHprose*>::iterator iter = m_UploadTaskMap.begin();
		for (iter;iter!=m_UploadTaskMap.end();iter++)
		{
			CString strKey = iter->first;
			if (strKey.Find(strIndex,0)==0) //该迭代器当前索引与要查找的索引前半部分匹配
			{
				if ( iter->second->m_TaskStatus==CallHprose::Upload_Waiting 
					|| iter->second->m_TaskStatus==CallHprose::Upload_Runing)
				{
					iter->second->m_TaskStatus = CallHprose::Upload_PAUSE;
					ZTools::WriteZToolsFormatLog("CUploadManager::StopUpload 收到暂停索引为【%s】的任务通知，正在等待线程安全结束...",iter->first);
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
			return; // 未找到该任务
		}
		if (iter->second->m_TaskStatus == CallHprose::Upload_STOP
			|| iter->second->m_TaskStatus == CallHprose::Upload_PAUSE
			|| iter->second->m_TaskStatus == CallHprose::Upload_Error)
		{
			map<CallHprose *,CWinThread *>::iterator iterThread = m_MapUpThread.find(iter->second);
			if (iterThread == m_MapUpThread.end())  // 上传线程已经结束的，加入等待队列重新开始，否则直接重置状态即可继续执行
			{
				iter->second->m_TaskStatus = CallHprose::Upload_Waiting;
				iter->second->dataFileObject.m_strMsg = "";
				iter->second->ResetErrFlag();
				m_UploadQueue.push(strIndex); //加入等待队列 等待重新开始执行
			}
			else
				iter->second->m_TaskStatus = CallHprose::Upload_Runing;

		}
	}
	else
	{
		//如果查询的是文件夹
		map<CString,CallHprose*>::iterator iter = m_UploadTaskMap.begin();
		for (iter;iter!=m_UploadTaskMap.end();iter++)
		{
			CString strKey = iter->first;
			if (strKey.Find(strIndex,0)!=0) //该迭代器当前索引与要查找的索引前半部分匹配
				continue;

			if (iter->second->m_TaskStatus == CallHprose::Upload_STOP 
				|| iter->second->m_TaskStatus == CallHprose::Upload_PAUSE
				|| iter->second->m_TaskStatus == CallHprose::Upload_Error)
			{
				map<CallHprose *,CWinThread *>::iterator iterThread = m_MapUpThread.find(iter->second);
				if (iterThread==m_MapUpThread.end()) // 上传线程已经结束的，加入等待队列重新开始，否则直接重置状态即可继续执行
				{
					iter->second->m_TaskStatus = CallHprose::Upload_Waiting;
					iter->second->dataFileObject.m_strMsg = "";
					iter->second->ResetErrFlag();
					m_UploadQueue.push(strKey); //加入等待队列 等待重新开始执行
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
			ZTools::WriteZToolsFormatLog("CUploadManager::StopUpload 收到停止索引为【%s】的任务通知，正在等待线程安全结束...",iter->first);
		}
	}
	else
	{
		//如果查询的是文件夹
		map<CString,CallHprose*>::iterator iter = m_UploadTaskMap.begin();
		for (iter;iter!=m_UploadTaskMap.end();iter++)
		{
			CString strKey = iter->first;
			if (strKey.Find(strIndex,0)==0) //该迭代器当前索引与要查找的索引前半部分匹配
			{
				if (  iter->second->m_TaskStatus==CallHprose::Upload_Waiting 
					|| iter->second->m_TaskStatus==CallHprose::Upload_Runing)
				{
					iter->second->m_TaskStatus = CallHprose::Upload_STOP;
					ZTools::WriteZToolsFormatLog("CUploadManager::StopUpload 收到停止索引为【%s】的任务通知，正在等待线程安全结束...",iter->first);
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
			thePersistMrg.DoOperation(pTask); //从xml中删除该记录
			delete pTask;
			pTask = NULL;
		}

		ZTools::WriteZToolsFormatLog("CUploadManager::DeleteUpload 收到删除索引为【%s】的任务通知，正在等待线程安全结束...",iter->first);
	}
	else
	{
		//如果查询的是文件夹
		map<CString,CallHprose*>::iterator iter = m_UploadTaskMap.begin();
		for (iter;iter!=m_UploadTaskMap.end();iter++)
		{
			CString strKey = iter->first;
			if (strKey.Find(strIndex,0)==0) //该迭代器当前索引与要查找的索引前半部分匹配
			{
				iter->second->m_TaskStatus = CallHprose::Upload_DELET;
				//iter->second->deltask(iter->second->m_sIndex);
				CTaskData * pTask = thePersistMrg.GetTaskDataCopied(iter->second->m_DataPathObj.m_sIdPath.c_str());
				if (pTask)
				{
					pTask->m_bDelete = TRUE;
					thePersistMrg.DoOperation(pTask); //从xml中删除该记录
					delete pTask;
					pTask = NULL;
				}
				ZTools::WriteZToolsFormatLog("CUploadManager::DeleteUpload 收到删除索引为【%s】的任务通知，正在等待线程安全结束...",iter->first);
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
			bHave = true;  // 保证队列元素不重复，且不修改原始队列元素顺序

		queTmp.push(str); 
		m_UploadQueue.pop();
	}

	m_UploadQueue = queTmp;
	if (!bHave)
		m_UploadQueue.push(strMapKey); // 向等待队列中增加一个任务

}