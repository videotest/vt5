

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Fri Oct 26 00:42:04 2007
 */
/* Compiler settings for .\VTTable.idl:
    Oicf, W1, Zp8, env=Win64 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

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


#ifndef __VTTableidl_h__
#define __VTTableidl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef ___DVTTable_FWD_DEFINED__
#define ___DVTTable_FWD_DEFINED__
typedef interface _DVTTable _DVTTable;
#endif 	/* ___DVTTable_FWD_DEFINED__ */


#ifndef ___DVTTableEvents_FWD_DEFINED__
#define ___DVTTableEvents_FWD_DEFINED__
typedef interface _DVTTableEvents _DVTTableEvents;
#endif 	/* ___DVTTableEvents_FWD_DEFINED__ */


#ifndef __VTTable_FWD_DEFINED__
#define __VTTable_FWD_DEFINED__

#ifdef __cplusplus
typedef class VTTable VTTable;
#else
typedef struct VTTable VTTable;
#endif /* __cplusplus */

#endif 	/* __VTTable_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 


#ifndef __VTTableLib_LIBRARY_DEFINED__
#define __VTTableLib_LIBRARY_DEFINED__

/* library VTTableLib */
/* [control][helpstring][helpfile][version][uuid] */ 


EXTERN_C const IID LIBID_VTTableLib;

#ifndef ___DVTTable_DISPINTERFACE_DEFINED__
#define ___DVTTable_DISPINTERFACE_DEFINED__

/* dispinterface _DVTTable */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__DVTTable;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("A4CCD95C-7C07-4B20-A24A-FEBA65927A1D")
    _DVTTable : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTTableVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTTable * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTTable * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTTable * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTTable * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTTable * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTTable * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTTable * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _DVTTableVtbl;

    interface _DVTTable
    {
        CONST_VTBL struct _DVTTableVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTTable_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _DVTTable_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _DVTTable_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _DVTTable_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _DVTTable_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _DVTTable_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _DVTTable_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTTable_DISPINTERFACE_DEFINED__ */


#ifndef ___DVTTableEvents_DISPINTERFACE_DEFINED__
#define ___DVTTableEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DVTTableEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__DVTTableEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("8321B3DD-9A35-4185-BC0D-8D212D6CC18F")
    _DVTTableEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTTableEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTTableEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTTableEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTTableEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTTableEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTTableEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTTableEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTTableEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _DVTTableEventsVtbl;

    interface _DVTTableEvents
    {
        CONST_VTBL struct _DVTTableEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTTableEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _DVTTableEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _DVTTableEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _DVTTableEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _DVTTableEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _DVTTableEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _DVTTableEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTTableEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_VTTable;

#ifdef __cplusplus

class DECLSPEC_UUID("C63275F8-6165-4432-9022-C7CC5F49F2A6")
VTTable;
#endif
#endif /* __VTTableLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


