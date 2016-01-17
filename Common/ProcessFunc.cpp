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
		sMsg.Format("δ�ҵ� %s", sExePath);
		::MessageBox( NULL, sMsg, sAppTital, MB_OK | MB_TOPMOST );
		return 0;
	}

	STARTUPINFO StartInfo;
	PROCESS_INFORMATION pinfo;
	memset(&StartInfo,0,sizeof(STARTUPINFO));
	StartInfo.cb = sizeof(STARTUPINFO);				//�趨�ṹ�Ĵ�С
	StartInfo.dwFlags   =   STARTF_USESHOWWINDOW;	//| STARTF_USESTDHANDLES;   
	StartInfo.wShowWindow = wShowFlag;
	if( !CreateProcess(
		(LPCSTR)sExePath, //��������·����
		(LPSTR)sParam.GetBuffer(),//��������exeNameΪNULLʱ���ɽ�����������ǰ��
		NULL,  //ʹ��Ĭ�Ͻ��̰�ȫ����
		NULL,  //ʹ��Ĭ���̰߳�ȫ����
		FALSE, //������̳�
		//CREATE_NEW_CONSOLE,
		NORMAL_PRIORITY_CLASS|CREATE_BREAKAWAY_FROM_JOB,// | CREATE_NO_WINDOW,  //ʹ���������ȼ�
		NULL,  //ʹ�ø����̵Ļ�������
		NULL,  //ָ������Ŀ¼
		&StartInfo, //�ӽ��������������ʾ
		&pinfo ) ) //���ڴ���½��̵ķ�����Ϣ
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
		ZTools::WriteZToolsFormatLog("���������쳣,ErrorCode -- %s", lpMsgBuf);
		CString s;
		s.Format("%s", lpMsgBuf);
		::MessageBox( NULL, s, sAppTital, MB_OK | MB_TOPMOST );
		LocalFree( lpMsgBuf );

		return 0;
	}

	AddProcessToJobKillOnClose(pinfo.hProcess);

	CloseHandle(pinfo.hThread); 
	CloseHandle(pinfo.hProcess); 

	ZTools::WriteZToolsLog(ZTools::FormatString("��������%d", pinfo.dwProcessId));

	return(pinfo.dwProcessId);
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

