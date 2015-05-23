

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sun Apr 26 01:55:52 2015
 */
/* Compiler settings for StatData.idl:
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


#ifndef __StatData_h_h__
#define __StatData_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IStatTableDisp_FWD_DEFINED__
#define __IStatTableDisp_FWD_DEFINED__
typedef interface IStatTableDisp IStatTableDisp;

#endif 	/* __IStatTableDisp_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __StatDataLib_LIBRARY_DEFINED__
#define __StatDataLib_LIBRARY_DEFINED__

/* library StatDataLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_StatDataLib;

#ifndef __IStatTableDisp_INTERFACE_DEFINED__
#define __IStatTableDisp_INTERFACE_DEFINED__

/* interface IStatTableDisp */
/* [unique][dual][uuid][object] */ 


EXTERN_C const IID IID_IStatTableDisp;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("340FA4B3-6D3F-470d-A489-689D0B4E8094")
    IStatTableDisp : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE getFirstParamPos( 
            /* [retval][out] */ VARIANT *pv_pos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE getNextParamPos( 
            /* [in] */ LONG_PTR lpos,
            /* [retval][out] */ VARIANT *pv_pos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE getParamKey( 
            /* [in] */ LONG_PTR lpos,
            /* [retval][out] */ VARIANT *pv_key) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE getFirstRowPos( 
            /* [retval][out] */ VARIANT *pv_pos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE getNextRowPos( 
            /* [in] */ LONG_PTR lpos,
            /* [retval][out] */ VARIANT *pv_pos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE getRowCount( 
            /* [retval][out] */ VARIANT *pv_count) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE getRowGroup( 
            /* [in] */ LONG_PTR lpos,
            /* [retval][out] */ VARIANT *pv_group) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE getRowClass( 
            /* [in] */ LONG_PTR lpos,
            /* [in] */ BSTR class_file,
            /* [retval][out] */ VARIANT *pv_class) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE deleteRow( 
            /* [in] */ LONG_PTR lpos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE getValueByKey( 
            /* [in] */ LONG_PTR lpos,
            /* [in] */ long lkey,
            /* [retval][out] */ VARIANT *pv_value) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE getValueByParamPos( 
            /* [in] */ LONG_PTR lpos,
            /* [in] */ LONG_PTR lparam_pos,
            /* [retval][out] */ VARIANT *pv_value) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE getGroupCount( 
            /* [retval][out] */ VARIANT *pv_count) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE getGroup( 
            /* [in] */ long lpos,
            /* [retval][out] */ VARIANT *pv_group_ret) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE updateObject( 
            long lflags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE relloadSettings( 
            long lflags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPrivateNamedData( 
            /* [retval][out] */ IDispatch **ppDisp) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsEmpty( 
            /* [retval][out] */ VARIANT_BOOL *pvbIsEmpty) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetType( 
            /* [retval][out] */ BSTR *pbstrType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE getValueByKeyInUnit( 
            /* [in] */ LONG_PTR lpos,
            /* [in] */ long lkey,
            /* [retval][out] */ VARIANT *pv_value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UpdateClassNames( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IStatTableDispVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IStatTableDisp * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IStatTableDisp * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IStatTableDisp * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IStatTableDisp * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IStatTableDisp * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IStatTableDisp * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IStatTableDisp * This,
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
        
        HRESULT ( STDMETHODCALLTYPE *getFirstParamPos )( 
            IStatTableDisp * This,
            /* [retval][out] */ VARIANT *pv_pos);
        
        HRESULT ( STDMETHODCALLTYPE *getNextParamPos )( 
            IStatTableDisp * This,
            /* [in] */ LONG_PTR lpos,
            /* [retval][out] */ VARIANT *pv_pos);
        
        HRESULT ( STDMETHODCALLTYPE *getParamKey )( 
            IStatTableDisp * This,
            /* [in] */ LONG_PTR lpos,
            /* [retval][out] */ VARIANT *pv_key);
        
        HRESULT ( STDMETHODCALLTYPE *getFirstRowPos )( 
            IStatTableDisp * This,
            /* [retval][out] */ VARIANT *pv_pos);
        
        HRESULT ( STDMETHODCALLTYPE *getNextRowPos )( 
            IStatTableDisp * This,
            /* [in] */ LONG_PTR lpos,
            /* [retval][out] */ VARIANT *pv_pos);
        
        HRESULT ( STDMETHODCALLTYPE *getRowCount )( 
            IStatTableDisp * This,
            /* [retval][out] */ VARIANT *pv_count);
        
        HRESULT ( STDMETHODCALLTYPE *getRowGroup )( 
            IStatTableDisp * This,
            /* [in] */ LONG_PTR lpos,
            /* [retval][out] */ VARIANT *pv_group);
        
        HRESULT ( STDMETHODCALLTYPE *getRowClass )( 
            IStatTableDisp * This,
            /* [in] */ LONG_PTR lpos,
            /* [in] */ BSTR class_file,
            /* [retval][out] */ VARIANT *pv_class);
        
        HRESULT ( STDMETHODCALLTYPE *deleteRow )( 
            IStatTableDisp * This,
            /* [in] */ LONG_PTR lpos);
        
        HRESULT ( STDMETHODCALLTYPE *getValueByKey )( 
            IStatTableDisp * This,
            /* [in] */ LONG_PTR lpos,
            /* [in] */ long lkey,
            /* [retval][out] */ VARIANT *pv_value);
        
        HRESULT ( STDMETHODCALLTYPE *getValueByParamPos )( 
            IStatTableDisp * This,
            /* [in] */ LONG_PTR lpos,
            /* [in] */ LONG_PTR lparam_pos,
            /* [retval][out] */ VARIANT *pv_value);
        
        HRESULT ( STDMETHODCALLTYPE *getGroupCount )( 
            IStatTableDisp * This,
            /* [retval][out] */ VARIANT *pv_count);
        
        HRESULT ( STDMETHODCALLTYPE *getGroup )( 
            IStatTableDisp * This,
            /* [in] */ long lpos,
            /* [retval][out] */ VARIANT *pv_group_ret);
        
        HRESULT ( STDMETHODCALLTYPE *updateObject )( 
            IStatTableDisp * This,
            long lflags);
        
        HRESULT ( STDMETHODCALLTYPE *relloadSettings )( 
            IStatTableDisp * This,
            long lflags);
        
        HRESULT ( STDMETHODCALLTYPE *GetPrivateNamedData )( 
            IStatTableDisp * This,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            IStatTableDisp * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Name )( 
            IStatTableDisp * This,
            /* [in] */ BSTR newVal);
        
        HRESULT ( STDMETHODCALLTYPE *IsEmpty )( 
            IStatTableDisp * This,
            /* [retval][out] */ VARIANT_BOOL *pvbIsEmpty);
        
        HRESULT ( STDMETHODCALLTYPE *GetType )( 
            IStatTableDisp * This,
            /* [retval][out] */ BSTR *pbstrType);
        
        HRESULT ( STDMETHODCALLTYPE *getValueByKeyInUnit )( 
            IStatTableDisp * This,
            /* [in] */ LONG_PTR lpos,
            /* [in] */ long lkey,
            /* [retval][out] */ VARIANT *pv_value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UpdateClassNames )( 
            IStatTableDisp * This);
        
        END_INTERFACE
    } IStatTableDispVtbl;

    interface IStatTableDisp
    {
        CONST_VTBL struct IStatTableDispVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IStatTableDisp_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IStatTableDisp_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IStatTableDisp_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IStatTableDisp_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IStatTableDisp_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IStatTableDisp_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IStatTableDisp_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IStatTableDisp_getFirstParamPos(This,pv_pos)	\
    ( (This)->lpVtbl -> getFirstParamPos(This,pv_pos) ) 

#define IStatTableDisp_getNextParamPos(This,lpos,pv_pos)	\
    ( (This)->lpVtbl -> getNextParamPos(This,lpos,pv_pos) ) 

#define IStatTableDisp_getParamKey(This,lpos,pv_key)	\
    ( (This)->lpVtbl -> getParamKey(This,lpos,pv_key) ) 

#define IStatTableDisp_getFirstRowPos(This,pv_pos)	\
    ( (This)->lpVtbl -> getFirstRowPos(This,pv_pos) ) 

#define IStatTableDisp_getNextRowPos(This,lpos,pv_pos)	\
    ( (This)->lpVtbl -> getNextRowPos(This,lpos,pv_pos) ) 

#define IStatTableDisp_getRowCount(This,pv_count)	\
    ( (This)->lpVtbl -> getRowCount(This,pv_count) ) 

#define IStatTableDisp_getRowGroup(This,lpos,pv_group)	\
    ( (This)->lpVtbl -> getRowGroup(This,lpos,pv_group) ) 

#define IStatTableDisp_getRowClass(This,lpos,class_file,pv_class)	\
    ( (This)->lpVtbl -> getRowClass(This,lpos,class_file,pv_class) ) 

#define IStatTableDisp_deleteRow(This,lpos)	\
    ( (This)->lpVtbl -> deleteRow(This,lpos) ) 

#define IStatTableDisp_getValueByKey(This,lpos,lkey,pv_value)	\
    ( (This)->lpVtbl -> getValueByKey(This,lpos,lkey,pv_value) ) 

#define IStatTableDisp_getValueByParamPos(This,lpos,lparam_pos,pv_value)	\
    ( (This)->lpVtbl -> getValueByParamPos(This,lpos,lparam_pos,pv_value) ) 

#define IStatTableDisp_getGroupCount(This,pv_count)	\
    ( (This)->lpVtbl -> getGroupCount(This,pv_count) ) 

#define IStatTableDisp_getGroup(This,lpos,pv_group_ret)	\
    ( (This)->lpVtbl -> getGroup(This,lpos,pv_group_ret) ) 

#define IStatTableDisp_updateObject(This,lflags)	\
    ( (This)->lpVtbl -> updateObject(This,lflags) ) 

#define IStatTableDisp_relloadSettings(This,lflags)	\
    ( (This)->lpVtbl -> relloadSettings(This,lflags) ) 

#define IStatTableDisp_GetPrivateNamedData(This,ppDisp)	\
    ( (This)->lpVtbl -> GetPrivateNamedData(This,ppDisp) ) 

#define IStatTableDisp_get_Name(This,pVal)	\
    ( (This)->lpVtbl -> get_Name(This,pVal) ) 

#define IStatTableDisp_put_Name(This,newVal)	\
    ( (This)->lpVtbl -> put_Name(This,newVal) ) 

#define IStatTableDisp_IsEmpty(This,pvbIsEmpty)	\
    ( (This)->lpVtbl -> IsEmpty(This,pvbIsEmpty) ) 

#define IStatTableDisp_GetType(This,pbstrType)	\
    ( (This)->lpVtbl -> GetType(This,pbstrType) ) 

#define IStatTableDisp_getValueByKeyInUnit(This,lpos,lkey,pv_value)	\
    ( (This)->lpVtbl -> getValueByKeyInUnit(This,lpos,lkey,pv_value) ) 

#define IStatTableDisp_UpdateClassNames(This)	\
    ( (This)->lpVtbl -> UpdateClassNames(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IStatTableDisp_INTERFACE_DEFINED__ */

#endif /* __StatDataLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


