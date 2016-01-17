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
		COPY_WAIT_SATRT  = 0,  // �ȴ���ʼ
		COPY_RUNNING     = 1,  //���ڿ���
		COPY_FINISH		 = 2,  //�������
		COPY_DELET       = 3,  // ɾ��
		COPY_STOP        = 4,  // ֹͣ
		COPY_PAUSE       = 5,  // ��ͣ
		COPY_RESUME      = 6,   // �ٴ�ִ��
		COPY_ERROR       = 7   // ִ�г���
	}COPY_STATE;

	typedef enum
		{
			TASK_FILE = 0, //�����ļ�
			TASK_DIR  = 1 //�����ļ���
		}TASK_TYPE;
public:
	CFileCopy(const char *pszSrc,const char *pszDest);
	bool ExcuteCopy(bool bForce = false); // ����ʽ������⺯��
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
	CString m_sIdPath; //id·��
	CString m_sNetDiskPath;
	CString m_strErrMsg; /* �洢������Ϣ*/
	bool    m_bForce;
	long long m_iTotalBytes;
	long long m_iCurrentBytes;
	COPY_STATE m_CopyState;
	COPY_STATE m_CtrlState;
	HANDLE   m_hThreadHandle;
	bool     m_bParamOK;
	TASK_TYPE  m_TaskType;
};