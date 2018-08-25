

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Aug 25 14:42:58 2018
 */
/* Compiler settings for OleClient.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.00.0603 
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

#ifndef __OleClient_h_h__
#define __OleClient_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IOleItemDataDisp_FWD_DEFINED__
#define __IOleItemDataDisp_FWD_DEFINED__
typedef interface IOleItemDataDisp IOleItemDataDisp;

#endif 	/* __IOleItemDataDisp_FWD_DEFINED__ */


#ifndef __CoOleItemData_FWD_DEFINED__
#define __CoOleItemData_FWD_DEFINED__

#ifdef __cplusplus
typedef class CoOleItemData CoOleItemData;
#else
typedef struct CoOleItemData CoOleItemData;
#endif /* __cplusplus */

#endif 	/* __CoOleItemData_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IOleItemDataDisp_INTERFACE_DEFINED__
#define __IOleItemDataDisp_INTERFACE_DEFINED__

/* interface IOleItemDataDisp */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IOleItemDataDisp;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("375B1D64-25A6-484b-894B-BA37F17BADD0")
    IOleItemDataDisp : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsEmpty( 
            /* [retval][out] */ VARIANT_BOOL *plPos) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IOleItemDataDispVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOleItemDataDisp * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOleItemDataDisp * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOleItemDataDisp * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IOleItemDataDisp * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IOleItemDataDisp * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IOleItemDataDisp * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IOleItemDataDisp * This,
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
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *IsEmpty )( 
            IOleItemDataDisp * This,
            /* [retval][out] */ VARIANT_BOOL *plPos);
        
        END_INTERFACE
    } IOleItemDataDispVtbl;

    interface IOleItemDataDisp
    {
        CONST_VTBL struct IOleItemDataDispVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOleItemDataDisp_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IOleItemDataDisp_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IOleItemDataDisp_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IOleItemDataDisp_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IOleItemDataDisp_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IOleItemDataDisp_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IOleItemDataDisp_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IOleItemDataDisp_IsEmpty(This,plPos)	\
    ( (This)->lpVtbl -> IsEmpty(This,plPos) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IOleItemDataDisp_INTERFACE_DEFINED__ */



#ifndef __OleClientLib_LIBRARY_DEFINED__
#define __OleClientLib_LIBRARY_DEFINED__

/* library OleClientLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_OleClientLib;

EXTERN_C const CLSID CLSID_CoOleItemData;

#ifdef __cplusplus

class DECLSPEC_UUID("42F0E2B8-A151-45aa-891A-3A6E05981226")
CoOleItemData;
#endif
#endif /* __OleClientLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


