#pragma once
#include <vector>
#include <map>
using namespace std;

//控件ID
enum {
	IDC_MAIN_TOOLBAR = 41001,
	IDC_MAIN_LOGINBAR,
	IDC_MAIN_MENUBAR,

	//
	IDC_LOGINBAR_USER,
	IDC_LOGINBAR_LOGIN
};

//////////////////////////////////////////////////////////////////////////
enum {
	IDM_TOOLBAR_BUTTON_FIRST = 45001,
	IDM_TOOLBAR_BUTTON_LAST = (IDM_TOOLBAR_BUTTON_FIRST + 256),

	IDM_VIEW_START = (IDM_TOOLBAR_BUTTON_LAST + 1),
	IDM_VIEW_LAST = (IDM_VIEW_START + 256)
};

//消息///////////////////////////////////////////////////////////

enum {
	WM_DUMMY = (WM_USER + 1),
	
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
	绑定webpage
	*/
	WM_ATTACH_WEBPAGE,

	/************************************************************************
	login bar clicked 
	*/
	WM_LOGINBAR_CLICKED
};

//CopyData 消息标识
enum{
	/*
	功能：
		WebPage
	*/
	CPMSG_WEBPAGE = 1,
	/*
	功能：
		通过PeraDC协议，启动客户端
	*/
	CPMSG_RUN_PERADESKCLIENT_BY_PERADC = 2,
	/*
	功能：
		注销消息，发送到PeraDCWebContainer
	*/
	CPMSG_LOGIN_OUT = 3,
	/*
	功能：
		刷新消息，发送到PeraDCWebContainer
	*/
	CPMSG_REFRESH = 4,
	/*
	功能：
		登录已经打开的客户端，并跳转到指定页面
	*/
	CPMSG_LOGIN_DESKCLIENT = 5
};

#define TIMER_LOADING_WINDOW						2

//ViewMg中，view类别
extern const LPCTSTR viewflagTaskModel;


//MDI主标题
extern const LPCTSTR g_lpszAppTitle;
//View-建模环境名称
extern const LPCTSTR g_lpszViewTitleTaskModel;

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

//////////////////////////////////////////////////////////////////////////

//Tooltip延时
extern const DWORD g_dwToolTipDelay;
//Tooltip左上角 相对于鼠标的偏移
extern const CPoint g_ToolTipOffset;