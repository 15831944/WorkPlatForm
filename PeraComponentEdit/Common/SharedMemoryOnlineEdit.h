#pragma once
#include "csharedmemory.h"

class SharedMemoryOnlineEdit
{
public:
	SharedMemoryOnlineEdit(void);
	~SharedMemoryOnlineEdit(void);

private:
	CSharedMemory* m_pSmOnlineEdit;
	CString m_strSharedName;
	HANDLE m_hJob;
	void InitJob();

public:
	BOOL InitSmOnlineEdit();
	BOOL ClearSmOnlineEdit();
	BOOL GetInfo(DWORD* dwPid, HWND* hWnd);
	BOOL SetInfo(DWORD dwPid, HWND hWnd);
	CString GetFlexwareBinPath();
	DWORD StartOpenTools();
	static HWND GetMainWindow(DWORD dwPID, CString strCaption, CString strClass);
	static BOOL CALLBACK EnumWindowCallback_GetMainWindow(HWND hWnd, LPARAM lParam);
	CString FormatSharedName();
};
