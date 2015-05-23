

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Apr 04 21:04:47 2015
 */
/* Compiler settings for ViewAX.odl:
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


#ifndef __ViewAX_h_h__
#define __ViewAX_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef ___DViewAX_FWD_DEFINED__
#define ___DViewAX_FWD_DEFINED__
typedef interface _DViewAX _DViewAX;

#endif 	/* ___DViewAX_FWD_DEFINED__ */


#ifndef ___DViewAXEvents_FWD_DEFINED__
#define ___DViewAXEvents_FWD_DEFINED__
typedef interface _DViewAXEvents _DViewAXEvents;

#endif 	/* ___DViewAXEvents_FWD_DEFINED__ */


#ifndef __ViewAX_FWD_DEFINED__
#define __ViewAX_FWD_DEFINED__

#ifdef __cplusplus
typedef class ViewAX ViewAX;
#else
typedef struct ViewAX ViewAX;
#endif /* __cplusplus */

#endif 	/* __ViewAX_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __VIEWAXLib_LIBRARY_DEFINED__
#define __VIEWAXLib_LIBRARY_DEFINED__

/* library VIEWAXLib */
/* [control][helpstring][helpfile][version][uuid] */ 

#pragma once
#pragma region Desktop Family
#pragma endregion

DEFINE_GUID(LIBID_VIEWAXLib,0x225ABEF5,0x2C0D,0x4804,0xB9,0x2D,0xA3,0x54,0x26,0x8F,0xBA,0x44);

#ifndef ___DViewAX_DISPINTERFACE_DEFINED__
#define ___DViewAX_DISPINTERFACE_DEFINED__

/* dispinterface _DViewAX */
/* [hidden][helpstring][uuid] */ 


DEFINE_GUID(DIID__DViewAX,0xA108B56B,0xB544,0x4EB3,0xAC,0x1D,0x10,0x00,0x64,0x35,0xD2,0x03);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("A108B56B-B544-4EB3-AC1D-10006435D203")
    _DViewAX : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DViewAXVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DViewAX * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DViewAX * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DViewAX * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DViewAX * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DViewAX * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DViewAX * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DViewAX * This,
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
    } _DViewAXVtbl;

    interface _DViewAX
    {
        CONST_VTBL struct _DViewAXVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DViewAX_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DViewAX_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DViewAX_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DViewAX_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DViewAX_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DViewAX_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DViewAX_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DViewAX_DISPINTERFACE_DEFINED__ */


#ifndef ___DViewAXEvents_DISPINTERFACE_DEFINED__
#define ___DViewAXEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DViewAXEvents */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DViewAXEvents,0x5A5575EA,0xBB7D,0x45A3,0x97,0x1B,0x8F,0x21,0x81,0xD7,0xE6,0x83);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("5A5575EA-BB7D-45A3-971B-8F2181D7E683")
    _DViewAXEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DViewAXEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DViewAXEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DViewAXEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DViewAXEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DViewAXEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DViewAXEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DViewAXEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DViewAXEvents * This,
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
    } _DViewAXEventsVtbl;

    interface _DViewAXEvents
    {
        CONST_VTBL struct _DViewAXEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DViewAXEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DViewAXEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DViewAXEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DViewAXEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DViewAXEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DViewAXEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DViewAXEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DViewAXEvents_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_ViewAX,0x14C00616,0x4768,0x4091,0x92,0x31,0x4D,0x18,0xC1,0x01,0xFF,0xBB);

#ifdef __cplusplus

class DECLSPEC_UUID("14C00616-4768-4091-9231-4D18C101FFBB")
ViewAX;
#endif
#endif /* __VIEWAXLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


