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
/* ����PerceivedType��ȡ��������                                           */
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
/* ��shell����������ȡexe·���ַ���                                   */
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

	if (strAppPath.Left(1)!="\"") //·��û��������������
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
/* HKEY_CLASSES_ROOT\$(strProID)������shell����                           */
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
/* ��HKEY_CLASSES_ROOT\$(strFileExt)�²���shell����                  */
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
/* ��ȡexe�����ĳ��򣬷���true����strAppName�ǿգ�����false�����ñ�ķ�ʽ��ȡ��������                                                                     */
/************************************************************************/
BOOL GetAssociatedAppName(LPCTSTR strFileExt,CString &strAppName,CString &strErrMsg,BOOL &bUsrChoice)
{
	strErrMsg  = "";
	strAppName = "";
	bUsrChoice = FALSE;

	HKEY   hFirstKey,keyFinal;
	if(ERROR_SUCCESS!=RegOpenKey(HKEY_CURRENT_USER,NULL,&hFirstKey))
	{
		strErrMsg = "RegOpenKey ��HKEY_CURRENT_USER��ʧ�ܣ�";
		return FALSE;
	}

	CString strPath = CString("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\") + CString(strFileExt) + "\\UserChoice";
	DWORD dwType;
	DWORD dwSize = 1000;
	char  szString[1000] = {0};
	if(ERROR_SUCCESS==RegOpenKey(hFirstKey,strPath,&keyFinal)) // ����ʹ���û�ѡ��
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
	return FALSE; // ����FALSE,ʹ��shell\open\command����������ȡ��exe·��
}
/************************************************************************/
/* ���ڴ�HKEY_CURRENT_USER\\Software\\Classes\\Applications��ȡһ��exe��·��  */
/************************************************************************/
BOOL GetApplicationPathFromReg(LPCTSTR strAppName,CString &strAppPath,CString &strErrMsg)
{
	strErrMsg  = "";
	strAppPath = "";

	HKEY   hFirstKey,keyFinal;
	if(ERROR_SUCCESS!=RegOpenKey(HKEY_CURRENT_USER,NULL,&hFirstKey))
	{
		strErrMsg = "RegOpenKey ��HKEY_CURRENT_USER��ʧ�ܣ�";
		return FALSE;
	}

	CString strPath = CString("Software\\Classes\\Applications\\") + CString(strAppName) + "\\shell\\open\\command";
	if(ERROR_SUCCESS!=RegOpenKey(hFirstKey,strPath,&keyFinal))  
	{
		RegCloseKey(hFirstKey);
		strErrMsg.Format("RegOpenKey��%s��ʧ�ܣ�",strPath);
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
		strErrMsg.Format("RegQueryValueEx��%s��Ĭ��ֵʧ�ܣ�",strPath);
		return FALSE;
	}

	CString strCmd = szString;
	strAppPath = GetExePathFromCmd(strCmd);
	return TRUE;
}
/************************************************************************/
/* ��ȡexe�ļ���·������ע����ϵͳĿ¼���̲��ң�              */
/************************************************************************/
BOOL GetAppPath(LPCTSTR strAppName,CString &strAppPath,CString &strErrMsg)
{
	strErrMsg  = "";
	strAppPath = "";

	HKEY   hFirstKey,keyFinal;
	if(ERROR_SUCCESS!=RegOpenKey(HKEY_LOCAL_MACHINE,NULL,&hFirstKey))
	{
		strErrMsg = "RegOpenKey ��HKEY_LOCAL_MACHINE��ʧ�ܣ�";
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

		strErrMsg.Format("RegOpenKey��%s��ʧ�ܣ�",strPath);
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
		strErrMsg.Format("RegQueryValueEx��%s��Ĭ��ֵʧ�ܣ�",strPath);
		return FALSE;
	}

	strAppPath = szString;
	return TRUE;
}
/************************************************************************/
/* ���ļ��汾��Ϣ�л�ȡ�ļ�������Ϣ                                   */
/************************************************************************/
BOOL GetAppDescription(LPCTSTR strAppPath,CString &strAppDescription,CString &strErrMsg)
{
	strErrMsg  = "";
	strAppDescription = "";
	CString strBlockName;
	//�Ҳ����汾��Ϣʱ���ļ���������Ϊ�ļ���exe����
	strAppDescription = strAppPath;
	strAppDescription = strAppDescription.Mid(strAppDescription.ReverseFind('\\')+1);

	DWORD dwSize = GetFileVersionInfoSize(strAppPath,NULL);
	if (dwSize==0)
	{
		//�Ҳ����汾��Ϣʱ���ļ���������Ϊ�ļ���exe����
		if (!IsAccessibleFile(strAppPath))
		{
			strAppDescription = "δ֪Ӧ�ó���";
			return TRUE;
		}
		return TRUE;
		/*strErrMsg.Format("GetFileVersionInfoSize��%s��,���ش�СΪ0",strAppPath);
		return FALSE;*/
	}

	char *pVersionData = new char[dwSize];
	if (!GetFileVersionInfo(strAppPath,0,dwSize,pVersionData))
	{
		delete pVersionData;
		strErrMsg.Format("GetFileVersionInfoSize��%s��,��ȡ��%d����С�İ汾��Ϣʧ��",strAppPath,dwSize);
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
			strErrMsg = "RegOpenKey ��HKEY_CURRENT_USER��ʧ�ܣ�";
			return FALSE;
		}

		CString strPath = "Software\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\Shell\\MuiCache";
		if(ERROR_SUCCESS!=RegOpenKey(hFirstKey,strPath,&keyFinal))  
		{
			RegCloseKey(hFirstKey);
			strErrMsg.Format("RegOpenKey��%s��ʧ�ܣ�",strPath);
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
			strErrMsg.Format("RegQueryValueEx��%s��ʧ�ܣ�","MRUList");
			return FALSE;
		}

		strAppDescription = szString;
	}
}
/************************************************************************/
/* �ⲿ���ýӿڣ���ȡһ��exe�ļ��Ĺ�������                                     */
/************************************************************************/
BOOL GetFileAssociateDescription(LPCTSTR strFileExt,CString & strAssociateDesp,CString & strErrMsg)
{
	CString strCmd,strAppName,strAppPath,strPerceivedType;
	int iErrCode = 0;
	BOOL bGetFromCmdLine = FALSE;
	BOOL bUsrChoice = FALSE;
	if(!HaveOpenCmd(strFileExt,strCmd,strPerceivedType,iErrCode))
	{
		//û���ҵ�cmd������
		if (!GetAssociatedAppName(strFileExt,strAppName,strErrMsg,bUsrChoice))
		{
			// Ҳû�д� HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\$(strFileExt)\UserChoice\ProgId���ҵ��򿪷�ʽ
			if(!strPerceivedType.IsEmpty() && GetCmdByPerceivedType(strPerceivedType,strCmd))
			{
				//��HKEY_CLASSES_ROOT\SystemFileAssociations\*\shell\open\command��ȡ�ɹ�
				bGetFromCmdLine = TRUE;
				goto _GET_FROM_CMD;
			}

			strAssociateDesp = "δ֪Ӧ�ó���";
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
		// strAppName��ֵ����ΪProgId,Ҳ������Application\*.exe��ʽ��exe�ļ�
		if (strAppName.Find("\\")<0)
		{
			//strAppNameΪProgId
			CString strCmdTmp;
			if (GetOpenCmdFormProgId(strAppName,strCmdTmp))
				strCmd = strCmdTmp;

			bGetFromCmdLine = TRUE;
		}
		else
		{
			//strAppName�а���exe����
			strAppName = strAppName.Mid(strAppName.Find("\\")+1);
			bGetFromCmdLine = FALSE;
		}
	}

	if(!bGetFromCmdLine)
	{
		strAppName.Replace(" ","");
		if (!GetAppPath(strAppName,strAppPath,strErrMsg)) //��ȡexe���ֵľ���·��(ע���ϵͳĿ¼�½��в���)
			bGetFromCmdLine = TRUE;
	}

_GET_FROM_CMD:
	if (bGetFromCmdLine)
		strAppPath = GetExePathFromCmd(strCmd); // ��shell\open\cmdĬ��ֵ����ȡexe�ļ�·��

	if (strAppPath.Find("%")>=0)
	{
		char szBuf[1000]={0};
		strAppPath.MakeLower();
	#ifndef _WIN64
		if (strAppPath.Find("%programfiles%")>=0)
		{
			if(IsWow64())
				strAppPath.Replace("%programfiles%","%ProgramW6432%"); //32λ��������64λϵͳ������ʱ,Program Files·����Ҫ��ProgramW6432ƥ�䵽�������ƥ�䵽Program Files(x86)Ŀ¼��
		}
	#endif
		DWORD iR = ExpandEnvironmentStrings(strAppPath,szBuf,1000); // ���ļ�·���еĻ�������չ���ɾ���·��
		strAppPath = szBuf;
	}
	return GetAppDescription(strAppPath,strAssociateDesp,strErrMsg); //��ȡexe�ļ���������Ϣ
}