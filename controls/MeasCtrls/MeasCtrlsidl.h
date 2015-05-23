

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Apr 04 21:05:46 2015
 */
/* Compiler settings for MeasCtrls.idl:
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


#ifndef __MeasCtrlsidl_h__
#define __MeasCtrlsidl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef ___DMeasCtrls_FWD_DEFINED__
#define ___DMeasCtrls_FWD_DEFINED__
typedef interface _DMeasCtrls _DMeasCtrls;

#endif 	/* ___DMeasCtrls_FWD_DEFINED__ */


#ifndef ___DMeasCtrlsEvents_FWD_DEFINED__
#define ___DMeasCtrlsEvents_FWD_DEFINED__
typedef interface _DMeasCtrlsEvents _DMeasCtrlsEvents;

#endif 	/* ___DMeasCtrlsEvents_FWD_DEFINED__ */


#ifndef __MeasCtrls_FWD_DEFINED__
#define __MeasCtrls_FWD_DEFINED__

#ifdef __cplusplus
typedef class MeasCtrls MeasCtrls;
#else
typedef struct MeasCtrls MeasCtrls;
#endif /* __cplusplus */

#endif 	/* __MeasCtrls_FWD_DEFINED__ */


#ifndef ___DChoiceGreekSymbol_FWD_DEFINED__
#define ___DChoiceGreekSymbol_FWD_DEFINED__
typedef interface _DChoiceGreekSymbol _DChoiceGreekSymbol;

#endif 	/* ___DChoiceGreekSymbol_FWD_DEFINED__ */


#ifndef ___DChoiceGreekSymbolEvents_FWD_DEFINED__
#define ___DChoiceGreekSymbolEvents_FWD_DEFINED__
typedef interface _DChoiceGreekSymbolEvents _DChoiceGreekSymbolEvents;

#endif 	/* ___DChoiceGreekSymbolEvents_FWD_DEFINED__ */


#ifndef __ChoiceGreekSymbol_FWD_DEFINED__
#define __ChoiceGreekSymbol_FWD_DEFINED__

#ifdef __cplusplus
typedef class ChoiceGreekSymbol ChoiceGreekSymbol;
#else
typedef struct ChoiceGreekSymbol ChoiceGreekSymbol;
#endif /* __cplusplus */

#endif 	/* __ChoiceGreekSymbol_FWD_DEFINED__ */


#ifndef ___DUserParamsListBox_FWD_DEFINED__
#define ___DUserParamsListBox_FWD_DEFINED__
typedef interface _DUserParamsListBox _DUserParamsListBox;

#endif 	/* ___DUserParamsListBox_FWD_DEFINED__ */


#ifndef ___DUserParamsListBoxEvents_FWD_DEFINED__
#define ___DUserParamsListBoxEvents_FWD_DEFINED__
typedef interface _DUserParamsListBoxEvents _DUserParamsListBoxEvents;

#endif 	/* ___DUserParamsListBoxEvents_FWD_DEFINED__ */


#ifndef __UserParamsListBox_FWD_DEFINED__
#define __UserParamsListBox_FWD_DEFINED__

#ifdef __cplusplus
typedef class UserParamsListBox UserParamsListBox;
#else
typedef struct UserParamsListBox UserParamsListBox;
#endif /* __cplusplus */

#endif 	/* __UserParamsListBox_FWD_DEFINED__ */


#ifndef ___DUserParamsComboBox_FWD_DEFINED__
#define ___DUserParamsComboBox_FWD_DEFINED__
typedef interface _DUserParamsComboBox _DUserParamsComboBox;

#endif 	/* ___DUserParamsComboBox_FWD_DEFINED__ */


#ifndef ___DUserParamsComboBoxEvents_FWD_DEFINED__
#define ___DUserParamsComboBoxEvents_FWD_DEFINED__
typedef interface _DUserParamsComboBoxEvents _DUserParamsComboBoxEvents;

#endif 	/* ___DUserParamsComboBoxEvents_FWD_DEFINED__ */


#ifndef __UserParamsComboBox_FWD_DEFINED__
#define __UserParamsComboBox_FWD_DEFINED__

#ifdef __cplusplus
typedef class UserParamsComboBox UserParamsComboBox;
#else
typedef struct UserParamsComboBox UserParamsComboBox;
#endif /* __cplusplus */

#endif 	/* __UserParamsComboBox_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __MeasCtrlsLib_LIBRARY_DEFINED__
#define __MeasCtrlsLib_LIBRARY_DEFINED__

/* library MeasCtrlsLib */
/* [control][helpstring][helpfile][version][uuid] */ 

#pragma once
#pragma region Desktop Family
#pragma endregion

DEFINE_GUID(LIBID_MeasCtrlsLib,0x40075923,0x4631,0x4A92,0xA6,0xBE,0x6F,0xA8,0x27,0x44,0x08,0x7A);

#ifndef ___DMeasCtrls_DISPINTERFACE_DEFINED__
#define ___DMeasCtrls_DISPINTERFACE_DEFINED__

/* dispinterface _DMeasCtrls */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DMeasCtrls,0xC3FAB755,0xE612,0x4640,0xA1,0xD8,0xA0,0x3B,0xDF,0xFB,0x6A,0xA1);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("C3FAB755-E612-4640-A1D8-A03BDFFB6AA1")
    _DMeasCtrls : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DMeasCtrlsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DMeasCtrls * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DMeasCtrls * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DMeasCtrls * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DMeasCtrls * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DMeasCtrls * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DMeasCtrls * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DMeasCtrls * This,
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
    } _DMeasCtrlsVtbl;

    interface _DMeasCtrls
    {
        CONST_VTBL struct _DMeasCtrlsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DMeasCtrls_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DMeasCtrls_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DMeasCtrls_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DMeasCtrls_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DMeasCtrls_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DMeasCtrls_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DMeasCtrls_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DMeasCtrls_DISPINTERFACE_DEFINED__ */


#ifndef ___DMeasCtrlsEvents_DISPINTERFACE_DEFINED__
#define ___DMeasCtrlsEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DMeasCtrlsEvents */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DMeasCtrlsEvents,0xB592174D,0x0E82,0x48DB,0xBB,0xCC,0xFD,0x22,0xC5,0x49,0x2F,0x58);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("B592174D-0E82-48DB-BBCC-FD22C5492F58")
    _DMeasCtrlsEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DMeasCtrlsEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DMeasCtrlsEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DMeasCtrlsEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DMeasCtrlsEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DMeasCtrlsEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DMeasCtrlsEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DMeasCtrlsEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DMeasCtrlsEvents * This,
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
    } _DMeasCtrlsEventsVtbl;

    interface _DMeasCtrlsEvents
    {
        CONST_VTBL struct _DMeasCtrlsEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DMeasCtrlsEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DMeasCtrlsEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DMeasCtrlsEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DMeasCtrlsEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DMeasCtrlsEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DMeasCtrlsEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DMeasCtrlsEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DMeasCtrlsEvents_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_MeasCtrls,0x9ACF8F66,0xB50F,0x46F9,0xAA,0x45,0xDB,0x14,0x27,0x6E,0xFA,0xCC);

#ifdef __cplusplus

class DECLSPEC_UUID("9ACF8F66-B50F-46F9-AA45-DB14276EFACC")
MeasCtrls;
#endif

#ifndef ___DChoiceGreekSymbol_DISPINTERFACE_DEFINED__
#define ___DChoiceGreekSymbol_DISPINTERFACE_DEFINED__

/* dispinterface _DChoiceGreekSymbol */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DChoiceGreekSymbol,0xD1BF7023,0x90E9,0x4DD6,0x9F,0x4A,0x2D,0x90,0x3A,0x6A,0xDD,0x0F);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("D1BF7023-90E9-4DD6-9F4A-2D903A6ADD0F")
    _DChoiceGreekSymbol : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DChoiceGreekSymbolVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DChoiceGreekSymbol * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DChoiceGreekSymbol * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DChoiceGreekSymbol * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DChoiceGreekSymbol * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DChoiceGreekSymbol * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DChoiceGreekSymbol * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DChoiceGreekSymbol * This,
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
    } _DChoiceGreekSymbolVtbl;

    interface _DChoiceGreekSymbol
    {
        CONST_VTBL struct _DChoiceGreekSymbolVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DChoiceGreekSymbol_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DChoiceGreekSymbol_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DChoiceGreekSymbol_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DChoiceGreekSymbol_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DChoiceGreekSymbol_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DChoiceGreekSymbol_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DChoiceGreekSymbol_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DChoiceGreekSymbol_DISPINTERFACE_DEFINED__ */


#ifndef ___DChoiceGreekSymbolEvents_DISPINTERFACE_DEFINED__
#define ___DChoiceGreekSymbolEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DChoiceGreekSymbolEvents */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DChoiceGreekSymbolEvents,0x5AEFB763,0x3801,0x4D40,0xB2,0x60,0x41,0xC0,0x02,0x23,0x2D,0xF6);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("5AEFB763-3801-4D40-B260-41C002232DF6")
    _DChoiceGreekSymbolEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DChoiceGreekSymbolEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DChoiceGreekSymbolEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DChoiceGreekSymbolEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DChoiceGreekSymbolEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DChoiceGreekSymbolEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DChoiceGreekSymbolEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DChoiceGreekSymbolEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DChoiceGreekSymbolEvents * This,
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
    } _DChoiceGreekSymbolEventsVtbl;

    interface _DChoiceGreekSymbolEvents
    {
        CONST_VTBL struct _DChoiceGreekSymbolEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DChoiceGreekSymbolEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DChoiceGreekSymbolEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DChoiceGreekSymbolEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DChoiceGreekSymbolEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DChoiceGreekSymbolEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DChoiceGreekSymbolEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DChoiceGreekSymbolEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DChoiceGreekSymbolEvents_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_ChoiceGreekSymbol,0x896FB8CB,0xC056,0x451F,0x9D,0xA6,0x33,0x71,0x4B,0xF9,0x2B,0x4D);

#ifdef __cplusplus

class DECLSPEC_UUID("896FB8CB-C056-451F-9DA6-33714BF92B4D")
ChoiceGreekSymbol;
#endif

#ifndef ___DUserParamsListBox_DISPINTERFACE_DEFINED__
#define ___DUserParamsListBox_DISPINTERFACE_DEFINED__

/* dispinterface _DUserParamsListBox */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DUserParamsListBox,0x1D715E46,0x1BC4,0x4DCC,0xB7,0x30,0x5D,0xD4,0xFA,0x89,0xA5,0xDC);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("1D715E46-1BC4-4DCC-B730-5DD4FA89A5DC")
    _DUserParamsListBox : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DUserParamsListBoxVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DUserParamsListBox * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DUserParamsListBox * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DUserParamsListBox * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DUserParamsListBox * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DUserParamsListBox * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DUserParamsListBox * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DUserParamsListBox * This,
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
    } _DUserParamsListBoxVtbl;

    interface _DUserParamsListBox
    {
        CONST_VTBL struct _DUserParamsListBoxVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DUserParamsListBox_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DUserParamsListBox_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DUserParamsListBox_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DUserParamsListBox_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DUserParamsListBox_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DUserParamsListBox_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DUserParamsListBox_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DUserParamsListBox_DISPINTERFACE_DEFINED__ */


#ifndef ___DUserParamsListBoxEvents_DISPINTERFACE_DEFINED__
#define ___DUserParamsListBoxEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DUserParamsListBoxEvents */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DUserParamsListBoxEvents,0xFD25AC5A,0xAF2F,0x43A0,0xA5,0x23,0x5F,0x03,0xCD,0x68,0x4F,0x6B);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("FD25AC5A-AF2F-43A0-A523-5F03CD684F6B")
    _DUserParamsListBoxEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DUserParamsListBoxEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DUserParamsListBoxEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DUserParamsListBoxEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DUserParamsListBoxEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DUserParamsListBoxEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DUserParamsListBoxEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DUserParamsListBoxEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DUserParamsListBoxEvents * This,
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
    } _DUserParamsListBoxEventsVtbl;

    interface _DUserParamsListBoxEvents
    {
        CONST_VTBL struct _DUserParamsListBoxEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DUserParamsListBoxEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DUserParamsListBoxEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DUserParamsListBoxEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DUserParamsListBoxEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DUserParamsListBoxEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DUserParamsListBoxEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DUserParamsListBoxEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DUserParamsListBoxEvents_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_UserParamsListBox,0xC58B9605,0x705F,0x411C,0x81,0x8D,0xF4,0x1E,0xF1,0xA9,0xE0,0xA0);

#ifdef __cplusplus

class DECLSPEC_UUID("C58B9605-705F-411C-818D-F41EF1A9E0A0")
UserParamsListBox;
#endif

#ifndef ___DUserParamsComboBox_DISPINTERFACE_DEFINED__
#define ___DUserParamsComboBox_DISPINTERFACE_DEFINED__

/* dispinterface _DUserParamsComboBox */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DUserParamsComboBox,0xF587A927,0x16A6,0x4405,0xB9,0xEC,0xFE,0xCD,0x95,0x15,0xD2,0x7C);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("F587A927-16A6-4405-B9EC-FECD9515D27C")
    _DUserParamsComboBox : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DUserParamsComboBoxVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DUserParamsComboBox * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DUserParamsComboBox * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DUserParamsComboBox * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DUserParamsComboBox * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DUserParamsComboBox * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DUserParamsComboBox * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DUserParamsComboBox * This,
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
    } _DUserParamsComboBoxVtbl;

    interface _DUserParamsComboBox
    {
        CONST_VTBL struct _DUserParamsComboBoxVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DUserParamsComboBox_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DUserParamsComboBox_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DUserParamsComboBox_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DUserParamsComboBox_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DUserParamsComboBox_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DUserParamsComboBox_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DUserParamsComboBox_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DUserParamsComboBox_DISPINTERFACE_DEFINED__ */


#ifndef ___DUserParamsComboBoxEvents_DISPINTERFACE_DEFINED__
#define ___DUserParamsComboBoxEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DUserParamsComboBoxEvents */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DUserParamsComboBoxEvents,0x51863815,0x27E0,0x40CA,0xA6,0x7A,0xE6,0x3A,0xA9,0xCB,0x2F,0x97);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("51863815-27E0-40CA-A67A-E63AA9CB2F97")
    _DUserParamsComboBoxEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DUserParamsComboBoxEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DUserParamsComboBoxEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DUserParamsComboBoxEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DUserParamsComboBoxEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DUserParamsComboBoxEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DUserParamsComboBoxEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DUserParamsComboBoxEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DUserParamsComboBoxEvents * This,
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
    } _DUserParamsComboBoxEventsVtbl;

    interface _DUserParamsComboBoxEvents
    {
        CONST_VTBL struct _DUserParamsComboBoxEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DUserParamsComboBoxEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DUserParamsComboBoxEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DUserParamsComboBoxEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DUserParamsComboBoxEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DUserParamsComboBoxEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DUserParamsComboBoxEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DUserParamsComboBoxEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DUserParamsComboBoxEvents_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_UserParamsComboBox,0x7FCC3AF7,0x8988,0x4D01,0xAA,0x89,0xD0,0x99,0xB4,0x4A,0x08,0x76);

#ifdef __cplusplus

class DECLSPEC_UUID("7FCC3AF7-8988-4D01-AA89-D099B44A0876")
UserParamsComboBox;
#endif
#endif /* __MeasCtrlsLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


