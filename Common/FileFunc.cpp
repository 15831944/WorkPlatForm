#include "StdAfx.h"
#include "FileFunc.h"
#include "StlHeader.h"
#include <shlobj.h> // for SHGetSpecialFolderPath
#include <winbase.h> // GetModuleHandleEx
#include <comdef.h>
#include "Tools.h"
//取得当前 Exe 或者 Dll 模块的名称
//GetModuleFileName 第一个参数 hModule 无法提供，如果提供 NULL, 只能得到创建进程的那个EXE文件。
CString GetCurModuleName(void)
{
	//CString strCommandLine = GetCommandLine();
	//if (__argc <= 0) return _T("");
	//CString strFullPathfilename = __argv[0];
	//if (strFullPathfilename.GetLength() <= 0) return _T("");
	//int nPos = strFullPathfilename.ReverseFind('\\');
	//if (nPos < 0 || nPos >= strFullPathfilename.GetLength() -1) return _T("");
	//CString strFileName = strFullPathfilename.Mid(nPos, strFullPathfilename.GetLength() -1);
	//return strFileName;

	CHAR chBuf[2048] = {NULL};
	DWORD dwBufSize = 2048;

	HMODULE hModule = NULL;
	::GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
		, (LPCSTR)&GetCurModuleName //这是函数名，强转
		, &hModule);
	DWORD dwLen = GetModuleFileNameA(hModule, chBuf, dwBufSize);

	if (dwLen == 0) return "";
	CString strFullPath = chBuf;

	strFullPath.Replace("\\", "/");
	int nPos = strFullPath.ReverseFind('/');
	if (nPos < 0 || nPos >= strFullPath.GetLength() - 1) return _T("");
	CString strFileName = strFullPath.Mid(nPos + 1, strFullPath.GetLength() - nPos);
	return strFileName;
}


//取得应用程序当前目录
CString GetExecDir(void)
{
	CString strRet = _T("");
	TCHAR exeFullPath[1024] = {0};
	GetModuleFileName(NULL, exeFullPath, 1024);
	strRet = exeFullPath;
	int nPos = strRet.ReverseFind('\\');
	strRet = strRet.Left(nPos);
	return strRet;
}


//文件路径是否存在
BOOL IsAccessibleDir(LPCTSTR strPath)
{
	WIN32_FIND_DATAW fd;
	HANDLE hFind;
	BOOL bRet = FALSE;

	if (strlen((char *)strPath) == 3)
	{
		char *p = (char *)strPath;
		if (((*p > 'A' && *p < 'Z') || (*p > 'a' && *p < 'z'))
			&& *(p+1) == ':'
			&& *(p+2) == '\\')
			return TRUE;
		else
			return FALSE;
	}
	CStringW strFileW = (WCHAR*)_bstr_t(strPath);
	hFind = FindFirstFileW(strFileW, &fd);
	if ((hFind != INVALID_HANDLE_VALUE) && (FILE_ATTRIBUTE_DIRECTORY == (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)))
	{
		bRet = TRUE;
	}
	FindClose(hFind);
	return bRet;
}


//文件是否存在
BOOL IsAccessibleFile(LPCTSTR lpszFile)
{
	BOOL bExist = TRUE;
	//检查文件是否存在
	WIN32_FIND_DATAW wfd;
	CStringW strFileW = (WCHAR*)_bstr_t(lpszFile);
	HANDLE hFile = FindFirstFileW(strFileW,&wfd);
	if ( hFile == INVALID_HANDLE_VALUE )
	{
		bExist = FALSE;
	}
	FindClose( hFile );
	if ( (FILE_ATTRIBUTE_DIRECTORY & wfd.dwFileAttributes) == FILE_ATTRIBUTE_DIRECTORY)
	{
		bExist = FALSE;
	}
	return bExist;
}


//创建子目录
CString CreateSubDir(LPCTSTR szParentDir, LPCTSTR szChildFolder)
{
	if (NULL == szParentDir || NULL == szChildFolder) return _T("");
	CString str;
	str.Format(_T("%s\\%s"), szParentDir, szChildFolder);
	if (!IsAccessibleDir(str))
	{
		if(!CreateDirectory(str, NULL)) return _T("");
		else if (!IsAccessibleDir(str)) return _T("");
	}
	return str;
}

CString GetCurUserAppDataDir(void)
{
	TCHAR szPath[2048] = {NULL};
	if (!SHGetSpecialFolderPath(NULL,(LPTSTR)szPath,CSIDL_APPDATA,FALSE)) return "";
	return szPath;
}


void DeleteFiles(LPCTSTR lpszPath)
{
	SHFILEOPSTRUCT shFileOp;
	memset(&shFileOp, NULL, sizeof(shFileOp));

	int nLen = _tcslen(lpszPath);
	TCHAR * pSrc = new TCHAR[nLen + 2];
	_tcscpy(pSrc, lpszPath);
	pSrc[nLen] = NULL;
	pSrc[nLen + 1] = NULL; //这点很恶心，结尾要2个NULL字符，否则不正确

	shFileOp.wFunc = FO_DELETE;
	shFileOp.pFrom = pSrc;
	shFileOp.pTo = NULL;
	shFileOp.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR;
	SHFileOperation(&shFileOp);

	delete[] pSrc;
	pSrc = NULL;
}
//
//
////将指定的文件内容转换为字节流，保存在 nDest 中
//BOOL CAnalyzeResultComponentData::ConvertFile2CString(CString& nDest, const CString& strPathfilename)
//{
//	nDest = _T("");	
//	if (strPathfilename.IsEmpty())
//	{
//		return FALSE;
//	}
//
//	//打开文件
//	CFile file;
//	if (!file.Open((LPCTSTR)strPathfilename
//		, CFile::modeRead | CFile::shareDenyNone))
//	{
//		return FALSE;
//	}
//	if (file.m_hFile == CFile::hFileNull)
//	{
//		return FALSE;
//	}
//
//	UINT nSize = (UINT)file.GetLength();
//	char *pBuf = new char[nSize + 2];
//	memset(pBuf, NULL, nSize + 2);
//	file.SeekToBegin();
//	char *p = pBuf;
//	DWORD nNumberOfBytesRead = 0;
//	UINT nTotalRead = 0;
//	while (ReadFile(file.m_hFile, p, 1024 , &nNumberOfBytesRead, NULL) && nNumberOfBytesRead > 0)
//	{
//		p += nNumberOfBytesRead;
//		nTotalRead += nNumberOfBytesRead;
//	}
//	file.Close();
//
//	if (nTotalRead == 0 || p == NULL) 
//	{
//		return FALSE;
//	}
//
//	//base64
//	int nEncodeLen = nTotalRead * 2;
//	char * pEecode = new char[nEncodeLen];
//	memset(pEecode, NULL, nEncodeLen);
//	BOOL bEocode = ATL::Base64Encode((BYTE*)pBuf, nTotalRead, pEecode, &nEncodeLen);
//	stringstream ss(pEecode);
//	string str;
//	if (bEocode)
//	{
//		while(!ss.eof())
//		{
//			ss >> str;
//			nDest += str.c_str();
//		}
//	}
//	//nDest.Remove('\r\n');
//	//nDest.Trim('\r\n');
//
//	delete[] pEecode;
//	delete[] pBuf;
//	return TRUE;
//}
LONGLONG FileSize(CString strFile)
{
	// stat 方式打开大文件和长文件名的文件有问题，fopen_s 打开已经在外部打开的Word文档失败，FindFirstFileW is all Ok!
	WIN32_FIND_DATAW DataFind;
	CStringW strFileW = (WCHAR*)_bstr_t(strFile);
	HANDLE hFind = FindFirstFileW(strFileW,&DataFind);
	if (hFind==INVALID_HANDLE_VALUE)
	{
		ZTools::WriteZToolsFormatLog("FileSize调用%FindFirstFileW返回INVALID_HANDLE_VALUE",strFile);
		return -1;
	}

	if(DataFind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		FindClose(hFind);
		ZTools::WriteZToolsFormatLog("FileSize调用%s判断为文件夹,FileSize将返回-1",strFile);
		return -1;
	}
	
	ULONGLONG lSize = ULONGLONG(DataFind.nFileSizeHigh)<< 32;
	lSize = lSize + DataFind.nFileSizeLow;
	FindClose(hFind);
	ZTools::WriteZToolsFormatLog("FileSize调用%FindFirstFileW返回有效句柄0x%08x,判定为文件,大小为%lld",strFile,hFind,lSize);
	return lSize;
}