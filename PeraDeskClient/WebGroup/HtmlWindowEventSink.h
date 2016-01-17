// (c) 2008 Kickstone Technology Ltd

#pragma once
#include "dispatchimpl.h"
#include "HtmlWindowEventCallback.h"
#include "DispatchImpl.h"
#include <mshtml.h>

// class to manage the DIID_HTMLWindowEvents2 sink

class CHtmlWindowEventSink : public CDispatchImpl {
public:
	CHtmlWindowEventSink(IHtmlWindowEventCallback *callback);
	~CHtmlWindowEventSink();

	bool advise(IHTMLWindow2 *pHtmlWindow);
	bool unadvise();

private:

	// CDispatchImpl functions
	HRESULT handleEvent(DISPID dispIdMember, DISPPARAMS *pDispParams);
	void finalRelease();

	// callback
	IHtmlWindowEventCallback *m_callback;

	// connection point cookie
	DWORD m_dwCookie;

	CComPtr<IHTMLWindow2> m_spHtmlWindow;

};
