#include "StdAfx.h"
#include "FilePro.h"
#include<Windows.h>
#include <shellapi.h>
#include<stdio.h>
#include <io.h>
#include <sstream>
#include <comdef.h>
#include "Tools.h"
#include <sstream>
#include "PeraFileSelectDlg.h"
#include "FileFunc.h"

#ifdef LINK_NETDISKDAEMON
	#include "../PeraDCNetDiskDaemon/PeraNetDiskSetting.h"
    extern BOOL GetFileAssociateDescription(LPCTSTR strFileExt,CString & strAssociateDesp,CString & strErrMsg);
#endif

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include "cryptopp561/md5.h"
#include "cryptopp561/filters.h"
#include "cryptopp561/channels.h"
#include "cryptopp561/files.h"
#include "cryptopp561/hex.h"
#ifdef _DEBUG
#pragma comment(lib, "cryptlibd.lib")
#else
#pragma comment(lib, "cryptlib.lib")
#endif

	typedef   struct
	{
		DWORD  m_dwDocProcessId;
		CString m_strDocFileName;
		HWND    m_hWndFindByDocName;
		HWND    m_hWndFindByProMain;
	}DataFindDocWnd;

	static BOOL CALLBACK EnumWindowCallback_GetMainWindow( HWND hWnd, LPARAM lParam )
	{
		DataFindDocWnd *pData = (DataFindDocWnd *)lParam;
		if (pData == NULL) return FALSE;

		DWORD dwProcessId = 0;
		GetWindowThreadProcessId(hWnd, &dwProcessId); //取得当前窗口对应的进程id
		CString strWindowText;
		GetWindowText(hWnd,strWindowText.GetBuffer(MAX_PATH),MAX_PATH);

		if (IsWindowVisible(hWnd) && GetParent(hWnd)==NULL)
		{
			if (dwProcessId == pData->m_dwDocProcessId)
			{
				pData->m_hWndFindByProMain = hWnd;
			}

			if (strWindowText.Find(pData->m_strDocFileName)!=-1)
			{
				pData->m_hWndFindByDocName = hWnd;
			}
		}

		if (pData->m_hWndFindByProMain !=NULL && pData->m_hWndFindByDocName!=NULL)
			return FALSE; // 及时终止遍历

		return TRUE;
	}

	//根据进程ID，主窗口标题（模糊匹配）取得文档所在窗口句柄
	HWND GetDocWindow( DWORD dwPID,CString strFileName)
	{
		DataFindDocWnd param;
		param.m_dwDocProcessId = dwPID;
		strFileName  = strFileName.Mid(strFileName.ReverseFind('\\')+1);
		param.m_strDocFileName = strFileName;
		param.m_hWndFindByProMain = NULL;
		param.m_hWndFindByDocName = NULL;

		int iStep = 0;
		while(iStep < 3 && param.m_hWndFindByProMain ==NULL && param.m_hWndFindByDocName ==NULL)
		{
			Sleep(3000);
			EnumWindows(EnumWindowCallback_GetMainWindow, (LPARAM)&param);
			iStep ++;
		}
		if (param.m_hWndFindByProMain !=NULL)
			return param.m_hWndFindByProMain;
		else
			return param.m_hWndFindByDocName;
	}

static CTime ConvertTime_tToCtime( const time_t * t)
{
	int nYear, nMonth, nDate, nHour, nMin, nSec;
	struct tm local;
	localtime_s(&local,t);
	nYear = local.tm_year + 1900;
	nMonth = local.tm_mon + 1;
	nDate = local.tm_mday;
	nHour = local.tm_hour;
	nMin  = local.tm_min;
	nSec  = local.tm_sec;
	CTime ptime(nYear, nMonth, nDate, nHour, nMin, nSec);
	return ptime;
}
CFilePro::CFilePro(void)
	:m_strErrMsg(_T(""))
{
	m_iErrCode = 0;
}


CFilePro::~CFilePro(void)
{
}

vector<CString>  CFilePro::GetSelectedFile(BOOL bMutilSelect,const char * pszDefExt,const char *pszFilter)
{
	DWORD flags = 6UL;
	if (bMutilSelect)
		flags = OFN_ALLOWMULTISELECT|OFN_PATHMUSTEXIST;

	vector<CString> vOutSelect;
	CFileDialog dlg(TRUE,pszDefExt,NULL,flags,pszFilter);
	const int MAX_FILE_NUMBER = 100;                                                           //至少允许选择10个文件
	dlg.m_ofn.lpstrFile = new TCHAR[_MAX_PATH * MAX_FILE_NUMBER]; //重新定义缓冲区大小          
	memset(dlg.m_ofn.lpstrFile, 0, _MAX_PATH * MAX_FILE_NUMBER);  //初始化定义的缓冲区
	dlg.m_ofn.nMaxFile = _MAX_PATH*MAX_FILE_NUMBER; // 指定缓冲区大小
	if (dlg.DoModal() != IDOK)
	{
		m_strErrMsg = "用户取消";
		delete[] dlg.m_ofn.lpstrFile;     //最后别忘了释放内存
		return vOutSelect;
	}

	POSITION   pos   =   dlg.GetStartPosition();  
	while(pos   !=   NULL)  
	{  
		CString	filepath   =   dlg.GetNextPathName(pos);
		vOutSelect.push_back(filepath);
	}
	delete[] dlg.m_ofn.lpstrFile;     //最后别忘了释放内存
	return vOutSelect;
}

int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lParam,LPARAM lpData)
{
	switch(uMsg)
	{
	case BFFM_INITIALIZED:
		::SendMessage(hwnd,BFFM_SETSELECTION,TRUE,lpData);
		HWND hFind = ::GetWindow(hwnd,GW_CHILD);
		while(hFind)
		{
			char szName[256] ={0};
			GetClassName(hFind,szName,256);
			if (_stricmp(szName,"Edit")==0)
			{
				CEdit *pEdit = (CEdit*)CWnd::FromHandle(hFind);
				//pEdit->EnableWindow(FALSE);
				pEdit->SetReadOnly(TRUE); //将控件设置为只读，方便复制，但不允许修改
				break;
			}
			hFind = ::GetWindow(hFind,GW_HWNDNEXT);
		}
		
		break;
	}
	return 0;
}
CString CFilePro::GetSelectedFolder()
{
	BROWSEINFO bi;  
	ZeroMemory(&bi,sizeof(BROWSEINFO)); 
	bi.hwndOwner = GetActiveWindow();   
	bi.lpszTitle = "请选择目录：";  

	static CString strLastDir = "";
	bi.ulFlags = BIF_USENEWUI | BIF_RETURNONLYFSDIRS;
	bi.lpfn = BrowseCallbackProc;
	bi.lParam = (LPARAM)(LPCTSTR)strLastDir;
	

	LPMALLOC pMalloc;  

	if(::GetCapture())  
		::ReleaseCapture(); 
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);  

	if(!pidl )
	{
		m_strErrMsg = "用户取消";
		return "";
	}

	CString strFolderOut = "";
	//TCHAR * path = new TCHAR[MAX_PATH];   
	WCHAR * path = new WCHAR[MAX_PATH];  
	SHGetPathFromIDListW(pidl,path);
	//SHGetPathFromIDList(pidl,path);  
	if(SUCCEEDED(SHGetMalloc(&pMalloc))) 
	{  
		pMalloc->Free(pidl);  
		pMalloc->Release();  
	}

	strFolderOut = path;
	delete [] path;
	if (strFolderOut.IsEmpty())
		m_strErrMsg = "非法路径";
	else
	{
		strLastDir = strFolderOut; // 记住上次打开的目录
	}

	return strFolderOut;
}

bool CFilePro::OpenFile(CString strFile,HANDLE * hOut)
{
	ZTools::WriteZToolsFormatLog("OpenFile,输入参数(%s,0x%08d)",strFile,hOut);
	if (!PathFileExists(strFile))
	{
		m_strErrMsg.Format("文件%s不存在!",strFile);
		return false;
	}

	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS|SEE_MASK_FLAG_NO_UI;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = "Open";
	ShExecInfo.lpFile = strFile;        
	ShExecInfo.lpParameters = NULL;  
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL;	

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	ShellExecuteEx(&ShExecInfo);
	DWORD dErrCode = GetLastError();
	static string strSysVer = "";
	if (strSysVer.empty())
	{
		OSVERSIONINFO   osversioninfo;
		osversioninfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&osversioninfo);
		if( (osversioninfo.dwMajorVersion == 5) && (osversioninfo.dwMinorVersion == 1)) 
			strSysVer = "xp";
		
		else if( (osversioninfo.dwMajorVersion == 6) && (osversioninfo.dwMinorVersion == 1))
			strSysVer =  "win7";

		else if( (osversioninfo.dwMajorVersion == 6) && (osversioninfo.dwMinorVersion == 2))
			strSysVer =  "win8";
		else
			strSysVer =  "other";

		ZTools::WriteZToolsFormatLog("OpenFile 获取系统版本为:%s",strSysVer.c_str());
	}

	if ("xp" == strSysVer)
	{
		//解决xp机器上打开多个word、excel文件失败的问题
		strFile.MakeLower();
		if (  strFile.Right(4)==".doc" 
			|| strFile.Right(4)==".xls" 
			|| strFile.Right(4)==".ppt"
			|| strFile.Right(5)==".docx"
			|| strFile.Right(5)==".xlsx"
			|| strFile.Right(5)==".pptx")
		{
			Sleep(1000);
		}
	}
	if (hOut)
	{
		*hOut = ShExecInfo.hProcess;
		WaitForInputIdle(ShExecInfo.hProcess, 5000);
		DWORD dwDocProcess = GetProcessId(ShExecInfo.hProcess);
		HWND hWnd = GetDocWindow(dwDocProcess,strFile);
		if (hWnd!=NULL)
		{

			//BringWindowToTop(hWnd);
			//::SetWindowPos(hWnd,HWND_TOPMOST,0,0,0,0, SWP_ASYNCWINDOWPOS|SWP_NOSIZE|SWP_NOMOVE);
			::SetForegroundWindow(hWnd);
		}
	}
	int iCode = (int)ShExecInfo.hInstApp;
	if ((int)ShExecInfo.hInstApp > 32)
		return true;
	else
	{
		if (dErrCode==1155)
		{
			ShExecInfo.lpVerb = "OpenAs";
			ShExecInfo.hInstApp = NULL;
			BOOL bOK = ShellExecuteEx(&ShExecInfo);
			ZTools::WriteZToolsLog("Openfile 未找到关联程序，使用OpenAs模式由用户选择关联程序");
			if (hOut)
				*hOut = NULL; // OpenAs模式不监控进程的退出（不知道打开文件进程的进程句柄）

			if(!bOK)
			{
				CString strCmd;
				strCmd.Format("rundll32 shell32, OpenAs_RunDLL  %s", strFile);
				WinExec(strCmd, SW_SHOWNORMAL); //手动执行打开选择关联方式对话框
				return true;

				/*dErrCode = GetLastError();
				m_strErrMsg = ZTools::GetLastErrorMessageString(dErrCode).c_str();
				return false;*/
			}

			return true; //返回真，外部可以根据hOut是否为NULL判断是否是OpenAs模式
		}
		m_strErrMsg = ZTools::GetLastErrorMessageString(dErrCode).c_str();
		return false;
	}

	/*HINSTANCE h = ShellExecuteA(0,"open",strFile,NULL,NULL,SW_SHOWNORMAL);
	if (hOut)
	{
		*hOut = h;
	}
	if ((int)h > 32)
	  return true;

	return false;*/
}

bool CFilePro::GetFileMD5(CString strInFile,CString & strOutMd5)
{
	if (!PathFileExists(strInFile))
	{
		m_strErrMsg.Format(_T("文件【%s】无法访问!"),strInFile);
		return false;
	}
	
	CryptoPP::Weak::MD5 md5;
	CryptoPP::HashFilter filter(md5);

	std::auto_ptr<CryptoPP::ChannelSwitch> channelSwitch(new CryptoPP::ChannelSwitch);
	channelSwitch->AddDefaultRoute(filter);

	std::wstring wstr 	= (wchar_t * )_bstr_t(LPCTSTR(strInFile)); 
	CryptoPP::FileSource(wstr.c_str(), true, channelSwitch.release());

	std::stringstream result;
	CryptoPP::HexEncoder encoder(new CryptoPP::FileSink(result), false);
	filter.TransferTo(encoder);
	std::string s = result.str();
	strOutMd5 = s.c_str();
	return true;
}
bool CFilePro::GetFileInfo(CString strInFile,SFileInfo &fileInfo,const char *pszTimeFormat)
{
	LONGLONG iFileSize =  FileSize(strInFile);
	if(iFileSize < 0)
	{
		m_strErrMsg.Format(_T("文件【%s】无法访问!"),strInFile);
		return false;
	}

	fileInfo.iFileSize =  iFileSize;

	WIN32_FIND_DATAW DataFind;
	CStringW strFileW = (WCHAR*)_bstr_t(strInFile);
	HANDLE hFind = FindFirstFileW(strFileW,&DataFind);
	if (hFind==INVALID_HANDLE_VALUE)
	{
		m_strErrMsg.Format(_T("文件【%s】无法访问!"),strInFile);
		return false;
	}
	if(DataFind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		FindClose(hFind);
		m_strErrMsg.Format(_T("【%s】是目录，不是文件!"),strInFile);
		return false;
	}
	FindClose(hFind);

	SYSTEMTIME  st;
	FileTimeToSystemTime(&DataFind.ftLastWriteTime,&st);
	fileInfo.m_strLastModifyTime.Format("%04d-%02d-%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay, st.wHour, st.wMinute, st.wSecond);
	FileTimeToSystemTime(&DataFind.ftCreationTime,&st);
	fileInfo.m_strCreateTime.Format("%04d-%02d-%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay, st.wHour, st.wMinute, st.wSecond);
	FileTimeToSystemTime(&DataFind.ftLastAccessTime,&st);
	fileInfo.m_strLastAccessTime.Format("%04d-%02d-%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay, st.wHour, st.wMinute, st.wSecond);

	fileInfo.m_strFullPath = strInFile;

	char szDriver[3],dir[1024],fileName[1024],ext[50];
	_splitpath_s(strInFile,szDriver,dir,fileName,ext);
	fileInfo.m_strFileName = fileName;
	fileInfo.m_strFileName +=ext;

	SHFILEINFOA info;
	if(SHGetFileInfoA(ext,FILE_ATTRIBUTE_NORMAL,
		&info,sizeof(info),SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES))
	{
		fileInfo.m_strFileType = info.szTypeName;
	}

#ifdef LINK_NETDISKDAEMON

	/*if (!GetFileAssociateDescription(ext,fileInfo.m_strOpenWithType,m_strErrMsg))
	return false;*/

	/*CString strDest = theSetting.GetCacheDir();
	fileInfo.m_strFileIcon.Format("%s\\IcoCache\\%s\\%s_64.png",strDest,ext+1,ext+1);
	if (!PathFileExists(fileInfo.m_strFileIcon))
	{
	vector<NewType * > vNew;
	NewType Tp;
	Tp.m_strExt = ext;
	Tp.m_strDesrciption = fileInfo.m_strFileType;
	vNew.push_back(&Tp);
	GetAllNewItemsIconFile(vNew,strDest);
	}*/
#endif // LINK_NETDISKDAEMON

	return true;
}
bool CFilePro::DeleteLocalFile(const CString & strFile,BOOL bAllowUndo)
{
	m_strErrMsg.Empty();
	if (!PathFileExists(strFile))
	{
		m_strErrMsg.Format( _T("文件【%s】不存在"),strFile);
		ZTools::WriteZToolsFormatLog(m_strErrMsg);
		return false;
	}

#if 0
	SHFILEOPSTRUCT  shDelFile;
	memset(&shDelFile,0,sizeof(SHFILEOPSTRUCT));
	shDelFile.fFlags |= FOF_SILENT;      // 不显示进度
	shDelFile.fFlags |= FOF_NOERRORUI;     //不报告错误信息
	shDelFile.fFlags |= FOF_NOCONFIRMATION;    //直接删除，不进行确认
	// 复制路径到一个以双NULL结束的string变量里
	CHAR buf[_MAX_PATH + 1]; 
	strcpy_s(buf, (LPCTSTR)strFile);   // 复制路径
	buf[strlen(buf)+1]=0;   // 在末尾加两个NULL

	// 设置SHFILEOPSTRUCT的参数为删除做准备
	shDelFile.wFunc = FO_DELETE;       // 执行的操作
	shDelFile.pFrom = buf;         // 操作的对象，也就是目录
	shDelFile.pTo = NULL;          // 必须设置为NULL
	if (bAllowUndo) //根据传递的bDelete参数确定是否删除到回收站
	{    
		shDelFile.fFlags |= FOF_ALLOWUNDO;    //删除到回收站
	} 
	else 
	{           
		shDelFile.fFlags &= ~FOF_ALLOWUNDO;    //直接删除，不进入回收站
	}
	if (SHFileOperation(&shDelFile)==0)
		return true;
#else
	if (::DeleteFile(strFile))
		return true;
#endif
	m_strErrMsg = ZTools::GetLastErrorMessageString().c_str();
	CString strTmp;
	if (PathFileExists(strFile))
		strTmp.Format("删除文件【%s】失败",strFile);
	else
	{
		m_strErrMsg.Empty();
		return true;
	}

	m_strErrMsg = strTmp + "," + m_strErrMsg;
	ZTools::WriteZToolsFormatLog(m_strErrMsg);
	return false;
}
bool CFilePro::ReNameLocalFile(const CString &  strFile,const CString & strNewName)
{
	m_strErrMsg.Empty();
	if (!PathFileExists(strFile))
	{
		m_strErrMsg.Format( _T("文件【%s】不存在"),strFile);
		return false;
	}
	if (PathFileExists(strNewName))
	{
		m_strErrMsg.Format( _T("重命名失败，已经存在【%s】文件，请指定新的文件名!"),strNewName);
		return false;
	}

	wstring strW = ZTools::s2ws((LPCTSTR)strNewName);
	if (strW.length() >MAX_PATH)
	{
		m_strErrMsg = "文件路径过长！";
		m_iErrCode = 1;
		return false;
	}
	if (rename(strFile,strNewName)==0)
		return true;

	m_strErrMsg = ZTools::GetLastErrorMessageString().c_str();
	if (m_strErrMsg.IsEmpty() && PathFileExists(strFile))
		m_strErrMsg.Format("重命名【%s】到【%s】失败！",strFile,strNewName);

	return false;
}
bool CFilePro::GetFolderItems(CString strFolder,Folder_Item &Items,
	bool bRecursion ,
	bool bInCludeFile ,
	bool bIncludeDir,
	bool RelativePath)
{
	if (!IsAccessibleDir(strFolder))
	{
		m_strErrMsg.Format("输入路径【%s】不存在!",strFolder);
		return false;
	}
	CString strFolderName = strFolder.Mid(strFolder.ReverseFind('\\')+1);
	CString strFolderPath = strFolder;

	Items.m_bIsFile = false;
	Items.m_strPath = strFolder;
	if (RelativePath)
		Items.m_strPath = strFolderName;
	
	CStringW strFolderW = (WCHAR*)_bstr_t(strFolder);
	WCHAR dir[MAX_PATH] = {0};
	WCHAR srcFileName[MAX_PATH] = {0};
	WCHAR desFileName[MAX_PATH] = {0};
	WCHAR *str = L"\\*.*";
	StrCpyW(dir,strFolderW);
	StrCatW(dir,str);

	//首先查找dir中符合要求的文件
	HANDLE hFile = NULL;
	WIN32_FIND_DATAW FindData;
	if ((hFile = FindFirstFileW(dir,&FindData)) !=NULL)
	{
		do
		{
			StrCpyW(srcFileName,strFolderW);
			StrCatW(srcFileName,L"\\");
			StrCatW(srcFileName,FindData.cFileName);
			//检查是不是目录
			//如果不是目录,则进行处理文件夹下面的文件
			if (!(FindData.dwFileAttributes & _A_SUBDIR) && bInCludeFile)
			{
				Folder_Item itf;
				itf.m_bIsFile = true;
				itf.m_strPath = srcFileName;
				if (RelativePath)
					itf.m_strPath = FindData.cFileName;
				Items.m_SubItem.push_back(itf);
			}
			else//处理目录，递归调用
			{
				if ( StrCmpW(FindData.cFileName, L"." ) != 0 && StrCmpW(FindData.cFileName, L".." ) != 0)
				{
					Folder_Item itf;
					itf.m_bIsFile = false;
					itf.m_strPath = srcFileName;
					if (RelativePath)
						itf.m_strPath = FindData.cFileName;

		
					CString str = (LPCTSTR)_bstr_t(srcFileName);
					if (bRecursion) // 如果递归
						GetFolderItems(str,itf,bRecursion,bInCludeFile,bIncludeDir,RelativePath);

					if (bIncludeDir || itf.m_SubItem.size() > 0)
						Items.m_SubItem.push_back(itf);
				}
			}
		} while (FindNextFileW(hFile,&FindData));
		FindClose(hFile);
	}
	return true;
}
vector<SelectItem> CFilePro::GetSelectItems(const char * pszSelMode,const char *pszFilter)
{
	if (pszSelMode==NULL && pszSelMode=="") pszSelMode = "fileAndFolder"; // 检查参数或设为默认值

	vector<SelectItem> vResult;
	BOOL bInCludeDir  = FALSE;
	BOOL bMultiSelect = FALSE;
	BOOL bInCludeFile = FALSE;
	if (_stricmp(pszSelMode,"singleFile")==0)
	{
		bInCludeFile = TRUE;
	}
	else if (_stricmp(pszSelMode,"singleFolder")==0)
	{
		bInCludeDir  = TRUE;
		bInCludeFile = TRUE;
		pszFilter = "文件夹|zzzzzzzzz.zzzzzzzzzzzzzzzzzzzzzz||";
	}
	else if (_stricmp(pszSelMode,"singleItem")==0)
	{
		bInCludeFile = TRUE;
		bInCludeDir  = TRUE;
		bMultiSelect = FALSE;
	}
	else if (_stricmp(pszSelMode,"multiFile")==0)
	{
		bInCludeFile = TRUE;
		bMultiSelect = TRUE;
	}
	else if (_stricmp(pszSelMode,"multiFolder")==0)
	{
		bInCludeDir  = TRUE;
		bMultiSelect = TRUE;
		pszFilter = "文件夹|zzzzzzzzz.zzzzzzzzzzzzzzzzzzzzzz||";
	}
	else if (_stricmp(pszSelMode,"fileAndFolder")==0)
	{
		bInCludeDir  = TRUE;
		bInCludeFile = TRUE;
		bMultiSelect = TRUE;
	}
	else
	{
		m_strErrMsg.Format("参数【%s】不正确",pszSelMode);
		return vResult;
	}

	DWORD flags = 6UL;
	if (bMultiSelect)
		flags = OFN_ALLOWMULTISELECT|OFN_PATHMUSTEXIST;

	CFileSelectDlg dlg(TRUE,NULL,NULL,flags,pszFilter,NULL,0UL,FALSE);
	dlg.m_ofn.lpstrTitle   =   "选择要上传的文件";                                    //至少允许选择100个文件
	dlg.m_ofn.lpstrFile = new TCHAR[MAX_PATH * SELECT_MAX_FILE_NUMBER]; //重新定义缓冲区大小          
	memset(dlg.m_ofn.lpstrFile, 0, MAX_PATH * SELECT_MAX_FILE_NUMBER);  //初始化定义的缓冲区
	dlg.m_ofn.nMaxFile = MAX_PATH*SELECT_MAX_FILE_NUMBER; // 指定缓冲区大小

	char szDir[MAX_PATH] = {0};
	GetModuleFileName(AfxGetInstanceHandle(),szDir,MAX_PATH);
	CString strConfigINI = szDir;
	strConfigINI.MakeLower();
	strConfigINI = strConfigINI.Left(strConfigINI.ReverseFind('.') + 1) + "ini";
	CHAR szDefaultpath[MAX_PATH];
	SHGetSpecialFolderPath(NULL,szDefaultpath,CSIDL_DESKTOPDIRECTORY,FALSE); // 找不到则默认为桌面路径
	CString strLastDir = "";
	GetPrivateProfileString("setting","LastSelectFolder",szDefaultpath,strLastDir.GetBuffer(512),512,strConfigINI);
	if (!PathFileExists(strLastDir))
		strLastDir = szDefaultpath; // 上次的路径不存在时，使用桌面路径
	dlg.m_ofn.lpstrInitialDir = strLastDir;
	strLastDir.ReleaseBuffer();

	if (dlg.DoModal() != IDOK)
	{
		m_strErrMsg = "用户取消";
		delete[] dlg.m_ofn.lpstrFile;     //最后别忘了释放内存
		return vResult;
	}
	
	strLastDir = CFileSelectDlg::m_strCurrendDirectory;
	WritePrivateProfileString("setting","LastSelectFolder",strLastDir,strConfigINI);
	int iSize = CFileSelectDlg::m_SelectedItemList.GetCount();
	for (int i=0;i<iSize;i++)
	{
		_SelectItem Item;
		Item.m_strPath =  CFileSelectDlg::m_SelectedItemList[i];
		Item.m_bIsFile = true;
		if (IsAccessibleDir(Item.m_strPath))
		{
			if (!bInCludeDir)
				continue; // 不包含目录时跳过选择的目录

			Item.m_bIsFile = false;
		}

		vResult.push_back(Item);
	}
	delete[] dlg.m_ofn.lpstrFile;     //最后别忘了释放内存
	return vResult;
}
void CFilePro::GetFolderItemCount(const struct _Folder_Item & fItem,int & iFolderCount,int &iFileCount,const bool & bRecursion)
{
	iFolderCount = 0;
	iFileCount   = 0;
	if (fItem.m_bIsFile)
		return;

	for (size_t j=0;j<fItem.m_SubItem.size();j++)
	{
		if (fItem.m_SubItem[j].m_bIsFile) 
			iFileCount++;
		else  // 是目录
		{
			iFolderCount++;
			if (bRecursion)
			{
				int iFile   = 0;
				int iFolder = 0;
				GetFolderItemCount(fItem.m_SubItem[j],iFolder,iFile,bRecursion); // 递归
				iFileCount += iFile;
				iFolderCount += iFolder;
			}
		}
	}
}
bool CFilePro::RemoveDir(const CString & strDir)
{
	CString strWildCard  = strDir;
	// 查找文件
	CStringW strFolderW = (WCHAR*)_bstr_t(strDir);
	WCHAR dir[MAX_PATH] = {0};
	WCHAR FileName[MAX_PATH] = {0};
	WCHAR *str = L"\\*.*";
	StrCpyW(dir,strFolderW);
	StrCatW(dir,str);

	//首先查找dir中符合要求的文件
	HANDLE hFile = NULL;
	WIN32_FIND_DATAW FindData;
	if ((hFile = FindFirstFileW(dir,&FindData)) !=NULL)
	{
		do 
		{
			//找到文件的路径
			CStringW strPathNameW = FindData.cFileName;
			if (strPathNameW.Compare(L".")==0 || strPathNameW.Compare(L"..")==0)
				continue;

			strPathNameW = strFolderW + CStringW(L"\\") +  strPathNameW;

			DWORD attr = GetFileAttributesW(strPathNameW);
			if((attr != 0xFFFFFFFF) && (attr & FILE_ATTRIBUTE_DIRECTORY))
				RemoveDir((LPCTSTR)_bstr_t(strPathNameW));
			else
			{
				if (!::DeleteFileW(strPathNameW))
				{
					m_strErrMsg.Format("删除文件%s失败",(LPCTSTR)_bstr_t(strPathNameW));
					ZTools::WriteZToolsFormatLog(m_strErrMsg);
					FindClose(hFile);
					return false;
				}
			}
		} while (FindNextFileW(hFile,&FindData));
	}
	FindClose(hFile);

	if (!::RemoveDirectoryW(strFolderW))
	{
		m_strErrMsg.Format("删除目录%s失败",strDir);
		ZTools::WriteZToolsFormatLog(m_strErrMsg);
		return false;
	}
	return true;
}