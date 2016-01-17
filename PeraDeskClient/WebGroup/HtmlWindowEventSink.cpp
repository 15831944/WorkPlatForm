// (c) 2008 Kickstone Technology Ltd

#include "stdafx.h"
#include "HtmlWindowEventSink.h"
#include <ExDispID.h>
#include <mshtmdid.h>


CHtmlWindowEventSink::CHtmlWindowEventSink(IHtmlWindowEventCallback *callback) : m_callback(callback), m_dwCookie(0) {
}

CHtmlWindowEventSink::~CHtmlWindowEventSink() {
}

///////////////////////////////////////////////////////////////////////////
// Connect/Disconnect to the DWebBrowserEvents2 Source
///////////////////////////////////////////////////////////////////////////
bool CHtmlWindowEventSink::advise(IHTMLWindow2 *pHtmlWindow) {
	HRESULT hr;

	if (!pHtmlWindow)
		return false;

	m_spHtmlWindow=pHtmlWindow;
	CComQIPtr<IConnectionPointContainer, &IID_IConnectionPointContainer> spCPContainer=m_spHtmlWindow;

	if (spCPContainer != NULL) {
		CComPtr<IConnectionPoint> spConnectionPoint;

		hr = spCPContainer->FindConnectionPoint(DIID_HTMLWindowEvents2, &spConnectionPoint);
		if (SUCCEEDED(hr) && !m_dwCookie) {
			hr = spConnectionPoint->Advise((IDispatch*)this, &m_dwCookie);
			if (FAILED(hr))
				ATLTRACE("CWebBrowserEventSink: advise(): Failed\n\n");
		}
		return SUCCEEDED(hr);
	}
	return false;
}

bool CHtmlWindowEventSink::unadvise() {
	HRESULT hr;

	if (!m_spHtmlWindow)
		return false;

	CComQIPtr<IConnectionPointContainer, &IID_IConnectionPointContainer> spCPContainer=m_spHtmlWindow;
	if (spCPContainer != NULL) {
		CComPtr<IConnectionPoint> spConnectionPoint;

		hr = spCPContainer->FindConnectionPoint(DIID_HTMLWindowEvents2, &spConnectionPoint);
		if (SUCCEEDED(hr) && m_dwCookie) {
			// Remove us from the list of people interested...
			hr = spConnectionPoint->Unadvise(m_dwCookie);
			if (FAILED(hr))
				ATLTRACE("CWebBrowserEventSink: unadvise(): Failed\n\n");
			m_dwCookie=0;
			m_spHtmlWindow=NULL;
		}
		return SUCCEEDED(hr);
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////
// Event handler
///////////////////////////////////////////////////////////////////////////
HRESULT CHtmlWindowEventSink::handleEvent(DISPID dispIdMember, DISPPARAMS *pDispParams){
	if (!pDispParams)
		return E_INVALIDARG;

	if (!m_callback)
		return E_UNEXPECTED;

	HRESULT hr=S_OK;
	switch (dispIdMember) {
		case DISPID_HTMLWINDOWEVENTS2_ONLOAD:
			m_callback->onLoadOccured(); break;
		case DISPID_HTMLWINDOWEVENTS2_ONUNLOAD:
			m_callback->onUnloadOccured(); break;
	}
	return hr;
}

// release the callback
void CHtmlWindowEventSink::finalRelease(){
	m_callback->releasePointer();
}