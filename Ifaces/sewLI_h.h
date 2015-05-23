

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sun Apr 26 01:55:50 2015
 */
/* Compiler settings for sewLI.idl:
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


#ifndef __sewLI_h_h__
#define __sewLI_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ISewLIView_FWD_DEFINED__
#define __ISewLIView_FWD_DEFINED__
typedef interface ISewLIView ISewLIView;

#endif 	/* __ISewLIView_FWD_DEFINED__ */


#ifndef __SewLIView_FWD_DEFINED__
#define __SewLIView_FWD_DEFINED__

#ifdef __cplusplus
typedef class SewLIView SewLIView;
#else
typedef struct SewLIView SewLIView;
#endif /* __cplusplus */

#endif 	/* __SewLIView_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __SewLIViewLib_LIBRARY_DEFINED__
#define __SewLIViewLib_LIBRARY_DEFINED__

/* library SewLIViewLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_SewLIViewLib;

#ifndef __ISewLIView_INTERFACE_DEFINED__
#define __ISewLIView_INTERFACE_DEFINED__

/* interface ISewLIView */
/* [unique][dual][uuid][object] */ 


EXTERN_C const IID IID_ISewLIView;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4B523B7D-81E1-48c5-BCAD-BA1414624894")
    ISewLIView : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OverlapProcent( 
            /* [retval][out] */ int *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OverlayCoefficient( 
            /* [retval][out] */ int *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ShowLiveVideo( 
            /* [retval][out] */ int *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ShowLiveVideo( 
            /* [in] */ int newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetLiveVideoRect( 
            /* [out] */ int *pLeft,
            /* [out] */ int *pTop,
            /* [out] */ int *pRight,
            /* [out] */ int *pBottom) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDocumentOrg( 
            /* [out] */ int *px,
            /* [out] */ int *py) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MoveLiveVideo( 
            /* [in] */ int x,
            /* [in] */ int y,
            /* [in] */ int nFlags) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LiveVideoMoving( 
            /* [retval][out] */ int *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_LiveVideoMoving( 
            /* [in] */ int newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MoveLiveVideoTo( 
            /* [in] */ int x,
            /* [in] */ int y,
            /* [in] */ int nFlags) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AutoMoveLiveVideoMode( 
            /* [retval][out] */ int *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_AutoMoveLiveVideoMode( 
            /* [in] */ int newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ShowLiveVideoMode( 
            /* [retval][out] */ int *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ShowLiveVideoMode( 
            /* [in] */ int newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_BaseFragment( 
            /* [retval][out] */ IUnknown **ppunkVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LastFragmentMode( 
            /* [retval][out] */ int *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_LastFragmentMode( 
            /* [in] */ int newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ShowFrame( 
            /* [retval][out] */ int *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ShowFrame( 
            /* [in] */ int newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_NextFragmentDirection( 
            /* [retval][out] */ double *pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ISewLIViewVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISewLIView * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISewLIView * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISewLIView * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISewLIView * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISewLIView * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISewLIView * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISewLIView * This,
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
            ISewLIView * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Name )( 
            ISewLIView * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_OverlapProcent )( 
            ISewLIView * This,
            /* [retval][out] */ int *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_OverlayCoefficient )( 
            ISewLIView * This,
            /* [retval][out] */ int *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ShowLiveVideo )( 
            ISewLIView * This,
            /* [retval][out] */ int *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ShowLiveVideo )( 
            ISewLIView * This,
            /* [in] */ int newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetLiveVideoRect )( 
            ISewLIView * This,
            /* [out] */ int *pLeft,
            /* [out] */ int *pTop,
            /* [out] */ int *pRight,
            /* [out] */ int *pBottom);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDocumentOrg )( 
            ISewLIView * This,
            /* [out] */ int *px,
            /* [out] */ int *py);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *MoveLiveVideo )( 
            ISewLIView * This,
            /* [in] */ int x,
            /* [in] */ int y,
            /* [in] */ int nFlags);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LiveVideoMoving )( 
            ISewLIView * This,
            /* [retval][out] */ int *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_LiveVideoMoving )( 
            ISewLIView * This,
            /* [in] */ int newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *MoveLiveVideoTo )( 
            ISewLIView * This,
            /* [in] */ int x,
            /* [in] */ int y,
            /* [in] */ int nFlags);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_AutoMoveLiveVideoMode )( 
            ISewLIView * This,
            /* [retval][out] */ int *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_AutoMoveLiveVideoMode )( 
            ISewLIView * This,
            /* [in] */ int newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ShowLiveVideoMode )( 
            ISewLIView * This,
            /* [retval][out] */ int *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ShowLiveVideoMode )( 
            ISewLIView * This,
            /* [in] */ int newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BaseFragment )( 
            ISewLIView * This,
            /* [retval][out] */ IUnknown **ppunkVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LastFragmentMode )( 
            ISewLIView * This,
            /* [retval][out] */ int *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_LastFragmentMode )( 
            ISewLIView * This,
            /* [in] */ int newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ShowFrame )( 
            ISewLIView * This,
            /* [retval][out] */ int *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ShowFrame )( 
            ISewLIView * This,
            /* [in] */ int newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_NextFragmentDirection )( 
            ISewLIView * This,
            /* [retval][out] */ double *pVal);
        
        END_INTERFACE
    } ISewLIViewVtbl;

    interface ISewLIView
    {
        CONST_VTBL struct ISewLIViewVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISewLIView_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISewLIView_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISewLIView_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISewLIView_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISewLIView_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISewLIView_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISewLIView_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ISewLIView_get_Name(This,pVal)	\
    ( (This)->lpVtbl -> get_Name(This,pVal) ) 

#define ISewLIView_put_Name(This,newVal)	\
    ( (This)->lpVtbl -> put_Name(This,newVal) ) 

#define ISewLIView_get_OverlapProcent(This,pVal)	\
    ( (This)->lpVtbl -> get_OverlapProcent(This,pVal) ) 

#define ISewLIView_get_OverlayCoefficient(This,pVal)	\
    ( (This)->lpVtbl -> get_OverlayCoefficient(This,pVal) ) 

#define ISewLIView_get_ShowLiveVideo(This,pVal)	\
    ( (This)->lpVtbl -> get_ShowLiveVideo(This,pVal) ) 

#define ISewLIView_put_ShowLiveVideo(This,newVal)	\
    ( (This)->lpVtbl -> put_ShowLiveVideo(This,newVal) ) 

#define ISewLIView_GetLiveVideoRect(This,pLeft,pTop,pRight,pBottom)	\
    ( (This)->lpVtbl -> GetLiveVideoRect(This,pLeft,pTop,pRight,pBottom) ) 

#define ISewLIView_GetDocumentOrg(This,px,py)	\
    ( (This)->lpVtbl -> GetDocumentOrg(This,px,py) ) 

#define ISewLIView_MoveLiveVideo(This,x,y,nFlags)	\
    ( (This)->lpVtbl -> MoveLiveVideo(This,x,y,nFlags) ) 

#define ISewLIView_get_LiveVideoMoving(This,pVal)	\
    ( (This)->lpVtbl -> get_LiveVideoMoving(This,pVal) ) 

#define ISewLIView_put_LiveVideoMoving(This,newVal)	\
    ( (This)->lpVtbl -> put_LiveVideoMoving(This,newVal) ) 

#define ISewLIView_MoveLiveVideoTo(This,x,y,nFlags)	\
    ( (This)->lpVtbl -> MoveLiveVideoTo(This,x,y,nFlags) ) 

#define ISewLIView_get_AutoMoveLiveVideoMode(This,pVal)	\
    ( (This)->lpVtbl -> get_AutoMoveLiveVideoMode(This,pVal) ) 

#define ISewLIView_put_AutoMoveLiveVideoMode(This,newVal)	\
    ( (This)->lpVtbl -> put_AutoMoveLiveVideoMode(This,newVal) ) 

#define ISewLIView_get_ShowLiveVideoMode(This,pVal)	\
    ( (This)->lpVtbl -> get_ShowLiveVideoMode(This,pVal) ) 

#define ISewLIView_put_ShowLiveVideoMode(This,newVal)	\
    ( (This)->lpVtbl -> put_ShowLiveVideoMode(This,newVal) ) 

#define ISewLIView_get_BaseFragment(This,ppunkVal)	\
    ( (This)->lpVtbl -> get_BaseFragment(This,ppunkVal) ) 

#define ISewLIView_get_LastFragmentMode(This,pVal)	\
    ( (This)->lpVtbl -> get_LastFragmentMode(This,pVal) ) 

#define ISewLIView_put_LastFragmentMode(This,newVal)	\
    ( (This)->lpVtbl -> put_LastFragmentMode(This,newVal) ) 

#define ISewLIView_get_ShowFrame(This,pVal)	\
    ( (This)->lpVtbl -> get_ShowFrame(This,pVal) ) 

#define ISewLIView_put_ShowFrame(This,newVal)	\
    ( (This)->lpVtbl -> put_ShowFrame(This,newVal) ) 

#define ISewLIView_get_NextFragmentDirection(This,pVal)	\
    ( (This)->lpVtbl -> get_NextFragmentDirection(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISewLIView_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_SewLIView;

#ifdef __cplusplus

class DECLSPEC_UUID("01B332D7-7AEF-4cc6-A047-654BC1EC2790")
SewLIView;
#endif
#endif /* __SewLIViewLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


