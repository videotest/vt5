

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Apr 04 21:03:06 2015
 */
/* Compiler settings for VTControls.odl:
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


#ifndef __VTControlsidl_h__
#define __VTControlsidl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef ___DVTKeyButton_FWD_DEFINED__
#define ___DVTKeyButton_FWD_DEFINED__
typedef interface _DVTKeyButton _DVTKeyButton;

#endif 	/* ___DVTKeyButton_FWD_DEFINED__ */


#ifndef ___DVTKeyButtonEvents_FWD_DEFINED__
#define ___DVTKeyButtonEvents_FWD_DEFINED__
typedef interface _DVTKeyButtonEvents _DVTKeyButtonEvents;

#endif 	/* ___DVTKeyButtonEvents_FWD_DEFINED__ */


#ifndef __VTKeyButton_FWD_DEFINED__
#define __VTKeyButton_FWD_DEFINED__

#ifdef __cplusplus
typedef class VTKeyButton VTKeyButton;
#else
typedef struct VTKeyButton VTKeyButton;
#endif /* __cplusplus */

#endif 	/* __VTKeyButton_FWD_DEFINED__ */


#ifndef ___DVTStaticText_FWD_DEFINED__
#define ___DVTStaticText_FWD_DEFINED__
typedef interface _DVTStaticText _DVTStaticText;

#endif 	/* ___DVTStaticText_FWD_DEFINED__ */


#ifndef ___DVTStaticTextEvents_FWD_DEFINED__
#define ___DVTStaticTextEvents_FWD_DEFINED__
typedef interface _DVTStaticTextEvents _DVTStaticTextEvents;

#endif 	/* ___DVTStaticTextEvents_FWD_DEFINED__ */


#ifndef __VTStaticText_FWD_DEFINED__
#define __VTStaticText_FWD_DEFINED__

#ifdef __cplusplus
typedef class VTStaticText VTStaticText;
#else
typedef struct VTStaticText VTStaticText;
#endif /* __cplusplus */

#endif 	/* __VTStaticText_FWD_DEFINED__ */


#ifndef ___DVTEditor_FWD_DEFINED__
#define ___DVTEditor_FWD_DEFINED__
typedef interface _DVTEditor _DVTEditor;

#endif 	/* ___DVTEditor_FWD_DEFINED__ */


#ifndef ___DVTEditorEvents_FWD_DEFINED__
#define ___DVTEditorEvents_FWD_DEFINED__
typedef interface _DVTEditorEvents _DVTEditorEvents;

#endif 	/* ___DVTEditorEvents_FWD_DEFINED__ */


#ifndef __VTEditor_FWD_DEFINED__
#define __VTEditor_FWD_DEFINED__

#ifdef __cplusplus
typedef class VTEditor VTEditor;
#else
typedef struct VTEditor VTEditor;
#endif /* __cplusplus */

#endif 	/* __VTEditor_FWD_DEFINED__ */


#ifndef ___DVTList_FWD_DEFINED__
#define ___DVTList_FWD_DEFINED__
typedef interface _DVTList _DVTList;

#endif 	/* ___DVTList_FWD_DEFINED__ */


#ifndef ___DVTListEvents_FWD_DEFINED__
#define ___DVTListEvents_FWD_DEFINED__
typedef interface _DVTListEvents _DVTListEvents;

#endif 	/* ___DVTListEvents_FWD_DEFINED__ */


#ifndef __VTList_FWD_DEFINED__
#define __VTList_FWD_DEFINED__

#ifdef __cplusplus
typedef class VTList VTList;
#else
typedef struct VTList VTList;
#endif /* __cplusplus */

#endif 	/* __VTList_FWD_DEFINED__ */


#ifndef ___DVTSlider_FWD_DEFINED__
#define ___DVTSlider_FWD_DEFINED__
typedef interface _DVTSlider _DVTSlider;

#endif 	/* ___DVTSlider_FWD_DEFINED__ */


#ifndef ___DVTSliderEvents_FWD_DEFINED__
#define ___DVTSliderEvents_FWD_DEFINED__
typedef interface _DVTSliderEvents _DVTSliderEvents;

#endif 	/* ___DVTSliderEvents_FWD_DEFINED__ */


#ifndef __VTSlider_FWD_DEFINED__
#define __VTSlider_FWD_DEFINED__

#ifdef __cplusplus
typedef class VTSlider VTSlider;
#else
typedef struct VTSlider VTSlider;
#endif /* __cplusplus */

#endif 	/* __VTSlider_FWD_DEFINED__ */


#ifndef ___DPushButton_FWD_DEFINED__
#define ___DPushButton_FWD_DEFINED__
typedef interface _DPushButton _DPushButton;

#endif 	/* ___DPushButton_FWD_DEFINED__ */


#ifndef ___DPushButtonEvents_FWD_DEFINED__
#define ___DPushButtonEvents_FWD_DEFINED__
typedef interface _DPushButtonEvents _DPushButtonEvents;

#endif 	/* ___DPushButtonEvents_FWD_DEFINED__ */


#ifndef __PushButton_FWD_DEFINED__
#define __PushButton_FWD_DEFINED__

#ifdef __cplusplus
typedef class PushButton PushButton;
#else
typedef struct PushButton PushButton;
#endif /* __cplusplus */

#endif 	/* __PushButton_FWD_DEFINED__ */


#ifndef ___DVTCheckBox_FWD_DEFINED__
#define ___DVTCheckBox_FWD_DEFINED__
typedef interface _DVTCheckBox _DVTCheckBox;

#endif 	/* ___DVTCheckBox_FWD_DEFINED__ */


#ifndef ___DVTCheckBoxEvents_FWD_DEFINED__
#define ___DVTCheckBoxEvents_FWD_DEFINED__
typedef interface _DVTCheckBoxEvents _DVTCheckBoxEvents;

#endif 	/* ___DVTCheckBoxEvents_FWD_DEFINED__ */


#ifndef __VTCheckBox_FWD_DEFINED__
#define __VTCheckBox_FWD_DEFINED__

#ifdef __cplusplus
typedef class VTCheckBox VTCheckBox;
#else
typedef struct VTCheckBox VTCheckBox;
#endif /* __cplusplus */

#endif 	/* __VTCheckBox_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_VTControls_0000_0000 */
/* [local] */ 

#pragma once
#pragma region Desktop Family
#pragma endregion


extern RPC_IF_HANDLE __MIDL_itf_VTControls_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_VTControls_0000_0000_v0_0_s_ifspec;


#ifndef __VTCONTROLSLib_LIBRARY_DEFINED__
#define __VTCONTROLSLib_LIBRARY_DEFINED__

/* library VTCONTROLSLib */
/* [control][helpstring][helpfile][version][uuid] */ 

typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_VTControls_0000_0000_0001
    {
        caAlignUp	= 0,
        caAlignDown	= 1
    } 	_CaptionAlignEnum;

typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_VTControls_0000_0000_0002
    {
        iahLeft	= 0,
        iahCenter	= 1,
        iahRight	= 2
    } 	_ImageAlignHorzEnum;

typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_VTControls_0000_0000_0003
    {
        iavTop	= 0,
        iavCenter	= 1,
        iavBottom	= 2
    } 	_ImageAlignVertEnum;

typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_VTControls_0000_0000_0004
    {
        dtTypeString	= 0,
        dtTypeInt	= 1,
        dtTypeFloat	= 2
    } 	_DataTypeEnum;


EXTERN_C const IID LIBID_VTCONTROLSLib;

#ifndef ___DVTKeyButton_DISPINTERFACE_DEFINED__
#define ___DVTKeyButton_DISPINTERFACE_DEFINED__

/* dispinterface _DVTKeyButton */
/* [hidden][helpstring][uuid] */ 


EXTERN_C const IID DIID__DVTKeyButton;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("ADD6255F-8137-4EAA-967F-3AC5CA9A7607")
    _DVTKeyButton : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTKeyButtonVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTKeyButton * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTKeyButton * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTKeyButton * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTKeyButton * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTKeyButton * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTKeyButton * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTKeyButton * This,
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
    } _DVTKeyButtonVtbl;

    interface _DVTKeyButton
    {
        CONST_VTBL struct _DVTKeyButtonVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTKeyButton_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTKeyButton_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTKeyButton_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTKeyButton_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTKeyButton_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTKeyButton_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTKeyButton_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTKeyButton_DISPINTERFACE_DEFINED__ */


#ifndef ___DVTKeyButtonEvents_DISPINTERFACE_DEFINED__
#define ___DVTKeyButtonEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DVTKeyButtonEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__DVTKeyButtonEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("D6B94F54-FE1E-4D28-9B84-85F8BA853863")
    _DVTKeyButtonEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTKeyButtonEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTKeyButtonEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTKeyButtonEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTKeyButtonEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTKeyButtonEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTKeyButtonEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTKeyButtonEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTKeyButtonEvents * This,
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
    } _DVTKeyButtonEventsVtbl;

    interface _DVTKeyButtonEvents
    {
        CONST_VTBL struct _DVTKeyButtonEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTKeyButtonEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTKeyButtonEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTKeyButtonEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTKeyButtonEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTKeyButtonEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTKeyButtonEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTKeyButtonEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTKeyButtonEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_VTKeyButton;

#ifdef __cplusplus

class DECLSPEC_UUID("6743979D-D00F-48A0-A036-DD7EE33A30B3")
VTKeyButton;
#endif

#ifndef ___DVTStaticText_DISPINTERFACE_DEFINED__
#define ___DVTStaticText_DISPINTERFACE_DEFINED__

/* dispinterface _DVTStaticText */
/* [hidden][helpstring][uuid] */ 


EXTERN_C const IID DIID__DVTStaticText;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("B8E8ACA1-F32D-4D62-8EE0-28AF2684CA5D")
    _DVTStaticText : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTStaticTextVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTStaticText * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTStaticText * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTStaticText * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTStaticText * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTStaticText * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTStaticText * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTStaticText * This,
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
    } _DVTStaticTextVtbl;

    interface _DVTStaticText
    {
        CONST_VTBL struct _DVTStaticTextVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTStaticText_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTStaticText_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTStaticText_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTStaticText_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTStaticText_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTStaticText_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTStaticText_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTStaticText_DISPINTERFACE_DEFINED__ */


#ifndef ___DVTStaticTextEvents_DISPINTERFACE_DEFINED__
#define ___DVTStaticTextEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DVTStaticTextEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__DVTStaticTextEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("9886E2C0-3CA3-4BCE-9712-5F884EA1EEF8")
    _DVTStaticTextEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTStaticTextEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTStaticTextEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTStaticTextEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTStaticTextEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTStaticTextEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTStaticTextEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTStaticTextEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTStaticTextEvents * This,
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
    } _DVTStaticTextEventsVtbl;

    interface _DVTStaticTextEvents
    {
        CONST_VTBL struct _DVTStaticTextEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTStaticTextEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTStaticTextEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTStaticTextEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTStaticTextEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTStaticTextEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTStaticTextEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTStaticTextEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTStaticTextEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_VTStaticText;

#ifdef __cplusplus

class DECLSPEC_UUID("D962DECE-FA07-47DC-A901-73DEE44FF021")
VTStaticText;
#endif

#ifndef ___DVTEditor_DISPINTERFACE_DEFINED__
#define ___DVTEditor_DISPINTERFACE_DEFINED__

/* dispinterface _DVTEditor */
/* [hidden][helpstring][uuid] */ 


EXTERN_C const IID DIID__DVTEditor;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("19B862BF-87E2-42C7-9831-57B6A65C369E")
    _DVTEditor : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTEditorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTEditor * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTEditor * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTEditor * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTEditor * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTEditor * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTEditor * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTEditor * This,
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
    } _DVTEditorVtbl;

    interface _DVTEditor
    {
        CONST_VTBL struct _DVTEditorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTEditor_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTEditor_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTEditor_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTEditor_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTEditor_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTEditor_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTEditor_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTEditor_DISPINTERFACE_DEFINED__ */


#ifndef ___DVTEditorEvents_DISPINTERFACE_DEFINED__
#define ___DVTEditorEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DVTEditorEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__DVTEditorEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("E96348B8-FA6A-4F9D-86B8-25792CDF4681")
    _DVTEditorEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTEditorEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTEditorEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTEditorEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTEditorEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTEditorEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTEditorEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTEditorEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTEditorEvents * This,
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
    } _DVTEditorEventsVtbl;

    interface _DVTEditorEvents
    {
        CONST_VTBL struct _DVTEditorEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTEditorEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTEditorEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTEditorEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTEditorEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTEditorEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTEditorEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTEditorEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTEditorEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_VTEditor;

#ifdef __cplusplus

class DECLSPEC_UUID("6BDF8B84-C1B9-448E-8965-0E4557467FDA")
VTEditor;
#endif

#ifndef ___DVTList_DISPINTERFACE_DEFINED__
#define ___DVTList_DISPINTERFACE_DEFINED__

/* dispinterface _DVTList */
/* [hidden][helpstring][uuid] */ 


EXTERN_C const IID DIID__DVTList;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("7850253B-44E2-44C5-A728-37DAAC031F2A")
    _DVTList : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTList * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTList * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTList * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTList * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTList * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTList * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTList * This,
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
    } _DVTListVtbl;

    interface _DVTList
    {
        CONST_VTBL struct _DVTListVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTList_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTList_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTList_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTList_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTList_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTList_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTList_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTList_DISPINTERFACE_DEFINED__ */


#ifndef ___DVTListEvents_DISPINTERFACE_DEFINED__
#define ___DVTListEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DVTListEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__DVTListEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("BD593DDD-9B97-4717-A77A-402F0C4D223A")
    _DVTListEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTListEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTListEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTListEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTListEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTListEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTListEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTListEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTListEvents * This,
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
    } _DVTListEventsVtbl;

    interface _DVTListEvents
    {
        CONST_VTBL struct _DVTListEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTListEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTListEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTListEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTListEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTListEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTListEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTListEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTListEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_VTList;

#ifdef __cplusplus

class DECLSPEC_UUID("AD7C61B8-1356-432F-BEBA-1F94620A9F3D")
VTList;
#endif

#ifndef ___DVTSlider_DISPINTERFACE_DEFINED__
#define ___DVTSlider_DISPINTERFACE_DEFINED__

/* dispinterface _DVTSlider */
/* [hidden][helpstring][uuid] */ 


EXTERN_C const IID DIID__DVTSlider;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("BF93DFCE-D919-4AB3-AC2E-AA7E75069D38")
    _DVTSlider : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTSliderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTSlider * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTSlider * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTSlider * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTSlider * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTSlider * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTSlider * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTSlider * This,
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
    } _DVTSliderVtbl;

    interface _DVTSlider
    {
        CONST_VTBL struct _DVTSliderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTSlider_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTSlider_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTSlider_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTSlider_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTSlider_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTSlider_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTSlider_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTSlider_DISPINTERFACE_DEFINED__ */


#ifndef ___DVTSliderEvents_DISPINTERFACE_DEFINED__
#define ___DVTSliderEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DVTSliderEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__DVTSliderEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("EDFD94E7-7B71-4251-9808-5C3BB3A5CC51")
    _DVTSliderEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTSliderEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTSliderEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTSliderEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTSliderEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTSliderEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTSliderEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTSliderEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTSliderEvents * This,
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
    } _DVTSliderEventsVtbl;

    interface _DVTSliderEvents
    {
        CONST_VTBL struct _DVTSliderEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTSliderEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTSliderEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTSliderEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTSliderEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTSliderEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTSliderEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTSliderEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTSliderEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_VTSlider;

#ifdef __cplusplus

class DECLSPEC_UUID("232343F7-5D35-475E-8FF7-F24E9118AB08")
VTSlider;
#endif

#ifndef ___DPushButton_DISPINTERFACE_DEFINED__
#define ___DPushButton_DISPINTERFACE_DEFINED__

/* dispinterface _DPushButton */
/* [hidden][helpstring][uuid] */ 


EXTERN_C const IID DIID__DPushButton;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("54A0BF84-9A92-11D3-B1C5-EB19BDBABA39")
    _DPushButton : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DPushButtonVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DPushButton * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DPushButton * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DPushButton * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DPushButton * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DPushButton * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DPushButton * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DPushButton * This,
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
    } _DPushButtonVtbl;

    interface _DPushButton
    {
        CONST_VTBL struct _DPushButtonVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DPushButton_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DPushButton_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DPushButton_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DPushButton_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DPushButton_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DPushButton_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DPushButton_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DPushButton_DISPINTERFACE_DEFINED__ */


#ifndef ___DPushButtonEvents_DISPINTERFACE_DEFINED__
#define ___DPushButtonEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DPushButtonEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__DPushButtonEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("54A0BF85-9A92-11D3-B1C5-EB19BDBABA39")
    _DPushButtonEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DPushButtonEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DPushButtonEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DPushButtonEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DPushButtonEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DPushButtonEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DPushButtonEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DPushButtonEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DPushButtonEvents * This,
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
    } _DPushButtonEventsVtbl;

    interface _DPushButtonEvents
    {
        CONST_VTBL struct _DPushButtonEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DPushButtonEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DPushButtonEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DPushButtonEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DPushButtonEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DPushButtonEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DPushButtonEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DPushButtonEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DPushButtonEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_PushButton;

#ifdef __cplusplus

class DECLSPEC_UUID("54A0BF86-9A92-11D3-B1C5-EB19BDBABA39")
PushButton;
#endif

#ifndef ___DVTCheckBox_DISPINTERFACE_DEFINED__
#define ___DVTCheckBox_DISPINTERFACE_DEFINED__

/* dispinterface _DVTCheckBox */
/* [hidden][helpstring][uuid] */ 


EXTERN_C const IID DIID__DVTCheckBox;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("9899E4F1-0A5E-47F9-8AB4-8F9F09F507C1")
    _DVTCheckBox : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTCheckBoxVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTCheckBox * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTCheckBox * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTCheckBox * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTCheckBox * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTCheckBox * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTCheckBox * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTCheckBox * This,
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
    } _DVTCheckBoxVtbl;

    interface _DVTCheckBox
    {
        CONST_VTBL struct _DVTCheckBoxVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTCheckBox_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTCheckBox_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTCheckBox_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTCheckBox_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTCheckBox_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTCheckBox_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTCheckBox_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTCheckBox_DISPINTERFACE_DEFINED__ */


#ifndef ___DVTCheckBoxEvents_DISPINTERFACE_DEFINED__
#define ___DVTCheckBoxEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DVTCheckBoxEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__DVTCheckBoxEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("B8C07E38-3C8F-41A5-9E3A-45B4F050DAC4")
    _DVTCheckBoxEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTCheckBoxEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTCheckBoxEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTCheckBoxEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTCheckBoxEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTCheckBoxEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTCheckBoxEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTCheckBoxEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTCheckBoxEvents * This,
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
    } _DVTCheckBoxEventsVtbl;

    interface _DVTCheckBoxEvents
    {
        CONST_VTBL struct _DVTCheckBoxEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTCheckBoxEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTCheckBoxEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTCheckBoxEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTCheckBoxEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTCheckBoxEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTCheckBoxEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTCheckBoxEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTCheckBoxEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_VTCheckBox;

#ifdef __cplusplus

class DECLSPEC_UUID("F6188DC0-9F7F-4021-AA3C-395956D021F1")
VTCheckBox;
#endif
#endif /* __VTCONTROLSLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


