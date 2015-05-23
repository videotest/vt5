

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sun Apr 26 01:46:32 2015
 */
/* Compiler settings for PropertyBrowser.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0603 
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


#ifndef __PropertyBrowser_h__
#define __PropertyBrowser_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __AllPropPage_FWD_DEFINED__
#define __AllPropPage_FWD_DEFINED__

#ifdef __cplusplus
typedef class AllPropPage AllPropPage;
#else
typedef struct AllPropPage AllPropPage;
#endif /* __cplusplus */

#endif 	/* __AllPropPage_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __PROPERTYBROWSERLib_LIBRARY_DEFINED__
#define __PROPERTYBROWSERLib_LIBRARY_DEFINED__

/* library PROPERTYBROWSERLib */
/* [helpstring][version][uuid] */ 


DEFINE_GUID(LIBID_PROPERTYBROWSERLib,0x2F1431F6,0xD876,0x11D3,0xA0,0x95,0x00,0x00,0xB4,0x93,0xA1,0x87);

DEFINE_GUID(CLSID_AllPropPage,0x2F143203,0xD876,0x11D3,0xA0,0x95,0x00,0x00,0xB4,0x93,0xA1,0x87);

#ifdef __cplusplus

class DECLSPEC_UUID("2F143203-D876-11D3-A095-0000B493A187")
AllPropPage;
#endif
#endif /* __PROPERTYBROWSERLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


