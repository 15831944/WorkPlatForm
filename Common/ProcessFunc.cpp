#include "StdAfx.h"
#include "ProcessFunc.h"
#include "Tools.h"
//#include "UserDefine.h"

static HANDLE s_hJob = NULL;

HANDLE GetJobKillOnClose( void )
{
	if (s_hJob != NULL && s_hJob != INVALID_HANDLE_VALUE)
	{
		return s_hJob;
	}

	s_hJob = NULL;
	s_hJob = CreateJobObject( NULL, NULL); // GLOBAL

	if (s_hJob != NULL)
	{
		JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = { 0 };

		// Configure all child processes associated with the job to terminate when the
		//设置为当JOB对象消失后，自动杀掉JOB对象中的所有进程
		jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE|JOB_OBJECT_LIMIT_SILENT_BREAKAWAY_OK;
		if( 0 == SetInformationJobObject( s_hJob, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli)))
		{
			CloseHandle(s_hJob);
			s_hJob = NULL;
		}
	}

	return s_hJob;
}

void ClearJobHandle( void )
{
	if (s_hJob != NULL && s_hJob != INVALID_HANDLE_VALUE)
	{
		CloseHandle(s_hJob);
	}
	s_hJob = NULL;
}

BOOL AddProcessToJobKillOnClose( DWORD dwPid )
{
	if (dwPid == 0)
	{
		return FALSE;
	}

	if (s_hJob == NULL || s_hJob == INVALID_HANDLE_VALUE)
	{
		GetJobKillOnClose();
	}

	if (s_hJob == NULL || s_hJob == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}	

	//通过进程ID获取到进程的句柄  
	HANDLE hProcess = OpenProcess( 0x0fff,  
		FALSE, dwPid );
	if (hProcess == NULL || hProcess == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	if(!AssignProcessToJobObject(s_hJob, hProcess))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL AddProcessToJobKillOnClose( HANDLE hProcess )
{
	if (hProcess == NULL || hProcess == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	if (s_hJob == NULL || s_hJob == INVALID_HANDLE_VALUE)
	{
		GetJobKillOnClose();
	}

	if (s_hJob == NULL || s_hJob == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}	
	
	if(!AssignProcessToJobObject(s_hJob, hProcess))
	{
		return FALSE;
	}

	return TRUE;
}

DWORD CreateProcessInJob(LPCTSTR lpExePath, LPCTSTR lpParam, WORD wShowFlag, LPCTSTR lpAppTital)
{
	CString sAppTital = lpAppTital == NULL ? "" : lpAppTital ;
	CString sExePath(lpExePath);
	CString sParam(lpParam);
	if (sExePath.IsEmpty())
		return 0;
	if (!PathFileExists(sExePath))
	{
		CString sMsg;
		sMsg.Format("未找到 %s", sExePath);
		::MessageBox( NULL, sMsg, sAppTital, MB_OK | MB_TOPMOST );
		return 0;
	}

	STARTUPINFO StartInfo;
	PROCESS_INFORMATION pinfo;
	memset(&StartInfo,0,sizeof(STARTUPINFO));
	StartInfo.cb = sizeof(STARTUPINFO);				//设定结构的大小
	StartInfo.dwFlags   =   STARTF_USESHOWWINDOW;	//| STARTF_USESTDHANDLES;   
	StartInfo.wShowWindow = wShowFlag;
	if( !CreateProcess(
		(LPCSTR)sExePath, //启动程序路径名
		(LPSTR)sParam.GetBuffer(),//参数（当exeName为NULL时，可将命令放入参数前）
		NULL,  //使用默认进程安全属性
		NULL,  //使用默认线程安全属性
		FALSE, //句柄不继承
		//CREATE_NEW_CONSOLE,
		NORMAL_PRIORITY_CLASS|CREATE_BREAKAWAY_FROM_JOB,// | CREATE_NO_WINDOW,  //使用正常优先级
		NULL,  //使用父进程的环境变量
		NULL,  //指定工作目录
		&StartInfo, //子进程主窗口如何显示
		&pinfo ) ) //用于存放新进程的返回信息
	{
		DWORD errCode = GetLastError();
		LPVOID lpMsgBuf;
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			errCode,
			0, // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL 
			);
		ZTools::WriteZToolsFormatLog("创建进程异常,ErrorCode -- %s", lpMsgBuf);
		CString s;
		s.Format("%s", lpMsgBuf);
		::MessageBox( NULL, s, sAppTital, MB_OK | MB_TOPMOST );
		LocalFree( lpMsgBuf );

		return 0;
	}

	AddProcessToJobKillOnClose(pinfo.hProcess);

	CloseHandle(pinfo.hThread); 
	CloseHandle(pinfo.hProcess); 

	ZTools::WriteZToolsLog(ZTools::FormatString("启动进程%d", pinfo.dwProcessId));

	return(pinfo.dwProcessId);
}

//************************************
// Method:    CreateJobKillOnClose
// FullName:  CreateJobKillOnClose
// Access:    public 
// Returns:   HANDLE
// Qualifier:
// Parameter: void
// 说明：此函数创建一个JOB对象，该对象消失时会自动杀掉其中的所有进程
//		将当前进程加入其中，这样所有子进程默认都会在其中进行管理
//		返回值非零表示设置成功
//************************************
//HANDLE CreateJobKillOnClose( void )
//{
//	HANDLE hJob = NULL;
//	hJob = CreateJobObject( NULL, NULL); // GLOBAL
//
//	if (hJob == NULL)
//	{
//		return FALSE;
//	}
//
//	JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = { 0 };
//
//	// Configure all child processes associated with the job to terminate when the
//	//设置为当JOB对象消失后，自动杀掉JOB对象中的所有进程
//	jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE|JOB_OBJECT_LIMIT_SILENT_BREAKAWAY_OK;
//	if( 0 == SetInformationJobObject( hJob, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli)))
//	{
//		CloseHandle(hJob);
//		hJob = NULL;
//		return FALSE;
//	}
//
//	//获取当前进程的ID  
//	DWORD processID = ::GetCurrentProcessId();  
//	//通过进程ID获取到进程的句柄  
//	HANDLE hProcess = OpenProcess( 0x0fff,  
//		FALSE, processID );
//	if (hProcess == NULL)
//	{
//		CloseHandle(hJob);
//		hJob = NULL;
//		return FALSE;
//	}
//	
//	BOOL b = AssignProcessToJobObject(hJob, hProcess);
//	if(!b)
//	{
//		CloseHandle(hJob);
//		hJob = NULL;
//		return FALSE;
//	}
//
//	return hJob;
//}

