#include "stdafx.h"
#include "CopyManager.h"
#include "FileCopy.h"
#include "Tools.h"
#include "CPeraDataPersist.h"
extern LONGLONG FileSize(CString strFile);
CCopyManager * CCopyManager::m_PManagetr = NULL;
CCopyManager::GabageCollector CCopyManager::m_gc;

CRITICAL_SECTION g_cs; // 定义临界区

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
		thePersistMrg.DoOperation(pTask); //更新xml中记录状态
		delete pTask;
		pTask = NULL;
	}

	pCp->m_CopyState = CFileCopy::COPY_RUNNING;
	pCp->ExcuteCopy(false); //在该拷贝线程函数中调用拷贝
	
	if (pCp->m_CtrlState == CFileCopy::COPY_RUNNING)
		pCp->m_CopyState = CFileCopy::COPY_FINISH;
	else
		pCp->m_CopyState = pCp->m_CtrlState;

	pTask = thePersistMrg.GetTaskDataCopied(pCp->m_sIdPath);
	if (pTask)
	{
		pTask->m_dwStatus = pCp->m_CopyState;
		thePersistMrg.DoOperation(pTask); //更新xml中记录状态
		delete pTask;
		pTask = NULL;
	}
	return 0;
}
void CCopyManager::LoadFinishedCopyTask(CString strSrc ,CString strDest,CString strMapKey,CString strIdPath,CString strNetDiskPath)
{
	ZTools::WriteZToolsFormatLog("加载已完成的历史拷贝任务：%s,%s,%s,%s",strSrc,strMapKey,strIdPath,strNetDiskPath);
	EnterCriticalSection(&g_cs); 
	map<CString,CFileCopy *>::iterator  iterFind = g_CopyProgressInfo.find(strMapKey);
	if (iterFind == g_CopyProgressInfo.end())
	{
		LONGLONG iFileSize	= FileSize(strSrc);
		if (iFileSize < 0 )
		{
			ZTools::WriteZToolsFormatLog("获取文件[%s]大小失败,FileSize返回%lld",strSrc,iFileSize);
			LeaveCriticalSection(&g_cs); 
			return;
		}
		CFileCopy *pcp = new CFileCopy((LPCTSTR)strSrc,strDest);
		pcp->m_sIdPath = strIdPath;
		pcp->m_sNetDiskPath = strNetDiskPath;
		pcp->m_CopyState = CFileCopy::COPY_FINISH;
		pcp->m_iCurrentBytes = pcp->m_iTotalBytes = iFileSize;
		g_CopyProgressInfo[strMapKey] = pcp; // 增加到拷贝历史记录里
	}
	LeaveCriticalSection(&g_cs); 
}
void CCopyManager::AddToManager(CString strSrc ,CString strDest,CString strMapKey,CString strIdPath,CString strNetDiskPath)
{
	ZTools::WriteZToolsFormatLog("CCopyManager::AddToManager开始调用...");
	EnterCriticalSection(&g_cs); 
	CFileCopy *pcp = new CFileCopy((LPCTSTR)strSrc,(LPCTSTR)strDest);
	pcp->m_sIdPath = strIdPath;
	pcp->m_sNetDiskPath = strNetDiskPath;
	//if (PathFileExists(strDest))
	//{
	//	pcp->m_CtrlState = CFileCopy::COPY_RESUM; //再次执行

	//	struct stat buf;
	//	int result = stat(strDest,&buf);
	//	if (result==0)
	//		pcp->m_iCurrentBytes = buf.st_size;	
	//}

	map<CString,CFileCopy *>::iterator  iterFind = g_CopyProgressInfo.find(strMapKey);
	if (iterFind == g_CopyProgressInfo.end())
		g_CopyProgressInfo[strMapKey] = pcp; // 增加到拷贝历史记录里

	else //重复添加到任务管理器时，执行替换操作
	{
		//return; // 该任务正在管理器当中
		iterFind->second->m_CtrlState = CFileCopy::COPY_DELET; // 发送删除命令
		map<CFileCopy *,CWinThread *>::iterator iterThread = m_MapCopyThread.find(iterFind->second);
		if (iterThread!=m_MapCopyThread.end())
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
		g_CopyProgressInfo.erase(iterFind);
		g_CopyProgressInfo[strMapKey] = pcp; // 增加到任务记录表中
	}


	if (m_iCurrentRunningThreadCount >= m_MaxThreadCount) //已经达到线程上限
	{
		UniquePushWaitTask(strMapKey);
		LeaveCriticalSection(&g_cs);  //记得解锁，否则管理线程将挂起
		ZTools::WriteZToolsFormatLog("CCopyManager::AddToManager 已经达到线程上限,调用结束...");
		return;
	}

	StartOneCopyThread(pcp);
	//ZTools::WriteZToolsFormatLog("CCopyManager::AddToManager调用结束...");
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
		    bHave = true;  // 保证队列元素不重复，且不修改原始队列元素顺序

		queTmp.push(str); 

		m_CopyQueue.pop();
	}

	m_CopyQueue = queTmp;
	if (!bHave)
	   m_CopyQueue.push(strMapKey); // 向等待队列中增加一个任务

}
void CCopyManager::DestoyrCopyProgressInfo()
{
	ZTools::WriteZToolsLog("清空拷贝管理器历史记录...");
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

	//如果查询的是文件夹
	map<CString,CFileCopy*>::iterator iter = g_CopyProgressInfo.begin();
	for (iter;iter!=g_CopyProgressInfo.end();iter++)
	{
		CString strKey = iter->first;
		if (strKey.Find(strIndex,0)==0) //该迭代器当前索引与要查找的索引前半部分匹配
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
			thePersistMrg.DoOperation(pTask); //从xml中删除该记录
			delete pTask;
			pTask = NULL;
		}
		ZTools::WriteZToolsFormatLog("CCopyManager::DeleteCopy 收到删除索引为【%s】的任务通知，正在等待线程安全结束...",iter->first);
	}
	else
	{
		//如果查询的是文件夹
		map<CString,CFileCopy*>::iterator iter = g_CopyProgressInfo.begin();
		for (iter;iter!=g_CopyProgressInfo.end();iter++)
		{
			CString strKey = iter->first;
			if (strKey.Find(strIndex,0)==0) //该迭代器当前索引与要查找的索引前半部分匹配
			{
				iter->second->m_CtrlState = CFileCopy::COPY_DELET;
				CTaskData * pTask = thePersistMrg.GetTaskDataCopied(iter->second->m_sIdPath);
				if (pTask)
				{
					pTask->m_bDelete = TRUE;
					thePersistMrg.DoOperation(pTask); //从xml中删除该记录
					delete pTask;
					pTask = NULL;
				}
				ZTools::WriteZToolsFormatLog("CCopyManager::DeleteCopy 收到删除索引为【%s】的任务通知，正在等待线程安全结束...",iter->first);
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
		ZTools::WriteZToolsFormatLog("CCopyManager::StopCopy 收到停止索引为【%s】的任务通知，正在等待线程安全结束...",iter->first);
	}
	else
	{
		//如果查询的是文件夹
		map<CString,CFileCopy*>::iterator iter = g_CopyProgressInfo.begin();
		for (iter;iter!=g_CopyProgressInfo.end();iter++)
		{
			CString strKey = iter->first;
			if (strKey.Find(strIndex,0)==0) //该迭代器当前索引与要查找的索引前半部分匹配
			{
				iter->second->m_CtrlState = CFileCopy::COPY_STOP;
				ZTools::WriteZToolsFormatLog("CCopyManager::StopCopy 收到停止索引为【%s】的任务通知，正在等待线程安全结束...",iter->first);
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
		ZTools::WriteZToolsFormatLog("CCopyManager::SuspendCopy 收到暂停索引为【%s】的任务通知，正在等待线程安全结束...",iter->first);
	}
	else
	{
		//如果查询的是文件夹
		map<CString,CFileCopy*>::iterator iter = g_CopyProgressInfo.begin();
		for (iter;iter!=g_CopyProgressInfo.end();iter++)
		{
			CString strKey = iter->first;
			if (strKey.Find(strIndex,0)==0) //该迭代器当前索引与要查找的索引前半部分匹配
			{
				iter->second->m_CtrlState = CFileCopy::COPY_PAUSE;
				ZTools::WriteZToolsFormatLog("CCopyManager::SuspendCopy 收到暂停索引为【%s】的任务通知，正在等待线程安全结束...",iter->first);
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
			ZTools::WriteZToolsFormatLog("索引为【%s】的任务状态已经重置,将重新执行",strIndex);
			iter->second->m_CopyState = CFileCopy::COPY_WAIT_SATRT;
			iter->second->m_CtrlState = CFileCopy::COPY_RESUME; 
			iter->second->m_strErrMsg = "";
			m_CopyQueue.push(strIndex); //加入等待队列 等待重新开始执行
		}
	}
	else
	{
		//如果查询的是文件夹
		map<CString,CFileCopy*>::iterator iter = g_CopyProgressInfo.begin();
		for (iter;iter!=g_CopyProgressInfo.end();iter++)
		{
			CString strKey = iter->first;
			if (strKey.Find(strIndex,0)==0) //该迭代器当前索引与要查找的索引前半部分匹配
			{
				if (iter->second->m_CtrlState == CFileCopy::COPY_STOP || iter->second->m_CtrlState == CFileCopy::COPY_PAUSE)
				{
					ZTools::WriteZToolsFormatLog("索引为【%s】的任务状态已经重置,将重新执行",strKey);
					iter->second->m_strErrMsg = "";
					iter->second->m_CopyState = CFileCopy::COPY_WAIT_SATRT;
					iter->second->m_CtrlState = CFileCopy::COPY_RESUME; 
					m_CopyQueue.push(strKey); //加入等待队列 等待重新开始执行
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
	//在此启动一个监护进程，管理子线程的状态
	HANDLE hThread =CreateThread(NULL,0,CopyManagerThreadPro,NULL,0,NULL);
}
CCopyManager::CCopyManager()
{	
	ZTools::WriteZToolsLog("拷贝管理器CCopyManager构造函数调用...");
	m_iCurrentRunningThreadCount = 0;
	m_MaxThreadCount = 1; // 拷贝线程限定为1个
	/// 在使用临界区前，必须进行初始化    
	InitializeCriticalSection(&g_cs);
	StartManagerThread();
}
CCopyManager::~CCopyManager ()
{
	/// 使用完后，显示删除以避免资源泄漏 
	ZTools::WriteZToolsLog("拷贝管理器CCopyManager析构函数调用...");
	DestoyrCopyProgressInfo();
	DeleteCriticalSection(&g_cs); 
}
void CCopyManager::StartOneTaskFromQueue()
{
	// 取出队列中的第一个任务
	CString strTop = m_CopyQueue.front();
	map<CString,CFileCopy*>::iterator iter = g_CopyProgressInfo.find(strTop);
	if (iter==g_CopyProgressInfo.end())
		return;

	StartOneCopyThread(iter->second);
	m_CopyQueue.pop(); // 删除队列中的第一个元素
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
			if (iter->second->m_CtrlState!=CFileCopy::COPY_DELET) //保留不是删除的任务
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
			//找出完成的，且取消的任务，将其从任务列表中删除
			EnterCriticalSection(&g_cs);
			if ( pManager->m_CopyQueue.size()>0)
				pManager->DeleteCanceledFromQueue();
			
			map<CString,CFileCopy*>::iterator iter = pManager->g_CopyProgressInfo.begin();
			for (iter;iter!=pManager->g_CopyProgressInfo.end();)
			{
				// 删除的需要清除线程和历史记录，停止的关闭线程，不删除历史记录
				if (iter->second->m_CtrlState == CFileCopy::COPY_DELET || iter->second->m_CtrlState == CFileCopy::COPY_STOP)
				{
					map<CFileCopy *, CWinThread *>::iterator iterFind = pManager->m_MapCopyThread.find(iter->second);
					if (iterFind!=pManager->m_MapCopyThread.end()) // 线程数据还存在
					{
						DWORD dExitCode = 0;
						CWinThread *pFinishedThread = pManager->m_MapCopyThread[iter->second];
						GetExitCodeThread(pFinishedThread->m_hThread,&dExitCode);

						if (dExitCode != STILL_ACTIVE)
						{
							pManager->m_MapCopyThread.erase(iterFind); //从线程映射表中删除该映射
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
					if (iter->second->m_CtrlState == CFileCopy::COPY_DELET)
					{
						delete iter->second;
						ZTools::WriteZToolsFormatLog("删除索引为【%s】的拷贝任务成功，拷贝线程已经安全退出...",iter->first);
						pManager->g_CopyProgressInfo.erase(iter++); //清空已取消的任务
					}
					else
					{
						iter++;
						ZTools::WriteZToolsFormatLog("停止索引为【%s】的拷贝任务成功，拷贝线程已经安全退出...",iter->first);
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
				|| iter->second->m_CopyState == CFileCopy::COPY_STOP) // 拷贝线程已经结束
			{
				map<CFileCopy *, CWinThread *>::iterator iterFind = pManager->m_MapCopyThread.find(iter->second);
				if (iterFind!=pManager->m_MapCopyThread.end())
				{
					CWinThread *pFinishedThread = pManager->m_MapCopyThread[iter->second];
					WaitForSingleObject(pFinishedThread->m_hThread,INFINITE); //等待线程退出后再删除线程对象，否则会崩溃
					pManager->m_MapCopyThread.erase(iterFind); //从线程映射表中删除该映射
					delete pFinishedThread; // 线程结束
					iRuningCount--; // 正在运行的上传线程数减少，退出完成任务的线程
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