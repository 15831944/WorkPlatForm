// DlgOpentools.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgOpentools.h"

#define TIMER_CLOSE								3
#define TIMER_FIND_WINDOW						11
#define TIMER_SEND_MISSON						13
#define TIMER_ANIMATE							14
#define TIMER_REFRESH_OPENTOOLS					15

UINT WM_FLEXWARE_RESIZE = ::RegisterWindowMessage (_T("WM_FLEXWARE_RESIZE"));

//新建组件(中间件>封装环境)
#define COPY_DATA_CREATE_COMPONENT			 	0
struct CREATE_COPYDATA
{
	void clear(void)
	{
		memset(&componentName, NULL, sizeof(CREATE_COPYDATA));
	}

	TCHAR componentName[256];
	TCHAR baseType[256];
	TCHAR localPath[1024];
	TCHAR paramsFilePath[1024];
	HWND hwndDlg;
};
//编辑组件(中间件>封装环境)
#define COPY_DATA_EDIT_COMPONENT			 	1
struct EDIT_COPYDATA
{
	void clear(void)
	{
		memset(&robotPath, NULL, sizeof(EDIT_COPYDATA));
	}

	TCHAR robotPath[1024];
	TCHAR paramsFilePath[1024];
	HWND hwndDlg;
};
//卸载工程(中间件>封装环境)
#define COPY_DATA_CLOSE_COMPONENT			 	2
struct CLOSE_COPYDATA
{
	void clear(void)
	{
		memset(&flag, NULL, sizeof(CLOSE_COPYDATA));
	}

	//保留
	TCHAR flag[256];
};
//组件编辑完成(封装环境>中间件)
#define COPY_DATA_COMPONENT_FINISH			 	3
struct FINISH_COPYDATA
{
	void clear(void)
	{
		memset(&flag, NULL, sizeof(FINISH_COPYDATA));
	}

	//标识封装环境是保存还取消
	TCHAR flag[256];
};

// CDlgOpentools 对话框

IMPLEMENT_DYNAMIC(CDlgOpentools, CDialog)

	CDlgOpentools::CDlgOpentools(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOpentools::IDD, pParent)
{
	m_dProcessId = 0;
	m_hWndOpentools = NULL;
	m_bApplied = FALSE;
}

CDlgOpentools::~CDlgOpentools()
{
}

void CDlgOpentools::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgOpentools, CDialog)
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_COPYDATA()
END_MESSAGE_MAP()

// CDlgOpentools 消息处理程序

BOOL CDlgOpentools::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText("组件编辑 - 类型：" + m_sBaseType + "  名称：" + m_sNodeName);

	HICON hIcon=AfxGetApp()->LoadIcon(IDI_ICON_OPENTOOLS);
	//SetIcon(hIcon, FALSE);
	SetIcon(hIcon, TRUE);
	
	// TODO:  在此添加额外的初始化
	CRect rcArce;
	SystemParametersInfo(SPI_GETWORKAREA,0,&rcArce,SPIF_SENDCHANGE);
	int nWorkHeight = rcArce.Height();
	int nWorkWidth = rcArce.Width();

// 	int x = 1120;
// 	int y = 630;
// 	int xStart = (nWorkWidth - x) / 2;
// 	int yStart = (nWorkHeight - y) / 2;

//	::SetWindowPos(m_hWnd, NULL, xStart, yStart, x, y, SWP_NOZORDER);
//	::SetWindowPos(m_hWnd, NULL, 0, 0, nWorkWidth, nWorkHeight, SWP_NOZORDER);
	SetWndPos();//设置窗口大小
	
	//////////////////////////////////////////////////////////////////////////
	//取窗口句柄
	DWORD dwPid = 0;
	HWND hWnd = 0;
	m_pSharedMemoryOnlineEdit->GetInfo(&dwPid, &hWnd);

	//窗口句柄有效，直接返回
	if (hWnd != 0 && IsWindow(hWnd))
	{
		m_hWndOpentools = hWnd;
		m_dProcessId = dwPid;
		//ShowWindow(SW_SHOWMAXIMIZED);
		BindWindow();
		//发送任务指令
		SetTimer(TIMER_SEND_MISSON, 1, NULL);
	}
	else
	{
		//窗口句柄无效时，从进程中查
		HANDLE hProcess = NULL;
		if (dwPid != 0)
		{
			hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwPid);
		}
		//若进程无效，重新启动封装环境
		if (hProcess == NULL || hProcess == INVALID_HANDLE_VALUE)
		{
			m_dProcessId = m_pSharedMemoryOnlineEdit->StartOpenTools();
			if (m_dProcessId == 0)
			{
				MyClose(IDCANCEL);
				return TRUE;
			}
			InitPaint();
			SetTimer(TIMER_FIND_WINDOW, 500, NULL);
		}
		else
		{
			m_dProcessId = dwPid;
			CloseHandle(hProcess);

			HWND hWndFindOpentools = SharedMemoryOnlineEdit::GetMainWindow(m_dProcessId, "组件封装", "HwndWrapper[PeraComponentEditor.exe");
			if (hWndFindOpentools && IsWindow(hWndFindOpentools))
			{
				m_hWndOpentools = hWndFindOpentools;

				//更新共享内存
				m_pSharedMemoryOnlineEdit->SetInfo(m_dProcessId, m_hWndOpentools);
				BindWindow();
				//发送任务指令
				SetTimer(TIMER_SEND_MISSON, 1, NULL);
			}
			else
			{
				InitPaint();
				SetTimer(TIMER_FIND_WINDOW, 500, NULL);
			}
		}
	}
	ShowWindow(SW_SHOWMAXIMIZED);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgOpentools::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == TIMER_CLOSE)
	{
		KillTimer(TIMER_FIND_WINDOW);
		KillTimer(TIMER_CLOSE);
		KillTimer(TIMER_SEND_MISSON);
		KillTimer(TIMER_ANIMATE);
		KillTimer(TIMER_REFRESH_OPENTOOLS);
		EndDialog(m_nCode);
	}
	else if (nIDEvent == TIMER_FIND_WINDOW)
	{
		//BeginWaitCursor();
		HWND hWnd = SharedMemoryOnlineEdit::GetMainWindow(m_dProcessId, "组件封装", "HwndWrapper[PeraComponentEditor.exe");
		if (hWnd && IsWindow(hWnd))
		{
			KillTimer(TIMER_FIND_WINDOW);

			m_hWndOpentools = hWnd;

			//更新共享内存
			m_pSharedMemoryOnlineEdit->SetInfo(m_dProcessId, m_hWndOpentools);

			// 			//发送指令
			// 			Sleep(1000);
			// 			SendMession();
			UndoInitPaint();
			BindWindow();
			SetTimer(TIMER_SEND_MISSON, 1, NULL);
			//EndWaitCursor();
		}
	}
	else if (nIDEvent == TIMER_SEND_MISSON)
	{
		KillTimer(TIMER_SEND_MISSON);
		//UndoInitPaint();
		SendMession();
	}
	else if (nIDEvent == TIMER_ANIMATE)
	{
		static int i = 0;
		i = i % m_nFrameCount;
		UpdateBk(i);
		i++;
	}
	else if (nIDEvent == TIMER_REFRESH_OPENTOOLS)
	{
		static int nRefreshCount = 0;
		nRefreshCount++;
		if (nRefreshCount < 3)
		{
			if (m_hWndOpentools && IsWindow(m_hWndOpentools))
			{
				::PostMessage(m_hWndOpentools, WM_SIZE, 0, 0);
			}
		}
		else
		{
			KillTimer(TIMER_REFRESH_OPENTOOLS);
		}
	}
	CDialog::OnTimer(nIDEvent);
}

void CDlgOpentools::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	return;
	CDialog::OnOK();
}


//查找封装弹出的窗口的回调函数的参数
typedef struct structGetOpentoolsPopupWindowParam
{
	struct structGetOpentoolsPopupWindowParam()
		: dwProcessId(0)
		, hwnd(NULL)
	{}
	HWND hwnd;
	DWORD dwProcessId;
} GET_OPENTOOLS_POPUP_WINDOW_PARAM;

//查找封装弹出的窗口的回调函数
BOOL CALLBACK CDlgOpentools::EnumWindowCallback_GetOpentoolsPopupWindow(HWND hWnd, LPARAM lParam)
{
	GET_OPENTOOLS_POPUP_WINDOW_PARAM *pData = (GET_OPENTOOLS_POPUP_WINDOW_PARAM *)lParam;
	if (pData == NULL) return TRUE;

	DWORD dwProcessId = 0;
	GetWindowThreadProcessId(hWnd, &dwProcessId); //取得当前窗口对应的进程id
	if (dwProcessId != pData->dwProcessId) return TRUE;
	
	//封装弹出的窗口，是窗口，父为空，所有者为opentools，可视的
	if (!IsWindow(hWnd))
	{
		return TRUE;
	}

	if (::GetParent(hWnd) != NULL)
	{
		return TRUE;
	}

	DWORD dwStyle = ::GetWindowLong(hWnd, GWL_STYLE);
	if ( (dwStyle&WS_VISIBLE) == 0 )
	{
		return TRUE;
	}	

	char szClassName[MAX_PATH];
	GetClassName(hWnd, szClassName,sizeof(szClassName) / sizeof(szClassName[0]));
	CString strClassName =  szClassName;
	strClassName.MakeLower();
	if(strClassName.Find("peracomponenteditor.exe") == -1) 
	{
		return TRUE;
	}

	pData->hwnd = hWnd;
	return FALSE;
}

//根据进程ID，封装弹出的窗口句柄
HWND CDlgOpentools::GetOpentoolsPopupWindow(DWORD dwPID)
{
	GET_OPENTOOLS_POPUP_WINDOW_PARAM param;
	param.dwProcessId = dwPID;
	EnumWindows(EnumWindowCallback_GetOpentoolsPopupWindow, (LPARAM)&param);
	return param.hwnd;
}

void CDlgOpentools::OnCancel()
{
	if (m_hWndOpentools && IsWindow(m_hWndOpentools))
	{
		::PostMessage(m_hWndOpentools, WM_KEYDOWN, VK_ESCAPE, 0);
		::PostMessage(m_hWndOpentools, WM_KEYUP, VK_ESCAPE, 0);
		Sleep(1);
	}
	// TODO: 在此添加专用代码和/或调用基类
	//封装环境有弹出窗口时不允许关闭
	if (m_dProcessId)
	{
		HWND hwnd = GetOpentoolsPopupWindow(m_dProcessId);
		if (hwnd != 0 && IsWindow(hwnd))
		{
			::BringWindowToTop(hwnd);
			return;
		}
	}

	//int ret = MessageBox("确定未保存直接退出在线封装环境吗？", "在线编辑", MB_OKCANCEL);
	//if (IDOK != ret)
	//{
	//	return;
	//}

	if (m_hWndOpentools && IsWindow(m_hWndOpentools))
	{		
		CString sFlag = "cancel";
		CLOSE_COPYDATA cdClose;
		cdClose.clear();
		strcpy_s(cdClose.flag, sFlag);

		COPYDATASTRUCT cpd;
		memset(&cpd, NULL, sizeof(COPYDATASTRUCT));
		cpd.cbData = sizeof(CLOSE_COPYDATA);
		cpd.dwData = COPY_DATA_CLOSE_COMPONENT;
		cpd.lpData = &cdClose;

		::SendMessage(m_hWndOpentools, WM_COPYDATA, NULL, (LPARAM)&cpd);
	}

	MyClose(IDCANCEL);

	//CDialog::OnCancel();
}

void CDlgOpentools::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (m_hWndOpentools && IsWindow(m_hWndOpentools))
	{
		CRect rc;
		GetClientRect(&rc);
		// 		//::SetWindowPos(m_hWndOpentools, NULL, 0, 0, rc.Width(), rc.Height(), SWP_NOZORDER|SWP_SHOWWINDOW);
		::MoveWindow(m_hWndOpentools, 0, 0, rc.Width(), rc.Height(), TRUE);
		int ns = rc.Width() - 8;
		ns = ns << 16;
		ns |= rc.Height() - 4;
		::SendMessage(m_hWndOpentools, WM_FLEXWARE_RESIZE, ns, TRUE);
	}
}

void CDlgOpentools::MyClose(int nCode)
{
	//ZTools::WriteZToolsLog("--ShowWindow(SW_HIDE);");
	//ShowWindow(SW_HIDE);
	//ZTools::WriteZToolsLog("ShowWindow(SW_HIDE);");
	UnbindWindow();
	//ZTools::WriteZToolsLog("UnbindWindow();");
	//下面这句是有时DSE会跑到其它程序后面
	//GetParent()->SetForegroundWindow();
	//GetParent()->BringWindowToTop();
	//ZTools::WriteZToolsLog("GetParent()->BringWindowToTop();");
	//GetParent()->SetActiveWindow();
	//ZTools::WriteZToolsLog("GetParent()->SetActiveWindow();");
	m_nCode = nCode;
	//ZTools::WriteZToolsLog("m_nCode = nCode;");
	//EndDialog(m_nCode);
	//ZTools::WriteZToolsLog("EndDialog(m_nCode);");
	SetTimer(TIMER_CLOSE, 500, NULL);
}
void CDlgOpentools::SendMession()
{
	if (m_sOpenFlag.CompareNoCase("CreateComponent") == 0
		|| m_sOpenFlag.CompareNoCase("CreateForm") == 0)
	{
		m_sLocalPath.TrimRight('\\');

		CREATE_COPYDATA cdCreate;
		cdCreate.clear();
		strcpy_s(cdCreate.componentName, m_sComponentName);
		strcpy_s(cdCreate.baseType, m_sBaseType);
		strcpy_s(cdCreate.localPath, m_sLocalPath);
		strcpy_s(cdCreate.paramsFilePath, m_sParamsFilePath);
		cdCreate.hwndDlg = m_hWnd;

		COPYDATASTRUCT cpd;
		memset(&cpd, NULL, sizeof(COPYDATASTRUCT));
		cpd.cbData = sizeof(CREATE_COPYDATA);
		cpd.dwData = COPY_DATA_CREATE_COMPONENT;
		cpd.lpData = &cdCreate;

		ZTools::WriteZToolsFormatLog("SendMessage Create\n%s\n%s\n%s\n%s", m_sComponentName, m_sBaseType, m_sLocalPath, m_sParamsFilePath);
		::SendMessage(m_hWndOpentools, WM_COPYDATA, NULL, (LPARAM)&cpd);
	}
	else if (m_sOpenFlag.CompareNoCase("EditComponent") == 0
		|| m_sOpenFlag.CompareNoCase("EditForm") == 0)
	{
		EDIT_COPYDATA cdEdit;
		cdEdit.clear();
		strcpy_s(cdEdit.robotPath, m_sLocalRobotPath);
		strcpy_s(cdEdit.paramsFilePath, m_sParamsFilePath);
		cdEdit.hwndDlg = m_hWnd;

		COPYDATASTRUCT cpd;
		memset(&cpd, NULL, sizeof(COPYDATASTRUCT));
		cpd.cbData = sizeof(EDIT_COPYDATA);
		cpd.dwData = COPY_DATA_EDIT_COMPONENT;
		cpd.lpData = &cdEdit;

		ZTools::WriteZToolsFormatLog("SendMessage Edit\n%s\n%s", m_sLocalRobotPath, m_sParamsFilePath);
		::SendMessage(m_hWndOpentools, WM_COPYDATA, NULL, (LPARAM)&cpd);
	}
	else
	{
		MyClose(IDCANCEL);
		return;
	}
}

void CDlgOpentools::SetWndPos()
{
	CRect rect;
	SystemParametersInfo(SPI_GETWORKAREA,0,&rect,SPIF_SENDCHANGE);
	//rect.DeflateRect(30, 30, 30, 30);
	::SetWindowPos(m_hWnd, NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER); //缩小一些
}

void CDlgOpentools::BindWindow()
{
	//////////////////////////////////////////////////////////////////////////
	CRect rcArce;
	SystemParametersInfo(SPI_GETWORKAREA,0,&rcArce,SPIF_SENDCHANGE);
	int nWorkHeight = rcArce.Height();
	int nWorkWidth = rcArce.Width();

	int x = 1120;
	int y = 630;
	int xStart = (nWorkWidth - x) / 2;
	int yStart = (nWorkHeight - y) / 2;

	//::SetWindowPos(m_hWnd, NULL, xStart , yStart, x, y, SWP_NOZORDER);
	{
		SetWndPos(); //缩小一些
	}
	//::SetWindowPos(m_hWnd, NULL, 0 , 0, nWorkWidth, nWorkHeight, SWP_NOZORDER); //设置为全屏
	//////////////////////////////////////////////////////////////////////////

	ModifyStyle(m_hWndOpentools, WS_THICKFRAME|WS_SYSMENU, WS_CHILD, 0);
	ModifyStyleEx(m_hWndOpentools, WS_EX_APPWINDOW, NULL, 0);

	//取得原始风格
	// 			DWORD dwExStyle = ::GetWindowLong(m_hWndOpentools, GWL_EXSTYLE);
	// 			DWORD dwStyle = ::GetWindowLong(m_hWndOpentools, GWL_STYLE);
	// 
	// 			//WRITELOG(GetWindowStyleStr(m_hwndApp));
	// 
	// 			DWORD m_dwAppStyle;
	// 			DWORD m_dwAppExStyle;
	// 			m_dwAppStyle = dwStyle;
	// 			m_dwAppExStyle = dwExStyle;
	// 			//添加
	// 			//dwStyle |= WS_CHILD; //如果嵌入的是wpf的程序，必须设置此属性，否则，会导致嵌入的wpf程序在切换tab标签页时，不显示，需要手工点击一次页面才显示。
	// 			dwStyle |= WS_VISIBLE;
	// 			dwStyle |= WS_CLIPSIBLINGS;
	// 			dwStyle |= WS_CLIPCHILDREN;
	// 
	// 			//dwExStyle |= WS_EX_TOOLWINDOW; //移走程序的任务栏图标
	// 
	// 			//移除
	// 			dwStyle &= ~WS_CAPTION;
	// 			dwStyle &= ~WS_POPUP;
	// 			dwStyle &= ~WS_OVERLAPPED;
	// 			dwStyle &= ~WS_THICKFRAME;
	// 			dwStyle &= ~WS_DLGFRAME;
	// 			dwStyle &= ~WS_BORDER; //边框
	// 			dwStyle &= ~WS_SYSMENU;
	// 			dwStyle &= ~WS_MINIMIZE;
	// 			dwStyle &= ~WS_MAXIMIZE;
	// 			dwStyle &= ~WS_MINIMIZEBOX;
	// 			dwStyle &= ~WS_MAXIMIZEBOX;
	// 
	// 			dwExStyle &= ~WS_EX_APPWINDOW;
	// 
	// 			//设置父子关系和窗口风格
	// 			::SetWindowLong(m_hWndOpentools, GWL_EXSTYLE, dwExStyle);
	// 			::SetWindowLong(m_hWndOpentools, GWL_STYLE, dwStyle);

	::SetParent(m_hWndOpentools, this->GetSafeHwnd());
	CRect rc;
	GetClientRect(&rc);
	::MoveWindow(m_hWndOpentools, 0, 0, rc.Width() - 1, rc.Height() - 1, TRUE);
	::SetWindowPos(m_hWndOpentools, NULL, 0, 0, rc.Width(), rc.Height(), SWP_NOZORDER|SWP_SHOWWINDOW|SWP_FRAMECHANGED|SWP_NOACTIVATE);
	::MoveWindow(m_hWndOpentools, 0, 0, rc.Width(), rc.Height(), TRUE);

	int ns = rc.Width() - 8;
	ns = ns << 16;
	ns |= rc.Height() - 4;
	//::SendMessage(m_hWndOpentools, WM_FLEXWARE_RESIZE, ns, TRUE);

	SetTimer(TIMER_REFRESH_OPENTOOLS, 1000, NULL);
	m_bApplied = FALSE;
}
void CDlgOpentools::UnbindWindow()
{
	//ZTools::WriteZToolsLog("--::ShowWindow(m_hWndOpentools, SW_HIDE);");
	::ShowWindow(m_hWndOpentools, SW_HIDE);
	//ZTools::WriteZToolsLog("::ShowWindow(m_hWndOpentools, SW_HIDE);");
	//ZTools::WriteZToolsLog("--::SetParent(m_hWndOpentools, NULL);");
	::SetParent(m_hWndOpentools, NULL);
	//ZTools::WriteZToolsLog("::SetParent(m_hWndOpentools, NULL);");
}
BOOL CDlgOpentools::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (pCopyDataStruct->dwData == COPY_DATA_COMPONENT_FINISH)
	{
		FINISH_COPYDATA* pCdFinish = (FINISH_COPYDATA*)pCopyDataStruct->lpData;

		CStringW strw = (WCHAR *)pCdFinish->flag;
		CString strFlag;
		strFlag = strw;

		//ZTools::WriteZToolsLog(ZTools::FormatString("收到封装环境发来完成消息--%s", strFlag));

		if (strFlag.CompareNoCase("save") == 0)
		{
			MyClose(IDOK);
			//ZTools::WriteZToolsLog("MyClose(IDOK);");
		}
		else if (strFlag.CompareNoCase("apply")==0)
		{
			m_bApplied = TRUE;
		}
		else
		{
			MyClose(IDCANCEL);
			//ZTools::WriteZToolsLog("MyClose(IDCANCEL);");
		}
	}

	//ZTools::WriteZToolsLog("--return CDialog::OnCopyData(pWnd, pCopyDataStruct);");
	return CDialog::OnCopyData(pWnd, pCopyDataStruct);
}
BOOL CDlgOpentools::ImageFromIDResource(UINT nID, LPCTSTR sTR, Gdiplus::Image * &pImg)
{
	HINSTANCE hInst = AfxGetResourceHandle();
	HRSRC hRsrc = ::FindResource (hInst,MAKEINTRESOURCE(nID),sTR); // type
	if (!hRsrc)
		return FALSE;

	// load resource into memory
	DWORD len = SizeofResource(hInst, hRsrc);
	BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);
	if (!lpRsrc)
		return FALSE;

	// Allocate global memory on which to create stream
	HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, len);
	BYTE* pmem = (BYTE*)GlobalLock(m_hMem);
	memcpy(pmem,lpRsrc,len);
	IStream* pstm;
	CreateStreamOnHGlobal(m_hMem,FALSE,&pstm);

	// load from stream
	pImg=Gdiplus::Image::FromStream(pstm);

	// free/release stuff
	GlobalUnlock(m_hMem);
	pstm->Release();
	FreeResource(lpRsrc);

	return TRUE;
}

BOOL CDlgOpentools::InitPaint()
{
	//////////////////////////////////////////////////////////////////////////

	m_dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	m_dwExStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);

	ModifyStyle(WS_CAPTION|WS_BORDER|WS_DLGFRAME|WS_THICKFRAME, NULL);
	ModifyStyleEx(WS_EX_WINDOWEDGE|WS_EX_OVERLAPPEDWINDOW|WS_EX_PALETTEWINDOW, NULL);

	DWORD dwExStyle=GetWindowLong(m_hWnd,GWL_EXSTYLE);
	if((dwExStyle & WS_EX_LAYERED) != WS_EX_LAYERED)
		SetWindowLong(m_hWnd, GWL_EXSTYLE, dwExStyle^WS_EX_LAYERED);

	ImageFromIDResource(IDB_PNG_ANIMATE, "PNG", m_pImage);

	m_nSrcWidth = m_pImage->GetWidth();
	m_nSrcHeight = m_pImage->GetHeight();
		
	m_nDstWidth = 70;
	m_nDstHeight = 70;
	m_nFrameCount = 12;
	m_nFrameDelay = 90;

	m_Blend.BlendOp=0; //theonlyBlendOpdefinedinWindows2000
	m_Blend.BlendFlags=0; //nothingelseisspecial...
	m_Blend.AlphaFormat=1; //...
	m_Blend.SourceConstantAlpha=255;//AC_SRC_ALPHA

	SetTimer(TIMER_ANIMATE,m_nFrameDelay,NULL);
	//////////////////////////////////////////////////////////////////////////
	return TRUE;
}

HBITMAP CDlgOpentools::KCreateCompatibleBitmap(int nWidth, int nHeight)  
{  
	BYTE                      * pBits ;   
	BITMAPINFOHEADER          bmih;   
	ZeroMemory( &bmih, sizeof(BITMAPINFOHEADER));   

	bmih.biSize                 = sizeof (BITMAPINFOHEADER) ;   
	bmih.biWidth                = nWidth;   
	bmih.biHeight               = nHeight;   
	bmih.biPlanes               = 1 ;   
	bmih.biBitCount             = 32;        //这里一定要是32   
	bmih.biCompression          = BI_RGB ;   
	bmih.biSizeImage            = 0 ;   
	bmih.biXPelsPerMeter        = 0 ;   
	bmih.biYPelsPerMeter        = 0 ;   
	bmih.biClrUsed              = 0 ;   
	bmih.biClrImportant          = 0 ;   

	HBITMAP hBitMap = CreateDIBSection (NULL, (BITMAPINFO *)  &bmih, 0, (VOID**)&pBits, NULL, 0) ;  
	return hBitMap;  
}  

BOOL CDlgOpentools::UpdateBk(int i)
{
	HDC hdcTemp=::GetDC (m_hWnd);
	m_hdcMemory=CreateCompatibleDC(hdcTemp);
	//HBITMAP hBitMap=CreateCompatibleBitmap(hdcTemp, m_nBkWidth, m_nBkHeight);
	HBITMAP hBitMap=KCreateCompatibleBitmap(m_nDstWidth, m_nDstHeight);
	SelectObject(m_hdcMemory,hBitMap);

	m_Blend.SourceConstantAlpha=255;
	HDC hdcScreen=::GetDC (m_hWnd);

	RECT rct;
	CWnd* pWnd = GetParent();
	if (pWnd)
	{
		pWnd->GetWindowRect(&rct);
	}
	else
	{
		GetWindowRect(&rct);
	}
	rct.left = (rct.left + rct.right - m_nDstWidth)/2;
	rct.top = (rct.top + rct.bottom - m_nDstHeight)/2;
	POINT ptWinPos={rct.left, rct.top};

	Gdiplus::Graphics graph(m_hdcMemory);

	Gdiplus::Rect rc(0,0,m_nDstWidth,m_nDstHeight);

	int nTempWidth = m_nSrcWidth/m_nFrameCount;
	graph.DrawImage(m_pImage, rc, i*nTempWidth,0,nTempWidth,m_nSrcHeight,Gdiplus::UnitPixel);

	SIZE sizeWindow={m_nDstWidth, m_nDstHeight};
	POINT ptSrc={0,0};
	DWORD dwExStyle=GetWindowLong(m_hWnd,GWL_EXSTYLE);
	if((dwExStyle & WS_EX_LAYERED) != WS_EX_LAYERED)
		SetWindowLong(m_hWnd, GWL_EXSTYLE, dwExStyle^WS_EX_LAYERED);

	BOOL bRet=FALSE;
	bRet= ::UpdateLayeredWindow( m_hWnd, hdcScreen, &ptWinPos,
		&sizeWindow, m_hdcMemory, &ptSrc, 0, &m_Blend, 2);
	graph.ReleaseHDC(m_hdcMemory);
	::ReleaseDC(m_hWnd,hdcScreen);
	hdcScreen=NULL;
	::ReleaseDC(m_hWnd,hdcTemp);
	hdcTemp=NULL;
	DeleteObject(hBitMap);
	DeleteDC(m_hdcMemory);
	m_hdcMemory=NULL;
	return bRet;
}

void CDlgOpentools::UndoInitPaint()
{
	KillTimer(TIMER_ANIMATE);

	ShowWindow(SW_HIDE);
	//////////////////////////////////////////////////////////////////////////
	CRect rcArce;
	SystemParametersInfo(SPI_GETWORKAREA,0,&rcArce,SPIF_SENDCHANGE);
	int nWorkHeight = rcArce.Height();
	int nWorkWidth = rcArce.Width();

// 	int x = 1120;
// 	int y = 630;
// 	int xStart = (nWorkWidth - x) / 2;
// 	int yStart = (nWorkHeight - y) / 2;
// 
	// 	::SetWindowPos(m_hWnd, NULL, xStart , yStart, x-1, y-1, SWP_NOZORDER|SWP_FRAMECHANGED);
	//::SetWindowPos(m_hWnd, NULL, 0, 0, nWorkWidth, nWorkHeight, SWP_NOZORDER);
	SetWndPos();//设置窗口大小
	//////////////////////////////////////////////////////////////////////////

	SetWindowLong(m_hWnd, GWL_STYLE, m_dwStyle);
	SetWindowLong(m_hWnd, GWL_EXSTYLE, m_dwExStyle);

	DWORD dwExStyle=GetWindowLong(m_hWnd,GWL_EXSTYLE);
	if((dwExStyle & WS_EX_LAYERED) == WS_EX_LAYERED)
		SetWindowLong(m_hWnd, GWL_EXSTYLE, dwExStyle^WS_EX_LAYERED);
	ShowWindow(SW_SHOW);
}