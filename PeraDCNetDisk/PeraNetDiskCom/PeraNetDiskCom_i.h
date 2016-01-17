

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Wed Dec 02 22:53:07 2015
 */
/* Compiler settings for PeraNetDiskCom.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __PeraNetDiskCom_i_h__
#define __PeraNetDiskCom_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __INetDiskCom_FWD_DEFINED__
#define __INetDiskCom_FWD_DEFINED__
typedef interface INetDiskCom INetDiskCom;
#endif 	/* __INetDiskCom_FWD_DEFINED__ */


#ifndef ___INetDiskComEvents_FWD_DEFINED__
#define ___INetDiskComEvents_FWD_DEFINED__
typedef interface _INetDiskComEvents _INetDiskComEvents;
#endif 	/* ___INetDiskComEvents_FWD_DEFINED__ */


#ifndef __NetDiskCom_FWD_DEFINED__
#define __NetDiskCom_FWD_DEFINED__

#ifdef __cplusplus
typedef class NetDiskCom NetDiskCom;
#else
typedef struct NetDiskCom NetDiskCom;
#endif /* __cplusplus */

#endif 	/* __NetDiskCom_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __INetDiskCom_INTERFACE_DEFINED__
#define __INetDiskCom_INTERFACE_DEFINED__

/* interface INetDiskCom */
/* [unique][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_INetDiskCom;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D6C9D706-8DE1-4DAF-B1E8-CDF2A9280BFC")
    INetDiskCom : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE InvokeMethod( 
            /* [in] */ BSTR bstrMethodName,
            /* [in] */ BSTR bstrJsonIn,
            /* [retval][out] */ VARIANT *pvRet) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetDiskComVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INetDiskCom * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INetDiskCom * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INetDiskCom * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INetDiskCom * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INetDiskCom * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INetDiskCom * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INetDiskCom * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *InvokeMethod )( 
            INetDiskCom * This,
            /* [in] */ BSTR bstrMethodName,
            /* [in] */ BSTR bstrJsonIn,
            /* [retval][out] */ VARIANT *pvRet);
        
        END_INTERFACE
    } INetDiskComVtbl;

    interface INetDiskCom
    {
        CONST_VTBL struct INetDiskComVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetDiskCom_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INetDiskCom_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INetDiskCom_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INetDiskCom_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INetDiskCom_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INetDiskCom_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INetDiskCom_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INetDiskCom_InvokeMethod(This,bstrMethodName,bstrJsonIn,pvRet)	\
    ( (This)->lpVtbl -> InvokeMethod(This,bstrMethodName,bstrJsonIn,pvRet) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INetDiskCom_INTERFACE_DEFINED__ */



#ifndef __PeraNetDiskComLib_LIBRARY_DEFINED__
#define __PeraNetDiskComLib_LIBRARY_DEFINED__

/* library PeraNetDiskComLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_PeraNetDiskComLib;

#ifndef ___INetDiskComEvents_DISPINTERFACE_DEFINED__
#define ___INetDiskComEvents_DISPINTERFACE_DEFINED__

/* dispinterface _INetDiskComEvents */
/* [uuid] */ 


EXTERN_C const IID DIID__INetDiskComEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("0A7C3E1A-818F-45F4-852D-BF3F10073AF8")
    _INetDiskComEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _INetDiskComEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _INetDiskComEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _INetDiskComEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _INetDiskComEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _INetDiskComEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _INetDiskComEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _INetDiskComEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _INetDiskComEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _INetDiskComEventsVtbl;

    interface _INetDiskComEvents
    {
        CONST_VTBL struct _INetDiskComEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _INetDiskComEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _INetDiskComEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _INetDiskComEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _INetDiskComEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _INetDiskComEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _INetDiskComEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _INetDiskComEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___INetDiskComEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_NetDiskCom;

#ifdef __cplusplus

class DECLSPEC_UUID("F581128B-AF7E-49F5-8150-011E7E44E751")
NetDiskCom;
#endif
#endif /* __PeraNetDiskComLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


