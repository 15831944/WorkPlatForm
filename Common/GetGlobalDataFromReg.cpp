#include "stdafx.h"
#include <string>
#include <atlbase.h>
using namespace ATL;

CString GetLM_WorkspacePath()
{
	CString sRegResult;

	HKEY hKeyParent = HKEY_LOCAL_MACHINE;
	CString sRegKeyPath = "Software\\Peraglobal";
	CString sRegKey = "PeraProcessDesigner";

	CRegKey rk;
	DWORD pCount = 1024;
	std::string strKeyValue;
	char szValue[1024];
	memset(szValue, 0, 1024);

	if(rk.Open(hKeyParent, sRegKeyPath, KEY_READ)== ERROR_SUCCESS)
	{
		if(rk.QueryStringValue(sRegKey, szValue, &pCount)== ERROR_SUCCESS)
		{
			sRegResult = szValue;
		}
	}
	rk.Close();

	if (!sRegResult.IsEmpty())
	{
		return sRegResult;
	}
	return sRegResult;
}

/*

*/
CString GetPeraGlobalDataFromReg( LPCTSTR lpszFlag, LPCTSTR lpszKey )
{
	CString sRegResult;
	CString sFlag = lpszFlag == NULL ? "" : lpszFlag;
	CString sRegKey = lpszKey == NULL ? "" : lpszKey;

	HKEY hKeyParent = NULL;
	if ( sFlag.CompareNoCase("lm") == 0 ) hKeyParent = HKEY_LOCAL_MACHINE;
	else if ( sFlag.CompareNoCase("cu") == 0 ) hKeyParent = HKEY_CURRENT_USER;
	else
		return "";

	CString sRegKeyPath = "Software\\Peraglobal";

	CRegKey rk;
	DWORD pCount = 1024;
	std::string strKeyValue;
	char szValue[1024];
	memset(szValue, 0, 1024);

	if(rk.Open(hKeyParent, sRegKeyPath, KEY_READ)== ERROR_SUCCESS)
	{
		if(rk.QueryStringValue(sRegKey, szValue, &pCount)== ERROR_SUCCESS)
		{
			sRegResult = szValue;
		}
	}
	rk.Close();

	if (!sRegResult.IsEmpty())
	{
		return sRegResult;
	}
	return sRegResult;
}

extern BOOL IsPeraProcessRunnerInstalled()
{
	CString sFlag = GetPeraGlobalDataFromReg("lm", "PeraProcessRunnerInstallFlag");
	if (sFlag.CompareNoCase("true") == 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


BOOL OnRegisterUrlProtocol( LPCTSTR lpszProtocolName )
{
	//写注册表
	//需要写入注册表的权限
	//暂时只写入 HKEY_CLASSES_ROOT，不写HKEY_CURRENT_USER\Software\Classes

	CString sProtocolName = lpszProtocolName == NULL ? "" : lpszProtocolName;
	if ( sProtocolName.IsEmpty() ) return FALSE;

	CString sRegKey = "URL Protocol";
	CString sRegValue; sRegValue.Format("URL:%s Protocol", sProtocolName);
	CString sRegPath = sProtocolName;

	CRegKey keyUrlProtocol;
	if (keyUrlProtocol.Create(HKEY_CLASSES_ROOT, sRegPath) != ERROR_SUCCESS)
	{
		return FALSE;
	}
	keyUrlProtocol.SetStringValue(sRegKey, sRegValue);

	sRegPath.Format(_T("%s\\shell\\open\\command"), sProtocolName);
	CRegKey keyOpenCommand;
	if (keyOpenCommand.Create(HKEY_CLASSES_ROOT, sRegPath) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	TCHAR exeFullPath[MAX_PATH] = {NULL};
	::GetModuleFileName(NULL, exeFullPath, MAX_PATH);
	CString sOpenCommand;
	sOpenCommand.Format(_T("\"%s\" %%1"), exeFullPath);
	keyOpenCommand.SetStringValue(_T(""), sOpenCommand);

	keyOpenCommand.Close();
	keyUrlProtocol.Close();
	return TRUE;
}

BOOL OnUnregisterUrlProtocol( LPCTSTR lpszModuleName )
{
	//清注册表
	//需要写入注册表的权限
	//暂时只写入 HKEY_CLASSES_ROOT，不写HKEY_CURRENT_USER\Software\Classes

	CString sModuleName = lpszModuleName == NULL ? "" : lpszModuleName;
	if ( sModuleName.IsEmpty() ) return FALSE;

	CString sRegPath = sModuleName;

	CRegKey keyUrlProtocol;
	if (keyUrlProtocol.Create(HKEY_CLASSES_ROOT, sRegPath) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	keyUrlProtocol.RecurseDeleteKey(_T(""));
	keyUrlProtocol.Close();
	return TRUE;
}