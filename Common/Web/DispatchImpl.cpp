// (c) 2008 Kickstone Technology Ltd


#include "stdafx.h"
#include "DispatchImpl.h"

CDispatchImpl::CDispatchImpl() : _refCount(0) {
}



///////////////////////////////////////////////////////////////////////////
// IUnknown methods
///////////////////////////////////////////////////////////////////////////
STDMETHODIMP CDispatchImpl::QueryInterface(const IID &riid, void **ppvObject) {
    if( ppvObject == NULL )
        return E_INVALIDARG;

    *ppvObject = NULL;

    if( riid == IID_IUnknown )
        *ppvObject = static_cast<IUnknown*>(this);
    else if( riid == IID_IDispatch )
        *ppvObject = static_cast<IDispatch*>(this);

    if( *ppvObject == NULL )
        return E_NOINTERFACE;

    AddRef();

    return S_OK;
}

STDMETHODIMP_(ULONG) CDispatchImpl::AddRef() {
    return InterlockedIncrement(&_refCount);
}


STDMETHODIMP_(ULONG) CDispatchImpl::Release() {
    LONG refCount = InterlockedDecrement(&_refCount);
    if( refCount == 0 ) {
		finalRelease();
        delete this;
        return 0;
    } else	
        return refCount;
}

///////////////////////////////////////////////////////////////////////////
// IDispatch methods
///////////////////////////////////////////////////////////////////////////
STDMETHODIMP CDispatchImpl::GetTypeInfoCount(UINT * /*pctinfo*/) {
    return E_NOTIMPL;
}

STDMETHODIMP CDispatchImpl::GetTypeInfo(UINT /*iTInfo*/, LCID /*lcid*/, ITypeInfo ** /*ppTInfo*/) {
    return E_NOTIMPL;
}

STDMETHODIMP CDispatchImpl::GetIDsOfNames(const IID &/*riid*/, LPOLESTR * /*rgszNames*/, UINT /*cNames*/, LCID /*lcid*/, DISPID * /*rgDispId*/) {
    return E_NOTIMPL;
}

STDMETHODIMP CDispatchImpl::Invoke(DISPID dispIdMember, const IID &/*riid*/, LCID /*lcid*/, WORD /*wFlags*/, DISPPARAMS *pDispParams, VARIANT * /*pVarResult*/, EXCEPINFO * /*pExcepInfo*/, UINT * /*puArgErr*/) {

	// call the subclass event handler
	return handleEvent(dispIdMember, pDispParams);
}



