

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Apr 04 21:03:56 2015
 */
/* Compiler settings for Data.odl:
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


#ifndef __Data_h_h__
#define __Data_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __INamedData_FWD_DEFINED__
#define __INamedData_FWD_DEFINED__
typedef interface INamedData INamedData;

#endif 	/* __INamedData_FWD_DEFINED__ */


#ifndef __NamedData_FWD_DEFINED__
#define __NamedData_FWD_DEFINED__

#ifdef __cplusplus
typedef class NamedData NamedData;
#else
typedef struct NamedData NamedData;
#endif /* __cplusplus */

#endif 	/* __NamedData_FWD_DEFINED__ */


#ifndef __IDataContext_FWD_DEFINED__
#define __IDataContext_FWD_DEFINED__
typedef interface IDataContext IDataContext;

#endif 	/* __IDataContext_FWD_DEFINED__ */


#ifndef __DataContext_FWD_DEFINED__
#define __DataContext_FWD_DEFINED__

#ifdef __cplusplus
typedef class DataContext DataContext;
#else
typedef struct DataContext DataContext;
#endif /* __cplusplus */

#endif 	/* __DataContext_FWD_DEFINED__ */


#ifndef __ITypeInfoManager_FWD_DEFINED__
#define __ITypeInfoManager_FWD_DEFINED__
typedef interface ITypeInfoManager ITypeInfoManager;

#endif 	/* __ITypeInfoManager_FWD_DEFINED__ */


#ifndef __TypeInfoManager_FWD_DEFINED__
#define __TypeInfoManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class TypeInfoManager TypeInfoManager;
#else
typedef struct TypeInfoManager TypeInfoManager;
#endif /* __cplusplus */

#endif 	/* __TypeInfoManager_FWD_DEFINED__ */


#ifndef __IObjectsDisp_FWD_DEFINED__
#define __IObjectsDisp_FWD_DEFINED__
typedef interface IObjectsDisp IObjectsDisp;

#endif 	/* __IObjectsDisp_FWD_DEFINED__ */


#ifndef __Objects_FWD_DEFINED__
#define __Objects_FWD_DEFINED__

#ifdef __cplusplus
typedef class Objects Objects;
#else
typedef struct Objects Objects;
#endif /* __cplusplus */

#endif 	/* __Objects_FWD_DEFINED__ */


#ifndef __Iobject_manager_FWD_DEFINED__
#define __Iobject_manager_FWD_DEFINED__
typedef interface Iobject_manager Iobject_manager;

#endif 	/* __Iobject_manager_FWD_DEFINED__ */


#ifndef __objectmanager_FWD_DEFINED__
#define __objectmanager_FWD_DEFINED__

#ifdef __cplusplus
typedef class objectmanager objectmanager;
#else
typedef struct objectmanager objectmanager;
#endif /* __cplusplus */

#endif 	/* __objectmanager_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __Data_LIBRARY_DEFINED__
#define __Data_LIBRARY_DEFINED__

/* library Data */
/* [version][uuid] */ 


DEFINE_GUID(LIBID_Data,0x526B6D32,0x1CAD,0x11D3,0xA5,0xCF,0x00,0x00,0xB4,0x93,0xFF,0x1B);

#ifndef __INamedData_DISPINTERFACE_DEFINED__
#define __INamedData_DISPINTERFACE_DEFINED__

/* dispinterface INamedData */
/* [uuid] */ 


DEFINE_GUID(DIID_INamedData,0xADBA6C03,0x1D68,0x11D3,0xA5,0xD0,0x00,0x00,0xB4,0x93,0xFF,0x1B);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("ADBA6C03-1D68-11D3-A5D0-0000B493FF1B")
    INamedData : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct INamedDataVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INamedData * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INamedData * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INamedData * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INamedData * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INamedData * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INamedData * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INamedData * This,
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
    } INamedDataVtbl;

    interface INamedData
    {
        CONST_VTBL struct INamedDataVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INamedData_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INamedData_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INamedData_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INamedData_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INamedData_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INamedData_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INamedData_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __INamedData_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_NamedData,0xADBA6C05,0x1D68,0x11D3,0xA5,0xD0,0x00,0x00,0xB4,0x93,0xFF,0x1B);

#ifdef __cplusplus

class DECLSPEC_UUID("ADBA6C05-1D68-11D3-A5D0-0000B493FF1B")
NamedData;
#endif

#ifndef __IDataContext_DISPINTERFACE_DEFINED__
#define __IDataContext_DISPINTERFACE_DEFINED__

/* dispinterface IDataContext */
/* [uuid] */ 


DEFINE_GUID(DIID_IDataContext,0xDCEE4109,0x9579,0x46d3,0x9E,0x57,0x57,0x6C,0xA1,0x6D,0x60,0xB4);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("DCEE4109-9579-46d3-9E57-576CA16D60B4")
    IDataContext : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IDataContextVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDataContext * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDataContext * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDataContext * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IDataContext * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IDataContext * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IDataContext * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IDataContext * This,
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
    } IDataContextVtbl;

    interface IDataContext
    {
        CONST_VTBL struct IDataContextVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDataContext_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IDataContext_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IDataContext_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IDataContext_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IDataContext_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IDataContext_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IDataContext_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IDataContext_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_DataContext,0xB438FADB,0x38E7,0x11D3,0x87,0xC5,0x00,0x00,0xB4,0x93,0xFF,0x1B);

#ifdef __cplusplus

class DECLSPEC_UUID("B438FADB-38E7-11D3-87C5-0000B493FF1B")
DataContext;
#endif

#ifndef __ITypeInfoManager_DISPINTERFACE_DEFINED__
#define __ITypeInfoManager_DISPINTERFACE_DEFINED__

/* dispinterface ITypeInfoManager */
/* [uuid] */ 


DEFINE_GUID(DIID_ITypeInfoManager,0xC49F7850,0x39FA,0x11D3,0x87,0xC6,0x00,0x00,0xB4,0x93,0xFF,0x1B);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("C49F7850-39FA-11D3-87C6-0000B493FF1B")
    ITypeInfoManager : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct ITypeInfoManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITypeInfoManager * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITypeInfoManager * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITypeInfoManager * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ITypeInfoManager * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ITypeInfoManager * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ITypeInfoManager * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ITypeInfoManager * This,
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
    } ITypeInfoManagerVtbl;

    interface ITypeInfoManager
    {
        CONST_VTBL struct ITypeInfoManagerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITypeInfoManager_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ITypeInfoManager_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ITypeInfoManager_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ITypeInfoManager_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ITypeInfoManager_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ITypeInfoManager_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ITypeInfoManager_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __ITypeInfoManager_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_TypeInfoManager,0xC49F7851,0x39FA,0x11D3,0x87,0xC6,0x00,0x00,0xB4,0x93,0xFF,0x1B);

#ifdef __cplusplus

class DECLSPEC_UUID("C49F7851-39FA-11D3-87C6-0000B493FF1B")
TypeInfoManager;
#endif

#ifndef __IObjectsDisp_DISPINTERFACE_DEFINED__
#define __IObjectsDisp_DISPINTERFACE_DEFINED__

/* dispinterface IObjectsDisp */
/* [uuid] */ 


DEFINE_GUID(DIID_IObjectsDisp,0x0185A230,0x5FF4,0x4f68,0xB9,0x6C,0xA4,0x4D,0x4C,0xEE,0x89,0x06);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("0185A230-5FF4-4f68-B96C-A44D4CEE8906")
    IObjectsDisp : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IObjectsDispVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IObjectsDisp * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IObjectsDisp * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IObjectsDisp * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IObjectsDisp * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IObjectsDisp * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IObjectsDisp * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IObjectsDisp * This,
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
    } IObjectsDispVtbl;

    interface IObjectsDisp
    {
        CONST_VTBL struct IObjectsDispVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IObjectsDisp_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IObjectsDisp_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IObjectsDisp_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IObjectsDisp_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IObjectsDisp_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IObjectsDisp_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IObjectsDisp_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IObjectsDisp_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_Objects,0x27DDB9F9,0x6F44,0x424b,0xAE,0x43,0x1D,0x65,0xD1,0xF7,0x90,0x1B);

#ifdef __cplusplus

class DECLSPEC_UUID("27DDB9F9-6F44-424b-AE43-1D65D1F7901B")
Objects;
#endif

#ifndef __Iobject_manager_DISPINTERFACE_DEFINED__
#define __Iobject_manager_DISPINTERFACE_DEFINED__

/* dispinterface Iobject_manager */
/* [uuid] */ 


DEFINE_GUID(DIID_Iobject_manager,0x46A13E4D,0x6738,0x4FB1,0x8B,0xC3,0xBA,0xEE,0xB5,0xB5,0xD3,0xAE);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("46A13E4D-6738-4FB1-8BC3-BAEEB5B5D3AE")
    Iobject_manager : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct Iobject_managerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            Iobject_manager * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            Iobject_manager * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            Iobject_manager * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            Iobject_manager * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            Iobject_manager * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            Iobject_manager * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            Iobject_manager * This,
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
    } Iobject_managerVtbl;

    interface Iobject_manager
    {
        CONST_VTBL struct Iobject_managerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define Iobject_manager_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define Iobject_manager_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define Iobject_manager_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define Iobject_manager_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define Iobject_manager_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define Iobject_manager_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define Iobject_manager_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __Iobject_manager_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_objectmanager,0xB0097FB7,0xFBA0,0x46A8,0x8F,0xA6,0xC4,0x86,0x5E,0x91,0x36,0xF9);

#ifdef __cplusplus

class DECLSPEC_UUID("B0097FB7-FBA0-46A8-8FA6-C4865E9136F9")
objectmanager;
#endif
#endif /* __Data_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


