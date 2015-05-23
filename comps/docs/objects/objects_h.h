

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Apr 04 21:04:59 2015
 */
/* Compiler settings for objects.odl:
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


#ifndef __objects_h_h__
#define __objects_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IMeasGroupMgrDisp_FWD_DEFINED__
#define __IMeasGroupMgrDisp_FWD_DEFINED__
typedef interface IMeasGroupMgrDisp IMeasGroupMgrDisp;

#endif 	/* __IMeasGroupMgrDisp_FWD_DEFINED__ */


#ifndef __MeasGroupMgr_FWD_DEFINED__
#define __MeasGroupMgr_FWD_DEFINED__

#ifdef __cplusplus
typedef class MeasGroupMgr MeasGroupMgr;
#else
typedef struct MeasGroupMgr MeasGroupMgr;
#endif /* __cplusplus */

#endif 	/* __MeasGroupMgr_FWD_DEFINED__ */


#ifndef __IFereMeasGroup_FWD_DEFINED__
#define __IFereMeasGroup_FWD_DEFINED__
typedef interface IFereMeasGroup IFereMeasGroup;

#endif 	/* __IFereMeasGroup_FWD_DEFINED__ */


#ifndef __FereMeasGroup_FWD_DEFINED__
#define __FereMeasGroup_FWD_DEFINED__

#ifdef __cplusplus
typedef class FereMeasGroup FereMeasGroup;
#else
typedef struct FereMeasGroup FereMeasGroup;
#endif /* __cplusplus */

#endif 	/* __FereMeasGroup_FWD_DEFINED__ */


#ifndef __IOpticMeasureMan_FWD_DEFINED__
#define __IOpticMeasureMan_FWD_DEFINED__
typedef interface IOpticMeasureMan IOpticMeasureMan;

#endif 	/* __IOpticMeasureMan_FWD_DEFINED__ */


#ifndef __OpticMeasureMan_FWD_DEFINED__
#define __OpticMeasureMan_FWD_DEFINED__

#ifdef __cplusplus
typedef class OpticMeasureMan OpticMeasureMan;
#else
typedef struct OpticMeasureMan OpticMeasureMan;
#endif /* __cplusplus */

#endif 	/* __OpticMeasureMan_FWD_DEFINED__ */


#ifndef __IOpticMeasGroup_FWD_DEFINED__
#define __IOpticMeasGroup_FWD_DEFINED__
typedef interface IOpticMeasGroup IOpticMeasGroup;

#endif 	/* __IOpticMeasGroup_FWD_DEFINED__ */


#ifndef __OpticMeasGroup_FWD_DEFINED__
#define __OpticMeasGroup_FWD_DEFINED__

#ifdef __cplusplus
typedef class OpticMeasGroup OpticMeasGroup;
#else
typedef struct OpticMeasGroup OpticMeasGroup;
#endif /* __cplusplus */

#endif 	/* __OpticMeasGroup_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __Objects_LIBRARY_DEFINED__
#define __Objects_LIBRARY_DEFINED__

/* library Objects */
/* [version][uuid] */ 


DEFINE_GUID(LIBID_Objects,0xA594B22B,0x8B74,0x11D3,0xA6,0x84,0x00,0x90,0x27,0x59,0x95,0xFE);

#ifndef __IMeasGroupMgrDisp_DISPINTERFACE_DEFINED__
#define __IMeasGroupMgrDisp_DISPINTERFACE_DEFINED__

/* dispinterface IMeasGroupMgrDisp */
/* [uuid] */ 


DEFINE_GUID(DIID_IMeasGroupMgrDisp,0xF71103BA,0x5014,0x4b2b,0xB6,0x82,0x3E,0xBF,0xE4,0x53,0xC0,0x1F);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("F71103BA-5014-4b2b-B682-3EBFE453C01F")
    IMeasGroupMgrDisp : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IMeasGroupMgrDispVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMeasGroupMgrDisp * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMeasGroupMgrDisp * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMeasGroupMgrDisp * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IMeasGroupMgrDisp * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IMeasGroupMgrDisp * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IMeasGroupMgrDisp * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IMeasGroupMgrDisp * This,
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
    } IMeasGroupMgrDispVtbl;

    interface IMeasGroupMgrDisp
    {
        CONST_VTBL struct IMeasGroupMgrDispVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMeasGroupMgrDisp_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMeasGroupMgrDisp_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMeasGroupMgrDisp_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMeasGroupMgrDisp_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IMeasGroupMgrDisp_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IMeasGroupMgrDisp_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IMeasGroupMgrDisp_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IMeasGroupMgrDisp_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_MeasGroupMgr,0x91678E7E,0xB56C,0x4c98,0x86,0x71,0x84,0xC4,0x0C,0x65,0x26,0x8F);

#ifdef __cplusplus

class DECLSPEC_UUID("91678E7E-B56C-4c98-8671-84C40C65268F")
MeasGroupMgr;
#endif

#ifndef __IFereMeasGroup_DISPINTERFACE_DEFINED__
#define __IFereMeasGroup_DISPINTERFACE_DEFINED__

/* dispinterface IFereMeasGroup */
/* [uuid] */ 


DEFINE_GUID(DIID_IFereMeasGroup,0xD64C3F55,0x9E69,0x44F8,0x86,0xF8,0x9D,0x6D,0xB9,0x57,0x52,0xFE);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("D64C3F55-9E69-44F8-86F8-9D6DB95752FE")
    IFereMeasGroup : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IFereMeasGroupVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFereMeasGroup * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFereMeasGroup * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFereMeasGroup * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IFereMeasGroup * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IFereMeasGroup * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IFereMeasGroup * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IFereMeasGroup * This,
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
    } IFereMeasGroupVtbl;

    interface IFereMeasGroup
    {
        CONST_VTBL struct IFereMeasGroupVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFereMeasGroup_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFereMeasGroup_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFereMeasGroup_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IFereMeasGroup_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IFereMeasGroup_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IFereMeasGroup_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IFereMeasGroup_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IFereMeasGroup_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_FereMeasGroup,0x1ADC0C0D,0xA5F2,0x40F6,0xAA,0x99,0x60,0xA1,0xB2,0x38,0xD0,0xC6);

#ifdef __cplusplus

class DECLSPEC_UUID("1ADC0C0D-A5F2-40F6-AA99-60A1B238D0C6")
FereMeasGroup;
#endif

#ifndef __IOpticMeasureMan_DISPINTERFACE_DEFINED__
#define __IOpticMeasureMan_DISPINTERFACE_DEFINED__

/* dispinterface IOpticMeasureMan */
/* [uuid] */ 


DEFINE_GUID(DIID_IOpticMeasureMan,0x67C3AAD6,0x0466,0x4F36,0x89,0x97,0xEA,0x56,0xBF,0x92,0x45,0x02);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("67C3AAD6-0466-4F36-8997-EA56BF924502")
    IOpticMeasureMan : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IOpticMeasureManVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOpticMeasureMan * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOpticMeasureMan * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOpticMeasureMan * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IOpticMeasureMan * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IOpticMeasureMan * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IOpticMeasureMan * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IOpticMeasureMan * This,
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
    } IOpticMeasureManVtbl;

    interface IOpticMeasureMan
    {
        CONST_VTBL struct IOpticMeasureManVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOpticMeasureMan_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IOpticMeasureMan_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IOpticMeasureMan_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IOpticMeasureMan_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IOpticMeasureMan_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IOpticMeasureMan_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IOpticMeasureMan_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IOpticMeasureMan_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_OpticMeasureMan,0xB8921000,0x6AC5,0x483D,0xA2,0x9E,0xB5,0x3B,0xBF,0x89,0x80,0x97);

#ifdef __cplusplus

class DECLSPEC_UUID("B8921000-6AC5-483D-A29E-B53BBF898097")
OpticMeasureMan;
#endif

#ifndef __IOpticMeasGroup_DISPINTERFACE_DEFINED__
#define __IOpticMeasGroup_DISPINTERFACE_DEFINED__

/* dispinterface IOpticMeasGroup */
/* [uuid] */ 


DEFINE_GUID(DIID_IOpticMeasGroup,0x28484FDE,0x18CE,0x420D,0x88,0x5B,0x1F,0xAE,0x63,0x9F,0x33,0xB7);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("28484FDE-18CE-420D-885B-1FAE639F33B7")
    IOpticMeasGroup : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IOpticMeasGroupVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOpticMeasGroup * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOpticMeasGroup * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOpticMeasGroup * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IOpticMeasGroup * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IOpticMeasGroup * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IOpticMeasGroup * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IOpticMeasGroup * This,
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
    } IOpticMeasGroupVtbl;

    interface IOpticMeasGroup
    {
        CONST_VTBL struct IOpticMeasGroupVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOpticMeasGroup_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IOpticMeasGroup_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IOpticMeasGroup_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IOpticMeasGroup_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IOpticMeasGroup_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IOpticMeasGroup_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IOpticMeasGroup_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IOpticMeasGroup_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_OpticMeasGroup,0x723BFD60,0xB073,0x4C1E,0xAF,0xCE,0xAB,0x17,0xF3,0x9F,0x70,0x63);

#ifdef __cplusplus

class DECLSPEC_UUID("723BFD60-B073-4C1E-AFCE-AB17F39F7063")
OpticMeasGroup;
#endif
#endif /* __Objects_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


