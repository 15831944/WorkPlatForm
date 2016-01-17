#include "stdafx.h"
#include "FilePro.h"
#include <Windows.h>
#include<io.h>
#include <direct.h>
#include <ShellAPI.h>
#include "Tools.h"
#include<shobjidl.h>
#include <ShlObj.h>
#include<afxwin.h>
void RefreshShellNewReg()
{
	LPSHELLFOLDER DesktopFolder;
	SHGetDesktopFolder(&DesktopFolder);
	if (DesktopFolder!=NULL)
	{
		LPSHELLVIEW view;
		::CoInitialize(NULL);
		HRESULT hs = DesktopFolder->CreateViewObject( NULL, IID_IShellView, (void**)&view);
		if (view!=NULL)
		{
			LPCONTEXTMENU CM;
			view->GetItemObject(SVGIO_BACKGROUND, IID_IContextMenu, (void**)&CM);
			if (CM!=NULL)
			{
				CMINVOKECOMMANDINFO CI;
				ZeroMemory(&CI, sizeof(CI));
				CI.cbSize = sizeof(CMINVOKECOMMANDINFO);
				CI.hwnd = NULL;

				HMENU hMenu = CreatePopupMenu();
				DWORD Flags = CMF_EXPLORE;

				hs = CM->QueryContextMenu(hMenu, 0, 0,UINT(-1), Flags);
				DestroyMenu(hMenu);
			}
		}
		::CoUninitialize();
	}

	////刷新桌面的图标缓存
	//HWND hWnd   =   ::GetDesktopWindow();  
	//hWnd   =   ::FindWindowEx(hWnd,   0,   "Progman",   0);  
	//hWnd   =   ::FindWindowEx(hWnd,   0,   "SHELLDLL_DefView",   0);  
	//hWnd   =   ::FindWindowEx(hWnd,   0,   "SysListView32",   0);   //这时hWnd才是你需要的那个
	//::PostMessage(hWnd,WM_RBUTTONDOWN,MK_RBUTTON,MAKELONG(6,6));
	//::PostMessage(hWnd,WM_RBUTTONUP,MK_RBUTTON,MAKELONG(6,6));
	//::PostMessage(hWnd,WM_LBUTTONDOWN,MK_LBUTTON,MAKELONG(4,4));
	//::PostMessage(hWnd,WM_LBUTTONUP,MK_LBUTTON,MAKELONG(4,4));
	//Sleep(500);
}
static const char * g_OfficeTypeFilter[] =
{
	".accdb",		 // access
	".doc",".docx", // word
	".mpp",         // project
	".pptx",".ppt", // powerpoint
	".pub",         // publisher
	".vsd",         //visio
	".xls",".xlsx"  // excel
};
/************************************************************************/
/* 在此字符串数组常量中添加过滤类型即可实现过滤                     */
/************************************************************************/
static const char *g_TypeFilter[] = {
				".contact", //联系人
				".library-ms",//库
				"Briefcase",//公文包
				"Folder",  //文件夹
				".lnk",   //快捷方式
				".bfc"    //xp下的公文包
			};

static bool IsUsefullType(const char *pszType)
{
	static int iSize = sizeof(g_TypeFilter)/sizeof(const char*);
	int i = 0;
	while(i<iSize)
	{
		if(_stricmp(g_TypeFilter[i],pszType)==0)
			return false; //被过滤掉
		i++;
	}
	return true;
}
static bool IsOfficeType(const char *pszType)
{
	static int iSize = sizeof(g_OfficeTypeFilter)/sizeof(const char*);
	int i = 0;
	while(i<iSize)
	{
		if(_stricmp(g_OfficeTypeFilter[i],pszType)==0)
			return true; //保留office类型
		i++;
	}
	return false;
}
std::string ws2s(const std::wstring& ws)
{
	_bstr_t bstr(ws.c_str());
	string strOut = (const char*)bstr;
	return strOut;
}
string GetWindowVesion()
{
	OSVERSIONINFO   osversioninfo;
	osversioninfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osversioninfo);
	if( (osversioninfo.dwMajorVersion == 5) && (osversioninfo.dwMinorVersion == 1)) 
		return "xp";

	if( (osversioninfo.dwMajorVersion == 6) && (osversioninfo.dwMinorVersion == 1))
		return "win7";

	if( (osversioninfo.dwMajorVersion == 6) && (osversioninfo.dwMinorVersion == 2))
		return "win8";

	return "xp";
}
void GetXpTypeItems(vector<NewType * > &vAllItems,BOOL bOnlyOfficeType)
{
	ZTools::WriteZToolsFormatLog("GetXpTypeItems进入...");
	HKEY   hFirstKey,keyFinal;
	LSTATUS lResult = 0;
	string strPath = "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Discardable\\PostSetup\\ShellNew";
	int iTryCount = 0;
_Retry:
	iTryCount ++;
	lResult = RegOpenKey(HKEY_CURRENT_USER,NULL,&hFirstKey);
	if(ERROR_SUCCESS!=lResult)
	{
		ZTools::WriteZToolsFormatLog("GetXpTypeItems RegOpenKey打开键值【HKEY_CURRENT_USER】失败,RegOpenKey返回【%d】,GetXpTypeItems返回",lResult);
		string strErr = ZTools::GetLastErrorMessageString(lResult).c_str();
		ZTools::WriteZToolsFormatLog("错误代码描述:%s",strErr.c_str());
		return;
	}
	lResult = RegOpenKey(hFirstKey,strPath.c_str(),&keyFinal);
	if(ERROR_SUCCESS!=lResult) 
	{
		ZTools::WriteZToolsFormatLog("GetXpTypeItems RegOpenKey打开键值【HKEY_CURRENT_USER\\%s】失败,RegOpenKey返回【%d】,GetXpTypeItems返回",strPath.c_str(),lResult);
		string strErr = ZTools::GetLastErrorMessageString(lResult).c_str();
		ZTools::WriteZToolsFormatLog("错误代码描述:%s",strErr.c_str());
		RegCloseKey(hFirstKey);
		if(lResult!=2 || iTryCount>2)
			return;

		RefreshShellNewReg();
		Sleep(500);
		ZTools::WriteZToolsFormatLog("进行第【%d】次重试",iTryCount);
		goto _Retry;
	}

	DWORD dwType;
	DWORD dwSize = 1000;
	WCHAR  szString[1000] = {0};

	DWORD   Item=0,dwLen=MAX_PATH; 
	char   ValNameStr[MAX_PATH];
	while(ERROR_SUCCESS==RegEnumValue(keyFinal,Item++,ValNameStr,&dwLen,NULL,&dwType,(BYTE *)szString,&dwSize))
	{
		dwLen = MAX_PATH;
		if (dwType!=REG_BINARY || _stricmp("~reserved~",ValNameStr)==0)
		{
			memset(szString,0,sizeof(1000));
			ZTools::WriteZToolsFormatLog("GetXpTypeItems 过滤掉~reserved~或者非REG_BINARY类型,Item=%d",Item);
			continue;
		}

		NewType *nt = new NewType;
		nt->m_strDesrciption = ValNameStr;
		wstring wstr = szString + 71;
		nt->m_strExt = ws2s(wstr);
		if (IsUsefullType(nt->m_strExt.c_str()))
		{
			if (bOnlyOfficeType)
			{
				if (!IsOfficeType(nt->m_strExt.c_str()))
				{
					ZTools::WriteZToolsFormatLog("过滤掉非Office类型【%s】,Item=%d",nt->m_strExt.c_str(),Item);
					memset(szString,0,sizeof(1000));
					continue; // 只保留office类型时，过滤掉不是office的文件类型
				}
			}
			vAllItems.push_back(nt);
			ZTools::WriteZToolsFormatLog("保留类型扩展名【%s】,类型描述【%s】,Item=%d",nt->m_strExt.c_str(),nt->m_strDesrciption.c_str(),Item);
		}
		else
			ZTools::WriteZToolsFormatLog("IsUsefullType过滤掉【%s】,Item=%d",nt->m_strExt.c_str(),Item);

		memset(szString,0,sizeof(1000));
	}

	RegCloseKey(keyFinal);
	RegCloseKey(hFirstKey);
}
static bool GetShellNewKeyPath(HKEY keyExt,string & strKeyPath)
{
	// 先从后缀名的默认值下找ShellNew
	HKEY keyFinal;
	DWORD dwType;
	DWORD dwSize = 1000;
	char  szString[1000] = {0};
	LONG lReturn = ::RegQueryValueEx (keyExt,"", NULL,&dwType, (BYTE *) szString, &dwSize);
	if (lReturn==ERROR_SUCCESS)
	{
		string strTmp =  string(szString) + string("\\ShellNew");
		if(ERROR_SUCCESS==RegOpenKey(keyExt,strTmp.c_str(),&keyFinal)) // 在默认值下找ShellNew,例如.xlsx\Excel.Sheet.12\ShellNew
		{
			strKeyPath  = strKeyPath + string("\\") + strTmp;
			ZTools::WriteZToolsFormatLog("从后缀名的默认值下找ShellNew成功!");
			RegCloseKey(keyFinal);
			return true;
		}
	}

	// 后缀名的默认值下找ShellNew失败,则直接从后缀名下找ShellNew，例如.xlsx\ShellNew
	if(ERROR_SUCCESS==RegOpenKey(keyExt,"ShellNew",&keyFinal)) 
	{
		strKeyPath +="\\ShellNew";
		RegCloseKey(keyFinal);
		ZTools::WriteZToolsFormatLog("直接从后缀名下找ShellNew成功!");
		return true;
	}

	return false;
}
static bool IsRightNewItem(HKEY keyShellNew,string & strKeyPath)
{
	//遍历ShellNew子键
	DWORD   Item=0,dwLen=MAX_PATH; 
	char   SubKeyStr[MAX_PATH];
	string strTmpPath = strKeyPath;
	while(ERROR_SUCCESS==RegEnumKeyEx(keyShellNew,Item++,SubKeyStr,&dwLen,NULL,NULL,NULL,NULL))   
	{
		strTmpPath = strKeyPath + string("\\") + SubKeyStr;

		dwLen = MAX_PATH; //每次调用RegEnumKeyEx前必须重新将KeySize的值设为KeyMaxLen缓冲区的大小，否则遍历失败
		if (_stricmp(SubKeyStr,"ShellNew")==0) //找到SHellNew子键
		{
			int a = 0;
			strKeyPath = strTmpPath;
			return true;
		}
		else //未找到SHellNew子键，则遍历该键下的子键
		{
			HKEY keySub;
			if (RegOpenKey(keyShellNew,SubKeyStr,&keySub)!=ERROR_SUCCESS)
				continue;

			if (IsRightNewItem(keySub,strTmpPath))
			{
				strKeyPath = strTmpPath;
				return true;
			}
			else
				continue;
		}

	}

	return false;
}

static bool GetTypeName(string strFile,string & strTypeName)
{
	SHFILEINFOA info;

	if(SHGetFileInfoA(strFile.c_str(),FILE_ATTRIBUTE_NORMAL,
		&info,sizeof(info),SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES))
	{
		strTypeName = info.szTypeName;
		return true;
	}

	return false;
}

static bool CreateNewFolder(string strDir,string & strNewPath)
{
	string strNewFileName = strDir + "\\新建文件夹";
	int iTryCount = 1;
	while (PathFileExists(strNewFileName.c_str()))
	{
		iTryCount++;
		CString strNewTypeComment;
		strNewTypeComment.Format("新建文件夹(%d)",iTryCount);
		strNewFileName =  strDir  + string("\\") + (LPCTSTR)strNewTypeComment;
	}
	
	if (_mkdir(strNewFileName.c_str())==0)
	{
		strNewPath = strNewFileName;
		return true;
	}

	return false;
}
void CFilePro::GetAllDesktopRightMenuNewItems(vector<NewType *> &vAllItems,BOOL bOnlyOfficeType)
{
	ZTools::WriteZToolsFormatLog("GetAllDesktopRightMenuNewItems进入");
	if(GetWindowVesion()=="xp")
		return GetXpTypeItems(vAllItems,bOnlyOfficeType);

	ZTools::WriteZToolsFormatLog("非Xp类型系统");
	HKEY   hFirstKey,keyFinal;
	LSTATUS lResult = 0;
	string strPath = "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Discardable\\PostSetup\\ShellNew";
	int iTryCount = 0;
_Retry:
	iTryCount ++;
	lResult = RegOpenKey(HKEY_CURRENT_USER,NULL,&hFirstKey);
	if(ERROR_SUCCESS!=lResult)
	{
		ZTools::WriteZToolsFormatLog("RegOpenKey打开键值【HKEY_CURRENT_USER】失败,RegOpenKey返回【%d】",lResult);
		string strErr = ZTools::GetLastErrorMessageString(lResult).c_str();
		ZTools::WriteZToolsFormatLog("错误代码描述:%s",strErr.c_str());
		return;
	}
	lResult = RegOpenKey(hFirstKey,strPath.c_str(),&keyFinal);
	if(ERROR_SUCCESS!=lResult) 
	{
		ZTools::WriteZToolsFormatLog("RegOpenKey打开键值【HKEY_CURRENT_USER\\%s】失败,RegOpenKey返回【%d】",strPath.c_str(),lResult);
		string strErr = ZTools::GetLastErrorMessageString(lResult).c_str();
		ZTools::WriteZToolsFormatLog("错误代码描述:%s",strErr.c_str());
		RegCloseKey(hFirstKey);
		if(lResult!=2 || iTryCount>2)
			return;

		RefreshShellNewReg();
		Sleep(500);
		ZTools::WriteZToolsFormatLog("进行第【%d】次重试",iTryCount);
		goto _Retry;
	}


	DWORD dwType;
	DWORD dwSize = 1000;
	char  szString[1000] = {0};

	LONG lReturn = ::RegQueryValueEx (keyFinal,"Classes", NULL,
		&dwType, (BYTE *) szString, &dwSize);

	RegCloseKey(hFirstKey);
	if(lReturn != ERROR_SUCCESS)
		return;

	string strNew = "";
	for(size_t i=0;i<dwSize;i++)
	{
		if (szString[i]==0 && strNew.size() > 0)
		{

			if(!IsUsefullType(strNew.c_str()))
			{
				strNew = "";
				continue;
			}
			if (bOnlyOfficeType)
			{
				if (!IsOfficeType(strNew.c_str()))
				{
					strNew = "";
					continue; // 只保留office类型时，过滤掉不是office的文件类型
				}
			}

			NewType  *newType = new NewType;
			newType->m_strExt = strNew;
			GetTypeName(newType->m_strExt,newType->m_strDesrciption);
			vAllItems.push_back(newType);
			strNew = "";
		}
		else
			strNew.append(1,szString[i]);
	}
}
bool CFilePro::CreateNewItem(string strItemType,string strDir,string &strNewPath)
{
	string strTypeComment = strNewPath;
	if (strTypeComment == "")
	{
		m_strErrMsg = "新建文件名为空!";
		return false;
	}
	/*if (!GetTypeName(strItemType,strTypeComment))
		strTypeComment = "新建项";*/

	HKEY   hFirstKey,keyExt,keyFinal;
	if(ERROR_SUCCESS!=RegOpenKey(HKEY_CLASSES_ROOT,NULL,&hFirstKey))
	{
		m_strErrMsg = "打开注册表键HKEY_CLASSES_ROOT失败";
		return false;
	}

	if(ERROR_SUCCESS!=RegOpenKey(hFirstKey,strItemType.c_str(),&keyExt))
	{
		m_strErrMsg.Format("打开注册表键HKEY_CLASSES_ROOT\\%s失败",strItemType.c_str());
		RegCloseKey(hFirstKey);
		return false;
	}

	string strShellNewPath = strItemType;
	if (!GetShellNewKeyPath(keyExt,strShellNewPath)) 
	{
		m_strErrMsg = "在注册表中未找到该类型的文件创建方法ShellNew";
		RegCloseKey(keyExt);
		RegCloseKey(hFirstKey);
		return false;
	}

	/*if (!IsRightNewItem(keyExt,strShellNewPath)) 
	{
		m_strErrMsg = "在注册表中未找到该类型的文件创建方法ShellNew";
		return false;
	}*/

	if(ERROR_SUCCESS!=RegOpenKey(hFirstKey,strShellNewPath.c_str(),&keyFinal)) 
	{
		m_strErrMsg.Format("打开注册表键HKEY_CLASSES_ROOT\\%s失败",strShellNewPath);
		RegCloseKey(keyExt);
		RegCloseKey(hFirstKey);
		return false;
	}

	DWORD dwType;
	DWORD dwSize = 1000;
	char  szString[1000] = {0};

	LONG lReturn = ::RegQueryValueEx (keyFinal,"FileName", NULL,
		&dwType, (BYTE *) szString, &dwSize);

	ZTools::WriteZToolsFormatLog("RegQueryValueEx获取模版文件名【%s】",szString);

	int iTryCount = 1;
	string strSplit = "\\";
	int iLen = strDir.length();
	if (iLen >0 && strDir[iLen-1] == '\\')
		strSplit = "";

	string strNewFileName = strDir  + strSplit + strTypeComment + strItemType;
	wstring strW = ZTools::s2ws(strNewFileName);
	if (strW.length() >MAX_PATH)
	{
		m_strErrMsg = "文件路径过长！";
		m_iErrCode = 1;
		return false;
	}

	if (PathFileExists(strNewFileName.c_str()))
	{
		// 业务需求，如果文件已经存在，则直接覆盖
		//m_strErrMsg.Format("文件【%s】已经存在，不能创建同名文件!",strNewFileName.c_str());
		//return false;
		ZTools::WriteZToolsFormatLog("【%s】文件已经存在，remove删除该文件...",strNewFileName.c_str());
		remove(strNewFileName.c_str());
	}
	//while(access(strNewFileName.c_str(),0)==0) //如果文件存在，则在名字后加后缀
	//{
	//	iTryCount++;
	//	CString strNewTypeComment;
	//	strNewTypeComment.Format("%s(%d)",strTypeComment.c_str(),iTryCount);
	//	strNewFileName =  strDir  + string("\\") + string((LPCTSTR)strNewTypeComment) + strItemType;
	//}

	if(ERROR_SUCCESS == lReturn) // 有模板的文件类型
	{
		string strSource = szString;
		if(strSource.find(':')== string::npos) // 不带盘符的模版文件名称
		{
			char szWinDir[MAX_PATH] = {0};
			GetWindowsDirectory(szWinDir,MAX_PATH); // 不带盘符的模版文件名称到默认模版目录下去寻找
			strSource = string(szWinDir) + string("\\ShellNew\\") + szString;
			ZTools::WriteZToolsFormatLog("使用默认的模版文件路径【%s】",strSource.c_str());
		}
		
		if (PathFileExists(strSource.c_str())) //模版文件存在
		{
			RegCloseKey(keyExt);
			RegCloseKey(hFirstKey);
			RegCloseKey(keyFinal);

			ZTools::WriteZToolsFormatLog("模版文件【%s】存在",strSource.c_str());
			ZTools::WriteZToolsFormatLog("MakeSureDirectoryPathExists调用【%s】",strNewFileName.c_str());
			if (!MakeSureDirectoryPathExists(strNewFileName.c_str()))
			{
				m_strErrMsg = ZTools::GetLastErrorMessageString().c_str();
				return false;
			}

			ZTools::WriteZToolsFormatLog("拷贝模版文件：CopyFile源=【%s】，目的=【%s】",strSource.c_str(),strNewFileName.c_str());
			if(::CopyFile(strSource.c_str(),strNewFileName.c_str(),TRUE))
			{
				strNewPath = strNewFileName;
				return true;
			}
			ZTools::WriteZToolsFormatLog("CopyFile失败！！！！！！！");
			m_strErrMsg = ZTools::GetLastErrorMessageString().c_str();
			return false;
		}
		else
		{
			ZTools::WriteZToolsFormatLog("模版文件【%s】无法找到，将创建空白文件，该文件可能无法正常打开！",strSource.c_str());
		}
	}

	lReturn = ::RegQueryValueEx (keyFinal,"NullFile", NULL,
		&dwType, (BYTE *) szString, &dwSize);

	RegCloseKey(keyExt);
	RegCloseKey(keyFinal);
	RegCloseKey(hFirstKey);
	

	ZTools::WriteZToolsFormatLog("RegQueryValueEx获取NullFile，返回值为【%ld】",lReturn);

	if (_stricmp(strItemType.c_str(),"Folder")==0) //创建文件夹
	{
		ZTools::WriteZToolsFormatLog("CreateNewFolder【%s】,【%s】",strDir.c_str(),strNewPath.c_str());
		return CreateNewFolder(strDir,strNewPath);
	}
	else
	{
		if (lReturn!= ERROR_SUCCESS)
		{
			ZTools::WriteZToolsFormatLog("系统中未找到[%s]文件的创建方法,本程序将创建空的文件，该文件可能不能正确打开!"
				,strItemType.c_str());
		}

		ZTools::WriteZToolsFormatLog("MakeSureDirectoryPathExists调用【%s】,准备创建空白文件",strNewFileName.c_str());
		if (!MakeSureDirectoryPathExists(strNewFileName.c_str()))
		{
			m_strErrMsg = ZTools::GetLastErrorMessageString().c_str();
			return false;
		}

		ZTools::WriteZToolsFormatLog("fopen_s调用【%s】,模式【w】",strNewFileName.c_str());
		FILE *pFile = NULL;
		fopen_s(&pFile,strNewFileName.c_str(),"w");
		if (pFile==NULL)
		{
			m_strErrMsg = ZTools::GetLastErrorMessageString().c_str();
			ZTools::WriteZToolsFormatLog("fopen_s【w】模式失败！");
			fopen_s(&pFile,strNewFileName.c_str(),"wb+");
			if (pFile==NULL)
			{
				ZTools::WriteZToolsFormatLog("fopen_s【wb+】模式失败！");
				return false;
			}
			else

				ZTools::WriteZToolsFormatLog("fopen_s【wb+】模式成功！");
		}

		int iRev = fclose(pFile);
		ZTools::WriteZToolsFormatLog("fclose返回【%d】",iRev);
		strNewPath = strNewFileName;
	}

	return true;
}