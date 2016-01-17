// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "PeraDeskClient.h"

#include "MainFrm.h"
//#include <DlgProcessNew.h>
//#include <DlgProcessOpen.h>
#include "CxBCGPVisualManager2007.h"
#include "PtrVectorHeader.h"
#include "CxBCGPToolbarButton.h"
//#include "DlgSvg.h"
//#include "XmlHistoryModel.h"
#include "ResLibImageSet.h"
#include "PeraDeskClientUtility.h"
//#include "DlgStructTreePropComponent.h"
//#include "DlgStructTreePropProcess.h"
//#include "DlgStructTreePropFlow.h"
//#include <dwmapi.h>
//#include "DlgIf.h"
//#include <ResLibData.h>
#include "DlgLoginBk.h"
#include "HookFunc.h"
//#include "DlgExpEditorDM.h"
//#include "DlgRunFlow.h"
//#include "GetFlexwareBin.h"
#include <shellapi.h>
#include "Tools.h"
//#include <DlgDistribution.h>
#include "CxWebPageEvent.h"
//#include "DlgViewRunFlow.h"
#include "JsData_HistoryMonitor.h"
#include "UtilityFunc.h"
//#include "DlgParamSet.h"
//#include "CxStructTreeNodeFlow.h"
//#include "CxArrayND.h"
//#include "CxStructTreeNodeParam.h"
//#include "JsData_Svg.h"
//#include "DlgFor.h"
#include "GetFlexwareBin.h"
#include "DlgSystemSetting.h"
#include "ConfigFunc.h"
#include <GetGlobalDataFromReg.h>
//DwmSetWindowAttribute
//#pragma comment(lib, "Dwmapi.lib")

#include "BaseHtmlDialog.h"
#include <afxstr.h>
#include "csharedmemory.h"
#include "MsgDlg.h"
#include "LoginFunc.h"
#include "RegFunc.h"
#include "LoadingWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*
在结构树更新组件时，如果是数据映射导致组件更新，则To参数Id保存在g_strLinkerToNodeId。
*/
//static CString g_strLinkerToNodeId;

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CBCGPMDIFrameWnd)

	const int  iMaxUserToolbars		= 10;
const UINT uiFirstUserToolBarId	= AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId	= uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CBCGPMDIFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_COMMAND(ID_WINDOW_MANAGER, OnWindowManager)
	ON_COMMAND(ID_VIEW_CUSTOMIZE, OnViewCustomize)
	ON_REGISTERED_MESSAGE(BCGM_RESETTOOLBAR, OnToolbarReset)
//	ON_REGISTERED_MESSAGE(WM_WEBPAGE_LOADWEB_FINISHED, OnLoadWebPageFinished)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_2000, ID_VIEW_APPLOOK_WIN7, OnAppLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_2000, ID_VIEW_APPLOOK_WIN7, OnUpdateAppLook)
	ON_COMMAND(ID_MDI_MOVE_TO_NEXT_GROUP, OnMdiMoveToNextGroup)
	ON_COMMAND(ID_MDI_MOVE_TO_PREV_GROUP, OnMdiMoveToPrevGroup)
	ON_COMMAND(ID_MDI_NEW_HORZ_TAB_GROUP, OnMdiNewHorzTabGroup)
	ON_COMMAND(ID_MDI_NEW_VERT_GROUP, OnMdiNewVertGroup)
	ON_COMMAND(ID_MDI_CANCEL, OnMdiCancel)
	//ON_COMMAND(IDM_MODULE_OPENJIANMO, &CMainFrame::OnMenuModuleOpenjianmo)
	//ON_COMMAND(IDM_MODULE_ACTIVEJIANMO, &CMainFrame::OnMenuModuleActivejianmo)
	//ON_COMMAND(IDM_MODULE_CLOSEJIANMO, &CMainFrame::OnMenuModuleClosejianmo)
	//ON_COMMAND(IDM_TASK_NEW, &CMainFrame::OnMenuTaskNew)
	//ON_COMMAND(IDM_TASK_OPEN, &CMainFrame::OnMenuTaskOpen)
	//ON_COMMAND(IDM_TASK_RUN, &CMainFrame::OnMenuTaskRun)
//	ON_COMMAND(IDM_TOOL_HISTORY_MONITOR, &CMainFrame::OnMenuHistoryMonitor)
//	ON_COMMAND(IDM_TASK_SAVEAS, &CMainFrame::OnMenuTaskSaveAs)
	ON_COMMAND_RANGE(IDM_TOOLBAR_BUTTON_FIRST, IDM_TOOLBAR_BUTTON_LAST, CMainFrame::OnToolbarClicked)
//	ON_COMMAND_RANGE(IDM_ROBOTBAR_BUTTON_FIRST, IDM_ROBOTBAR_BUTTON_LAST, CMainFrame::OnRobotbarClicked)
	//	ON_COMMAND_RANGE(IDM_LOGINBAR_BUTTON_FIRST, IDM_LOGINBAR_BUTTON_LAST, CMainFrame::OnLoginbarClicked)
//	ON_MESSAGE(WM_UPDATE_DATAMAPPING, &CMainFrame::OnUpdateDataMapping)
	//ON_MESSAGE(WM_UPDATE_COMPONENT_BY_LINKER, &CMainFrame::OnUpdateComponentByLinker)
	//ON_MESSAGE(WM_POP_MENU, &CMainFrame::OnPopMenuForSvg)
////	ON_MESSAGE(WM_STRUCTTREE_UPDATE_PARAM_ARRAY, &CMainFrame::OnStructTreeUpdateParamArray)
//	ON_MESSAGE(WM_STRUCTTREE_RUN_COMPNENT, &CMainFrame::OnStructTreeRunComponent)
//	ON_MESSAGE(WM_STRUCTTREE_UPDATE_PROP, &CMainFrame::OnStructTreeUpdateProp)
//	ON_MESSAGE(WM_STRUCTTREE_SHOW_TOOLTIP, &CMainFrame::OnStructTreeShowToolTip)
//	ON_MESSAGE(WM_STRUCTTREE_SHOW_DATAMAPPING, &CMainFrame::OnStructTreeShowDataMapping)
//	ON_MESSAGE(WM_SHOW_SVG_DRAG_LAYER, &CMainFrame::OnShowSvgDragLayer)

//	ON_MESSAGE(WM_UPDATE_RESLIB_TREE, &CMainFrame::OnUpdateResLibTree)
//
	//ON_MESSAGE(WM_STRUCTTREE_EDIT_IF, &CMainFrame::OnStructtreeEditIf)
//	ON_MESSAGE(WM_STRUCTTREE_EDIT_ROOTPARAMS, &CMainFrame::OnStructtreeEditRootParams)
	//ON_MESSAGE(WM_STRUCTTREE_EDIT_FOR, &CMainFrame::OnStructtreeEditFor)
	//ON_MESSAGE(WM_STRUCTTREE_EDIT_DATAMAPPING_EXP, &CMainFrame::OnStructtreeEditDataMappingExp)
	//ON_MESSAGE(WM_STRUCTTREE_RUN_PROCESS, &CMainFrame::OnStructTreeRunProcess)
	//ON_MESSAGE(WM_STRUCTTREE_RUN_SUB_PROCESS, &CMainFrame::OnStructTreeRunSubProcess)
	//ON_MESSAGE(WM_STRUCTTREE_SAVEAS_SUB_PROCESS, &CMainFrame::OnStructTreeSaveAsSubProcess)
//	ON_MESSAGE(WM_SHOW_RUNFLOW_DIALOG, &CMainFrame::OnShowRunFlowDialog)
//	ON_MESSAGE(WM_STRUCTTREE_UPDATE_DMS_DATA, &CMainFrame::OnStructtreeUpdateDmsData)

	//ON_MESSAGE(WM_VIEW_RUNFLOW_WEBPAGE, &CMainFrame::OnViewRunFlowWebPage)
	ON_MESSAGE(WM_ATTACH_WEBPAGE, &CMainFrame::OnAttachWebPage)
	ON_MESSAGE(WM_LOGINBAR_CLICKED, &CMainFrame::OnLoginbarClicked)

	ON_MESSAGE(WM_PROGRESS_SHOW, &CMainFrame::OnShowProgress)
	ON_COMMAND(IDM_VIEW_TOOLBARZ, &CMainFrame::OnViewToolbarz)
	ON_COMMAND(IDM_VIEW_COMPONENT, &CMainFrame::OnViewComponent)
	ON_COMMAND(IDM_VIEW_LOGIN, &CMainFrame::OnViewLogin)
	ON_UPDATE_COMMAND_UI(IDM_VIEW_COMPONENT, &CMainFrame::OnUpdateViewComponent)
	ON_UPDATE_COMMAND_UI(IDM_VIEW_LOGIN, &CMainFrame::OnUpdateViewLogin)
	ON_UPDATE_COMMAND_UI(IDM_VIEW_TOOLBARZ, &CMainFrame::OnUpdateViewToolbarz)

//	ON_COMMAND(IDM_TASK_INFO, &CMainFrame::OnTaskInfo)
//	ON_COMMAND(IDM_TASK_EXIT, &CMainFrame::OnTaskExit)
	//ON_COMMAND(IDM_TASK_CLOSEALL, &CMainFrame::OnTaskCloseall)
//	ON_COMMAND(IDM_TASK_SAVE, &CMainFrame::OnTaskSave)
//	ON_COMMAND(IDM_TOOL_FLEXWARE, &CMainFrame::OnToolFlexware)
//	ON_COMMAND(IDM_DISTRIBUTION, &CMainFrame::OnDistribution)
	ON_COMMAND(IDM_HELP_ABOUT, &CMainFrame::OnHelpAbout)
	ON_COMMAND(IDM_SYS_LOGIN, &CMainFrame::OnSysLogin)
	ON_COMMAND(IDM_SYS_LOGOUT, &CMainFrame::OnSysLogout)
	ON_UPDATE_COMMAND_UI(IDM_SYS_LOGIN, &CMainFrame::OnUpdateSysLogin)
	ON_UPDATE_COMMAND_UI(IDM_SYS_LOGOUT, &CMainFrame::OnUpdateSysLogout)


//	ON_COMMAND_RANGE(IDM_HISTORY_MODEL_START, IDM_HISTORY_MODEL_LAST, CMainFrame::OnMenuHistoryRecent)
	ON_COMMAND_RANGE(IDM_VIEW_START, IDM_VIEW_LAST, CMainFrame::OnMenuViewChange)
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(IDM_HELP, &CMainFrame::OnHelp)
//	ON_COMMAND(IDM_TASK_PUBLISH, &CMainFrame::OnTaskPublish)
//	ON_COMMAND(IDM_BAR_TREE, &CMainFrame::OnBarTree)
	//ON_UPDATE_COMMAND_UI(IDM_BAR_TREE, &CMainFrame::OnUpdateBarTree)
	//ON_COMMAND(IDM_RES, &CMainFrame::OnRes)
	//ON_UPDATE_COMMAND_UI(IDM_RES, &CMainFrame::OnUpdateRes)
	ON_WM_COPYDATA()
	ON_WM_SIZE()
	ON_WM_THEMECHANGED()
	ON_WM_ACTIVATE()
	ON_WM_ACTIVATEAPP()
	ON_WM_CAPTURECHANGED()
	ON_WM_CHANGEUISTATE()
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	//	CBCGPVisualManager::GetInstance ()->m_bNcTextCenter = FALSE;
	m_nAppLook = theApp.GetInt (_T("ApplicationLook"), ID_VIEW_APPLOOK_2007);
	m_nAppLook = ID_VIEW_APPLOOK_2007_2;
	// TODO: add member initialization code here
//	m_pDlgProgress = NULL;
}

CMainFrame::~CMainFrame()
{
	HookWebPageTitle(FALSE, g_lpszAppTitle);
	//UnhookWindowsHookEx(g_hPrevHookXP);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

	if (CBCGPMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	//RECT rectDesktop;
	//SystemParametersInfo(SPI_GETWORKAREA,0,(PVOID)&rectDesktop,0);

	//this->SetWindowPos ( NULL, 0, 0
	//	, rectDesktop.right - rectDesktop.left
	//	, rectDesktop.bottom - rectDesktop.top 
	//	, SWP_NOACTIVATE | SWP_NOZORDER );

	//去掉win7外边框
	//DWORD dwAttr = DWMNCRP_DISABLED;
	//globalData.DwmSetWindowAttribute (this->GetSafeHwnd(), DWMWA_NCRENDERING_POLICY, &dwAttr, sizeof(DWORD) );
	//globalData.SetDPIAware();

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(m_hIcon, TRUE); // Set big icon
	//SetIcon(m_hIcon, FALSE); // Set small icon

	//if (CBCGPToolBar::GetUserImages () == NULL)
	//{
	//	// Load toolbar user images:
	//	if (!m_UserImages.Load (_T(".\\UserImages.bmp")))
	//	{
	//		TRACE(_T("Failed to load user images\n"));
	//	}
	//	else
	//	{
	//		CBCGPToolBar::SetUserImages (&m_UserImages);
	//	}
	//}


	//	CBCGPToolBar::EnableQuickCustomization ();


	// TODO: Define your own basic commands. Be sure, that each pulldown 
	// menu have at least one basic command.

	//CList<UINT, UINT>	lstBasicCommands;

	//lstBasicCommands.AddTail (ID_VIEW_TOOLBARS);
	//lstBasicCommands.AddTail (ID_FILE_NEW);
	//lstBasicCommands.AddTail (ID_FILE_OPEN);
	//lstBasicCommands.AddTail (ID_FILE_SAVE);
	//lstBasicCommands.AddTail (ID_APP_EXIT);
	//lstBasicCommands.AddTail (ID_EDIT_CUT);
	//lstBasicCommands.AddTail (ID_EDIT_PASTE);
	//lstBasicCommands.AddTail (ID_EDIT_UNDO);
	//lstBasicCommands.AddTail (ID_RECORD_NEXT);
	//lstBasicCommands.AddTail (ID_RECORD_LAST);
	//lstBasicCommands.AddTail (ID_APP_ABOUT);
	//lstBasicCommands.AddTail (ID_VIEW_TOOLBAR);
	//lstBasicCommands.AddTail (ID_VIEW_CUSTOMIZE);
	//lstBasicCommands.AddTail (ID_WINDOW_TILE_HORZ);
	//lstBasicCommands.AddTail (ID_VIEW_APPLOOK_2000);
	//lstBasicCommands.AddTail (ID_VIEW_APPLOOK_XP);
	//lstBasicCommands.AddTail (ID_VIEW_APPLOOK_2003);
	//lstBasicCommands.AddTail (ID_VIEW_APPLOOK_2007);
	//lstBasicCommands.AddTail (ID_VIEW_APPLOOK_VS2005);
	//lstBasicCommands.AddTail (ID_VIEW_APPLOOK_WIN_XP);
	//lstBasicCommands.AddTail (ID_VIEW_APPLOOK_2007_1);
	//lstBasicCommands.AddTail (ID_VIEW_APPLOOK_2007_2);
	//lstBasicCommands.AddTail (ID_VIEW_APPLOOK_2007_3);
	//lstBasicCommands.AddTail (ID_VIEW_APPLOOK_VS2008);
	//lstBasicCommands.AddTail (ID_VIEW_APPLOOK_VS2010);
	//lstBasicCommands.AddTail (ID_VIEW_APPLOOK_2010_1);
	//lstBasicCommands.AddTail (ID_VIEW_APPLOOK_2010_2);
	//lstBasicCommands.AddTail (ID_VIEW_APPLOOK_2010_3);
	//lstBasicCommands.AddTail (ID_VIEW_APPLOOK_WIN7);

	//CBCGPToolBar::SetBasicCommands (lstBasicCommands);

	// Menu will not take the focus on activation:
	CBCGPPopupMenu::SetForceMenuFocus (FALSE);

	if (!m_wndMenuBar.Create (this))
	{
		TRACE0("Failed to create menubar\n");
		return -1;      // fail to create
	}

	m_wndMenuBar.SetBarStyle(m_wndMenuBar.GetBarStyle() | CBRS_SIZE_DYNAMIC);
	//m_wndMenuBar.SetShowAllCommands(TRUE);
	//m_wndMenuBar.SetMaximizeMode(TRUE);
	CCxBCGPMenuBar::SetRecentlyUsedMenus(FALSE); //不加这句话，菜单初始时，只显示一个下拉键头，不显示全部菜单项.

	// Detect color depth. 256 color toolbars can be used in the
	// high or true color modes only (bits per pixel is > 8):
	CClientDC dc (this);
	BOOL bIsHighColor = dc.GetDeviceCaps (BITSPIXEL) > 8;

	UINT uiToolbarHotID = bIsHighColor ? IDB_TOOLBAR256 : 0;

	//if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
	//	| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
	//	!m_wndToolBar.LoadToolBar(IDR_MAINFRAME, 0, 0, FALSE, 0, 0, uiToolbarHotID))
	//{
	//	TRACE0("Failed to create toolbar\n");
	//	return -1;      // fail to create
	//}

	//if (!m_wndStatusBar.Create(this) ||
	//	!m_wndStatusBar.SetIndicators(indicators,
	//	  sizeof(indicators)/sizeof(UINT)))
	//{
	//	TRACE0("Failed to create status bar\n");
	//	return -1;      // fail to create
	//}

	
//	LoadRobotbar();
	LoadLoginbar();
	LoadToolbar();

	EnableDocking(CBRS_ALIGN_TOP);

	// TODO: Delete these three lines if you don't want the toolbar to be dockable

	m_wndMenuBar.ShowWindow(SW_HIDE);

	HWND hwndTabBar = m_wndClientArea.GetMDITabs().GetSafeHwnd();
	if ( IsWindow( hwndTabBar ) )
	{
		::ShowWindow( hwndTabBar, SW_HIDE );
	}

//	m_wndMenuBar.EnableDocking(CBRS_ALIGN_TOP);
//	DockControlBar(&m_wndMenuBar);
	//DockControlBarLeftOf( &m_wndMenuBar, &m_wndLoginBar );

	//m_wndRobotBar.EnableDocking(CBRS_ALIGN_TOP);
	//DockControlBar(&m_wndRobotBar);
	//DockControlBarLeftOf( &m_wndRobotBar, &m_wndLoginBar );


	//DockControlBar(&m_wndToolBar);

	//DockControlBarLeftOf( &m_wndRobotBar, &m_wndLoginBar );
	//DockControlBarLeftOf( &m_wndToolBar, &m_wndRobotBar );

	//DockControlBarLeftOf( &m_wndMenuBar, &m_wndLoginBar );
//	DockControlBarLeftOf( &m_wndToolBar, &m_wndRobotBar );

	m_wndLoginBar.EnableDocking(CBRS_ALIGN_TOP);
	DockControlBar(&m_wndLoginBar, AFX_IDW_DOCKBAR_TOP);
	//DockControlBarLeftOf(&m_wndLoginBar, &m_wndRobotBar);
	//DockControlBarLeftOf( &m_wndRobotBar, &m_wndLoginBar );

	m_wndToolBar.EnableDocking(CBRS_ALIGN_TOP);
	DockControlBar(&m_wndToolBar);

	//DockControlBarLeftOf( &m_wndLoginBar, &m_wndToolBar );
	//DockControlBarLeftOf( &m_wndToolBar, &m_wndLoginBar );

	DockControlBarLeftOf( &m_wndLoginBar, &m_wndToolBar );

	CRect rcLogin, rcTool;
	m_wndLoginBar.GetWindowRect(&rcLogin);
	m_wndToolBar.GetWindowRect(&rcTool);
	
	//m_wndToolBar.UpdateWindow();

	//m_wndToolBar.EnableCustomizeButton (TRUE, ID_VIEW_CUSTOMIZE, _T("Customize..."));

	// Allow user-defined toolbars operations:
	InitUserToobars (NULL,
		uiFirstUserToolBarId,
		uiLastUserToolBarId);

	// Enable windows manager:
	EnableWindowsDialog (ID_WINDOW_MANAGER, IDS_WINDOWS_MANAGER, TRUE);

	//////////////////////////////////////////////////////////////////////////
	//注掉这段代码，在菜单栏、工具栏等空白区点击鼠标右键，将不弹出默认的自定义右键菜单
	// Enable control bar context menu (list of bars + customize command):
	//EnableControlBarMenu (	
	//	TRUE,				// Enable
	//	ID_VIEW_CUSTOMIZE, 	// Customize command ID
	//	_T("Customize..."),	// Customize command text
	//	ID_VIEW_TOOLBARS,	// Menu items with this ID will be replaced by toolbars menu
	//	FALSE,
	//	FALSE,
	//	TRUE);

	OnAppLook (m_nAppLook);

	//if (theApp.m_pDlgDragLayer == NULL)
	//{
	//	theApp.m_pDlgDragLayer = new CDlgDragLayer();
	//	theApp.m_pDlgDragLayer->Create(CDlgDragLayer::IDD, this);
	//}

	theApp.LoadDefaultFont();

	//HOOK，改变网页对话框标题
	//g_hPrevHookXP = SetWindowsHookEx(WH_CALLWNDPROC, HookProcXP, 0, GetCurrentThreadId());
	HookWebPageTitle(TRUE, g_lpszAppTitle);

	AdjustClientArea();

	SetWindowText( g_lpszAppTitle );
	SetTitle( g_lpszAppTitle );
	ShowWindow(SW_HIDE);

	//{ //初始化菜单最近打开文件
	//	SetTimer(0, 0, 0);
	//}

	//OnShowRunFlowDialog(0,0);

	// 任务栏上只显示程序名
	ModifyStyle(FWS_ADDTOTITLE, 0);

	// VISUAL_MANAGER
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CBCGPMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CBCGPMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CBCGPMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers




void CMainFrame::OnViewCustomize()
{
	//------------------------------------
	// Create a customize toolbars dialog:
	//------------------------------------
	CBCGPToolbarCustomize* pDlgCust = new CBCGPToolbarCustomize (this,
		TRUE /* Automatic menus scaning */);

	pDlgCust->EnableUserDefinedToolbars ();
	pDlgCust->Create ();
}

afx_msg LRESULT CMainFrame::OnToolbarReset(WPARAM /*wp*/,LPARAM)
{
	// TODO: reset toolbar with id = (UINT) wp to its initial state:
	//
	// UINT uiToolBarId = (UINT) wp;
	// if (uiToolBarId == IDR_MAINFRAME)
	// {
	//		do something with m_wndToolBar
	// }

	return 0;
}

void CMainFrame::OnWindowManager() 
{
	ShowWindowsDialog ();
}

void CMainFrame::OnAppLook(UINT id)
{
	CBCGPDockManager::SetDockMode (BCGP_DT_SMART);

	m_nAppLook = id;

	CBCGPTabbedControlBar::m_StyleTabWnd = CBCGPTabWnd::STYLE_3D;

	CBCGPMDITabParams mdiTabParams;
	mdiTabParams.m_bTabIcons = TRUE;

	//switch (m_nAppLook)
	//{
	//case ID_VIEW_APPLOOK_2000:
	//	// enable Office 2000 look:
	//	CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager));
	//	break;

	//case ID_VIEW_APPLOOK_XP:
	//	// enable Office XP look:
	//	CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerXP));
	//	break;

	//case ID_VIEW_APPLOOK_WIN_XP:
	//	// enable Windows XP look (in other OS Office XP look will be used):
	//	CBCGPWinXPVisualManager::m_b3DTabsXPTheme = TRUE;
	//	CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPWinXPVisualManager));
	//	break;

	//case ID_VIEW_APPLOOK_2003:
	//	// enable Office 2003 look:
	//	CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2003));

	//	mdiTabParams.m_style = CBCGPTabWnd::STYLE_3D_VS2005;
	//	mdiTabParams.m_bAutoColor = TRUE;
	//	CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
	//	break;

	//case ID_VIEW_APPLOOK_2007:
	//case ID_VIEW_APPLOOK_2007_1:
	//case ID_VIEW_APPLOOK_2007_2:
	//case ID_VIEW_APPLOOK_2007_3:
	// enable Office 2007 look:
	//switch (m_nAppLook)
	//{
	//case ID_VIEW_APPLOOK_2007:
	//	CCxBCGPVisualManager2007::SetStyle (CCxBCGPVisualManager2007::VS2007_LunaBlue);
	//	break;

	//case ID_VIEW_APPLOOK_2007_1:
	//	CCxBCGPVisualManager2007::SetStyle (CCxBCGPVisualManager2007::VS2007_ObsidianBlack);
	//	break;

	//case ID_VIEW_APPLOOK_2007_2:
	//	CCxBCGPVisualManager2007::SetStyle (CCxBCGPVisualManager2007::VS2007_Silver);
	//	break;

	//case ID_VIEW_APPLOOK_2007_3:
	//	CCxBCGPVisualManager2007::SetStyle (CCxBCGPVisualManager2007::VS2007_Aqua);
	//	break;
	//}

	//CCxBCGPVisualManager2007::SetDefaultManager (RUNTIME_CLASS (CCxBCGPVisualManager2007));

	mdiTabParams.m_style = CBCGPTabWnd::STYLE_3D_VS2005;
	mdiTabParams.m_bAutoColor = TRUE;
	CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
	//break;

	//case ID_VIEW_APPLOOK_2010_1:
	//case ID_VIEW_APPLOOK_2010_2:
	//case ID_VIEW_APPLOOK_2010_3:
	//	// enable Office 2010 look:
	//	switch (m_nAppLook)
	//	{
	//	case ID_VIEW_APPLOOK_2010_1:
	//		CBCGPVisualManager2010::SetStyle (CBCGPVisualManager2010::VS2010_Blue);
	//		break;

	//	case ID_VIEW_APPLOOK_2010_2:
	//		CBCGPVisualManager2010::SetStyle (CBCGPVisualManager2010::VS2010_Black);
	//		break;

	//	case ID_VIEW_APPLOOK_2010_3:
	//		CBCGPVisualManager2010::SetStyle (CBCGPVisualManager2010::VS2010_Silver);
	//		break;
	//	}
	//	CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2010));

	//	mdiTabParams.m_style = CBCGPTabWnd::STYLE_3D_VS2005;
	//	mdiTabParams.m_bAutoColor = TRUE;
	//	CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
	//	break;

	//case ID_VIEW_APPLOOK_WIN7:
	//	// enable Windows look:
	//	CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerScenic));

	//	mdiTabParams.m_style = CBCGPTabWnd::STYLE_3D_VS2005;
	//	mdiTabParams.m_bAutoColor = TRUE;
	//	CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
	//	break;

	//case ID_VIEW_APPLOOK_VS2005:
	//	// enable VS 2005 look:
	//	CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerVS2005));

	//	mdiTabParams.m_style = CBCGPTabWnd::STYLE_3D_VS2005;
	//	mdiTabParams.m_bDocumentMenu = TRUE;
	//	CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
	//	break;

	//case ID_VIEW_APPLOOK_VS2008:
	//	// enable VS 2008 look:
	//	CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerVS2008));

	//	mdiTabParams.m_style = CBCGPTabWnd::STYLE_3D_VS2005;
	//	mdiTabParams.m_bDocumentMenu = TRUE;
	//	CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
	//	break;

	//case ID_VIEW_APPLOOK_VS2010:
	//	// enable VS 2010 look:
	//	CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerVS2010));

	//	mdiTabParams.m_bDocumentMenu = TRUE;
	//	CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
	//	break;
	//}

	//mdiTabParams.m_style = CBCGPTabWnd::STYLE_3D_SCROLLED; 
	//mdiTabParams.m_bAutoColor = FALSE;
	//mdiTabParams.m_bTabIcons = FALSE;
	//mdiTabParams.m_bSetActiveTabBoldFont = FALSE;
	//mdiTabParams.m_nTabMinFullWidth = 129;
	//mdiTabParams.m_bActiveTabCloseButton = FALSE;
	//mdiTabParams.m_bDocumentMenu = TRUE;
	//mdiTabParams.m_bEnableTabSwap = FALSE;
	//mdiTabParams.m_bEnableDragTab = FALSE;
	//mdiTabParams.m_bTabCloseButton = FALSE;
	//EnableMDITabbedGroups (TRUE, mdiTabParams);

	CBCGPDockManager* pDockManager = GetDockManager ();
	if (pDockManager != NULL)
	{
		ASSERT_VALID (pDockManager);
		pDockManager->AdjustBarFrames ();
	}

	CBCGPTabbedControlBar::ResetTabs ();

	RecalcLayout ();
	RedrawWindow (NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt (_T("ApplicationLook"), m_nAppLook);
}

void CMainFrame::OnUpdateAppLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio (m_nAppLook == pCmdUI->m_nID);
}

// BOOL CMainFrame::OnShowMDITabContextMenu (CPoint point, DWORD dwAllowedItems, BOOL bDrop)
// {
// 	CMenu menu;
// 	VERIFY(menu.LoadMenu (bDrop ? IDR_POPUP_DROP_MDITABS : IDR_POPUP_MDITABS));
// 
// 	CMenu* pPopup = menu.GetSubMenu(0);
// 	ASSERT(pPopup != NULL);
// 
// 	if ((dwAllowedItems & BCGP_MDI_CREATE_HORZ_GROUP) == 0)
// 	{
// 		pPopup->DeleteMenu (ID_MDI_NEW_HORZ_TAB_GROUP, MF_BYCOMMAND);
// 	}
// 
// 	if ((dwAllowedItems & BCGP_MDI_CREATE_VERT_GROUP) == 0)
// 	{
// 		pPopup->DeleteMenu (ID_MDI_NEW_VERT_GROUP, MF_BYCOMMAND);
// 	}
// 
// 	if ((dwAllowedItems & BCGP_MDI_CAN_MOVE_NEXT) == 0)
// 	{
// 		pPopup->DeleteMenu (ID_MDI_MOVE_TO_NEXT_GROUP, MF_BYCOMMAND);
// 	}
// 
// 	if ((dwAllowedItems & BCGP_MDI_CAN_MOVE_PREV) == 0)
// 	{
// 		pPopup->DeleteMenu (ID_MDI_MOVE_TO_PREV_GROUP, MF_BYCOMMAND);
// 	}
// 
// 	CBCGPPopupMenu* pPopupMenu = new CBCGPPopupMenu;
// 	pPopupMenu->SetAutoDestroy (FALSE);
// 	pPopupMenu->Create (this, point.x, point.y, pPopup->GetSafeHmenu ());
// 
// 	return TRUE;
// }

void CMainFrame::OnMdiMoveToNextGroup() 
{
	MDITabMoveToNextGroup ();
}

void CMainFrame::OnMdiMoveToPrevGroup() 
{
	MDITabMoveToNextGroup (FALSE);
}

void CMainFrame::OnMdiNewHorzTabGroup() 
{
	MDITabNewGroup (FALSE);
}

void CMainFrame::OnMdiNewVertGroup() 
{
	MDITabNewGroup ();
}

void CMainFrame::OnMdiCancel() 
{
	// TODO: Add your command handler code here

}

CBCGPMDIChildWnd* CMainFrame::CreateDocumentWindow (LPCTSTR lpcszDocName, CObject* /*pObj*/)
{
	if (lpcszDocName != NULL && lpcszDocName [0] != '\0')
	{
		CDocument* pDoc = AfxGetApp()->OpenDocumentFile (lpcszDocName);

		if (pDoc != NULL)
		{
			POSITION pos = pDoc->GetFirstViewPosition();

			if (pos != NULL)
			{
				CView* pView = pDoc->GetNextView (pos);
				if (pView == NULL)
				{
					return NULL;
				}

				return DYNAMIC_DOWNCAST (CBCGPMDIChildWnd, pView->GetParent ());
			}   
		}
	}

	return NULL;
}

void CMainFrame::OnClose() 
{
	int nRet = IDOK;
	if ( !theApp.m_bCloseByLogin )
	{
		nRet = ::MessageBox( GetMainWnd(), "即将退出协同工作台，是否继续？"
			, g_lpszAppTitle, MB_OKCANCEL|MB_ICONQUESTION );
	}
	theApp.m_bCloseByLogin = FALSE;

	if ( nRet != IDOK ) return;

	//20140403与青田冯维确认，当前版本下建模和协同工作台直接关闭时不注销用户，只有用户点击注销时才注销，lc
	//Logout(theApp.m_LoginData.m_strTicket);
	//在此发送COPYDATA消息给PeraDCWebContainer通知调用js的modifyDefaultTask方法，记住打开的页面
	theWebPageMgr.SendCopyDataLoginOut(m_hWnd);
	theApp.m_LoginData.Clear();

	if (g_pLoadingWindow != NULL)
	{
		g_pLoadingWindow->Release();
		g_pLoadingWindow->DestroyWindow();
		delete g_pLoadingWindow;
		g_pLoadingWindow = NULL;
	}

	SaveMDIState (theApp.GetRegSectionPath ());
	CBCGPMDIFrameWnd::OnClose();
}

// RIBBON_APP

//void CMainFrame::OnMenuModuleOpenjianmo()
//{
//	theViewMgr.NewView( g_lpszViewTitleTaskModel, viewflagTaskModel );
//}

//void CMainFrame::OnMenuModuleActivejianmo()
//{
//	VIEW_DATA * pView = theViewMgr.Find( g_lpszViewTitleTaskModel );
//	theViewMgr.ActiveView(pView);
//}


//void CMainFrame::OnMenuModuleClosejianmo()
//{
//	VIEW_DATA * pView = theViewMgr.Find( g_lpszViewTitleTaskModel );
//	theViewMgr.CloseView(pView);
//}


//void CMainFrame::OnMenuTaskNew()
//{
//	//theApp.m_processMgr.OnNew();
//	return;
//}

//void CMainFrame::OnMenuTaskSaveAs()
//{
//	//theApp.m_processMgr.OnSaveAs();
//	return;
//}

//void CMainFrame::OnMenuTaskOpen()
//{
//	//theApp.m_processMgr.OnOpen();
//	return;
//}

//void CMainFrame::OnMenuTaskRun()
//{
//	//if ( theApp.m_pBarJianmoTree == NULL ) return;
//	//theApp.m_pBarJianmoTree->m_wndGridTree.OnMenuJianmoTreeProcessRun();
//	return;
//}

void CMainFrame::LoadToolbar()
{
	if (
		!m_wndToolBar.CreateEx(this
		, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		/*| CBRS_GRIPPER*/ | CBRS_TOOLTIPS /*| CBRS_FLYBY*/ | CBRS_SIZE_FIXED | CBRS_ORIENT_HORZ
		, CRect(1,0,1,0)
		, IDC_MAIN_TOOLBAR)
		)
	{
		TRACE0("Failed to create toolbar\n");
		return ;      // fail to create
	}
	m_wndToolBar.SetShowTooltips(TRUE);

	m_wndToolBar.LoadData();

	//CString strMainToolbarTitle;
	//strMainToolbarTitle.LoadString (IDS_MAIN_TOOLBAR);
	//m_wndToolBar.SetWindowText (strMainToolbarTitle);
	m_wndToolBar.SetWindowText ("工具栏");
}
//
//void CMainFrame::LoadRobotbar()
//{
//	if (
//		!m_wndRobotBar.CreateEx(this
//		, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
//		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC | CBRS_ORIENT_HORZ
//		, CRect(1,0,1,0)
//		, IDC_MAIN_ROBOTBAR)
//		)
//	{
//		TRACE0("Failed to create toolbar\n");
//		return ;      // fail to create
//	}
//	m_wndRobotBar.SetShowTooltips(TRUE);
//
//	m_wndRobotBar.LoadData();
//
//	m_wndRobotBar.SetWindowText ("组件");
//}

void CMainFrame::LoadLoginbar()
{
	if (
		!m_wndLoginBar.CreateEx(this
		, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| /*CBRS_GRIPPER |*/ CBRS_TOOLTIPS /*| CBRS_FLYBY*/ | CBRS_SIZE_FIXED | CBRS_ORIENT_HORZ
		, IDC_MAIN_LOGINBAR)
		)
	{
		TRACE0("Failed to create toolbar\n");
		return ;      // fail to create
	}
	m_wndLoginBar.SetShowTooltips(TRUE);

	//	m_wndLoginBar.LoadData();

	m_wndLoginBar.SetWindowText ("登录");
}

void CMainFrame::OnToolbarClicked( UINT nID )
{
	nID -= IDM_TOOLBAR_BUTTON_FIRST;

	switch ( nID )
	{
	case CCxBCGPToolBar::IMG_TOOLBAR_LOGOUT:
		MenuLogout(); break;
	case CCxBCGPToolBar::IMG_TOOLBAR_MESSAGE:
		MenuMessage(); break;
	case CCxBCGPToolBar::IMG_TOOLBAR_REFRESH:
		MenuRefresh(); break;
	case CCxBCGPToolBar::IMG_TOOLBAR_FLEXWARE:
		MenuRunFlexware(); break;
	case CCxBCGPToolBar::IMG_TOOLBAR_PORTAL:
		MenuPortal(); break;
	case CCxBCGPToolBar::IMG_TOOLBAR_SETTING:
		MenuSetting(); break;
	case CCxBCGPToolBar::IMG_TOOLBAR_ABOUT:
		MenuAbout(); break;
	default:
		break;
	}
}
//
//
//void CMainFrame::OnRobotbarClicked( UINT nID )
//{
//	nID -= IDM_ROBOTBAR_BUTTON_FIRST;
//
//	//switch ( nID )
//	//{
//	//case CCxStaticRes::RT_NULL + 1:
//	//	break;
//	//default:
//	//	break;
//	//}
//}


void CMainFrame::OnLoginbarClicked( UINT nID )
{
	//nID -= IDM_LOGINBAR_BUTTON_FIRST;

	//switch ( nID )
	//{
	//case CCxLoginBar::IMG_LOGIN:
	//	OnMenuTaskNew();
	//	break;
	//case CCxLoginBar::IMG_LOGOUT:
	//	OnMenuTaskOpen();
	//	break;
	//default:
	//	break;
	//}
}



void CMainFrame::SaveXpdl( void )
{
	//CDlgSvg * pSvg = GetSvgDilogPtr();
	//if ( pSvg == NULL ) return;

	//CWebMethodData wmd;
	//wmd.m_strMethodName = "saveXpdl";
	//pSvg->InvokeJsMethod( &wmd );

	//CXmlHistoryModel xhm;
	//xhm.Load();

	//CHistoryModelData * pHMD = new CHistoryModelData();
	//pHMD->m_strName = theApp.m_strXpdlName;
	//pHMD->m_strPath = theApp.m_strXpdlWorkPath;
	//xhm.Insert( pHMD );

	//xhm.Save();
}

//void CMainFrame::ShowHistoryMonitor( void )
//{
//	VIEW_DATA * pView = theViewMgr.Find( g_lpszViewTitleHistoryMonitor );
//	if ( pView != NULL )
//	{
//		theViewMgr.ActiveView(pView);
//		return;
//	}
//
//	if ( theApp.m_LoginData.m_strTicket.IsEmpty() )
//	{
//		CDlgLoginBk dlgLogin;
//		dlgLogin.DoModal();
//		if ( theApp.m_LoginData.m_strTicket.IsEmpty() )
//		{
//			return;
//		}
//	}
//	theViewMgr.NewView( g_lpszViewTitleHistoryMonitor, viewflagHistoryMonitor );
//
//	CDlgHistoryMonitor * pMonitor = GetHistoryMonitorDilogPtr();
//	if ( pMonitor == NULL ) return;
//
//	pMonitor->LoadUrl();
//}
//
//void CMainFrame::ShowDistribution( void )
//{
//	VIEW_DATA * pView = theViewMgr.Find( g_lpszViewTitleDistribution);
//	if ( pView != NULL )
//	{
//		theViewMgr.ActiveView(pView);
//		return;
//	}
//
//	if ( theApp.m_LoginData.m_strTicket.IsEmpty() )
//	{
//		CDlgLoginBk dlgLogin;
//		dlgLogin.DoModal();
//		if ( theApp.m_LoginData.m_strTicket.IsEmpty() )
//		{
//			return;
//		}
//	}
//	theViewMgr.NewView( g_lpszViewTitleDistribution, viewflagDistribution );
//
//	CDlgDistribution * pDistribution = GetDistributionDilogPtr();
//	if ( pDistribution == NULL ) return;
//
//	pDistribution->LoadUrl();
//}
//
//void CMainFrame::ShowDataMapping( void )
//{
//	CDlgSvg * pSvg = GetSvgDilogPtr();
//	if ( pSvg == NULL ) return;
//
//	CWebMethodData wmd;
//	wmd.m_strMethodName = "showDataMapping";
//	pSvg->InvokeJsMethod( &wmd );
//
//	wmd.Clear();
//	wmd.m_strMethodName = "getDataMapping";
//	pSvg->InvokeJsMethod( &wmd );
//	if (wmd.m_strResult.IsEmpty()) return;
//
//	//if (!UpdateStructTree_UpdateDataMapping( wmd.m_strResult )) return;
//	//theApp.m_pBarJianmoTree->m_wndGridTree.UpdateDataMapping();
//
//	//theApp.m_pBarJianmoTree->m_wndGridTree.m_bModified = TRUE;
//}

LRESULT CMainFrame::OnStructTreeShowDataMapping(WPARAM, LPARAM)
{
//	ShowDataMapping();
	return 0;
}


//void CMainFrame::UpdateRobotbar( list< CResLibData* > BaseComponentList )
//{
//	m_wndRobotBar.UpdateResData(BaseComponentList);
//}


BOOL CMainFrame::GetToolbarButtonToolTipText (CBCGPToolbarButton* pButton, CString& strTTText)
{
	return FALSE;
}



//void CMainFrame::OnMenuHistoryMonitor()
//{
////	ShowHistoryMonitor();
//}


void CMainFrame::OnViewToolbarz()
{
	// TODO: 在此添加命令处理程序代码
	BOOL b = m_wndToolBar.IsVisible();
	if(b)
		m_wndToolBar.ShowControlBar(FALSE, FALSE, FALSE);
	else
		m_wndToolBar.ShowControlBar(TRUE, FALSE, TRUE);
}

void CMainFrame::OnViewComponent()
{
	// TODO: 在此添加命令处理程序代码
	//BOOL b = m_wndRobotBar.IsVisible();
	//if(b)
	//	m_wndRobotBar.ShowControlBar(FALSE, FALSE, FALSE);
	//else
	//	m_wndRobotBar.ShowControlBar(TRUE, FALSE, TRUE);
}

void CMainFrame::OnViewLogin()
{
	// TODO: 在此添加命令处理程序代码
	BOOL b = m_wndLoginBar.IsVisible();
	if(b)
		m_wndLoginBar.ShowControlBar(FALSE, FALSE, FALSE);
	else
		m_wndLoginBar.ShowControlBar(TRUE, FALSE, TRUE);
}


void CMainFrame::OnUpdateViewComponent(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	//BOOL b = m_wndRobotBar.IsVisible();
	//if(b)
	//	pCmdUI->SetCheck(TRUE);  
	//else
	//	pCmdUI->SetCheck(FALSE);  
}


void CMainFrame::OnUpdateViewLogin(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	BOOL b = m_wndLoginBar.IsVisible();
	if(b)
		pCmdUI->SetCheck(TRUE); 
	else
		pCmdUI->SetCheck(FALSE);  
}


void CMainFrame::OnUpdateViewToolbarz(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	BOOL b = m_wndToolBar.IsVisible();
	if(b)
		pCmdUI->SetCheck(TRUE); 
	else
		pCmdUI->SetCheck(FALSE);  
}

//
//void CMainFrame::OnTimer(UINT_PTR nIDEvent)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//
//	switch(nIDEvent)
//	{
//	case 0: //最近打开文件更新
//		{
//			//m_wndLoginBar.MoveToRight();
//
//			m_xhm.Clear();
//			m_xhm.Load();
//
//			CMenu* pMenu = CMenu::FromHandle(m_wndMenuBar.GetDefaultMenu())->GetSubMenu(0)->GetSubMenu(7);
//			if(!pMenu)
//			{
//				KillTimer(nIDEvent);
//				return;
//			}
//			//pMenu->GetSubMenu(0)->DeleteMenu(0, MF_BYPOSITION);
//			// 		pMenu->GetSubMenu(0)->DeleteMenu(0, MF_BYPOSITION);
//			// 		pMenu->GetSubMenu(0)->DeleteMenu(0, MF_BYPOSITION);
//			// 		pMenu->GetSubMenu(0)->DeleteMenu(0, MF_BYPOSITION);
//			//pMenu->GetSubMenu(0)->GetSubMenu(0)->AppendMenu(0, 1, "fgsdgd");
//			//pMenu->GetSubMenu(0)->AppendMenuA(MF_POPUP, 0, "test");
//			//pMenu->DeleteMenu(0, MF_BYPOSITION);
//			int n = pMenu->GetMenuItemCount();
//			for(int i = 0; i < n; i++)
//			{
//				pMenu->DeleteMenu(0, MF_BYPOSITION);
//			}
//			//pMenu->AppendMenu(0, 1, "test");
//
//			for ( int i=0; i<(int)m_xhm.m_vDatas.size(); i++ )
//			{
//				CString& str = m_xhm.m_vDatas[i]->m_strPath;
//				pMenu->AppendMenu(0, IDM_HISTORY_MODEL_START + i, str);
//			}
//
//			if(m_xhm.m_vDatas.empty())
//			{
//				pMenu->AppendMenu(0, 1, "空");
//			}
//
//			m_wndMenuBar.CreateFromMenu(m_wndMenuBar.GetDefaultMenu(), TRUE, TRUE);
//			KillTimer(0);
//			return;
//		}
//	case 1: //窗口切换更新
//		{
//			CMenu* pMenu = CMenu::FromHandle(m_wndMenuBar.GetDefaultMenu())->GetSubMenu(1)->GetSubMenu(7);
//			if(!pMenu)
//			{
//				KillTimer(nIDEvent);
//				return;
//			}
//
//			int n = pMenu->GetMenuItemCount();
//			for(int i = 0; i < n; i++)
//			{
//				pMenu->DeleteMenu(0, MF_BYPOSITION);
//			}
//
//			for ( int i=0; i<(int)theViewMgr.m_vViews.size(); i++ )
//			{
//				CString& str = theViewMgr.m_vViews[i]->m_strTitle;
//				pMenu->AppendMenu(0, IDM_VIEW_START + i, str);
//			}
//
//			if(theViewMgr.m_vViews.empty())
//			{
//				pMenu->AppendMenu(0, 1, "空");
//			}
//
//			m_wndMenuBar.CreateFromMenu(m_wndMenuBar.GetDefaultMenu(), TRUE, TRUE);
//			KillTimer(nIDEvent);
//			return;
//		}
//	}
//
//
//	// 	static bool bHide = false;
//	// 	if(bHide == false)
//	// 	{
//	// 		CMenu* pMenu = CMenu::FromHandle(m_wndMenuBar.GetDefaultMenu());
//	// 		pMenu->GetSubMenu(1)->DeleteMenu(0, MF_BYPOSITION);
//	// 		pMenu->GetSubMenu(1)->DeleteMenu(0, MF_BYPOSITION);
//	// 		pMenu->GetSubMenu(1)->DeleteMenu(0, MF_BYPOSITION);
//	// 		pMenu->GetSubMenu(1)->DeleteMenu(0, MF_BYPOSITION);
//	// 		m_wndMenuBar.CreateFromMenu(pMenu->GetSafeHmenu(), TRUE, TRUE);
//	// 		bHide = true;
//	// 		KillTimer(0);
//	// 	}
//	CBCGPMDIFrameWnd::OnTimer(nIDEvent);
//}


BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_NCLBUTTONDOWN:
	case WM_NCRBUTTONDOWN:
	case WM_NCMBUTTONDOWN:
	case WM_NCLBUTTONUP:
	case WM_NCRBUTTONUP:
	case WM_NCMBUTTONUP:
	case WM_MOUSEMOVE:
		if (m_pToolTip->GetSafeHwnd () != NULL)
		{
			m_pToolTip->RelayEvent(pMsg);
		}
		break;
	}

	return __super::PreTranslateMessage(pMsg);
}







void CMainFrame::OnHelpAbout()
{
	// TODO: 在此添加命令处理程序代码
	theApp.OnAppAbout();
}


void CMainFrame::OnSysLogin()
{
	// TODO: 在此添加命令处理程序代码
	if ( theApp.m_LoginData.m_strTicket.IsEmpty() )
	{
		CDlgLoginBk dlgLogin;
		dlgLogin.DoModal();
		if ( theApp.m_LoginData.m_strTicket.IsEmpty() )
		{
			return;
		}
	}
	m_wndLoginBar.RedrawWindow();
}


void CMainFrame::OnSysLogout()
{
	// TODO: 在此添加命令处理程序代码
	//登出
	//pBar->m_LoginData.m_strTicket = "";
	vector<CView*> vViews;
	for (int i=0; i<(int)theViewMgr.m_vViews.size(); i++)
	{
		if ( theViewMgr.m_vViews[i]->m_strTitle.CompareNoCase((LPCTSTR)g_lpszViewTitleTaskModel) != 0 )
		{
			vViews.push_back(theViewMgr.m_vViews[i]->m_pView);
		}
	}
	if (vViews.size() == 0)
	{
//		Logout(theApp.m_LoginData.m_strTicket);
		theApp.m_LoginData.Clear();
	}
	else
	{
		int nRet = ::MessageBox(theApp.m_pMainWnd->GetSafeHwnd(), "注销当前用户会关闭除建模环境以外的页面，是否继续？", g_lpszAppTitle, MB_OKCANCEL|MB_ICONQUESTION);
		switch (nRet)
		{
		case IDOK:
			{
				for (int i = 0; i < (int)vViews.size(); i++)
				{
					vViews[i]->SendMessage(WM_COMMAND, ID_FILE_CLOSE);
				}
//				Logout(theApp.m_LoginData.m_strTicket);
				theApp.m_LoginData.Clear();
			}
			break;
		case IDCANCEL:
		default:
			break;
		}
	}
	m_wndLoginBar.RedrawWindow();
}


void CMainFrame::OnUpdateSysLogin(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if ( !theApp.m_LoginData.m_strTicket.IsEmpty() )
	{
		pCmdUI->Enable(FALSE);
	}
}


void CMainFrame::OnUpdateSysLogout(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if ( theApp.m_LoginData.m_strTicket.IsEmpty() )
	{
		pCmdUI->Enable(FALSE);
	}
}




void CMainFrame::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	// 	pPopupMenu->GetMenuBar()->CommandToIndex(ID_EDIT_PASTE)
	// 	if(nIndex)
	// 	{
	// 		pPopupMenu->AppendMenuA(0, 1, "test");
	// 	}
	// 	__super::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	// TODO: 在此处添加消息处理程序代码
}


void CMainFrame::OnMenuViewChange(UINT nID)
{
	VIEW_DATA* pView = theViewMgr.m_vViews[nID - IDM_VIEW_START];
	if(!pView)
	{
		return;
	}
	theViewMgr.ActiveView(pView);
}

void CMainFrame::OnHelp()
{
	// TODO: 在此添加命令处理程序代码
	OnHelpAbout();
}


BOOL CMainFrame::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	if (pCopyDataStruct == NULL)
	{
		return __super::OnCopyData(pWnd, pCopyDataStruct);
	}

	switch ( pCopyDataStruct->dwData )
	{
	case CPMSG_WEBPAGE:
		//OnWebPageEvent( pCopyDataStruct->lpData );
		theWebPageMgr.OnWebPageEvent( pCopyDataStruct->lpData );
		break;

	case CPMSG_RUN_PERADESKCLIENT_BY_PERADC:
		OnRunPeraDeskClientByPeraDC( pCopyDataStruct->lpData );
		break;

	default:
		break;
	}

	return __super::OnCopyData(pWnd, pCopyDataStruct);
}


void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (m_wndLoginBar.m_hWnd && m_wndToolBar.m_hWnd)
	{
		// 调整工具条位置,强制使其重新布局
		DockControlBarLeftOf(&m_wndToolBar,&m_wndLoginBar);
		DockControlBarLeftOf(&m_wndLoginBar,&m_wndToolBar);
	}
	m_wndToolBar.MoveToRight();

}


LRESULT CMainFrame::OnLoginbarClicked( WPARAM wParam, LPARAM lParam )
{
	m_wndLoginBar.OnClicked();
	return 0;
}

LRESULT CMainFrame::OnAttachWebPage( WPARAM wParam, LPARAM lParam )
{
	CCxWebPageEvent * pEvent = (CCxWebPageEvent *)wParam;
	if ( pEvent == NULL ) return 0;

	CString sTabCaption;
	switch ( pEvent->GetModuleFlag() )
	{
	case CPeraWebPageMgrHost::FW_TABVIEW_TASK_MODEL:
		sTabCaption = viewflagTaskModel;
		break;

	default:
		sTabCaption = "Unknown";
		break;
	}

	
	//sTabTitle.Format("运行实例:%s", theApp.m_processMgr.m_sFlowId);

	//if ( !IsWindow( pEvent->m_hWndWebPage ) )
	//{
	//	::MessageBox( GetMainWnd(),"WebPage主窗口不存在", g_lpszAppTitle, MB_OK);
	//	return 0;
	//}

	//CCxWPENavigate* pEventNavigate = dynamic_cast<CCxWPENavigate*>(pEvent->GetExData());
	//if (pEventNavigate)
	//{
	//	//http://192.168.50.165/ws/planInfo/showCliceRunRobotPage.do?xpdlId=56823&processOid=56825&ticket-proxy=URsbcthJeiUHKsJnycRY
	//	//processOid=56825
	//	CString sUrl = pEventNavigate->m_strUrl;
	//	CString sFlowId = sUrl.Right(sUrl.GetLength() - sUrl.Find("processOid=") - 11);
	//	if (sFlowId.Find("&") != -1)
	//	{
	//		sFlowId = sFlowId.Left(sFlowId.Find("&"));
	//	}
	//	if (!sFlowId.IsEmpty())
	//	{
	//		sTabTitle.Format("运行实例:%s", sFlowId);
	//	}
	//}

	theViewMgr.NewView( sTabCaption, pEvent->m_hWndWebPage );

	//m_hwndWebPage = pEvent->m_hWndWebPage;
	//if ( !IsWindow( m_hwndWebPage ) )
	//{
	//	m_hwndWebPage = NULL; return 0;
	//}
	ZTools::WriteZToolsFormatLog( "CMainFrame::OnAttachWebPage \r\n %s", "AttachWindow end" );

	//m_dwExStyleWebPage = ::GetWindowLong(m_hwndWebPage, GWL_EXSTYLE);
	//m_dwStyleWebPage = ::GetWindowLong(m_hwndWebPage, GWL_STYLE);

	//AttachWindow( this->GetSafeHwnd(), m_hwndWebPage, FALSE );
	//ZTools::WriteZToolsFormatLog( "CDlgViewRunFlow::OnAttachWebPage \r\n %s", "AttachWindow end" );

	//CRect rcClient;
	//GetClientRect(&rcClient);
	////rcClient.bottom -= 38;

	//::MoveWindow(m_hwndWebPage, 0, 0
	//	, rcClient.Width(), rcClient.Height(), TRUE);

	//::ShowWindow( m_hwndWebPage, SW_SHOW );

	::PostMessage( pEvent->m_hWndWebPage, WM_WEBPAGE_PROCESS_ATTACHED, 0, 0 );
	ZTools::WriteZToolsFormatLog( "CMainFrame::OnAttachWebPage \r\n %s", "PostMessage WM_WEBPAGE_PROCESS_ATTACHED end" );

	return 0;
}


LRESULT CMainFrame::OnThemeChanged()
{
	// 该功能要求使用 Windows XP 或更高版本。
	// 符号 _WIN32_WINNT 必须 >= 0x0501。
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	theApp.LoadDefaultFont();

	//有时候结构树标题栏下边有条边线不绘制问题
	//if ( theApp.m_pBarJianmoTree != NULL 
	//	&& IsWindow( theApp.m_pBarJianmoTree->GetSafeHwnd() ) )
	//{
	//	::PostMessage( theApp.m_pBarJianmoTree->GetSafeHwnd(), WM_NCPAINT, 0, 0 );
	//}

	return 1;
}


void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	__super::OnActivate(nState, pWndOther, bMinimized);

	//有时候结构树标题栏下边有条边线不绘制问题
	//if ( theApp.m_pBarJianmoTree != NULL 
	//	&& IsWindow( theApp.m_pBarJianmoTree->GetSafeHwnd() ) )
	//{
	//	::PostMessage( theApp.m_pBarJianmoTree->GetSafeHwnd(), WM_NCPAINT, 0, 0 );
	//}

	//if ( 
	//	nState == 0 
	//	&& theApp.m_pBarJianmoTree != NULL 
	//	&& IsWindow( theApp.m_pBarJianmoTree->GetSafeHwnd() )
	//	)
	//{
	//	theApp.m_pBarJianmoTree->m_wndGridTree.m_ToolTip2.Deactivate ();
	//}

}


void CMainFrame::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
	__super::OnActivateApp(bActive, dwThreadID);

	//有时候结构树标题栏下边有条边线不绘制问题
	//if ( theApp.m_pBarJianmoTree != NULL 
	//	&& IsWindow( theApp.m_pBarJianmoTree->GetSafeHwnd() ) )
	//{
	//	::PostMessage( theApp.m_pBarJianmoTree->GetSafeHwnd(), WM_NCPAINT, 0, 0 );
	//}
}


void CMainFrame::OnCaptureChanged(CWnd *pWnd)
{
	//有时候结构树标题栏下边有条边线不绘制问题
	//if ( theApp.m_pBarJianmoTree != NULL 
	//	&& IsWindow( theApp.m_pBarJianmoTree->GetSafeHwnd() ) )
	//{
	//	::PostMessage( theApp.m_pBarJianmoTree->GetSafeHwnd(), WM_NCPAINT, 0, 0 );
	//}

	__super::OnCaptureChanged(pWnd);
}


void CMainFrame::OnChangeUIState(UINT nAction, UINT nUIElement)
{
	// 该功能要求使用 Windows 2000 或更高版本。
	// 符号 _WIN32_WINNT 和 WINVER 必须 >= 0x0500。
	__super::OnChangeUIState(nAction, nUIElement);

	//有时候结构树标题栏下边有条边线不绘制问题
	//if ( theApp.m_pBarJianmoTree != NULL 
	//	&& IsWindow( theApp.m_pBarJianmoTree->GetSafeHwnd() ) )
	//{
	//	::PostMessage( theApp.m_pBarJianmoTree->GetSafeHwnd(), WM_NCPAINT, 0, 0 );
	//}
}


void CMainFrame::MenuMessage()
{
	if (theApp.m_pMsgDlg==NULL)
	{
		CMsgDlg *pdlg = new CMsgDlg;
		pdlg->Create(CMsgDlg::IDD,CWnd::GetDesktopWindow());
		pdlg->ShowWindow(SW_MAXIMIZE);
		theApp.m_pMsgDlg = pdlg;
	}
	else
	{
		theApp.m_pMsgDlg->RefreshMsg();
		theApp.m_pMsgDlg->ShowWindow(SW_MAXIMIZE);
		theApp.m_pMsgDlg->SetForegroundWindow();
	}

	
	return;

	TCHAR szDir[MAX_PATH] = {NULL};
	if ( !SHGetSpecialFolderPath( this->GetSafeHwnd()
		, szDir
		, CSIDL_PROGRAM_FILES
		, FALSE ) )
	{
		return;
	}

	CString strDir = szDir == NULL ? "" : szDir;
	if ( !IsAccessibleDir( szDir ) )
	{
		return;
	}

	strDir += "\\Internet Explorer";
	strDir += "\\iexplore.exe";
	if ( !IsAccessibleFile( strDir ) )
		return;

	CString strUrl;
	strUrl = GetConfig("PeraDeskClient", "MsgUrl");
	if (strUrl.IsEmpty()|| theApp.m_LoginData.m_strTicket.IsEmpty())
	{
		strUrl = "about:blank";
	}
	else
	{
		AttachTicket(strUrl, theApp.m_LoginData.m_strTicket);
	}
	ShellExecute(NULL, "open", strDir, strUrl, NULL, SW_MAXIMIZE);
}

void CMainFrame::MenuPortal()
{
	if ( globalData.bIsWindows7 )
	{

	}

	TCHAR szDir[MAX_PATH] = {NULL};
	if ( !SHGetSpecialFolderPath( this->GetSafeHwnd()
		, szDir
		, CSIDL_PROGRAM_FILES
		, FALSE ) )
	{
		return;
	}

	CString strDir = szDir == NULL ? "" : szDir;
	if ( !IsAccessibleDir( szDir ) )
	{
		return;
	}

	strDir += "\\Internet Explorer";
	strDir += "\\iexplore.exe";
	if ( !IsAccessibleFile( strDir ) )
	{
		return;
	}

	CString strUrl;
	strUrl = GetConfig("PeraDeskClient", "Portal");
	if (
		strUrl.IsEmpty()
		|| theApp.m_LoginData.m_strTicket.IsEmpty()
		)
	{
		strUrl = "about:blank";
	}
	else
	{
		AttachTicket(strUrl, theApp.m_LoginData.m_strTicket);
	}

	ShellExecute(NULL, "open", strDir, strUrl, NULL, SW_MAXIMIZE);
}
void CMainFrame::MenuRefresh()
{
	//在此发送COPYDATA消息给PeraDCWebContainer通知调用js的refreshWindow方法刷新页面
	theWebPageMgr.SendCopyDataRefresh(m_hWnd);
}
#define SM_WORKSPACE_LOGIN_INFO_LENGTH 300
void CMainFrame::MenuRunFlexware()
{
	CString strPath = GetPeraGlobalDataFromReg( "lm", "PeraProcessDesigner" );
	if ( !IsAccessibleDir( strPath ) )
	{
		return;
	}
	strPath += "\\PeraProcessDesigner.exe";
	if ( !IsAccessibleFile( strPath ) )
	{
		return;
	}
	
	CString strTik = "ticket-proxy=";
	CString strTik2 = theApp.m_LoginData.m_strTicket;
	if ( theApp.m_LoginData.m_strTicket.GetLength() > strTik.GetLength() )
	{
		strTik2 = theApp.m_LoginData.m_strTicket.Left( strTik.GetLength() );
		if ( strTik2.CompareNoCase( strTik ) == 0 )
		{
			strTik2 = theApp.m_LoginData.m_strTicket.Right(
				 theApp.m_LoginData.m_strTicket.GetLength() - strTik.GetLength() );
		}
	}

	CSharedMemory smLoginInfo("SM_WORKSPACE_LOGIN_INFO",SM_WORKSPACE_LOGIN_INFO_LENGTH);
	if (smLoginInfo.AlreadyExist())
	{
		CString strLoginInfo;
		smLoginInfo.Lock();
		strLoginInfo = (char*)smLoginInfo.GetData();
		smLoginInfo.GetData();
		smLoginInfo.Unlock();

		CCxLoginInfo logInfoData;
		logInfoData.Parse(strLoginInfo);
		if (logInfoData.m_strUser!=theApp.m_LoginData.m_strUser)
		{
			// 建模环境已经打开，且登录用户不同或者未登录，给提示，不激活窗口
			MessageBox("其他用户已经打开了过程建模,请关闭后重试！",g_lpszAppTitle,MB_OK|MB_ICONINFORMATION);
			return;
		}
		else
		{
			ZTools::WriteZToolsFormatLog("用户【%s】已经登录，激活窗口...",logInfoData.m_strUser);
		}
	}

	CString sParam;
	sParam.Format( "-json:{ \
				   \"userName\":\"%s\", \
				   \"realName\":\"%s\", \
				   \"ticket-proxy\":\"%s\" \
				   }",
				   theApp.m_LoginData.m_strUser, 
				   theApp.m_LoginData.m_strRealName, 
				   strTik2 );

	ShellExecute(NULL, "open", strPath, (LPCTSTR)sParam, NULL, SW_SHOW);
}

void CMainFrame::MenuLogin()
{

}

void CMainFrame::MenuLogout()
{
	//登出

	int nRet = ::MessageBox(theApp.m_pMainWnd->GetSafeHwnd(), "即将注销协同工作台，是否继续？"
		, g_lpszAppTitle, MB_OKCANCEL|MB_ICONQUESTION );
	if ( nRet != IDOK )
		return;

	//pBar->m_LoginData.m_strTicket = "";
	vector<CView*> vViews;
	for (int i=0; i<(int)theViewMgr.m_vViews.size(); i++)
	{
		if ( theViewMgr.m_vViews[i]->m_strTitle.CompareNoCase((LPCTSTR)g_lpszViewTitleTaskModel) != 0 )
		{
			vViews.push_back(theViewMgr.m_vViews[i]->m_pView);
		}
	}
	if (vViews.size() == 0)
	{
		//			Logout(theApp.m_LoginData.m_strTicket);
		//theApp.m_LoginData.Clear();
	}
	else
	{
		CString sMsg;
		sMsg.Format("注销当前用户会关闭除 [%s] 以外的页面，是否继续？", g_lpszViewTitleTaskModel);
		int nRet = ::MessageBox( GetMainWnd(), sMsg, g_lpszAppTitle, MB_OKCANCEL|MB_ICONQUESTION);
		switch (nRet)
		{
		case IDOK:
			{
				for (int i = 0; i < (int)vViews.size(); i++)
				{
					vViews[i]->SendMessage(WM_COMMAND, ID_FILE_CLOSE);
				}
				//					Logout(theApp.m_LoginData.m_strTicket);
				//theApp.m_LoginData.Clear();
			}
			break;
		case IDCANCEL:
		default:
			break;
		}
	}

	//在此发送COPYDATA消息给PeraDCWebContainer通知调用js的modifyDefaultTask方法，记住打开的页面
	theWebPageMgr.SendCopyDataLoginOut(theApp.m_pMainWnd->m_hWnd);

	if ( theApp.m_pMainWnd != NULL )
	{
		// 注销时切换到空白页面，避免再次登录后刷新页面出现从非空到非空页面的现象
		theWebPageMgr.Navigate(CPeraWebPageMgrHost::FW_TABVIEW_TASK_MODEL,"about:blank");
		theApp.m_pMainWnd->ShowWindow( SW_HIDE );
	}

	WriteRegForDeskClient( _T("LastUser"), theApp.m_LoginData.m_strUser ); // 记住最后的用户
	Logout( theApp.m_LoginData.m_strTicket );
	theApp.m_LoginData.Clear();
	theApp.m_cxPeraDCData.m_sUrl = GetConfig("PeraDeskClient", "TaskModel");
	if (theApp.m_pMsgDlg)
	{
		/*delete theApp.m_pMsgDlg;
		theApp.m_pMsgDlg = NULL;*/

		theApp.m_pMsgDlg->OpenUrl("about:blank");
		theApp.m_pMsgDlg->ShowWindow(SW_HIDE);
	}


	CDlgLoginBk dlgLogin;
	dlgLogin.DoModal();
	if ( theApp.m_LoginData.m_strTicket.IsEmpty() )
	{
		theApp.m_bCloseByLogin = TRUE;
		::PostMessage( theApp.m_pMainWnd->GetSafeHwnd(), WM_CLOSE, 0, 0 );
		return;
	}
	else
	{
		CString strUrl;
		strUrl = theApp.m_cxPeraDCData.m_sUrl;
		AttachTicket(strUrl, theApp.m_LoginData.m_strTicket);
		theWebPageMgr.Navigate( CPeraWebPageMgrHost::FW_TABVIEW_TASK_MODEL, strUrl );

		theApp.m_pMainWnd->ShowWindow( SW_SHOW );
		HWND hForeWnd = ::GetForegroundWindow();
		DWORD dwForeID = ::GetWindowThreadProcessId(hForeWnd,NULL);
		DWORD dwCurID = ::GetCurrentThreadId();
		::AttachThreadInput(dwCurID,dwForeID,TRUE);
		theApp.m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
		theApp.m_pMainWnd->SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
		theApp.m_pMainWnd->SetWindowPos(&CWnd::wndNoTopMost,0,0,0,0, SWP_NOSIZE|SWP_NOMOVE);
		theApp.m_pMainWnd->SetForegroundWindow();
		::AttachThreadInput(dwCurID,dwForeID,FALSE);
	}
}

void CMainFrame::MenuAbout()
{
	theApp.OnAppAbout();
}

void CMainFrame::MenuSetting()
{
	CDlgSystemSetting dlg;
	dlg.DoModal();
}

void CMainFrame::OnRunPeraDeskClientByPeraDC( LPVOID lpData )
{
	CString sCommandline = lpData == NULL ? "" : (LPCTSTR)lpData;
	if ( sCommandline.IsEmpty() ) return;

	CCxPeraDCData data;
	if ( !data.Parse( sCommandline ) ) return;
	theApp.m_LoginData.m_strRealName = data.m_sRealName;
	/*if (theApp.m_LoginData.m_strUser.Compare(data.m_sUser)!=0)*/
	if (theApp.m_pMsgDlg)
	{
		//delete theApp.m_pMsgDlg;// 从外部启动的时候，关闭消息页面
		//theApp.m_pMsgDlg = NULL;
		theApp.m_pMsgDlg->OpenUrl("about:blank");
		theApp.m_pMsgDlg->ShowWindow(SW_HIDE);
	}

	theApp.m_LoginData.m_strUser = data.m_sUser;
	theApp.m_LoginData.m_strTicket = data.m_sTicket;
	CString sUrl = data.m_sUrl;
	AttachTicket(sUrl, theApp.m_LoginData.m_strTicket);
	theWebPageMgr.Navigate( CPeraWebPageMgrHost::FW_TABVIEW_TASK_MODEL, sUrl );

	::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
	Sleep(10);
	::SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
	m_wndLoginBar.RedrawWindow();
}

//void CMainFrame::DockControlBarLeftOf(CToolBar* pBar, CToolBar* pLeftOf)
//{
//	//设置工具条并列停靠在同一条边上
//	CRect rect; //矩形区域定义
//	DWORD dw;
//	UINT n=0;
//	RecalcLayout();//重新显示
//	pLeftOf->GetWindowRect(&rect);
//	rect.OffsetRect(1,0);//设置偏移值以停靠在同一边上
//	dw=pLeftOf->GetBarStyle();
//	n=(dw&CBRS_ALIGN_TOP)?AFX_IDW_DOCKBAR_TOP:n;
//	n=(dw&CBRS_ALIGN_BOTTOM&&n==0)?AFX_IDW_DOCKBAR_BOTTOM:n;
//	n=(dw&CBRS_ALIGN_LEFT&&n==0)?AFX_IDW_DOCKBAR_LEFT:n;
//	n=(dw&CBRS_ALIGN_RIGHT&&n==0)?AFX_IDW_DOCKBAR_RIGHT:n;
//	DockControlBar(pBar,n,&rect);
//}

void CMainFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	lpMMI->ptMinTrackSize.x = 980;
	lpMMI->ptMinTrackSize.y = 680;
	__super::OnGetMinMaxInfo(lpMMI);
}
LRESULT CMainFrame::OnShowProgress( WPARAM wParam, LPARAM )
{
	if (wParam == TRUE)
	{
		ZTools::WriteZToolsLog( "显示滚动进度\n" );
		if ( g_pLoadingWindow != NULL ) g_pLoadingWindow->Show();
	}
	else
	{
		ZTools::WriteZToolsLog( "隐藏滚动进度\n" );
		if ( g_pLoadingWindow != NULL ) g_pLoadingWindow->Hide();
	}
	return 0;
}