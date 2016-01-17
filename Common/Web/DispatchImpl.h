// (c) 2008 Kickstone Technology Ltd

#pragma once

// base implementation of an IDispatch class

class CDispatchImpl : public IDispatch{
public:
	CDispatchImpl();
	virtual ~CDispatchImpl() {}

    // IUnknown 
    STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    // IDispatch
    STDMETHODIMP GetTypeInfoCount(UINT *pctinfo);        
    STDMETHODIMP GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);    
    STDMETHODIMP GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
	STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);


private:

	// functions overridden by the subclasses
	// handle any events
	virtual HRESULT handleEvent(DISPID dispIdMember, DISPPARAMS *pDispParams)=0;
	//allow the objects to do stuff when they are delete
	virtual void finalRelease(){}

	LONG _refCount;

};
