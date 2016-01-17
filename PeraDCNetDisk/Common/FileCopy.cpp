#include "stdafx.h"
#include "FileCopy.h"
#include <direct.h>
#include <io.h>
#include "comutil.h"
#include "CommonMethod.h"
#include "FileFunc.h"
#include "ConvertData.h"
#include <sstream>

#define  COPY_ONCE_SIZE  (1024*1024)

static DWORD WINAPI CopyFileCallBack(LARGE_INTEGER TotalFileSize, LARGE_INTEGER TotalBytesTransferred,
	LARGE_INTEGER StreamSize,LARGE_INTEGER StreamBytesTransferred,
	DWORD dwStreamNumber,DWORD dwCallbackReason,
	HANDLE hSourceFile, HANDLE hDestinationFile, LPVOID lpData)
{
	CFileCopy *parma = (CFileCopy*)lpData;
	parma->m_iTotalBytes = TotalFileSize.QuadPart;
	parma->m_iCurrentBytes = TotalBytesTransferred.QuadPart;

	if (parma->m_CtrlState == CFileCopy::COPY_STOP)
		return PROGRESS_CANCEL;

	if(parma->m_CtrlState == CFileCopy::COPY_DELET)
		return PROGRESS_STOP;

	return PROGRESS_CONTINUE;
}
void CFileCopy::CalcfolderSpace(const char * pszDir,bool bRecusion)
{
	char dir[MAX_PATH] = {0};
	char srcFileName[MAX_PATH] = {0};
	char desFileName[MAX_PATH] = {0};
	char *str = "\\*.*";
	strcpy_s(dir,pszDir);
	strcat_s(dir,str);
	//���Ȳ���dir�з���Ҫ����ļ�
	long hFile;
	_finddata_t fileinfo;
	if ((hFile = _findfirst(dir,&fileinfo)) != -1)
	{
		do
		{
			strcpy_s(srcFileName,pszDir);
			strcat_s(srcFileName,"\\");
			strcat_s(srcFileName,fileinfo.name);
			//����ǲ���Ŀ¼
			//�������Ŀ¼,����д����ļ���������ļ�
			if (!(fileinfo.attrib & _A_SUBDIR))
				m_iTotalBytes+= fileinfo.size;
			else//����Ŀ¼���ݹ����
			{
				if ( strcmp(fileinfo.name, "." ) != 0 && strcmp(fileinfo.name, ".." ) != 0 && bRecusion)
				{
					CalcfolderSpace(srcFileName,bRecusion);
				}
			}
		} while (_findnext(hFile,&fileinfo) == 0);
		_findclose(hFile);
		return ;
	}
}
void CFileCopy::CheckParam()
{
	if (!m_bParamOK)
		return;

	if (!PathFileExists(strSrc))
	{
		m_strErrMsg.Format("�����ļ���%s���޷���",strSrc);
		m_bParamOK = false;
		return;
	}

	MakeSureDirectoryPathExists(strDst); // ������Ŀ¼�����ڣ��򴴽����Ŀ¼

	strDestFileName = GetFileName((LPCTSTR)strDst).c_str();
	if (!IsAccessibleFile(strSrc))
	{
		m_TaskType = TASK_DIR;
		CalcfolderSpace(strSrc,true);
		return;
	}
	m_TaskType = TASK_FILE;
	LONGLONG iFileSize = FileSize(strSrc);
	m_iTotalBytes = iFileSize;
}
CFileCopy::CFileCopy(const char *pszSrc,const char *pszDest)
{
	m_bParamOK = true;
	if (NULL == pszSrc)
	{
		m_strErrMsg = "����Դ�ļ�·��Ϊ��";
		m_bParamOK = false;
		strSrc = "";
	}
	else
		strSrc = pszSrc;

	if(NULL == pszDest)
	{
		m_strErrMsg = "���Ŀ��·��Ϊ��";
		m_bParamOK = false;
		strDst = "";
	}
	else
		strDst = pszDest;

	m_bForce = false;
	m_CtrlState = COPY_RUNNING;
	m_iTotalBytes   = -1;
	m_iCurrentBytes = 0;
	m_hThreadHandle = NULL;
	m_CopyState = COPY_WAIT_SATRT;
	CheckParam();
}
UINT CFileCopy::CopyFileThreadPro(LPVOID lpParamter)
{
	CFileCopy *param = (CFileCopy*)lpParamter;
	if (param->m_TaskType == CFileCopy::TASK_DIR)
	{
		param->copyDir(param->strSrc,param->strDst);
		return 0;
	}

	long long iFileSize = param->m_iTotalBytes;

	long long iReadBytes   = COPY_ONCE_SIZE;
	if (iFileSize<COPY_ONCE_SIZE)
		iReadBytes = iFileSize; //��������Сʱһ���Զ�д


	std::wstring wstrFilePath = s2ws_1((LPCTSTR)param->strSrc);
	std::ifstream fIn(wstrFilePath, std::ios::binary);
	if (fIn == NULL || fIn.is_open() == false)
	{
		param->m_strErrMsg.Format( _T("��Դ�ļ���%s��ʧ�ܣ�"),param->strSrc);
		param->m_CtrlState = COPY_ERROR;
		return 1;
	}
	FILE *pDstFile = NULL;
	fopen_s(&pDstFile,param->strDst,"wb+");
	if (NULL == pDstFile)
	{
		param->m_strErrMsg.Format( _T("����Ŀ���ļ���%s��ʧ�ܣ�"),param->strDst);
		param->m_CtrlState = COPY_ERROR;
		return 1;
	}
	long long iAlloacSize = iReadBytes + 1;
	char *pData = new char[(int)iAlloacSize];
	memset(pData,0,(int)iAlloacSize);

	long long iReaded = 0;
	long long iWritten = 0;
	if (param->m_CtrlState== COPY_RESUME)
	{
		iReaded = iWritten = param->m_iCurrentBytes;
		fIn.seekg(iReaded,std::ios::beg); 
		fseek(pDstFile,(long)iWritten,SEEK_SET);
		param->m_CtrlState = COPY_RUNNING;
	}

	while(1)
	{
		int iReadThis  = fIn.read(pData, iReadBytes).gcount();
		if (0==iReadBytes)
			break;

		iReaded  += iReadThis;
		int iWriteThis = fwrite(pData,1,iReadThis,pDstFile); //д
		iWritten += iWriteThis;

		param->m_iCurrentBytes = iWritten;
		
		if ((iReaded + iReadBytes)>iFileSize)
			iReadBytes = iFileSize - iReaded;

		if (param->m_CtrlState == CFileCopy::COPY_DELET)
		{
			param->m_strErrMsg = _T("�û�ɾ����");
			param->m_CtrlState = COPY_DELET;
			break;
		}
		else if (param->m_CtrlState == CFileCopy::COPY_STOP)
		{
			param->m_strErrMsg = _T("�û�ֹͣ��");
			param->m_CtrlState = COPY_STOP;
			break;
		}
	}

	delete []pData;
	fIn.close();
	fclose(pDstFile);
	if (param->m_CtrlState == CFileCopy::COPY_DELET)
	{
		param->m_strErrMsg = "�û�ȡ������!";
		//MessageBox(NULL,"�û�ȡ������!","��ʾ",MB_ICONINFORMATION);
		//remove(param->strDst);
	}
	return 0;
}

UINT CFileCopy::SysCopyFileThreadPro(LPVOID lpParamter)
{
	CFileCopy *param = (CFileCopy*)lpParamter;
	if (param->m_TaskType == CFileCopy::TASK_DIR)
	{
		param->copyDir(param->strSrc,param->strDst);
		return 0;
	}

	BOOL bok = CopyFileEx(param->strSrc,
		param->strDst,
		CopyFileCallBack,
		param,
		NULL,
		COPY_FILE_FAIL_IF_EXISTS);

	if (!bok)
	{
		if (param->m_CtrlState == CFileCopy::COPY_DELET)
		{
			//MessageBox(NULL,"�û�ȡ������!","��ʾ",MB_ICONINFORMATION);
			param->m_strErrMsg = "�û�ɾ������!";
		}
		else
		{
			CString str;
			str.Format("����ʧ�ܣ�Ŀ���ļ���%s�������Ѿ����ڣ�",param->strDst);
			//MessageBox(NULL,str,"��ʾ",MB_ICONERROR);
			param->m_strErrMsg = str;
		}

		return 1;
	}
	//MessageBox(NULL,"���Ƴɹ���","��ʾ",MB_ICONINFORMATION);
	return 0;
}
int CFileCopy::copyDir(const char * pSrc,const char *pDes)
{
	if (NULL == pSrc || NULL == pDes)	return -1;
	_mkdir(pDes);
	char dir[MAX_PATH] = {0};
	char srcFileName[MAX_PATH] = {0};
	char desFileName[MAX_PATH] = {0};
	char *str = "\\*.*";
	strcpy_s(dir,pSrc);
	strcat_s(dir,str);
	//���Ȳ���dir�з���Ҫ����ļ�
	long hFile;
	_finddata_t fileinfo;
	if ((hFile = _findfirst(dir,&fileinfo)) != -1)
	{
		do
		{
			if (m_CtrlState== COPY_DELET || m_CtrlState== COPY_STOP)
			{
				m_strErrMsg = "�û�ȡ������!";
				break;
			}
			strcpy_s(srcFileName,pSrc);
			strcat_s(srcFileName,"\\");
			strcat_s(srcFileName,fileinfo.name);
			strcpy_s(desFileName,pDes);
			strcat_s(desFileName,"\\");
			strcat_s(desFileName,fileinfo.name);
			//����ǲ���Ŀ¼
			//�������Ŀ¼,����д����ļ���������ļ�
			if (!(fileinfo.attrib & _A_SUBDIR))
			{
				CopyFileEx(srcFileName,desFileName,NULL,NULL,NULL,COPY_FILE_FAIL_IF_EXISTS);
				m_iCurrentBytes += fileinfo.size; // ͳ�Ƶ�ǰ��������
			}
			else//����Ŀ¼���ݹ����
			{
				if ( strcmp(fileinfo.name, "." ) != 0 && strcmp(fileinfo.name, ".." ) != 0 )
				{
					copyDir(srcFileName,desFileName);
				}
			}
		} while (_findnext(hFile,&fileinfo) == 0);
		_findclose(hFile);
		return 1;
	}
	return -3;
}
bool CFileCopy::ExcuteCopy(bool bForce)
{
	if (!m_bParamOK)
	{
		m_CtrlState = COPY_ERROR;
		return false;
	}

	if (m_CopyState != COPY_RESUME && PathFileExists(strDst)) //����ļ��Ѿ�����
	{
		if(remove(strDst)!=0)
		{
			m_strErrMsg.Format("���Ŀ���ļ���%s���Ѿ�����,���޷����ǣ�",strDst);
			m_bParamOK = false;
			return false;
		}
	}
	if (CopyFileThreadPro(this)==0)
		return true;

	return false;
}