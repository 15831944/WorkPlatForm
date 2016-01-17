#pragma once 
#include <queue>
#include "StlHeader.h"
#include "Tools.h"


class CFileCopy;

class CCopyManager
{
public:
	static CCopyManager * Instance();
	vector<CFileCopy *> GetCopyProgressInfo(CString strIndex,BOOL IsFile = true);
	void DeleteCopy(CString strIndex,BOOL bIsFile);   //删除拷贝操作
	void StopCopy(CString strIndex,BOOL bIsFile);     //停止拷贝操作
	void PauseCopy(CString strIndex,BOOL bIsFile);  // 暂停拷贝操作
	void ResumeCopy(CString strIndex,BOOL bIsFile);   // 继续拷贝操作
	void AddToManager(CString strSrc ,CString strDest,CString strMapKey,CString strIdPath,CString strNetDiskPath);
	void SetMaxThreadCount(int iThreadCount);
	void LoadFinishedCopyTask(CString strSrc ,CString strDest,CString strMapKey,CString strIdPath,CString strNetDiskPath);
	static DWORD WINAPI CopyManagerThreadPro(LPVOID lpParamter);
	static UINT  CopyThreadPro(LPVOID lpParamter); //拷贝线程函数
private:
	CCopyManager();
	~CCopyManager ();
	void DestoyrCopyProgressInfo();
	void StartManagerThread();
	void StartOneCopyThread(CFileCopy *pCp);
	void StartOneTaskFromQueue();
	void DeleteCanceledFromQueue(); // 从队列中删除已经删除（取消）的任务
	void GetRunningThreads(HANDLE **pHandleArry);
	void UniquePushWaitTask(CString strKey);
public:
	map<CString,CFileCopy *>    g_CopyProgressInfo;
	map<CFileCopy *,CWinThread *> m_MapCopyThread;
	queue<CString> m_CopyQueue; //等待中的拷贝任务
	int m_MaxThreadCount;
	int m_iCurrentRunningThreadCount;
private:
	class GabageCollector // 定义私有内嵌类
	{
	public:
		GabageCollector()
		{
			ZTools::WriteZToolsLog("垃圾回收器CCopyManager::GabageCollector构建...");
		}
		~GabageCollector()
		{
			if (m_PManagetr)
			{
				delete m_PManagetr;
				m_PManagetr = 0;
				ZTools::WriteZToolsLog("垃圾回收器CCopyManager::GabageCollector执行单例对象清理工作结束...");
			}
		}

	};
	static CCopyManager * m_PManagetr;
	static GabageCollector m_gc; // 定义垃圾回收器的静态对象，程序结束后，系统会调用该对象的析构函数，以执行对单例对象的析构
};