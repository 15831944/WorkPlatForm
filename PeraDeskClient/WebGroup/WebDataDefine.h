#pragma once
#include <atlcomcli.h>
#include <atlstr.h>
#include "StlHeader.h"

#define WCB_FALSE						0
#define WCB_TRUE						1

enum{
	WCB_LOADWEB_FINISHED = 20,
	WCB_SHOW_PROGRESS    = 21,
	WCB_CUSTOM
};

//与 js 方法中的参数结构体对应
typedef map< CString, CComVariant > WEBCALLBACK_METHOD_PARAM_MAP;

class CWebMethodParamData
{
public:
	CWebMethodParamData();
	virtual ~CWebMethodParamData();
	void Clear(void);

public:
	CString m_strName;
	CString m_strValue;
	CString m_strDataType;
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
	CString m_strWebID;
	CString m_strMethodName;
	vector< CWebMethodParamData * > m_vParams;
	CString m_strResult;
	CString m_strInfo;
};
