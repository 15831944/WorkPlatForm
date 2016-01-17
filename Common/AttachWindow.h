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
	//反序列化
	BOOL Deserialize( LPCTSTR lpszData );
	//序列化
	CString Serialize();
	/*
	功能：
		host端，创建client进程。
	参数：
		nId：					1起始索引编号，在host端进程内，编号确保唯一。
		nExeFlag：				内嵌进程的标识，有可能host端，会内嵌多个同一进程。
		dwHostProcessId：		host端pid。
		hHostWnd：				host端主窗口，用于接收copydata消息。
		dwResponseCopyDataId：	host端定义的，处理copydata的分支消息。
		lpszClientPath：		client进程exe全路径。
		lpszExData:				扩展数据，比如："{url:"about:blank"}"
	*/
	BOOL CreateClient( LPCTSTR lpszId, int nExeFlag, ULONG_PTR dwResponseCopyDataId
		, DWORD dwHostProcessId, HWND hHostWnd
		, LPCTSTR lpszClientPath, LPCTSTR lpszExData = NULL );
	//client 应答 host 的创建操作
	BOOL ResponseHost( DWORD dwClientProcessId, HWND hClientWnd );
	//当 client 进程创建成功后，host 吸附 client 主窗口
	BOOL Attach( HWND hParentWnd, int nProcessWndAttachedMsgCode, BOOL bTheFilled = TRUE );

public:
	CString m_sId;
	int m_nExeFlag; //用于标识 Client 类别
	ULONG_PTR m_dwResponseCopyDataId;
	HWND m_hHostWnd;
	HWND m_hClientWnd;
	DWORD m_dwHostProcessId;
	DWORD m_dwClientProcessId;
	CString m_sExData; //保存一个json对象的字符串，存储进程启动时，用的附加参数
};

