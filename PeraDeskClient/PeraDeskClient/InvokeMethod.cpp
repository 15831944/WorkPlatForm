#include "StdAfx.h"
#include "InvokeMethod.h"

#include <tlhelp32.h>
#include "Tools.h"

#include "PeraIceNetDisk.h"

static BOOL FindProcessByName(LPCTSTR szFileName)
{
	// ���ý��̿���ö�ٽ��̵ķ�������ָ�����ƽ���
	HANDLE hProcesses;
	PROCESSENTRY32 lpe =
	{
		sizeof(PROCESSENTRY32)
	};
	CString strFileName(szFileName);
	strFileName.MakeLower();

	// �������̿���
	hProcesses = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcesses == INVALID_HANDLE_VALUE)
		return FALSE;
	// ��ȡ��һ������ʵ��
	BOOL isExist = ::Process32First(hProcesses, &lpe);
	BOOL isRunning = FALSE;
	CString strName;
	while (isExist)
	{
		strName = lpe.szExeFile;
		strName.MakeLower();
		if (-1 != strFileName.Find(strName))
		{
			isRunning = TRUE;
			break;
		}
		// ������һ������ʵ��
		isExist = ::Process32Next(hProcesses, &lpe);
	}

	CloseHandle(hProcesses);
	return isRunning;
}

//static void StartDaemon()
//{
//	if (!FindProcessByName("PeraDCNetDiskDaemon.exe"))
//	{
//		SHELLEXECUTEINFO si;
//		memset(&si, NULL, sizeof(si));
//		si.cbSize = sizeof(si);
//		si.nShow = SW_HIDE;
//		si.fMask = SEE_MASK_NOCLOSEPROCESS;
//		si.lpVerb = _T("open");
//		std::string strDaemonPath = ZTools::FormatString("%sPeraDCNetDiskDaemon.exe", ZTools::GetCurrentModulePath("PeraNetDiskCom.dll").c_str());		
//		si.lpFile = strDaemonPath.c_str();
//		si.lpParameters = NULL;
//		ShellExecuteEx(&si);
//		Sleep(1000);
//		CloseHandle(si.hProcess);
//	}
//}

BOOL InvokeMethod( string& sResult
	, LPCTSTR lpszMethodName, LPCTSTR lpszJsonStr )
{
	CPeraIceNetDisk peraIceNetDisk;

	string sMethod = lpszMethodName == NULL ? "" : lpszMethodName;
	if ( sMethod.empty() ) return FALSE;
	string sJsonStr = lpszJsonStr == NULL ? "" : lpszJsonStr;

	sResult = peraIceNetDisk.InvokeMethod( sMethod, sJsonStr );
	if (sResult.empty())
		return FALSE;

	//ZTools::WriteZToolsFormatLog("retrun:\n%s", strRet.c_str());
	return TRUE;
}

extern BOOL CanInvokeMethod( void )
{
	return FindProcessByName( "PeraDCNetDiskDaemon.exe" );
}
