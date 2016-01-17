#include "StdAfx.h"
#include "ToolsProUnity.h"
#include "ToolSetDlg.h"
#include "ToolManagerDlg.h"
#include "ToolGridListDlg.h"
#include "Tools.h"
#include "CommonMethod.h"
#pragma comment(lib, "version.lib")

CString CToolsProUnity::g_strConfigXml; // xml路径

CToolsProUnity::CToolsProUnity(void)
{
}
CToolsProUnity::~CToolsProUnity(void)
{
}
 CString CToolsProUnity::GetExePathFormKey(const ToolObject * pTool,CString strKeyRoot,BOOL bGetKeyV64)
 {
	 if (pTool==NULL) return "";
	 if (strKeyRoot.IsEmpty()) return "";

	 REGSAM samDesired = KEY_WOW64_32KEY; // 64位下默认获取的是WOW的注册表,32位下没差别
	 if (bGetKeyV64)
	 {
		 if (IsWow64())
			 samDesired = KEY_WOW64_64KEY; // 64位系统下，且指定获取非WOW的注册表
		 else
			return ""; //32位系统下没有Wow6432Node节点
	 }

	 HKEY   keyFinal,keyRoot(NULL);
	 if (strKeyRoot=="HKEY_CLASSES_ROOT")
		 keyRoot = HKEY_CLASSES_ROOT;
	 else if (strKeyRoot=="HKEY_CURRENT_USER")
		 keyRoot = HKEY_CURRENT_USER;
	 else if (strKeyRoot=="HKEY_LOCAL_MACHINE")
		 keyRoot = HKEY_LOCAL_MACHINE;
	 else
	 {
		 ZTools::WriteZToolsFormatLog("不正确的键值【%s】",strKeyRoot);
		 return "";
	 }

	 CString strKeyPath = pTool->m_strKeyName; 
	 if(ERROR_SUCCESS!=RegOpenKeyEx(keyRoot,strKeyPath,0,KEY_READ|samDesired,&keyFinal))
	 {
		 return "";
	 }

	 DWORD dwType;
	 DWORD dwSize = 1000;
	 char  szString[1000] = {0};
	 LONG lReturn = ::RegQueryValueEx (keyFinal,pTool->m_strKeyValueName, NULL,&dwType, (BYTE *) szString, &dwSize);
	 RegCloseKey(keyFinal);
	 if (lReturn==ERROR_SUCCESS)
	 {
		 CString strFile = szString;
		 strFile.MakeLower();
		 int iPos = strFile.Find(".exe");
		 if (iPos > 0) 
		 { // 从默认值中提取出exe文件路径
			 strFile = strFile.Left(iPos + 4);
			 strFile.TrimLeft("\""); // 去掉左边可能的引号
		 }
		 if (!pTool->m_strRunName.IsEmpty()) // 如果启动名称非空
		 {
			 strFile = strFile + CString("\\") + pTool->m_strRunPath + CString("\\") + pTool->m_strRunName;
			 strFile.Replace("\\\\","\\");
		 }
		 strFile.MakeLower();
		 if (strFile.Find(".exe") > 0)
		 {
			 strFile = ExpandEnvironmentStrings(strFile,TRUE);
			 if (PathFileExists(strFile))
				 return strFile; // exe 路径有效
		 }
	 }
	 return "";
 }
CString CToolsProUnity::GetExePathFormKey(const ToolObject * pTool,CString strKeyRoot)
{
	CString str = GetExePathFormKey(pTool,strKeyRoot,FALSE); // 尝试默认重定向后的注册表
	if (str.IsEmpty())
		return GetExePathFormKey(pTool,strKeyRoot,TRUE); // 尝试获取非重定向后的注册表

	return str;
}
BOOL CToolsProUnity::StartApp(LPCTSTR strApp,LPCTSTR strParam)
{
	if (strApp==NULL) return FALSE;
	CString str = strApp;
	if (str.IsEmpty()) return FALSE;

	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS|SEE_MASK_FLAG_NO_UI;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = "Open";
	ShExecInfo.lpFile = strApp;        
	ShExecInfo.lpParameters = strParam;  
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL;	
	return ShellExecuteEx(&ShExecInfo);
}
CString CToolsProUnity::GetFilePathFormServInfo(const ToolObject *pTool)
{
	CString strFullPath = ExpandEnvironmentStrings(pTool->m_strAbsPath,TRUE);
	if (PathFileExists(strFullPath)) 
	{
		ZTools::WriteZToolsFormatLog("从绝对路径【%s】获得可执行文件路径成功：%s",pTool->m_strAbsPath,strFullPath);
		return strFullPath;
	}

	strFullPath = GetExePathFormKey(pTool,pTool->m_strDefaultKey);
	if (!strFullPath.IsEmpty())
	{
		ZTools::WriteZToolsFormatLog("从主键【%s】获得可执行文件路径成功：%s",pTool->m_strDefaultKey,strFullPath);
		return strFullPath;
	}

	strFullPath = GetExePathFormKey(pTool,pTool->m_strReverseKey);
	if (!strFullPath.IsEmpty())
	{
		ZTools::WriteZToolsFormatLog("从备用键【%s】获得可执行文件路径成功：%s",pTool->m_strReverseKey,strFullPath);
		return strFullPath;
	}

	ZTools::WriteZToolsFormatLog("ID=%s的软件从清册中获取可执行文件失败!",pTool->m_strID);
	return "";
}
BOOL CToolsProUnity::IsOpenByParam(const ToolObject *pTool)
{
	if (pTool->m_strDefaultKey.IsEmpty()
	&& pTool->m_strReverseKey.IsEmpty()
	&& pTool->m_strAbsPath.IsEmpty()
	&& (!pTool->m_strRunParam.IsEmpty()))
		return TRUE;

	return FALSE;
}
BOOL CToolsProUnity::StartTool(ToolObject *pTool)
{
	ZTools::WriteZToolsFormatLog("StartTool开始调用...");
	if (pTool==NULL)
		return FALSE;

	if (pTool->m_strPathConfigType.Compare("1")==0)
	{
		ZTools::WriteZToolsFormatLog("打开URL:pathConfig=1,urlAddress=%s",pTool->m_strURL);
		return OpenUrlWithIE(pTool->m_strURL);
	}

	BOOL bNeedConfig = TRUE;
	CString strRunParam = pTool->m_strRunParam; // 启动参数为软件清册中的参数
	CString strRunExe = GetFilePathFormServInfo(pTool);
	if (!strRunExe.IsEmpty())
		bNeedConfig = FALSE;

	if (bNeedConfig)
	{
		if (IsOpenByParam(pTool))
		{
			ZTools::WriteZToolsFormatLog("注册表默认主键、备用主键为空，绝对路径为空,启动参数非空=【%s】，调用系统的open接口打开...",pTool->m_strRunParam);
			return StartApp(pTool->m_strRunParam,"");
		}
	}

	pTool->m_strLocalPath.Empty(); //在此弹出配置界面时，本地路径应该是空的
	pTool->m_strLocalRunParam = pTool->m_strRunParam; // 在此弹出配置界面时，本地参数适用软件清册中的参数

	BOOL bTryCount = 0;
	while(1)
	{
		if (bNeedConfig)
		{
			if (!ConfigTool(pTool,TRUE))
			{
				ZTools::WriteZToolsFormatLog("ID=%s,用户放弃配置!",pTool->m_strID);
				return FALSE;
			}
			strRunExe = pTool->m_strLocalPath; //使用用户新配置的路径
			strRunParam = pTool->m_strLocalRunParam; //使用用户新配置的参数
			bTryCount++;
		}

		if (StartApp(strRunExe,strRunParam)) 
			break;
		else
		{
			bNeedConfig = TRUE;// 弹出提示后继续让用户配置
			MessageBox(NULL,"启动失败,请检查配置是否正确!","提示",MB_OK|MB_ICONERROR); 
		}
	}

	// 运行成功，记录本地配置信息
	pTool->m_strLocalPath = strRunExe;
	pTool->m_strLocalRunParam = strRunParam;
	WriteToolConfigXml(*pTool);

	return TRUE;
}
BOOL CToolsProUnity::ConfigTool(ToolObject * pTool,BOOL bCallInStartApp)
{
	ZTools::WriteZToolsFormatLog("ConfigTool 开始调用...");
	if (pTool==NULL)
		return FALSE;

	if (bCallInStartApp == FALSE)
	{
		ToolObject toolLocal;
		if ( GetToolFormXml(pTool->m_strID,toolLocal))
		{

			//启动配置时，如果本地有配置过,则配置界面初始化时，加载已配置项
			pTool->m_strLocalPath = toolLocal.m_strLocalPath;
			pTool->m_strLocalRunParam = toolLocal.m_strLocalRunParam;
			ZTools::WriteZToolsFormatLog("ID=%s的软件本地找到配置！",pTool->m_strID);

			if (!CompareLocalWithService(*pTool,toolLocal))
			{
				WriteToolConfigXml(*pTool); //软件清册信息有变化,不论是否保存，都先更新软件清册中的信息
				pTool->m_strLocalPath.Empty(); 
				pTool->m_strLocalRunParam = pTool->m_strRunParam;
				ZTools::WriteZToolsFormatLog("ID=%s的软件已随服务器软件清册更新!",pTool->m_strID);
			}
		}
		else
		{
			// 本地未配置过，本地参数采用软件清册中的参数,文件路径自动匹配得出
			pTool->m_strLocalPath = GetFilePathFormServInfo(pTool);  //自动从软件清册中查找路径，可能为空
			pTool->m_strLocalRunParam = pTool->m_strRunParam;
		}
	}

	CToolSetDlg dlg(pTool); // 启动配置执行程序对话框
	if (dlg.DoModal()!=IDOK)
		return FALSE;

	pTool->m_strLocalPath		 = dlg.m_strLocalPath;    //使用配置的可执行程序路径
	pTool->m_strLocalRunParam    = dlg.m_strLocalRunParam; // 使用本地配置的参数
	WriteToolConfigXml(*pTool);
	return TRUE;
}
BOOL CToolsProUnity::StartManagerUI()
{
	map<CString,ToolObject> toolMap;
	LoadAllLocalTool(toolMap);
	//CToolManagerDlg dlg(toolMap);
	CToolGridListDlg dlg(toolMap);
	if (dlg.DoModal()!=IDOK)
		return FALSE;

	return TRUE;
}
BOOL CToolsProUnity::CompareLocalWithService(const ToolObject & toolService, const ToolObject & toolLocal)
{
	if (toolService.m_strDesrciption.Compare(toolLocal.m_strDesrciption)!=0)
		return FALSE;
	if (toolService.m_strName.Compare(toolLocal.m_strName)!=0)
		return FALSE;
	if (toolService.m_strAbsPath.Compare(toolLocal.m_strAbsPath)!=0)
		return FALSE;
	if (toolService.m_strRunParam.Compare(toolLocal.m_strRunParam)!=0)
		return FALSE;
	if (toolService.m_strDefaultKey.Compare(toolLocal.m_strDefaultKey)!=0)
		return FALSE;
	if (toolService.m_strReverseKey.Compare(toolLocal.m_strReverseKey)!=0)
		return FALSE;
	if (toolService.m_strKeyName.Compare(toolLocal.m_strKeyName)!=0)
		return FALSE;
	if (toolService.m_strKeyValueName.Compare(toolLocal.m_strKeyValueName)!=0)
		return FALSE;
	if (toolService.m_strRunPath.Compare(toolLocal.m_strRunPath)!=0)
		return FALSE;
	if (toolService.m_strRunName.Compare(toolLocal.m_strRunName)!=0)
		return FALSE;

	return TRUE;
}
CString CToolsProUnity::GetToolVersion(CString strToolPath)
{
	strToolPath = ExpandEnvironmentStrings(strToolPath,TRUE);
	if (strToolPath=="" || !PathFileExists(strToolPath))
		return "";

	DWORD dwSize = GetFileVersionInfoSize(strToolPath,NULL);
	if (dwSize==0) return "";

	char *pVersionData = new char[dwSize];
	if (!GetFileVersionInfo(strToolPath,0,dwSize,pVersionData))
	{
		delete pVersionData;
		return "";
	}

	UINT nQuerySize;
	DWORD* pTransTable;
	CString strVersion;
	CString strBlockName;
	if (!::VerQueryValue(pVersionData, _T("\\VarFileInfo\\Translation"),(void **)&pTransTable, &nQuerySize) )
		goto _TEST_GET_;

	DWORD dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));

	LPVOID lpData;
	strBlockName.Format(_T("\\StringFileInfo\\%08lx\\%s"),dwLangCharset, _T("ProductVersion"));

	if ( ::VerQueryValue((void **)pVersionData, strBlockName.GetBuffer(0),&lpData, &nQuerySize))
	{  
		strVersion = (LPCTSTR)lpData;
		strBlockName.ReleaseBuffer();
		delete pVersionData;
		return strVersion;
	}
_TEST_GET_:
	static const char * strLanguageList[] = {"040904e4","040904b0"}; 
	int iTestCount = sizeof(strLanguageList)/sizeof(const char*);
	int iTest = 0;
	while(1)
	{
		strBlockName.Format(_T("\\StringFileInfo\\%s\\%s"),strLanguageList[iTest],_T("ProductVersion"));
		if ( ::VerQueryValue((void **)pVersionData, strBlockName.GetBuffer(0),&lpData, &nQuerySize))
		{
			strVersion = (LPCTSTR)lpData;
			break;
		}
		iTest++;
		if (iTest >= iTestCount)
			break;
	}

	strBlockName.ReleaseBuffer();
	delete pVersionData;
	return strVersion;
}
BOOL CToolsProUnity::CheckStartApp(const CString & strToolId)
{
	ZTools::WriteZToolsFormatLog("CheckStartApp开始调用：输入 ID = %s",strToolId);

	ToolObject toolTocal;
	if (!GetToolFormXml(strToolId,toolTocal))
		return FALSE;
	
	if (!PathFileExists(toolTocal.m_strLocalPath))
		return FALSE;

	return StartApp(toolTocal.m_strLocalPath,toolTocal.m_strLocalRunParam);
}
 BOOL CToolsProUnity::OpenUrlWithIE(const CString &strUrl)
 {
	 TCHAR szDir[MAX_PATH] = {NULL};
	 if ( !SHGetSpecialFolderPath(NULL
		 , szDir
		 , CSIDL_PROGRAM_FILES
		 , FALSE ) )
	 {
		 return FALSE;
	 }

	 CString strDir  = szDir;
	 strDir += "\\Internet Explorer";
	 strDir += "\\iexplore.exe";
	 if ( !PathFileExists( strDir ) )
		 return FALSE;

	int iRev = (int)ShellExecute(NULL, "open", strDir, strUrl, NULL, SW_MAXIMIZE);
	if (iRev > 32)
	 return TRUE;

	return FALSE;
 }