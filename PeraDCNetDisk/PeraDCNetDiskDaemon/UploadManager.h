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
	CString m_strPath;   //查询文件夹的时候要用
	CString m_strIdPath; //查询文件夹的时候要用
	BOOL    m_bCreateIndex;
};
class CUploadManager
{
public:
	static CUploadManager * Instance(); //获取管理器单例
	static DWORD WINAPI UploadManagerThreadPro(LPVOID lpParamter); // 任务管理线程函数
	static UINT UploadThreadPro(LPVOID lpParamter); //上传线程函数

	void AddToManager(CString str ,const CUploadInputParam & param);
	void SetMaxThreadCount(int iThreadCount); //设置最大线程数
	void StartOneTaskFromQueue(); // 从任务队列中取出一个任务并启动
	vector<CallHprose *> GetUploadProgressInfo(CString strIndex,BOOL IsFile = true); // 查询进度函数
	void StopUpload(CString strIndex,BOOL bIsFile);    //停止上传操作
	void DeleteUpload(CString strIndex,BOOL bIsFile);    //删除上传操作
	void PauseUpload(CString strIndex,BOOL bIsFile);  // 暂停上传操作
	void ResumeUpload(CString strIndex,BOOL bIsFile);  // 继续上传操作
	void LoadFinishedUploadTask(CString str ,const CUploadInputParam & param); // 用于启动时恢复加载已完成的上传任务
private:
	CUploadManager(); // 管理器构造函数，设置为保护型，只允许
	~CUploadManager ();
	void StartManagerThread(); // 启动管理线程的函数
	void StartOneUploadThread(CallHprose *pHpose);//启动新的上传任务
	void DeleteCanceledFromQueue(); // 从等待队列中删除取消的任务
	void DestoyrUploadProgressInfo(); // 清空上传管理器中的任务记录
	void GetSystemConfig();
	void UniquePushWaitTask(CString strMapKey);
public:
	queue<CString> m_UploadQueue; //等待中的上传任务
	int m_MaxThreadCount;
	int m_iCurrentRunningThreadCount;
	map<CString,CallHprose *> m_UploadTaskMap;
	map<CallHprose *,CWinThread *> m_MapUpThread;
private:
	class GabageCollector // 定义私有内嵌类
	{
	public:
		GabageCollector()
		{
			ZTools::WriteZToolsLog("垃圾回收器CUploadManager::GabageCollector构建...");
		}
		~GabageCollector()
		{
			if (m_PManagetr)
			{
				delete m_PManagetr;
				m_PManagetr = 0;
				ZTools::WriteZToolsLog("垃圾回收器CUploadManager::GabageCollector执行单例对象清理工作结束...");
			}
		}

	};
	static CUploadManager * m_PManagetr;
	static GabageCollector m_gc; // 定义垃圾回收器的静态对象，程序结束后，系统会调用该对象的析构函数，以执行对单例对象的析构
};