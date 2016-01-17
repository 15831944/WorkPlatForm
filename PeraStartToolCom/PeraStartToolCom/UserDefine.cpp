#include "StdAfx.h"
#include "UserDefine.h"
#include "FileFunc.h"

const LPCTSTR viewflagModel = _T("建模环境");
const LPCTSTR viewflagRunProcess = _T("运行设计过程");
const LPCTSTR viewflagHistoryMonitor = _T("历史监控");
const LPCTSTR viewflagDistribution = _T("分布式资源管理");
const LPCTSTR viewflagWebPage = _T("WebPage");
const LPCTSTR viewflagWelcome = _T("Welcome");

//////////////////////////////////////////////////////////////////////////

const LPCTSTR g_lpszAppTitle = _T("综合设计环境");
//View-建模环境名称
const LPCTSTR g_lpszViewTitleModel = _T("建模环境");
//View-管理监控名称
const LPCTSTR g_lpszViewTitleHistoryMonitor = _T("作业管理");
//View-分布式资源管理名称
const LPCTSTR g_lpszViewTitleDistribution = _T("分布式资源管理");


//////////////////////////////////////////////////////////////////////////

//TabAera底色
const COLORREF g_crTabBarBg = RGB( 0xd4, 0xd4, 0xd4 );
const COLORREF g_crTabBarBg2 = RGB( 0xc8, 0xc8, 0xc8 );
//TabAera最左侧竖边线
const COLORREF g_crTabBarLeftLine = RGB( 151, 151, 151 );

//工具栏上边线
const COLORREF g_crToolbarTop = RGB( 0x97, 0x97, 0x97 ); //RGB( 0xff, 0x0, 0x0 ); //
const COLORREF g_crToolbarTop2 = RGB( 0xff, 0xff, 0xff ); //RGB( 0x0, 0xff, 0x0 ); //

//工具栏、Robot栏底色
const COLORREF g_crToolbarBg = RGB( 0xe9, 0xe9, 0xe9 ); //RGB(0,255,0); //
//splitter底色
const COLORREF g_crSplitterBg = RGB( 0xc8, 0xc8, 0xc8 ); //RGB(0xff,0xff,0x0); //

//结构树bar等外边框、内边框颜色
const COLORREF g_crBarBorderOut = RGB( 0x77, 0x77, 0x77 ); //RGB(0xff,0x0,0x0); //
const COLORREF g_crBarBorderIn = RGB( 0xff, 0xff, 0xff ); //RGB(0xff,0xff,0x0); //
const COLORREF g_crBarCaptionBottom = RGB( 0xff, 0xff, 0xfe ); //RGB(0xff,0x0,0x0); //
//结构树bar标题栏渐变色
const COLORREF g_crBarCaptionStart = RGB( 0xee, 0xee, 0xee ); //RGB(0,210,0);//
const COLORREF g_crBarCaptionEnd = RGB( 0xdf, 0xdf, 0xdf ); //RGB(0,250,0); //

//结构树////////////////////
//表头底色
const COLORREF g_crStructTreeHeaderBg = RGB( 0xec, 0xec, 0xec );
//表头上边线
const COLORREF g_crStructTreeHeaderTop = g_crStructTreeHeaderBg;
//表头下边线
const COLORREF g_crStructTreeHeaderBottom = RGB( 0xcc, 0xcc, 0xcc );
//客户区域底色
const COLORREF g_crStructTreeClientBg = RGB( 0xfb, 0xfb, 0xfb ); //RGB(0xff, 0, 0); //
//选中的单元格底色
const COLORREF g_crStructTreeSelItemBg = RGB( 0xff, 0xe4, 0x91 ); //RGB(0xff, 0, 0); //
//文字颜色
const COLORREF g_crStructTreeText = RGB( 123, 123, 123 );
//文字颜色-不可修改
const COLORREF g_crStructTreeTextInvalid = RGB( 0x99, 0x99, 0x99 );
//文字颜色-可编辑
const COLORREF g_crStructTreeTextCanEdit = RGB( 10, 54, 143 );
//每个Item图标的透明色
const COLORREF g_crStructTreeImgBg = RGB( 0xff, 0xff, 0xff );
//树线
const COLORREF g_crStructTreeTreeLine = RGB( 128, 128, 128 );

//组件资源库/////////////////////
//Tree底色
const COLORREF g_crResLibTreeBg = g_crStructTreeClientBg;
//Image底色
const COLORREF g_crResLibImageBg = RGB( 0xff, 0xff, 0xff );

//test
const COLORREF g_crTestBoder = RGB( 255, 0, 0 );
const COLORREF g_crTestBg = RGB( 0, 0, 255 );

//数组编辑//////////////////////
//表头分割线
extern const COLORREF g_crNDArrayHeaderBorder = RGB( 125, 125, 125 );

extern const COLORREF g_crEditErrBkColor = RGB(255 , 255 ,0);
//////////////////////////////////////////////////////////////////////////

//Tooltip延时
const DWORD g_dwToolTipDelay = 500;
//Tooltip左上角 相对于鼠标的偏移
const CPoint g_ToolTipOffset(1, 20);