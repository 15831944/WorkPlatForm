

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


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


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IToolCore,0x2974AC23,0x9791,0x4C55,0xA3,0x78,0xB2,0x7A,0xFB,0x6F,0x4D,0x48);


MIDL_DEFINE_GUID(IID, LIBID_PeraStartToolComLib,0xAF2AEA18,0x4BA3,0x4BDA,0x82,0x6A,0x01,0xD5,0x04,0x9C,0x0E,0x5C);


MIDL_DEFINE_GUID(IID, DIID__IToolCoreEvents,0x32398B4F,0xAEE5,0x46DC,0x9D,0xF3,0xE0,0x1F,0x99,0x35,0x16,0xF2);


MIDL_DEFINE_GUID(CLSID, CLSID_ToolCore,0xE2F4589C,0x4D73,0x4691,0x98,0x19,0x5A,0xF5,0x0D,0x9A,0xFE,0xCD);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



