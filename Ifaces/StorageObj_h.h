

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Thu Feb 16 00:54:46 2017
 */
/* Compiler settings for StorageObj.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.00.0603 
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


#ifndef __StorageObj_h_h__
#define __StorageObj_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IStorageObject_FWD_DEFINED__
#define __IStorageObject_FWD_DEFINED__
typedef interface IStorageObject IStorageObject;

#endif 	/* __IStorageObject_FWD_DEFINED__ */


#ifndef __IStorageObjectMisc_FWD_DEFINED__
#define __IStorageObjectMisc_FWD_DEFINED__
typedef interface IStorageObjectMisc IStorageObjectMisc;

#endif 	/* __IStorageObjectMisc_FWD_DEFINED__ */


#ifndef __StorageObject_FWD_DEFINED__
#define __StorageObject_FWD_DEFINED__

#ifdef __cplusplus
typedef class StorageObject StorageObject;
#else
typedef struct StorageObject StorageObject;
#endif /* __cplusplus */

#endif 	/* __StorageObject_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __StorageObjectLib_LIBRARY_DEFINED__
#define __StorageObjectLib_LIBRARY_DEFINED__

/* library StorageObjectLib */
/* [helpstring][version][uuid] */ 


enum AdditionalSettings
    {
        asAbortObjectCountCalculation	= ( 1 << 0 ) 
    } ;

EXTERN_C const IID LIBID_StorageObjectLib;

#ifndef __IStorageObject_INTERFACE_DEFINED__
#define __IStorageObject_INTERFACE_DEFINED__

/* interface IStorageObject */
/* [unique][dual][uuid][object] */ 


EXTERN_C const IID IID_IStorageObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3EE8C5C0-4144-4dbf-9AEF-12D97135FCE4")
    IStorageObject : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InitNew( 
            /* [in] */ BSTR bstr_file_name,
            /* [in] */ VARIANT_BOOL bCreateTemp) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadFile( 
            /* [in] */ BSTR bstr_file_name) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InsertRecord( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DeleteRecord( 
            /* [in] */ long lrecord) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCurrentRecord( 
            /* [retval][out] */ long *plrecord) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetCurrentRecord( 
            /* [in] */ long lrecord,
            /* [in] */ DWORD dwFlags) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetRecordCount( 
            /* [retval][out] */ long *plcount) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddObject( 
            /* [in] */ IDispatch *pdisp_object) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetObject( 
            /* [in] */ BSTR bstr_type,
            /* [retval][out] */ IDispatch **pdisp_object) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetObjectName( 
            /* [in] */ BSTR bstr_type,
            /* [retval][out] */ BSTR *pbstr_name) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetObjectCount( 
            /* [retval][out] */ long *plcount) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetObjectType( 
            /* [in] */ long lidx,
            /* [retval][out] */ BSTR *bstr_type) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DeleteObject( 
            /* [in] */ BSTR bstr_type) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ExportRecord( 
            /* [in] */ IDispatch *pdisp_data) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ImportRecord( 
            /* [in] */ IDispatch *pdisp_data) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ClearDocument( 
            /* [in] */ IDispatch *pdisp_data) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetExportRecord( 
            /* [retval][out] */ long *plrecord) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SerializeContent( 
            /* [retval][out] */ VARIANT_BOOL *pvbVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SerializeContent( 
            /* [in] */ VARIANT_BOOL vbNewVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IStorageObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IStorageObject * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IStorageObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IStorageObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IStorageObject * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IStorageObject * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IStorageObject * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IStorageObject * This,
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
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InitNew )( 
            IStorageObject * This,
            /* [in] */ BSTR bstr_file_name,
            /* [in] */ VARIANT_BOOL bCreateTemp);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadFile )( 
            IStorageObject * This,
            /* [in] */ BSTR bstr_file_name);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InsertRecord )( 
            IStorageObject * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeleteRecord )( 
            IStorageObject * This,
            /* [in] */ long lrecord);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCurrentRecord )( 
            IStorageObject * This,
            /* [retval][out] */ long *plrecord);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetCurrentRecord )( 
            IStorageObject * This,
            /* [in] */ long lrecord,
            /* [in] */ DWORD dwFlags);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetRecordCount )( 
            IStorageObject * This,
            /* [retval][out] */ long *plcount);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddObject )( 
            IStorageObject * This,
            /* [in] */ IDispatch *pdisp_object);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetObject )( 
            IStorageObject * This,
            /* [in] */ BSTR bstr_type,
            /* [retval][out] */ IDispatch **pdisp_object);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetObjectName )( 
            IStorageObject * This,
            /* [in] */ BSTR bstr_type,
            /* [retval][out] */ BSTR *pbstr_name);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetObjectCount )( 
            IStorageObject * This,
            /* [retval][out] */ long *plcount);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetObjectType )( 
            IStorageObject * This,
            /* [in] */ long lidx,
            /* [retval][out] */ BSTR *bstr_type);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeleteObject )( 
            IStorageObject * This,
            /* [in] */ BSTR bstr_type);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ExportRecord )( 
            IStorageObject * This,
            /* [in] */ IDispatch *pdisp_data);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ImportRecord )( 
            IStorageObject * This,
            /* [in] */ IDispatch *pdisp_data);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ClearDocument )( 
            IStorageObject * This,
            /* [in] */ IDispatch *pdisp_data);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetExportRecord )( 
            IStorageObject * This,
            /* [retval][out] */ long *plrecord);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            IStorageObject * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Name )( 
            IStorageObject * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SerializeContent )( 
            IStorageObject * This,
            /* [retval][out] */ VARIANT_BOOL *pvbVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_SerializeContent )( 
            IStorageObject * This,
            /* [in] */ VARIANT_BOOL vbNewVal);
        
        END_INTERFACE
    } IStorageObjectVtbl;

    interface IStorageObject
    {
        CONST_VTBL struct IStorageObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IStorageObject_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IStorageObject_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IStorageObject_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IStorageObject_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IStorageObject_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IStorageObject_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IStorageObject_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IStorageObject_InitNew(This,bstr_file_name,bCreateTemp)	\
    ( (This)->lpVtbl -> InitNew(This,bstr_file_name,bCreateTemp) ) 

#define IStorageObject_LoadFile(This,bstr_file_name)	\
    ( (This)->lpVtbl -> LoadFile(This,bstr_file_name) ) 

#define IStorageObject_InsertRecord(This)	\
    ( (This)->lpVtbl -> InsertRecord(This) ) 

#define IStorageObject_DeleteRecord(This,lrecord)	\
    ( (This)->lpVtbl -> DeleteRecord(This,lrecord) ) 

#define IStorageObject_GetCurrentRecord(This,plrecord)	\
    ( (This)->lpVtbl -> GetCurrentRecord(This,plrecord) ) 

#define IStorageObject_SetCurrentRecord(This,lrecord,dwFlags)	\
    ( (This)->lpVtbl -> SetCurrentRecord(This,lrecord,dwFlags) ) 

#define IStorageObject_GetRecordCount(This,plcount)	\
    ( (This)->lpVtbl -> GetRecordCount(This,plcount) ) 

#define IStorageObject_AddObject(This,pdisp_object)	\
    ( (This)->lpVtbl -> AddObject(This,pdisp_object) ) 

#define IStorageObject_GetObject(This,bstr_type,pdisp_object)	\
    ( (This)->lpVtbl -> GetObject(This,bstr_type,pdisp_object) ) 

#define IStorageObject_GetObjectName(This,bstr_type,pbstr_name)	\
    ( (This)->lpVtbl -> GetObjectName(This,bstr_type,pbstr_name) ) 

#define IStorageObject_GetObjectCount(This,plcount)	\
    ( (This)->lpVtbl -> GetObjectCount(This,plcount) ) 

#define IStorageObject_GetObjectType(This,lidx,bstr_type)	\
    ( (This)->lpVtbl -> GetObjectType(This,lidx,bstr_type) ) 

#define IStorageObject_DeleteObject(This,bstr_type)	\
    ( (This)->lpVtbl -> DeleteObject(This,bstr_type) ) 

#define IStorageObject_ExportRecord(This,pdisp_data)	\
    ( (This)->lpVtbl -> ExportRecord(This,pdisp_data) ) 

#define IStorageObject_ImportRecord(This,pdisp_data)	\
    ( (This)->lpVtbl -> ImportRecord(This,pdisp_data) ) 

#define IStorageObject_ClearDocument(This,pdisp_data)	\
    ( (This)->lpVtbl -> ClearDocument(This,pdisp_data) ) 

#define IStorageObject_GetExportRecord(This,plrecord)	\
    ( (This)->lpVtbl -> GetExportRecord(This,plrecord) ) 

#define IStorageObject_get_Name(This,pVal)	\
    ( (This)->lpVtbl -> get_Name(This,pVal) ) 

#define IStorageObject_put_Name(This,newVal)	\
    ( (This)->lpVtbl -> put_Name(This,newVal) ) 

#define IStorageObject_get_SerializeContent(This,pvbVal)	\
    ( (This)->lpVtbl -> get_SerializeContent(This,pvbVal) ) 

#define IStorageObject_put_SerializeContent(This,vbNewVal)	\
    ( (This)->lpVtbl -> put_SerializeContent(This,vbNewVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IStorageObject_INTERFACE_DEFINED__ */


#ifndef __IStorageObjectMisc_INTERFACE_DEFINED__
#define __IStorageObjectMisc_INTERFACE_DEFINED__

/* interface IStorageObjectMisc */
/* [object][uuid] */ 


EXTERN_C const IID IID_IStorageObjectMisc;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("02E437D0-88E3-41b1-BDDF-76493BC6848E")
    IStorageObjectMisc : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetRecordDib( 
            /* [out] */ BYTE **ppbi,
            /* [out] */ DWORD *pdwSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetObjectPropBag( 
            /* [in] */ BSTR bstr_type,
            /* [out] */ IUnknown **ppunkPropBag) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IStorageObjectMiscVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IStorageObjectMisc * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IStorageObjectMisc * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IStorageObjectMisc * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetRecordDib )( 
            IStorageObjectMisc * This,
            /* [out] */ BYTE **ppbi,
            /* [out] */ DWORD *pdwSize);
        
        HRESULT ( STDMETHODCALLTYPE *GetObjectPropBag )( 
            IStorageObjectMisc * This,
            /* [in] */ BSTR bstr_type,
            /* [out] */ IUnknown **ppunkPropBag);
        
        END_INTERFACE
    } IStorageObjectMiscVtbl;

    interface IStorageObjectMisc
    {
        CONST_VTBL struct IStorageObjectMiscVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IStorageObjectMisc_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IStorageObjectMisc_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IStorageObjectMisc_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IStorageObjectMisc_GetRecordDib(This,ppbi,pdwSize)	\
    ( (This)->lpVtbl -> GetRecordDib(This,ppbi,pdwSize) ) 

#define IStorageObjectMisc_GetObjectPropBag(This,bstr_type,ppunkPropBag)	\
    ( (This)->lpVtbl -> GetObjectPropBag(This,bstr_type,ppunkPropBag) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IStorageObjectMisc_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_StorageObject;

#ifdef __cplusplus

class DECLSPEC_UUID("772EFE3C-15DD-4a54-92C5-8476E5870428")
StorageObject;
#endif
#endif /* __StorageObjectLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


