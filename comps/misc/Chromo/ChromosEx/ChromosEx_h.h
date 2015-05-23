

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Mon Apr 06 20:14:14 2015
 */
/* Compiler settings for ChromosEx.odl:
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


#ifndef __ChromosEx_h_h__
#define __ChromosEx_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IChromosGallery_FWD_DEFINED__
#define __IChromosGallery_FWD_DEFINED__
typedef interface IChromosGallery IChromosGallery;

#endif 	/* __IChromosGallery_FWD_DEFINED__ */


#ifndef __ChromosGallery_FWD_DEFINED__
#define __ChromosGallery_FWD_DEFINED__

#ifdef __cplusplus
typedef class ChromosGallery ChromosGallery;
#else
typedef struct ChromosGallery ChromosGallery;
#endif /* __cplusplus */

#endif 	/* __ChromosGallery_FWD_DEFINED__ */


#ifndef __ICarioObj_FWD_DEFINED__
#define __ICarioObj_FWD_DEFINED__
typedef interface ICarioObj ICarioObj;

#endif 	/* __ICarioObj_FWD_DEFINED__ */


#ifndef __CarioObj_FWD_DEFINED__
#define __CarioObj_FWD_DEFINED__

#ifdef __cplusplus
typedef class CarioObj CarioObj;
#else
typedef struct CarioObj CarioObj;
#endif /* __cplusplus */

#endif 	/* __CarioObj_FWD_DEFINED__ */


#ifndef __ICarioAnalizeView_FWD_DEFINED__
#define __ICarioAnalizeView_FWD_DEFINED__
typedef interface ICarioAnalizeView ICarioAnalizeView;

#endif 	/* __ICarioAnalizeView_FWD_DEFINED__ */


#ifndef __CarioAnalizeView_FWD_DEFINED__
#define __CarioAnalizeView_FWD_DEFINED__

#ifdef __cplusplus
typedef class CarioAnalizeView CarioAnalizeView;
#else
typedef struct CarioAnalizeView CarioAnalizeView;
#endif /* __cplusplus */

#endif 	/* __CarioAnalizeView_FWD_DEFINED__ */


#ifndef __ICarioIdioEditView_FWD_DEFINED__
#define __ICarioIdioEditView_FWD_DEFINED__
typedef interface ICarioIdioEditView ICarioIdioEditView;

#endif 	/* __ICarioIdioEditView_FWD_DEFINED__ */


#ifndef __CarioIdioEditView_FWD_DEFINED__
#define __CarioIdioEditView_FWD_DEFINED__

#ifdef __cplusplus
typedef class CarioIdioEditView CarioIdioEditView;
#else
typedef struct CarioIdioEditView CarioIdioEditView;
#endif /* __cplusplus */

#endif 	/* __CarioIdioEditView_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __ChromosEx_LIBRARY_DEFINED__
#define __ChromosEx_LIBRARY_DEFINED__

/* library ChromosEx */
/* [version][uuid] */ 


DEFINE_GUID(LIBID_ChromosEx,0x3B969DD0,0xD454,0x4F4F,0xB2,0x2E,0x90,0x61,0x78,0x36,0x35,0x27);

#ifndef __IChromosGallery_DISPINTERFACE_DEFINED__
#define __IChromosGallery_DISPINTERFACE_DEFINED__

/* dispinterface IChromosGallery */
/* [uuid] */ 


DEFINE_GUID(DIID_IChromosGallery,0x65272489,0x9943,0x4DE6,0x83,0xBE,0x04,0xB4,0x73,0x2B,0xC8,0x7D);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("65272489-9943-4DE6-83BE-04B4732BC87D")
    IChromosGallery : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IChromosGalleryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IChromosGallery * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IChromosGallery * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IChromosGallery * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IChromosGallery * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IChromosGallery * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IChromosGallery * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IChromosGallery * This,
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
    } IChromosGalleryVtbl;

    interface IChromosGallery
    {
        CONST_VTBL struct IChromosGalleryVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IChromosGallery_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IChromosGallery_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IChromosGallery_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IChromosGallery_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IChromosGallery_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IChromosGallery_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IChromosGallery_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IChromosGallery_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_ChromosGallery,0xAE25F9DE,0x076C,0x4779,0x95,0x4E,0xDE,0x35,0x27,0xD3,0x31,0x49);

#ifdef __cplusplus

class DECLSPEC_UUID("AE25F9DE-076C-4779-954E-DE3527D33149")
ChromosGallery;
#endif

#ifndef __ICarioObj_DISPINTERFACE_DEFINED__
#define __ICarioObj_DISPINTERFACE_DEFINED__

/* dispinterface ICarioObj */
/* [uuid] */ 


DEFINE_GUID(DIID_ICarioObj,0x5761789C,0xD91B,0x4EDF,0x87,0x42,0x40,0xAB,0x3E,0xDE,0xBD,0x3E);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("5761789C-D91B-4EDF-8742-40AB3EDEBD3E")
    ICarioObj : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct ICarioObjVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICarioObj * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICarioObj * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICarioObj * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ICarioObj * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ICarioObj * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ICarioObj * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ICarioObj * This,
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
    } ICarioObjVtbl;

    interface ICarioObj
    {
        CONST_VTBL struct ICarioObjVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICarioObj_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICarioObj_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICarioObj_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICarioObj_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ICarioObj_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ICarioObj_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ICarioObj_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __ICarioObj_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_CarioObj,0x108E66F4,0x697C,0x409C,0x88,0x62,0x20,0x69,0x9E,0x8E,0x2A,0x63);

#ifdef __cplusplus

class DECLSPEC_UUID("108E66F4-697C-409C-8862-20699E8E2A63")
CarioObj;
#endif

#ifndef __ICarioAnalizeView_DISPINTERFACE_DEFINED__
#define __ICarioAnalizeView_DISPINTERFACE_DEFINED__

/* dispinterface ICarioAnalizeView */
/* [uuid] */ 


DEFINE_GUID(DIID_ICarioAnalizeView,0xFEE7B82E,0x2704,0x46C8,0x83,0xC2,0xAD,0xAE,0x33,0xB3,0xFD,0x30);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("FEE7B82E-2704-46C8-83C2-ADAE33B3FD30")
    ICarioAnalizeView : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct ICarioAnalizeViewVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICarioAnalizeView * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICarioAnalizeView * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICarioAnalizeView * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ICarioAnalizeView * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ICarioAnalizeView * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ICarioAnalizeView * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ICarioAnalizeView * This,
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
    } ICarioAnalizeViewVtbl;

    interface ICarioAnalizeView
    {
        CONST_VTBL struct ICarioAnalizeViewVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICarioAnalizeView_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICarioAnalizeView_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICarioAnalizeView_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICarioAnalizeView_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ICarioAnalizeView_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ICarioAnalizeView_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ICarioAnalizeView_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __ICarioAnalizeView_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_CarioAnalizeView,0xD17CF4FD,0xF5EF,0x4788,0x84,0x0C,0x56,0xDF,0x29,0x02,0x2D,0x11);

#ifdef __cplusplus

class DECLSPEC_UUID("D17CF4FD-F5EF-4788-840C-56DF29022D11")
CarioAnalizeView;
#endif

#ifndef __ICarioIdioEditView_DISPINTERFACE_DEFINED__
#define __ICarioIdioEditView_DISPINTERFACE_DEFINED__

/* dispinterface ICarioIdioEditView */
/* [uuid] */ 


DEFINE_GUID(DIID_ICarioIdioEditView,0xE4695896,0xA2D1,0x44ad,0x93,0x96,0x68,0xAF,0x07,0x9C,0x10,0xB3);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("E4695896-A2D1-44ad-9396-68AF079C10B3")
    ICarioIdioEditView : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct ICarioIdioEditViewVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICarioIdioEditView * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICarioIdioEditView * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICarioIdioEditView * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ICarioIdioEditView * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ICarioIdioEditView * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ICarioIdioEditView * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ICarioIdioEditView * This,
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
    } ICarioIdioEditViewVtbl;

    interface ICarioIdioEditView
    {
        CONST_VTBL struct ICarioIdioEditViewVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICarioIdioEditView_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICarioIdioEditView_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICarioIdioEditView_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICarioIdioEditView_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ICarioIdioEditView_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ICarioIdioEditView_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ICarioIdioEditView_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __ICarioIdioEditView_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_CarioIdioEditView,0xF1EF20B8,0x89D5,0x4d53,0xBE,0x4F,0x07,0xDD,0x87,0x08,0x84,0x3E);

#ifdef __cplusplus

class DECLSPEC_UUID("F1EF20B8-89D5-4d53-BE4F-07DD8708843E")
CarioIdioEditView;
#endif
#endif /* __ChromosEx_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


