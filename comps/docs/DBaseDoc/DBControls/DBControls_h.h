

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Apr 04 21:03:51 2015
 */
/* Compiler settings for DBControls.odl:
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


#ifndef __DBControls_h_h__
#define __DBControls_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef ___DDBText_FWD_DEFINED__
#define ___DDBText_FWD_DEFINED__
typedef interface _DDBText _DDBText;

#endif 	/* ___DDBText_FWD_DEFINED__ */


#ifndef ___DDBTextEvents_FWD_DEFINED__
#define ___DDBTextEvents_FWD_DEFINED__
typedef interface _DDBTextEvents _DDBTextEvents;

#endif 	/* ___DDBTextEvents_FWD_DEFINED__ */


#ifndef __DBText_FWD_DEFINED__
#define __DBText_FWD_DEFINED__

#ifdef __cplusplus
typedef class DBText DBText;
#else
typedef struct DBText DBText;
#endif /* __cplusplus */

#endif 	/* __DBText_FWD_DEFINED__ */


#ifndef ___DDBNumber_FWD_DEFINED__
#define ___DDBNumber_FWD_DEFINED__
typedef interface _DDBNumber _DDBNumber;

#endif 	/* ___DDBNumber_FWD_DEFINED__ */


#ifndef ___DDBNumberEvents_FWD_DEFINED__
#define ___DDBNumberEvents_FWD_DEFINED__
typedef interface _DDBNumberEvents _DDBNumberEvents;

#endif 	/* ___DDBNumberEvents_FWD_DEFINED__ */


#ifndef __DBNumber_FWD_DEFINED__
#define __DBNumber_FWD_DEFINED__

#ifdef __cplusplus
typedef class DBNumber DBNumber;
#else
typedef struct DBNumber DBNumber;
#endif /* __cplusplus */

#endif 	/* __DBNumber_FWD_DEFINED__ */


#ifndef ___DDBDateTime_FWD_DEFINED__
#define ___DDBDateTime_FWD_DEFINED__
typedef interface _DDBDateTime _DDBDateTime;

#endif 	/* ___DDBDateTime_FWD_DEFINED__ */


#ifndef ___DDBDateTimeEvents_FWD_DEFINED__
#define ___DDBDateTimeEvents_FWD_DEFINED__
typedef interface _DDBDateTimeEvents _DDBDateTimeEvents;

#endif 	/* ___DDBDateTimeEvents_FWD_DEFINED__ */


#ifndef __DBDateTime_FWD_DEFINED__
#define __DBDateTime_FWD_DEFINED__

#ifdef __cplusplus
typedef class DBDateTime DBDateTime;
#else
typedef struct DBDateTime DBDateTime;
#endif /* __cplusplus */

#endif 	/* __DBDateTime_FWD_DEFINED__ */


#ifndef ___DDBObject_FWD_DEFINED__
#define ___DDBObject_FWD_DEFINED__
typedef interface _DDBObject _DDBObject;

#endif 	/* ___DDBObject_FWD_DEFINED__ */


#ifndef ___DDBObjectEvents_FWD_DEFINED__
#define ___DDBObjectEvents_FWD_DEFINED__
typedef interface _DDBObjectEvents _DDBObjectEvents;

#endif 	/* ___DDBObjectEvents_FWD_DEFINED__ */


#ifndef __DBObject_FWD_DEFINED__
#define __DBObject_FWD_DEFINED__

#ifdef __cplusplus
typedef class DBObject DBObject;
#else
typedef struct DBObject DBObject;
#endif /* __cplusplus */

#endif 	/* __DBObject_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __DBCONTROLSLib_LIBRARY_DEFINED__
#define __DBCONTROLSLib_LIBRARY_DEFINED__

/* library DBCONTROLSLib */
/* [control][helpstring][helpfile][version][uuid] */ 

#pragma once
#pragma region Desktop Family
#pragma endregion

DEFINE_GUID(LIBID_DBCONTROLSLib,0x59213681,0x18D6,0x46B4,0xAF,0x62,0xE7,0x37,0xD7,0x24,0x10,0xBF);

#ifndef ___DDBText_DISPINTERFACE_DEFINED__
#define ___DDBText_DISPINTERFACE_DEFINED__

/* dispinterface _DDBText */
/* [hidden][helpstring][uuid] */ 


DEFINE_GUID(DIID__DDBText,0xEF00F5FB,0x9F2B,0x4921,0x8B,0x3C,0x67,0xA9,0x0D,0xD6,0x76,0x94);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("EF00F5FB-9F2B-4921-8B3C-67A90DD67694")
    _DDBText : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DDBTextVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DDBText * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DDBText * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DDBText * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DDBText * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DDBText * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DDBText * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DDBText * This,
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
    } _DDBTextVtbl;

    interface _DDBText
    {
        CONST_VTBL struct _DDBTextVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DDBText_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DDBText_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DDBText_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DDBText_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DDBText_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DDBText_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DDBText_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DDBText_DISPINTERFACE_DEFINED__ */


#ifndef ___DDBTextEvents_DISPINTERFACE_DEFINED__
#define ___DDBTextEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DDBTextEvents */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DDBTextEvents,0xC16587C7,0x6AA5,0x407F,0xA1,0xD4,0x29,0xE6,0x79,0x43,0xBD,0x42);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("C16587C7-6AA5-407F-A1D4-29E67943BD42")
    _DDBTextEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DDBTextEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DDBTextEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DDBTextEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DDBTextEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DDBTextEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DDBTextEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DDBTextEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DDBTextEvents * This,
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
    } _DDBTextEventsVtbl;

    interface _DDBTextEvents
    {
        CONST_VTBL struct _DDBTextEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DDBTextEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DDBTextEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DDBTextEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DDBTextEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DDBTextEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DDBTextEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DDBTextEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DDBTextEvents_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_DBText,0x4A17BF90,0xE003,0x4C5F,0xAA,0x99,0x25,0x7B,0x34,0xD8,0xDF,0x0F);

#ifdef __cplusplus

class DECLSPEC_UUID("4A17BF90-E003-4C5F-AA99-257B34D8DF0F")
DBText;
#endif

#ifndef ___DDBNumber_DISPINTERFACE_DEFINED__
#define ___DDBNumber_DISPINTERFACE_DEFINED__

/* dispinterface _DDBNumber */
/* [hidden][helpstring][uuid] */ 


DEFINE_GUID(DIID__DDBNumber,0x739709C4,0xA075,0x4520,0xB4,0x03,0xBF,0xE5,0xD8,0xF1,0x60,0xD9);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("739709C4-A075-4520-B403-BFE5D8F160D9")
    _DDBNumber : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DDBNumberVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DDBNumber * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DDBNumber * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DDBNumber * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DDBNumber * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DDBNumber * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DDBNumber * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DDBNumber * This,
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
    } _DDBNumberVtbl;

    interface _DDBNumber
    {
        CONST_VTBL struct _DDBNumberVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DDBNumber_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DDBNumber_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DDBNumber_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DDBNumber_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DDBNumber_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DDBNumber_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DDBNumber_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DDBNumber_DISPINTERFACE_DEFINED__ */


#ifndef ___DDBNumberEvents_DISPINTERFACE_DEFINED__
#define ___DDBNumberEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DDBNumberEvents */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DDBNumberEvents,0xD6DD5A3C,0x473C,0x482F,0xB1,0xE1,0x48,0x89,0x98,0xCA,0x12,0xAD);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("D6DD5A3C-473C-482F-B1E1-488998CA12AD")
    _DDBNumberEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DDBNumberEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DDBNumberEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DDBNumberEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DDBNumberEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DDBNumberEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DDBNumberEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DDBNumberEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DDBNumberEvents * This,
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
    } _DDBNumberEventsVtbl;

    interface _DDBNumberEvents
    {
        CONST_VTBL struct _DDBNumberEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DDBNumberEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DDBNumberEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DDBNumberEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DDBNumberEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DDBNumberEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DDBNumberEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DDBNumberEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DDBNumberEvents_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_DBNumber,0x2868ADBF,0x0EAE,0x4038,0xA2,0x26,0x82,0x73,0x32,0xEC,0xA3,0x26);

#ifdef __cplusplus

class DECLSPEC_UUID("2868ADBF-0EAE-4038-A226-827332ECA326")
DBNumber;
#endif

#ifndef ___DDBDateTime_DISPINTERFACE_DEFINED__
#define ___DDBDateTime_DISPINTERFACE_DEFINED__

/* dispinterface _DDBDateTime */
/* [hidden][helpstring][uuid] */ 


DEFINE_GUID(DIID__DDBDateTime,0x414F5379,0xB140,0x4586,0x87,0xA5,0x98,0xD9,0xB8,0x2B,0xE4,0xCE);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("414F5379-B140-4586-87A5-98D9B82BE4CE")
    _DDBDateTime : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DDBDateTimeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DDBDateTime * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DDBDateTime * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DDBDateTime * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DDBDateTime * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DDBDateTime * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DDBDateTime * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DDBDateTime * This,
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
    } _DDBDateTimeVtbl;

    interface _DDBDateTime
    {
        CONST_VTBL struct _DDBDateTimeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DDBDateTime_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DDBDateTime_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DDBDateTime_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DDBDateTime_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DDBDateTime_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DDBDateTime_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DDBDateTime_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DDBDateTime_DISPINTERFACE_DEFINED__ */


#ifndef ___DDBDateTimeEvents_DISPINTERFACE_DEFINED__
#define ___DDBDateTimeEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DDBDateTimeEvents */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DDBDateTimeEvents,0xF8205CAF,0x6501,0x4E8F,0x9D,0x6A,0x4B,0xFA,0xEC,0xF1,0x6E,0x0B);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("F8205CAF-6501-4E8F-9D6A-4BFAECF16E0B")
    _DDBDateTimeEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DDBDateTimeEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DDBDateTimeEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DDBDateTimeEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DDBDateTimeEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DDBDateTimeEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DDBDateTimeEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DDBDateTimeEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DDBDateTimeEvents * This,
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
    } _DDBDateTimeEventsVtbl;

    interface _DDBDateTimeEvents
    {
        CONST_VTBL struct _DDBDateTimeEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DDBDateTimeEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DDBDateTimeEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DDBDateTimeEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DDBDateTimeEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DDBDateTimeEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DDBDateTimeEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DDBDateTimeEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DDBDateTimeEvents_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_DBDateTime,0x1AC3AEB0,0x92EC,0x4E43,0x89,0xD3,0x86,0x65,0xED,0x35,0x18,0x99);

#ifdef __cplusplus

class DECLSPEC_UUID("1AC3AEB0-92EC-4E43-89D3-8665ED351899")
DBDateTime;
#endif

#ifndef ___DDBObject_DISPINTERFACE_DEFINED__
#define ___DDBObject_DISPINTERFACE_DEFINED__

/* dispinterface _DDBObject */
/* [hidden][helpstring][uuid] */ 


DEFINE_GUID(DIID__DDBObject,0x6F09781B,0x09DA,0x4e83,0x90,0x7A,0xF2,0x11,0x40,0xAD,0x7F,0x7C);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("6F09781B-09DA-4e83-907A-F21140AD7F7C")
    _DDBObject : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DDBObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DDBObject * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DDBObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DDBObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DDBObject * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DDBObject * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DDBObject * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DDBObject * This,
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
    } _DDBObjectVtbl;

    interface _DDBObject
    {
        CONST_VTBL struct _DDBObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DDBObject_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DDBObject_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DDBObject_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DDBObject_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DDBObject_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DDBObject_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DDBObject_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DDBObject_DISPINTERFACE_DEFINED__ */


#ifndef ___DDBObjectEvents_DISPINTERFACE_DEFINED__
#define ___DDBObjectEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DDBObjectEvents */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DDBObjectEvents,0x0ED332C9,0xCC5F,0x4bd2,0xAE,0xC6,0xC4,0xC9,0x95,0x68,0xAE,0x1D);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("0ED332C9-CC5F-4bd2-AEC6-C4C99568AE1D")
    _DDBObjectEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DDBObjectEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DDBObjectEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DDBObjectEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DDBObjectEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DDBObjectEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DDBObjectEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DDBObjectEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DDBObjectEvents * This,
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
    } _DDBObjectEventsVtbl;

    interface _DDBObjectEvents
    {
        CONST_VTBL struct _DDBObjectEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DDBObjectEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DDBObjectEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DDBObjectEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DDBObjectEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DDBObjectEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DDBObjectEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DDBObjectEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DDBObjectEvents_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_DBObject,0x31BD1B24,0x7B8D,0x4017,0xB5,0xBF,0xEB,0xB8,0x5F,0x96,0x12,0xE3);

#ifdef __cplusplus

class DECLSPEC_UUID("31BD1B24-7B8D-4017-B5BF-EBB85F9612E3")
DBObject;
#endif
#endif /* __DBCONTROLSLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


