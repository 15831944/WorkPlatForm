// FileManage.cpp : CFileManage 的实现

#include "stdafx.h"
#include "FileManage.h"
#include "Tools.h"
#include "ZipTool.h"
#include <fstream>
#include <sstream>
#include <comdef.h>
#include <sys/stat.h>

#include <comdef.h>
std::string ws2s_1(const std::wstring& ws)
{
	_bstr_t t = ws.c_str();
	char* pchar = (char*)t;
	std::string result = pchar;
	return result;
}

std::wstring s2ws_1(const std::string& s)
{
	_bstr_t t = s.c_str();
	wchar_t* pwchar = (wchar_t*)t;
	std::wstring result = pwchar;
	return result;
}

//获取文件的MD5码
std::string GetFileMD5(std::string strFilePath)
{
	CryptoPP::Weak::MD5 md5;
	CryptoPP::HashFilter filter(md5);

	std::auto_ptr<CryptoPP::ChannelSwitch> channelSwitch(new CryptoPP::ChannelSwitch);
	channelSwitch->AddDefaultRoute(filter);
	std::wstring wstr = s2ws_1(strFilePath);
	CryptoPP::FileSource(wstr.c_str(), true, channelSwitch.release());

	std::stringstream result;
	CryptoPP::HexEncoder encoder(new CryptoPP::FileSink(result), false);
	filter.TransferTo(encoder);
	std::string strResult = result.str();
	return strResult;
}

//获取文件长度
long GetFileSize(const char *filename)
{
	struct stat     f_stat;
	if (stat(filename, &f_stat) == -1) {
		return -1;
	}
	return (long)f_stat.st_size;
}

//文件路径是否存在
BOOL IsAccessibleDir(LPCTSTR strPath)
{
	WIN32_FIND_DATA fd;
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

	CString str = strPath;
	str.TrimRight('\\');
	hFind = FindFirstFile(str, &fd);
	if ((hFind != INVALID_HANDLE_VALUE) && (FILE_ATTRIBUTE_DIRECTORY == (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)))
	{
		bRet = TRUE;
	}
	FindClose(hFind);
	return bRet;
}


void ListAllFileInDrectory(std::vector<CString>& vPaths, LPCTSTR lpszFolderPath)
{
	CString strDir = lpszFolderPath == NULL ? "" : lpszFolderPath;
	CString strPath;

	WIN32_FIND_DATA FindFileData;
	HANDLE hFile = NULL;
	TCHAR szFilePath[MAX_PATH] = {NULL};

	if (!IsAccessibleDir(strDir)) return;

	//构造代表子目录和文件夹路径的字符串，使用通配符“*”
	lstrcpy(szFilePath, (LPCTSTR)strDir);
	lstrcat(szFilePath, "\\*");
	//查找第一个文件/目录，获得查找句柄
	hFile = FindFirstFile(szFilePath, &FindFileData);

	if (hFile == INVALID_HANDLE_VALUE) return;

	do
	{
		//    过滤“.”和“..”，不需要遍历
		if (lstrcmp(FindFileData.cFileName, TEXT(".")) == 0 
			|| lstrcmp(FindFileData.cFileName, TEXT("..")) == 0)
		{
			continue;
		}
		//构造成全路径
		//wsprintf(szFullPath, "%s\\%s", szPath, FindFileData.cFileName);
		strPath.Format("%s\\%s", (LPCTSTR)strDir, FindFileData.cFileName);

		//如果是目录，则递归调用，列举下级目录
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			ListAllFileInDrectory(vPaths, (LPCTSTR)strPath);
		}
		else
		{
			vPaths.push_back(strPath);
		}
	}
	while (FindNextFile(hFile, &FindFileData));

	return;
}

DWORD GetAllPathInDir(std::vector<CString>& vPaths, LPCTSTR lpszFolderPath, BOOL bClear = TRUE)
{
	if (bClear) vPaths.clear();
	ListAllFileInDrectory(vPaths, lpszFolderPath);
	return (DWORD)vPaths.size();
}

/*
功能：
	通过弹出对话框，选择目录。
参数：
	lpszTitle：弹出对话框的说明性文字信息，非对话框标题。
	hwndOwner：附属窗口句柄。
返回：
	用户选择的目录字符串。
	例如：如果选择盘符，将返回“C:\”。如果选择目录，将返回“C:\Windows\”
*/
CString GetDirByOpenFileDialog(LPCTSTR lpszTitle, HWND hwndOwner)
{
	//if (hwndOwner == NULL || !IsWindow(hwndOwner)) return "";

	CString strCurDir;  //选择的目录
	HRESULT hRet = S_OK;

	TCHAR szTitle[MAX_PATH] = _T(""); //显示的信息，非对话框标题
	TCHAR szDirName[MAX_PATH] = _T(""); //保存当前选中的目录名称（不是全路径）
	TCHAR szDirFullPath[MAX_PATH * 2] = {NULL}; //保存目录全路径

	LPMALLOC pMalloc = NULL; // 用于释放 每次查询的 pItemIdList 接口
	LPITEMIDLIST pItemIdList = NULL;
	
	if (lpszTitle != NULL)
	{
		strncpy_s(szTitle, lpszTitle, MAX_PATH);
	}

	BROWSEINFO bi;
	memset(&bi, NULL, sizeof(BROWSEINFO));
	bi.hwndOwner = hwndOwner;
	bi.pszDisplayName = szDirName;
	bi.lpszTitle = szTitle;
	bi.ulFlags = BIF_NEWDIALOGSTYLE;

	hRet = ::OleInitialize(NULL);
	if (hRet != S_OK //初始化成功
		&& hRet != S_FALSE) //已经初始化
	{
		//AfxMessageBox("调用 OleInitialize 初始化 OLE 库 失败");
		return "";
	}

	InitCommonControls();
	pItemIdList = SHBrowseForFolder(&bi);
	if (pItemIdList != NULL)
	{
		if (SHGetPathFromIDList(pItemIdList, szDirFullPath))
		{
			strCurDir = szDirFullPath;
			if(!strCurDir.IsEmpty() && strCurDir.GetAt(strCurDir.GetLength() - 1) != '\\')
			{
				strCurDir += "\\";
			}
		}

		if (SUCCEEDED(SHGetMalloc(&pMalloc)))
		{
			pMalloc->Free(pItemIdList);
			pMalloc->Release();
		}
	}

	::OleUninitialize();

	return strCurDir;
}

CString ConvertVector2JsonStr( std::vector<CString>& vSrc )
{
	CString strJson;
	Json::Value root;
	Json::FastWriter fast_writer;

	try
	{
		for (int i=0; i<(int)vSrc.size(); i++)
		{
			root.append((LPCTSTR)vSrc[i]);
		}

		strJson = fast_writer.write(root).c_str();
		return strJson;
	}
	catch (...)
	{
		return "";
	}
}


// CFileManage

std::string CFileManage::GetFileName(std::string strPath)
{
	if (strPath.empty())
	{
		return "";
	}

	//替换斜杠
	std::replace(strPath.begin(), strPath.end(), '/', '\\');

	if (strPath.find('\\') == std::string::npos)
	{
		return strPath;
	}

	std::string::size_type nPos = strPath.find_last_of('\\') + 1;
	std::string strFileName = strPath.substr(nPos, strPath.length() - nPos);

	return strFileName;
}

std::string CFileManage::GetFileDir(std::string strPath)
{
	if (strPath.empty())
	{
		return strPath;
	}

	//替换斜杠
	std::replace(strPath.begin(), strPath.end(), '/', '\\');

	if (strPath.find('\\') == std::string::npos)
	{
		return "";
	}

	std::string::size_type nPos = strPath.find_last_of('\\');
	std::string strDir = strPath.substr(0, nPos + 1);

	return strDir;
}

BOOL CFileManage::IsEqualNoCase(std::string strSrc, std::string strDst)
{
	ZTools::LowerString(strSrc);
	ZTools::LowerString(strDst);
	if (strSrc.compare(strDst) == 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

std::string CFileManage::GetLastErrorMsg()
{
	LPVOID lpMsgBuf;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		0, // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
		);
	std::string strResult = ZTools::FormatString("%s", lpMsgBuf);
	LocalFree(lpMsgBuf);
	return strResult;
}

BOOL CFileManage::CheckFilePath(std::string strPath)
{
	if (strPath.empty())
	{
		ZTools::WriteZToolsFormatLog("文件路径为空");
		return FALSE;
	}

	//替换斜杠
	std::replace(strPath.begin(), strPath.end(), '/', '\\');

	if (strPath.find(':') == std::string::npos || strPath.find('\\') == std::string::npos)
	{
		ZTools::WriteZToolsFormatLog("文件路径不合法:%s", strPath.c_str());
		return FALSE;
	}
	return TRUE;
}

BOOL CFileManage::CheckDirPath(std::string strPath)
{
	if (strPath.empty())
	{
		ZTools::WriteZToolsFormatLog("目录路径为空");
		return FALSE;
	}

	//替换斜杠
	std::replace(strPath.begin(), strPath.end(), '/', '\\');

	if (strPath.find(':') == std::string::npos || strPath.find_last_of('\\') != strPath.length() - 1)
	{
		ZTools::WriteZToolsFormatLog("目录路径不合法:%s", strPath.c_str());
		return FALSE;
	}
	return TRUE;
}

STDMETHODIMP CFileManage::SaveFile(BSTR bstrFile, BSTR bstrContent, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::SaveFile");
	CComVariant vRet(false);

	// TODO: 在此添加实现代码
	std::string strFile = _bstr_t(bstrFile);
	ZTools::WriteZToolsFormatLog("strFile:%s", strFile.c_str());
	std::string strContent = _bstr_t(bstrContent);
	//ZTools::WriteZToolsFormatLog("strContent:%s", strContent.c_str());

	//目前只在建模页面写入xpdl文件时用到，默认存为utf8格式，如需ascii，另提供接口
	ZTools::MBToUTF8(strContent);
	
	//替换斜杠
	std::replace(strFile.begin(), strFile.end(), '/', '\\');

	//校验路径
	if (!CheckFilePath(strFile))
	{
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	std::string strDir = GetFileDir(strFile);
	if (!MakeSureDirectoryPathExists(strDir.c_str()))
	{
		ZTools::WriteZToolsFormatLog("创建文件夹失败:%s", strDir.c_str());
		//ZTools::WriteZToolsLog(GetLastErrorMsg());
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	//std::locale::global(std::locale(""));
	std::ofstream fOut(strFile);
	if (fOut == NULL || fOut.is_open() == false)
	{
		ZTools::WriteZToolsFormatLog("写入文件 %s 失败", strFile.c_str());
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}
	fOut << strContent;
	fOut.close();

	vRet = true;
	vRet.Detach(pvRet);
	return S_OK;
}

STDMETHODIMP CFileManage::OpenFile(BSTR bstrFile, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::OpenFile");
	CComVariant vRet(_bstr_t("").GetBSTR());

	// TODO: 在此添加实现代码
	std::string strFile = _bstr_t(bstrFile);
	ZTools::WriteZToolsFormatLog("strFile:%s", strFile.c_str());

	//替换斜杠
	std::replace(strFile.begin(), strFile.end(), '/', '\\');

	//校验路径
	if (!CheckFilePath(strFile))
	{
		vRet = _bstr_t("").GetBSTR();
		vRet.Detach(pvRet);
		return S_OK;
	}
	
	//std::locale::global(std::locale(""));
	std::ifstream fIn(strFile);
	if (fIn == NULL || fIn.is_open() == false)
	{
		ZTools::WriteZToolsFormatLog("读取文件 %s 失败", strFile.c_str());
		vRet = _bstr_t("").GetBSTR();
		vRet.Detach(pvRet);
		return S_OK;
	}

	std::ostringstream ssIn;
	ssIn << fIn.rdbuf();
	fIn.close();

	std::string strResult = ssIn.str();

	//目前只在建模页面读取xpdl文件时用到，默认存为utf8格式，如需ascii，另提供接口
	ZTools::UTF8ToMB(strResult);

	vRet = _bstr_t(strResult.c_str()).GetBSTR();
	vRet.Detach(pvRet);
	return S_OK;
}

STDMETHODIMP CFileManage::SaveFileDefault(BSTR bstrFile, BSTR bstrContent, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::SaveFileDefault");
	CComVariant vRet(false);

	// TODO: 在此添加实现代码
	std::string strFile = _bstr_t(bstrFile);
	ZTools::WriteZToolsFormatLog("strFile:%s", strFile.c_str());
	std::string strContent = _bstr_t(bstrContent);
	//ZTools::WriteZToolsFormatLog("strContent:%s", strContent.c_str());

	//替换斜杠
	std::replace(strFile.begin(), strFile.end(), '/', '\\');

	//校验路径
	if (!CheckFilePath(strFile))
	{
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	std::string strDir = GetFileDir(strFile);
	if (!MakeSureDirectoryPathExists(strDir.c_str()))
	{
		ZTools::WriteZToolsFormatLog("创建文件夹失败:%s", strDir.c_str());
		//ZTools::WriteZToolsLog(GetLastErrorMsg());
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	//std::locale::global(std::locale(""));
	std::ofstream fOut(strFile);
	if (fOut == NULL || fOut.is_open() == false)
	{
		ZTools::WriteZToolsFormatLog("写入文件 %s 失败", strFile.c_str());
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}
	fOut << strContent;
	fOut.close();

	vRet = true;
	vRet.Detach(pvRet);
	return S_OK;
}

STDMETHODIMP CFileManage::OpenFileDefault(BSTR bstrFile, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::OpenFileDefault");
	CComVariant vRet(_bstr_t("").GetBSTR());

	// TODO: 在此添加实现代码
	std::string strFile = _bstr_t(bstrFile);
	ZTools::WriteZToolsFormatLog("strFile:%s", strFile.c_str());

	//替换斜杠
	std::replace(strFile.begin(), strFile.end(), '/', '\\');

	//校验路径
	if (!CheckFilePath(strFile))
	{
		vRet = _bstr_t("").GetBSTR();
		vRet.Detach(pvRet);
		return S_OK;
	}

	//std::locale::global(std::locale(""));
	std::ifstream fIn(strFile);
	if (fIn == NULL || fIn.is_open() == false)
	{
		ZTools::WriteZToolsFormatLog("读取文件 %s 失败", strFile.c_str());
		vRet = _bstr_t("").GetBSTR();
		vRet.Detach(pvRet);
		return S_OK;
	}

	std::ostringstream ssIn;
	ssIn << fIn.rdbuf();
	fIn.close();

	std::string strResult = ssIn.str();

	vRet = _bstr_t(strResult.c_str()).GetBSTR();
	vRet.Detach(pvRet);
	return S_OK;
}

STDMETHODIMP CFileManage::CopyFile(BSTR bstrSrcFile, BSTR bstrDstFile, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::CopyFile");
	CComVariant vRet(false);

	// TODO: 在此添加实现代码
	std::string strSrcFile = _bstr_t(bstrSrcFile);
	ZTools::WriteZToolsFormatLog("strSrcFile:%s", strSrcFile.c_str());
	std::string strDstFile = _bstr_t(bstrDstFile);
	ZTools::WriteZToolsFormatLog("strDstFile:%s", strDstFile.c_str());

	//替换斜杠
	std::replace(strSrcFile.begin(), strSrcFile.end(), '/', '\\');
	std::replace(strDstFile.begin(), strDstFile.end(), '/', '\\');

	//校验路径
	if (!CheckFilePath(strSrcFile) || !CheckFilePath(strDstFile))
	{
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	//源文件是否存在
	if (!PathFileExists(strSrcFile.c_str()))
	{
		ZTools::WriteZToolsFormatLog("文件不存在：%s", strSrcFile.c_str());
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	//源目的是否相同
	if (IsEqualNoCase(strSrcFile, strDstFile) == TRUE)
	{
		ZTools::WriteZToolsFormatLog("源与目的路径相同：%s -- %s", strSrcFile.c_str(), strDstFile.c_str());
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	//目的路径是否存在
	if (!MakeSureDirectoryPathExists(strDstFile.c_str()))
	{
		ZTools::WriteZToolsFormatLog("创建文件夹失败:%s", strDstFile.c_str());
		//ZTools::WriteZToolsLog(GetLastErrorMsg());
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	if (!::CopyFile(strSrcFile.c_str(), strDstFile.c_str(), FALSE))
	{
		ZTools::WriteZToolsFormatLog("拷贝文件失败：%s -- %s", strSrcFile.c_str(), strDstFile.c_str());
		//ZTools::WriteZToolsLog(GetLastErrorMsg());
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	vRet = true;
	vRet.Detach(pvRet);
	return S_OK;
}

STDMETHODIMP CFileManage::CopyDir(BSTR bstrSrcDir, BSTR bstrDstDir, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::CopyDir");
	CComVariant vRet(false);

	// TODO: 在此添加实现代码
	std::string strSrcDir = _bstr_t(bstrSrcDir);
	ZTools::WriteZToolsFormatLog("strSrcDir:%s", strSrcDir.c_str());
	std::string strDstDir = _bstr_t(bstrDstDir);
	ZTools::WriteZToolsFormatLog("strDstDir:%s", strDstDir.c_str());

	//替换斜杠
	std::replace(strSrcDir.begin(), strSrcDir.end(), '/', '\\');
	std::replace(strDstDir.begin(), strDstDir.end(), '/', '\\');

	//校验路径
	if (!CheckDirPath(strSrcDir) || !CheckDirPath(strDstDir))
	{
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	//源文件是否存在
	if (!PathFileExists(strSrcDir.c_str()))
	{
		ZTools::WriteZToolsFormatLog("文件目录不存在：%s", strSrcDir.c_str());
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	//源目的是否相同
	if (IsEqualNoCase(strSrcDir, strDstDir) == TRUE)
	{
		ZTools::WriteZToolsFormatLog("源与目的路径相同：%s -- %s", strSrcDir.c_str(), strDstDir.c_str());
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	//目的路径是否存在
	if (!MakeSureDirectoryPathExists(strDstDir.c_str()))
	{
		ZTools::WriteZToolsFormatLog("创建文件夹失败:%s", strDstDir.c_str());
		//ZTools::WriteZToolsLog(GetLastErrorMsg());
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	strSrcDir += "*";
	int nSrcDir = strSrcDir.length() + 2;
	char* psSrcDir = new char[nSrcDir];
	memset(psSrcDir, 0, nSrcDir);
	memcpy(psSrcDir, strSrcDir.c_str(), strSrcDir.length());

	int nDstDir = strDstDir.length() + 2;
	char* psDstDir = new char[nDstDir];
	memset(psDstDir, 0, nDstDir);
	memcpy(psDstDir, strDstDir.c_str(), strDstDir.length());

	SHFILEOPSTRUCT FileOp; 
	ZeroMemory((void*)&FileOp,sizeof(SHFILEOPSTRUCT));

	FileOp.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR ; 
	FileOp.hNameMappings = NULL; 
	FileOp.hwnd = NULL; 
	FileOp.lpszProgressTitle = NULL; 
	FileOp.pFrom = psSrcDir;
	FileOp.pTo = psDstDir;
	FileOp.wFunc = FO_COPY; 

	int nRet = SHFileOperation(&FileOp);
	
	delete[] psSrcDir;
	delete[] psDstDir;

	if (nRet != 0)
	{
		ZTools::WriteZToolsFormatLog("拷贝文件夹失败:%s -- %s -- 0x%x", strSrcDir.c_str(), strDstDir.c_str(), nRet);
		//ZTools::WriteZToolsLog(GetLastErrorMsg());
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	vRet = true;
	vRet.Detach(pvRet);
	return S_OK;
}

STDMETHODIMP CFileManage::CreateDir(BSTR bstrDir, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::CreateDir");
	CComVariant vRet(false);

	std::string strDir = _bstr_t(bstrDir);
	ZTools::WriteZToolsFormatLog("strDir:%s", strDir.c_str());

	//替换斜杠
	std::replace(strDir.begin(), strDir.end(), '/', '\\');

	//校验路径
	if (!CheckDirPath(strDir))
	{
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	if (!MakeSureDirectoryPathExists(strDir.c_str()))
	{
		ZTools::WriteZToolsFormatLog("创建文件夹失败:%s", strDir.c_str());
		//ZTools::WriteZToolsLog(GetLastErrorMsg());
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	vRet = true;
	vRet.Detach(pvRet);
	return S_OK;
}

STDMETHODIMP CFileManage::DeleteFile(BSTR bstrFile, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::DeleteFile");
	CComVariant vRet(false);

	// TODO: 在此添加实现代码
	std::string strFile = _bstr_t(bstrFile);
	ZTools::WriteZToolsFormatLog("strFile:%s", strFile.c_str());

	//替换斜杠
	std::replace(strFile.begin(), strFile.end(), '/', '\\');

	//校验路径
	if (!CheckFilePath(strFile))
	{
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	//源文件是否存在
	if (!PathFileExists(strFile.c_str()))
	{
		ZTools::WriteZToolsFormatLog("文件不存在：%s", strFile.c_str());
		vRet = true;//删除时若不存在认为删除成功
		vRet.Detach(pvRet);
		return S_OK;
	}
	
	if (!::DeleteFile(strFile.c_str()))
	{
		ZTools::WriteZToolsFormatLog("删除文件失败：%s", strFile.c_str());
		//ZTools::WriteZToolsLog(GetLastErrorMsg());
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	vRet = true;
	vRet.Detach(pvRet);
	return S_OK;
}

STDMETHODIMP CFileManage::DeleteDir(BSTR bstrDir, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::DeleteDir");
	CComVariant vRet(false);

	// TODO: 在此添加实现代码
	std::string strDir = _bstr_t(bstrDir);
	ZTools::WriteZToolsFormatLog("strDir:%s", strDir.c_str());

	//替换斜杠
	std::replace(strDir.begin(), strDir.end(), '/', '\\');

	//校验路径
	if (!CheckDirPath(strDir))
	{
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	//源文件是否存在
	if (!PathFileExists(strDir.c_str()))
	{
		ZTools::WriteZToolsFormatLog("文件目录不存在：%s", strDir.c_str());
		vRet = true;
		vRet.Detach(pvRet);
		return S_OK;
	}

	strDir += "*";
	int nDir = strDir.length() + 2;
	char* psDir = new char[nDir];
	memset(psDir, 0, nDir);
	memcpy(psDir, strDir.c_str(), strDir.length());

	SHFILEOPSTRUCT FileOp; 
	ZeroMemory((void*)&FileOp,sizeof(SHFILEOPSTRUCT));

	FileOp.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR ; 
	FileOp.hNameMappings = NULL; 
	FileOp.hwnd = NULL; 
	FileOp.lpszProgressTitle = NULL; 
	FileOp.pFrom = psDir;
	FileOp.pTo = NULL;
	FileOp.wFunc = FO_DELETE; 

	int nRet = SHFileOperation(&FileOp);
	
	delete[] psDir;

	if (nRet != 0)
	{
		ZTools::WriteZToolsFormatLog("删除文件夹失败:%s -- 0x%x", strDir.c_str(), nRet);
		//ZTools::WriteZToolsLog(GetLastErrorMsg());
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	strDir = strDir.substr(0, strDir.find_last_of('\\') + 1);
	if (!::RemoveDirectory(strDir.c_str()))
	{
		ZTools::WriteZToolsFormatLog("删除文件夹失败:%s", strDir.c_str(), nRet);
		//ZTools::WriteZToolsLog(GetLastErrorMsg());
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}	

	vRet = true;
	vRet.Detach(pvRet);
	return S_OK;
}

STDMETHODIMP CFileManage::SelectFile(VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::SelectFile");
	CComVariant vRet(_bstr_t("").GetBSTR());

	CFileDialog dlg(true);
	dlg.m_ofn.FlagsEx = OFN_EX_NOPLACESBAR;
	dlg.m_ofn.lpstrFilter = "所有文件(*.*)\0*.*\0\0";

	if (dlg.DoModal() == IDOK)
	{
		CString sPathName = dlg.GetPathName();
		vRet = _bstr_t(sPathName).GetBSTR();
	}

	vRet.Detach(pvRet);
	return S_OK;
}

STDMETHODIMP CFileManage::SelectDir(VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::SelectDir");
	CComVariant vRet(_bstr_t("").GetBSTR());

	CWnd * pWnd = AfxGetMainWnd();
	CString strDir = GetDirByOpenFileDialog("请选择目录", pWnd->GetSafeHwnd());
	vRet = _bstr_t(strDir).GetBSTR();

	vRet.Detach(pvRet);
	return S_OK;
}

STDMETHODIMP CFileManage::GetFileList(BSTR bstrDir, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::GetFileList");
	CComVariant vRet(_bstr_t("").GetBSTR());

	std::string strDir = _bstr_t(bstrDir);
	ZTools::WriteZToolsFormatLog("strDir:%s", strDir.c_str());

	//替换斜杠
	std::replace(strDir.begin(), strDir.end(), '/', '\\');

	//校验路径
	if (!CheckDirPath(strDir))
	{
		vRet = _bstr_t("").GetBSTR();
		vRet.Detach(pvRet);
		return S_OK;
	}

	strDir = strDir.substr(0, strDir.find_last_not_of('\\') + 1);

	CString sResult;
	std::vector<CString> vPaths;
	if (!strDir.empty())
	{
		GetAllPathInDir(vPaths, strDir.c_str());
	}
	if (vPaths.size() > 0)
	{
		sResult = ConvertVector2JsonStr(vPaths);
	}

	vRet = _bstr_t(sResult).GetBSTR();
	vRet.Detach(pvRet);
	return S_OK;
}

//清空文件夹
STDMETHODIMP CFileManage::ClearDir(BSTR bstrDir, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::ClearDir");
	CComVariant vRet(false);

	// TODO: 在此添加实现代码
	std::string strDir = _bstr_t(bstrDir);
	ZTools::WriteZToolsFormatLog("strDir:%s", strDir.c_str());

	//替换斜杠
	std::replace(strDir.begin(), strDir.end(), '/', '\\');

	//校验路径
	if (!CheckDirPath(strDir))
	{
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	//源文件是否存在
	if (!PathFileExists(strDir.c_str()))
	{
		ZTools::WriteZToolsFormatLog("文件目录不存在：%s", strDir.c_str());
		vRet = true;
		vRet.Detach(pvRet);
		return S_OK;
	}

	strDir += "*";
	int nDir = strDir.length() + 2;
	char* psDir = new char[nDir];
	memset(psDir, 0, nDir);
	memcpy(psDir, strDir.c_str(), strDir.length());

	SHFILEOPSTRUCT FileOp; 
	ZeroMemory((void*)&FileOp,sizeof(SHFILEOPSTRUCT));

	FileOp.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR ; 
	FileOp.hNameMappings = NULL; 
	FileOp.hwnd = NULL; 
	FileOp.lpszProgressTitle = NULL; 
	FileOp.pFrom = psDir;
	FileOp.pTo = NULL;
	FileOp.wFunc = FO_DELETE; 

	int nRet = SHFileOperation(&FileOp);
	
	delete[] psDir;

	if (nRet != 0)
	{
		ZTools::WriteZToolsFormatLog("清空文件夹失败:%s -- 0x%x", strDir.c_str(), nRet);
		//ZTools::WriteZToolsLog(GetLastErrorMsg());
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	vRet = true;
	vRet.Detach(pvRet);
	return S_OK;
}

//重命名文件
STDMETHODIMP CFileManage::RenameFile(BSTR bstrOldName, BSTR bstrNewName, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::RenameFile");
	CComVariant vRet(false);

	// TODO: 在此添加实现代码
	std::string strFile = _bstr_t(bstrOldName);
	std::string strFileDst = _bstr_t(bstrNewName);
	ZTools::WriteZToolsFormatLog("strFile:%s", strFile.c_str());
	ZTools::WriteZToolsFormatLog("strFileDst:%s", strFileDst.c_str());

	//替换斜杠
	std::replace(strFile.begin(), strFile.end(), '/', '\\');
	std::replace(strFileDst.begin(), strFileDst.end(), '/', '\\');

	//校验路径
	if (!CheckFilePath(strFile) || !CheckFilePath(strFileDst))
	{
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	//源文件是否存在
	if (!PathFileExists(strFile.c_str()))
	{
		ZTools::WriteZToolsFormatLog("文件不存在：%s", strFile.c_str());
		vRet = false;//重命名时若不存在认为重命名失败
		vRet.Detach(pvRet);
		return S_OK;
	}

	if (::rename(strFile.c_str(), strFileDst.c_str()) == -1)
	{
		ZTools::WriteZToolsFormatLog("重命名文件失败：%s", strFile.c_str());
		//ZTools::WriteZToolsLog(GetLastErrorMsg());
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	vRet = true;
	vRet.Detach(pvRet);
	return S_OK;
}

//移动文件
STDMETHODIMP CFileManage::MoveFile(BSTR srcFilePath, BSTR dstFilePath, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加实现代码
	//RenameFile(srcFilePath, dstFilePath, pvRet);  //处理与重命名方法基本一致，直接调用

	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::MoveFile");
	CComVariant vRet(false);

	// TODO: 在此添加实现代码
	std::string strFile = _bstr_t(srcFilePath);
	std::string strFileDst = _bstr_t(dstFilePath);
	ZTools::WriteZToolsFormatLog("strFile:%s", strFile.c_str());
	ZTools::WriteZToolsFormatLog("strFileDst:%s", strFileDst.c_str());

	//替换斜杠
	std::replace(strFile.begin(), strFile.end(), '/', '\\');
	std::replace(strFileDst.begin(), strFileDst.end(), '/', '\\');

	//校验路径
	if (!CheckFilePath(strFile) || !CheckFilePath(strFileDst))
	{
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	//源文件是否存在
	if (!PathFileExists(strFile.c_str()))
	{
		ZTools::WriteZToolsFormatLog("文件不存在：%s", strFile.c_str());
		vRet = false;//移动时若不存在认为移动失败
		vRet.Detach(pvRet);
		return S_OK;
	}

	if (::rename(strFile.c_str(), strFileDst.c_str()) == -1)
	{
		ZTools::WriteZToolsFormatLog("移动文件失败：%s", strFile.c_str());
		//ZTools::WriteZToolsLog(GetLastErrorMsg());
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	vRet = true;
	vRet.Detach(pvRet);

	return S_OK;
}

//移动文件夹
STDMETHODIMP CFileManage::MoveDir(BSTR srcDir, BSTR dstDir, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::MoveDir");
	CComVariant vRet(false);

	std::string strDir = _bstr_t(srcDir);
	std::string strDirDst = _bstr_t(dstDir);
	ZTools::WriteZToolsFormatLog("strDir%s", strDir.c_str());
	ZTools::WriteZToolsFormatLog("strDirDst:%s", strDirDst.c_str());

	//替换斜杠
	std::replace(strDir.begin(), strDir.end(), '/', '\\');
	std::replace(strDirDst.begin(), strDirDst.end(), '/', '\\');

	//校验路径
	if (!CheckDirPath(strDir) || !CheckDirPath(strDirDst))
	{
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	//源文件目录是否存在
	if (!PathFileExists(strDir.c_str()))
	{
		ZTools::WriteZToolsFormatLog("文件目录不存在：%s", strDir.c_str());
		vRet = false;//移动时若不存在认为移动失败
		vRet.Detach(pvRet);
		return S_OK;
	}

	int nDir = strDir.length() + 2;
	char* psDir = new char[nDir];
	memset(psDir, 0, nDir);
	memcpy(psDir, strDir.c_str(), strDir.length());

	int nDirDst = strDirDst.length() + 2;
	char* psDirDst = new char[nDirDst];
	memset(psDirDst, 0, nDirDst);
	memcpy(psDirDst, strDirDst.c_str(), strDirDst.length());

	SHFILEOPSTRUCT FileOp; 
	ZeroMemory((void*)&FileOp,sizeof(SHFILEOPSTRUCT));

	FileOp.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR ; 
	FileOp.hNameMappings = NULL; 
	FileOp.hwnd = NULL; 
	FileOp.lpszProgressTitle = NULL; 
	FileOp.pFrom = psDir;
	FileOp.pTo = psDirDst;
	FileOp.wFunc = FO_MOVE; 

	int nRet = SHFileOperation(&FileOp);
	
	delete[] psDir;
	delete[] psDirDst;

	if (nRet != 0)
	{
		ZTools::WriteZToolsFormatLog("移动文件目录失败：%s", strDir.c_str());
		//ZTools::WriteZToolsLog(GetLastErrorMsg());
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	vRet = true;
	vRet.Detach(pvRet);

	return S_OK;
}

//比较文件
STDMETHODIMP CFileManage::CompareFile(BSTR srcFilePath, BSTR dstFilePath, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::CompareFile");
	CComVariant vRet(false);

	// TODO: 在此添加实现代码
	std::string strFile = _bstr_t(srcFilePath);
	std::string strFileDst = _bstr_t(dstFilePath);
	ZTools::WriteZToolsFormatLog("strFile:%s", strFile.c_str());
	ZTools::WriteZToolsFormatLog("strFileDst:%s", strFileDst.c_str());

	//替换斜杠
	std::replace(strFile.begin(), strFile.end(), '/', '\\');
	std::replace(strFileDst.begin(), strFileDst.end(), '/', '\\');

	//校验路径
	if (!CheckFilePath(strFile) || !CheckFilePath(strFileDst))
	{
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	//源文件是否存在
	if (!PathFileExists(strFile.c_str()) || !PathFileExists(strFileDst.c_str()))
	{
		if(!PathFileExists(strFile.c_str())) ZTools::WriteZToolsFormatLog("文件不存在：%s", strFile.c_str());
		if(!PathFileExists(strFileDst.c_str())) ZTools::WriteZToolsFormatLog("文件不存在：%s", strFileDst.c_str());
		vRet = false;//比较时若不存在认为比较失败
		vRet.Detach(pvRet);
		return S_OK;
	}

	if (GetFileSize(strFile.c_str()) != GetFileSize(strFileDst.c_str()))
	{//文件长度是否相同
		ZTools::WriteZToolsFormatLog("文件长度不同：%s", strFile.c_str());
		ZTools::WriteZToolsFormatLog("文件长度不同：%s", strFileDst.c_str());
		//ZTools::WriteZToolsLog(GetLastErrorMsg());
		vRet = false;
		//vRet = "文件长度不同";
		vRet.Detach(pvRet);
		return S_OK;
	}

	if (GetFileMD5(strFile) != GetFileMD5(strFileDst))
	{//文件内容校验
		ZTools::WriteZToolsFormatLog("比较文件失败：%s", strFile.c_str());
		ZTools::WriteZToolsFormatLog("比较文件失败：%s", strFileDst.c_str());
		//ZTools::WriteZToolsLog(GetLastErrorMsg());
		vRet = false;
		//vRet = "比较文件失败";
		vRet.Detach(pvRet);
		return S_OK;
	}

	vRet = true;
	vRet.Detach(pvRet);

	return S_OK;
}

//获取文件内容
STDMETHODIMP CFileManage::GetContentFromZip(BSTR bstrZip, BSTR bstrFile, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::GetContentFromZip");
	CComVariant vRet(_bstr_t("").GetBSTR());

	// TODO: 在此添加实现代码
	std::string strZip = _bstr_t(bstrZip);
	ZTools::WriteZToolsFormatLog("strZip:%s", strZip.c_str());
	std::string strFile = _bstr_t(bstrFile);
	ZTools::WriteZToolsFormatLog("strFile:%s", strFile.c_str());

	vRet = _bstr_t(ZipTool::GetContentFromZip(strZip, strFile).c_str()).GetBSTR();
	vRet.Detach(pvRet);

	return S_OK;
}

// 解压一个文件到某处
STDMETHODIMP CFileManage::UnzipFile(BSTR bstrZip, BSTR bstrFileSrc, BSTR bstrFileDst, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加实现代码
	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::UnzipFile");
	CComVariant vRet(false);

	std::string strZip = _bstr_t(bstrZip);
	ZTools::WriteZToolsFormatLog("strZip:%s", strZip.c_str());
	std::string strFileSrc = _bstr_t(bstrFileSrc);
	ZTools::WriteZToolsFormatLog("strFileSrc:%s", strFileSrc.c_str());
	std::string strFileDst = _bstr_t(bstrFileDst);
	ZTools::WriteZToolsFormatLog("strFileDst:%s", strFileDst.c_str());

	vRet = ZipTool::UnzipFile(strZip, strFileSrc, strFileDst);
	if (vRet.boolVal == false)
	{
		ZTools::WriteZToolsLog("解压文件出现异常");
	}
	vRet.Detach(pvRet);

	return S_OK;
}

// 解压整个包到某文件夹
STDMETHODIMP CFileManage::Unzip(BSTR bstrZip, BSTR bstrDir, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加实现代码
	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::Unzip");
	CComVariant vRet(false);

	std::string strZip = _bstr_t(bstrZip);
	ZTools::WriteZToolsFormatLog("strZip:%s", strZip.c_str());
	std::string strDir = _bstr_t(bstrDir);
	ZTools::WriteZToolsFormatLog("strDir:%s", strDir.c_str());

	vRet = ZipTool::Unzip(strZip, strDir);
	if (vRet.boolVal == false)
	{
		ZTools::WriteZToolsLog("解压文件出现异常");
	}
	vRet.Detach(pvRet);

	return S_OK;
}

//解压包中的某个文件到指定目录
STDMETHODIMP CFileManage::UnzipDir(BSTR bstrZip, BSTR bstrDir, BSTR bstrDirDst, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加实现代码
	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::UnzipDir");
	CComVariant vRet(false);

	std::string strZip = _bstr_t(bstrZip);
	ZTools::WriteZToolsFormatLog("strZip:%s", strZip.c_str());
	std::string strDir = _bstr_t(bstrDir);
	ZTools::WriteZToolsFormatLog("strDir:%s", strDir.c_str());
	std::string strDirDst = _bstr_t(bstrDirDst);
	ZTools::WriteZToolsFormatLog("strDirDst:%s", strDirDst.c_str());

	vRet = ZipTool::UnzipDir(strZip, strDir, strDirDst);
	if (vRet.boolVal == false)
	{
		ZTools::WriteZToolsLog("解压文件出现异常");
	}
	vRet.Detach(pvRet);

	return S_OK;
}

// 从指定目录创建ZIP文件
STDMETHODIMP CFileManage::ZipFromDir(BSTR bstrZip, BSTR bstrDir, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加实现代码
	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::ZipFromDir");
	CComVariant vRet(false);

	std::string strZip = _bstr_t(bstrZip);
	ZTools::WriteZToolsFormatLog("strZip:%s", strZip.c_str());
	std::string strDir = _bstr_t(bstrDir);
	ZTools::WriteZToolsFormatLog("strDir:%s", strDir.c_str());

	vRet = ZipTool::ZipFromDir(strZip, strDir, ZipTool::STORE);
	vRet.Detach(pvRet);

	return S_OK;
}

//从JsonString指定列表创建ZIP
STDMETHODIMP CFileManage::ZipFromJsonString(BSTR bstrZip, BSTR bstrJson, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加实现代码
	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::ZipFromJsonString");
	CComVariant vRet(false);

	std::string strZip = _bstr_t(bstrZip);
	ZTools::WriteZToolsFormatLog("strZip:%s", strZip.c_str());
	std::string strJson = _bstr_t(bstrJson);
	ZTools::WriteZToolsFormatLog("strJson:%s", strJson.c_str());

	vRet = ZipTool::ZipFromJsonString(strZip, strJson, ZipTool::STORE);
	vRet.Detach(pvRet);

	return S_OK;
}

//获取以UTF8编码文件的内容
STDMETHODIMP CFileManage::GetUtf8ContentFromZip(BSTR bstrZip, BSTR bstrFile, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::GetUtf8ContentFromZip");
	CComVariant vRet(_bstr_t("").GetBSTR());

	// TODO: 在此添加实现代码
	std::string strZip = _bstr_t(bstrZip);
	ZTools::WriteZToolsFormatLog("strZip:%s", strZip.c_str());
	std::string strFile = _bstr_t(bstrFile);
	ZTools::WriteZToolsFormatLog("strFile:%s", strFile.c_str());

	std::string strContent = ZipTool::GetContentFromZip(strZip, strFile);
	ZTools::UTF8ToMB(strContent); //转成UTF8格式

	vRet = _bstr_t(strContent.c_str()).GetBSTR();
	vRet.Detach(pvRet);

	return S_OK;
}


STDMETHODIMP CFileManage::ZipFromDirDeflate(BSTR bstrZip, BSTR bstrDir, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加实现代码
	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::ZipFromDirDeflate");
	CComVariant vRet(false);

	std::string strZip = _bstr_t(bstrZip);
	ZTools::WriteZToolsFormatLog("strZip:%s", strZip.c_str());
	std::string strDir = _bstr_t(bstrDir);
	ZTools::WriteZToolsFormatLog("strDir:%s", strDir.c_str());

	vRet = ZipTool::ZipFromDir(strZip, strDir, ZipTool::DEFLATE);
	vRet.Detach(pvRet);

	return S_OK;
}


STDMETHODIMP CFileManage::ZipFromJsonStringDeflate(BSTR bstrZip, BSTR bstrJson, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加实现代码
	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::ZipFromJsonStringDeflate");
	CComVariant vRet(false);

	std::string strZip = _bstr_t(bstrZip);
	ZTools::WriteZToolsFormatLog("strZip:%s", strZip.c_str());
	std::string strJson = _bstr_t(bstrJson);
	ZTools::WriteZToolsFormatLog("strJson:%s", strJson.c_str());

	vRet = ZipTool::ZipFromJsonString(strZip, strJson, ZipTool::DEFLATE);
	vRet.Detach(pvRet);

	return S_OK;
}

CFileManage::CFileManage()
{
	ZTools::InitZToolsLog("PeraFileManage", "PeraFileManage.dll");
}
