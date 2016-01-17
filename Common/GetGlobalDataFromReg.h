#pragma once

/*
功能：
	从注册表中读取Peraglobal的全局数据。
参数：
	lpszFlag:
		"LM" = HKEY_LOCAL_MACHINE
		"CU" = HKEY_CURRENT_USER
	lpszKey:
		键值，如："Depends"
返回值：
	字符串型的变量。
*/
extern CString GetPeraGlobalDataFromReg( LPCTSTR lpszFlag, LPCTSTR lpszKey );

/*
功能：
	从注册表中读取PeraProcessRunnerInstallFlag的数据来判断单机版执行环境是否安装。
参数：
返回值：
*/
extern BOOL IsPeraProcessRunnerInstalled();

extern BOOL OnRegisterUrlProtocol( LPCTSTR lpszProtocolName );

extern BOOL OnUnregisterUrlProtocol( LPCTSTR lpszModuleName );