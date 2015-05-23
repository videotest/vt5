

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Apr 04 21:04:33 2015
 */
/* Compiler settings for Palette.odl:
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


#ifndef __Palette_h_h__
#define __Palette_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef ___DPalette_FWD_DEFINED__
#define ___DPalette_FWD_DEFINED__
typedef interface _DPalette _DPalette;

#endif 	/* ___DPalette_FWD_DEFINED__ */


#ifndef ___DPaletteEvents_FWD_DEFINED__
#define ___DPaletteEvents_FWD_DEFINED__
typedef interface _DPaletteEvents _DPaletteEvents;

#endif 	/* ___DPaletteEvents_FWD_DEFINED__ */


#ifndef __Palette_FWD_DEFINED__
#define __Palette_FWD_DEFINED__

#ifdef __cplusplus
typedef class Palette Palette;
#else
typedef struct Palette Palette;
#endif /* __cplusplus */

#endif 	/* __Palette_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __PALETTELib_LIBRARY_DEFINED__
#define __PALETTELib_LIBRARY_DEFINED__

/* library PALETTELib */
/* [control][helpstring][helpfile][version][uuid] */ 

#pragma once
#pragma region Desktop Family
#pragma endregion

DEFINE_GUID(LIBID_PALETTELib,0x57C4CB45,0x9E87,0x11D3,0xA5,0x37,0x00,0x00,0xB4,0x93,0xA1,0x87);

#ifndef ___DPalette_DISPINTERFACE_DEFINED__
#define ___DPalette_DISPINTERFACE_DEFINED__

/* dispinterface _DPalette */
/* [hidden][helpstring][uuid] */ 


DEFINE_GUID(DIID__DPalette,0x57C4CB46,0x9E87,0x11D3,0xA5,0x37,0x00,0x00,0xB4,0x93,0xA1,0x87);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("57C4CB46-9E87-11D3-A537-0000B493A187")
    _DPalette : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DPaletteVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DPalette * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DPalette * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DPalette * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DPalette * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DPalette * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DPalette * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DPalette * This,
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
    } _DPaletteVtbl;

    interface _DPalette
    {
        CONST_VTBL struct _DPaletteVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DPalette_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DPalette_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DPalette_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DPalette_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DPalette_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DPalette_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DPalette_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DPalette_DISPINTERFACE_DEFINED__ */


#ifndef ___DPaletteEvents_DISPINTERFACE_DEFINED__
#define ___DPaletteEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DPaletteEvents */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DPaletteEvents,0x57C4CB47,0x9E87,0x11D3,0xA5,0x37,0x00,0x00,0xB4,0x93,0xA1,0x87);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("57C4CB47-9E87-11D3-A537-0000B493A187")
    _DPaletteEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DPaletteEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DPaletteEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DPaletteEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DPaletteEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DPaletteEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DPaletteEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DPaletteEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DPaletteEvents * This,
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
    } _DPaletteEventsVtbl;

    interface _DPaletteEvents
    {
        CONST_VTBL struct _DPaletteEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DPaletteEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DPaletteEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DPaletteEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DPaletteEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DPaletteEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DPaletteEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DPaletteEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DPaletteEvents_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_Palette,0x57C4CB48,0x9E87,0x11D3,0xA5,0x37,0x00,0x00,0xB4,0x93,0xA1,0x87);

#ifdef __cplusplus

class DECLSPEC_UUID("57C4CB48-9E87-11D3-A537-0000B493A187")
Palette;
#endif
#endif /* __PALETTELib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


