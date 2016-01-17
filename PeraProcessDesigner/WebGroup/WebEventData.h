#pragma once
#include "WebDataDefine.h"

//////////////////////////////////////////////////////////////////////////

//用于附加进程时传送的扩展数据
//CAttachWindow::CreateClient 使用
class CAttachWindowExData
{
public:
	CAttachWindowExData();
	~CAttachWindowExData();

	void	Clear();
	BOOL	Parse( LPCTSTR lpData );
	CString GetJsonStr();
public:
	CString m_sUrl;
};


//完成状态
//WEBTYPE_RUNFLOW
//WE_RUNFLOW_COMPLETED
class CWERunFlowCompleted : public CWebEventData
{
public:
	CWERunFlowCompleted();
	virtual ~CWERunFlowCompleted();
	virtual BOOL ParseData( LPCTSTR lpData ); //data:"{}"
	virtual CString GetDataStr(); //data:"{}"
	virtual void ClearData();
public:
	CString m_sStatus;
};


//获取子流程id
//WEBTYPE_RUNFLOW
//WE_RUNFLOW_SUBFLOWID
class CWERunFlowSubFlowId : public CWebEventData
{
public:
	CWERunFlowSubFlowId();
	virtual ~CWERunFlowSubFlowId();
	virtual BOOL ParseData( LPCTSTR lpData ); //data:"{}"
	virtual CString GetDataStr(); //data:"{}"
	virtual void ClearData();
public:
	CString m_sSubFlowId;
};

//取消运行流程
//WEBTYPE_RUNFLOW
//WE_RUNFLOW_CANCEL
class CWERunFlowCancel : public CWebEventData
{
public:
	CWERunFlowCancel();
	virtual ~CWERunFlowCancel();
	virtual BOOL ParseData( LPCTSTR lpData ); //data:"{}"
	virtual CString GetDataStr(); //data:"{}"
	virtual void ClearData();
};

//创建查看流程对话框时使用
//解析出，查看流程对话框显示的 url、宽、高。
class CJsData_ShowViewRunFlow
{
public:
	CJsData_ShowViewRunFlow();
	~CJsData_ShowViewRunFlow();
	void Clear();
	BOOL Parse( LPCTSTR lpszData );
public:
	CString m_strUrl;
	int m_nWidth;
	int m_nHeight;
};