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
		//����Ϊ��JOB������ʧ���Զ�ɱ��JOB�����е����н���
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

	//ͨ������ID��ȡ�����̵ľ��  
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
// ˵�����˺�������һ��JOB���󣬸ö�����ʧʱ���Զ�ɱ�����е����н���
//		����ǰ���̼������У����������ӽ���Ĭ�϶��������н��й���
//		����ֵ�����ʾ���óɹ�
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
//	//����Ϊ��JOB������ʧ���Զ�ɱ��JOB�����е����н���
//	jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE|JOB_OBJECT_LIMIT_SILENT_BREAKAWAY_OK;
//	if( 0 == SetInformationJobObject( hJob, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli)))
//	{
//		CloseHandle(hJob);
//		hJob = NULL;
//		return FALSE;
//	}
//
//	//��ȡ��ǰ���̵�ID  
//	DWORD processID = ::GetCurrentProcessId();  
//	//ͨ������ID��ȡ�����̵ľ��  
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

