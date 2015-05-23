

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sun Apr 26 01:43:02 2015
 */
/* Compiler settings for dataview.idl:
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


#ifndef __dataview_h_h__
#define __dataview_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IClassView_FWD_DEFINED__
#define __IClassView_FWD_DEFINED__
typedef interface IClassView IClassView;

#endif 	/* __IClassView_FWD_DEFINED__ */


#ifndef __IGridView_FWD_DEFINED__
#define __IGridView_FWD_DEFINED__
typedef interface IGridView IGridView;

#endif 	/* __IGridView_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_dataview_0000_0000 */
/* [local] */ 

typedef typedef /* [custom][public] */ __int3264 LONG_PTR;
;



extern RPC_IF_HANDLE __MIDL_itf_dataview_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_dataview_0000_0000_v0_0_s_ifspec;


#ifndef __DataViewTypeLib_LIBRARY_DEFINED__
#define __DataViewTypeLib_LIBRARY_DEFINED__

/* library DataViewTypeLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_DataViewTypeLib;

#ifndef __IClassView_INTERFACE_DEFINED__
#define __IClassView_INTERFACE_DEFINED__

/* interface IClassView */
/* [object][uuid] */ 


EXTERN_C const IID IID_IClassView;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D3A6B9E3-F6D3-49e6-B178-F57B5D536C39")
    IClassView : public IUnknown
    {
    public:
    };
    
    
#else 	/* C style interface */

    typedef struct IClassViewVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IClassView * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IClassView * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IClassView * This);
        
        END_INTERFACE
    } IClassViewVtbl;

    interface IClassView
    {
        CONST_VTBL struct IClassViewVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IClassView_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IClassView_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IClassView_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IClassView_INTERFACE_DEFINED__ */


#ifndef __IGridView_INTERFACE_DEFINED__
#define __IGridView_INTERFACE_DEFINED__

/* interface IGridView */
/* [object][uuid] */ 


EXTERN_C const IID IID_IGridView;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00EBE551-A884-4405-943D-B1A668408245")
    IGridView : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetObjectByRow( 
            long lrow,
            IUnknown **ppunkObject) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetRowHeight( 
            long lrow,
            long *lpheight) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetRowHeight( 
            long lrow,
            long lheight) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetColumnWidth( 
            long lcolumn,
            long *lpwidth) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetColumnWidth( 
            long lcolumn,
            long lwidth) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IGridViewVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IGridView * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IGridView * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IGridView * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetObjectByRow )( 
            IGridView * This,
            long lrow,
            IUnknown **ppunkObject);
        
        HRESULT ( STDMETHODCALLTYPE *GetRowHeight )( 
            IGridView * This,
            long lrow,
            long *lpheight);
        
        HRESULT ( STDMETHODCALLTYPE *SetRowHeight )( 
            IGridView * This,
            long lrow,
            long lheight);
        
        HRESULT ( STDMETHODCALLTYPE *GetColumnWidth )( 
            IGridView * This,
            long lcolumn,
            long *lpwidth);
        
        HRESULT ( STDMETHODCALLTYPE *SetColumnWidth )( 
            IGridView * This,
            long lcolumn,
            long lwidth);
        
        END_INTERFACE
    } IGridViewVtbl;

    interface IGridView
    {
        CONST_VTBL struct IGridViewVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGridView_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IGridView_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IGridView_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IGridView_GetObjectByRow(This,lrow,ppunkObject)	\
    ( (This)->lpVtbl -> GetObjectByRow(This,lrow,ppunkObject) ) 

#define IGridView_GetRowHeight(This,lrow,lpheight)	\
    ( (This)->lpVtbl -> GetRowHeight(This,lrow,lpheight) ) 

#define IGridView_SetRowHeight(This,lrow,lheight)	\
    ( (This)->lpVtbl -> SetRowHeight(This,lrow,lheight) ) 

#define IGridView_GetColumnWidth(This,lcolumn,lpwidth)	\
    ( (This)->lpVtbl -> GetColumnWidth(This,lcolumn,lpwidth) ) 

#define IGridView_SetColumnWidth(This,lcolumn,lwidth)	\
    ( (This)->lpVtbl -> SetColumnWidth(This,lcolumn,lwidth) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IGridView_INTERFACE_DEFINED__ */

#endif /* __DataViewTypeLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


