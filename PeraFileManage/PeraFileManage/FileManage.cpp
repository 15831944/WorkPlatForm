// FileManage.cpp : CFileManage ��ʵ��

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

//��ȡ�ļ���MD5��
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

//��ȡ�ļ�����
long GetFileSize(const char *filename)
{
	struct stat     f_stat;
	if (stat(filename, &f_stat) == -1) {
		return -1;
	}
	return (long)f_stat.st_size;
}

//�ļ�·���Ƿ����
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

	//���������Ŀ¼���ļ���·�����ַ�����ʹ��ͨ�����*��
	lstrcpy(szFilePath, (LPCTSTR)strDir);
	lstrcat(szFilePath, "\\*");
	//���ҵ�һ���ļ�/Ŀ¼����ò��Ҿ��
	hFile = FindFirstFile(szFilePath, &FindFileData);

	if (hFile == INVALID_HANDLE_VALUE) return;

	do
	{
		//    ���ˡ�.���͡�..��������Ҫ����
		if (lstrcmp(FindFileData.cFileName, TEXT(".")) == 0 
			|| lstrcmp(FindFileData.cFileName, TEXT("..")) == 0)
		{
			continue;
		}
		//�����ȫ·��
		//wsprintf(szFullPath, "%s\\%s", szPath, FindFileData.cFileName);
		strPath.Format("%s\\%s", (LPCTSTR)strDir, FindFileData.cFileName);

		//�����Ŀ¼����ݹ���ã��о��¼�Ŀ¼
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
���ܣ�
	ͨ�������Ի���ѡ��Ŀ¼��
������
	lpszTitle�������Ի����˵����������Ϣ���ǶԻ�����⡣
	hwndOwner���������ھ����
���أ�
	�û�ѡ���Ŀ¼�ַ�����
	���磺���ѡ���̷��������ء�C:\�������ѡ��Ŀ¼�������ء�C:\Windows\��
*/
CString GetDirByOpenFileDialog(LPCTSTR lpszTitle, HWND hwndOwner)
{
	//if (hwndOwner == NULL || !IsWindow(hwndOwner)) return "";

	CString strCurDir;  //ѡ���Ŀ¼
	HRESULT hRet = S_OK;

	TCHAR szTitle[MAX_PATH] = _T(""); //��ʾ����Ϣ���ǶԻ������
	TCHAR szDirName[MAX_PATH] = _T(""); //���浱ǰѡ�е�Ŀ¼���ƣ�����ȫ·����
	TCHAR szDirFullPath[MAX_PATH * 2] = {NULL}; //����Ŀ¼ȫ·��

	LPMALLOC pMalloc = NULL; // �����ͷ� ÿ�β�ѯ�� pItemIdList �ӿ�
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
	if (hRet != S_OK //��ʼ���ɹ�
		&& hRet != S_FALSE) //�Ѿ���ʼ��
	{
		//AfxMessageBox("���� OleInitialize ��ʼ�� OLE �� ʧ��");
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

	//�滻б��
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

	//�滻б��
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
		ZTools::WriteZToolsFormatLog("�ļ�·��Ϊ��");
		return FALSE;
	}

	//�滻б��
	std::replace(strPath.begin(), strPath.end(), '/', '\\');

	if (strPath.find(':') == std::string::npos || strPath.find('\\') == std::string::npos)
	{
		ZTools::WriteZToolsFormatLog("�ļ�·�����Ϸ�:%s", strPath.c_str());
		return FALSE;
	}
	return TRUE;
}

BOOL CFileManage::CheckDirPath(std::string strPath)
{
	if (strPath.empty())
	{
		ZTools::WriteZToolsFormatLog("Ŀ¼·��Ϊ��");
		return FALSE;
	}

	//�滻б��
	std::replace(strPath.begin(), strPath.end(), '/', '\\');

	if (strPath.find(':') == std::string::npos || strPath.find_last_of('\\') != strPath.length() - 1)
	{
		ZTools::WriteZToolsFormatLog("Ŀ¼·�����Ϸ�:%s", strPath.c_str());
		return FALSE;
	}
	return TRUE;
}

STDMETHODIMP CFileManage::SaveFile(BSTR bstrFile, BSTR bstrContent, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::SaveFile");
	CComVariant vRet(false);

	// TODO: �ڴ����ʵ�ִ���
	std::string strFile = _bstr_t(bstrFile);
	ZTools::WriteZToolsFormatLog("strFile:%s", strFile.c_str());
	std::string strContent = _bstr_t(bstrContent);
	//ZTools::WriteZToolsFormatLog("strContent:%s", strContent.c_str());

	//Ŀǰֻ�ڽ�ģҳ��д��xpdl�ļ�ʱ�õ���Ĭ�ϴ�Ϊutf8��ʽ������ascii�����ṩ�ӿ�
	ZTools::MBToUTF8(strContent);
	
	//�滻б��
	std::replace(strFile.begin(), strFile.end(), '/', '\\');

	//У��·��
	if (!CheckFilePath(strFile))
	{
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	std::string strDir = GetFileDir(strFile);
	if (!MakeSureDirectoryPathExists(strDir.c_str()))
	{
		ZTools::WriteZToolsFormatLog("�����ļ���ʧ��:%s", strDir.c_str());
		//ZTools::WriteZToolsLog(GetLastErrorMsg());
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	//std::locale::global(std::locale(""));
	std::ofstream fOut(strFile);
	if (fOut == NULL || fOut.is_open() == false)
	{
		ZTools::WriteZToolsFormatLog("д���ļ� %s ʧ��", strFile.c_str());
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

	// TODO: �ڴ����ʵ�ִ���
	std::string strFile = _bstr_t(bstrFile);
	ZTools::WriteZToolsFormatLog("strFile:%s", strFile.c_str());

	//�滻б��
	std::replace(strFile.begin(), strFile.end(), '/', '\\');

	//У��·��
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
		ZTools::WriteZToolsFormatLog("��ȡ�ļ� %s ʧ��", strFile.c_str());
		vRet = _bstr_t("").GetBSTR();
		vRet.Detach(pvRet);
		return S_OK;
	}

	std::ostringstream ssIn;
	ssIn << fIn.rdbuf();
	fIn.close();

	std::string strResult = ssIn.str();

	//Ŀǰֻ�ڽ�ģҳ���ȡxpdl�ļ�ʱ�õ���Ĭ�ϴ�Ϊutf8��ʽ������ascii�����ṩ�ӿ�
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

	// TODO: �ڴ����ʵ�ִ���
	std::string strFile = _bstr_t(bstrFile);
	ZTools::WriteZToolsFormatLog("strFile:%s", strFile.c_str());
	std::string strContent = _bstr_t(bstrContent);
	//ZTools::WriteZToolsFormatLog("strContent:%s", strContent.c_str());

	//�滻б��
	std::replace(strFile.begin(), strFile.end(), '/', '\\');

	//У��·��
	if (!CheckFilePath(strFile))
	{
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	std::string strDir = GetFileDir(strFile);
	if (!MakeSureDirectoryPathExists(strDir.c_str()))
	{
		ZTools::WriteZToolsFormatLog("�����ļ���ʧ��:%s", strDir.c_str());
		//ZTools::WriteZToolsLog(GetLastErrorMsg());
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	//std::locale::global(std::locale(""));
	std::ofstream fOut(strFile);
	if (fOut == NULL || fOut.is_open() == false)
	{
		ZTools::WriteZToolsFormatLog("д���ļ� %s ʧ��", strFile.c_str());
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

	// TODO: �ڴ����ʵ�ִ���
	std::string strFile = _bstr_t(bstrFile);
	ZTools::WriteZToolsFormatLog("strFile:%s", strFile.c_str());

	//�滻б��
	std::replace(strFile.begin(), strFile.end(), '/', '\\');

	//У��·��
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
		ZTools::WriteZToolsFormatLog("��ȡ�ļ� %s ʧ��", strFile.c_str());
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

	// TODO: �ڴ����ʵ�ִ���
	std::string strSrcFile = _bstr_t(bstrSrcFile);
	ZTools::WriteZToolsFormatLog("strSrcFile:%s", strSrcFile.c_str());
	std::string strDstFile = _bstr_t(bstrDstFile);
	ZTools::WriteZToolsFormatLog("strDstFile:%s", strDstFile.c_str());

	//�滻б��
	std::replace(strSrcFile.begin(), strSrcFile.end(), '/', '\\');
	std::replace(strDstFile.begin(), strDstFile.end(), '/', '\\');

	//У��·��
	if (!CheckFilePath(strSrcFile) || !CheckFilePath(strDstFile))
	{
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	//Դ�ļ��Ƿ����
	if (!PathFileExists(strSrcFile.c_str()))
	{
		ZTools::WriteZToolsFormatLog("�ļ������ڣ�%s", strSrcFile.c_str());
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	//ԴĿ���Ƿ���ͬ
	if (IsEqualNoCase(strSrcFile, strDstFile) == TRUE)
	{
		ZTools::WriteZToolsFormatLog("Դ��Ŀ��·����ͬ��%s -- %s", strSrcFile.c_str(), strDstFile.c_str());
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	//Ŀ��·���Ƿ����
	if (!MakeSureDirectoryPathExists(strDstFile.c_str()))
	{
		ZTools::WriteZToolsFormatLog("�����ļ���ʧ��:%s", strDstFile.c_str());
		//ZTools::WriteZToolsLog(GetLastErrorMsg());
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	if (!::CopyFile(strSrcFile.c_str(), strDstFile.c_str(), FALSE))
	{
		ZTools::WriteZToolsFormatLog("�����ļ�ʧ�ܣ�%s -- %s", strSrcFile.c_str(), strDstFile.c_str());
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

	// TODO: �ڴ����ʵ�ִ���
	std::string strSrcDir = _bstr_t(bstrSrcDir);
	ZTools::WriteZToolsFormatLog("strSrcDir:%s", strSrcDir.c_str());
	std::string strDstDir = _bstr_t(bstrDstDir);
	ZTools::WriteZToolsFormatLog("strDstDir:%s", strDstDir.c_str());

	//�滻б��
	std::replace(strSrcDir.begin(), strSrcDir.end(), '/', '\\');
	std::replace(strDstDir.begin(), strDstDir.end(), '/', '\\');

	//У��·��
	if (!CheckDirPath(strSrcDir) || !CheckDirPath(strDstDir))
	{
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	//Դ�ļ��Ƿ����
	if (!PathFileExists(strSrcDir.c_str()))
	{
		ZTools::WriteZToolsFormatLog("�ļ�Ŀ¼�����ڣ�%s", strSrcDir.c_str());
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	//ԴĿ���Ƿ���ͬ
	if (IsEqualNoCase(strSrcDir, strDstDir) == TRUE)
	{
		ZTools::WriteZToolsFormatLog("Դ��Ŀ��·����ͬ��%s -- %s", strSrcDir.c_str(), strDstDir.c_str());
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	//Ŀ��·���Ƿ����
	if (!MakeSureDirectoryPathExists(strDstDir.c_str()))
	{
		ZTools::WriteZToolsFormatLog("�����ļ���ʧ��:%s", strDstDir.c_str());
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
		ZTools::WriteZToolsFormatLog("�����ļ���ʧ��:%s -- %s -- 0x%x", strSrcDir.c_str(), strDstDir.c_str(), nRet);
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

	//�滻б��
	std::replace(strDir.begin(), strDir.end(), '/', '\\');

	//У��·��
	if (!CheckDirPath(strDir))
	{
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	if (!MakeSureDirectoryPathExists(strDir.c_str()))
	{
		ZTools::WriteZToolsFormatLog("�����ļ���ʧ��:%s", strDir.c_str());
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

	// TODO: �ڴ����ʵ�ִ���
	std::string strFile = _bstr_t(bstrFile);
	ZTools::WriteZToolsFormatLog("strFile:%s", strFile.c_str());

	//�滻б��
	std::replace(strFile.begin(), strFile.end(), '/', '\\');

	//У��·��
	if (!CheckFilePath(strFile))
	{
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	//Դ�ļ��Ƿ����
	if (!PathFileExists(strFile.c_str()))
	{
		ZTools::WriteZToolsFormatLog("�ļ������ڣ�%s", strFile.c_str());
		vRet = true;//ɾ��ʱ����������Ϊɾ���ɹ�
		vRet.Detach(pvRet);
		return S_OK;
	}
	
	if (!::DeleteFile(strFile.c_str()))
	{
		ZTools::WriteZToolsFormatLog("ɾ���ļ�ʧ�ܣ�%s", strFile.c_str());
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

	// TODO: �ڴ����ʵ�ִ���
	std::string strDir = _bstr_t(bstrDir);
	ZTools::WriteZToolsFormatLog("strDir:%s", strDir.c_str());

	//�滻б��
	std::replace(strDir.begin(), strDir.end(), '/', '\\');

	//У��·��
	if (!CheckDirPath(strDir))
	{
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	//Դ�ļ��Ƿ����
	if (!PathFileExists(strDir.c_str()))
	{
		ZTools::WriteZToolsFormatLog("�ļ�Ŀ¼�����ڣ�%s", strDir.c_str());
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
		ZTools::WriteZToolsFormatLog("ɾ���ļ���ʧ��:%s -- 0x%x", strDir.c_str(), nRet);
		//ZTools::WriteZToolsLog(GetLastErrorMsg());
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	strDir = strDir.substr(0, strDir.find_last_of('\\') + 1);
	if (!::RemoveDirectory(strDir.c_str()))
	{
		ZTools::WriteZToolsFormatLog("ɾ���ļ���ʧ��:%s", strDir.c_str(), nRet);
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
	dlg.m_ofn.lpstrFilter = "�����ļ�(*.*)\0*.*\0\0";

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
	CString strDir = GetDirByOpenFileDialog("��ѡ��Ŀ¼", pWnd->GetSafeHwnd());
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

	//�滻б��
	std::replace(strDir.begin(), strDir.end(), '/', '\\');

	//У��·��
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

//����ļ���
STDMETHODIMP CFileManage::ClearDir(BSTR bstrDir, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::ClearDir");
	CComVariant vRet(false);

	// TODO: �ڴ����ʵ�ִ���
	std::string strDir = _bstr_t(bstrDir);
	ZTools::WriteZToolsFormatLog("strDir:%s", strDir.c_str());

	//�滻б��
	std::replace(strDir.begin(), strDir.end(), '/', '\\');

	//У��·��
	if (!CheckDirPath(strDir))
	{
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	//Դ�ļ��Ƿ����
	if (!PathFileExists(strDir.c_str()))
	{
		ZTools::WriteZToolsFormatLog("�ļ�Ŀ¼�����ڣ�%s", strDir.c_str());
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
		ZTools::WriteZToolsFormatLog("����ļ���ʧ��:%s -- 0x%x", strDir.c_str(), nRet);
		//ZTools::WriteZToolsLog(GetLastErrorMsg());
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	vRet = true;
	vRet.Detach(pvRet);
	return S_OK;
}

//�������ļ�
STDMETHODIMP CFileManage::RenameFile(BSTR bstrOldName, BSTR bstrNewName, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::RenameFile");
	CComVariant vRet(false);

	// TODO: �ڴ����ʵ�ִ���
	std::string strFile = _bstr_t(bstrOldName);
	std::string strFileDst = _bstr_t(bstrNewName);
	ZTools::WriteZToolsFormatLog("strFile:%s", strFile.c_str());
	ZTools::WriteZToolsFormatLog("strFileDst:%s", strFileDst.c_str());

	//�滻б��
	std::replace(strFile.begin(), strFile.end(), '/', '\\');
	std::replace(strFileDst.begin(), strFileDst.end(), '/', '\\');

	//У��·��
	if (!CheckFilePath(strFile) || !CheckFilePath(strFileDst))
	{
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	//Դ�ļ��Ƿ����
	if (!PathFileExists(strFile.c_str()))
	{
		ZTools::WriteZToolsFormatLog("�ļ������ڣ�%s", strFile.c_str());
		vRet = false;//������ʱ����������Ϊ������ʧ��
		vRet.Detach(pvRet);
		return S_OK;
	}

	if (::rename(strFile.c_str(), strFileDst.c_str()) == -1)
	{
		ZTools::WriteZToolsFormatLog("�������ļ�ʧ�ܣ�%s", strFile.c_str());
		//ZTools::WriteZToolsLog(GetLastErrorMsg());
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	vRet = true;
	vRet.Detach(pvRet);
	return S_OK;
}

//�ƶ��ļ�
STDMETHODIMP CFileManage::MoveFile(BSTR srcFilePath, BSTR dstFilePath, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: �ڴ����ʵ�ִ���
	//RenameFile(srcFilePath, dstFilePath, pvRet);  //��������������������һ�£�ֱ�ӵ���

	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::MoveFile");
	CComVariant vRet(false);

	// TODO: �ڴ����ʵ�ִ���
	std::string strFile = _bstr_t(srcFilePath);
	std::string strFileDst = _bstr_t(dstFilePath);
	ZTools::WriteZToolsFormatLog("strFile:%s", strFile.c_str());
	ZTools::WriteZToolsFormatLog("strFileDst:%s", strFileDst.c_str());

	//�滻б��
	std::replace(strFile.begin(), strFile.end(), '/', '\\');
	std::replace(strFileDst.begin(), strFileDst.end(), '/', '\\');

	//У��·��
	if (!CheckFilePath(strFile) || !CheckFilePath(strFileDst))
	{
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	//Դ�ļ��Ƿ����
	if (!PathFileExists(strFile.c_str()))
	{
		ZTools::WriteZToolsFormatLog("�ļ������ڣ�%s", strFile.c_str());
		vRet = false;//�ƶ�ʱ����������Ϊ�ƶ�ʧ��
		vRet.Detach(pvRet);
		return S_OK;
	}

	if (::rename(strFile.c_str(), strFileDst.c_str()) == -1)
	{
		ZTools::WriteZToolsFormatLog("�ƶ��ļ�ʧ�ܣ�%s", strFile.c_str());
		//ZTools::WriteZToolsLog(GetLastErrorMsg());
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	vRet = true;
	vRet.Detach(pvRet);

	return S_OK;
}

//�ƶ��ļ���
STDMETHODIMP CFileManage::MoveDir(BSTR srcDir, BSTR dstDir, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::MoveDir");
	CComVariant vRet(false);

	std::string strDir = _bstr_t(srcDir);
	std::string strDirDst = _bstr_t(dstDir);
	ZTools::WriteZToolsFormatLog("strDir%s", strDir.c_str());
	ZTools::WriteZToolsFormatLog("strDirDst:%s", strDirDst.c_str());

	//�滻б��
	std::replace(strDir.begin(), strDir.end(), '/', '\\');
	std::replace(strDirDst.begin(), strDirDst.end(), '/', '\\');

	//У��·��
	if (!CheckDirPath(strDir) || !CheckDirPath(strDirDst))
	{
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	//Դ�ļ�Ŀ¼�Ƿ����
	if (!PathFileExists(strDir.c_str()))
	{
		ZTools::WriteZToolsFormatLog("�ļ�Ŀ¼�����ڣ�%s", strDir.c_str());
		vRet = false;//�ƶ�ʱ����������Ϊ�ƶ�ʧ��
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
		ZTools::WriteZToolsFormatLog("�ƶ��ļ�Ŀ¼ʧ�ܣ�%s", strDir.c_str());
		//ZTools::WriteZToolsLog(GetLastErrorMsg());
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	vRet = true;
	vRet.Detach(pvRet);

	return S_OK;
}

//�Ƚ��ļ�
STDMETHODIMP CFileManage::CompareFile(BSTR srcFilePath, BSTR dstFilePath, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::CompareFile");
	CComVariant vRet(false);

	// TODO: �ڴ����ʵ�ִ���
	std::string strFile = _bstr_t(srcFilePath);
	std::string strFileDst = _bstr_t(dstFilePath);
	ZTools::WriteZToolsFormatLog("strFile:%s", strFile.c_str());
	ZTools::WriteZToolsFormatLog("strFileDst:%s", strFileDst.c_str());

	//�滻б��
	std::replace(strFile.begin(), strFile.end(), '/', '\\');
	std::replace(strFileDst.begin(), strFileDst.end(), '/', '\\');

	//У��·��
	if (!CheckFilePath(strFile) || !CheckFilePath(strFileDst))
	{
		vRet = false;
		vRet.Detach(pvRet);
		return S_OK;
	}

	//Դ�ļ��Ƿ����
	if (!PathFileExists(strFile.c_str()) || !PathFileExists(strFileDst.c_str()))
	{
		if(!PathFileExists(strFile.c_str())) ZTools::WriteZToolsFormatLog("�ļ������ڣ�%s", strFile.c_str());
		if(!PathFileExists(strFileDst.c_str())) ZTools::WriteZToolsFormatLog("�ļ������ڣ�%s", strFileDst.c_str());
		vRet = false;//�Ƚ�ʱ����������Ϊ�Ƚ�ʧ��
		vRet.Detach(pvRet);
		return S_OK;
	}

	if (GetFileSize(strFile.c_str()) != GetFileSize(strFileDst.c_str()))
	{//�ļ������Ƿ���ͬ
		ZTools::WriteZToolsFormatLog("�ļ����Ȳ�ͬ��%s", strFile.c_str());
		ZTools::WriteZToolsFormatLog("�ļ����Ȳ�ͬ��%s", strFileDst.c_str());
		//ZTools::WriteZToolsLog(GetLastErrorMsg());
		vRet = false;
		//vRet = "�ļ����Ȳ�ͬ";
		vRet.Detach(pvRet);
		return S_OK;
	}

	if (GetFileMD5(strFile) != GetFileMD5(strFileDst))
	{//�ļ�����У��
		ZTools::WriteZToolsFormatLog("�Ƚ��ļ�ʧ�ܣ�%s", strFile.c_str());
		ZTools::WriteZToolsFormatLog("�Ƚ��ļ�ʧ�ܣ�%s", strFileDst.c_str());
		//ZTools::WriteZToolsLog(GetLastErrorMsg());
		vRet = false;
		//vRet = "�Ƚ��ļ�ʧ��";
		vRet.Detach(pvRet);
		return S_OK;
	}

	vRet = true;
	vRet.Detach(pvRet);

	return S_OK;
}

//��ȡ�ļ�����
STDMETHODIMP CFileManage::GetContentFromZip(BSTR bstrZip, BSTR bstrFile, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::GetContentFromZip");
	CComVariant vRet(_bstr_t("").GetBSTR());

	// TODO: �ڴ����ʵ�ִ���
	std::string strZip = _bstr_t(bstrZip);
	ZTools::WriteZToolsFormatLog("strZip:%s", strZip.c_str());
	std::string strFile = _bstr_t(bstrFile);
	ZTools::WriteZToolsFormatLog("strFile:%s", strFile.c_str());

	vRet = _bstr_t(ZipTool::GetContentFromZip(strZip, strFile).c_str()).GetBSTR();
	vRet.Detach(pvRet);

	return S_OK;
}

// ��ѹһ���ļ���ĳ��
STDMETHODIMP CFileManage::UnzipFile(BSTR bstrZip, BSTR bstrFileSrc, BSTR bstrFileDst, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: �ڴ����ʵ�ִ���
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
		ZTools::WriteZToolsLog("��ѹ�ļ������쳣");
	}
	vRet.Detach(pvRet);

	return S_OK;
}

// ��ѹ��������ĳ�ļ���
STDMETHODIMP CFileManage::Unzip(BSTR bstrZip, BSTR bstrDir, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: �ڴ����ʵ�ִ���
	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::Unzip");
	CComVariant vRet(false);

	std::string strZip = _bstr_t(bstrZip);
	ZTools::WriteZToolsFormatLog("strZip:%s", strZip.c_str());
	std::string strDir = _bstr_t(bstrDir);
	ZTools::WriteZToolsFormatLog("strDir:%s", strDir.c_str());

	vRet = ZipTool::Unzip(strZip, strDir);
	if (vRet.boolVal == false)
	{
		ZTools::WriteZToolsLog("��ѹ�ļ������쳣");
	}
	vRet.Detach(pvRet);

	return S_OK;
}

//��ѹ���е�ĳ���ļ���ָ��Ŀ¼
STDMETHODIMP CFileManage::UnzipDir(BSTR bstrZip, BSTR bstrDir, BSTR bstrDirDst, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: �ڴ����ʵ�ִ���
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
		ZTools::WriteZToolsLog("��ѹ�ļ������쳣");
	}
	vRet.Detach(pvRet);

	return S_OK;
}

// ��ָ��Ŀ¼����ZIP�ļ�
STDMETHODIMP CFileManage::ZipFromDir(BSTR bstrZip, BSTR bstrDir, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: �ڴ����ʵ�ִ���
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

//��JsonStringָ���б���ZIP
STDMETHODIMP CFileManage::ZipFromJsonString(BSTR bstrZip, BSTR bstrJson, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: �ڴ����ʵ�ִ���
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

//��ȡ��UTF8�����ļ�������
STDMETHODIMP CFileManage::GetUtf8ContentFromZip(BSTR bstrZip, BSTR bstrFile, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ZTools::WriteZToolsLog(">>>>>>PeraFileManage::CFileManage::GetUtf8ContentFromZip");
	CComVariant vRet(_bstr_t("").GetBSTR());

	// TODO: �ڴ����ʵ�ִ���
	std::string strZip = _bstr_t(bstrZip);
	ZTools::WriteZToolsFormatLog("strZip:%s", strZip.c_str());
	std::string strFile = _bstr_t(bstrFile);
	ZTools::WriteZToolsFormatLog("strFile:%s", strFile.c_str());

	std::string strContent = ZipTool::GetContentFromZip(strZip, strFile);
	ZTools::UTF8ToMB(strContent); //ת��UTF8��ʽ

	vRet = _bstr_t(strContent.c_str()).GetBSTR();
	vRet.Detach(pvRet);

	return S_OK;
}


STDMETHODIMP CFileManage::ZipFromDirDeflate(BSTR bstrZip, BSTR bstrDir, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: �ڴ����ʵ�ִ���
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

	// TODO: �ڴ����ʵ�ִ���
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
