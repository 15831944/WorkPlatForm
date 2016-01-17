#pragma once
#include "StlHeader.h"
#include <atlstr.h>

/*
使用说明：

*/
//////////////////////////////////////////////////////////////////////////

//注意：禁止创建局部对象。
class CCxWebPageEvent;
class CCxWebPageMgr;

extern BOOL CanParseJsonObjPtr( LPVOID lpData );

/************************************************************************
WebPage事件的附加数据的基类。
CCxWebPageEvent 构造函数的第三个参数。
*************************************************************************/
class CCxWebPageEventExData
{
	friend class CCxWebPageEvent;

public:
	CCxWebPageEventExData();
	virtual ~CCxWebPageEventExData();
public:
	virtual void Clear(void);
public:
	BOOL GetCopy( CCxWebPageEventExData *& pOther );
protected:
	virtual BOOL _OnGetCopy( CCxWebPageEventExData *& pOther ) = 0;

public:
	//
	virtual BOOL Parse( LPCTSTR lpszData );
protected:
	//{...,"data":{},...}
	//解析data对应的对象
	virtual BOOL OnParseJsonObj( LPVOID lpJsonObj );
	//生成："data":{}
	virtual BOOL OnGetJsonStr( LPVOID lpJsonObj );
};

class CCxWPENavigate : public CCxWebPageEventExData
{
public:
	CCxWPENavigate();
	virtual ~CCxWPENavigate();
	virtual void Clear(void);
protected:
	virtual BOOL _OnGetCopy( CCxWebPageEventExData *& pOther );
	virtual BOOL OnParseJsonObj( LPVOID lpJsonObj );
	virtual BOOL OnGetJsonStr( LPVOID lpJsonObj );
public:
	CString m_strUrl;
};

//////////////////////////////////////////////////////////////////////////

class CCxWebPageEvent
{
	friend class CCxWebPageMgr;
	friend class CPeraWebPageMgr;

public:
	CCxWebPageEvent( int nModuleFlag = 0, int nEventFlag = 0 );
	virtual ~CCxWebPageEvent(void);

	virtual void Clear(void);
	BOOL Parse( LPCTSTR lpszData );
	CString GetJsonStr( void );
	//解析扩展数据 "data":{} 
	virtual BOOL ParseExData( void );

	//创建同步事件（调用方创建）
	BOOL CreateSyncEvent(void);
	//响应同步事件（被调用方应答）
	BOOL AnswerSyncEvent(void);

	//取得属性
	int GetModuleFlag(void) const { return m_nModuleFlag; }
	int GetEventFlag(void) const { return m_nEventFlag; }
	CCxWebPageEventExData * GetExData(void) { return m_pData; }
	//
	BOOL SendCopyData( HWND hwndRecv );

protected:
	virtual BOOL OnParsing( LPVOID lpJsonObj );
	virtual BOOL OnGetJsonStr( LPVOID lpJsonObj );

public:
	//事件标识，构造时，值同 m_strId
	//进程间通讯时，应答方设置该值，与对方进程中的 m_strId 匹配。
	CString m_strEventId;
	BOOL m_bAnswer; //是否为应答。默认为：FALSE
	BOOL m_bSync; //是否需要同步，接收事件方，必须应答。

	DWORD m_dwMainProcessId; //客户端进程Id。
	HWND m_hWndMain; //客户端进程的窗口句柄。
	DWORD m_dwWebPageProcessId; //WebPage进程。
	HWND m_hWndWebPage; //WebPage窗口句柄。

protected:
	//在本进程内，对象的唯一标识，不会作为序列化数据传递。
	CString m_strId;
	//WebPage模块标识
	int m_nModuleFlag;
	//事件标识
	int m_nEventFlag;
	//每个事件的扩展数据（m_nEventFlag 对应的数据）
	CCxWebPageEventExData * m_pData;
	//指向一个 Json::Value * （Json对象指针）。对应对象中的 "data":{}
	LPVOID m_lpJsonObj;
	//
	CCxWebPageMgr * m_pMgr;


};
