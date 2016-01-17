#pragma once
#include "CxWebPageEvent.h"

//����������//////////////////////////////////////////////////
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

//���������н���///////////////////////////////////////////////////////
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