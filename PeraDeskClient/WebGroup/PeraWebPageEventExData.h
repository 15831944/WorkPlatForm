#pragma once
#include "CxWebPageEvent.h"

//启动子流程//////////////////////////////////////////////////
class CWPERunFlowStart : public CCxWebPageEventExData
{
public:
	CWPERunFlowStart();
	virtual ~CWPERunFlowStart();
	virtual void Clear(void);
protected:
	virtual BOOL _OnGetCopy( CCxWebPageEventExData *& pDest );
	virtual BOOL OnParseJsonObj( LPVOID lpJsonObj );
	virtual BOOL OnGetJsonStr( LPVOID lpJsonObj );
	
public:
	CString m_strUrl;
	CString m_strFlowId;
};

//子流程运行结束///////////////////////////////////////////////////////
class CWPERunFlowCompleted : public CCxWebPageEventExData
{
public:
	static const LPCTSTR S_STOP;
	static const LPCTSTR S_COMPLETED;

	CWPERunFlowCompleted();
	virtual ~CWPERunFlowCompleted();
	virtual void Clear();
protected:
	virtual BOOL _OnGetCopy( CCxWebPageEventExData *& pDest );
	virtual BOOL OnParseJsonObj( LPVOID lpJsonObj );
	virtual BOOL OnGetJsonStr( LPVOID lpJsonObj );
public:
	CString m_strStatus;
};