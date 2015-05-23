

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Apr 04 21:04:10 2015
 */
/* Compiler settings for HistAX.odl:
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


#ifndef __HistAX_h_h__
#define __HistAX_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef ___DHistAX_FWD_DEFINED__
#define ___DHistAX_FWD_DEFINED__
typedef interface _DHistAX _DHistAX;

#endif 	/* ___DHistAX_FWD_DEFINED__ */


#ifndef ___DHistAXEvents_FWD_DEFINED__
#define ___DHistAXEvents_FWD_DEFINED__
typedef interface _DHistAXEvents _DHistAXEvents;

#endif 	/* ___DHistAXEvents_FWD_DEFINED__ */


#ifndef __HistAX_FWD_DEFINED__
#define __HistAX_FWD_DEFINED__

#ifdef __cplusplus
typedef class HistAX HistAX;
#else
typedef struct HistAX HistAX;
#endif /* __cplusplus */

#endif 	/* __HistAX_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __HISTAXLib_LIBRARY_DEFINED__
#define __HISTAXLib_LIBRARY_DEFINED__

/* library HISTAXLib */
/* [control][helpstring][helpfile][version][uuid] */ 

#pragma once
#pragma region Desktop Family
#pragma endregion

DEFINE_GUID(LIBID_HISTAXLib,0x06FDDE55,0xF981,0x11D3,0xA0,0xC1,0x00,0x00,0xB4,0x93,0xA1,0x87);

#ifndef ___DHistAX_DISPINTERFACE_DEFINED__
#define ___DHistAX_DISPINTERFACE_DEFINED__

/* dispinterface _DHistAX */
/* [hidden][helpstring][uuid] */ 


DEFINE_GUID(DIID__DHistAX,0x06FDDE56,0xF981,0x11D3,0xA0,0xC1,0x00,0x00,0xB4,0x93,0xA1,0x87);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("06FDDE56-F981-11D3-A0C1-0000B493A187")
    _DHistAX : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DHistAXVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DHistAX * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DHistAX * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DHistAX * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DHistAX * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DHistAX * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DHistAX * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DHistAX * This,
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
    } _DHistAXVtbl;

    interface _DHistAX
    {
        CONST_VTBL struct _DHistAXVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DHistAX_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DHistAX_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DHistAX_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DHistAX_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DHistAX_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DHistAX_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DHistAX_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DHistAX_DISPINTERFACE_DEFINED__ */


#ifndef ___DHistAXEvents_DISPINTERFACE_DEFINED__
#define ___DHistAXEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DHistAXEvents */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DHistAXEvents,0x06FDDE57,0xF981,0x11D3,0xA0,0xC1,0x00,0x00,0xB4,0x93,0xA1,0x87);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("06FDDE57-F981-11D3-A0C1-0000B493A187")
    _DHistAXEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DHistAXEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DHistAXEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DHistAXEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DHistAXEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DHistAXEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DHistAXEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DHistAXEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DHistAXEvents * This,
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
    } _DHistAXEventsVtbl;

    interface _DHistAXEvents
    {
        CONST_VTBL struct _DHistAXEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DHistAXEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DHistAXEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DHistAXEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DHistAXEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DHistAXEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DHistAXEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DHistAXEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DHistAXEvents_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_HistAX,0x65BD787C,0xFDC7,0x4f83,0xA7,0x88,0x13,0xE6,0x26,0x68,0x35,0xE9);

#ifdef __cplusplus

class DECLSPEC_UUID("65BD787C-FDC7-4f83-A788-13E6266835E9")
HistAX;
#endif
#endif /* __HISTAXLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


