

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Aug 25 14:43:02 2018
 */
/* Compiler settings for statobject.idl:
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


#ifndef __statobject_h_h__
#define __statobject_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IStatObjectDisp_FWD_DEFINED__
#define __IStatObjectDisp_FWD_DEFINED__
typedef interface IStatObjectDisp IStatObjectDisp;

#endif 	/* __IStatObjectDisp_FWD_DEFINED__ */


#ifndef __ICmpStatObjectDisp_FWD_DEFINED__
#define __ICmpStatObjectDisp_FWD_DEFINED__
typedef interface ICmpStatObjectDisp ICmpStatObjectDisp;

#endif 	/* __ICmpStatObjectDisp_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __StatObjectLib_LIBRARY_DEFINED__
#define __StatObjectLib_LIBRARY_DEFINED__

/* library StatObjectLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_StatObjectLib;

#ifndef __IStatObjectDisp_INTERFACE_DEFINED__
#define __IStatObjectDisp_INTERFACE_DEFINED__

/* interface IStatObjectDisp */
/* [unique][dual][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IStatObjectDisp;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("477B5F5D-D84C-4139-A1CD-79432A5DA9C8")
    IStatObjectDisp : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetValue( 
            /* [in] */ long lClass,
            /* [in] */ long lKey,
            /* [retval][out] */ VARIANT *pvarValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetValueGlobal( 
            /* [in] */ long lKey,
            /* [retval][out] */ VARIANT *pvarValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetClassCount( 
            /* [retval][out] */ long *plSz) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetClassValue( 
            /* [in] */ long lClassID,
            /* [retval][out] */ long *plClassValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetKeyCount( 
            /* [retval][out] */ long *plSz) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetKeyValue( 
            /* [in] */ long lKeyIndex,
            /* [retval][out] */ long *plKeyValue) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetValueInUnit( 
            /* [in] */ long lClass,
            /* [in] */ long lKey,
            /* [retval][out] */ VARIANT *pvarValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetValueGlobalInUnit( 
            /* [in] */ long lKey,
            /* [retval][out] */ VARIANT *pvarValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadSettingsFromSD( 
            /* [in] */ BSTR bstrSection) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE StoreSettingsToSD( 
            /* [in] */ BSTR bstrSection) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPrivateNamedData( 
            /* [retval][out] */ IDispatch **ppDisp) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE isEmpty( 
            /* [retval][out] */ boolean *pbRes) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetBagProperty( 
            /* [in] */ BSTR bstrName,
            /* [in] */ VARIANT var) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetBagProperty( 
            /* [in] */ BSTR bstrName,
            /* [retval][out] */ VARIANT *var) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetValue2( 
            /* [in] */ long lClass,
            /* [in] */ long lKey,
            /* [retval][out] */ double *pvarValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetValueGlobal2( 
            /* [in] */ long lKey,
            /* [retval][out] */ double *pvarValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetValueInUnit2( 
            /* [in] */ long lClass,
            /* [in] */ long lKey,
            /* [retval][out] */ double *pvarValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetValueGlobalInUnit2( 
            /* [in] */ long lKey,
            /* [retval][out] */ double *pvarValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetStatParamInfo( 
            /* [in] */ long lKey,
            /* [retval][out] */ VARIANT *pParamInfo) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Calculate2( 
            /* [in] */ IUnknown *punkO,
            /* [in] */ long lClassDivision,
            /* [in] */ DWORD dwFlags) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UpdateClassNames( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IStatObjectDispVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IStatObjectDisp * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IStatObjectDisp * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IStatObjectDisp * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IStatObjectDisp * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IStatObjectDisp * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IStatObjectDisp * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IStatObjectDisp * This,
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
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetValue )( 
            IStatObjectDisp * This,
            /* [in] */ long lClass,
            /* [in] */ long lKey,
            /* [retval][out] */ VARIANT *pvarValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetValueGlobal )( 
            IStatObjectDisp * This,
            /* [in] */ long lKey,
            /* [retval][out] */ VARIANT *pvarValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetClassCount )( 
            IStatObjectDisp * This,
            /* [retval][out] */ long *plSz);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetClassValue )( 
            IStatObjectDisp * This,
            /* [in] */ long lClassID,
            /* [retval][out] */ long *plClassValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetKeyCount )( 
            IStatObjectDisp * This,
            /* [retval][out] */ long *plSz);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetKeyValue )( 
            IStatObjectDisp * This,
            /* [in] */ long lKeyIndex,
            /* [retval][out] */ long *plKeyValue);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            IStatObjectDisp * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Name )( 
            IStatObjectDisp * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetValueInUnit )( 
            IStatObjectDisp * This,
            /* [in] */ long lClass,
            /* [in] */ long lKey,
            /* [retval][out] */ VARIANT *pvarValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetValueGlobalInUnit )( 
            IStatObjectDisp * This,
            /* [in] */ long lKey,
            /* [retval][out] */ VARIANT *pvarValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadSettingsFromSD )( 
            IStatObjectDisp * This,
            /* [in] */ BSTR bstrSection);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *StoreSettingsToSD )( 
            IStatObjectDisp * This,
            /* [in] */ BSTR bstrSection);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPrivateNamedData )( 
            IStatObjectDisp * This,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *isEmpty )( 
            IStatObjectDisp * This,
            /* [retval][out] */ boolean *pbRes);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetBagProperty )( 
            IStatObjectDisp * This,
            /* [in] */ BSTR bstrName,
            /* [in] */ VARIANT var);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetBagProperty )( 
            IStatObjectDisp * This,
            /* [in] */ BSTR bstrName,
            /* [retval][out] */ VARIANT *var);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetValue2 )( 
            IStatObjectDisp * This,
            /* [in] */ long lClass,
            /* [in] */ long lKey,
            /* [retval][out] */ double *pvarValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetValueGlobal2 )( 
            IStatObjectDisp * This,
            /* [in] */ long lKey,
            /* [retval][out] */ double *pvarValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetValueInUnit2 )( 
            IStatObjectDisp * This,
            /* [in] */ long lClass,
            /* [in] */ long lKey,
            /* [retval][out] */ double *pvarValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetValueGlobalInUnit2 )( 
            IStatObjectDisp * This,
            /* [in] */ long lKey,
            /* [retval][out] */ double *pvarValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetStatParamInfo )( 
            IStatObjectDisp * This,
            /* [in] */ long lKey,
            /* [retval][out] */ VARIANT *pParamInfo);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Calculate2 )( 
            IStatObjectDisp * This,
            /* [in] */ IUnknown *punkO,
            /* [in] */ long lClassDivision,
            /* [in] */ DWORD dwFlags);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UpdateClassNames )( 
            IStatObjectDisp * This);
        
        END_INTERFACE
    } IStatObjectDispVtbl;

    interface IStatObjectDisp
    {
        CONST_VTBL struct IStatObjectDispVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IStatObjectDisp_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IStatObjectDisp_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IStatObjectDisp_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IStatObjectDisp_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IStatObjectDisp_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IStatObjectDisp_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IStatObjectDisp_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IStatObjectDisp_GetValue(This,lClass,lKey,pvarValue)	\
    ( (This)->lpVtbl -> GetValue(This,lClass,lKey,pvarValue) ) 

#define IStatObjectDisp_GetValueGlobal(This,lKey,pvarValue)	\
    ( (This)->lpVtbl -> GetValueGlobal(This,lKey,pvarValue) ) 

#define IStatObjectDisp_GetClassCount(This,plSz)	\
    ( (This)->lpVtbl -> GetClassCount(This,plSz) ) 

#define IStatObjectDisp_GetClassValue(This,lClassID,plClassValue)	\
    ( (This)->lpVtbl -> GetClassValue(This,lClassID,plClassValue) ) 

#define IStatObjectDisp_GetKeyCount(This,plSz)	\
    ( (This)->lpVtbl -> GetKeyCount(This,plSz) ) 

#define IStatObjectDisp_GetKeyValue(This,lKeyIndex,plKeyValue)	\
    ( (This)->lpVtbl -> GetKeyValue(This,lKeyIndex,plKeyValue) ) 

#define IStatObjectDisp_get_Name(This,pVal)	\
    ( (This)->lpVtbl -> get_Name(This,pVal) ) 

#define IStatObjectDisp_put_Name(This,newVal)	\
    ( (This)->lpVtbl -> put_Name(This,newVal) ) 

#define IStatObjectDisp_GetValueInUnit(This,lClass,lKey,pvarValue)	\
    ( (This)->lpVtbl -> GetValueInUnit(This,lClass,lKey,pvarValue) ) 

#define IStatObjectDisp_GetValueGlobalInUnit(This,lKey,pvarValue)	\
    ( (This)->lpVtbl -> GetValueGlobalInUnit(This,lKey,pvarValue) ) 

#define IStatObjectDisp_LoadSettingsFromSD(This,bstrSection)	\
    ( (This)->lpVtbl -> LoadSettingsFromSD(This,bstrSection) ) 

#define IStatObjectDisp_StoreSettingsToSD(This,bstrSection)	\
    ( (This)->lpVtbl -> StoreSettingsToSD(This,bstrSection) ) 

#define IStatObjectDisp_GetPrivateNamedData(This,ppDisp)	\
    ( (This)->lpVtbl -> GetPrivateNamedData(This,ppDisp) ) 

#define IStatObjectDisp_isEmpty(This,pbRes)	\
    ( (This)->lpVtbl -> isEmpty(This,pbRes) ) 

#define IStatObjectDisp_SetBagProperty(This,bstrName,var)	\
    ( (This)->lpVtbl -> SetBagProperty(This,bstrName,var) ) 

#define IStatObjectDisp_GetBagProperty(This,bstrName,var)	\
    ( (This)->lpVtbl -> GetBagProperty(This,bstrName,var) ) 

#define IStatObjectDisp_GetValue2(This,lClass,lKey,pvarValue)	\
    ( (This)->lpVtbl -> GetValue2(This,lClass,lKey,pvarValue) ) 

#define IStatObjectDisp_GetValueGlobal2(This,lKey,pvarValue)	\
    ( (This)->lpVtbl -> GetValueGlobal2(This,lKey,pvarValue) ) 

#define IStatObjectDisp_GetValueInUnit2(This,lClass,lKey,pvarValue)	\
    ( (This)->lpVtbl -> GetValueInUnit2(This,lClass,lKey,pvarValue) ) 

#define IStatObjectDisp_GetValueGlobalInUnit2(This,lKey,pvarValue)	\
    ( (This)->lpVtbl -> GetValueGlobalInUnit2(This,lKey,pvarValue) ) 

#define IStatObjectDisp_GetStatParamInfo(This,lKey,pParamInfo)	\
    ( (This)->lpVtbl -> GetStatParamInfo(This,lKey,pParamInfo) ) 

#define IStatObjectDisp_Calculate2(This,punkO,lClassDivision,dwFlags)	\
    ( (This)->lpVtbl -> Calculate2(This,punkO,lClassDivision,dwFlags) ) 

#define IStatObjectDisp_UpdateClassNames(This)	\
    ( (This)->lpVtbl -> UpdateClassNames(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IStatObjectDisp_INTERFACE_DEFINED__ */


#ifndef __ICmpStatObjectDisp_INTERFACE_DEFINED__
#define __ICmpStatObjectDisp_INTERFACE_DEFINED__

/* interface ICmpStatObjectDisp */
/* [unique][dual][helpstring][uuid][object] */ 


EXTERN_C const IID IID_ICmpStatObjectDisp;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F4B2EF4D-AA00-4992-8EB8-2C9FD6E7CA93")
    ICmpStatObjectDisp : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Statuses( 
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Statuses( 
            /* [in] */ VARIANT newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Compare( 
            /* [retval][out] */ LONG *rc) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPrivateNamedData( 
            /* [retval][out] */ IDispatch **ppDisp) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ICmpStatObjectDispVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICmpStatObjectDisp * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICmpStatObjectDisp * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICmpStatObjectDisp * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ICmpStatObjectDisp * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ICmpStatObjectDisp * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ICmpStatObjectDisp * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ICmpStatObjectDisp * This,
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
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Statuses )( 
            ICmpStatObjectDisp * This,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Statuses )( 
            ICmpStatObjectDisp * This,
            /* [in] */ VARIANT newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Compare )( 
            ICmpStatObjectDisp * This,
            /* [retval][out] */ LONG *rc);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPrivateNamedData )( 
            ICmpStatObjectDisp * This,
            /* [retval][out] */ IDispatch **ppDisp);
        
        END_INTERFACE
    } ICmpStatObjectDispVtbl;

    interface ICmpStatObjectDisp
    {
        CONST_VTBL struct ICmpStatObjectDispVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICmpStatObjectDisp_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICmpStatObjectDisp_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICmpStatObjectDisp_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICmpStatObjectDisp_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ICmpStatObjectDisp_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ICmpStatObjectDisp_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ICmpStatObjectDisp_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ICmpStatObjectDisp_get_Statuses(This,pVal)	\
    ( (This)->lpVtbl -> get_Statuses(This,pVal) ) 

#define ICmpStatObjectDisp_put_Statuses(This,newVal)	\
    ( (This)->lpVtbl -> put_Statuses(This,newVal) ) 

#define ICmpStatObjectDisp_Compare(This,rc)	\
    ( (This)->lpVtbl -> Compare(This,rc) ) 

#define ICmpStatObjectDisp_GetPrivateNamedData(This,ppDisp)	\
    ( (This)->lpVtbl -> GetPrivateNamedData(This,ppDisp) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICmpStatObjectDisp_INTERFACE_DEFINED__ */

#endif /* __StatObjectLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


