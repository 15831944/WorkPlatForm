#include "StdAfx.h"
#include "SharedMemoryOnlineEdit.h"
#include "Tools.h"
#include "GetFlexwareBin.h"
#include "ProcessFunc.h"
#include "PeraProcessDesignerUtility.h"
#include "PeraProcessDesigner.h"

struct OPENTOOLS_INFO_STRUCT 
{
	DWORD dwPid;
	HWND hWnd;
};

SharedMemoryOnlineEdit::SharedMemoryOnlineEdit(void)
{
	m_pSmOnlineEdit = NULL;
	m_hJob = NULL;
	InitJob();
	m_strSharedName = FormatSharedName();
	InitSmOnlineEdit();
}

SharedMemoryOnlineEdit::~SharedMemoryOnlineEdit(void)
{
	ClearSmOnlineEdit();
}

//���ҽ��������ڵĻص������Ĳ���
typedef struct structGetMainWindowParam
{
	structGetMainWindowParam()
		: dwProcessId(0)
		, hwnd(NULL)
	{}
	HWND hwnd;
	DWORD dwProcessId;
	TCHAR tcCaption[1024];
	TCHAR tcClass[1024];
} GETMAINWINDOW_PARAM;

//���ҽ��������ڵĻص�����
BOOL CALLBACK SharedMemoryOnlineEdit::EnumWindowCallback_GetMainWindow(HWND hWnd, LPARAM lParam)
{
	GETMAINWINDOW_PARAM *pData = (GETMAINWINDOW_PARAM *)lParam;
	if (pData == NULL) return TRUE;

	DWORD dwProcessId = 0;
	GetWindowThreadProcessId(hWnd, &dwProcessId); //ȡ�õ�ǰ���ڶ�Ӧ�Ľ���id
	if (dwProcessId != pData->dwProcessId) return TRUE;

	if (!IsWindow(hWnd)
		|| ::GetParent(hWnd) != NULL) return TRUE;

	{
		CString strFindCaption(pData->tcCaption);
		if (!strFindCaption.IsEmpty())
		{
			CString strWindowCaption("");
			LPTSTR lpsz = strWindowCaption.GetBufferSetLength(256 + 1);
			::GetWindowText(hWnd, lpsz, 256);

			if (strWindowCaption.Find(strFindCaption) == -1)
				return TRUE;
		}
	}

	{
		CString strFindClass(pData->tcClass);
		strFindClass = strFindClass.MakeLower();
		CString strWindowClass("");
		LPTSTR lpsz = strWindowClass.GetBufferSetLength(256 + 1);
		::GetClassName(hWnd, lpsz, 256);
		strWindowClass = strWindowClass.MakeLower();

		if (strWindowClass.Find(strFindClass) == -1)
			return TRUE;
	}

	pData->hwnd = hWnd;
	return FALSE;
}

//���ݽ���ID�������ڱ��⣨ģ��ƥ�䣩ȡ�ý��������ھ��
HWND SharedMemoryOnlineEdit::GetMainWindow(DWORD dwPID, CString strCaption, CString strClass)
{
	GETMAINWINDOW_PARAM param;
	param.dwProcessId = dwPID;
	strcpy_s(param.tcCaption, 1024, (char *)(LPCTSTR)strCaption);
	strcpy_s(param.tcClass, 1024, (char *)(LPCTSTR)strClass);
	EnumWindows(EnumWindowCallback_GetMainWindow, (LPARAM)&param);
	return param.hwnd;
}

CString SharedMemoryOnlineEdit::FormatSharedName()
{
	CString strSharedName;
	DWORD dwPid = 0;

	//��ǰ����ID
	dwPid = GetCurrentProcessId();
	strSharedName.Format("%s-%d", "PeraComponentEditorInfo", dwPid);
	return strSharedName;
}

BOOL SharedMemoryOnlineEdit::InitSmOnlineEdit()
{
	if (m_pSmOnlineEdit)
	{
		return TRUE;
	}

	m_pSmOnlineEdit = new CSharedMemory(m_strSharedName, sizeof(OPENTOOLS_INFO_STRUCT));

	//����Ѵ��ڣ���ʾ�����������̴�������ά������ڴ�
	if (m_pSmOnlineEdit->AlreadyExist())
	{
		delete m_pSmOnlineEdit;
		m_pSmOnlineEdit = NULL;
		return TRUE;
	}
	else//�����PeraProcessDesigner.exe��ִ�����²���
	{
		DWORD dwPID = StartOpenTools();
		//���¹����ڴ�
		SetInfo(dwPID, NULL);
	}

	return TRUE;
}

BOOL SharedMemoryOnlineEdit::ClearSmOnlineEdit()
{
	if (m_pSmOnlineEdit == NULL)
	{
		return TRUE;
	}

	//�ؽ��̲���չ����ڴ����ֵ
	OPENTOOLS_INFO_STRUCT info;
	memset(&info, 0, sizeof(OPENTOOLS_INFO_STRUCT));

	m_pSmOnlineEdit->Lock();
	memcpy(&info, m_pSmOnlineEdit->GetData(), sizeof(OPENTOOLS_INFO_STRUCT));
	m_pSmOnlineEdit->Unlock();

	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, info.dwPid);
	if (hProcess != NULL && hProcess != INVALID_HANDLE_VALUE)
	{
		ZTools::WriteZToolsLog(ZTools::FormatString("��OnlineEdit���ر�PeraComponentEditor����%d", info.dwPid));

		TerminateProcess(hProcess, 0);
		CloseHandle(hProcess);
		hProcess = NULL;
	}
	else
	{
		ZTools::WriteZToolsLog(ZTools::FormatString("��OnlineEdit������PeraComponentEditor����%dʧ��", info.dwPid));
	}

	m_pSmOnlineEdit->Lock();
	memset(m_pSmOnlineEdit->GetData(), 0, sizeof(OPENTOOLS_INFO_STRUCT));
	m_pSmOnlineEdit->Unlock();

	delete m_pSmOnlineEdit;
	m_pSmOnlineEdit = NULL;
	return TRUE;
}

BOOL SharedMemoryOnlineEdit::GetInfo( DWORD* dwPid, HWND* hWnd )
{
	OPENTOOLS_INFO_STRUCT info;
	memset(&info, 0, sizeof(OPENTOOLS_INFO_STRUCT));

	if (m_pSmOnlineEdit)
	{
		m_pSmOnlineEdit->Lock();
		memcpy(&info, m_pSmOnlineEdit->GetData(), sizeof(OPENTOOLS_INFO_STRUCT));
		m_pSmOnlineEdit->Unlock();
	}
	else
	{
		CSharedMemory* pSmOnlineEdit = NULL;
		pSmOnlineEdit = new CSharedMemory(m_strSharedName, sizeof(OPENTOOLS_INFO_STRUCT));
		pSmOnlineEdit->Lock();
		memcpy(&info, pSmOnlineEdit->GetData(), sizeof(OPENTOOLS_INFO_STRUCT));
		pSmOnlineEdit->Unlock();
		delete pSmOnlineEdit;
		pSmOnlineEdit = NULL;
	}
	*dwPid = info.dwPid;
	*hWnd = info.hWnd;

	return TRUE;
}

BOOL SharedMemoryOnlineEdit::SetInfo( DWORD dwPid, HWND hWnd )
{
	OPENTOOLS_INFO_STRUCT info;
	memset(&info, 0, sizeof(OPENTOOLS_INFO_STRUCT));
	info.dwPid = dwPid;
	info.hWnd = hWnd;

	if (m_pSmOnlineEdit)
	{
		m_pSmOnlineEdit->Lock();
		memcpy(m_pSmOnlineEdit->GetData(), &info, sizeof(OPENTOOLS_INFO_STRUCT));
		m_pSmOnlineEdit->Unlock();
	}
	else
	{
		CSharedMemory* pSmOnlineEdit = NULL;
		pSmOnlineEdit = new CSharedMemory(m_strSharedName, sizeof(OPENTOOLS_INFO_STRUCT));
		pSmOnlineEdit->Lock();
		memcpy(pSmOnlineEdit->GetData(), &info, sizeof(OPENTOOLS_INFO_STRUCT));
		pSmOnlineEdit->Unlock();
		delete pSmOnlineEdit;
		pSmOnlineEdit = NULL;
	}

	return TRUE;
}

DWORD SharedMemoryOnlineEdit::StartOpenTools()
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char exeFullPath[MAX_PATH]; // MAX_PATH
	GetModuleFileName(NULL, exeFullPath, MAX_PATH);//�õ�����ģ�����ƣ�ȫ·��
	_splitpath_s( exeFullPath, drive, dir, fname, ext );

	CString sPeraComponentEditorPath = GetPeraComponentEditorPath();
	if (sPeraComponentEditorPath.IsEmpty())
		return 0;
	if (!PathFileExists(sPeraComponentEditorPath))
	{
		CString sMsg;
		sMsg.Format("δ�ҵ� %s", sPeraComponentEditorPath);
		::MessageBox(GetMainWnd(), sMsg, g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		return 0;
	}
	CString sParam;
	sParam.Format("\"%s\" -type webedit -visibility hidden", sPeraComponentEditorPath);

	STARTUPINFO StartInfo;
	PROCESS_INFORMATION pinfo;
	memset(&StartInfo,0,sizeof(STARTUPINFO));
	StartInfo.cb = sizeof(STARTUPINFO);				//�趨�ṹ�Ĵ�С
	StartInfo.dwFlags   =   STARTF_USESHOWWINDOW;	//| STARTF_USESTDHANDLES;   
	StartInfo.wShowWindow = SW_HIDE;
	if( !CreateProcess(
		NULL, //��������·����
		(char*)(sParam.GetBuffer()),//��������exeNameΪNULLʱ���ɽ�����������ǰ��
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
		ZTools::WriteZToolsFormatLog("�������߱༭��װ���������쳣,ErrorCode -- %s", lpMsgBuf);
		if (errCode == ERROR_ELEVATION_REQUIRED)
		{
			::MessageBox( GetMainWnd(), "�볢���Թ���Ա������ǰ����", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION );
		}
		else
		{
			CString s;
			s.Format("%s", lpMsgBuf);
			::MessageBox(GetMainWnd(), s, g_lpszAppTitle, MB_OK|MB_ICONINFORMATION );
		}
		LocalFree( lpMsgBuf );

		return 0;
	}

	if (m_hJob != NULL && m_hJob != INVALID_HANDLE_VALUE)
	{
		AssignProcessToJobObject(m_hJob, pinfo.hProcess);
	}

	CloseHandle(pinfo.hThread); 
	CloseHandle(pinfo.hProcess); 

	ZTools::WriteZToolsLog(ZTools::FormatString("��OnlineEdit������PeraComponentEditor������%d", pinfo.dwProcessId));

	return(pinfo.dwProcessId);
	/*
	//createprocess������Ȩ�����⣬�����滻Ϊshellexecuteex
	CString sOpentoolsBinPath = GetFlexwareBinPath();
	CString sOpentoolsPath = sOpentoolsBinPath + "\\ComponentEditor.exe";
	CString sParam;
	sParam.Format("-type webedit -visibility hidden");

	//����
	SHELLEXECUTEINFO si;
	memset(&si, NULL, sizeof(si));
	si.cbSize = sizeof(si);
	//si.nShow = SW_HIDE;
	si.fMask = SEE_MASK_NOCLOSEPROCESS;
	si.lpVerb = _T("open");
	si.lpDirectory = sOpentoolsBinPath;
	si.lpFile = sOpentoolsPath;
	si.lpParameters = sParam;
	if (!ShellExecuteEx(&si))
	{
		return 0;
	}
	DWORD dwPid = ::GetProcessId(si.hProcess);
	
	ZTools::WriteZToolsLog(ZTools::FormatString("��OnlineEdit������PeraComponentEditor������%d", dwPid));

	return dwPid;
	*/
}

CString SharedMemoryOnlineEdit::GetFlexwareBinPath()
{
	CString sRegResult;
	sRegResult = GetFlexwareBinPathMethod();
	return sRegResult;
}

void SharedMemoryOnlineEdit::InitJob()
{
	m_hJob = CreateJobObject( NULL, NULL); // GLOBAL

	if (m_hJob == NULL || m_hJob == INVALID_HANDLE_VALUE)
	{
		m_hJob = NULL;
		return;
	}

	JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = { 0 };

	// Configure all child processes associated with the job to terminate when the
	//����Ϊ��JOB������ʧ���Զ�ɱ��JOB�����е����н���
	jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE|JOB_OBJECT_LIMIT_SILENT_BREAKAWAY_OK;
	if( 0 == SetInformationJobObject( m_hJob, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli)))
	{
		CloseHandle(m_hJob);
		m_hJob = NULL;
		return;
	}
}
