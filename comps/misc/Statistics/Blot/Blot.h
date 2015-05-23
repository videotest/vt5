

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sun Apr 12 10:29:24 2015
 */
/* Compiler settings for Blot.idl:
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

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __Blot_h__
#define __Blot_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IBlotOb_FWD_DEFINED__
#define __IBlotOb_FWD_DEFINED__
typedef interface IBlotOb IBlotOb;

#endif 	/* __IBlotOb_FWD_DEFINED__ */


#ifndef __BlotOb_FWD_DEFINED__
#define __BlotOb_FWD_DEFINED__

#ifdef __cplusplus
typedef class BlotOb BlotOb;
#else
typedef struct BlotOb BlotOb;
#endif /* __cplusplus */

#endif 	/* __BlotOb_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IBlotOb_INTERFACE_DEFINED__
#define __IBlotOb_INTERFACE_DEFINED__

/* interface IBlotOb */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IBlotOb;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EDCFC6FE-F30F-419B-930E-94DB8A5598E2")
    IBlotOb : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Classes( 
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Classes( 
            /* [in] */ VARIANT newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Table( 
            /* [retval][out] */ IUnknown **pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Table( 
            /* [in] */ IUnknown *newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_XPrecision( 
            /* [retval][out] */ FLOAT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_XPrecision( 
            /* [in] */ FLOAT newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_YPrecision( 
            /* [retval][out] */ FLOAT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_YPrecision( 
            /* [in] */ FLOAT newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ SHORT *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Area( 
            /* [in] */ SHORT Index,
            /* [retval][out] */ FLOAT *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MassX( 
            /* [in] */ SHORT Index,
            /* [retval][out] */ FLOAT *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MassY( 
            /* [in] */ SHORT Index,
            /* [retval][out] */ FLOAT *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Find( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddClass( 
            /* [in] */ SHORT iClass) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetPictureSize( 
            /* [in] */ FLOAT x,
            /* [in] */ FLOAT y) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetOffs( 
            /* [in] */ BSTR Picture,
            /* [in] */ FLOAT x,
            /* [in] */ FLOAT y) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MaxOverlapArea( 
            /* [retval][out] */ FLOAT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_MaxOverlapArea( 
            /* [in] */ FLOAT newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_XShift( 
            /* [in] */ SHORT Index,
            /* [retval][out] */ FLOAT *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_YShift( 
            /* [in] */ SHORT Index,
            /* [retval][out] */ FLOAT *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_NCadr( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_NCadr( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetFrameSize( 
            /* [in] */ FLOAT x,
            /* [in] */ FLOAT y) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IBlotObVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBlotOb * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBlotOb * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBlotOb * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IBlotOb * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IBlotOb * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IBlotOb * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IBlotOb * This,
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
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Classes )( 
            IBlotOb * This,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Classes )( 
            IBlotOb * This,
            /* [in] */ VARIANT newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Table )( 
            IBlotOb * This,
            /* [retval][out] */ IUnknown **pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Table )( 
            IBlotOb * This,
            /* [in] */ IUnknown *newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_XPrecision )( 
            IBlotOb * This,
            /* [retval][out] */ FLOAT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_XPrecision )( 
            IBlotOb * This,
            /* [in] */ FLOAT newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_YPrecision )( 
            IBlotOb * This,
            /* [retval][out] */ FLOAT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_YPrecision )( 
            IBlotOb * This,
            /* [in] */ FLOAT newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IBlotOb * This,
            /* [retval][out] */ SHORT *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Area )( 
            IBlotOb * This,
            /* [in] */ SHORT Index,
            /* [retval][out] */ FLOAT *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MassX )( 
            IBlotOb * This,
            /* [in] */ SHORT Index,
            /* [retval][out] */ FLOAT *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MassY )( 
            IBlotOb * This,
            /* [in] */ SHORT Index,
            /* [retval][out] */ FLOAT *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Find )( 
            IBlotOb * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddClass )( 
            IBlotOb * This,
            /* [in] */ SHORT iClass);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetPictureSize )( 
            IBlotOb * This,
            /* [in] */ FLOAT x,
            /* [in] */ FLOAT y);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetOffs )( 
            IBlotOb * This,
            /* [in] */ BSTR Picture,
            /* [in] */ FLOAT x,
            /* [in] */ FLOAT y);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MaxOverlapArea )( 
            IBlotOb * This,
            /* [retval][out] */ FLOAT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_MaxOverlapArea )( 
            IBlotOb * This,
            /* [in] */ FLOAT newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_XShift )( 
            IBlotOb * This,
            /* [in] */ SHORT Index,
            /* [retval][out] */ FLOAT *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_YShift )( 
            IBlotOb * This,
            /* [in] */ SHORT Index,
            /* [retval][out] */ FLOAT *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_NCadr )( 
            IBlotOb * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_NCadr )( 
            IBlotOb * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetFrameSize )( 
            IBlotOb * This,
            /* [in] */ FLOAT x,
            /* [in] */ FLOAT y);
        
        END_INTERFACE
    } IBlotObVtbl;

    interface IBlotOb
    {
        CONST_VTBL struct IBlotObVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBlotOb_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IBlotOb_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IBlotOb_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IBlotOb_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IBlotOb_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IBlotOb_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IBlotOb_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IBlotOb_get_Classes(This,pVal)	\
    ( (This)->lpVtbl -> get_Classes(This,pVal) ) 

#define IBlotOb_put_Classes(This,newVal)	\
    ( (This)->lpVtbl -> put_Classes(This,newVal) ) 

#define IBlotOb_get_Table(This,pVal)	\
    ( (This)->lpVtbl -> get_Table(This,pVal) ) 

#define IBlotOb_put_Table(This,newVal)	\
    ( (This)->lpVtbl -> put_Table(This,newVal) ) 

#define IBlotOb_get_XPrecision(This,pVal)	\
    ( (This)->lpVtbl -> get_XPrecision(This,pVal) ) 

#define IBlotOb_put_XPrecision(This,newVal)	\
    ( (This)->lpVtbl -> put_XPrecision(This,newVal) ) 

#define IBlotOb_get_YPrecision(This,pVal)	\
    ( (This)->lpVtbl -> get_YPrecision(This,pVal) ) 

#define IBlotOb_put_YPrecision(This,newVal)	\
    ( (This)->lpVtbl -> put_YPrecision(This,newVal) ) 

#define IBlotOb_get_Count(This,pVal)	\
    ( (This)->lpVtbl -> get_Count(This,pVal) ) 

#define IBlotOb_get_Area(This,Index,pVal)	\
    ( (This)->lpVtbl -> get_Area(This,Index,pVal) ) 

#define IBlotOb_get_MassX(This,Index,pVal)	\
    ( (This)->lpVtbl -> get_MassX(This,Index,pVal) ) 

#define IBlotOb_get_MassY(This,Index,pVal)	\
    ( (This)->lpVtbl -> get_MassY(This,Index,pVal) ) 

#define IBlotOb_Find(This)	\
    ( (This)->lpVtbl -> Find(This) ) 

#define IBlotOb_AddClass(This,iClass)	\
    ( (This)->lpVtbl -> AddClass(This,iClass) ) 

#define IBlotOb_SetPictureSize(This,x,y)	\
    ( (This)->lpVtbl -> SetPictureSize(This,x,y) ) 

#define IBlotOb_SetOffs(This,Picture,x,y)	\
    ( (This)->lpVtbl -> SetOffs(This,Picture,x,y) ) 

#define IBlotOb_get_MaxOverlapArea(This,pVal)	\
    ( (This)->lpVtbl -> get_MaxOverlapArea(This,pVal) ) 

#define IBlotOb_put_MaxOverlapArea(This,newVal)	\
    ( (This)->lpVtbl -> put_MaxOverlapArea(This,newVal) ) 

#define IBlotOb_get_XShift(This,Index,pVal)	\
    ( (This)->lpVtbl -> get_XShift(This,Index,pVal) ) 

#define IBlotOb_get_YShift(This,Index,pVal)	\
    ( (This)->lpVtbl -> get_YShift(This,Index,pVal) ) 

#define IBlotOb_get_NCadr(This,pVal)	\
    ( (This)->lpVtbl -> get_NCadr(This,pVal) ) 

#define IBlotOb_put_NCadr(This,newVal)	\
    ( (This)->lpVtbl -> put_NCadr(This,newVal) ) 

#define IBlotOb_SetFrameSize(This,x,y)	\
    ( (This)->lpVtbl -> SetFrameSize(This,x,y) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IBlotOb_INTERFACE_DEFINED__ */



#ifndef __BlotLib_LIBRARY_DEFINED__
#define __BlotLib_LIBRARY_DEFINED__

/* library BlotLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_BlotLib;

EXTERN_C const CLSID CLSID_BlotOb;

#ifdef __cplusplus

class DECLSPEC_UUID("390CF2CE-AFAC-4DA4-B794-95232AB9258D")
BlotOb;
#endif
#endif /* __BlotLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


