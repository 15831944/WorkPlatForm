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

//查找进程主窗口的回调函数的参数
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

//查找进程主窗口的回调函数
BOOL CALLBACK SharedMemoryOnlineEdit::EnumWindowCallback_GetMainWindow(HWND hWnd, LPARAM lParam)
{
	GETMAINWINDOW_PARAM *pData = (GETMAINWINDOW_PARAM *)lParam;
	if (pData == NULL) return TRUE;

	DWORD dwProcessId = 0;
	GetWindowThreadProcessId(hWnd, &dwProcessId); //取得当前窗口对应的进程id
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

//根据进程ID，主窗口标题（模糊匹配）取得进程主窗口句柄
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

	//当前进程ID
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

	//如果已存在，表示已有其它进程创建并在维护这块内存
	if (m_pSmOnlineEdit->AlreadyExist())
	{
		delete m_pSmOnlineEdit;
		m_pSmOnlineEdit = NULL;
		return TRUE;
	}
	else//如果是PeraProcessDesigner.exe中执行以下操作
	{
		DWORD dwPID = StartOpenTools();
		//更新共享内存
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

	//关进程并清空共享内存里的值
	OPENTOOLS_INFO_STRUCT info;
	memset(&info, 0, sizeof(OPENTOOLS_INFO_STRUCT));

	m_pSmOnlineEdit->Lock();
	memcpy(&info, m_pSmOnlineEdit->GetData(), sizeof(OPENTOOLS_INFO_STRUCT));
	m_pSmOnlineEdit->Unlock();

	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, info.dwPid);
	if (hProcess != NULL && hProcess != INVALID_HANDLE_VALUE)
	{
		ZTools::WriteZToolsLog(ZTools::FormatString("【OnlineEdit】关闭PeraComponentEditor进程%d", info.dwPid));

		TerminateProcess(hProcess, 0);
		CloseHandle(hProcess);
		hProcess = NULL;
	}
	else
	{
		ZTools::WriteZToolsLog(ZTools::FormatString("【OnlineEdit】访问PeraComponentEditor进程%d失败", info.dwPid));
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
	GetModuleFileName(NULL, exeFullPath, MAX_PATH);//得到程序模块名称，全路径
	_splitpath_s( exeFullPath, drive, dir, fname, ext );

	CString sPeraComponentEditorPath = GetPeraComponentEditorPath();
	if (sPeraComponentEditorPath.IsEmpty())
		return 0;
	if (!PathFileExists(sPeraComponentEditorPath))
	{
		CString sMsg;
		sMsg.Format("未找到 %s", sPeraComponentEditorPath);
		::MessageBox(GetMainWnd(), sMsg, g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		return 0;
	}
	CString sParam;
	sParam.Format("\"%s\" -type webedit -visibility hidden", sPeraComponentEditorPath);

	STARTUPINFO StartInfo;
	PROCESS_INFORMATION pinfo;
	memset(&StartInfo,0,sizeof(STARTUPINFO));
	StartInfo.cb = sizeof(STARTUPINFO);				//设定结构的大小
	StartInfo.dwFlags   =   STARTF_USESHOWWINDOW;	//| STARTF_USESTDHANDLES;   
	StartInfo.wShowWindow = SW_HIDE;
	if( !CreateProcess(
		NULL, //启动程序路径名
		(char*)(sParam.GetBuffer()),//参数（当exeName为NULL时，可将命令放入参数前）
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
		ZTools::WriteZToolsFormatLog("创建在线编辑封装环境进程异常,ErrorCode -- %s", lpMsgBuf);
		if (errCode == ERROR_ELEVATION_REQUIRED)
		{
			::MessageBox( GetMainWnd(), "请尝试以管理员启动当前进程", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION );
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

	ZTools::WriteZToolsLog(ZTools::FormatString("【OnlineEdit】启动PeraComponentEditor，进程%d", pinfo.dwProcessId));

	return(pinfo.dwProcessId);
	/*
	//createprocess可能有权限问题，考虑替换为shellexecuteex
	CString sOpentoolsBinPath = GetFlexwareBinPath();
	CString sOpentoolsPath = sOpentoolsBinPath + "\\ComponentEditor.exe";
	CString sParam;
	sParam.Format("-type webedit -visibility hidden");

	//运行
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
	
	ZTools::WriteZToolsLog(ZTools::FormatString("【OnlineEdit】启动PeraComponentEditor，进程%d", dwPid));

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
	//设置为当JOB对象消失后，自动杀掉JOB对象中的所有进程
	jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE|JOB_OBJECT_LIMIT_SILENT_BREAKAWAY_OK;
	if( 0 == SetInformationJobObject( m_hJob, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli)))
	{
		CloseHandle(m_hJob);
		m_hJob = NULL;
		return;
	}
}
