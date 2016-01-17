#include "StdAfx.h"
#include "ProcessFunc.h"

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

