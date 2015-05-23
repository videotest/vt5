

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Apr 04 21:04:52 2015
 */
/* Compiler settings for axform.odl:
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


#ifndef __axform_h_h__
#define __axform_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IFormManager_FWD_DEFINED__
#define __IFormManager_FWD_DEFINED__
typedef interface IFormManager IFormManager;

#endif 	/* __IFormManager_FWD_DEFINED__ */


#ifndef __FormManager_FWD_DEFINED__
#define __FormManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class FormManager FormManager;
#else
typedef struct FormManager FormManager;
#endif /* __cplusplus */

#endif 	/* __FormManager_FWD_DEFINED__ */


#ifndef __IActiveXSite_FWD_DEFINED__
#define __IActiveXSite_FWD_DEFINED__
typedef interface IActiveXSite IActiveXSite;

#endif 	/* __IActiveXSite_FWD_DEFINED__ */


#ifndef __IActiveXSiteEvents_FWD_DEFINED__
#define __IActiveXSiteEvents_FWD_DEFINED__
typedef interface IActiveXSiteEvents IActiveXSiteEvents;

#endif 	/* __IActiveXSiteEvents_FWD_DEFINED__ */


#ifndef __ActiveXSite_FWD_DEFINED__
#define __ActiveXSite_FWD_DEFINED__

#ifdef __cplusplus
typedef class ActiveXSite ActiveXSite;
#else
typedef struct ActiveXSite ActiveXSite;
#endif /* __cplusplus */

#endif 	/* __ActiveXSite_FWD_DEFINED__ */


#ifndef __IAXFileFilter_FWD_DEFINED__
#define __IAXFileFilter_FWD_DEFINED__
typedef interface IAXFileFilter IAXFileFilter;

#endif 	/* __IAXFileFilter_FWD_DEFINED__ */


#ifndef __AXFileFilter_FWD_DEFINED__
#define __AXFileFilter_FWD_DEFINED__

#ifdef __cplusplus
typedef class AXFileFilter AXFileFilter;
#else
typedef struct AXFileFilter AXFileFilter;
#endif /* __cplusplus */

#endif 	/* __AXFileFilter_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __Axform_LIBRARY_DEFINED__
#define __Axform_LIBRARY_DEFINED__

/* library Axform */
/* [version][uuid] */ 


DEFINE_GUID(LIBID_Axform,0xBBC08690,0xFE5A,0x46c4,0xB8,0x2D,0xE4,0xEC,0xDA,0x7B,0xD5,0x7F);

#ifndef __IFormManager_DISPINTERFACE_DEFINED__
#define __IFormManager_DISPINTERFACE_DEFINED__

/* dispinterface IFormManager */
/* [uuid] */ 


DEFINE_GUID(DIID_IFormManager,0xC4633177,0xA0FD,0x440A,0xA2,0xDE,0x73,0xDB,0xCA,0xC1,0x88,0xA5);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("C4633177-A0FD-440A-A2DE-73DBCAC188A5")
    IFormManager : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IFormManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFormManager * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFormManager * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFormManager * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IFormManager * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IFormManager * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IFormManager * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IFormManager * This,
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
    } IFormManagerVtbl;

    interface IFormManager
    {
        CONST_VTBL struct IFormManagerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFormManager_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFormManager_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFormManager_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IFormManager_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IFormManager_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IFormManager_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IFormManager_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IFormManager_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_FormManager,0x57F4F94D,0x7AD5,0x4821,0xAC,0x4B,0x79,0x39,0xA6,0x29,0x2D,0x76);

#ifdef __cplusplus

class DECLSPEC_UUID("57F4F94D-7AD5-4821-AC4B-7939A6292D76")
FormManager;
#endif

#ifndef __IActiveXSite_DISPINTERFACE_DEFINED__
#define __IActiveXSite_DISPINTERFACE_DEFINED__

/* dispinterface IActiveXSite */
/* [uuid] */ 


DEFINE_GUID(DIID_IActiveXSite,0xC7D2F99D,0xE12E,0x4edf,0x8F,0xB4,0x0A,0x77,0x82,0x6E,0xB8,0x65);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("C7D2F99D-E12E-4edf-8FB4-0A77826EB865")
    IActiveXSite : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IActiveXSiteVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IActiveXSite * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IActiveXSite * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IActiveXSite * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IActiveXSite * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IActiveXSite * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IActiveXSite * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IActiveXSite * This,
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
    } IActiveXSiteVtbl;

    interface IActiveXSite
    {
        CONST_VTBL struct IActiveXSiteVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IActiveXSite_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IActiveXSite_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IActiveXSite_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IActiveXSite_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IActiveXSite_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IActiveXSite_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IActiveXSite_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IActiveXSite_DISPINTERFACE_DEFINED__ */


#ifndef __IActiveXSiteEvents_DISPINTERFACE_DEFINED__
#define __IActiveXSiteEvents_DISPINTERFACE_DEFINED__

/* dispinterface IActiveXSiteEvents */
/* [uuid] */ 


DEFINE_GUID(DIID_IActiveXSiteEvents,0x9B6C9468,0xF9C4,0x460f,0xBD,0x44,0x5D,0x41,0xBA,0xF3,0x20,0x5D);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("9B6C9468-F9C4-460f-BD44-5D41BAF3205D")
    IActiveXSiteEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IActiveXSiteEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IActiveXSiteEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IActiveXSiteEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IActiveXSiteEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IActiveXSiteEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IActiveXSiteEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IActiveXSiteEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IActiveXSiteEvents * This,
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
    } IActiveXSiteEventsVtbl;

    interface IActiveXSiteEvents
    {
        CONST_VTBL struct IActiveXSiteEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IActiveXSiteEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IActiveXSiteEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IActiveXSiteEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IActiveXSiteEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IActiveXSiteEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IActiveXSiteEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IActiveXSiteEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IActiveXSiteEvents_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_ActiveXSite,0x4666C0CC,0xFD8F,0x48ef,0x9F,0x7B,0x2D,0x9D,0x5F,0x3E,0x9C,0x99);

#ifdef __cplusplus

class DECLSPEC_UUID("4666C0CC-FD8F-48ef-9F7B-2D9D5F3E9C99")
ActiveXSite;
#endif

#ifndef __IAXFileFilter_DISPINTERFACE_DEFINED__
#define __IAXFileFilter_DISPINTERFACE_DEFINED__

/* dispinterface IAXFileFilter */
/* [uuid] */ 


DEFINE_GUID(DIID_IAXFileFilter,0x4E093842,0xD7EC,0x11d3,0xA0,0x94,0x00,0x00,0xB4,0x93,0xA1,0x87);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("4E093842-D7EC-11d3-A094-0000B493A187")
    IAXFileFilter : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IAXFileFilterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAXFileFilter * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAXFileFilter * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAXFileFilter * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IAXFileFilter * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IAXFileFilter * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IAXFileFilter * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IAXFileFilter * This,
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
    } IAXFileFilterVtbl;

    interface IAXFileFilter
    {
        CONST_VTBL struct IAXFileFilterVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAXFileFilter_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAXFileFilter_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAXFileFilter_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAXFileFilter_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IAXFileFilter_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IAXFileFilter_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IAXFileFilter_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IAXFileFilter_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_AXFileFilter,0x4E093841,0xD7EC,0x11d3,0xA0,0x94,0x00,0x00,0xB4,0x93,0xA1,0x87);

#ifdef __cplusplus

class DECLSPEC_UUID("4E093841-D7EC-11d3-A094-0000B493A187")
AXFileFilter;
#endif
#endif /* __Axform_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


