#include "stdafx.h"
#include "CommonMethod.h"
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
CString ExpandEnvironmentStrings(CString strSrc,BOOL bOutLongPath)
{
	CString strFullPath = strSrc;
#ifndef _WIN64 // 32λ������������64λϵͳ��
	strFullPath.MakeLower();
	if (strFullPath.Find("%programfiles%")>=0)
	{
		if(IsWow64())
			strFullPath.Replace("%programfiles%","%ProgramW6432%"); //32λ��������64λϵͳ������ʱ,Program Files·����Ҫ��ProgramW6432ƥ�䵽�������ƥ�䵽Program Files(x86)Ŀ¼��
	}
#endif

	char szBuf[2*MAX_PATH] = {0};
	if (strFullPath.Find("%")>=0)
	{
		ExpandEnvironmentStrings(strFullPath,szBuf,2*MAX_PATH);
		strFullPath = szBuf;
	}

	if (bOutLongPath)
	{
		GetLongPathName(strFullPath,szBuf,2*MAX_PATH);
		strFullPath = szBuf;
	}

	return strFullPath;
}