#pragma once
#include "CallHprose.h"
#include<queue>
#include "Tools.h"
using namespace std;

class CUploadInputParam
{
public:
	CUploadInputParam()
	{
		m_strFilePath.Empty();
		m_strHproseRpc.Empty();
		m_strIndex.Empty();
		m_strPath.Empty();
		m_strIdPath.Empty();
		m_bCreateIndex = FALSE;
	}
	CUploadInputParam(CString strHprose,CString strFilePath,CString strIndex)
	{
		m_strFilePath = strFilePath;
		m_strHproseRpc = strHprose;
		m_strIndex = strIndex;
		m_bCreateIndex = FALSE;
	}
	void SetDataPath(CString strPath,CString strIdPath)
	{
		m_strPath = strPath;
		m_strIdPath = strIdPath;
	}
	CString m_strFilePath;
	CString m_strHproseRpc;
	CString m_strIndex;
	CString m_strPath;   //��ѯ�ļ��е�ʱ��Ҫ��
	CString m_strIdPath; //��ѯ�ļ��е�ʱ��Ҫ��
	BOOL    m_bCreateIndex;
};
class CUploadManager
{
public:
	static CUploadManager * Instance(); //��ȡ����������
	static DWORD WINAPI UploadManagerThreadPro(LPVOID lpParamter); // ��������̺߳���
	static UINT UploadThreadPro(LPVOID lpParamter); //�ϴ��̺߳���

	void AddToManager(CString str ,const CUploadInputParam & param);
	void SetMaxThreadCount(int iThreadCount); //��������߳���
	void StartOneTaskFromQueue(); // �����������ȡ��һ����������
	vector<CallHprose *> GetUploadProgressInfo(CString strIndex,BOOL IsFile = true); // ��ѯ���Ⱥ���
	void StopUpload(CString strIndex,BOOL bIsFile);    //ֹͣ�ϴ�����
	void DeleteUpload(CString strIndex,BOOL bIsFile);    //ɾ���ϴ�����
	void PauseUpload(CString strIndex,BOOL bIsFile);  // ��ͣ�ϴ�����
	void ResumeUpload(CString strIndex,BOOL bIsFile);  // �����ϴ�����
	void LoadFinishedUploadTask(CString str ,const CUploadInputParam & param); // ��������ʱ�ָ���������ɵ��ϴ�����
private:
	CUploadManager(); // ���������캯��������Ϊ�����ͣ�ֻ����
	~CUploadManager ();
	void StartManagerThread(); // ���������̵߳ĺ���
	void StartOneUploadThread(CallHprose *pHpose);//�����µ��ϴ�����
	void DeleteCanceledFromQueue(); // �ӵȴ�������ɾ��ȡ��������
	void DestoyrUploadProgressInfo(); // ����ϴ��������е������¼
	void GetSystemConfig();
	void UniquePushWaitTask(CString strMapKey);
public:
	queue<CString> m_UploadQueue; //�ȴ��е��ϴ�����
	int m_MaxThreadCount;
	int m_iCurrentRunningThreadCount;
	map<CString,CallHprose *> m_UploadTaskMap;
	map<CallHprose *,CWinThread *> m_MapUpThread;
private:
	class GabageCollector // ����˽����Ƕ��
	{
	public:
		GabageCollector()
		{
			ZTools::WriteZToolsLog("����������CUploadManager::GabageCollector����...");
		}
		~GabageCollector()
		{
			if (m_PManagetr)
			{
				delete m_PManagetr;
				m_PManagetr = 0;
				ZTools::WriteZToolsLog("����������CUploadManager::GabageCollectorִ�е�����������������...");
			}
		}

	};
	static CUploadManager * m_PManagetr;
	static GabageCollector m_gc; // ���������������ľ�̬���󣬳��������ϵͳ����øö����������������ִ�жԵ������������
};