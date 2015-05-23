

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Wed Oct 10 23:39:58 2007
 */
/* Compiler settings for .\DBaseDoc.odl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

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


#ifndef __DBaseDoc_h_h__
#define __DBaseDoc_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IGalleryView_FWD_DEFINED__
#define __IGalleryView_FWD_DEFINED__
typedef interface IGalleryView IGalleryView;
#endif 	/* __IGalleryView_FWD_DEFINED__ */


#ifndef __GalleryView_FWD_DEFINED__
#define __GalleryView_FWD_DEFINED__

#ifdef __cplusplus
typedef class GalleryView GalleryView;
#else
typedef struct GalleryView GalleryView;
#endif /* __cplusplus */

#endif 	/* __GalleryView_FWD_DEFINED__ */


#ifndef __IBlankView_FWD_DEFINED__
#define __IBlankView_FWD_DEFINED__
typedef interface IBlankView IBlankView;
#endif 	/* __IBlankView_FWD_DEFINED__ */


#ifndef __BlankView_FWD_DEFINED__
#define __BlankView_FWD_DEFINED__

#ifdef __cplusplus
typedef class BlankView BlankView;
#else
typedef struct BlankView BlankView;
#endif /* __cplusplus */

#endif 	/* __BlankView_FWD_DEFINED__ */


#ifndef __IDBaseGridView_FWD_DEFINED__
#define __IDBaseGridView_FWD_DEFINED__
typedef interface IDBaseGridView IDBaseGridView;
#endif 	/* __IDBaseGridView_FWD_DEFINED__ */


#ifndef __DBaseGridView_FWD_DEFINED__
#define __DBaseGridView_FWD_DEFINED__

#ifdef __cplusplus
typedef class DBaseGridView DBaseGridView;
#else
typedef struct DBaseGridView DBaseGridView;
#endif /* __cplusplus */

#endif 	/* __DBaseGridView_FWD_DEFINED__ */


#ifndef __IAlbomFactory_FWD_DEFINED__
#define __IAlbomFactory_FWD_DEFINED__
typedef interface IAlbomFactory IAlbomFactory;
#endif 	/* __IAlbomFactory_FWD_DEFINED__ */


#ifndef __AlbomFactory_FWD_DEFINED__
#define __AlbomFactory_FWD_DEFINED__

#ifdef __cplusplus
typedef class AlbomFactory AlbomFactory;
#else
typedef struct AlbomFactory AlbomFactory;
#endif /* __cplusplus */

#endif 	/* __AlbomFactory_FWD_DEFINED__ */


#ifndef __IAlbomDB_FWD_DEFINED__
#define __IAlbomDB_FWD_DEFINED__
typedef interface IAlbomDB IAlbomDB;
#endif 	/* __IAlbomDB_FWD_DEFINED__ */


#ifndef __AlbomDB_FWD_DEFINED__
#define __AlbomDB_FWD_DEFINED__

#ifdef __cplusplus
typedef class AlbomDB AlbomDB;
#else
typedef struct AlbomDB AlbomDB;
#endif /* __cplusplus */

#endif 	/* __AlbomDB_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 


#ifndef __DBaseDoc_LIBRARY_DEFINED__
#define __DBaseDoc_LIBRARY_DEFINED__

/* library DBaseDoc */
/* [helpstring][version][uuid] */ 


DEFINE_GUID(LIBID_DBaseDoc,0x5DF7A4E0,0x7BCB,0x4c10,0xB8,0x99,0x31,0x97,0x81,0x65,0x48,0x2C);

#ifndef __IGalleryView_DISPINTERFACE_DEFINED__
#define __IGalleryView_DISPINTERFACE_DEFINED__

/* dispinterface IGalleryView */
/* [uuid] */ 


DEFINE_GUID(DIID_IGalleryView,0xF97A3846,0xBF78,0x40FF,0x9E,0x4B,0xFF,0xDE,0xC7,0xE6,0xA0,0x33);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("F97A3846-BF78-40FF-9E4B-FFDEC7E6A033")
    IGalleryView : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IGalleryViewVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IGalleryView * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IGalleryView * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IGalleryView * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IGalleryView * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IGalleryView * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IGalleryView * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IGalleryView * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IGalleryViewVtbl;

    interface IGalleryView
    {
        CONST_VTBL struct IGalleryViewVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGalleryView_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IGalleryView_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IGalleryView_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IGalleryView_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IGalleryView_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IGalleryView_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IGalleryView_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IGalleryView_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_GalleryView,0x72F13015,0x810C,0x4689,0x94,0x99,0x41,0x7E,0xA0,0x0F,0x37,0x51);

#ifdef __cplusplus

class DECLSPEC_UUID("72F13015-810C-4689-9499-417EA00F3751")
GalleryView;
#endif

#ifndef __IBlankView_DISPINTERFACE_DEFINED__
#define __IBlankView_DISPINTERFACE_DEFINED__

/* dispinterface IBlankView */
/* [uuid] */ 


DEFINE_GUID(DIID_IBlankView,0x0CC9CDE3,0x3819,0x4848,0xB7,0x72,0xC0,0xD8,0xC8,0x8F,0x78,0xEC);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("0CC9CDE3-3819-4848-B772-C0D8C88F78EC")
    IBlankView : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IBlankViewVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBlankView * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBlankView * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBlankView * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IBlankView * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IBlankView * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IBlankView * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IBlankView * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IBlankViewVtbl;

    interface IBlankView
    {
        CONST_VTBL struct IBlankViewVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBlankView_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBlankView_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBlankView_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBlankView_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IBlankView_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IBlankView_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IBlankView_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IBlankView_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_BlankView,0x92A26F3B,0x69AC,0x44F2,0xB1,0xAB,0x8D,0xCC,0xF8,0x7D,0xB4,0xA3);

#ifdef __cplusplus

class DECLSPEC_UUID("92A26F3B-69AC-44F2-B1AB-8DCCF87DB4A3")
BlankView;
#endif

#ifndef __IDBaseGridView_DISPINTERFACE_DEFINED__
#define __IDBaseGridView_DISPINTERFACE_DEFINED__

/* dispinterface IDBaseGridView */
/* [uuid] */ 


DEFINE_GUID(DIID_IDBaseGridView,0x42CA470A,0xFDBF,0x47e8,0xA0,0xB4,0xE3,0xCD,0xE7,0x82,0x62,0x3C);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("42CA470A-FDBF-47e8-A0B4-E3CDE782623C")
    IDBaseGridView : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IDBaseGridViewVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDBaseGridView * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDBaseGridView * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDBaseGridView * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IDBaseGridView * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IDBaseGridView * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IDBaseGridView * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IDBaseGridView * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IDBaseGridViewVtbl;

    interface IDBaseGridView
    {
        CONST_VTBL struct IDBaseGridViewVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDBaseGridView_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDBaseGridView_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDBaseGridView_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDBaseGridView_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IDBaseGridView_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IDBaseGridView_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IDBaseGridView_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IDBaseGridView_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_DBaseGridView,0x5B9B0B42,0xB147,0x4b14,0x9A,0xF1,0x28,0x4F,0x61,0x40,0xB5,0xAC);

#ifdef __cplusplus

class DECLSPEC_UUID("5B9B0B42-B147-4b14-9AF1-284F6140B5AC")
DBaseGridView;
#endif

#ifndef __IAlbomFactory_DISPINTERFACE_DEFINED__
#define __IAlbomFactory_DISPINTERFACE_DEFINED__

/* dispinterface IAlbomFactory */
/* [uuid] */ 


DEFINE_GUID(DIID_IAlbomFactory,0x2838997B,0x3E4F,0x4130,0x94,0x7F,0x9B,0x0B,0xFB,0xE8,0x3C,0xA5);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("2838997B-3E4F-4130-947F-9B0BFBE83CA5")
    IAlbomFactory : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IAlbomFactoryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAlbomFactory * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAlbomFactory * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAlbomFactory * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IAlbomFactory * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IAlbomFactory * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IAlbomFactory * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IAlbomFactory * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IAlbomFactoryVtbl;

    interface IAlbomFactory
    {
        CONST_VTBL struct IAlbomFactoryVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAlbomFactory_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAlbomFactory_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAlbomFactory_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAlbomFactory_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAlbomFactory_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAlbomFactory_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAlbomFactory_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IAlbomFactory_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_AlbomFactory,0x9518D746,0xCD50,0x40C5,0x96,0x0A,0xE3,0x1A,0x27,0xD7,0x3F,0xAB);

#ifdef __cplusplus

class DECLSPEC_UUID("9518D746-CD50-40C5-960A-E31A27D73FAB")
AlbomFactory;
#endif

#ifndef __IAlbomDB_DISPINTERFACE_DEFINED__
#define __IAlbomDB_DISPINTERFACE_DEFINED__

/* dispinterface IAlbomDB */
/* [uuid] */ 


DEFINE_GUID(DIID_IAlbomDB,0xCF1154A4,0xBE94,0x4665,0x9D,0xE2,0x15,0x9E,0x00,0x0C,0xE5,0x45);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("CF1154A4-BE94-4665-9DE2-159E000CE545")
    IAlbomDB : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IAlbomDBVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAlbomDB * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAlbomDB * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAlbomDB * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IAlbomDB * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IAlbomDB * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IAlbomDB * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IAlbomDB * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IAlbomDBVtbl;

    interface IAlbomDB
    {
        CONST_VTBL struct IAlbomDBVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAlbomDB_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAlbomDB_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAlbomDB_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAlbomDB_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAlbomDB_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAlbomDB_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAlbomDB_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IAlbomDB_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_AlbomDB,0x08A33477,0xB75E,0x4148,0x81,0x36,0x01,0x29,0x76,0x54,0xA1,0x50);

#ifdef __cplusplus

class DECLSPEC_UUID("08A33477-B75E-4148-8136-01297654A150")
AlbomDB;
#endif
#endif /* __DBaseDoc_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


