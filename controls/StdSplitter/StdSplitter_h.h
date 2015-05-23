

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Apr 04 21:04:40 2015
 */
/* Compiler settings for StdSplitter.odl:
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


#ifndef __StdSplitter_h_h__
#define __StdSplitter_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ISplitterStandart_FWD_DEFINED__
#define __ISplitterStandart_FWD_DEFINED__
typedef interface ISplitterStandart ISplitterStandart;

#endif 	/* __ISplitterStandart_FWD_DEFINED__ */


#ifndef __SplitterStandart_FWD_DEFINED__
#define __SplitterStandart_FWD_DEFINED__

#ifdef __cplusplus
typedef class SplitterStandart SplitterStandart;
#else
typedef struct SplitterStandart SplitterStandart;
#endif /* __cplusplus */

#endif 	/* __SplitterStandart_FWD_DEFINED__ */


#ifndef __ISplitterCompare_FWD_DEFINED__
#define __ISplitterCompare_FWD_DEFINED__
typedef interface ISplitterCompare ISplitterCompare;

#endif 	/* __ISplitterCompare_FWD_DEFINED__ */


#ifndef __SplitterCompare_FWD_DEFINED__
#define __SplitterCompare_FWD_DEFINED__

#ifdef __cplusplus
typedef class SplitterCompare SplitterCompare;
#else
typedef struct SplitterCompare SplitterCompare;
#endif /* __cplusplus */

#endif 	/* __SplitterCompare_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __StdSplitter_LIBRARY_DEFINED__
#define __StdSplitter_LIBRARY_DEFINED__

/* library StdSplitter */
/* [version][uuid] */ 


DEFINE_GUID(LIBID_StdSplitter,0xAC8FC6C6,0x27A1,0x11D3,0xA5,0xDC,0x00,0x00,0xB4,0x93,0xFF,0x1B);

#ifndef __ISplitterStandart_DISPINTERFACE_DEFINED__
#define __ISplitterStandart_DISPINTERFACE_DEFINED__

/* dispinterface ISplitterStandart */
/* [uuid] */ 


DEFINE_GUID(DIID_ISplitterStandart,0xAC8FC6D3,0x27A1,0x11D3,0xA5,0xDC,0x00,0x00,0xB4,0x93,0xFF,0x1B);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("AC8FC6D3-27A1-11D3-A5DC-0000B493FF1B")
    ISplitterStandart : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct ISplitterStandartVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISplitterStandart * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISplitterStandart * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISplitterStandart * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISplitterStandart * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISplitterStandart * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISplitterStandart * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISplitterStandart * This,
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
    } ISplitterStandartVtbl;

    interface ISplitterStandart
    {
        CONST_VTBL struct ISplitterStandartVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISplitterStandart_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISplitterStandart_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISplitterStandart_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISplitterStandart_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISplitterStandart_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISplitterStandart_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISplitterStandart_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __ISplitterStandart_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_SplitterStandart,0xa8902372,0x27ae,0x11d3,0xa5,0xdc,0x00,0x00,0xb4,0x93,0xff,0x1b);

#ifdef __cplusplus

class DECLSPEC_UUID("a8902372-27ae-11d3-a5dc-0000b493ff1b")
SplitterStandart;
#endif

#ifndef __ISplitterCompare_DISPINTERFACE_DEFINED__
#define __ISplitterCompare_DISPINTERFACE_DEFINED__

/* dispinterface ISplitterCompare */
/* [uuid] */ 


DEFINE_GUID(DIID_ISplitterCompare,0xFDF4B577,0x3B45,0x446A,0xB8,0xD3,0x96,0x2E,0x26,0x9A,0x1C,0x23);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("FDF4B577-3B45-446A-B8D3-962E269A1C23")
    ISplitterCompare : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct ISplitterCompareVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISplitterCompare * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISplitterCompare * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISplitterCompare * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISplitterCompare * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISplitterCompare * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISplitterCompare * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISplitterCompare * This,
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
    } ISplitterCompareVtbl;

    interface ISplitterCompare
    {
        CONST_VTBL struct ISplitterCompareVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISplitterCompare_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISplitterCompare_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISplitterCompare_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISplitterCompare_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISplitterCompare_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISplitterCompare_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISplitterCompare_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __ISplitterCompare_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_SplitterCompare,0x20EA12E2,0x8691,0x4AEF,0x9F,0x1B,0x04,0x88,0xB5,0x67,0x08,0xC4);

#ifdef __cplusplus

class DECLSPEC_UUID("20EA12E2-8691-4AEF-9F1B-0488B56708C4")
SplitterCompare;
#endif
#endif /* __StdSplitter_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


