#include "StdAfx.h"
#include "RegFunc.h"

BOOL WriteRegForDeskClient( LPCTSTR lpszKey, LPCTSTR lpszValue )
{
	CString sKey = lpszKey == NULL ? "" : lpszKey;
	CString sValue = lpszValue == NULL ? "" : lpszValue;

	if ( sKey.IsEmpty() ) return FALSE;

	CString sRangesDir;
	sRangesDir.Format("Software\\Peraglobal\\PeraDeskClient");
	CRegKey keyRanges;
	keyRanges.Create(HKEY_CURRENT_USER, sRangesDir);
	keyRanges.SetStringValue( lpszKey, lpszValue );
	return TRUE;
}

CString GetRegForDeskClient( LPCTSTR lpszKey )
{
	CString sRegResult;

	HKEY hKeyParent = HKEY_CURRENT_USER;
	CString sRegKeyPath = "Software\\Peraglobal\\PeraDeskClient";
	CString sKey = lpszKey == NULL ? "" : lpszKey;
	if ( sKey.IsEmpty() ) return "";

	CRegKey rk;
	DWORD pCount = 1024;
	std::string strKeyValue;
	char szValue[1024] = {NULL};
	//memset(szValue, 0, 1024);

	if(rk.Open(hKeyParent, sRegKeyPath)== ERROR_SUCCESS)
	{
		if(rk.QueryStringValue(sKey, szValue, &pCount)== ERROR_SUCCESS)
		{
			sRegResult = szValue;
		}
	}
	rk.Close();

	return sRegResult;
}