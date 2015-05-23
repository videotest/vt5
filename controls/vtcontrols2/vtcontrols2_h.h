

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Apr 04 21:05:07 2015
 */
/* Compiler settings for vtcontrols2.odl:
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


#ifndef __vtcontrols2_h_h__
#define __vtcontrols2_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef ___DVTComboBox_FWD_DEFINED__
#define ___DVTComboBox_FWD_DEFINED__
typedef interface _DVTComboBox _DVTComboBox;

#endif 	/* ___DVTComboBox_FWD_DEFINED__ */


#ifndef ___DVTComboBoxEvents_FWD_DEFINED__
#define ___DVTComboBoxEvents_FWD_DEFINED__
typedef interface _DVTComboBoxEvents _DVTComboBoxEvents;

#endif 	/* ___DVTComboBoxEvents_FWD_DEFINED__ */


#ifndef __VTComboBox_FWD_DEFINED__
#define __VTComboBox_FWD_DEFINED__

#ifdef __cplusplus
typedef class VTComboBox VTComboBox;
#else
typedef struct VTComboBox VTComboBox;
#endif /* __cplusplus */

#endif 	/* __VTComboBox_FWD_DEFINED__ */


#ifndef __VTComboBox2_FWD_DEFINED__
#define __VTComboBox2_FWD_DEFINED__

#ifdef __cplusplus
typedef class VTComboBox2 VTComboBox2;
#else
typedef struct VTComboBox2 VTComboBox2;
#endif /* __cplusplus */

#endif 	/* __VTComboBox2_FWD_DEFINED__ */


#ifndef ___DVTColorPicker_FWD_DEFINED__
#define ___DVTColorPicker_FWD_DEFINED__
typedef interface _DVTColorPicker _DVTColorPicker;

#endif 	/* ___DVTColorPicker_FWD_DEFINED__ */


#ifndef ___DVTColorPickerEvents_FWD_DEFINED__
#define ___DVTColorPickerEvents_FWD_DEFINED__
typedef interface _DVTColorPickerEvents _DVTColorPickerEvents;

#endif 	/* ___DVTColorPickerEvents_FWD_DEFINED__ */


#ifndef __VTColorPicker_FWD_DEFINED__
#define __VTColorPicker_FWD_DEFINED__

#ifdef __cplusplus
typedef class VTColorPicker VTColorPicker;
#else
typedef struct VTColorPicker VTColorPicker;
#endif /* __cplusplus */

#endif 	/* __VTColorPicker_FWD_DEFINED__ */


#ifndef ___DVTWidth_FWD_DEFINED__
#define ___DVTWidth_FWD_DEFINED__
typedef interface _DVTWidth _DVTWidth;

#endif 	/* ___DVTWidth_FWD_DEFINED__ */


#ifndef ___DVTWidthEvents_FWD_DEFINED__
#define ___DVTWidthEvents_FWD_DEFINED__
typedef interface _DVTWidthEvents _DVTWidthEvents;

#endif 	/* ___DVTWidthEvents_FWD_DEFINED__ */


#ifndef __VTWidth_FWD_DEFINED__
#define __VTWidth_FWD_DEFINED__

#ifdef __cplusplus
typedef class VTWidth VTWidth;
#else
typedef struct VTWidth VTWidth;
#endif /* __cplusplus */

#endif 	/* __VTWidth_FWD_DEFINED__ */


#ifndef ___DColorRange_FWD_DEFINED__
#define ___DColorRange_FWD_DEFINED__
typedef interface _DColorRange _DColorRange;

#endif 	/* ___DColorRange_FWD_DEFINED__ */


#ifndef ___DColorRangeEvents_FWD_DEFINED__
#define ___DColorRangeEvents_FWD_DEFINED__
typedef interface _DColorRangeEvents _DColorRangeEvents;

#endif 	/* ___DColorRangeEvents_FWD_DEFINED__ */


#ifndef __ColorRange_FWD_DEFINED__
#define __ColorRange_FWD_DEFINED__

#ifdef __cplusplus
typedef class ColorRange ColorRange;
#else
typedef struct ColorRange ColorRange;
#endif /* __cplusplus */

#endif 	/* __ColorRange_FWD_DEFINED__ */


#ifndef ___DVTRadioBtn_FWD_DEFINED__
#define ___DVTRadioBtn_FWD_DEFINED__
typedef interface _DVTRadioBtn _DVTRadioBtn;

#endif 	/* ___DVTRadioBtn_FWD_DEFINED__ */


#ifndef ___DVTRadioBtnEvents_FWD_DEFINED__
#define ___DVTRadioBtnEvents_FWD_DEFINED__
typedef interface _DVTRadioBtnEvents _DVTRadioBtnEvents;

#endif 	/* ___DVTRadioBtnEvents_FWD_DEFINED__ */


#ifndef __VTRadioBtn_FWD_DEFINED__
#define __VTRadioBtn_FWD_DEFINED__

#ifdef __cplusplus
typedef class VTRadioBtn VTRadioBtn;
#else
typedef struct VTRadioBtn VTRadioBtn;
#endif /* __cplusplus */

#endif 	/* __VTRadioBtn_FWD_DEFINED__ */


#ifndef ___DVTProgressBar_FWD_DEFINED__
#define ___DVTProgressBar_FWD_DEFINED__
typedef interface _DVTProgressBar _DVTProgressBar;

#endif 	/* ___DVTProgressBar_FWD_DEFINED__ */


#ifndef ___DVTProgressBarEvents_FWD_DEFINED__
#define ___DVTProgressBarEvents_FWD_DEFINED__
typedef interface _DVTProgressBarEvents _DVTProgressBarEvents;

#endif 	/* ___DVTProgressBarEvents_FWD_DEFINED__ */


#ifndef __VTProgressBar_FWD_DEFINED__
#define __VTProgressBar_FWD_DEFINED__

#ifdef __cplusplus
typedef class VTProgressBar VTProgressBar;
#else
typedef struct VTProgressBar VTProgressBar;
#endif /* __cplusplus */

#endif 	/* __VTProgressBar_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __VTCONTROLS2Lib_LIBRARY_DEFINED__
#define __VTCONTROLS2Lib_LIBRARY_DEFINED__

/* library VTCONTROLS2Lib */
/* [control][helpstring][helpfile][version][uuid] */ 

#pragma once
#pragma region Desktop Family
#pragma endregion

DEFINE_GUID(LIBID_VTCONTROLS2Lib,0xD4975FBA,0x118A,0x4368,0xB9,0x41,0x8A,0x44,0x1A,0x0C,0xCF,0xA4);

#ifndef ___DVTComboBox_DISPINTERFACE_DEFINED__
#define ___DVTComboBox_DISPINTERFACE_DEFINED__

/* dispinterface _DVTComboBox */
/* [hidden][helpstring][uuid] */ 


DEFINE_GUID(DIID__DVTComboBox,0xF0A32695,0x7EDB,0x45A1,0x97,0xF6,0x4E,0xF7,0xC2,0xA8,0xA9,0x88);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("F0A32695-7EDB-45A1-97F6-4EF7C2A8A988")
    _DVTComboBox : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTComboBoxVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTComboBox * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTComboBox * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTComboBox * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTComboBox * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTComboBox * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTComboBox * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTComboBox * This,
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
    } _DVTComboBoxVtbl;

    interface _DVTComboBox
    {
        CONST_VTBL struct _DVTComboBoxVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTComboBox_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTComboBox_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTComboBox_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTComboBox_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTComboBox_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTComboBox_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTComboBox_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTComboBox_DISPINTERFACE_DEFINED__ */


#ifndef ___DVTComboBoxEvents_DISPINTERFACE_DEFINED__
#define ___DVTComboBoxEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DVTComboBoxEvents */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DVTComboBoxEvents,0x5CA42129,0x892D,0x4041,0x8A,0x82,0x0D,0xDA,0xA3,0x14,0x49,0xE6);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("5CA42129-892D-4041-8A82-0DDAA31449E6")
    _DVTComboBoxEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTComboBoxEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTComboBoxEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTComboBoxEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTComboBoxEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTComboBoxEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTComboBoxEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTComboBoxEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTComboBoxEvents * This,
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
    } _DVTComboBoxEventsVtbl;

    interface _DVTComboBoxEvents
    {
        CONST_VTBL struct _DVTComboBoxEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTComboBoxEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTComboBoxEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTComboBoxEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTComboBoxEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTComboBoxEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTComboBoxEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTComboBoxEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTComboBoxEvents_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_VTComboBox,0xB4B4C08F,0x8D9A,0x4655,0xA0,0xF1,0xD5,0xB0,0x41,0x9D,0x1E,0xD7);

#ifdef __cplusplus

class DECLSPEC_UUID("B4B4C08F-8D9A-4655-A0F1-D5B0419D1ED7")
VTComboBox;
#endif

DEFINE_GUID(CLSID_VTComboBox2,0x824C79E5,0x6DDF,0x427a,0x90,0xA9,0x5F,0xC4,0xC7,0x85,0x9C,0x91);

#ifdef __cplusplus

class DECLSPEC_UUID("824C79E5-6DDF-427a-90A9-5FC4C7859C91")
VTComboBox2;
#endif

#ifndef ___DVTColorPicker_DISPINTERFACE_DEFINED__
#define ___DVTColorPicker_DISPINTERFACE_DEFINED__

/* dispinterface _DVTColorPicker */
/* [hidden][helpstring][uuid] */ 


DEFINE_GUID(DIID__DVTColorPicker,0xF6816F65,0xEA89,0x471C,0x99,0x39,0x31,0x64,0xB7,0xD3,0x00,0x41);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("F6816F65-EA89-471C-9939-3164B7D30041")
    _DVTColorPicker : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTColorPickerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTColorPicker * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTColorPicker * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTColorPicker * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTColorPicker * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTColorPicker * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTColorPicker * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTColorPicker * This,
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
    } _DVTColorPickerVtbl;

    interface _DVTColorPicker
    {
        CONST_VTBL struct _DVTColorPickerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTColorPicker_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTColorPicker_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTColorPicker_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTColorPicker_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTColorPicker_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTColorPicker_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTColorPicker_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTColorPicker_DISPINTERFACE_DEFINED__ */


#ifndef ___DVTColorPickerEvents_DISPINTERFACE_DEFINED__
#define ___DVTColorPickerEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DVTColorPickerEvents */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DVTColorPickerEvents,0xC187CFDD,0x9AAE,0x4943,0x8F,0xE1,0x68,0x72,0xCA,0xA1,0xC4,0x56);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("C187CFDD-9AAE-4943-8FE1-6872CAA1C456")
    _DVTColorPickerEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTColorPickerEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTColorPickerEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTColorPickerEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTColorPickerEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTColorPickerEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTColorPickerEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTColorPickerEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTColorPickerEvents * This,
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
    } _DVTColorPickerEventsVtbl;

    interface _DVTColorPickerEvents
    {
        CONST_VTBL struct _DVTColorPickerEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTColorPickerEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTColorPickerEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTColorPickerEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTColorPickerEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTColorPickerEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTColorPickerEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTColorPickerEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTColorPickerEvents_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_VTColorPicker,0xBC383BC9,0x6FD1,0x4FF7,0x82,0xAD,0xB4,0x05,0x34,0x1D,0x5E,0x60);

#ifdef __cplusplus

class DECLSPEC_UUID("BC383BC9-6FD1-4FF7-82AD-B405341D5E60")
VTColorPicker;
#endif

#ifndef ___DVTWidth_DISPINTERFACE_DEFINED__
#define ___DVTWidth_DISPINTERFACE_DEFINED__

/* dispinterface _DVTWidth */
/* [hidden][helpstring][uuid] */ 


DEFINE_GUID(DIID__DVTWidth,0x92B9F891,0xE68C,0x4C52,0xA7,0x63,0x5C,0x71,0xCD,0xE6,0xE1,0xC7);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("92B9F891-E68C-4C52-A763-5C71CDE6E1C7")
    _DVTWidth : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTWidthVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTWidth * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTWidth * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTWidth * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTWidth * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTWidth * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTWidth * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTWidth * This,
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
    } _DVTWidthVtbl;

    interface _DVTWidth
    {
        CONST_VTBL struct _DVTWidthVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTWidth_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTWidth_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTWidth_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTWidth_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTWidth_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTWidth_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTWidth_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTWidth_DISPINTERFACE_DEFINED__ */


#ifndef ___DVTWidthEvents_DISPINTERFACE_DEFINED__
#define ___DVTWidthEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DVTWidthEvents */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DVTWidthEvents,0xEEB7070C,0x932F,0x41F1,0x90,0x8F,0x93,0xDC,0x32,0x36,0x27,0x96);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("EEB7070C-932F-41F1-908F-93DC32362796")
    _DVTWidthEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTWidthEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTWidthEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTWidthEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTWidthEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTWidthEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTWidthEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTWidthEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTWidthEvents * This,
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
    } _DVTWidthEventsVtbl;

    interface _DVTWidthEvents
    {
        CONST_VTBL struct _DVTWidthEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTWidthEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTWidthEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTWidthEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTWidthEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTWidthEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTWidthEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTWidthEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTWidthEvents_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_VTWidth,0x5DF91A6E,0x34A7,0x4CE0,0xA2,0x31,0x8E,0x0E,0x7E,0xF3,0x40,0xE5);

#ifdef __cplusplus

class DECLSPEC_UUID("5DF91A6E-34A7-4CE0-A231-8E0E7EF340E5")
VTWidth;
#endif

#ifndef ___DColorRange_DISPINTERFACE_DEFINED__
#define ___DColorRange_DISPINTERFACE_DEFINED__

/* dispinterface _DColorRange */
/* [hidden][helpstring][uuid] */ 


DEFINE_GUID(DIID__DColorRange,0xC21B81BB,0x7E16,0x4630,0xA7,0x13,0x1A,0x04,0x4A,0xB9,0x0E,0x58);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("C21B81BB-7E16-4630-A713-1A044AB90E58")
    _DColorRange : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DColorRangeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DColorRange * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DColorRange * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DColorRange * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DColorRange * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DColorRange * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DColorRange * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DColorRange * This,
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
    } _DColorRangeVtbl;

    interface _DColorRange
    {
        CONST_VTBL struct _DColorRangeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DColorRange_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DColorRange_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DColorRange_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DColorRange_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DColorRange_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DColorRange_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DColorRange_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DColorRange_DISPINTERFACE_DEFINED__ */


#ifndef ___DColorRangeEvents_DISPINTERFACE_DEFINED__
#define ___DColorRangeEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DColorRangeEvents */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DColorRangeEvents,0x5264AD94,0xC1D2,0x4AFE,0x93,0x59,0xDB,0xE4,0x7F,0x00,0xF9,0x2F);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("5264AD94-C1D2-4AFE-9359-DBE47F00F92F")
    _DColorRangeEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DColorRangeEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DColorRangeEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DColorRangeEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DColorRangeEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DColorRangeEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DColorRangeEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DColorRangeEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DColorRangeEvents * This,
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
    } _DColorRangeEventsVtbl;

    interface _DColorRangeEvents
    {
        CONST_VTBL struct _DColorRangeEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DColorRangeEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DColorRangeEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DColorRangeEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DColorRangeEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DColorRangeEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DColorRangeEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DColorRangeEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DColorRangeEvents_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_ColorRange,0xA02750E3,0xB2CC,0x49DB,0xA8,0x4B,0x2C,0x9C,0x96,0x08,0x1C,0x8F);

#ifdef __cplusplus

class DECLSPEC_UUID("A02750E3-B2CC-49DB-A84B-2C9C96081C8F")
ColorRange;
#endif

#ifndef ___DVTRadioBtn_DISPINTERFACE_DEFINED__
#define ___DVTRadioBtn_DISPINTERFACE_DEFINED__

/* dispinterface _DVTRadioBtn */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DVTRadioBtn,0x5A81931C,0xE10B,0x4143,0xA9,0x60,0xD2,0xB0,0x66,0xF4,0xF5,0xBE);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("5A81931C-E10B-4143-A960-D2B066F4F5BE")
    _DVTRadioBtn : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTRadioBtnVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTRadioBtn * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTRadioBtn * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTRadioBtn * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTRadioBtn * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTRadioBtn * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTRadioBtn * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTRadioBtn * This,
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
    } _DVTRadioBtnVtbl;

    interface _DVTRadioBtn
    {
        CONST_VTBL struct _DVTRadioBtnVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTRadioBtn_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTRadioBtn_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTRadioBtn_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTRadioBtn_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTRadioBtn_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTRadioBtn_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTRadioBtn_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTRadioBtn_DISPINTERFACE_DEFINED__ */


#ifndef ___DVTRadioBtnEvents_DISPINTERFACE_DEFINED__
#define ___DVTRadioBtnEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DVTRadioBtnEvents */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DVTRadioBtnEvents,0xDE1D575C,0x14DD,0x4734,0xBA,0xDE,0xB8,0x8B,0x12,0xA1,0x5A,0xC8);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("DE1D575C-14DD-4734-BADE-B88B12A15AC8")
    _DVTRadioBtnEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTRadioBtnEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTRadioBtnEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTRadioBtnEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTRadioBtnEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTRadioBtnEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTRadioBtnEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTRadioBtnEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTRadioBtnEvents * This,
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
    } _DVTRadioBtnEventsVtbl;

    interface _DVTRadioBtnEvents
    {
        CONST_VTBL struct _DVTRadioBtnEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTRadioBtnEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTRadioBtnEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTRadioBtnEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTRadioBtnEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTRadioBtnEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTRadioBtnEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTRadioBtnEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTRadioBtnEvents_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_VTRadioBtn,0x565483D1,0x812A,0x43BB,0xA2,0x28,0x86,0x65,0x1F,0x54,0x97,0xBC);

#ifdef __cplusplus

class DECLSPEC_UUID("565483D1-812A-43BB-A228-86651F5497BC")
VTRadioBtn;
#endif

#ifndef ___DVTProgressBar_DISPINTERFACE_DEFINED__
#define ___DVTProgressBar_DISPINTERFACE_DEFINED__

/* dispinterface _DVTProgressBar */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DVTProgressBar,0xD0DB664C,0x3B19,0x404B,0x8B,0x54,0x8A,0xCA,0x7A,0x50,0xBC,0xC0);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("D0DB664C-3B19-404B-8B54-8ACA7A50BCC0")
    _DVTProgressBar : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTProgressBarVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTProgressBar * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTProgressBar * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTProgressBar * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTProgressBar * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTProgressBar * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTProgressBar * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTProgressBar * This,
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
    } _DVTProgressBarVtbl;

    interface _DVTProgressBar
    {
        CONST_VTBL struct _DVTProgressBarVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTProgressBar_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTProgressBar_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTProgressBar_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTProgressBar_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTProgressBar_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTProgressBar_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTProgressBar_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTProgressBar_DISPINTERFACE_DEFINED__ */


#ifndef ___DVTProgressBarEvents_DISPINTERFACE_DEFINED__
#define ___DVTProgressBarEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DVTProgressBarEvents */
/* [helpstring][uuid] */ 


DEFINE_GUID(DIID__DVTProgressBarEvents,0x799BA6E4,0xC589,0x47EF,0xB8,0x96,0xA1,0x04,0x73,0x1B,0x4A,0xD9);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("799BA6E4-C589-47EF-B896-A104731B4AD9")
    _DVTProgressBarEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DVTProgressBarEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DVTProgressBarEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DVTProgressBarEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DVTProgressBarEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DVTProgressBarEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DVTProgressBarEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DVTProgressBarEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DVTProgressBarEvents * This,
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
    } _DVTProgressBarEventsVtbl;

    interface _DVTProgressBarEvents
    {
        CONST_VTBL struct _DVTProgressBarEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DVTProgressBarEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DVTProgressBarEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DVTProgressBarEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DVTProgressBarEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DVTProgressBarEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DVTProgressBarEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DVTProgressBarEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DVTProgressBarEvents_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_VTProgressBar,0xED1CBCF6,0x7824,0x4294,0x92,0xD1,0x23,0x0E,0x9B,0x26,0xDE,0x2C);

#ifdef __cplusplus

class DECLSPEC_UUID("ED1CBCF6-7824-4294-92D1-230E9B26DE2C")
VTProgressBar;
#endif
#endif /* __VTCONTROLS2Lib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


