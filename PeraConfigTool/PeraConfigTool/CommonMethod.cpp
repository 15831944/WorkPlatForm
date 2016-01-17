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
				pEdit->SetReadOnly(TRUE); //���ؼ�����Ϊֻ�������㸴�ƣ����������޸�
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
	bi.lpszTitle = "��ѡ��Ŀ¼��";  

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
		::MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd,"�Ƿ�·��","����",MB_OK|MB_ICONERROR);
		return "";
	}
	else
	{
		strLastDir = strFolderOut; // ��ס�ϴδ򿪵�Ŀ¼
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

	//�����ṹ��������Ž��̿�����Ϣ
	PROCESSENTRY32   processinfo;   
	processinfo.dwSize = sizeof(processinfo);   

	//���̿��ջ�ȡ�����б�
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);  

	if( NULL == hSnapshot )  
	{
		return   0;   
	}  

	//��Snapshot�������õĽ��̿��գ��õ���һ�����̼�¼��Ϣ
	BOOL bStatus = Process32First( hSnapshot, &processinfo );

	int nLen     = strlen( processinfo.szExeFile ) + 1;  





	while( bStatus )   
	{
		//�ҵ����ؽ���ID
		if( _stricmp(pszName,processinfo.szExeFile) == 0 ) 
		{
			CloseHandle(hSnapshot);
			return processinfo.th32ProcessID;   	
		}

		//��Snapshot�������õĽ��̿��գ��õ���һ�����̼�¼��Ϣ
		bStatus = Process32Next(hSnapshot,   &processinfo);   
	}   

	//�رս��̿��վ��
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