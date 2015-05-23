

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Apr 04 21:04:18 2015
 */
/* Compiler settings for LookupHelperAX.odl:
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


#ifndef __LookupHelperAX_h_h__
#define __LookupHelperAX_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef ___DLookupHelperAX_FWD_DEFINED__
#define ___DLookupHelperAX_FWD_DEFINED__
typedef interface _DLookupHelperAX _DLookupHelperAX;

#endif 	/* ___DLookupHelperAX_FWD_DEFINED__ */


#ifndef ___DLookupHelperAXEvents_FWD_DEFINED__
#define ___DLookupHelperAXEvents_FWD_DEFINED__
typedef interface _DLookupHelperAXEvents _DLookupHelperAXEvents;

#endif 	/* ___DLookupHelperAXEvents_FWD_DEFINED__ */


#ifndef __LookupHelperAX_FWD_DEFINED__
#define __LookupHelperAX_FWD_DEFINED__

#ifdef __cplusplus
typedef class LookupHelperAX LookupHelperAX;
#else
typedef struct LookupHelperAX LookupHelperAX;
#endif /* __cplusplus */

#endif 	/* __LookupHelperAX_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __LOOKUPHELPERAXLib_LIBRARY_DEFINED__
#define __LOOKUPHELPERAXLib_LIBRARY_DEFINED__

/* library LOOKUPHELPERAXLib */
/* [control][helpstring][helpfile][version][uuid] */ 

#pragma once
#pragma region Desktop Family
#pragma endregion

DEFINE_GUID(LIBID_LOOKUPHELPERAXLib,0xF970DB7A,0x64A1,0x4da2,0x9A,0x88,0x1F,0xAE,0xDD,0x79,0x00,0x12);

#ifndef ___DLookupHelperAX_DISPINTERFACE_DEFINED__
#define ___DLookupHelperAX_DISPINTERFACE_DEFINED__

/* dispinterface _DLookupHelperAX */
/* [hidden][helpstring][uuid] */ 


DEFINE_GUID(DIID__DLookupHelperAX,0x5161EFE8,0xF62D,0x4818,0x9B,0x91,0xE1,0x76,0xC0,0x16,0x4C,0xE0);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("5161EFE8-F62D-4818-9B91-E176C0164CE0")
    _DLookupHelperAX : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DLookupHelperAXVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DLookupHelperAX * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DLookupHelperAX * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DLookupHelperAX * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DLookupHelperAX * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DLookupHelperAX * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DLookupHelperAX * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DLookupHelperAX * This,
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
    } _DLookupHelperAXVtbl;

    interface _DLookupHelperAX
    {
        CONST_VTBL struct _DLookupHelperAXVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DLookupHelperAX_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DLookupHelperAX_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DLookupHelperAX_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DLookupHelperAX_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DLookupHelperAX_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DLookupHelperAX_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DLookupHelperAX_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DLookupHelperAX_DISPINTERFACE_DEFINED__ */


#ifndef ___DLookupHelperAXEvents_DISPINTERFACE_DEFINED__
#define ___DLookupHelperAXEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DLookupHelperAXEvents */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DLookupHelperAXEvents,0x91E410AA,0x7B21,0x4fa1,0xB7,0x37,0x30,0x26,0x04,0x99,0xDB,0x30);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("91E410AA-7B21-4fa1-B737-30260499DB30")
    _DLookupHelperAXEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DLookupHelperAXEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DLookupHelperAXEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DLookupHelperAXEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DLookupHelperAXEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DLookupHelperAXEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DLookupHelperAXEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DLookupHelperAXEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DLookupHelperAXEvents * This,
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
    } _DLookupHelperAXEventsVtbl;

    interface _DLookupHelperAXEvents
    {
        CONST_VTBL struct _DLookupHelperAXEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DLookupHelperAXEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DLookupHelperAXEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DLookupHelperAXEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DLookupHelperAXEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DLookupHelperAXEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DLookupHelperAXEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DLookupHelperAXEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DLookupHelperAXEvents_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_LookupHelperAX,0x3FEAC05C,0x7050,0x431E,0xB1,0x34,0x26,0xBB,0xDE,0xB3,0x2E,0xDA);

#ifdef __cplusplus

class DECLSPEC_UUID("3FEAC05C-7050-431E-B134-26BBDEB32EDA")
LookupHelperAX;
#endif
#endif /* __LOOKUPHELPERAXLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


