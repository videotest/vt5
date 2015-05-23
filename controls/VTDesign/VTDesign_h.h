

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Apr 04 21:04:47 2015
 */
/* Compiler settings for VTDesign.odl:
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


#ifndef __VTDesign_h_h__
#define __VTDesign_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef ___DVTDLine_FWD_DEFINED__
#define ___DVTDLine_FWD_DEFINED__
typedef interface _DVTDLine _DVTDLine;

#endif 	/* ___DVTDLine_FWD_DEFINED__ */


#ifndef ___DVTDLineEvents_FWD_DEFINED__
#define ___DVTDLineEvents_FWD_DEFINED__
typedef interface _DVTDLineEvents _DVTDLineEvents;

#endif 	/* ___DVTDLineEvents_FWD_DEFINED__ */


#ifndef __VTDLine_FWD_DEFINED__
#define __VTDLine_FWD_DEFINED__

#ifdef __cplusplus
typedef class VTDLine VTDLine;
#else
typedef struct VTDLine VTDLine;
#endif /* __cplusplus */

#endif 	/* __VTDLine_FWD_DEFINED__ */


#ifndef ___DVTDRectangle_FWD_DEFINED__
#define ___DVTDRectangle_FWD_DEFINED__
typedef interface _DVTDRectangle _DVTDRectangle;

#endif 	/* ___DVTDRectangle_FWD_DEFINED__ */


#ifndef ___DVTDRectangleEvents_FWD_DEFINED__
#define ___DVTDRectangleEvents_FWD_DEFINED__
typedef interface _DVTDRectangleEvents _DVTDRectangleEvents;

#endif 	/* ___DVTDRectangleEvents_FWD_DEFINED__ */


#ifndef __VTDRectangle_FWD_DEFINED__
#define __VTDRectangle_FWD_DEFINED__

#ifdef __cplusplus
typedef class VTDRectangle VTDRectangle;
#else
typedef struct VTDRectangle VTDRectangle;
#endif /* __cplusplus */

#endif 	/* __VTDRectangle_FWD_DEFINED__ */


#ifndef ___DVTDEllipse_FWD_DEFINED__
#define ___DVTDEllipse_FWD_DEFINED__
typedef interface _DVTDEllipse _DVTDEllipse;

#endif 	/* ___DVTDEllipse_FWD_DEFINED__ */


#ifndef ___DVTDEllipseEvents_FWD_DEFINED__
#define ___DVTDEllipseEvents_FWD_DEFINED__
typedef interface _DVTDEllipseEvents _DVTDEllipseEvents;

#endif 	/* ___DVTDEllipseEvents_FWD_DEFINED__ */


#ifndef __VTDEllipse_FWD_DEFINED__
#define __VTDEllipse_FWD_DEFINED__

#ifdef __cplusplus
typedef class VTDEllipse VTDEllipse;
#else
typedef struct VTDEllipse VTDEllipse;
#endif /* __cplusplus */

#endif 	/* __VTDEllipse_FWD_DEFINED__ */


#ifndef ___DVTDText_FWD_DEFINED__
#define ___DVTDText_FWD_DEFINED__
typedef interface _DVTDText _DVTDText;

#endif 	/* ___DVTDText_FWD_DEFINED__ */


#ifndef ___DVTDTextEvents_FWD_DEFINED__
#define ___DVTDTextEvents_FWD_DEFINED__
typedef interface _DVTDTextEvents _DVTDTextEvents;

#endif 	/* ___DVTDTextEvents_FWD_DEFINED__ */


#ifndef __VTDText_FWD_DEFINED__
#define __VTDText_FWD_DEFINED__

#ifdef __cplusplus
typedef class VTDText VTDText;
#else
typedef struct VTDText VTDText;
#endif /* __cplusplus */

#endif 	/* __VTDText_FWD_DEFINED__ */


#ifndef ___DVTDImage_FWD_DEFINED__
#define ___DVTDImage_FWD_DEFINED__
typedef interface _DVTDImage _DVTDImage;

#endif 	/* ___DVTDImage_FWD_DEFINED__ */


#ifndef ___DVTDImageEvents_FWD_DEFINED__
#define ___DVTDImageEvents_FWD_DEFINED__
typedef interface _DVTDImageEvents _DVTDImageEvents;

#endif 	/* ___DVTDImageEvents_FWD_DEFINED__ */


#ifndef __VTDImage_FWD_DEFINED__
#define __VTDImage_FWD_DEFINED__

#ifdef __cplusplus
typedef class VTDImage VTDImage;
#else
typedef struct VTDImage VTDImage;
#endif /* __cplusplus */

#endif 	/* __VTDImage_FWD_DEFINED__ */


#ifndef ___DVTD3DBar_FWD_DEFINED__
#define ___DVTD3DBar_FWD_DEFINED__
typedef interface _DVTD3DBar _DVTD3DBar;

#endif 	/* ___DVTD3DBar_FWD_DEFINED__ */


#ifndef ___DVTD3DBarEvents_FWD_DEFINED__
#define ___DVTD3DBarEvents_FWD_DEFINED__
typedef interface _DVTD3DBarEvents _DVTD3DBarEvents;

#endif 	/* ___DVTD3DBarEvents_FWD_DEFINED__ */


#ifndef __VTD3DBar_FWD_DEFINED__
#define __VTD3DBar_FWD_DEFINED__

#ifdef __cplusplus
typedef class VTD3DBar VTD3DBar;
#else
typedef struct VTD3DBar VTD3DBar;
#endif /* __cplusplus */

#endif 	/* __VTD3DBar_FWD_DEFINED__ */


#ifndef ___DVTDSystem_FWD_DEFINED__
#define ___DVTDSystem_FWD_DEFINED__
typedef interface _DVTDSystem _DVTDSystem;

#endif 	/* ___DVTDSystem_FWD_DEFINED__ */


#ifndef ___DVTDSystemEvents_FWD_DEFINED__
#define ___DVTDSystemEvents_FWD_DEFINED__
typedef interface _DVTDSystemEvents _DVTDSystemEvents;

#endif 	/* ___DVTDSystemEvents_FWD_DEFINED__ */


#ifndef __VTDSystem_FWD_DEFINED__
#define __VTDSystem_FWD_DEFINED__

#ifdef __cplusplus
typedef class VTDSystem VTDSystem;
#else
typedef struct VTDSystem VTDSystem;
#endif /* __cplusplus */

#endif 	/* __VTDSystem_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __VTDESIGNLib_LIBRARY_DEFINED__
#define __VTDESIGNLib_LIBRARY_DEFINED__

/* library VTDESIGNLib */
/* [control][helpstring][helpfile][version][uuid] */ 

#pragma once
#pragma region Desktop Family
#pragma endregion
typedef 
enum __MIDL___MIDL_itf_VTDesign_0000_0000_0001
    {
        lsSOLID	= 0,
        lsDASH	= 1,
        lsDOT	= 2,
        lsDASHDOT	= 3,
        lsDASHDOTDOT	= 4,
        lsDASHDOTDOTDOT	= 5,
        lsDASH_GAP	= 6,
        lsDOT_GAP	= 7,
        lsDASHDOT_GAP	= 8,
        lsDASHDOTDOT_GAP	= 9,
        lsDASHDOTDOTDOT_GAP	= 10
    } 	LineStyle;

typedef 
enum __MIDL___MIDL_itf_VTDesign_0000_0000_0002
    {
        asNone	= 0,
        asPureArrow	= 1,
        asOpenArrow	= 2,
        asStealthArrow	= 3,
        asDiamondArrow	= 4,
        asOvalArrow	= 5
    } 	ArrowStyle;

typedef 
enum __MIDL___MIDL_itf_VTDesign_0000_0000_0003
    {
        btEdgeNone	= 0,
        btEdgeRaised	= 1,
        btEdgeSunken	= 2,
        btEdgeEtched	= 3,
        btEdgeBump	= 4
    } 	BorderType;

typedef 
enum __MIDL___MIDL_itf_VTDesign_0000_0000_0004
    {
        ftNormal	= 0,
        ftPositive	= 1,
        ftNegative	= 2
    } 	FlipType;

typedef 
enum __MIDL___MIDL_itf_VTDesign_0000_0000_0005
    {
        ptLeft	= 0,
        ptRight	= 1,
        ptTop	= 2,
        ptBottom	= 3
    } 	PositionType;

typedef 
enum __MIDL___MIDL_itf_VTDesign_0000_0000_0006
    {
        atLeft	= 0,
        atCenter	= 1,
        atRight	= 2
    } 	AlignType;

typedef 
enum __MIDL___MIDL_itf_VTDesign_0000_0000_0007
    {
        ahLeft	= 0,
        ahCenter	= 1,
        ahRight	= 2
    } 	AlignHoriz;

typedef 
enum __MIDL___MIDL_itf_VTDesign_0000_0000_0008
    {
        avTop	= 0,
        avCenter	= 1,
        avBottom	= 2
    } 	AlignVert;

typedef 
enum __MIDL___MIDL_itf_VTDesign_0000_0000_0009
    {
        stDate	= 0,
        stDateTime	= 1,
        stPageNum	= 2
    } 	SystemType;


DEFINE_GUID(LIBID_VTDESIGNLib,0xBC127B08,0x026C,0x11D4,0x81,0x24,0x00,0x00,0xE8,0xDF,0x68,0xC3);

#ifndef ___DVTDLine_DISPINTERFACE_DEFINED__
#define ___DVTDLine_DISPINTERFACE_DEFINED__

/* dispinterface _DVTDLine */
/* [hidden][helpstring][uuid] */ 


DEFINE_GUID(DIID__DVTDLine,0xBC127B09,0x026C,0x11D4,0x81,0x24,0x00,0x00,0xE8,0xDF,0x68,0xC3);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("BC127B09-026C-11D4-8124-0000E8DF68C3")
    _DVTDLine : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTDLineVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTDLine * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTDLine * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTDLine * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTDLine * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTDLine * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTDLine * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTDLine * This,
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
    } _DVTDLineVtbl;

    interface _DVTDLine
    {
        CONST_VTBL struct _DVTDLineVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTDLine_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTDLine_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTDLine_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTDLine_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTDLine_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTDLine_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTDLine_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTDLine_DISPINTERFACE_DEFINED__ */


#ifndef ___DVTDLineEvents_DISPINTERFACE_DEFINED__
#define ___DVTDLineEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DVTDLineEvents */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DVTDLineEvents,0xBC127B0A,0x026C,0x11D4,0x81,0x24,0x00,0x00,0xE8,0xDF,0x68,0xC3);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("BC127B0A-026C-11D4-8124-0000E8DF68C3")
    _DVTDLineEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTDLineEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTDLineEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTDLineEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTDLineEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTDLineEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTDLineEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTDLineEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTDLineEvents * This,
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
    } _DVTDLineEventsVtbl;

    interface _DVTDLineEvents
    {
        CONST_VTBL struct _DVTDLineEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTDLineEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTDLineEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTDLineEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTDLineEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTDLineEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTDLineEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTDLineEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTDLineEvents_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_VTDLine,0xBC127B0B,0x026C,0x11D4,0x81,0x24,0x00,0x00,0xE8,0xDF,0x68,0xC3);

#ifdef __cplusplus

class DECLSPEC_UUID("BC127B0B-026C-11D4-8124-0000E8DF68C3")
VTDLine;
#endif

#ifndef ___DVTDRectangle_DISPINTERFACE_DEFINED__
#define ___DVTDRectangle_DISPINTERFACE_DEFINED__

/* dispinterface _DVTDRectangle */
/* [hidden][helpstring][uuid] */ 


DEFINE_GUID(DIID__DVTDRectangle,0xBC127B0D,0x026C,0x11D4,0x81,0x24,0x00,0x00,0xE8,0xDF,0x68,0xC3);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("BC127B0D-026C-11D4-8124-0000E8DF68C3")
    _DVTDRectangle : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTDRectangleVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTDRectangle * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTDRectangle * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTDRectangle * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTDRectangle * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTDRectangle * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTDRectangle * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTDRectangle * This,
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
    } _DVTDRectangleVtbl;

    interface _DVTDRectangle
    {
        CONST_VTBL struct _DVTDRectangleVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTDRectangle_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTDRectangle_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTDRectangle_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTDRectangle_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTDRectangle_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTDRectangle_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTDRectangle_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTDRectangle_DISPINTERFACE_DEFINED__ */


#ifndef ___DVTDRectangleEvents_DISPINTERFACE_DEFINED__
#define ___DVTDRectangleEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DVTDRectangleEvents */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DVTDRectangleEvents,0xBC127B0E,0x026C,0x11D4,0x81,0x24,0x00,0x00,0xE8,0xDF,0x68,0xC3);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("BC127B0E-026C-11D4-8124-0000E8DF68C3")
    _DVTDRectangleEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTDRectangleEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTDRectangleEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTDRectangleEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTDRectangleEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTDRectangleEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTDRectangleEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTDRectangleEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTDRectangleEvents * This,
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
    } _DVTDRectangleEventsVtbl;

    interface _DVTDRectangleEvents
    {
        CONST_VTBL struct _DVTDRectangleEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTDRectangleEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTDRectangleEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTDRectangleEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTDRectangleEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTDRectangleEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTDRectangleEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTDRectangleEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTDRectangleEvents_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_VTDRectangle,0xBC127B0F,0x026C,0x11D4,0x81,0x24,0x00,0x00,0xE8,0xDF,0x68,0xC3);

#ifdef __cplusplus

class DECLSPEC_UUID("BC127B0F-026C-11D4-8124-0000E8DF68C3")
VTDRectangle;
#endif

#ifndef ___DVTDEllipse_DISPINTERFACE_DEFINED__
#define ___DVTDEllipse_DISPINTERFACE_DEFINED__

/* dispinterface _DVTDEllipse */
/* [hidden][helpstring][uuid] */ 


DEFINE_GUID(DIID__DVTDEllipse,0xBC127B11,0x026C,0x11D4,0x81,0x24,0x00,0x00,0xE8,0xDF,0x68,0xC3);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("BC127B11-026C-11D4-8124-0000E8DF68C3")
    _DVTDEllipse : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTDEllipseVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTDEllipse * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTDEllipse * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTDEllipse * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTDEllipse * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTDEllipse * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTDEllipse * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTDEllipse * This,
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
    } _DVTDEllipseVtbl;

    interface _DVTDEllipse
    {
        CONST_VTBL struct _DVTDEllipseVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTDEllipse_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTDEllipse_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTDEllipse_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTDEllipse_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTDEllipse_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTDEllipse_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTDEllipse_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTDEllipse_DISPINTERFACE_DEFINED__ */


#ifndef ___DVTDEllipseEvents_DISPINTERFACE_DEFINED__
#define ___DVTDEllipseEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DVTDEllipseEvents */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DVTDEllipseEvents,0xBC127B12,0x026C,0x11D4,0x81,0x24,0x00,0x00,0xE8,0xDF,0x68,0xC3);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("BC127B12-026C-11D4-8124-0000E8DF68C3")
    _DVTDEllipseEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTDEllipseEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTDEllipseEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTDEllipseEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTDEllipseEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTDEllipseEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTDEllipseEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTDEllipseEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTDEllipseEvents * This,
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
    } _DVTDEllipseEventsVtbl;

    interface _DVTDEllipseEvents
    {
        CONST_VTBL struct _DVTDEllipseEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTDEllipseEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTDEllipseEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTDEllipseEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTDEllipseEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTDEllipseEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTDEllipseEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTDEllipseEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTDEllipseEvents_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_VTDEllipse,0xBC127B13,0x026C,0x11D4,0x81,0x24,0x00,0x00,0xE8,0xDF,0x68,0xC3);

#ifdef __cplusplus

class DECLSPEC_UUID("BC127B13-026C-11D4-8124-0000E8DF68C3")
VTDEllipse;
#endif

#ifndef ___DVTDText_DISPINTERFACE_DEFINED__
#define ___DVTDText_DISPINTERFACE_DEFINED__

/* dispinterface _DVTDText */
/* [hidden][helpstring][uuid] */ 


DEFINE_GUID(DIID__DVTDText,0xBC127B15,0x026C,0x11D4,0x81,0x24,0x00,0x00,0xE8,0xDF,0x68,0xC3);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("BC127B15-026C-11D4-8124-0000E8DF68C3")
    _DVTDText : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTDTextVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTDText * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTDText * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTDText * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTDText * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTDText * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTDText * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTDText * This,
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
    } _DVTDTextVtbl;

    interface _DVTDText
    {
        CONST_VTBL struct _DVTDTextVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTDText_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTDText_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTDText_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTDText_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTDText_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTDText_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTDText_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTDText_DISPINTERFACE_DEFINED__ */


#ifndef ___DVTDTextEvents_DISPINTERFACE_DEFINED__
#define ___DVTDTextEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DVTDTextEvents */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DVTDTextEvents,0xBC127B16,0x026C,0x11D4,0x81,0x24,0x00,0x00,0xE8,0xDF,0x68,0xC3);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("BC127B16-026C-11D4-8124-0000E8DF68C3")
    _DVTDTextEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTDTextEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTDTextEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTDTextEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTDTextEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTDTextEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTDTextEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTDTextEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTDTextEvents * This,
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
    } _DVTDTextEventsVtbl;

    interface _DVTDTextEvents
    {
        CONST_VTBL struct _DVTDTextEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTDTextEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTDTextEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTDTextEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTDTextEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTDTextEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTDTextEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTDTextEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTDTextEvents_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_VTDText,0xBC127B17,0x026C,0x11D4,0x81,0x24,0x00,0x00,0xE8,0xDF,0x68,0xC3);

#ifdef __cplusplus

class DECLSPEC_UUID("BC127B17-026C-11D4-8124-0000E8DF68C3")
VTDText;
#endif

#ifndef ___DVTDImage_DISPINTERFACE_DEFINED__
#define ___DVTDImage_DISPINTERFACE_DEFINED__

/* dispinterface _DVTDImage */
/* [hidden][helpstring][uuid] */ 


DEFINE_GUID(DIID__DVTDImage,0xBC127B19,0x026C,0x11D4,0x81,0x24,0x00,0x00,0xE8,0xDF,0x68,0xC3);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("BC127B19-026C-11D4-8124-0000E8DF68C3")
    _DVTDImage : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTDImageVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTDImage * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTDImage * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTDImage * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTDImage * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTDImage * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTDImage * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTDImage * This,
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
    } _DVTDImageVtbl;

    interface _DVTDImage
    {
        CONST_VTBL struct _DVTDImageVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTDImage_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTDImage_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTDImage_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTDImage_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTDImage_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTDImage_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTDImage_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTDImage_DISPINTERFACE_DEFINED__ */


#ifndef ___DVTDImageEvents_DISPINTERFACE_DEFINED__
#define ___DVTDImageEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DVTDImageEvents */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DVTDImageEvents,0xBC127B1A,0x026C,0x11D4,0x81,0x24,0x00,0x00,0xE8,0xDF,0x68,0xC3);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("BC127B1A-026C-11D4-8124-0000E8DF68C3")
    _DVTDImageEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTDImageEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTDImageEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTDImageEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTDImageEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTDImageEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTDImageEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTDImageEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTDImageEvents * This,
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
    } _DVTDImageEventsVtbl;

    interface _DVTDImageEvents
    {
        CONST_VTBL struct _DVTDImageEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTDImageEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTDImageEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTDImageEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTDImageEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTDImageEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTDImageEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTDImageEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTDImageEvents_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_VTDImage,0xBC127B1B,0x026C,0x11D4,0x81,0x24,0x00,0x00,0xE8,0xDF,0x68,0xC3);

#ifdef __cplusplus

class DECLSPEC_UUID("BC127B1B-026C-11D4-8124-0000E8DF68C3")
VTDImage;
#endif

#ifndef ___DVTD3DBar_DISPINTERFACE_DEFINED__
#define ___DVTD3DBar_DISPINTERFACE_DEFINED__

/* dispinterface _DVTD3DBar */
/* [hidden][helpstring][uuid] */ 


DEFINE_GUID(DIID__DVTD3DBar,0xBC127B1D,0x026C,0x11D4,0x81,0x24,0x00,0x00,0xE8,0xDF,0x68,0xC3);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("BC127B1D-026C-11D4-8124-0000E8DF68C3")
    _DVTD3DBar : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTD3DBarVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTD3DBar * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTD3DBar * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTD3DBar * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTD3DBar * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTD3DBar * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTD3DBar * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTD3DBar * This,
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
    } _DVTD3DBarVtbl;

    interface _DVTD3DBar
    {
        CONST_VTBL struct _DVTD3DBarVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTD3DBar_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTD3DBar_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTD3DBar_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTD3DBar_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTD3DBar_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTD3DBar_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTD3DBar_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTD3DBar_DISPINTERFACE_DEFINED__ */


#ifndef ___DVTD3DBarEvents_DISPINTERFACE_DEFINED__
#define ___DVTD3DBarEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DVTD3DBarEvents */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DVTD3DBarEvents,0xBC127B1E,0x026C,0x11D4,0x81,0x24,0x00,0x00,0xE8,0xDF,0x68,0xC3);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("BC127B1E-026C-11D4-8124-0000E8DF68C3")
    _DVTD3DBarEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTD3DBarEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTD3DBarEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTD3DBarEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTD3DBarEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTD3DBarEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTD3DBarEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTD3DBarEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTD3DBarEvents * This,
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
    } _DVTD3DBarEventsVtbl;

    interface _DVTD3DBarEvents
    {
        CONST_VTBL struct _DVTD3DBarEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTD3DBarEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTD3DBarEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTD3DBarEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTD3DBarEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTD3DBarEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTD3DBarEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTD3DBarEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTD3DBarEvents_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_VTD3DBar,0xBC127B1F,0x026C,0x11D4,0x81,0x24,0x00,0x00,0xE8,0xDF,0x68,0xC3);

#ifdef __cplusplus

class DECLSPEC_UUID("BC127B1F-026C-11D4-8124-0000E8DF68C3")
VTD3DBar;
#endif

#ifndef ___DVTDSystem_DISPINTERFACE_DEFINED__
#define ___DVTDSystem_DISPINTERFACE_DEFINED__

/* dispinterface _DVTDSystem */
/* [hidden][helpstring][uuid] */ 


DEFINE_GUID(DIID__DVTDSystem,0xBC127B21,0x026C,0x11D4,0x81,0x24,0x00,0x00,0xE8,0xDF,0x68,0xC3);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("BC127B21-026C-11D4-8124-0000E8DF68C3")
    _DVTDSystem : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTDSystemVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTDSystem * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTDSystem * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTDSystem * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTDSystem * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTDSystem * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTDSystem * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTDSystem * This,
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
    } _DVTDSystemVtbl;

    interface _DVTDSystem
    {
        CONST_VTBL struct _DVTDSystemVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTDSystem_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTDSystem_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTDSystem_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTDSystem_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTDSystem_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTDSystem_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTDSystem_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTDSystem_DISPINTERFACE_DEFINED__ */


#ifndef ___DVTDSystemEvents_DISPINTERFACE_DEFINED__
#define ___DVTDSystemEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DVTDSystemEvents */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DVTDSystemEvents,0xBC127B22,0x026C,0x11D4,0x81,0x24,0x00,0x00,0xE8,0xDF,0x68,0xC3);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("BC127B22-026C-11D4-8124-0000E8DF68C3")
    _DVTDSystemEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTDSystemEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTDSystemEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTDSystemEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTDSystemEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTDSystemEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTDSystemEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTDSystemEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTDSystemEvents * This,
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
    } _DVTDSystemEventsVtbl;

    interface _DVTDSystemEvents
    {
        CONST_VTBL struct _DVTDSystemEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTDSystemEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTDSystemEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTDSystemEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTDSystemEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTDSystemEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTDSystemEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTDSystemEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTDSystemEvents_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_VTDSystem,0xBC127B23,0x026C,0x11D4,0x81,0x24,0x00,0x00,0xE8,0xDF,0x68,0xC3);

#ifdef __cplusplus

class DECLSPEC_UUID("BC127B23-026C-11D4-8124-0000E8DF68C3")
VTDSystem;
#endif
#endif /* __VTDESIGNLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


