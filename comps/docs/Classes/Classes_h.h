

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Apr 04 21:03:32 2015
 */
/* Compiler settings for Classes.odl:
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


#ifndef __Classes_h_h__
#define __Classes_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IStatGroupMgrDisp_FWD_DEFINED__
#define __IStatGroupMgrDisp_FWD_DEFINED__
typedef interface IStatGroupMgrDisp IStatGroupMgrDisp;

#endif 	/* __IStatGroupMgrDisp_FWD_DEFINED__ */


#ifndef __StatGroupMgr_FWD_DEFINED__
#define __StatGroupMgr_FWD_DEFINED__

#ifdef __cplusplus
typedef class StatGroupMgr StatGroupMgr;
#else
typedef struct StatGroupMgr StatGroupMgr;
#endif /* __cplusplus */

#endif 	/* __StatGroupMgr_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __Classes_LIBRARY_DEFINED__
#define __Classes_LIBRARY_DEFINED__

/* library Classes */
/* [version][uuid] */ 


DEFINE_GUID(LIBID_Classes,0xC87FE69E,0xFA95,0x425E,0xB4,0x2D,0x46,0x5A,0x9E,0xA4,0x5F,0x2F);

#ifndef __IStatGroupMgrDisp_DISPINTERFACE_DEFINED__
#define __IStatGroupMgrDisp_DISPINTERFACE_DEFINED__

/* dispinterface IStatGroupMgrDisp */
/* [uuid] */ 


DEFINE_GUID(DIID_IStatGroupMgrDisp,0x941D678C,0x6F39,0x4e4f,0x81,0xFA,0xD2,0x2E,0x89,0xC9,0x7F,0xB4);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("941D678C-6F39-4e4f-81FA-D22E89C97FB4")
    IStatGroupMgrDisp : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IStatGroupMgrDispVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IStatGroupMgrDisp * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IStatGroupMgrDisp * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IStatGroupMgrDisp * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IStatGroupMgrDisp * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IStatGroupMgrDisp * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IStatGroupMgrDisp * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IStatGroupMgrDisp * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } IStatGroupMgrDispVtbl;

    interface IStatGroupMgrDisp
    {
        CONST_VTBL struct IStatGroupMgrDispVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IStatGroupMgrDisp_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IStatGroupMgrDisp_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IStatGroupMgrDisp_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IStatGroupMgrDisp_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IStatGroupMgrDisp_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IStatGroupMgrDisp_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IStatGroupMgrDisp_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IStatGroupMgrDisp_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_StatGroupMgr,0xCE4AA513,0x3A74,0x4b10,0xAD,0x36,0xA0,0xD9,0x67,0x8F,0x1E,0xB1);

#ifdef __cplusplus

class DECLSPEC_UUID("CE4AA513-3A74-4b10-AD36-A0D9678F1EB1")
StatGroupMgr;
#endif
#endif /* __Classes_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


