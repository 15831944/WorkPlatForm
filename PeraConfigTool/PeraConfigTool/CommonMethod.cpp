#include "StdAfx.h"
#include "CommonMethod.h"
#include<tlhelp32.h>
int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lParam,LPARAM lpData)
{
	switch(uMsg)
	{
	case BFFM_INITIALIZED:
		::SendMessage(hwnd,BFFM_SETSELECTION,TRUE,lpData);
		HWND hFind = ::GetWindow(hwnd,GW_CHILD);
		while(hFind)
		{
			char szName[256] ={0};
			GetClassName(hFind,szName,256);
			if (_stricmp(szName,"Edit")==0)
			{
				CEdit *pEdit = (CEdit*)CWnd::FromHandle(hFind);
				//pEdit->EnableWindow(FALSE);
				pEdit->SetReadOnly(TRUE); //将控件设置为只读，方便复制，但不允许修改
				break;
			}
			hFind = ::GetWindow(hFind,GW_HWNDNEXT);
		}

		break;
	}
	return 0;
}
CString GetSelectedFolder()
{
	BROWSEINFO bi;  
	ZeroMemory(&bi,sizeof(BROWSEINFO)); 
	bi.hwndOwner = GetActiveWindow();   
	bi.lpszTitle = "请选择目录：";  

	static CString strLastDir = "";
	bi.ulFlags = BIF_USENEWUI | BIF_RETURNONLYFSDIRS;
	bi.lpfn = BrowseCallbackProc;
	bi.lParam = (LPARAM)(LPCTSTR)strLastDir;


	LPMALLOC pMalloc;  

	if(::GetCapture())  
		::ReleaseCapture(); 
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);  

	if(!pidl )
		return "";

	CString strFolderOut = "";
	TCHAR * path = new TCHAR[MAX_PATH];   

	SHGetPathFromIDList(pidl,path);  
	if(SUCCEEDED(SHGetMalloc(&pMalloc))) 
	{  
		pMalloc->Free(pidl);  
		pMalloc->Release();  
	}

	strFolderOut = path;
	delete [] path;
	if (strFolderOut.IsEmpty())
	{
		::MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd,"非法路径","错误",MB_OK|MB_ICONERROR);
		return "";
	}
	else
	{
		strLastDir = strFolderOut; // 记住上次打开的目录
	}

	return strFolderOut;
}
DWORD FindProcess(const char * pszName)
{
	if( NULL == pszName )
	{
		return -1;
	}

	HANDLE           hSnapshot;   

	//创建结构体用来存放进程快照信息
	PROCESSENTRY32   processinfo;   
	processinfo.dwSize = sizeof(processinfo);   

	//进程快照获取进程列表
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);  

	if( NULL == hSnapshot )  
	{
		return   0;   
	}  

	//从Snapshot（上面获得的进程快照）得到第一个进程记录信息
	BOOL bStatus = Process32First( hSnapshot, &processinfo );

	int nLen     = strlen( processinfo.szExeFile ) + 1;  





	while( bStatus )   
	{
		//找到返回进程ID
		if( _stricmp(pszName,processinfo.szExeFile) == 0 ) 
		{
			CloseHandle(hSnapshot);
			return processinfo.th32ProcessID;   	
		}

		//从Snapshot（上面获得的进程快照）得到下一个进程记录信息
		bStatus = Process32Next(hSnapshot,   &processinfo);   
	}   

	//关闭进程快照句柄
	CloseHandle(hSnapshot);
	return   -1;   
}
BOOL KillProcess(DWORD dwProcessId)
{
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE,FALSE,dwProcessId);

	if( NULL == hProcess )
		return FALSE;

	if( !TerminateProcess(hProcess, 0) )
		return FALSE;

	return TRUE;
}