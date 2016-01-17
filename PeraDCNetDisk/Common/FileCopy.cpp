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
	//首先查找dir中符合要求的文件
	long hFile;
	_finddata_t fileinfo;
	if ((hFile = _findfirst(dir,&fileinfo)) != -1)
	{
		do
		{
			strcpy_s(srcFileName,pszDir);
			strcat_s(srcFileName,"\\");
			strcat_s(srcFileName,fileinfo.name);
			//检查是不是目录
			//如果不是目录,则进行处理文件夹下面的文件
			if (!(fileinfo.attrib & _A_SUBDIR))
				m_iTotalBytes+= fileinfo.size;
			else//处理目录，递归调用
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
		m_strErrMsg.Format("输入文件【%s】无法打开",strSrc);
		m_bParamOK = false;
		return;
	}

	MakeSureDirectoryPathExists(strDst); // 如果输出目录不存在，则创建输出目录

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
		m_strErrMsg = "输入源文件路径为空";
		m_bParamOK = false;
		strSrc = "";
	}
	else
		strSrc = pszSrc;

	if(NULL == pszDest)
	{
		m_strErrMsg = "输出目标路径为空";
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
		iReadBytes = iFileSize; //数据量极小时一次性读写


	std::wstring wstrFilePath = s2ws_1((LPCTSTR)param->strSrc);
	std::ifstream fIn(wstrFilePath, std::ios::binary);
	if (fIn == NULL || fIn.is_open() == false)
	{
		param->m_strErrMsg.Format( _T("打开源文件【%s】失败！"),param->strSrc);
		param->m_CtrlState = COPY_ERROR;
		return 1;
	}
	FILE *pDstFile = NULL;
	fopen_s(&pDstFile,param->strDst,"wb+");
	if (NULL == pDstFile)
	{
		param->m_strErrMsg.Format( _T("创建目标文件【%s】失败！"),param->strDst);
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
		int iWriteThis = fwrite(pData,1,iReadThis,pDstFile); //写
		iWritten += iWriteThis;

		param->m_iCurrentBytes = iWritten;
		
		if ((iReaded + iReadBytes)>iFileSize)
			iReadBytes = iFileSize - iReaded;

		if (param->m_CtrlState == CFileCopy::COPY_DELET)
		{
			param->m_strErrMsg = _T("用户删除！");
			param->m_CtrlState = COPY_DELET;
			break;
		}
		else if (param->m_CtrlState == CFileCopy::COPY_STOP)
		{
			param->m_strErrMsg = _T("用户停止！");
			param->m_CtrlState = COPY_STOP;
			break;
		}
	}

	delete []pData;
	fIn.close();
	fclose(pDstFile);
	if (param->m_CtrlState == CFileCopy::COPY_DELET)
	{
		param->m_strErrMsg = "用户取消拷贝!";
		//MessageBox(NULL,"用户取消拷贝!","提示",MB_ICONINFORMATION);
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
			//MessageBox(NULL,"用户取消拷贝!","提示",MB_ICONINFORMATION);
			param->m_strErrMsg = "用户删除拷贝!";
		}
		else
		{
			CString str;
			str.Format("复制失败，目标文件【%s】可能已经存在！",param->strDst);
			//MessageBox(NULL,str,"提示",MB_ICONERROR);
			param->m_strErrMsg = str;
		}

		return 1;
	}
	//MessageBox(NULL,"复制成功！","提示",MB_ICONINFORMATION);
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
	//首先查找dir中符合要求的文件
	long hFile;
	_finddata_t fileinfo;
	if ((hFile = _findfirst(dir,&fileinfo)) != -1)
	{
		do
		{
			if (m_CtrlState== COPY_DELET || m_CtrlState== COPY_STOP)
			{
				m_strErrMsg = "用户取消拷贝!";
				break;
			}
			strcpy_s(srcFileName,pSrc);
			strcat_s(srcFileName,"\\");
			strcat_s(srcFileName,fileinfo.name);
			strcpy_s(desFileName,pDes);
			strcat_s(desFileName,"\\");
			strcat_s(desFileName,fileinfo.name);
			//检查是不是目录
			//如果不是目录,则进行处理文件夹下面的文件
			if (!(fileinfo.attrib & _A_SUBDIR))
			{
				CopyFileEx(srcFileName,desFileName,NULL,NULL,NULL,COPY_FILE_FAIL_IF_EXISTS);
				m_iCurrentBytes += fileinfo.size; // 统计当前拷贝进度
			}
			else//处理目录，递归调用
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

	if (m_CopyState != COPY_RESUME && PathFileExists(strDst)) //输出文件已经存在
	{
		if(remove(strDst)!=0)
		{
			m_strErrMsg.Format("输出目标文件“%s”已经存在,且无法覆盖！",strDst);
			m_bParamOK = false;
			return false;
		}
	}
	if (CopyFileThreadPro(this)==0)
		return true;

	return false;
}