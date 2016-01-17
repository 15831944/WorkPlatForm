// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"

#include "MainFrm.h"
//#include <DlgProcessNew.h>
//#include <DlgProcessOpen.h>
#include "CxBCGPVisualManager2007.h"
#include "PtrVectorHeader.h"
#include "CxBCGPToolbarButton.h"
#include "DlgSvg.h"
#include "XmlHistoryModel.h"
#include "ResLibImageSet.h"
#include "PeraProcessDesignerUtility.h"
#include "DlgStructTreePropComponent.h"
#include "DlgStructTreePropProcess.h"
#include "DlgStructTreePropFlow.h"
//#include <dwmapi.h>
#include "DlgIf.h"
#include <ResLibData.h>
#include "DlgLoginBk.h"
#include "HookFunc.h"
#include "DlgExpEditorDM.h"
#include "DlgRunFlow.h"
#include "GetFlexwareBin.h"
#include <shellapi.h>
#include "Tools.h"
#include <DlgDistribution.h>
#include <InvokeWsFunc.h>
#include "DlgViewRunFlow.h"
#include "UtilityFunc.h"
#include "DlgParamSet.h"
#include "CxStructTreeNodeFlow.h"
#include "CxArrayND.h"
#include "CxStructTreeNodeParam.h"
#include "JsData_Svg.h"
#include "DlgFor.h"
#include "DataMapExpressEditDlg.h"
#include "LoadingWindow.h"
#include "AttachWindow.h"
#include "WebEventData.h"
#include "WebCommunicationHost.h"
#include "Psapi.h"
//DwmSetWindowAttribute
//#pragma comment(lib, "Dwmapi.lib")
#pragma comment(lib, "Psapi.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*
在结构树更新组件时，如果是数据映射导致组件更新，则To参数Id保存在g_strLinkerToNodeId。
*/
static CString g_strLinkerToNodeId;

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
	ON_REGISTERED_MESSAGE(WM_WEB_LOADED, OnWebLoaded)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_2000, ID_VIEW_APPLOOK_WIN7, OnAppLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_2000, ID_VIEW_APPLOOK_WIN7, OnUpdateAppLook)
	ON_COMMAND(ID_MDI_MOVE_TO_NEXT_GROUP, OnMdiMoveToNextGroup)
	ON_COMMAND(ID_MDI_MOVE_TO_PREV_GROUP, OnMdiMoveToPrevGroup)
	ON_COMMAND(ID_MDI_NEW_HORZ_TAB_GROUP, OnMdiNewHorzTabGroup)
	ON_COMMAND(ID_MDI_NEW_VERT_GROUP, OnMdiNewVertGroup)
	ON_COMMAND(ID_MDI_CANCEL, OnMdiCancel)
	ON_COMMAND(IDM_MODULE_OPENJIANMO, &CMainFrame::OnMenuModuleOpenjianmo)
	ON_COMMAND(IDM_MODULE_ACTIVEJIANMO, &CMainFrame::OnMenuModuleActivejianmo)
	ON_COMMAND(IDM_MODULE_CLOSEJIANMO, &CMainFrame::OnMenuModuleClosejianmo)
	ON_COMMAND(IDM_TASK_NEW, &CMainFrame::OnMenuTaskNew)
	ON_COMMAND(IDM_TASK_OPEN, &CMainFrame::OnMenuTaskOpen)
	ON_COMMAND(IDM_TASK_RUN, &CMainFrame::OnMenuTaskRun)
	ON_COMMAND(IDM_TOOL_JOBMGR, &CMainFrame::OnMenuJobMgr)
	ON_COMMAND(IDM_TASK_SAVEAS, &CMainFrame::OnMenuTaskSaveAs)
	ON_COMMAND_RANGE(IDM_TOOLBAR_BUTTON_FIRST, IDM_TOOLBAR_BUTTON_LAST, CMainFrame::OnToolbarClicked)
	ON_COMMAND_RANGE(IDM_ROBOTBAR_BUTTON_FIRST, IDM_ROBOTBAR_BUTTON_LAST, CMainFrame::OnRobotbarClicked)
	//	ON_COMMAND_RANGE(IDM_LOGINBAR_BUTTON_FIRST, IDM_LOGINBAR_BUTTON_LAST, CMainFrame::OnLoginbarClicked)
	ON_MESSAGE(WM_UPDATE_DATAMAPPING, &CMainFrame::OnUpdateDataMapping)
	ON_MESSAGE(WM_UPDATE_COMPONENT_BY_LINKER, &CMainFrame::OnUpdateComponentByLinker)
	ON_MESSAGE(WM_POP_MENU, &CMainFrame::OnPopMenuForSvg)
	ON_MESSAGE(WM_STRUCTTREE_UPDATE_PARAM_ARRAY, &CMainFrame::OnStructTreeUpdateParamArray)
	ON_MESSAGE(WM_STRUCTTREE_RUN_COMPNENT, &CMainFrame::OnStructTreeRunComponent)
	ON_MESSAGE(WM_STRUCTTREE_UPDATE_PROP, &CMainFrame::OnStructTreeUpdateProp)
	ON_MESSAGE(WM_STRUCTTREE_SHOW_TOOLTIP, &CMainFrame::OnStructTreeShowToolTip)
	ON_MESSAGE(WM_STRUCTTREE_SHOW_DATAMAPPING, &CMainFrame::OnStructTreeShowDataMapping)
	ON_MESSAGE(WM_SHOW_SVG_DRAG_LAYER, &CMainFrame::OnShowSvgDragLayer)

	ON_MESSAGE(WM_UPDATE_RESLIB_TREE, &CMainFrame::OnUpdateResLibTree)

	ON_MESSAGE(WM_STRUCTTREE_EDIT_IF, &CMainFrame::OnStructtreeEditIf)
	ON_MESSAGE(WM_STRUCTTREE_EDIT_ROOTPARAMS, &CMainFrame::OnStructtreeEditRootParams)
	ON_MESSAGE(WM_STRUCTTREE_EDIT_FOR, &CMainFrame::OnStructtreeEditFor)
	ON_MESSAGE(WM_STRUCTTREE_EDIT_DATAMAPPING_EXP, &CMainFrame::OnStructtreeEditDataMappingExp)
	ON_MESSAGE(WM_STRUCTTREE_RUN_PROCESS, &CMainFrame::OnStructTreeRunProcess)
	ON_MESSAGE(WM_STRUCTTREE_RUN_SUB_PROCESS, &CMainFrame::OnStructTreeRunSubProcess)
	ON_MESSAGE(WM_STRUCTTREE_SAVEAS_SUB_PROCESS, &CMainFrame::OnStructTreeSaveAsSubProcess)
	ON_MESSAGE(WM_SHOW_RUNFLOW_DIALOG, &CMainFrame::OnShowRunFlowDialog)

	ON_MESSAGE(WM_SHOW_RUNFLOW_VIEW, &CMainFrame::OnViewRunFlowWebPage)
	ON_REGISTERED_MESSAGE(WM_AW_PROCESS_ATTACHING, &CMainFrame::OnAwProcessAttaching)

	ON_MESSAGE(WM_PROGRESS_SHOW, &CMainFrame::OnShowProgress)
	ON_COMMAND(IDM_VIEW_TOOLBARZ, &CMainFrame::OnViewToolbarz)
	ON_COMMAND(IDM_VIEW_COMPONENT, &CMainFrame::OnViewComponent)
	ON_COMMAND(IDM_VIEW_LOGIN, &CMainFrame::OnViewLogin)
	ON_UPDATE_COMMAND_UI(IDM_VIEW_COMPONENT, &CMainFrame::OnUpdateViewComponent)
	ON_UPDATE_COMMAND_UI(IDM_VIEW_LOGIN, &CMainFrame::OnUpdateViewLogin)
	ON_UPDATE_COMMAND_UI(IDM_VIEW_TOOLBARZ, &CMainFrame::OnUpdateViewToolbarz)
	ON_WM_TIMER()
	ON_COMMAND(IDM_TASK_INFO, &CMainFrame::OnTaskInfo)
	ON_COMMAND(IDM_TASK_EXIT, &CMainFrame::OnTaskExit)
	ON_COMMAND(IDM_TASK_CLOSEALL, &CMainFrame::OnTaskCloseall)
	ON_COMMAND(IDM_TASK_SAVE, &CMainFrame::OnTaskSave)
	ON_COMMAND(IDM_TOOL_FLEXWARE, &CMainFrame::OnToolFlexware)
	ON_COMMAND(IDM_DISTRIBUTION, &CMainFrame::OnDistribution)
	ON_COMMAND(IDM_HELP_ABOUT, &CMainFrame::OnHelpAbout)
	ON_COMMAND(IDM_SYS_LOGIN, &CMainFrame::OnSysLogin)
	ON_COMMAND(IDM_SYS_LOGOUT, &CMainFrame::OnSysLogout)
	ON_UPDATE_COMMAND_UI(IDM_SYS_LOGIN, &CMainFrame::OnUpdateSysLogin)
	ON_UPDATE_COMMAND_UI(IDM_SYS_LOGOUT, &CMainFrame::OnUpdateSysLogout)
	ON_COMMAND(IDM_TASK_RECENT, &CMainFrame::OnTaskRecent)
	ON_UPDATE_COMMAND_UI(IDM_TASK_RECENT, &CMainFrame::OnUpdateTaskRecent)
	ON_COMMAND_RANGE(IDM_HISTORY_MODEL_START, IDM_HISTORY_MODEL_LAST, CMainFrame::OnMenuHistoryRecent)
	ON_COMMAND_RANGE(IDM_VIEW_START, IDM_VIEW_LAST, CMainFrame::OnMenuViewChange)
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(IDM_HELP, &CMainFrame::OnHelp)
	ON_COMMAND(IDM_TASK_PUBLISH, &CMainFrame::OnTaskPublish)
	ON_COMMAND(IDM_BAR_TREE, &CMainFrame::OnBarTree)
	ON_UPDATE_COMMAND_UI(IDM_BAR_TREE, &CMainFrame::OnUpdateBarTree)
	ON_COMMAND(IDM_RES, &CMainFrame::OnRes)
	ON_UPDATE_COMMAND_UI(IDM_RES, &CMainFrame::OnUpdateRes)
	ON_WM_COPYDATA()
	ON_WM_SIZE()
	ON_WM_THEMECHANGED()
	ON_WM_ACTIVATE()
	ON_WM_ACTIVATEAPP()
	ON_WM_CAPTURECHANGED()
	ON_WM_CHANGEUISTATE()
	ON_MESSAGE(WM_LOGINBAR_CLICKED_LOGOUT, &CMainFrame::OnLoginbarClickedLogout)
	ON_MESSAGE(WM_UPDATE_MAIN_TITLE, &CMainFrame::OnUpdateMainTitle)
	ON_MESSAGE(WM_STRUCTTREE_REFRESH, &CMainFrame::OnStructTreeRefresh)
	ON_REGISTERED_MESSAGE(WM_WEBEVENT_ASYNC,OnWebEventAsync)
	ON_WM_LBUTTONDOWN()
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

	LoadToolbar();
	LoadRobotbar();
	LoadLoginbar();

	EnableDocking(CBRS_ALIGN_TOP);



	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_TOP);
	DockControlBar(&m_wndMenuBar);
	//DockControlBarLeftOf( &m_wndMenuBar, &m_wndLoginBar );

	m_wndRobotBar.EnableDocking(CBRS_ALIGN_TOP);
	DockControlBar(&m_wndRobotBar);
	//DockControlBarLeftOf( &m_wndRobotBar, &m_wndLoginBar );

	m_wndToolBar.EnableDocking(CBRS_ALIGN_TOP);
	//DockControlBar(&m_wndToolBar);

	//DockControlBarLeftOf( &m_wndRobotBar, &m_wndLoginBar );
	//DockControlBarLeftOf( &m_wndToolBar, &m_wndRobotBar );

	//DockControlBarLeftOf( &m_wndMenuBar, &m_wndLoginBar );
	DockControlBarLeftOf( &m_wndToolBar, &m_wndRobotBar );

	m_wndLoginBar.EnableDocking(CBRS_ALIGN_TOP);
	DockControlBarLeftOf(&m_wndLoginBar, &m_wndRobotBar);
	DockControlBarLeftOf( &m_wndRobotBar, &m_wndLoginBar );
	DockControlBarLeftOf( &m_wndToolBar, &m_wndRobotBar );


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

	if (theApp.m_pDlgDragLayer == NULL)
	{
		theApp.m_pDlgDragLayer = new CDlgDragLayer();
		theApp.m_pDlgDragLayer->Create(CDlgDragLayer::IDD, this);
	}

	theApp.LoadDefaultFont();

	//HOOK，改变网页对话框标题
	//g_hPrevHookXP = SetWindowsHookEx(WH_CALLWNDPROC, HookProcXP, 0, GetCurrentThreadId());
	HookWebPageTitle(TRUE, g_lpszAppTitle);

	AdjustClientArea();

	SetWindowText( g_lpszAppTitle );
	SetTitle( g_lpszAppTitle );
	ShowWindow(SW_HIDE);

	{ //初始化菜单最近打开文件
		SetTimer( TIMER_MAINFRAME_RECENTLYMENU, TIMER_MAINFRAME_RECENTLYMENU_TIMEOUT, 0);
	}

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

	mdiTabParams.m_style = CBCGPTabWnd::STYLE_3D_SCROLLED; 
	mdiTabParams.m_bAutoColor = FALSE;
	mdiTabParams.m_bTabIcons = FALSE;
	mdiTabParams.m_bSetActiveTabBoldFont = FALSE;
	mdiTabParams.m_nTabMinFullWidth = 129;
	mdiTabParams.m_bActiveTabCloseButton = TRUE;
	mdiTabParams.m_bDocumentMenu = TRUE;
	mdiTabParams.m_bEnableTabSwap = FALSE;
	mdiTabParams.m_bEnableDragTab = FALSE;
	EnableMDITabbedGroups (TRUE, mdiTabParams);

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
	//只有一个建模页签而不会有多个建模页签需判断保存的情况下，先尝试关闭建模页面的内容，并置为初始状态，然后关闭所有文档
	if (!theApp.m_processMgr.OnClose())
	{
		return;
	}

	if (theApp.m_pDlgDragLayer != NULL)
	{
		if ( IsWindow( theApp.m_pDlgDragLayer->GetSafeHwnd() ) )
			theApp.m_pDlgDragLayer->DestroyWindow();
		delete theApp.m_pDlgDragLayer;
		theApp.m_pDlgDragLayer = NULL;
	}

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

void CMainFrame::OnMenuModuleOpenjianmo()
{
	theViewMgr.NewView( g_lpszViewTitleModel, viewflagModel );
}

void CMainFrame::OnMenuModuleActivejianmo()
{
	VIEW_DATA * pView = theViewMgr.Find( g_lpszViewTitleModel );
	theViewMgr.ActiveView(pView);
}


void CMainFrame::OnMenuModuleClosejianmo()
{
	VIEW_DATA * pView = theViewMgr.Find( g_lpszViewTitleModel );
	theViewMgr.CloseView(pView);
}


void CMainFrame::OnMenuTaskNew()
{
	theApp.m_processMgr.OnNew();
	return;
}

void CMainFrame::OnMenuTaskSaveAs()
{
	theApp.m_processMgr.OnSaveAs();
	return;
}

void CMainFrame::OnMenuTaskOpen()
{
	theApp.m_processMgr.OnOpen();
	return;
}

void CMainFrame::OnMenuTaskRun()
{
	if ( theApp.m_pBarJianmoTree == NULL ) return;
	theApp.m_pBarJianmoTree->m_wndGridTree.OnMenuJianmoTreeProcessRun();
	return;
}

void CMainFrame::LoadToolbar()
{
	if (
		!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC | CBRS_ORIENT_HORZ
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

void CMainFrame::LoadRobotbar()
{
	if (
		!m_wndRobotBar.CreateEx(this
		, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC | CBRS_ORIENT_HORZ
		, CRect(1,0,1,0)
		, IDC_MAIN_ROBOTBAR)
		)
	{
		TRACE0("Failed to create toolbar\n");
		return ;      // fail to create
	}
	m_wndRobotBar.SetShowTooltips(TRUE);

	m_wndRobotBar.LoadData();

	m_wndRobotBar.SetWindowText ("组件");
}

void CMainFrame::LoadLoginbar()
{
	if (
		!m_wndLoginBar.CreateEx(this
		, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_FIXED | CBRS_ORIENT_HORZ
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
	case CCxBCGPToolBar::IMG_NEW_PROCESS:
		OnMenuTaskNew();
		break;
	case CCxBCGPToolBar::IMG_OPEN_PROCESS:
		OnMenuTaskOpen();
		break;
	case CCxBCGPToolBar::IMG_RUN_PROCESS:
		OnMenuTaskRun();
		break;
	case CCxBCGPToolBar::IMG_SAVE:
		theApp.m_processMgr.OnSave();
		break;
	case CCxBCGPToolBar::IMG_SAVEAS:
		theApp.m_processMgr.OnSaveAs();
		break;
	case CCxBCGPToolBar::IMG_INFO:
		OnTaskInfo();
		break;
	case CCxBCGPToolBar::IMG_PUBLISH:
		OnTaskPublish();
		break;
	case CCxBCGPToolBar::IMG_DATALINKER:
		ShowDataMapping();
		break;
	case CCxBCGPToolBar::IMG_JOBMGR:
		ShowJobMgr();
		break;
	case CCxBCGPToolBar::IMG_PREVIEW:
		ShowDistribution();
		break;
	case CCxBCGPToolBar::IMG_HELP:
		{
			theApp.OnAppAbout();
		}
		break;
		// 	case CCxBCGPToolBar::IMG_ABOUT:
		// 		{
		// 			//theViewMgr.NewView(viewflagWebPage, viewflagWebPage);
		// 			theApp.OnAppAbout();
		// 		}
		break;
	default:
		break;
	}
}


void CMainFrame::OnRobotbarClicked( UINT nID )
{
	nID -= IDM_ROBOTBAR_BUTTON_FIRST;

	//switch ( nID )
	//{
	//case CCxStaticRes::RT_NULL + 1:
	//	break;
	//default:
	//	break;
	//}
}


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
	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return;

	CWebMethodData wmd;
	wmd.m_sMethodName = "saveXpdl";
	pSvg->InvokeJsMethod( &wmd );

	CXmlHistoryModel xhm;
	xhm.Load();

	CHistoryModelData * pHMD = new CHistoryModelData();
	pHMD->m_strName = theApp.m_strXpdlName;
	pHMD->m_strPath = theApp.m_strXpdlWorkPath;
	xhm.Insert( pHMD );

	xhm.Save();
}

void CMainFrame::ShowJobMgr( void )
{
	VIEW_DATA * pView = theViewMgr.Find( g_lpszViewTitleJobMgr );
	if ( pView != NULL )
	{
		theViewMgr.ActiveView(pView);
		return;
	}

	if ( theApp.m_LoginData.m_strTicket.IsEmpty() )
	{
		CDlgLoginBk dlgLogin;
		dlgLogin.DoModal();
		if ( theApp.m_LoginData.m_strTicket.IsEmpty() )
		{
			return;
		}
	}
	theViewMgr.NewView( g_lpszViewTitleJobMgr, viewflagJobMgr );

	CDlgJobMgr * pMonitor = GetJobMgrDilogPtr();
	if ( pMonitor == NULL ) return;

	pMonitor->LoadUrl();
}

void CMainFrame::ShowDistribution( void )
{
	VIEW_DATA * pView = theViewMgr.Find( g_lpszViewTitleDistribution);
	if ( pView != NULL )
	{
		theViewMgr.ActiveView(pView);
		return;
	}

	if ( theApp.m_LoginData.m_strTicket.IsEmpty() )
	{
		CDlgLoginBk dlgLogin;
		dlgLogin.DoModal();
		if ( theApp.m_LoginData.m_strTicket.IsEmpty() )
		{
			return;
		}
	}
	theViewMgr.NewView( g_lpszViewTitleDistribution, viewflagDistribution );

	CDlgDistribution * pDistribution = GetDistributionDilogPtr();
	if ( pDistribution == NULL ) return;

	pDistribution->LoadUrl();
}

void CMainFrame::ShowDataMapping( void )
{
	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return;

	CWebMethodData wmd;
	wmd.m_sMethodName = "showDataMapping";
	pSvg->InvokeJsMethod( &wmd );

	wmd.Clear();
	wmd.m_sMethodName = "getDataMapping";
	pSvg->InvokeJsMethod( &wmd );
	if (wmd.m_sResult.IsEmpty()) return;

	VIEW_DATA *pViewData = theViewMgr.Find(g_lpszViewTitleModel);
	if (pViewData)
		theViewMgr.ActiveView(pViewData);

	//if (!UpdateStructTree_UpdateDataMapping( wmd.m_strResult )) return;
	//theApp.m_pBarJianmoTree->m_wndGridTree.UpdateDataMapping();

	//theApp.m_pBarJianmoTree->m_wndGridTree.m_bModified = TRUE;
}

LRESULT CMainFrame::OnStructTreeShowDataMapping(WPARAM, LPARAM)
{
	ShowDataMapping();
	return 0;
}

LRESULT CMainFrame::OnShowSvgDragLayer(WPARAM wp, LPARAM lp)
{
	RedrawWindow();

	BOOL bShow = (BOOL)wp;
	int iButton = (int)lp;

	m_wndRobotBar.DragLayer(iButton);
	return 0;
}


void CMainFrame::UpdateRobotbar( list< CResLibData* > BaseComponentList )
{
	m_wndRobotBar.UpdateResData(BaseComponentList);
}


BOOL CMainFrame::GetToolbarButtonToolTipText (CBCGPToolbarButton* pButton, CString& strTTText)
{
	return FALSE;
}

LRESULT CMainFrame::OnStructTreeUpdateParamArray (WPARAM wParam, LPARAM lParam)
{
	CCxStructTreeNodeParam * pParamNode = reinterpret_cast< CCxStructTreeNodeParam * >( wParam );
	if ( pParamNode == NULL ) return 0;
	CString * pStr = (CString *)lParam;
	theApp.m_pBarJianmoTree->m_wndGridTree.m_StructTree.OnUpdateParamValue( pParamNode, (LPCTSTR)(*pStr) );
	delete pStr;

	if ( pParamNode->IsArray() )
	{
		BOOL bValid = !( (pParamNode->m_dwStatus & CCxStructTreeNode::NS_NAME_INVALID) == 
			CCxStructTreeNode::NS_NAME_INVALID );

		pParamNode->SetStatus( bValid ,TRUE );
	}

	//theApp.m_pBarJianmoTree->m_wndGridTree.m_StructTree.UpdateArrayParamNode( pParamNode );
	theApp.m_pBarJianmoTree->m_wndGridTree.AdjustLayout();
	OnUpdateDataMapping( (WPARAM)pParamNode, 0 );
	return 0;
}

LRESULT CMainFrame::OnUpdateDataMapping (WPARAM wParam, LPARAM lParam)
{
	CCxStructTreeNode * pSelNode = (CCxStructTreeNode *)wParam;
	CCxStructTreeNodeParam* pParamNode = NULL;
	CString* pToId = (CString*)lParam;

	theApp.m_pBarJianmoTree->m_wndGridTree.m_StructTree.RetriveDataMapping();
	theApp.m_pBarJianmoTree->m_wndGridTree.m_StructTree.ResetRunFlag();
	theApp.m_pBarJianmoTree->m_wndGridTree.m_StructTree.SetSelNode(pSelNode);
	theApp.m_pBarJianmoTree->m_wndGridTree.m_StructTree.OnNodeSelected(TRUE);
	theApp.m_pBarJianmoTree->m_wndGridTree.m_StructTree.UpdateGridAllRowStatus();

	if ( pToId != NULL )
	{
		CString strId = *pToId;
		delete pToId; pToId = NULL;
		CCxStructTreeNode * pNode = theApp.m_pBarJianmoTree->m_wndGridTree.m_StructTree.Find( strId );
		if ( pNode != NULL )
		{
			CCxStructTreeNodeParam * pTo = NULL;
			if ( pNode->IsParamNode() )
			{
				pTo = (CCxStructTreeNodeParam *)pNode;
			}
			if ( pTo != NULL )
			{
				vector< CCxStructTreeNodeParam* > vSpecNodes;
				if ( 0 == pTo->GetLinkerFromNodes( vSpecNodes ) )
				{
					//删除数据映射
					if ( pTo->IsIn() )
					{
						pTo->SetStatus( TRUE, TRUE );

						CJsData_Svg_UpdateParamStatus svgUpdateParamStatus;
						CJsData_Svg_UpdateParamStatusData * pUpdateParamStatusData = NULL;
						pUpdateParamStatusData = new CJsData_Svg_UpdateParamStatusData();
						pUpdateParamStatusData->m_dwStatus = pTo->m_dwStatus;
						pUpdateParamStatusData->m_strId = pTo->m_strID;
						svgUpdateParamStatus.m_vData.push_back( pUpdateParamStatusData );
						CString str1 = svgUpdateParamStatus.GetJsonStr();
						CDlgSvg* pSvg = GetSvgDilogPtr();
						if ( pSvg != NULL )
						{
							pSvg->JsMethod_updateParamStatus( str1 );
						}
					}
				}
				else
				{
					pTo->OnAfterNewLinker();
				}
				
			}
		}
	}

	if ( !g_strLinkerToNodeId.IsEmpty() )
	{
		vector< CString > vIds;
		Json::Reader reader;
		Json::Value arr;
		CString strId;
		vector< CCxStructTreeNode* > vToNodes;
		try
		{
			if (!reader.parse((char*)(LPCTSTR)g_strLinkerToNodeId, arr, false)) return 0;
			if (!arr.isArray()) return 0;
			for ( Json::Value::iterator itrChild = arr.begin(); itrChild != arr.end(); ++itrChild)
			{
				strId = ( (*itrChild).isString() ) ? (*itrChild).asCString() : _T("");
				vIds.push_back( strId );
			}
			g_strLinkerToNodeId = "";
		}
		catch (...)
		{
			g_strLinkerToNodeId = "";
			return 0;
		}

		for (int i=0; i<(int)vIds.size(); i++)
		{
			CCxStructTreeNode * pNode = theApp.m_pBarJianmoTree->m_wndGridTree.m_StructTree.Find( vIds[i] );
			if ( pNode != NULL )
			{
				vToNodes.push_back( pNode );
			}
			else
			{
				return 0;
			}
		}

		for ( int i=0; i<(int)vToNodes.size(); i++ )
		{
			CCxStructTreeNodeParam * pTo = NULL;
			if ( vToNodes[i]->IsParamNode() )
			{
				pTo = (CCxStructTreeNodeParam *)vToNodes[i];
			}
			if ( pTo != NULL )
			{
				if ( pTo->IsIn() )
				{
					pTo->SetStatus( FALSE, TRUE );
				}

				pTo->OnAfterNewLinker();
			}
		}

		
		
		//if ( pNode != NULL )
		//{
		//	CCxStructTreeNodeParam * pTo = NULL;
		//	if ( pNode->IsParamNode() )
		//	{
		//		pTo = (CCxStructTreeNodeParam *)pNode;
		//	}
		//	if ( pTo != NULL )
		//	{
		//		if ( pTo->IsIn() )
		//		{
		//			pTo->SetStatus( FALSE, TRUE );
		//		}

		//		pTo->OnAfterNewLinker();
		//	}
		//}
	}

	return 0;
}

LRESULT CMainFrame::OnPopMenuForSvg (WPARAM wParam, LPARAM /*lParam*/)
{
	CCxStructTreePopMenuData * popd = (CCxStructTreePopMenuData *)wParam;
	if (wParam == NULL)
	{
		return 0;
	}

	CDlgSvg* pSvg = GetSvgDilogPtr();
	if (pSvg == NULL)
	{
		return 0;
	}

	CWebMethodData wmd;
	CWebMethodParamData * pParam = NULL;
	//方法名称
	wmd.m_sMethodName = "xpdlNodeSelect";
	//第1个参数
	pParam = new CWebMethodParamData();
	pParam->m_sName = "id";
	pParam->m_sValue = popd->m_strId;
	wmd.m_vParams.push_back(pParam);
	pSvg->InvokeJsMethod( &wmd );

	theApp.m_pBarJianmoTree->m_wndGridTree.ShowPopMenuForSvg(popd);
	delete popd;
	return 1;
}

LRESULT CMainFrame::OnStructTreeRunComponent (WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return 0;

	//CCxStructTreeRowT * pRow = (CCxStructTreeRowT *) theApp.m_pBarJianmoTree->m_wndGridTree.GetCurSel();
	//if (pRow == NULL) return 0;
	CCxStructTreeNodeComponent * pNode = reinterpret_cast< CCxStructTreeNodeComponent * >( theApp.m_pBarJianmoTree->m_wndGridTree.GetCurSelNodeData() );
	if ( pNode == NULL ) return 0;
	if ( pNode->m_pTree == NULL ) return 0;

	//pNode->m_pTree->BeforeRunning( pNode );
	if (!theApp.m_processMgr.GetComponentStateCanSave()) return 0;
	if ( !pNode->CanRun() )
	{
		::MessageBox(GetMainWnd(), "存在未成功运行的上游节点，无法运行", g_lpszAppTitle, MB_OK|MB_ICONERROR);
		return 0;
	}
	CString strData = pSvg->JsMethod_runComponent( pNode );
	CString strFlag = "runComponentCompleted";

	theApp.m_pBarJianmoTree->m_wndGridTree.m_StructTree.SvgEvent(strFlag, strData);

	//CWebMethodParamData * pParam = NULL;
	//CWebMethodData wmd;
	////
	//wmd.m_strMethodName = "runComponent";
	////第1个参数
	//pParam = new CWebMethodParamData();
	//pParam->m_strName = "id";
	//pParam->m_strValue = pNode->m_strID;
	//wmd.m_vParams.push_back(pParam);
	////
	//pSvg->InvokeJsMethod( &wmd );
	theApp.m_pBarJianmoTree->m_wndGridTree.m_bModified = TRUE;
	return 0;
}


LRESULT CMainFrame::OnStructTreeUpdateProp (WPARAM wParam, LPARAM /*lParam*/)
{
	CCxStructTreeNode * pNode = reinterpret_cast<CCxStructTreeNode *>(wParam);
	theApp.m_pBarJianmoTree->m_wndGridTree.m_StructTree.UpdateNodeProp(pNode);
	return 0;
}


LRESULT CMainFrame::OnStructTreeShowToolTip (WPARAM wParam, LPARAM lParam)
{
	CCxStructTreeNode * pNode = reinterpret_cast<CCxStructTreeNode *>(wParam);
	if ( pNode == NULL ) return 0;
	BOOL bShow = (BOOL)lParam;

	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return 0;

	CPoint pt;
	::GetCursorPos( &pt );
	pt.Offset(g_ToolTipOffset);
	//::ScreenToClient(pSvg->GetSafeHwnd(), &pt);
	CRect rc(pt.x,pt.y,pt.x,pt.y);
	//rc.InflateRect(5,5);

	if (!bShow)
	{
		pSvg->m_ToolTip.Deactivate();
	}
	else
	{
		CString strTip;
		if ( pNode->IsComponentNode() )
			strTip = ((CCxStructTreeNodeComponent*)pNode)->GetTooltipText();
		else if ( pNode->IsFlowNode() || pNode->IsRootNode() )
			strTip = ((CCxStructTreeNodeFlow*)pNode)->GetTooltipText();
		if ( strTip.IsEmpty() ) return 0;

		//strTip.Format( "名称：%s\r\n作者：%s\r\n时间：%s\r\n版本：%s\r\n描述：%s"
		//	, pNode->m_strName
		//	, pNode->m_strAuthor
		//	, pNode->m_strCreated
		//	, pNode->m_strVersion
		//	, pNode->m_strDescription);

		pSvg->m_ToolTip.SetTextMargin (TEXT_MARGIN + 2);
		pSvg->m_ToolTip.SetFont (GetFont ());
		pSvg->m_ToolTip.Track (rc, strTip);
	}
	return 0;
}
afx_msg LRESULT CMainFrame::OnUpdateResLibTree(WPARAM wParam, LPARAM lParam)
{
	theApp.m_pDlgResLibTree->m_tree.Load();

	list< CResLibData* > BaseComponentList;
	theApp.m_pDlgResLibTree->m_tree.GetBaseComponentRoot( BaseComponentList );
	CMainFrame * pMainFrame = reinterpret_cast< CMainFrame * > ( theApp.m_pMainWnd );
	pMainFrame->UpdateRobotbar( BaseComponentList );
	return 0;
}

afx_msg LRESULT CMainFrame::OnStructtreeEditIf(WPARAM wParam, LPARAM lParam)
{
	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return FALSE;
	CWebMethodData wmd;
	wmd.m_sMethodName = "getSubProcessStateCanSave";
	pSvg->InvokeJsMethod( &wmd );
	CString sResult = wmd.m_sResult;
	Json::Value root;
	Json::Reader reader;
	if (!reader.parse((LPCTSTR)sResult, root))
	{
		::MessageBox(theApp.m_pMainWnd->GetSafeHwnd(), "getSubProcessStateCanSave返回值格式不正确", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}

	CString sFlag = root["flag"].asString().c_str();	
	if (sFlag.CompareNoCase("false") == 0)
	{
		CString sMsg = root["msg"].asString().c_str();
		::MessageBox(theApp.m_pMainWnd->GetSafeHwnd(), "分支节点中存在空节点，不允许编辑", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}

	std::string str = *(CString*)wParam;
	delete (CString*)wParam;
	CDlgIf dlg(str);
	dlg.DoModal();
	return 0;

	// 	CDlgExpEditorDM dlg("{\"newExp\" : \"NewExpression == true\", \"oldExp\" : \"Old\", \"Error\" : \"ERR\"}");
	// 	CDlgExpEditorDM dlg("{\"OldExp\":\"公式组件_1.b=公式组件.a\",\"NewExp\":\"\",\"Error\":\"\"}");
	// 	dlg.DoModal();
	// 	return 0;
}

LRESULT CMainFrame::OnStructtreeEditRootParams( WPARAM wParam, LPARAM lParam )
{
	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return FALSE;
	CWebMethodData wmd;
	// 	wmd.m_strMethodName = "GetRootParams";
	// 
	// 	if(!pSvg->InvokeJsMethod( &wmd ))
	// 	{
	// 		AfxMessageBox("GetRootParams调用失败！");
	// 		return FALSE;
	// 	}
	// 
	// 	CString sResult = wmd.m_strResult;
	 CCxStructTreeNode* pNode =theApp.m_pBarJianmoTree->m_wndGridTree.GetCurSelNodeData();
	CString sResult = CDlgParamSet::GetJsonStrFromSelNode(pNode).c_str();
	//AfxMessageBox(sResult);

	Json::Value root;
	Json::Reader reader;
	if (!reader.parse((LPCTSTR)sResult, root))
	{
		//::MessageBox(theApp.m_pMainWnd->GetSafeHwnd(), "GetRootParams返回值格式不正确", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		::MessageBox(theApp.m_pMainWnd->GetSafeHwnd(), "Json字符串格式不正确", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}

	CDlgParamSet dlg(sResult);

	if(dlg.DoModal() != IDOK)
	{
		return FALSE;
	}

	CWebMethodParamData * pParam = NULL;
	//
	wmd.m_sMethodName = "SetProcessParams";
	//第1个参数
	pParam = new CWebMethodParamData();
	pParam->m_sName = "jsonData";
	pParam->m_sValue = dlg.m_strJson.c_str();
	wmd.m_vParams.push_back(pParam);
	//
	if ( !pSvg->InvokeJsMethod( &wmd ) )
	{
		::MessageBox(GetMainWnd(),"调用SetProcessParams失败！", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	else if ( wmd.m_sResult.IsEmpty() )
	{
		::MessageBox( GetMainWnd(),"调用SetProcessParams返回值为空！", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	else
	{//更新结构树
		//AfxMessageBox(wmd.m_strResult);
		if(dlg.m_bParamsChanged)
		{//参数改变
			//theApp.m_pBarJianmoTree->m_wndGridTree.m_StructTree.SvgEvent( CString("updateFlowParams"), wmd.m_strResult );
			theApp.m_pBarJianmoTree->m_wndGridTree.m_StructTree.SvgEvent( CString("updateParams"), wmd.m_sResult );
		}

		return FALSE;
	}

	return 0;
}

LRESULT CMainFrame::OnStructtreeEditFor( WPARAM wParam, LPARAM lParam )
{
	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return FALSE;
	CWebMethodData wmd;
	// 	wmd.m_strMethodName = "GetRootParams";
	// 
	// 	if(!pSvg->InvokeJsMethod( &wmd ))
	// 	{
	// 		AfxMessageBox("GetRootParams调用失败！");
	// 		return FALSE;
	// 	}
	// 
	// 	CString sResult = wmd.m_strResult;

	CString sResult = CDlgParamSetBase::GetJsonStrFromSelNode(theApp.m_pBarJianmoTree->m_wndGridTree.GetCurSelNodeData()).c_str();
	//AfxMessageBox(sResult);

	Json::Value root;
	Json::Reader reader;
	if (!reader.parse((LPCTSTR)sResult, root))
	{
		//::MessageBox(theApp.m_pMainWnd->GetSafeHwnd(), "GetRootParams返回值格式不正确", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		::MessageBox(theApp.m_pMainWnd->GetSafeHwnd(), "Json字符串格式不正确", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}

	CDlgFor dlg(sResult);

	if(dlg.DoModal() != IDOK)
	{
		return FALSE;
	}

	CWebMethodParamData * pParam = NULL;
	//
	wmd.m_sMethodName = "SetProcessParams";
	//第1个参数
	pParam = new CWebMethodParamData();
	pParam->m_sName = "jsonData";
	pParam->m_sValue = dlg.m_strJson.c_str();
	wmd.m_vParams.push_back(pParam);
	//
	if ( !pSvg->InvokeJsMethod( &wmd ) )
	{
		::MessageBox(GetMainWnd(),"调用SetProcessParams失败！", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	else if ( wmd.m_sResult.IsEmpty() )
	{
		::MessageBox( GetMainWnd(),"调用SetProcessParams返回值为空！", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	else
	{//更新结构树
		//AfxMessageBox(wmd.m_strResult);
		if(dlg.m_bParamsChanged)
		{//参数改变
			//theApp.m_pBarJianmoTree->m_wndGridTree.m_StructTree.SvgEvent( CString("updateFlowParams"), wmd.m_strResult );
			theApp.m_pBarJianmoTree->m_wndGridTree.m_StructTree.SvgEvent( CString("updateParams"), wmd.m_sResult );
		}

		return FALSE;
	}

	return 0;
}



afx_msg LRESULT CMainFrame::OnStructtreeEditDataMappingExp(WPARAM wParam, LPARAM lParam)
{
	std::string str = *(CString*)wParam;
	delete (CString*)wParam;
	//CDlgExpEditorDM dlg(str.c_str());
	//str = "{\"OldExp\":\"#{n9sx64aa82ba296-9848-4b36-9b29-cbb41ba2e0c1} = #{nw8272b64b08669-ddd1-46cf-aa61-c2ca0945a6b0[4,5]} + 45\",\"NewExp\":\"\",\"Error\":null}";
	CDataMapExpressEditDlg dlg(str.c_str(),this);
	dlg.DoModal();
	return 0;
}

afx_msg LRESULT CMainFrame::OnStructTreeRunProcess(WPARAM wParam, LPARAM lParam)
{
	if ( theApp.m_pBarJianmoTree == NULL ) return 0;
	theApp.m_pBarJianmoTree->m_wndGridTree.OnMenuJianmoTreeProcessRun();
	return 0;
}

afx_msg LRESULT CMainFrame::OnStructTreeRunSubProcess(WPARAM wParam, LPARAM lParam)
{
	if ( theApp.m_pBarJianmoTree == NULL ) return 0;
	theApp.m_pBarJianmoTree->m_wndGridTree.OnMenuJianmoTreeFlowRun();
	return 0;
}

afx_msg LRESULT CMainFrame::OnStructTreeSaveAsSubProcess(WPARAM wParam, LPARAM lParam)
{
	if ( theApp.m_pBarJianmoTree == NULL ) return 0;
	theApp.m_pBarJianmoTree->m_wndGridTree.OnMenuStructTreeFlowSaveAs();
	return 0;
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

void CMainFrame::OnMenuJobMgr()
{
	ShowJobMgr();
}


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
	BOOL b = m_wndRobotBar.IsVisible();
	if(b)
		m_wndRobotBar.ShowControlBar(FALSE, FALSE, FALSE);
	else
		m_wndRobotBar.ShowControlBar(TRUE, FALSE, TRUE);
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
	BOOL b = m_wndRobotBar.IsVisible();
	if(b)
		pCmdUI->SetCheck(TRUE);  
	else
		pCmdUI->SetCheck(FALSE);  
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


void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	switch(nIDEvent)
	{
	case TIMER_MAINFRAME_RECENTLYMENU: //最近打开文件更新
		{
			//m_wndLoginBar.MoveToRight();

			m_xhm.Clear();
			m_xhm.Load();

			CMenu* pMenu = CMenu::FromHandle(m_wndMenuBar.GetDefaultMenu())->GetSubMenu(0)->GetSubMenu(7);
			if(!pMenu)
			{
				KillTimer(nIDEvent);
				return;
			}
			//pMenu->GetSubMenu(0)->DeleteMenu(0, MF_BYPOSITION);
			// 		pMenu->GetSubMenu(0)->DeleteMenu(0, MF_BYPOSITION);
			// 		pMenu->GetSubMenu(0)->DeleteMenu(0, MF_BYPOSITION);
			// 		pMenu->GetSubMenu(0)->DeleteMenu(0, MF_BYPOSITION);
			//pMenu->GetSubMenu(0)->GetSubMenu(0)->AppendMenu(0, 1, "fgsdgd");
			//pMenu->GetSubMenu(0)->AppendMenuA(MF_POPUP, 0, "test");
			//pMenu->DeleteMenu(0, MF_BYPOSITION);
			int n = pMenu->GetMenuItemCount();
			for(int i = 0; i < n; i++)
			{
				pMenu->DeleteMenu(0, MF_BYPOSITION);
			}
			//pMenu->AppendMenu(0, 1, "test");

			for ( int i=0; i<(int)m_xhm.m_vDatas.size(); i++ )
			{
				CString& str = m_xhm.m_vDatas[i]->m_strPath;
				pMenu->AppendMenu(0, IDM_HISTORY_MODEL_START + i, str);
			}

			if(m_xhm.m_vDatas.empty())
			{
				pMenu->AppendMenu(0, 1, "空");
			}

			m_wndMenuBar.CreateFromMenu(m_wndMenuBar.GetDefaultMenu(), TRUE, TRUE);
			KillTimer(nIDEvent);
			return;
		}
	case TIMER_MAINFRAME_UPDATEWINDOW: //窗口切换更新
		{
			CMenu* pMenu = CMenu::FromHandle(m_wndMenuBar.GetDefaultMenu())->GetSubMenu(1)->GetSubMenu(7);
			if(!pMenu)
			{
				KillTimer(nIDEvent);
				return;
			}

			int n = pMenu->GetMenuItemCount();
			for(int i = 0; i < n; i++)
			{
				pMenu->DeleteMenu(0, MF_BYPOSITION);
			}

			for ( int i=0; i<(int)theViewMgr.m_vViews.size(); i++ )
			{
				CString& str = theViewMgr.m_vViews[i]->m_strTitle;
				pMenu->AppendMenu(0, IDM_VIEW_START + i, str);
			}

			if(theViewMgr.m_vViews.empty())
			{
				pMenu->AppendMenu(0, 1, "空");
			}

			m_wndMenuBar.CreateFromMenu(m_wndMenuBar.GetDefaultMenu(), TRUE, TRUE);
			KillTimer(nIDEvent);
			return;
		}
	}


	// 	static bool bHide = false;
	// 	if(bHide == false)
	// 	{
	// 		CMenu* pMenu = CMenu::FromHandle(m_wndMenuBar.GetDefaultMenu());
	// 		pMenu->GetSubMenu(1)->DeleteMenu(0, MF_BYPOSITION);
	// 		pMenu->GetSubMenu(1)->DeleteMenu(0, MF_BYPOSITION);
	// 		pMenu->GetSubMenu(1)->DeleteMenu(0, MF_BYPOSITION);
	// 		pMenu->GetSubMenu(1)->DeleteMenu(0, MF_BYPOSITION);
	// 		m_wndMenuBar.CreateFromMenu(pMenu->GetSafeHmenu(), TRUE, TRUE);
	// 		bHide = true;
	// 		KillTimer(0);
	// 	}
	CBCGPMDIFrameWnd::OnTimer(nIDEvent);
}


BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	switch (pMsg->message)
	{
	case WM_SYSKEYDOWN:
		if( (::GetKeyState(VK_MENU) < 0) )
		{
			::SetFocus( this->GetSafeHwnd() );
		}
	case WM_KEYDOWN:
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

LRESULT CMainFrame::OnShowRunFlowDialog( WPARAM wParam, LPARAM lParam )
{
	LPCTSTR lpszUrl = (LPCTSTR)wParam;
	LPCTSTR lpszFlowId = (LPCTSTR)lParam;
	if ( lpszUrl == NULL || lpszFlowId == NULL ) return 0;

	CDlgRunFlow dlg;
	dlg.m_lpszUrl = lpszUrl;
	dlg.m_lpszFlowId = lpszFlowId;

	if ( !dlg.DoModal() )
	{
		return 0;
	}

	if ( !dlg.m_strResultData.IsEmpty() )
	{
		CCxStructTreeRunResult rr;
		if ( !rr.Parse( dlg.m_strResultData, &theApp.m_pBarJianmoTree->m_wndGridTree.m_StructTree ) )
		{
			ZTools::WriteZToolsFormatLog("解析运行结果失败 %s", "\r\n" );
		}
		else
		{
			rr.UpdateFileParam();
			dlg.m_strResultData = rr.GetSvgResultJsonStr();
		}

		CDlgSvg * pSvg = GetSvgDilogPtr();
		if ( !pSvg->JsMethod_parseSubProcess( dlg.m_strResultData ) )
		{
			HideLoading();
			return 0;
		}

		theApp.m_pBarJianmoTree->m_wndGridTree.m_StructTree.UpdateParams( dlg.m_strResultData );
	}

	return 0;
}


LRESULT CMainFrame::OnViewRunFlowWebPage( WPARAM wParam, LPARAM lParam )
{
	CString *  pJsonObjStr = (CString *)wParam;
	if ( pJsonObjStr == NULL ) return 0;

	CJsData_ShowViewRunFlow jsd;
	if ( !jsd.Parse( *pJsonObjStr ) ) return 0;

	CDlgViewRunFlow dlg;
	dlg.m_strUrl = jsd.m_strUrl;
	if ( jsd.m_nHeight != 0 )
		dlg.m_nHeight = jsd.m_nHeight;
	if ( jsd.m_nWidth != 0 )
		dlg.m_nWidth = jsd.m_nWidth;

	delete pJsonObjStr; pJsonObjStr = NULL;

	dlg.DoModal();

	CDlgJobMgr *pHm = GetJobMgrDilogPtr();
	if ( pHm != NULL )
	{
		pHm->JsMethod_reloadGrid();
	}

	return 0;
}


void CMainFrame::OnTaskInfo()
{
	// TODO: 在此添加命令处理程序代码
	if ( theApp.m_pMainWnd == NULL ) return;
	if ( !IsWindow( theApp.m_pMainWnd->GetSafeHwnd() ) ) return;
	CCxStructTreeNode * pNode = (CCxStructTreeNodeFlow*)theApp.m_pBarJianmoTree->m_wndGridTree.m_StructTree.GetRoot();
	if ( pNode == NULL ) return;

	theApp.m_pMainWnd->PostMessage( WM_STRUCTTREE_UPDATE_PROP
		, reinterpret_cast<WPARAM>(pNode) );
}


void CMainFrame::OnTaskExit()
{
	// TODO: 在此添加命令处理程序代码
	OnClose();
}


void CMainFrame::OnTaskCloseall()
{
	// TODO: 在此添加命令处理程序代码
	theApp.m_processMgr.OnClose();
}


void CMainFrame::OnTaskSave()
{
	// TODO: 在此添加命令处理程序代码
	theApp.m_processMgr.OnSave();
}


void CMainFrame::OnToolFlexware()
{
	// TODO: 在此添加命令处理程序代码
	CString strAppPath = GetPeraComponentPath();
	//运行
	SHELLEXECUTEINFO si;
	memset(&si, NULL, sizeof(si));
	si.cbSize = sizeof(si);
	//si.nShow = SW_HIDE;
	si.fMask = SEE_MASK_NOCLOSEPROCESS;
	si.lpVerb = _T("open");
	si.lpDirectory = "";
	si.lpFile = strAppPath;
	si.lpParameters = "";
	if (!ShellExecuteEx(&si))
	{
		ZTools::WriteZToolsLog(ZTools::FormatString("%s执行失败", strAppPath));
		return;
	}
}


void CMainFrame::OnDistribution()
{
	// TODO: 在此添加命令处理程序代码
	ShowDistribution();
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
	LoginOut();
	WriteShareMemoryLoginInfo(); // 退出登录时更新共享内存
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


void CMainFrame::OnTaskRecent()
{
	// TODO: 在此添加命令处理程序代码

}


void CMainFrame::OnUpdateTaskRecent(CCmdUI *pCmdUI)
{
	return;
	// TODO: 在此添加命令更新用户界面处理程序代码
	//BOOL CDlgStart::LoadHistoryList( void )
	{		
		CMenu* pMenu = CMenu::FromHandle(m_wndMenuBar.GetDefaultMenu());
		pMenu->GetSubMenu(1)->DeleteMenu(0, MF_BYPOSITION);
		// 		pMenu->GetSubMenu(0)->DeleteMenu(0, MF_BYPOSITION);
		// 		pMenu->GetSubMenu(0)->DeleteMenu(0, MF_BYPOSITION);
		// 		pMenu->GetSubMenu(0)->DeleteMenu(0, MF_BYPOSITION);
		m_wndMenuBar.CreateFromMenu(pMenu->GetSafeHmenu(), TRUE, TRUE);
		return;

		//		CMenu* pMenu = CMenu::FromHandle(m_wndMenuBar.GetDefaultMenu())->GetSubMenu(0);
		pMenu->AppendMenuA(MF_POPUP, 0, "test");
		m_wndMenuBar.CreateFromMenu(pMenu->GetSafeHmenu(), TRUE, TRUE);
		return;
		if(!pMenu) return;

		CXmlHistoryModel m_xhm;
		m_xhm.Load();

		for ( int i=0; i<(int)m_xhm.m_vDatas.size(); i++ )
		{
			// 			pRow->GetItem (COL_NAME)->SetValue ( (LPCTSTR)m_xhm.m_vDatas[i]->m_strName );
			// 			pRow->GetItem (COL_PATH)->SetValue ( (LPCTSTR)m_xhm.m_vDatas[i]->m_strPath );
			// 
			// 			pBtn = new CCxGridButtonItem (_T("打开"), ID_GRID_BUTTON_FIRST);
			// 			pRow->ReplaceItem (COL_OP, pBtn);

			// 			CString& str = m_xhm.m_vDatas[i]->m_strPath;
			// 			pMenu->AppendMenu(MF_STRING|MF_ENABLED, 11, str);

			CMenu menu;
			menu.CreatePopupMenu(); GetMenu()->AppendMenu(MF_POPUP,(UINT)menu.m_hMenu,"test");
			menu.Detach(); 
			break;

		}

		m_wndMenuBar.CreateFromMenu(pMenu->GetSafeHmenu(), TRUE, TRUE);

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


void CMainFrame::OnMenuHistoryRecent(UINT nID)
{
	CHistoryModelData* pData = m_xhm.m_vDatas[ nID - IDM_HISTORY_MODEL_START ];
	if (pData != NULL)
	{
		CString sFilePath = pData->m_strPath;
		//打开前先关闭当前设计过程
		if (!theApp.m_processMgr.AskIsSave())
		{
			return;
		}
		theApp.m_processMgr.Close();
		//打开设计过程
		if (!theApp.m_processMgr.Open(sFilePath))
		{
			CString sMsg;
			sMsg.Format("fail to open %s", sFilePath);
			::MessageBox(theApp.m_pMainWnd->GetSafeHwnd(), sMsg, g_lpszAppTitle, MB_OK|MB_ICONERROR);
			return;
		}

		theApp.m_processMgr.SetModified(FALSE);
	}
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


void CMainFrame::OnTaskPublish()
{
	// TODO: 在此添加命令处理程序代码
	theApp.m_processMgr.Publish();
}


void CMainFrame::OnBarTree()
{
	// TODO: 在此添加命令处理程序代码
	BOOL b = theApp.m_pBarJianmoTree->IsVisible();
	if(b)
		theApp.m_pBarJianmoTree->ShowControlBar(FALSE, FALSE, FALSE);
	else
		theApp.m_pBarJianmoTree->ShowControlBar(TRUE, FALSE, TRUE);
}


void CMainFrame::OnUpdateBarTree(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	BOOL b = theApp.m_pBarJianmoTree->IsVisible();
	if(b)
		pCmdUI->SetCheck(TRUE);  
	else
		pCmdUI->SetCheck(FALSE);  
}


void CMainFrame::OnRes()
{
	// TODO: 在此添加命令处理程序代码
	BOOL b = theApp.m_pBarJianmoComponentLib->IsVisible();
	if(b)
		theApp.m_pBarJianmoComponentLib->ShowControlBar(FALSE, FALSE, FALSE);
	else
		theApp.m_pBarJianmoComponentLib->ShowControlBar(TRUE, FALSE, TRUE);
}


void CMainFrame::OnUpdateRes(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	BOOL b =  theApp.m_pBarJianmoComponentLib->IsVisible();
	if(b)
		pCmdUI->SetCheck(TRUE);  
	else
		pCmdUI->SetCheck(FALSE);  
}

BOOL CMainFrame::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	if (pCopyDataStruct == NULL)
	{
		return __super::OnCopyData(pWnd, pCopyDataStruct);
	}

	switch ( pCopyDataStruct->dwData )
	{
	case CPMSG_WEBEVENT:
		{
			ZTools::WriteZToolsLog( "CMainFrame::OnCopyData CPMSG_WEBEVENT" );
			if ( !theCommHost.Receive( (LPCTSTR)pCopyDataStruct->lpData ) )
			{
				ZTools::WriteZToolsLog( "CMainFrame::OnCopyData 解析失败"  );
				return FALSE;
			}
		}
		break;

	//case CPMSG_WEBPAGE_getClientSubFlowId:
	//	{
	//		theApp.m_processMgr.m_sFlowId = (LPCTSTR)pCopyDataStruct->lpData == NULL ? "" : (LPCTSTR)pCopyDataStruct->lpData;
	//		theApp.m_bGetClientSubFlowId = TRUE;
	//		break;
	//	}

	case CPMSG_WORKSPACE_OPENWS:
		{
			CString sCommandLine = (LPCTSTR)pCopyDataStruct->lpData;
			if ( theApp.m_CmdLine.Parse( sCommandLine ) )
			{
				CString sOpenPath = theApp.m_CmdLine.GetValue( NULL );
				if ( PathFileExists( sOpenPath ) )
				{
					if( !theApp.m_processMgr.AskIsSave() )
					{
						break;
					}
					theApp.m_processMgr.Close();
					if( !theApp.m_processMgr.Open( sOpenPath ) )
					{
						break;
					}
					theApp.m_processMgr.SetModified(FALSE);
				}
			}
		}break;
	case CPMSG_WORKSPACE_MAKESUREINFO:
		{
			if( theApp.m_processMgr.GetProcessStateNeedSave() )
			{
				CSharedMemory Mem;
				CString sMemData = g_lpszDoubleOpenWsMemStr;
				int nLength = sMemData.GetLength() + MAX_PATH;
				Mem.Init( g_lpszDoubleOpenWsMemName, nLength );
				memset( Mem.GetData(), 0, nLength);
				memcpy( Mem.GetData(), (LPCTSTR)sMemData, sMemData.GetLength() + 1);
			}
		}break;
	case CPMSG_AW_PROCESS_ATTACHING:
		{
			ZTools::WriteZToolsLog( "CMainFrame::OnCopyData CPMSG_AW_PROCESS_ATTACHING" );
			OnCpMsgAwProcessAttaching( (LPCTSTR)pCopyDataStruct->lpData );
		}
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

	m_wndLoginBar.MoveToRight();

}


LRESULT CMainFrame::OnAwProcessAttaching( WPARAM wParam, LPARAM lParam )
{
	//CCxWebPageEvent * pEvent = (CCxWebPageEvent *)wParam;
	//if ( pEvent == NULL ) return 0;

	//CString sTabTitle;
	//sTabTitle.Format("运行实例:%s", theApp.m_processMgr.m_sFlowId);

	//if ( !IsWindow( pEvent->m_hWndWebPage ) )
	//{
	//	::MessageBox( GetMainWnd(),"WebPage主窗口不存在", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
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

	//theViewMgr.NewView( sTabTitle, pEvent->m_hWndWebPage );


	ZTools::WriteZToolsLog( "CMainFrame::OnAwProcessAttaching begin" );
	CAttachWindow* pAttachWindow = (CAttachWindow*)wParam;
	if ( pAttachWindow == NULL )
	{
		ZTools::WriteZToolsLog( "CMainFrame::OnAwProcessAttaching 参数无效" );
		delete g_pawRunProcess; g_pawRunProcess = NULL;
		theApp.m_processMgr.ClearRunVar();
		return 0;
	}
	
	if ( pAttachWindow->m_nExeFlag != WEBTYPE_RUNPROCESS )
	{
		ZTools::WriteZToolsLog( "CMainFrame::OnAwProcessAttaching 类型不匹配 WEBTYPE_RUNPROCESS" );
		delete pAttachWindow; pAttachWindow = NULL;
		delete g_pawRunProcess; g_pawRunProcess = NULL;
		theApp.m_processMgr.ClearRunVar();
		return 0;
	
	}
	
	if ( g_pawRunProcess == NULL )
	{
		ZTools::WriteZToolsLog( "CMainFrame::OnAwProcessAttaching g_pawRunProcess为空" );
		delete pAttachWindow; pAttachWindow = NULL;
		theApp.m_processMgr.ClearRunVar();
		return 0;
	}
	if ( pAttachWindow->m_sId.CompareNoCase( g_pawRunProcess->m_sId ) != 0 )
	{
		ZTools::WriteZToolsLog( "CMainFrame::OnAwProcessAttaching 请求附加的 WebContainer 不是同一个" );
		delete pAttachWindow; pAttachWindow = NULL;
		delete g_pawRunProcess; g_pawRunProcess = NULL;
		theApp.m_processMgr.ClearRunVar();
		return 0;
	}
	if ( !IsWindow( pAttachWindow->m_hClientWnd ) )
	{
		::MessageBox( GetMainWnd(),"WebPage主窗口不存在", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		delete pAttachWindow; pAttachWindow = NULL;
		delete g_pawRunProcess; g_pawRunProcess = NULL;
		theApp.m_processMgr.ClearRunVar();
		return 0;
	}
	CString sTabTitle;
	sTabTitle.Format("运行实例:%s", theApp.m_processMgr.m_sFlowId);
	if ( !pAttachWindow->m_sExData.IsEmpty() )
	{
		CAttachWindowExData exData;
		if ( !exData.Parse( pAttachWindow->m_sExData ) )
		{
			ZTools::WriteZToolsLog( "CMainFrame::OnAwProcessAttaching 解析数据失败" );
			delete pAttachWindow; pAttachWindow = NULL;
			delete g_pawRunProcess; g_pawRunProcess = NULL;
			theApp.m_processMgr.ClearRunVar();
			return 0;
		}
		CString sUrl = exData.m_sUrl;
		CString sFlowId = sUrl.Right(sUrl.GetLength() - sUrl.Find("processOid=") - 11);
		if (sFlowId.Find("&") != -1)
		{
			sFlowId = sFlowId.Left(sFlowId.Find("&"));
		}
		if (!sFlowId.IsEmpty())
		{
			sTabTitle.Format("运行实例:%s", sFlowId);
		}
	}
	HWND hWebClient = pAttachWindow->m_hClientWnd;
	g_pawRunProcess->m_hClientWnd = pAttachWindow->m_hClientWnd;
	g_pawRunProcess->m_dwClientProcessId = pAttachWindow->m_dwClientProcessId;
	delete pAttachWindow; pAttachWindow = NULL;

	theViewMgr.NewView( sTabTitle, g_pawRunProcess->m_hClientWnd );
	
	//m_hwndWebPage = pEvent->m_hWndWebPage;
	//if ( !IsWindow( m_hwndWebPage ) )
	//{
	//	m_hwndWebPage = NULL; return 0;
	//}
	ZTools::WriteZToolsFormatLog( "CMainFrame::OnAwProcessAttaching \r\n %s", "AttachWindow end" );

	//m_dwExStyleWebPage = ::GetWindowLong(m_hwndWebPage, GWL_EXSTYLE);
	//m_dwStyleWebPage = ::GetWindowLong(m_hwndWebPage, GWL_STYLE);

	//AttachWindow( this->GetSafeHwnd(), m_hwndWebPage, FALSE );
	//ZTools::WriteZToolsFormatLog( "CDlgViewRunFlow::OnAwProcessAttaching \r\n %s", "AttachWindow end" );

	//CRect rcClient;
	//GetClientRect(&rcClient);
	////rcClient.bottom -= 38;

	//::MoveWindow(m_hwndWebPage, 0, 0
	//	, rcClient.Width(), rcClient.Height(), TRUE);

	//::ShowWindow( m_hwndWebPage, SW_SHOW );

	::PostMessage( hWebClient, WM_AW_PROCESS_ATTACHED, 0, 0 );
	ZTools::WriteZToolsFormatLog( "CMainFrame::OnAwProcessAttaching \r\n %s", "PostMessage WM_AW_PROCESS_ATTACHED end" );

	return 0;
}


LRESULT CMainFrame::OnThemeChanged()
{
	// 该功能要求使用 Windows XP 或更高版本。
	// 符号 _WIN32_WINNT 必须 >= 0x0501。
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	theApp.LoadDefaultFont();

	//有时候结构树标题栏下边有条边线不绘制问题
	if ( theApp.m_pBarJianmoTree != NULL 
		&& IsWindow( theApp.m_pBarJianmoTree->GetSafeHwnd() ) )
	{
		::PostMessage( theApp.m_pBarJianmoTree->GetSafeHwnd(), WM_NCPAINT, 0, 0 );
	}

	return 1;
}


void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	__super::OnActivate(nState, pWndOther, bMinimized);

	//有时候结构树标题栏下边有条边线不绘制问题
	if ( theApp.m_pBarJianmoTree != NULL 
		&& IsWindow( theApp.m_pBarJianmoTree->GetSafeHwnd() ) )
	{
		::PostMessage( theApp.m_pBarJianmoTree->GetSafeHwnd(), WM_NCPAINT, 0, 0 );
	}

	if ( 
		nState == 0 
		&& theApp.m_pBarJianmoTree != NULL 
		&& IsWindow( theApp.m_pBarJianmoTree->GetSafeHwnd() )
		)
	{
		theApp.m_pBarJianmoTree->m_wndGridTree.m_ToolTip2.Deactivate ();
	}

}


void CMainFrame::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
	__super::OnActivateApp(bActive, dwThreadID);

	//有时候结构树标题栏下边有条边线不绘制问题
	if ( theApp.m_pBarJianmoTree != NULL 
		&& IsWindow( theApp.m_pBarJianmoTree->GetSafeHwnd() ) )
	{
		::PostMessage( theApp.m_pBarJianmoTree->GetSafeHwnd(), WM_NCPAINT, 0, 0 );
	}
}


void CMainFrame::OnCaptureChanged(CWnd *pWnd)
{
	//有时候结构树标题栏下边有条边线不绘制问题
	if ( theApp.m_pBarJianmoTree != NULL 
		&& IsWindow( theApp.m_pBarJianmoTree->GetSafeHwnd() ) )
	{
		::PostMessage( theApp.m_pBarJianmoTree->GetSafeHwnd(), WM_NCPAINT, 0, 0 );
	}

	__super::OnCaptureChanged(pWnd);
}


void CMainFrame::OnChangeUIState(UINT nAction, UINT nUIElement)
{
	// 该功能要求使用 Windows 2000 或更高版本。
	// 符号 _WIN32_WINNT 和 WINVER 必须 >= 0x0500。
	__super::OnChangeUIState(nAction, nUIElement);

	//有时候结构树标题栏下边有条边线不绘制问题
	if ( theApp.m_pBarJianmoTree != NULL 
		&& IsWindow( theApp.m_pBarJianmoTree->GetSafeHwnd() ) )
	{
		::PostMessage( theApp.m_pBarJianmoTree->GetSafeHwnd(), WM_NCPAINT, 0, 0 );
	}
}

LRESULT CMainFrame::OnUpdateComponentByLinker (WPARAM wParam, LPARAM lParam)
{
	g_strLinkerToNodeId = "";
	CString* pStr = (CString*)lParam;
	if ( pStr == NULL ) return 0;
	g_strLinkerToNodeId = *pStr;
	delete pStr; pStr = NULL;
	return 0;
}

LRESULT CMainFrame::OnWebLoaded(WPARAM, LPARAM)
{
	CRunFlowExec eExec;
	eExec.m_strFlowId = theApp.m_processMgr.m_sFlowId;
	CString strResult = InvokeWsMethod( eExec.GetMethodName(), eExec.GetMethodParamStr() );

	if (theApp.m_processMgr.m_bProcessStarting)
	{
		theApp.m_processMgr.ClearRunVar();
	}
	return 0;
}

LRESULT CMainFrame::OnLoginbarClickedLogout( WPARAM wParam, LPARAM lParam )
{
	LoginOut();
	WriteShareMemoryLoginInfo(); // 退出登录时更新共享内存
	m_wndLoginBar.RedrawWindow();
	return TRUE;
}
LRESULT CMainFrame::OnUpdateMainTitle(WPARAM, LPARAM)
{
	CString strTitle = g_lpszAppTitle;
	CCxStructTreeNodeFlow * pRootNode = theApp.m_processMgr.GetTreeRoot();
	if (pRootNode && !pRootNode->m_strText.IsEmpty())
	{
		strTitle += " - ";
		strTitle += pRootNode->m_strText;
	}
	if (!theApp.m_processMgr.m_sProcessFilePath.IsEmpty())
	{
		strTitle +="(";
		strTitle += theApp.m_processMgr.m_sProcessFilePath;
		strTitle +=")";
	}
	if (pRootNode && pRootNode->m_strText.IsEmpty() && !theApp.m_processMgr.m_sProcessFilePath.IsEmpty())
		return S_OK; // 切换设计过程文件时，根节点为空，文件路径不为空，且换完成后，过滤切换过程中的设置标题消息

	SetWindowText(strTitle);
	return S_OK;
}

LRESULT CMainFrame::OnStructTreeRefresh( WPARAM wParam, LPARAM lParam )
{
	CCxStructTreeNode* pNode = (CCxStructTreeNode*)wParam;
	if ( pNode == NULL )
	{
		return 0;
	}
	OnUpdateDataMapping( NULL, NULL );
	theApp.m_pBarJianmoTree->m_wndGridTree.AddNode( pNode, NULL, -1, FALSE );
	theApp.m_pBarJianmoTree->m_wndGridTree.AdjustLayout();
	theApp.m_pBarJianmoTree->m_wndGridTree.m_StructTree.SetSelNode( pNode );
	theApp.m_pBarJianmoTree->m_wndGridTree.m_StructTree.OnNodeSelected(TRUE);
	theApp.m_pBarJianmoTree->m_wndGridTree.m_StructTree.UpdateGridAllRowStatus();
	OnUpdateMainTitle( NULL, NULL );
	return 0;
}

BOOL CMainFrame::OnCpMsgAwProcessAttaching( LPCTSTR lpszData )
{
	ZTools::WriteZToolsLog( "CMainFrame::OnCpMsgAwProcessAttaching begin" );
	if ( lpszData == NULL )
	{
		ZTools::WriteZToolsLog( "CMainFrame::OnCpMsgAwProcessAttaching 参数为空" );
		return FALSE;
	}
	CAttachWindow* pAw = new CAttachWindow();
	if ( !pAw->Deserialize( lpszData ) )
	{
		delete pAw; pAw = NULL;
		ZTools::WriteZToolsLog( "CMainFrame::OnCpMsgAwProcessAttaching 解析失败" );
		return FALSE;
	}
	HWND hWnd = NULL;
	switch( pAw->m_nExeFlag )
	{
	case WEBTYPE_RUNFLOW_VIEW:
		hWnd = theApp.m_hwndViewRunFlow;
		break;
	case WEBTYPE_RUNFLOW:
		hWnd = theApp.m_hwndRunFlow;
		break;
	case WEBTYPE_RUNPROCESS:
		hWnd = GetSafeHwnd();
		break;
	default:
		ZTools::WriteZToolsFormatLog( "CMainFrame::OnCpMsgAwProcessAttaching 进程类型无效[%d]", pAw->m_nExeFlag );
		break;
	}
	if ( !IsWindow( hWnd ) )
	{
		delete pAw; pAw = NULL;
		ZTools::WriteZToolsLog( "CMainFrame::OnCpMsgAwProcessAttaching  指定句柄为空" );
		return FALSE;
	}
	::PostMessage( hWnd, WM_AW_PROCESS_ATTACHING, (WPARAM)pAw, 0 );
	ZTools::WriteZToolsLog( "CMainFrame::OnCpMsgAwProcessAttaching  end" );
	return TRUE;
}

LRESULT CMainFrame::OnWebEventAsync( WPARAM wParam, LPARAM lParam )
{
	ZTools::WriteZToolsLog( "CMainFrame::OnWebEventAsync begin" );
	CWebEventData* pWebEventData = (CWebEventData*)wParam;
	if ( pWebEventData == NULL )
	{
		ZTools::WriteZToolsLog( "CMainFrame::OnWebEventAsync 参数为空" );
		return 0;
	}
	if ( !theCommHost.Handle( pWebEventData ) )
	{
		ZTools::WriteZToolsLog( "CMainFrame::OnWebEventAsync Handle Failed" );
	}
	delete pWebEventData; pWebEventData = NULL;
	ZTools::WriteZToolsLog( "CMainFrame::OnWebEventAsync  end" );
	return 0;
}


void CMainFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CPoint ptScreen = point;
	ClientToScreen(&ptScreen);
	CWnd * pWnd = CWnd::WindowFromPoint(ptScreen);
	if (pWnd)
	{
		DWORD dwProcessId = 0;
		GetWindowThreadProcessId(pWnd->GetSafeHwnd(), &dwProcessId); //取得当前窗口对应的进程id
		DWORD dwCurId = GetCurrentProcessId();
		if (dwProcessId != dwCurId )
			return;
	}

	__super::OnLButtonDown(nFlags, point);
}
