

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


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

MIDL_DEFINE_GUID(IID, IID_INetDiskCom,0xD6C9D706,0x8DE1,0x4DAF,0xB1,0xE8,0xCD,0xF2,0xA9,0x28,0x0B,0xFC);


MIDL_DEFINE_GUID(IID, LIBID_PeraNetDiskComLib,0x4886FF90,0x0DCE,0x4D86,0xA7,0x65,0x54,0x56,0x09,0x0F,0xC5,0xF8);


MIDL_DEFINE_GUID(IID, DIID__INetDiskComEvents,0x0A7C3E1A,0x818F,0x45F4,0x85,0x2D,0xBF,0x3F,0x10,0x07,0x3A,0xF8);


MIDL_DEFINE_GUID(CLSID, CLSID_NetDiskCom,0xF581128B,0xAF7E,0x49F5,0x81,0x50,0x01,0x1E,0x7E,0x44,0xE7,0x51);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



