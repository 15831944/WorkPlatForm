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

	////ˢ�������ͼ�껺��
	//HWND hWnd   =   ::GetDesktopWindow();  
	//hWnd   =   ::FindWindowEx(hWnd,   0,   "Progman",   0);  
	//hWnd   =   ::FindWindowEx(hWnd,   0,   "SHELLDLL_DefView",   0);  
	//hWnd   =   ::FindWindowEx(hWnd,   0,   "SysListView32",   0);   //��ʱhWnd��������Ҫ���Ǹ�
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
/* �ڴ��ַ������鳣������ӹ������ͼ���ʵ�ֹ���                     */
/************************************************************************/
static const char *g_TypeFilter[] = {
				".contact", //��ϵ��
				".library-ms",//��
				"Briefcase",//���İ�
				"Folder",  //�ļ���
				".lnk",   //��ݷ�ʽ
				".bfc"    //xp�µĹ��İ�
			};

static bool IsUsefullType(const char *pszType)
{
	static int iSize = sizeof(g_TypeFilter)/sizeof(const char*);
	int i = 0;
	while(i<iSize)
	{
		if(_stricmp(g_TypeFilter[i],pszType)==0)
			return false; //�����˵�
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
			return true; //����office����
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
	ZTools::WriteZToolsFormatLog("GetXpTypeItems����...");
	HKEY   hFirstKey,keyFinal;
	LSTATUS lResult = 0;
	string strPath = "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Discardable\\PostSetup\\ShellNew";
	int iTryCount = 0;
_Retry:
	iTryCount ++;
	lResult = RegOpenKey(HKEY_CURRENT_USER,NULL,&hFirstKey);
	if(ERROR_SUCCESS!=lResult)
	{
		ZTools::WriteZToolsFormatLog("GetXpTypeItems RegOpenKey�򿪼�ֵ��HKEY_CURRENT_USER��ʧ��,RegOpenKey���ء�%d��,GetXpTypeItems����",lResult);
		string strErr = ZTools::GetLastErrorMessageString(lResult).c_str();
		ZTools::WriteZToolsFormatLog("�����������:%s",strErr.c_str());
		return;
	}
	lResult = RegOpenKey(hFirstKey,strPath.c_str(),&keyFinal);
	if(ERROR_SUCCESS!=lResult) 
	{
		ZTools::WriteZToolsFormatLog("GetXpTypeItems RegOpenKey�򿪼�ֵ��HKEY_CURRENT_USER\\%s��ʧ��,RegOpenKey���ء�%d��,GetXpTypeItems����",strPath.c_str(),lResult);
		string strErr = ZTools::GetLastErrorMessageString(lResult).c_str();
		ZTools::WriteZToolsFormatLog("�����������:%s",strErr.c_str());
		RegCloseKey(hFirstKey);
		if(lResult!=2 || iTryCount>2)
			return;

		RefreshShellNewReg();
		Sleep(500);
		ZTools::WriteZToolsFormatLog("���еڡ�%d��������",iTryCount);
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
			ZTools::WriteZToolsFormatLog("GetXpTypeItems ���˵�~reserved~���߷�REG_BINARY����,Item=%d",Item);
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
					ZTools::WriteZToolsFormatLog("���˵���Office���͡�%s��,Item=%d",nt->m_strExt.c_str(),Item);
					memset(szString,0,sizeof(1000));
					continue; // ֻ����office����ʱ�����˵�����office���ļ�����
				}
			}
			vAllItems.push_back(nt);
			ZTools::WriteZToolsFormatLog("����������չ����%s��,����������%s��,Item=%d",nt->m_strExt.c_str(),nt->m_strDesrciption.c_str(),Item);
		}
		else
			ZTools::WriteZToolsFormatLog("IsUsefullType���˵���%s��,Item=%d",nt->m_strExt.c_str(),Item);

		memset(szString,0,sizeof(1000));
	}

	RegCloseKey(keyFinal);
	RegCloseKey(hFirstKey);
}
static bool GetShellNewKeyPath(HKEY keyExt,string & strKeyPath)
{
	// �ȴӺ�׺����Ĭ��ֵ����ShellNew
	HKEY keyFinal;
	DWORD dwType;
	DWORD dwSize = 1000;
	char  szString[1000] = {0};
	LONG lReturn = ::RegQueryValueEx (keyExt,"", NULL,&dwType, (BYTE *) szString, &dwSize);
	if (lReturn==ERROR_SUCCESS)
	{
		string strTmp =  string(szString) + string("\\ShellNew");
		if(ERROR_SUCCESS==RegOpenKey(keyExt,strTmp.c_str(),&keyFinal)) // ��Ĭ��ֵ����ShellNew,����.xlsx\Excel.Sheet.12\ShellNew
		{
			strKeyPath  = strKeyPath + string("\\") + strTmp;
			ZTools::WriteZToolsFormatLog("�Ӻ�׺����Ĭ��ֵ����ShellNew�ɹ�!");
			RegCloseKey(keyFinal);
			return true;
		}
	}

	// ��׺����Ĭ��ֵ����ShellNewʧ��,��ֱ�ӴӺ�׺������ShellNew������.xlsx\ShellNew
	if(ERROR_SUCCESS==RegOpenKey(keyExt,"ShellNew",&keyFinal)) 
	{
		strKeyPath +="\\ShellNew";
		RegCloseKey(keyFinal);
		ZTools::WriteZToolsFormatLog("ֱ�ӴӺ�׺������ShellNew�ɹ�!");
		return true;
	}

	return false;
}
static bool IsRightNewItem(HKEY keyShellNew,string & strKeyPath)
{
	//����ShellNew�Ӽ�
	DWORD   Item=0,dwLen=MAX_PATH; 
	char   SubKeyStr[MAX_PATH];
	string strTmpPath = strKeyPath;
	while(ERROR_SUCCESS==RegEnumKeyEx(keyShellNew,Item++,SubKeyStr,&dwLen,NULL,NULL,NULL,NULL))   
	{
		strTmpPath = strKeyPath + string("\\") + SubKeyStr;

		dwLen = MAX_PATH; //ÿ�ε���RegEnumKeyExǰ�������½�KeySize��ֵ��ΪKeyMaxLen�������Ĵ�С���������ʧ��
		if (_stricmp(SubKeyStr,"ShellNew")==0) //�ҵ�SHellNew�Ӽ�
		{
			int a = 0;
			strKeyPath = strTmpPath;
			return true;
		}
		else //δ�ҵ�SHellNew�Ӽ���������ü��µ��Ӽ�
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
	string strNewFileName = strDir + "\\�½��ļ���";
	int iTryCount = 1;
	while (PathFileExists(strNewFileName.c_str()))
	{
		iTryCount++;
		CString strNewTypeComment;
		strNewTypeComment.Format("�½��ļ���(%d)",iTryCount);
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
	ZTools::WriteZToolsFormatLog("GetAllDesktopRightMenuNewItems����");
	if(GetWindowVesion()=="xp")
		return GetXpTypeItems(vAllItems,bOnlyOfficeType);

	ZTools::WriteZToolsFormatLog("��Xp����ϵͳ");
	HKEY   hFirstKey,keyFinal;
	LSTATUS lResult = 0;
	string strPath = "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Discardable\\PostSetup\\ShellNew";
	int iTryCount = 0;
_Retry:
	iTryCount ++;
	lResult = RegOpenKey(HKEY_CURRENT_USER,NULL,&hFirstKey);
	if(ERROR_SUCCESS!=lResult)
	{
		ZTools::WriteZToolsFormatLog("RegOpenKey�򿪼�ֵ��HKEY_CURRENT_USER��ʧ��,RegOpenKey���ء�%d��",lResult);
		string strErr = ZTools::GetLastErrorMessageString(lResult).c_str();
		ZTools::WriteZToolsFormatLog("�����������:%s",strErr.c_str());
		return;
	}
	lResult = RegOpenKey(hFirstKey,strPath.c_str(),&keyFinal);
	if(ERROR_SUCCESS!=lResult) 
	{
		ZTools::WriteZToolsFormatLog("RegOpenKey�򿪼�ֵ��HKEY_CURRENT_USER\\%s��ʧ��,RegOpenKey���ء�%d��",strPath.c_str(),lResult);
		string strErr = ZTools::GetLastErrorMessageString(lResult).c_str();
		ZTools::WriteZToolsFormatLog("�����������:%s",strErr.c_str());
		RegCloseKey(hFirstKey);
		if(lResult!=2 || iTryCount>2)
			return;

		RefreshShellNewReg();
		Sleep(500);
		ZTools::WriteZToolsFormatLog("���еڡ�%d��������",iTryCount);
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
					continue; // ֻ����office����ʱ�����˵�����office���ļ�����
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
		m_strErrMsg = "�½��ļ���Ϊ��!";
		return false;
	}
	/*if (!GetTypeName(strItemType,strTypeComment))
		strTypeComment = "�½���";*/

	HKEY   hFirstKey,keyExt,keyFinal;
	if(ERROR_SUCCESS!=RegOpenKey(HKEY_CLASSES_ROOT,NULL,&hFirstKey))
	{
		m_strErrMsg = "��ע����HKEY_CLASSES_ROOTʧ��";
		return false;
	}

	if(ERROR_SUCCESS!=RegOpenKey(hFirstKey,strItemType.c_str(),&keyExt))
	{
		m_strErrMsg.Format("��ע����HKEY_CLASSES_ROOT\\%sʧ��",strItemType.c_str());
		RegCloseKey(hFirstKey);
		return false;
	}

	string strShellNewPath = strItemType;
	if (!GetShellNewKeyPath(keyExt,strShellNewPath)) 
	{
		m_strErrMsg = "��ע�����δ�ҵ������͵��ļ���������ShellNew";
		RegCloseKey(keyExt);
		RegCloseKey(hFirstKey);
		return false;
	}

	/*if (!IsRightNewItem(keyExt,strShellNewPath)) 
	{
		m_strErrMsg = "��ע�����δ�ҵ������͵��ļ���������ShellNew";
		return false;
	}*/

	if(ERROR_SUCCESS!=RegOpenKey(hFirstKey,strShellNewPath.c_str(),&keyFinal)) 
	{
		m_strErrMsg.Format("��ע����HKEY_CLASSES_ROOT\\%sʧ��",strShellNewPath);
		RegCloseKey(keyExt);
		RegCloseKey(hFirstKey);
		return false;
	}

	DWORD dwType;
	DWORD dwSize = 1000;
	char  szString[1000] = {0};

	LONG lReturn = ::RegQueryValueEx (keyFinal,"FileName", NULL,
		&dwType, (BYTE *) szString, &dwSize);

	ZTools::WriteZToolsFormatLog("RegQueryValueEx��ȡģ���ļ�����%s��",szString);

	int iTryCount = 1;
	string strSplit = "\\";
	int iLen = strDir.length();
	if (iLen >0 && strDir[iLen-1] == '\\')
		strSplit = "";

	string strNewFileName = strDir  + strSplit + strTypeComment + strItemType;
	wstring strW = ZTools::s2ws(strNewFileName);
	if (strW.length() >MAX_PATH)
	{
		m_strErrMsg = "�ļ�·��������";
		m_iErrCode = 1;
		return false;
	}

	if (PathFileExists(strNewFileName.c_str()))
	{
		// ҵ����������ļ��Ѿ����ڣ���ֱ�Ӹ���
		//m_strErrMsg.Format("�ļ���%s���Ѿ����ڣ����ܴ���ͬ���ļ�!",strNewFileName.c_str());
		//return false;
		ZTools::WriteZToolsFormatLog("��%s���ļ��Ѿ����ڣ�removeɾ�����ļ�...",strNewFileName.c_str());
		remove(strNewFileName.c_str());
	}
	//while(access(strNewFileName.c_str(),0)==0) //����ļ����ڣ��������ֺ�Ӻ�׺
	//{
	//	iTryCount++;
	//	CString strNewTypeComment;
	//	strNewTypeComment.Format("%s(%d)",strTypeComment.c_str(),iTryCount);
	//	strNewFileName =  strDir  + string("\\") + string((LPCTSTR)strNewTypeComment) + strItemType;
	//}

	if(ERROR_SUCCESS == lReturn) // ��ģ����ļ�����
	{
		string strSource = szString;
		if(strSource.find(':')== string::npos) // �����̷���ģ���ļ�����
		{
			char szWinDir[MAX_PATH] = {0};
			GetWindowsDirectory(szWinDir,MAX_PATH); // �����̷���ģ���ļ����Ƶ�Ĭ��ģ��Ŀ¼��ȥѰ��
			strSource = string(szWinDir) + string("\\ShellNew\\") + szString;
			ZTools::WriteZToolsFormatLog("ʹ��Ĭ�ϵ�ģ���ļ�·����%s��",strSource.c_str());
		}
		
		if (PathFileExists(strSource.c_str())) //ģ���ļ�����
		{
			RegCloseKey(keyExt);
			RegCloseKey(hFirstKey);
			RegCloseKey(keyFinal);

			ZTools::WriteZToolsFormatLog("ģ���ļ���%s������",strSource.c_str());
			ZTools::WriteZToolsFormatLog("MakeSureDirectoryPathExists���á�%s��",strNewFileName.c_str());
			if (!MakeSureDirectoryPathExists(strNewFileName.c_str()))
			{
				m_strErrMsg = ZTools::GetLastErrorMessageString().c_str();
				return false;
			}

			ZTools::WriteZToolsFormatLog("����ģ���ļ���CopyFileԴ=��%s����Ŀ��=��%s��",strSource.c_str(),strNewFileName.c_str());
			if(::CopyFile(strSource.c_str(),strNewFileName.c_str(),TRUE))
			{
				strNewPath = strNewFileName;
				return true;
			}
			ZTools::WriteZToolsFormatLog("CopyFileʧ�ܣ�������������");
			m_strErrMsg = ZTools::GetLastErrorMessageString().c_str();
			return false;
		}
		else
		{
			ZTools::WriteZToolsFormatLog("ģ���ļ���%s���޷��ҵ����������հ��ļ������ļ������޷������򿪣�",strSource.c_str());
		}
	}

	lReturn = ::RegQueryValueEx (keyFinal,"NullFile", NULL,
		&dwType, (BYTE *) szString, &dwSize);

	RegCloseKey(keyExt);
	RegCloseKey(keyFinal);
	RegCloseKey(hFirstKey);
	

	ZTools::WriteZToolsFormatLog("RegQueryValueEx��ȡNullFile������ֵΪ��%ld��",lReturn);

	if (_stricmp(strItemType.c_str(),"Folder")==0) //�����ļ���
	{
		ZTools::WriteZToolsFormatLog("CreateNewFolder��%s��,��%s��",strDir.c_str(),strNewPath.c_str());
		return CreateNewFolder(strDir,strNewPath);
	}
	else
	{
		if (lReturn!= ERROR_SUCCESS)
		{
			ZTools::WriteZToolsFormatLog("ϵͳ��δ�ҵ�[%s]�ļ��Ĵ�������,�����򽫴����յ��ļ������ļ����ܲ�����ȷ��!"
				,strItemType.c_str());
		}

		ZTools::WriteZToolsFormatLog("MakeSureDirectoryPathExists���á�%s��,׼�������հ��ļ�",strNewFileName.c_str());
		if (!MakeSureDirectoryPathExists(strNewFileName.c_str()))
		{
			m_strErrMsg = ZTools::GetLastErrorMessageString().c_str();
			return false;
		}

		ZTools::WriteZToolsFormatLog("fopen_s���á�%s��,ģʽ��w��",strNewFileName.c_str());
		FILE *pFile = NULL;
		fopen_s(&pFile,strNewFileName.c_str(),"w");
		if (pFile==NULL)
		{
			m_strErrMsg = ZTools::GetLastErrorMessageString().c_str();
			ZTools::WriteZToolsFormatLog("fopen_s��w��ģʽʧ�ܣ�");
			fopen_s(&pFile,strNewFileName.c_str(),"wb+");
			if (pFile==NULL)
			{
				ZTools::WriteZToolsFormatLog("fopen_s��wb+��ģʽʧ�ܣ�");
				return false;
			}
			else

				ZTools::WriteZToolsFormatLog("fopen_s��wb+��ģʽ�ɹ���");
		}

		int iRev = fclose(pFile);
		ZTools::WriteZToolsFormatLog("fclose���ء�%d��",iRev);
		strNewPath = strNewFileName;
	}

	return true;
}