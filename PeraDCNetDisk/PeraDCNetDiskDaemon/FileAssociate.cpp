#include "stdafx.h"
#include "FileFunc.h"
#pragma comment(lib, "version.lib")
BOOL IsWow64() 
{ 
	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL); 
	LPFN_ISWOW64PROCESS fnIsWow64Process = NULL; 
	BOOL bIsWow64 = FALSE; 
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress( GetModuleHandle("kernel32"),"IsWow64Process"); 
	if (NULL != fnIsWow64Process) 
	{ 
		fnIsWow64Process(GetCurrentProcess(),&bIsWow64);
	} 
	return bIsWow64; 
}
/************************************************************************/
/* 根据PerceivedType获取关联程序                                           */
/************************************************************************/
BOOL GetCmdByPerceivedType(const CString & strPerceivedType,CString &strCmd)
{
	HKEY   hFirstKey,keyFinal;
	if(ERROR_SUCCESS!=RegOpenKey(HKEY_CLASSES_ROOT,NULL,&hFirstKey))
		return FALSE;

	//HKEY_CLASSES_ROOT\SystemFileAssociations\*\shell\open\command
	CString strKeyPath = CString("SystemFileAssociations\\") + strPerceivedType + "\\shell\\open\\command"; 
	if(ERROR_SUCCESS!=RegOpenKey(hFirstKey,strKeyPath,&keyFinal))
	{
		RegCloseKey(hFirstKey);
		return FALSE;
	}

	DWORD dwType;
	DWORD dwSize = 1000;
	char  szString[1000] = {0};
	LONG lReturn = ::RegQueryValueEx (keyFinal,"", NULL,&dwType, (BYTE *) szString, &dwSize);
	RegCloseKey(hFirstKey);
	RegCloseKey(keyFinal);
	if (lReturn==ERROR_SUCCESS)
	{
		strCmd = szString;
		return TRUE;
	}

	return FALSE;
}
/************************************************************************/
/* 从shell命令行中提取exe路径字符串                                   */
/************************************************************************/
CString GetExePathFromCmd(CString strCmd)
{
	CString strAppPath  = strCmd;
	strAppPath.MakeLower();
	int iPosFind = strAppPath.Find("rundll32.exe");
	if (iPosFind >=0) 
	{
		int iLen =strlen("rundll32.exe");
		strAppPath = strAppPath.Mid(iPosFind+iLen);
		if (strCmd.Left(1)=="\"")
			strAppPath.TrimLeft("\"");

		strAppPath.TrimLeft(" ");
		iPosFind = strAppPath.Find(",");
		if (iPosFind<=0)
			return "";

		strAppPath = strAppPath.Left(iPosFind);
		strAppPath.TrimLeft("\"");
		strAppPath.TrimRight("\"");
		return strAppPath;
	}

	if (strAppPath.Left(1)!="\"") //路径没有引号括起来的
	{
		int iPos = strAppPath.Find(" ");
		strAppPath = strAppPath.Left(iPos);
	}
	else
	{
		int iPos = strAppPath.Find("\" ");
		strAppPath = strAppPath.Mid(1,iPos);
	}

	if (strAppPath.Left(1)=="\"")
		strAppPath = strAppPath.Mid(1);
	if (strAppPath.Right(1)=="\"")
		strAppPath = strAppPath.Mid(0,strAppPath.GetLength()-1);

	return strAppPath;
}
/************************************************************************/
/* HKEY_CLASSES_ROOT\$(strProID)下其他shell命令                           */
/************************************************************************/
BOOL GetOpenCmdFormProgId(const CString & strProID,CString & strCmd)
{
	HKEY   hFirstKey,keyFinal;
	if(ERROR_SUCCESS!=RegOpenKey(HKEY_CLASSES_ROOT,NULL,&hFirstKey))
	{
		return FALSE;
	}
	CString strCmdKeyPath = CString(strProID) + "\\shell\\open\\command";
	if(ERROR_SUCCESS!=RegOpenKey(hFirstKey,strCmdKeyPath,&keyFinal))
	{
		RegCloseKey(hFirstKey);
		return FALSE;
	}

	DWORD dwType;
	DWORD dwSize = 1000;
	char  szString[1000] = {0};
	LONG lReturn = ::RegQueryValueEx (keyFinal,"", NULL,&dwType, (BYTE *) szString, &dwSize);
	RegCloseKey(hFirstKey);
	RegCloseKey(keyFinal);
	if (lReturn!=ERROR_SUCCESS)
		return FALSE;

	strCmd = szString;
	if (strCmd.IsEmpty())
		return FALSE;

	return TRUE;
}
/************************************************************************/
/* 从HKEY_CLASSES_ROOT\$(strFileExt)下查找shell命令                  */
/************************************************************************/
BOOL HaveOpenCmd(LPCTSTR strFileExt,CString & strCmd,CString & strPerceivedType,int & iErrCode)
{
	HKEY   hFirstKey,keyFinal;
	iErrCode = 0;
	if(ERROR_SUCCESS!=RegOpenKey(HKEY_CLASSES_ROOT,NULL,&hFirstKey))
	{
		iErrCode = 1;
		return FALSE;
	}

	if(ERROR_SUCCESS!=RegOpenKey(hFirstKey,strFileExt,&keyFinal))
	{
		iErrCode = 2;
		RegCloseKey(hFirstKey);
		return FALSE;
	}


	DWORD dwType;
	DWORD dwSize = 1000;
	char  szString[1000] = {0};
	LONG lReturn = ::RegQueryValueEx (keyFinal,"PerceivedType", NULL,&dwType, (BYTE *) szString, &dwSize);
	if (lReturn==ERROR_SUCCESS)
		strPerceivedType = szString;

	dwSize = 1000;
	lReturn = ::RegQueryValueEx (keyFinal,"", NULL,&dwType, (BYTE *) szString, &dwSize);
	RegCloseKey(hFirstKey);
	RegCloseKey(keyFinal);
	if (lReturn!=ERROR_SUCCESS)
	{
		iErrCode = 3;
		return FALSE;
	}
	return GetOpenCmdFormProgId(szString,strCmd);
}
/************************************************************************/
/* 获取exe关联的程序，返回true，则strAppName非空，返回false，则用别的方式获取关联程序                                                                     */
/************************************************************************/
BOOL GetAssociatedAppName(LPCTSTR strFileExt,CString &strAppName,CString &strErrMsg,BOOL &bUsrChoice)
{
	strErrMsg  = "";
	strAppName = "";
	bUsrChoice = FALSE;

	HKEY   hFirstKey,keyFinal;
	if(ERROR_SUCCESS!=RegOpenKey(HKEY_CURRENT_USER,NULL,&hFirstKey))
	{
		strErrMsg = "RegOpenKey 【HKEY_CURRENT_USER】失败！";
		return FALSE;
	}

	CString strPath = CString("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\") + CString(strFileExt) + "\\UserChoice";
	DWORD dwType;
	DWORD dwSize = 1000;
	char  szString[1000] = {0};
	if(ERROR_SUCCESS==RegOpenKey(hFirstKey,strPath,&keyFinal)) // 优先使用用户选择
	{
		LONG lReturn = ::RegQueryValueEx (keyFinal,"Progid", NULL,&dwType, (BYTE *) szString, &dwSize);
		RegCloseKey(hFirstKey);
		RegCloseKey(keyFinal);
		if (lReturn==ERROR_SUCCESS)
		{
			strAppName = szString; 
			bUsrChoice = TRUE;
			return TRUE;
		}
	}

	RegCloseKey(hFirstKey);
	return FALSE; // 返回FALSE,使用shell\open\command命令行中提取的exe路径
}
/************************************************************************/
/* 用于从HKEY_CURRENT_USER\\Software\\Classes\\Applications获取一个exe的路径  */
/************************************************************************/
BOOL GetApplicationPathFromReg(LPCTSTR strAppName,CString &strAppPath,CString &strErrMsg)
{
	strErrMsg  = "";
	strAppPath = "";

	HKEY   hFirstKey,keyFinal;
	if(ERROR_SUCCESS!=RegOpenKey(HKEY_CURRENT_USER,NULL,&hFirstKey))
	{
		strErrMsg = "RegOpenKey 【HKEY_CURRENT_USER】失败！";
		return FALSE;
	}

	CString strPath = CString("Software\\Classes\\Applications\\") + CString(strAppName) + "\\shell\\open\\command";
	if(ERROR_SUCCESS!=RegOpenKey(hFirstKey,strPath,&keyFinal))  
	{
		RegCloseKey(hFirstKey);
		strErrMsg.Format("RegOpenKey【%s】失败！",strPath);
		return FALSE;
	}

	DWORD dwType;
	DWORD dwSize = 1000;
	char  szString[1000] = {0};
	LONG lReturn = ::RegQueryValueEx (keyFinal,"", NULL,&dwType, (BYTE *) szString, &dwSize);
	RegCloseKey(hFirstKey);
	RegCloseKey(keyFinal);
	if (lReturn!=ERROR_SUCCESS)
	{
		strErrMsg.Format("RegQueryValueEx【%s】默认值失败！",strPath);
		return FALSE;
	}

	CString strCmd = szString;
	strAppPath = GetExePathFromCmd(strCmd);
	return TRUE;
}
/************************************************************************/
/* 获取exe文件的路径（从注册表和系统目录进程查找）              */
/************************************************************************/
BOOL GetAppPath(LPCTSTR strAppName,CString &strAppPath,CString &strErrMsg)
{
	strErrMsg  = "";
	strAppPath = "";

	HKEY   hFirstKey,keyFinal;
	if(ERROR_SUCCESS!=RegOpenKey(HKEY_LOCAL_MACHINE,NULL,&hFirstKey))
	{
		strErrMsg = "RegOpenKey 【HKEY_LOCAL_MACHINE】失败！";
		return FALSE;
	}

	CString strPath = CString("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\") + strAppName;
	if(ERROR_SUCCESS!=RegOpenKey(hFirstKey,strPath,&keyFinal))  
	{
		RegCloseKey(hFirstKey);
		CHAR szSyspath[MAX_PATH];
		SHGetSpecialFolderPath(NULL,szSyspath,CSIDL_WINDOWS,FALSE);
		CString strTmp;
		strTmp.Format("%s\\%s",szSyspath,strAppName);
		if (IsAccessibleFile(strTmp))
		{
			strAppPath = strTmp;
			return TRUE;
		}

		SHGetSpecialFolderPath(NULL,szSyspath,CSIDL_SYSTEM,FALSE);
		strTmp.Format("%s\\%s",szSyspath,strAppName);
		if (IsAccessibleFile(strTmp))
		{
			strAppPath = strTmp;
			return TRUE;
		}
		if (GetApplicationPathFromReg(strAppName,strAppPath,strErrMsg))
			return TRUE;

		strErrMsg.Format("RegOpenKey【%s】失败！",strPath);
		return FALSE;
	}

	DWORD dwType;
	DWORD dwSize = 1000;
	char  szString[1000] = {0};
	LONG lReturn = ::RegQueryValueEx (keyFinal,"", NULL,&dwType, (BYTE *) szString, &dwSize);
	RegCloseKey(hFirstKey);
	RegCloseKey(keyFinal);
	if (lReturn!=ERROR_SUCCESS)
	{
		strErrMsg.Format("RegQueryValueEx【%s】默认值失败！",strPath);
		return FALSE;
	}

	strAppPath = szString;
	return TRUE;
}
/************************************************************************/
/* 从文件版本信息中获取文件描述信息                                   */
/************************************************************************/
BOOL GetAppDescription(LPCTSTR strAppPath,CString &strAppDescription,CString &strErrMsg)
{
	strErrMsg  = "";
	strAppDescription = "";
	CString strBlockName;
	//找不到版本信息时，文件描述设置为文件的exe名称
	strAppDescription = strAppPath;
	strAppDescription = strAppDescription.Mid(strAppDescription.ReverseFind('\\')+1);

	DWORD dwSize = GetFileVersionInfoSize(strAppPath,NULL);
	if (dwSize==0)
	{
		//找不到版本信息时，文件描述设置为文件的exe名称
		if (!IsAccessibleFile(strAppPath))
		{
			strAppDescription = "未知应用程序";
			return TRUE;
		}
		return TRUE;
		/*strErrMsg.Format("GetFileVersionInfoSize【%s】,返回大小为0",strAppPath);
		return FALSE;*/
	}

	char *pVersionData = new char[dwSize];
	if (!GetFileVersionInfo(strAppPath,0,dwSize,pVersionData))
	{
		delete pVersionData;
		strErrMsg.Format("GetFileVersionInfoSize【%s】,获取【%d】大小的版本信息失败",strAppPath,dwSize);
		return FALSE;
	}

	UINT nQuerySize;
	DWORD* pTransTable;
	if (!::VerQueryValue(pVersionData, _T("\\VarFileInfo\\Translation"),(void **)&pTransTable, &nQuerySize) )
		goto _TEST_GET_;

	DWORD dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));

	LPVOID lpData;
	strBlockName.Format(_T("\\StringFileInfo\\%08lx\\%s"),dwLangCharset, _T("FileDescription"));

	if ( ::VerQueryValue((void **)pVersionData, strBlockName.GetBuffer(0),&lpData, &nQuerySize))
	{  
		strAppDescription = (LPCTSTR)lpData;
		strBlockName.ReleaseBuffer();
		delete pVersionData;
		return TRUE;
	}
_TEST_GET_:
	static const char * strLanguageList[] = {"040904e4","040904b0"}; 
	int iTestCount = sizeof(strLanguageList)/sizeof(const char*);
	int iTest = 0;
	while(1)
	{
		strBlockName.Format(_T("\\StringFileInfo\\%s\\%s"),strLanguageList[iTest],_T("FileDescription"));
		if ( ::VerQueryValue((void **)pVersionData, strBlockName.GetBuffer(0),&lpData, &nQuerySize))
		{
			strAppDescription = (LPCTSTR)lpData;
			break;
		}
		iTest++;
		if (iTest >= iTestCount)
			break;
	}

	strBlockName.ReleaseBuffer();
	delete pVersionData;
	return TRUE;

	if (strAppDescription.IsEmpty())
	{
		HKEY   hFirstKey,keyFinal;
		if(ERROR_SUCCESS!=RegOpenKey(HKEY_CURRENT_USER,NULL,&hFirstKey))
		{
			strErrMsg = "RegOpenKey 【HKEY_CURRENT_USER】失败！";
			return FALSE;
		}

		CString strPath = "Software\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\Shell\\MuiCache";
		if(ERROR_SUCCESS!=RegOpenKey(hFirstKey,strPath,&keyFinal))  
		{
			RegCloseKey(hFirstKey);
			strErrMsg.Format("RegOpenKey【%s】失败！",strPath);
			return FALSE;
		}

		DWORD dwType;
		DWORD dwSize = 1000;
		char  szString[1000] = {0};

		LONG lReturn = ::RegQueryValueEx (keyFinal,strAppPath, NULL,&dwType, (BYTE *) szString, &dwSize);
		RegCloseKey(hFirstKey);
		RegCloseKey(keyFinal);
		if (lReturn!=ERROR_SUCCESS)
		{
			strErrMsg.Format("RegQueryValueEx【%s】失败！","MRUList");
			return FALSE;
		}

		strAppDescription = szString;
	}
}
/************************************************************************/
/* 外部调用接口（获取一个exe文件的关联程序）                                     */
/************************************************************************/
BOOL GetFileAssociateDescription(LPCTSTR strFileExt,CString & strAssociateDesp,CString & strErrMsg)
{
	CString strCmd,strAppName,strAppPath,strPerceivedType;
	int iErrCode = 0;
	BOOL bGetFromCmdLine = FALSE;
	BOOL bUsrChoice = FALSE;
	if(!HaveOpenCmd(strFileExt,strCmd,strPerceivedType,iErrCode))
	{
		//没有找到cmd打开命令
		if (!GetAssociatedAppName(strFileExt,strAppName,strErrMsg,bUsrChoice))
		{
			// 也没有从 HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\$(strFileExt)\UserChoice\ProgId下找到打开方式
			if(!strPerceivedType.IsEmpty() && GetCmdByPerceivedType(strPerceivedType,strCmd))
			{
				//从HKEY_CLASSES_ROOT\SystemFileAssociations\*\shell\open\command获取成功
				bGetFromCmdLine = TRUE;
				goto _GET_FROM_CMD;
			}

			strAssociateDesp = "未知应用程序";
			strErrMsg = "";
			return TRUE;
		}
		
	}
	if (!bUsrChoice)
	{
		if (!GetAssociatedAppName(strFileExt,strAppName,strErrMsg,bUsrChoice))
			bGetFromCmdLine = TRUE;
	}
	
	if (bUsrChoice) 
	{
		// strAppName的值可能为ProgId,也可能是Application\*.exe格式的exe文件
		if (strAppName.Find("\\")<0)
		{
			//strAppName为ProgId
			CString strCmdTmp;
			if (GetOpenCmdFormProgId(strAppName,strCmdTmp))
				strCmd = strCmdTmp;

			bGetFromCmdLine = TRUE;
		}
		else
		{
			//strAppName中包含exe名字
			strAppName = strAppName.Mid(strAppName.Find("\\")+1);
			bGetFromCmdLine = FALSE;
		}
	}

	if(!bGetFromCmdLine)
	{
		strAppName.Replace(" ","");
		if (!GetAppPath(strAppName,strAppPath,strErrMsg)) //获取exe名字的绝对路径(注册表，系统目录下进行查找)
			bGetFromCmdLine = TRUE;
	}

_GET_FROM_CMD:
	if (bGetFromCmdLine)
		strAppPath = GetExePathFromCmd(strCmd); // 从shell\open\cmd默认值中提取exe文件路径

	if (strAppPath.Find("%")>=0)
	{
		char szBuf[1000]={0};
		strAppPath.MakeLower();
	#ifndef _WIN64
		if (strAppPath.Find("%programfiles%")>=0)
		{
			if(IsWow64())
				strAppPath.Replace("%programfiles%","%ProgramW6432%"); //32位程序，且在64位系统下运行时,Program Files路径需要用ProgramW6432匹配到，否则会匹配到Program Files(x86)目录下
		}
	#endif
		DWORD iR = ExpandEnvironmentStrings(strAppPath,szBuf,1000); // 将文件路径中的环境变量展开成绝对路径
		strAppPath = szBuf;
	}
	return GetAppDescription(strAppPath,strAssociateDesp,strErrMsg); //获取exe文件的描述信息
}