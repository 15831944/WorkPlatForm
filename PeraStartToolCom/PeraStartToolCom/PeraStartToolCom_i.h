

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Sat Dec 19 18:27:10 2015
 */
/* Compiler settings for PeraStartToolCom.idl:
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

#ifndef __PeraStartToolCom_i_h__
#define __PeraStartToolCom_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IToolCore_FWD_DEFINED__
#define __IToolCore_FWD_DEFINED__
typedef interface IToolCore IToolCore;
#endif 	/* __IToolCore_FWD_DEFINED__ */


#ifndef ___IToolCoreEvents_FWD_DEFINED__
#define ___IToolCoreEvents_FWD_DEFINED__
typedef interface _IToolCoreEvents _IToolCoreEvents;
#endif 	/* ___IToolCoreEvents_FWD_DEFINED__ */


#ifndef __ToolCore_FWD_DEFINED__
#define __ToolCore_FWD_DEFINED__

#ifdef __cplusplus
typedef class ToolCore ToolCore;
#else
typedef struct ToolCore ToolCore;
#endif /* __cplusplus */

#endif 	/* __ToolCore_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IToolCore_INTERFACE_DEFINED__
#define __IToolCore_INTERFACE_DEFINED__

/* interface IToolCore */
/* [unique][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IToolCore;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2974AC23-9791-4C55-A378-B27AFB6F4D48")
    IToolCore : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InvokeApp( 
            /* [in] */ BSTR strInJson,
            /* [retval][out] */ VARIANT *pVOut) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ManageApp( 
            /* [in] */ BSTR strInJson,
            /* [retval][out] */ VARIANT *pVOut) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ConfigApp( 
            /* [in] */ BSTR strInJson,
            /* [retval][out] */ VARIANT *pVOut) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InvokeAppById( 
            /* [in] */ BSTR strInParam,
            /* [retval][out] */ VARIANT *pVOut) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OpenUrl( 
            /* [in] */ BSTR strUrl,
            /* [retval][out] */ VARIANT *pVout) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IToolCoreVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IToolCore * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IToolCore * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IToolCore * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IToolCore * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IToolCore * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IToolCore * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IToolCore * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InvokeApp )( 
            IToolCore * This,
            /* [in] */ BSTR strInJson,
            /* [retval][out] */ VARIANT *pVOut);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ManageApp )( 
            IToolCore * This,
            /* [in] */ BSTR strInJson,
            /* [retval][out] */ VARIANT *pVOut);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ConfigApp )( 
            IToolCore * This,
            /* [in] */ BSTR strInJson,
            /* [retval][out] */ VARIANT *pVOut);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InvokeAppById )( 
            IToolCore * This,
            /* [in] */ BSTR strInParam,
            /* [retval][out] */ VARIANT *pVOut);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OpenUrl )( 
            IToolCore * This,
            /* [in] */ BSTR strUrl,
            /* [retval][out] */ VARIANT *pVout);
        
        END_INTERFACE
    } IToolCoreVtbl;

    interface IToolCore
    {
        CONST_VTBL struct IToolCoreVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IToolCore_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IToolCore_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IToolCore_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IToolCore_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IToolCore_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IToolCore_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IToolCore_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IToolCore_InvokeApp(This,strInJson,pVOut)	\
    ( (This)->lpVtbl -> InvokeApp(This,strInJson,pVOut) ) 

#define IToolCore_ManageApp(This,strInJson,pVOut)	\
    ( (This)->lpVtbl -> ManageApp(This,strInJson,pVOut) ) 

#define IToolCore_ConfigApp(This,strInJson,pVOut)	\
    ( (This)->lpVtbl -> ConfigApp(This,strInJson,pVOut) ) 

#define IToolCore_InvokeAppById(This,strInParam,pVOut)	\
    ( (This)->lpVtbl -> InvokeAppById(This,strInParam,pVOut) ) 

#define IToolCore_OpenUrl(This,strUrl,pVout)	\
    ( (This)->lpVtbl -> OpenUrl(This,strUrl,pVout) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IToolCore_INTERFACE_DEFINED__ */



#ifndef __PeraStartToolComLib_LIBRARY_DEFINED__
#define __PeraStartToolComLib_LIBRARY_DEFINED__

/* library PeraStartToolComLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_PeraStartToolComLib;

#ifndef ___IToolCoreEvents_DISPINTERFACE_DEFINED__
#define ___IToolCoreEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IToolCoreEvents */
/* [uuid] */ 


EXTERN_C const IID DIID__IToolCoreEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("32398B4F-AEE5-46DC-9DF3-E01F993516F2")
    _IToolCoreEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IToolCoreEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IToolCoreEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IToolCoreEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IToolCoreEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IToolCoreEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IToolCoreEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IToolCoreEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IToolCoreEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IToolCoreEventsVtbl;

    interface _IToolCoreEvents
    {
        CONST_VTBL struct _IToolCoreEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IToolCoreEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _IToolCoreEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _IToolCoreEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _IToolCoreEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _IToolCoreEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _IToolCoreEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _IToolCoreEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IToolCoreEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ToolCore;

#ifdef __cplusplus

class DECLSPEC_UUID("E2F4589C-4D73-4691-9819-5AF50D9AFECD")
ToolCore;
#endif
#endif /* __PeraStartToolComLib_LIBRARY_DEFINED__ */

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


