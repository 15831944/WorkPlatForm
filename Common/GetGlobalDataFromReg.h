#pragma once

/*
���ܣ�
	��ע����ж�ȡPeraglobal��ȫ�����ݡ�
������
	lpszFlag:
		"LM" = HKEY_LOCAL_MACHINE
		"CU" = HKEY_CURRENT_USER
	lpszKey:
		��ֵ���磺"Depends"
����ֵ��
	�ַ����͵ı�����
*/
extern CString GetPeraGlobalDataFromReg( LPCTSTR lpszFlag, LPCTSTR lpszKey );

/*
���ܣ�
	��ע����ж�ȡPeraProcessRunnerInstallFlag���������жϵ�����ִ�л����Ƿ�װ��
������
����ֵ��
*/
extern BOOL IsPeraProcessRunnerInstalled();

extern BOOL OnRegisterUrlProtocol( LPCTSTR lpszProtocolName );

extern BOOL OnUnregisterUrlProtocol( LPCTSTR lpszModuleName );