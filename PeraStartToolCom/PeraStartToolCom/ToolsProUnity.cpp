#include "StdAfx.h"
#include "ToolsProUnity.h"
#include "ToolSetDlg.h"
#include "ToolManagerDlg.h"
#include "ToolGridListDlg.h"
#include "Tools.h"
#include "CommonMethod.h"
#pragma comment(lib, "version.lib")

CString CToolsProUnity::g_strConfigXml; // xml·��

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

	 REGSAM samDesired = KEY_WOW64_32KEY; // 64λ��Ĭ�ϻ�ȡ����WOW��ע���,32λ��û���
	 if (bGetKeyV64)
	 {
		 if (IsWow64())
			 samDesired = KEY_WOW64_64KEY; // 64λϵͳ�£���ָ����ȡ��WOW��ע���
		 else
			return ""; //32λϵͳ��û��Wow6432Node�ڵ�
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
		 ZTools::WriteZToolsFormatLog("����ȷ�ļ�ֵ��%s��",strKeyRoot);
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
		 { // ��Ĭ��ֵ����ȡ��exe�ļ�·��
			 strFile = strFile.Left(iPos + 4);
			 strFile.TrimLeft("\""); // ȥ����߿��ܵ�����
		 }
		 if (!pTool->m_strRunName.IsEmpty()) // ����������Ʒǿ�
		 {
			 strFile = strFile + CString("\\") + pTool->m_strRunPath + CString("\\") + pTool->m_strRunName;
			 strFile.Replace("\\\\","\\");
		 }
		 strFile.MakeLower();
		 if (strFile.Find(".exe") > 0)
		 {
			 strFile = ExpandEnvironmentStrings(strFile,TRUE);
			 if (PathFileExists(strFile))
				 return strFile; // exe ·����Ч
		 }
	 }
	 return "";
 }
CString CToolsProUnity::GetExePathFormKey(const ToolObject * pTool,CString strKeyRoot)
{
	CString str = GetExePathFormKey(pTool,strKeyRoot,FALSE); // ����Ĭ���ض�����ע���
	if (str.IsEmpty())
		return GetExePathFormKey(pTool,strKeyRoot,TRUE); // ���Ի�ȡ���ض�����ע���

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
		ZTools::WriteZToolsFormatLog("�Ӿ���·����%s����ÿ�ִ���ļ�·���ɹ���%s",pTool->m_strAbsPath,strFullPath);
		return strFullPath;
	}

	strFullPath = GetExePathFormKey(pTool,pTool->m_strDefaultKey);
	if (!strFullPath.IsEmpty())
	{
		ZTools::WriteZToolsFormatLog("��������%s����ÿ�ִ���ļ�·���ɹ���%s",pTool->m_strDefaultKey,strFullPath);
		return strFullPath;
	}

	strFullPath = GetExePathFormKey(pTool,pTool->m_strReverseKey);
	if (!strFullPath.IsEmpty())
	{
		ZTools::WriteZToolsFormatLog("�ӱ��ü���%s����ÿ�ִ���ļ�·���ɹ���%s",pTool->m_strReverseKey,strFullPath);
		return strFullPath;
	}

	ZTools::WriteZToolsFormatLog("ID=%s�����������л�ȡ��ִ���ļ�ʧ��!",pTool->m_strID);
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
	ZTools::WriteZToolsFormatLog("StartTool��ʼ����...");
	if (pTool==NULL)
		return FALSE;

	if (pTool->m_strPathConfigType.Compare("1")==0)
	{
		ZTools::WriteZToolsFormatLog("��URL:pathConfig=1,urlAddress=%s",pTool->m_strURL);
		return OpenUrlWithIE(pTool->m_strURL);
	}

	BOOL bNeedConfig = TRUE;
	CString strRunParam = pTool->m_strRunParam; // ��������Ϊ�������еĲ���
	CString strRunExe = GetFilePathFormServInfo(pTool);
	if (!strRunExe.IsEmpty())
		bNeedConfig = FALSE;

	if (bNeedConfig)
	{
		if (IsOpenByParam(pTool))
		{
			ZTools::WriteZToolsFormatLog("ע���Ĭ����������������Ϊ�գ�����·��Ϊ��,���������ǿ�=��%s��������ϵͳ��open�ӿڴ�...",pTool->m_strRunParam);
			return StartApp(pTool->m_strRunParam,"");
		}
	}

	pTool->m_strLocalPath.Empty(); //�ڴ˵������ý���ʱ������·��Ӧ���ǿյ�
	pTool->m_strLocalRunParam = pTool->m_strRunParam; // �ڴ˵������ý���ʱ�����ز��������������еĲ���

	BOOL bTryCount = 0;
	while(1)
	{
		if (bNeedConfig)
		{
			if (!ConfigTool(pTool,TRUE))
			{
				ZTools::WriteZToolsFormatLog("ID=%s,�û���������!",pTool->m_strID);
				return FALSE;
			}
			strRunExe = pTool->m_strLocalPath; //ʹ���û������õ�·��
			strRunParam = pTool->m_strLocalRunParam; //ʹ���û������õĲ���
			bTryCount++;
		}

		if (StartApp(strRunExe,strRunParam)) 
			break;
		else
		{
			bNeedConfig = TRUE;// ������ʾ��������û�����
			MessageBox(NULL,"����ʧ��,���������Ƿ���ȷ!","��ʾ",MB_OK|MB_ICONERROR); 
		}
	}

	// ���гɹ�����¼����������Ϣ
	pTool->m_strLocalPath = strRunExe;
	pTool->m_strLocalRunParam = strRunParam;
	WriteToolConfigXml(*pTool);

	return TRUE;
}
BOOL CToolsProUnity::ConfigTool(ToolObject * pTool,BOOL bCallInStartApp)
{
	ZTools::WriteZToolsFormatLog("ConfigTool ��ʼ����...");
	if (pTool==NULL)
		return FALSE;

	if (bCallInStartApp == FALSE)
	{
		ToolObject toolLocal;
		if ( GetToolFormXml(pTool->m_strID,toolLocal))
		{

			//��������ʱ��������������ù�,�����ý����ʼ��ʱ��������������
			pTool->m_strLocalPath = toolLocal.m_strLocalPath;
			pTool->m_strLocalRunParam = toolLocal.m_strLocalRunParam;
			ZTools::WriteZToolsFormatLog("ID=%s����������ҵ����ã�",pTool->m_strID);

			if (!CompareLocalWithService(*pTool,toolLocal))
			{
				WriteToolConfigXml(*pTool); //��������Ϣ�б仯,�����Ƿ񱣴棬���ȸ����������е���Ϣ
				pTool->m_strLocalPath.Empty(); 
				pTool->m_strLocalRunParam = pTool->m_strRunParam;
				ZTools::WriteZToolsFormatLog("ID=%s�����������������������!",pTool->m_strID);
			}
		}
		else
		{
			// ����δ���ù������ز��������������еĲ���,�ļ�·���Զ�ƥ��ó�
			pTool->m_strLocalPath = GetFilePathFormServInfo(pTool);  //�Զ����������в���·��������Ϊ��
			pTool->m_strLocalRunParam = pTool->m_strRunParam;
		}
	}

	CToolSetDlg dlg(pTool); // ��������ִ�г���Ի���
	if (dlg.DoModal()!=IDOK)
		return FALSE;

	pTool->m_strLocalPath		 = dlg.m_strLocalPath;    //ʹ�����õĿ�ִ�г���·��
	pTool->m_strLocalRunParam    = dlg.m_strLocalRunParam; // ʹ�ñ������õĲ���
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
	ZTools::WriteZToolsFormatLog("CheckStartApp��ʼ���ã����� ID = %s",strToolId);

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