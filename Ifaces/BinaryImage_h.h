

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Aug 25 14:42:52 2018
 */
/* Compiler settings for BinaryImage.idl:
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


#ifndef __BinaryImage_h_h__
#define __BinaryImage_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IBinaryImageDisp_FWD_DEFINED__
#define __IBinaryImageDisp_FWD_DEFINED__
typedef interface IBinaryImageDisp IBinaryImageDisp;

#endif 	/* __IBinaryImageDisp_FWD_DEFINED__ */


#ifndef __BinaryImageDisp_FWD_DEFINED__
#define __BinaryImageDisp_FWD_DEFINED__

#ifdef __cplusplus
typedef class BinaryImageDisp BinaryImageDisp;
#else
typedef struct BinaryImageDisp BinaryImageDisp;
#endif /* __cplusplus */

#endif 	/* __BinaryImageDisp_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __ABinaryImage_LIBRARY_DEFINED__
#define __ABinaryImage_LIBRARY_DEFINED__

/* library ABinaryImage */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_ABinaryImage;

#ifndef __IBinaryImageDisp_INTERFACE_DEFINED__
#define __IBinaryImageDisp_INTERFACE_DEFINED__

/* interface IBinaryImageDisp */
/* [unique][dual][uuid][object] */ 


EXTERN_C const IID IID_IBinaryImageDisp;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("72AE473E-F865-41ee-9B53-1AD260212355")
    IBinaryImageDisp : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetType( 
            /* [retval][out] */ BSTR *pbstrType) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IBinaryImageDispVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBinaryImageDisp * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBinaryImageDisp * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBinaryImageDisp * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IBinaryImageDisp * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IBinaryImageDisp * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IBinaryImageDisp * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IBinaryImageDisp * This,
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
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            IBinaryImageDisp * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Name )( 
            IBinaryImageDisp * This,
            /* [in] */ BSTR newVal);
        
        HRESULT ( STDMETHODCALLTYPE *GetType )( 
            IBinaryImageDisp * This,
            /* [retval][out] */ BSTR *pbstrType);
        
        END_INTERFACE
    } IBinaryImageDispVtbl;

    interface IBinaryImageDisp
    {
        CONST_VTBL struct IBinaryImageDispVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBinaryImageDisp_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IBinaryImageDisp_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IBinaryImageDisp_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IBinaryImageDisp_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IBinaryImageDisp_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IBinaryImageDisp_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IBinaryImageDisp_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IBinaryImageDisp_get_Name(This,pVal)	\
    ( (This)->lpVtbl -> get_Name(This,pVal) ) 

#define IBinaryImageDisp_put_Name(This,newVal)	\
    ( (This)->lpVtbl -> put_Name(This,newVal) ) 

#define IBinaryImageDisp_GetType(This,pbstrType)	\
    ( (This)->lpVtbl -> GetType(This,pbstrType) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IBinaryImageDisp_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_BinaryImageDisp;

#ifdef __cplusplus

class DECLSPEC_UUID("8DDF82EA-680E-4ae5-917C-766F6CC0F8BC")
BinaryImageDisp;
#endif
#endif /* __ABinaryImage_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


