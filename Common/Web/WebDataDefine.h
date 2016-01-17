#pragma once
#include <atlcomcli.h>
#include <atlstr.h>
#include "StlHeader.h"

#define WCB_FALSE						0
#define WCB_TRUE						1

/*
异步处理WebEvent消息。
通常在OnCopyData中，先解析出WE_XXX，然后根据消息代号，决定是同步处理还是异步处理，
如果是异步处理，则给对应的主窗口Post本消息。
参数：
	wp：CWebEventData*
	lp：0
*/
extern UINT WM_WEBEVENT_ASYNC;
/*
内嵌页面加载完成。
通常由web回调，在CBaseHtmlDialog::OnWebCallback中，Post本消息给主窗口，做后续处理。
参数：
wp：0。
lp：0。
*/
extern UINT WM_WEB_LOADED;

//与 js 方法中的参数结构体对应
typedef map< CString, CComVariant > WEBCALLBACK_METHOD_PARAM_MAP;


class CWebMethodParamData
{
public:
	CWebMethodParamData();
	virtual ~CWebMethodParamData();
	void Clear(void);

public:
	CString m_sName;
	CString m_sValue;
	CString m_sDataType;
};

class CWebMethodData
{
public:
	CWebMethodData();
	virtual ~CWebMethodData();
	void Clear(void);

	CString GetJsonStr(void);
	BOOL Parse(LPCTSTR lpszJsonStr);
public:
	//CString m_strWebID;
	CString m_sMethodName;
	vector< CWebMethodParamData * > m_vParams;
	CString m_sResult;
	CString m_sInfo;
};

//进程间通讯，进程内数据解析等
class CWebEventData
{
public:
	CWebEventData();
	virtual ~CWebEventData();
	void Clear();
	//派生类中，删除 m_sData 解析出来的数据对象，如 url。
	virtual void ClearData();
	//{ webId:"xxx", webType:"xxx", webEvent:"xxx", data:"xxx" }
	BOOL Parse( LPCTSTR lpData );
	//生成json串
	CString GetJsonStr(); // for copydata
	//解析data数据
	virtual BOOL ParseData( LPCTSTR lpData ); //data:"{}"
	//生成data数据
	virtual CString GetDataStr(); //data:"{}"
	//拷贝id、webId、webType、webEvent
	BOOL CopyEventHeaderFrom( CWebEventData* pSrc );
	
public:
	CString m_sId; // 跨进程 CWebEventData 对象的唯一标识
	CString m_sWebId;	//等于 CAttachWindow::m_sId;
	DWORD m_dwWebType;	// 等于 CAttachWindow::m_nExeFlag
	DWORD m_dwWebEvent;
	CString m_sData;
};