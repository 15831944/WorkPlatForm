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
	void DeleteCopy(CString strIndex,BOOL bIsFile);   //ɾ����������
	void StopCopy(CString strIndex,BOOL bIsFile);     //ֹͣ��������
	void PauseCopy(CString strIndex,BOOL bIsFile);  // ��ͣ��������
	void ResumeCopy(CString strIndex,BOOL bIsFile);   // ������������
	void AddToManager(CString strSrc ,CString strDest,CString strMapKey,CString strIdPath,CString strNetDiskPath);
	void SetMaxThreadCount(int iThreadCount);
	void LoadFinishedCopyTask(CString strSrc ,CString strDest,CString strMapKey,CString strIdPath,CString strNetDiskPath);
	static DWORD WINAPI CopyManagerThreadPro(LPVOID lpParamter);
	static UINT  CopyThreadPro(LPVOID lpParamter); //�����̺߳���
private:
	CCopyManager();
	~CCopyManager ();
	void DestoyrCopyProgressInfo();
	void StartManagerThread();
	void StartOneCopyThread(CFileCopy *pCp);
	void StartOneTaskFromQueue();
	void DeleteCanceledFromQueue(); // �Ӷ�����ɾ���Ѿ�ɾ����ȡ����������
	void GetRunningThreads(HANDLE **pHandleArry);
	void UniquePushWaitTask(CString strKey);
public:
	map<CString,CFileCopy *>    g_CopyProgressInfo;
	map<CFileCopy *,CWinThread *> m_MapCopyThread;
	queue<CString> m_CopyQueue; //�ȴ��еĿ�������
	int m_MaxThreadCount;
	int m_iCurrentRunningThreadCount;
private:
	class GabageCollector // ����˽����Ƕ��
	{
	public:
		GabageCollector()
		{
			ZTools::WriteZToolsLog("����������CCopyManager::GabageCollector����...");
		}
		~GabageCollector()
		{
			if (m_PManagetr)
			{
				delete m_PManagetr;
				m_PManagetr = 0;
				ZTools::WriteZToolsLog("����������CCopyManager::GabageCollectorִ�е�����������������...");
			}
		}

	};
	static CCopyManager * m_PManagetr;
	static GabageCollector m_gc; // ���������������ľ�̬���󣬳��������ϵͳ����øö����������������ִ�жԵ������������
};