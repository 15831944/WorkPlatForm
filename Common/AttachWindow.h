#pragma once
#include "StlHeader.h"
#include <atlstr.h>

extern UINT WM_AW_PROCESS_ATTACHING;
extern UINT WM_AW_PROCESS_ATTACHED;
extern UINT WM_AW_PROCESS_DETACHING;
extern UINT WM_AW_PROCESS_DETACHED;

class CAttachWindow
{
public:
	CAttachWindow();
	~CAttachWindow(void);
	//
	void Clear();
	//
	BOOL CopyFrom( CAttachWindow* pSrc );
	//
	//�����л�
	BOOL Deserialize( LPCTSTR lpszData );
	//���л�
	CString Serialize();
	/*
	���ܣ�
		host�ˣ�����client���̡�
	������
		nId��					1��ʼ������ţ���host�˽����ڣ����ȷ��Ψһ��
		nExeFlag��				��Ƕ���̵ı�ʶ���п���host�ˣ�����Ƕ���ͬһ���̡�
		dwHostProcessId��		host��pid��
		hHostWnd��				host�������ڣ����ڽ���copydata��Ϣ��
		dwResponseCopyDataId��	host�˶���ģ�����copydata�ķ�֧��Ϣ��
		lpszClientPath��		client����exeȫ·����
		lpszExData:				��չ���ݣ����磺"{url:"about:blank"}"
	*/
	BOOL CreateClient( LPCTSTR lpszId, int nExeFlag, ULONG_PTR dwResponseCopyDataId
		, DWORD dwHostProcessId, HWND hHostWnd
		, LPCTSTR lpszClientPath, LPCTSTR lpszExData = NULL );
	//client Ӧ�� host �Ĵ�������
	BOOL ResponseHost( DWORD dwClientProcessId, HWND hClientWnd );
	//�� client ���̴����ɹ���host ���� client ������
	BOOL Attach( HWND hParentWnd, int nProcessWndAttachedMsgCode, BOOL bTheFilled = TRUE );

public:
	CString m_sId;
	int m_nExeFlag; //���ڱ�ʶ Client ���
	ULONG_PTR m_dwResponseCopyDataId;
	HWND m_hHostWnd;
	HWND m_hClientWnd;
	DWORD m_dwHostProcessId;
	DWORD m_dwClientProcessId;
	CString m_sExData; //����һ��json������ַ������洢��������ʱ���õĸ��Ӳ���
};

