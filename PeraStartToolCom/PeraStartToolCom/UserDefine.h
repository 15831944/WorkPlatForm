#pragma once
#include <vector>
#include <map>
using namespace std;

//控件标志
enum {
	CF_BAR_JIANMO_STRUCTTREE = 1,
	CF_BAR_JIANMO_RESLIB,
	CF_BAR_MAIN_TOOLBAR,
	CF_BAR_MAIN_ROBOTBAR
};

//菜单控件ID
enum {
	//任毅的本地资源库菜单//////////
	ID_TREE_MENU_ADD = 40001,
	ID_TREE_MENU_DEL,     
	ID_TREE_MENU_EDIT,    
	ID_TREE_MENU_SEND
};

//对话框控件ID
enum {
	//N维数组编辑对话框////////////////
	ID_NDARRAY_VIEW = 41001,
	ID_NAE_ARRAY_DIM,

	//MainFrame toolbar
	IDC_MAIN_TOOLBAR,
	IDC_MAIN_ROBOTBAR,
	IDC_MAIN_LOGINBAR,
	IDC_MAIN_MENUBAR,

	//
	IDC_LOGINBAR_USER,
	IDC_LOGINBAR_LOGIN,

	//
	IDC_RESLIBIMGLIST_DUMMY
};

//////////////////////////////////////////////////////////////////////////
enum {
	IDM_TOOLBAR_BUTTON_FIRST = 45001,
	IDM_TOOLBAR_BUTTON_LAST = (IDM_TOOLBAR_BUTTON_FIRST + 256),

	ID_GRID_BUTTON_FIRST = (IDM_TOOLBAR_BUTTON_LAST + 1),
	ID_GRID_BUTTON_LAST = (ID_GRID_BUTTON_FIRST + 256),

	IDM_ROBOTBAR_BUTTON_FIRST = (ID_GRID_BUTTON_LAST + 1),
	IDM_ROBOTBAR_BUTTON_LAST = (IDM_ROBOTBAR_BUTTON_FIRST + 256),

	IDM_HISTORY_MODEL_START = (IDM_ROBOTBAR_BUTTON_LAST + 1),
	IDM_HISTORY_MODEL_LAST = (IDM_HISTORY_MODEL_START + 256),

	IDM_VIEW_START = (IDM_HISTORY_MODEL_LAST + 1),
	IDM_VIEW_LAST = (IDM_VIEW_START + 256)
};



//消息///////////////////////////////////////////////////////////

enum {

	/************************************************************************
	本地资源树上，当节点选择改变时，发送本消息，刷新图片列表。
	*/
	WM_RESLIB_NOTIFY_SELCHANGED = (WM_USER + 1),
	
	/************************************************************************
	功能：
		本地资源库-图标列表中，当在图标上点击删除时，给资源树发送此消息，删除本地资源。
	参数：
		wp：LPARAM * 数组。
		lp：数组元素个数。
	说明：
		接收方删除数据指针。
	*/
	WM_RESLIB_NOTIFY_DELETE,

	/************************************************************************
	功能：
		本地资源库-图标列表中，当在图标上点击更新属性时，给资源树发送此消息，更新本地资源库。
	参数：
		wp：LPARAM 元素对象。
		lp：0。
	*/
	WM_RESLIB_NOTIFY_UPDATE_PROP,

	/************************************************************************
	功能：
		本消息用于更新结构树中的数据映射信息。
	参数：
		wp：选中的节点指针
		lp：0
	说明：
		当js调用updatestructtree时，给主窗口PostMessage本消息，避免js与PeraProcessDesigner阻塞调用。
	*/
	WM_UPDATE_DATAMAPPING,

	/************************************************************************
	功能：
		本消息用于在结构树上，调用js方法运行组件。
	参数：
		wp：0
		lp：0
	说明：
		当需要在结构树上运行组件时，通过给结构树Post本消息，实现，目的是防止js与PeraProcessDesigner发生阻塞调用。
	*/
	WM_STRUCTTREE_RUN_COMPNENT,

	/************************************************************************
	功能：
		本消息用于异步运行设计过程。
	参数：
		wp：0
		lp：0
	说明：
	*/
	WM_STRUCTTREE_RUN_PROCESS,
	
	/************************************************************************
	功能：
		本消息用于异步运行子流程。
	参数：
		wp：0
		lp：0
	说明：
	*/
	WM_STRUCTTREE_RUN_SUB_PROCESS,
	
	/************************************************************************
	功能：
		本消息用于异步另存为子流程。
	参数：
		wp：0
		lp：0
	说明：
	*/
	WM_STRUCTTREE_SAVEAS_SUB_PROCESS,
	
	/************************************************************************
	功能：
		本消息用于从SVG上更新组件、设计过程的属性信息。
	参数：
		wp：节点对象指针
		lp：0
	说明：
		当需要在SVG上更新属性时，给主窗口发送本消息，由主窗口调用结构树方法弹出属性编辑界面。
		编辑后，调用js的propUpdate方法，回填数据。
		目的1：防止js与PeraProcessDesigner发生阻塞调用。
		目的2：保持更新属性的窗口一致性。
		由于这个消息传递了全局的指针对象，所以要确保在处理本消息时，节点对象指针有效，否则会出现异常。
	*/
	WM_STRUCTTREE_UPDATE_PROP,
	
	/************************************************************************
	功能：
		编辑数组时，更新参数值 （删除以前对应元素的行，添加新元素对应的行）。
	参数：
		wp：当前参数节点对象
		lp：0
	*/
	WM_STRUCTTREE_UPDATE_PARAM_ARRAY,
	
	/************************************************************************
	功能：
		本消息用于从SVG上显示/不显示属性信息。
	参数：
		wp：节点对象指针
		lp：BOOL值，TRUE：显示；FALSE：不显示
	说明：
		当需要在SVG上更新属性时，给主窗口发送本消息，由主窗口调用结构树方法显示。
		由于这个消息传递了全局的指针对象，所以要确保在处理本消息时，节点对象指针有效，否则会出现异常。
	*/
	WM_STRUCTTREE_SHOW_TOOLTIP,

	/************************************************************************
	功能：
		显示数据映射对话框。
	参数：
		wp：0
		lp：0
	*/
	WM_STRUCTTREE_SHOW_DATAMAPPING,

	/************************************************************************
	功能：
		分支设置对话框。
	参数：
		wp：Json字符串
		lp：0
	说明：
	    当需要在SVG上编辑条件分支，给主窗口发送本消息，由主窗口调用结构树方法显示。
	*/
	WM_STRUCTTREE_EDIT_IF,

	/************************************************************************
	功能：
		编辑根节点参数。
	参数：
		wp：Json字符串
		lp：0
	说明：
	    当需要在SVG上编辑根节点参数，给主窗口发送本消息，由主窗口调用结构树方法显示。
	*/
	WM_STRUCTTREE_EDIT_ROOTPARAMS,

	/************************************************************************
	功能：
		循环设置。
	参数：
		wp：0
		lp：0
	说明：
	    当需要在SVG上设置循环，给主窗口发送本消息，由主窗口调用结构树方法显示。
	*/
	WM_STRUCTTREE_EDIT_FOR,

	/************************************************************************
	功能：
		编辑数据映射表达式对话框。
	参数：
		wp：Json字符串
		lp：0
	说明：
	    当需要在SVG上编辑数据映射表达式，给主窗口发送本消息，由主窗口调用结构树方法显示。
	*/
	WM_STRUCTTREE_EDIT_DATAMAPPING_EXP,

	/************************************************************************
	功能：
		显示svg拖动层窗口。
	参数：
		wp：TRUE/FALSE
		lp：序号
	说明：
	    显示/取消。
	*/
	WM_SHOW_SVG_DRAG_LAYER,
	/************************************************************************
	功能：
		当本地数据准备好后，Post给MDI本消息。
	参数：
		wp：0
		lp：0
	*/
	WM_UPDATE_RESLIB_TREE,

	/************************************************************************
	登录对话框关闭按钮鼠标悬停事件
	*/
	WM_LOGIN_CLOSEBUTTON_MOUSEOVER,
	
	/************************************************************************
	登录对话框关闭按钮鼠标悬停事件
	*/
	WM_LOGIN_CLOSEBUTTON_MOUSELEAVE,
	
	/************************************************************************
	显示/隐藏进度对话框
	*/
	WM_PROGRESS_SHOW,

	/************************************************************************
	弹出子流程运行模态对话框
	*/
	WM_SHOW_RUNFLOW_DIALOG,

	/************************************************************************
	子流程运行完成
	*/
	WM_RUNFLOW_COMPLETED,

	/************************************************************************
	绑定webpage
	*/
	WM_ATTACH_WEBPAGE,

	/************************************************************************
	在作业管理中，查看子流程
	*/
	WM_VIEW_RUNFLOW_WEBPAGE,
	/************************************************************************
	下载操作消息
	*/
	WM_RESLIB_NOTIFY_DOWNLOAD,
	/************************************************************************
	更新dms数据
	*/
	WM_STRUCTTREE_UPDATE_DMS_DATA,
	/************************************************************************
	SVG弹出右键菜单,wParam是CCxStructTreePopMenuData，需要手动释放
	*/
	WM_POP_MENU,

	/************************************************************************
	功能：
		更新组件时，标识是数据映射更新
	参数：
		wp：0
		lp：To节点Id字符串指针
	*/
	WM_UPDATE_COMPONENT_BY_LINKER,
	WM_USER_SORTMSG,

	/************************************************************************
	login bar clicked 
	*/
	WM_LOGINBAR_CLICKED_LOGOUT,
	/**
	编辑框 CPeraAutoCheckEdit 使用SendMessage 发送消息到字符串校验窗口
	*/
	WM_EDIT_AUTOCHECK,
	/*
	if添加表达式编辑器对话框上结构树鼠标左键双击消息
	*/
	WM_STRUCTTREEE_LDLK, 
	/*
	if添加表达式编辑器对话框上结构树失去焦点消息
	*/
	WM_STRUCTTREEE_LEAVEFOCUS
};

//CopyData 消息标识
enum{
	/*
	功能：
		WebPage
	*/
	CPMSG_WEBPAGE = 1
};

//ViewMg中，view类别
extern const LPCTSTR viewflagModel;
extern const LPCTSTR viewflagRunProcess;
extern const LPCTSTR viewflagHistoryMonitor;
extern const LPCTSTR viewflagDistribution;
extern const LPCTSTR viewflagWebPage;
extern const LPCTSTR viewflagWelcome;

//MDI主标题
extern const LPCTSTR g_lpszAppTitle;
//View-建模环境名称
extern const LPCTSTR g_lpszViewTitleModel;
//View-管理监控名称
extern const LPCTSTR g_lpszViewTitleHistoryMonitor;
//View-分布式资源管理名称
extern const LPCTSTR g_lpszViewTitleDistribution;

//TabAera底色
extern const COLORREF g_crTabBarBg;
extern const COLORREF g_crTabBarBg2;
//TabAera最左侧竖边线
extern const COLORREF g_crTabBarLeftLine;

//工具栏上边线
extern const COLORREF g_crToolbarTop;
extern const COLORREF g_crToolbarTop2;

//工具栏、Robot栏底色
extern const COLORREF g_crToolbarBg;
//splitter底色
extern const COLORREF g_crSplitterBg;

//结构树bar等外边框、内边框颜色
extern const COLORREF g_crBarBorderOut;
extern const COLORREF g_crBarBorderIn;
extern const COLORREF g_crBarCaptionBottom;
//结构树bar标题栏渐变色
extern const COLORREF g_crBarCaptionStart;
extern const COLORREF g_crBarCaptionEnd;

//结构树////////////////////
//表头底色
extern const COLORREF g_crStructTreeHeaderBg;
//表头上边线
extern const COLORREF g_crStructTreeHeaderTop;
//表头下边线
extern const COLORREF g_crStructTreeHeaderBottom;
//客户区域底色
extern const COLORREF g_crStructTreeClientBg;
//选中的单元格底色
extern const COLORREF g_crStructTreeSelItemBg;
//文字颜色-正常
extern const COLORREF g_crStructTreeText;
//文字颜色-不可修改
extern const COLORREF g_crStructTreeTextInvalid;
//文字颜色-可编辑
extern const COLORREF g_crStructTreeTextCanEdit;
//每个Item图标的透明色
extern const COLORREF g_crStructTreeImgBg;
//树线
extern const COLORREF g_crStructTreeTreeLine;

//组件资源库/////////////////////
//Tree底色
extern const COLORREF g_crResLibTreeBg;
//Image底色
extern const COLORREF g_crResLibImageBg;

//test
extern const COLORREF g_crTestBoder;
extern const COLORREF g_crTestBg;

//数组编辑//////////////////////
//表头分割线
extern const COLORREF g_crNDArrayHeaderBorder;

// 编辑框字符串校验不通过时背景颜色
extern const COLORREF g_crEditErrBkColor;

//////////////////////////////////////////////////////////////////////////

//Tooltip延时
extern const DWORD g_dwToolTipDelay;
//Tooltip左上角 相对于鼠标的偏移
extern const CPoint g_ToolTipOffset;