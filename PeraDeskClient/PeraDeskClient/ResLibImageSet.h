#pragma once
#include "StlHeader.h"

/*
使用说明：
在App初始化期间，添加 LoadResLibImageSet();
在App结束前，添加 ClearResLibImageSet();
*/

class CCxStaticRes
{
public:
	static const enum {
		RT_NULL = 0,
		RT_SCRIPT_DRIVER,
		RT_DATA_PARSER,
		RT_EXCEL,
		RT_TOOL_INTEGRATION,
		RT_WORD,
		RT_FORMULA,
		RT_FORM,
		RT_DOCUMENT,
		RT_PATRAN,
		RT_WORKBENCH,

		//static res
		RT_CUSTOM_ROBOT_42, //自定义robot
		RT_CUSTOM_ROBOT, //自定义robot
		RT_FLOW_42, //流程
		RT_FLOW_16, //流程
		RT_PC_16, //本地资源库 根节点
		RT_FOLDER_16, //本地资源库 文件夹
		RT_COMPONENT_16, //本地资源库 组件
		//拖拽图标
		RT_DRAG_DSTFORM,
		RT_DRAG_DST,
		RT_DRAG_HIGHLIGHT_FROM,
		RT_DRAG_HIGHLIGHT,

		//分支编辑对话框相关图标
		RT_IF_ARROW,	//焦点箭头
		RT_IF_ARROW_UP,	//焦点箭头
		RT_IF_ARROW_UP_GRAY,	//焦点箭头
		RT_IF_ARROW_DOWN,	//焦点箭头
		RT_IF_ARROW_DOWN_GRAY,	//焦点箭头

		//登陆对话框资源
		RT_DLG_LOGIN_BK_BK,	//登陆的背景窗口的背景
		RT_DLG_LOGIN_CLOSE_NOR,	//关闭的正常
		RT_DLG_LOGIN_CLOSE_OVER,	//关闭的滑过
		RT_DLG_LOGIN_MAIN_BK,	//登陆主窗的背景

		//进度对话框资源
		RT_DLG_PROGRESS,			//进度对话框用到的png模仿动画效果资源

		//参数设置对话框资源
		RT_DLG_PARAMSET			//
	};

	CCxStaticRes();
	~CCxStaticRes();
	void Clear(void);
public:
	int m_nCode;
	CString m_strId;
	CString m_strPath;
	Image * m_pImgNormal;
};

extern void ClearResLibImageSet(void);
extern void LoadResLibImageSet(void);
extern CString ConvertRobotCode2Id(int nCode);
extern int ConvertRobotId2Code(LPCTSTR lpszId);
extern CCxStaticRes * GetRobotImageData(LPCTSTR lpszId, int nSize);
extern CCxStaticRes * GetRobotImageData(int nCode, int nSize);
extern CCxStaticRes * GetStaticResImageData(LPCTSTR lpszId);
extern CCxStaticRes * GetStaticResImageData(int nCode);
extern CCxStaticRes * GetFlowIfResImageData(LPCTSTR lpszId);
extern CCxStaticRes * GetFlowIfResImageData(int nCode);
extern CCxStaticRes * GetDlgLoginResImageData(LPCTSTR lpszId);
extern CCxStaticRes * GetDlgLoginResImageData(int nCode);
extern CCxStaticRes * GetDlgProgressResImageData(LPCTSTR lpszId);
extern CCxStaticRes * GetDlgProgressResImageData(int nCode);
extern CCxStaticRes * GetDlgParamSetResImageData(LPCTSTR lpszId);
extern CCxStaticRes * GetDlgParamSetResImageData(int nCode);