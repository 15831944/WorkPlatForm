#pragma once
#include <map>
#include <string>
#include <vector>
#include <queue>
using  namespace std;

class CFileCopy
{
public:
	typedef enum{
		COPY_WAIT_SATRT  = 0,  // 等待开始
		COPY_RUNNING     = 1,  //正在拷贝
		COPY_FINISH		 = 2,  //拷贝完成
		COPY_DELET       = 3,  // 删除
		COPY_STOP        = 4,  // 停止
		COPY_PAUSE       = 5,  // 暂停
		COPY_RESUME      = 6,   // 再次执行
		COPY_ERROR       = 7   // 执行出错
	}COPY_STATE;

	typedef enum
		{
			TASK_FILE = 0, //拷贝文件
			TASK_DIR  = 1 //拷贝文件夹
		}TASK_TYPE;
public:
	CFileCopy(const char *pszSrc,const char *pszDest);
	bool ExcuteCopy(bool bForce = false); // 阻塞式拷贝入库函数
	CString GetErrMsg(){return m_strErrMsg;}
	COPY_STATE GetCopyState(){ return m_CopyState; }
	int copyDir(const char * pSrc,const char *pDes);
	static UINT CopyFileThreadPro(LPVOID lpParamter);
	static UINT SysCopyFileThreadPro(LPVOID lpParamter);

private:
	void CheckParam();
	void CalcfolderSpace(const char * pszDir,bool bRecusion);
public:
	CString strSrc;
	CString strDst;
	CString strDestFileName;
	CString m_sIdPath; //id路径
	CString m_sNetDiskPath;
	CString m_strErrMsg; /* 存储出错信息*/
	bool    m_bForce;
	long long m_iTotalBytes;
	long long m_iCurrentBytes;
	COPY_STATE m_CopyState;
	COPY_STATE m_CtrlState;
	HANDLE   m_hThreadHandle;
	bool     m_bParamOK;
	TASK_TYPE  m_TaskType;
};