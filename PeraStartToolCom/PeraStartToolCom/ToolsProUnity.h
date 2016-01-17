#pragma once
#include "ToolObjectDefine.h"
#include <map>
using namespace std;

#define USE_TINY_XML 0 
/************************************************************************/
/* 启动工具处理类                                                                 */
/************************************************************************/
class CToolsProUnity
{
public:
	CToolsProUnity(void);
	~CToolsProUnity(void);
public:
	static BOOL StartTool(ToolObject *ptool);
	static BOOL StartManagerUI();
	static BOOL ConfigTool(ToolObject * ptool,BOOL bCallInStartApp = FALSE);
	static BOOL CheckStartApp(const CString & strToolId);
	static CString GetToolVersion(CString strToolPath);
	static BOOL CompareLocalWithService(const ToolObject & toolService, const ToolObject & toolLocal);
	static CString GetExePathFormKey(const ToolObject * pTool,CString strKeyRoot);
	static CString GetExePathFormKey(const ToolObject * pTool,CString strKeyRoot,BOOL bGetKeyV64);
	static CString GetFilePathFormServInfo(const ToolObject *pTool);
	static BOOL StartApp(LPCTSTR strApp,LPCTSTR strParam);
	static BOOL IsOpenByParam(const ToolObject *pTool);
    static BOOL OpenUrlWithIE(const CString &strUrl);

	/************************************************************************/
	/* 根据ID从xml中获取工具的记录                                               */
	/************************************************************************/
	static BOOL GetToolFormXml(const CString & strId,ToolObject & toolLocal);
	/************************************************************************/
	/* 加载本地所有配置记录                                                  */
	/************************************************************************/
	static BOOL LoadAllLocalTool(map<CString,ToolObject> & mapTools);
	/************************************************************************/
	/* 将配置的工具写入xml中                                                   */
	/************************************************************************/
	static BOOL WriteToolConfigXml(const ToolObject & Tool); 
	/************************************************************************/
	/* 从本地配置xml中删除配置项                                             */
	/************************************************************************/
	static void  DeleteToolConfigFromXml(const CString & strToolId);
	/************************************************************************/
	/* 从本地配置xml中删除所有工具的配置项                                     */
	/************************************************************************/
	static void  DeleteAllConfigFromXml();
public:
	static CString g_strConfigXml;
};

