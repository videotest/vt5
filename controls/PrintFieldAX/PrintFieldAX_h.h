

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Apr 04 21:04:33 2015
 */
/* Compiler settings for PrintFieldAX.odl:
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


#ifndef __PrintFieldAX_h_h__
#define __PrintFieldAX_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef ___DPrintFieldAX_FWD_DEFINED__
#define ___DPrintFieldAX_FWD_DEFINED__
typedef interface _DPrintFieldAX _DPrintFieldAX;

#endif 	/* ___DPrintFieldAX_FWD_DEFINED__ */


#ifndef ___DPrintFieldAXEvents_FWD_DEFINED__
#define ___DPrintFieldAXEvents_FWD_DEFINED__
typedef interface _DPrintFieldAXEvents _DPrintFieldAXEvents;

#endif 	/* ___DPrintFieldAXEvents_FWD_DEFINED__ */


#ifndef __PrintFieldAX_FWD_DEFINED__
#define __PrintFieldAX_FWD_DEFINED__

#ifdef __cplusplus
typedef class PrintFieldAX PrintFieldAX;
#else
typedef struct PrintFieldAX PrintFieldAX;
#endif /* __cplusplus */

#endif 	/* __PrintFieldAX_FWD_DEFINED__ */


#ifndef ___DViewLoader_FWD_DEFINED__
#define ___DViewLoader_FWD_DEFINED__
typedef interface _DViewLoader _DViewLoader;

#endif 	/* ___DViewLoader_FWD_DEFINED__ */


#ifndef ___DViewLoaderEvents_FWD_DEFINED__
#define ___DViewLoaderEvents_FWD_DEFINED__
typedef interface _DViewLoaderEvents _DViewLoaderEvents;

#endif 	/* ___DViewLoaderEvents_FWD_DEFINED__ */


#ifndef __ViewLoader_FWD_DEFINED__
#define __ViewLoader_FWD_DEFINED__

#ifdef __cplusplus
typedef class ViewLoader ViewLoader;
#else
typedef struct ViewLoader ViewLoader;
#endif /* __cplusplus */

#endif 	/* __ViewLoader_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __PRINTFIELDAXLib_LIBRARY_DEFINED__
#define __PRINTFIELDAXLib_LIBRARY_DEFINED__

/* library PRINTFIELDAXLib */
/* [control][helpstring][helpfile][version][uuid] */ 

#pragma once
#pragma region Desktop Family
#pragma endregion

DEFINE_GUID(LIBID_PRINTFIELDAXLib,0xE72BB405,0xD5B6,0x4CA6,0x90,0x71,0x56,0xAB,0xC5,0xE1,0xDB,0x40);

#ifndef ___DPrintFieldAX_DISPINTERFACE_DEFINED__
#define ___DPrintFieldAX_DISPINTERFACE_DEFINED__

/* dispinterface _DPrintFieldAX */
/* [hidden][helpstring][uuid] */ 


DEFINE_GUID(DIID__DPrintFieldAX,0x3851E866,0xC6D3,0x46C4,0xBD,0x74,0xEB,0x3B,0x16,0xE6,0x06,0x92);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("3851E866-C6D3-46C4-BD74-EB3B16E60692")
    _DPrintFieldAX : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DPrintFieldAXVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DPrintFieldAX * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DPrintFieldAX * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DPrintFieldAX * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DPrintFieldAX * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DPrintFieldAX * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DPrintFieldAX * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DPrintFieldAX * This,
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
    } _DPrintFieldAXVtbl;

    interface _DPrintFieldAX
    {
        CONST_VTBL struct _DPrintFieldAXVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DPrintFieldAX_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DPrintFieldAX_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DPrintFieldAX_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DPrintFieldAX_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DPrintFieldAX_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DPrintFieldAX_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DPrintFieldAX_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DPrintFieldAX_DISPINTERFACE_DEFINED__ */


#ifndef ___DPrintFieldAXEvents_DISPINTERFACE_DEFINED__
#define ___DPrintFieldAXEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DPrintFieldAXEvents */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DPrintFieldAXEvents,0x453AD094,0xD102,0x42CD,0x86,0x6D,0xEF,0x4E,0x80,0xDA,0xA2,0x43);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("453AD094-D102-42CD-866D-EF4E80DAA243")
    _DPrintFieldAXEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DPrintFieldAXEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DPrintFieldAXEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DPrintFieldAXEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DPrintFieldAXEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DPrintFieldAXEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DPrintFieldAXEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DPrintFieldAXEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DPrintFieldAXEvents * This,
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
    } _DPrintFieldAXEventsVtbl;

    interface _DPrintFieldAXEvents
    {
        CONST_VTBL struct _DPrintFieldAXEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DPrintFieldAXEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DPrintFieldAXEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DPrintFieldAXEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DPrintFieldAXEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DPrintFieldAXEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DPrintFieldAXEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DPrintFieldAXEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DPrintFieldAXEvents_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_PrintFieldAX,0x38E8C2F4,0xE97A,0x44C9,0xB0,0xAC,0xE9,0x59,0xE8,0x05,0xF5,0x3D);

#ifdef __cplusplus

class DECLSPEC_UUID("38E8C2F4-E97A-44C9-B0AC-E959E805F53D")
PrintFieldAX;
#endif

#ifndef ___DViewLoader_DISPINTERFACE_DEFINED__
#define ___DViewLoader_DISPINTERFACE_DEFINED__

/* dispinterface _DViewLoader */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DViewLoader,0x08305E68,0xA9B4,0x4069,0x96,0xD4,0xEC,0xBA,0xF5,0x93,0xCE,0x5C);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("08305E68-A9B4-4069-96D4-ECBAF593CE5C")
    _DViewLoader : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DViewLoaderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DViewLoader * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DViewLoader * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DViewLoader * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DViewLoader * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DViewLoader * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DViewLoader * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DViewLoader * This,
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
    } _DViewLoaderVtbl;

    interface _DViewLoader
    {
        CONST_VTBL struct _DViewLoaderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DViewLoader_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DViewLoader_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DViewLoader_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DViewLoader_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DViewLoader_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DViewLoader_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DViewLoader_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DViewLoader_DISPINTERFACE_DEFINED__ */


#ifndef ___DViewLoaderEvents_DISPINTERFACE_DEFINED__
#define ___DViewLoaderEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DViewLoaderEvents */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DViewLoaderEvents,0x8360C193,0x90B2,0x4972,0xA2,0xA0,0xC5,0x1C,0x20,0xDD,0xCB,0xE6);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("8360C193-90B2-4972-A2A0-C51C20DDCBE6")
    _DViewLoaderEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DViewLoaderEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DViewLoaderEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DViewLoaderEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DViewLoaderEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DViewLoaderEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DViewLoaderEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DViewLoaderEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DViewLoaderEvents * This,
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
    } _DViewLoaderEventsVtbl;

    interface _DViewLoaderEvents
    {
        CONST_VTBL struct _DViewLoaderEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DViewLoaderEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DViewLoaderEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DViewLoaderEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DViewLoaderEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DViewLoaderEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DViewLoaderEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DViewLoaderEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DViewLoaderEvents_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_ViewLoader,0x67DF5557,0x7009,0x432C,0x8C,0x3C,0x1E,0x56,0x4C,0x07,0x48,0x11);

#ifdef __cplusplus

class DECLSPEC_UUID("67DF5557-7009-432C-8C3C-1E564C074811")
ViewLoader;
#endif
#endif /* __PRINTFIELDAXLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


