

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Thu Feb 16 00:54:31 2017
 */
/* Compiler settings for measure.idl:
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


#ifndef __measure_h_h__
#define __measure_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ICalibr_FWD_DEFINED__
#define __ICalibr_FWD_DEFINED__
typedef interface ICalibr ICalibr;

#endif 	/* __ICalibr_FWD_DEFINED__ */


#ifndef __IMeasureObject_FWD_DEFINED__
#define __IMeasureObject_FWD_DEFINED__
typedef interface IMeasureObject IMeasureObject;

#endif 	/* __IMeasureObject_FWD_DEFINED__ */


#ifndef __IMeasureObject2_FWD_DEFINED__
#define __IMeasureObject2_FWD_DEFINED__
typedef interface IMeasureObject2 IMeasureObject2;

#endif 	/* __IMeasureObject2_FWD_DEFINED__ */


#ifndef __IMeasureView_FWD_DEFINED__
#define __IMeasureView_FWD_DEFINED__
typedef interface IMeasureView IMeasureView;

#endif 	/* __IMeasureView_FWD_DEFINED__ */


#ifndef __IManualMeasureObject_FWD_DEFINED__
#define __IManualMeasureObject_FWD_DEFINED__
typedef interface IManualMeasureObject IManualMeasureObject;

#endif 	/* __IManualMeasureObject_FWD_DEFINED__ */


#ifndef __IManualMeasureObject2_FWD_DEFINED__
#define __IManualMeasureObject2_FWD_DEFINED__
typedef interface IManualMeasureObject2 IManualMeasureObject2;

#endif 	/* __IManualMeasureObject2_FWD_DEFINED__ */


#ifndef __IMeasParamGroup_FWD_DEFINED__
#define __IMeasParamGroup_FWD_DEFINED__
typedef interface IMeasParamGroup IMeasParamGroup;

#endif 	/* __IMeasParamGroup_FWD_DEFINED__ */


#ifndef __ICalcObjectContainer_FWD_DEFINED__
#define __ICalcObjectContainer_FWD_DEFINED__
typedef interface ICalcObjectContainer ICalcObjectContainer;

#endif 	/* __ICalcObjectContainer_FWD_DEFINED__ */


#ifndef __IMeasureManager_FWD_DEFINED__
#define __IMeasureManager_FWD_DEFINED__
typedef interface IMeasureManager IMeasureManager;

#endif 	/* __IMeasureManager_FWD_DEFINED__ */


#ifndef __IUnitManager_FWD_DEFINED__
#define __IUnitManager_FWD_DEFINED__
typedef interface IUnitManager IUnitManager;

#endif 	/* __IUnitManager_FWD_DEFINED__ */


#ifndef __ICalcObject_FWD_DEFINED__
#define __ICalcObject_FWD_DEFINED__
typedef interface ICalcObject ICalcObject;

#endif 	/* __ICalcObject_FWD_DEFINED__ */


#ifndef __IClassObject_FWD_DEFINED__
#define __IClassObject_FWD_DEFINED__
typedef interface IClassObject IClassObject;

#endif 	/* __IClassObject_FWD_DEFINED__ */


#ifndef __IPriority_FWD_DEFINED__
#define __IPriority_FWD_DEFINED__
typedef interface IPriority IPriority;

#endif 	/* __IPriority_FWD_DEFINED__ */


#ifndef __ICalcObject2_FWD_DEFINED__
#define __ICalcObject2_FWD_DEFINED__
typedef interface ICalcObject2 ICalcObject2;

#endif 	/* __ICalcObject2_FWD_DEFINED__ */


#ifndef __ICompositeObject_FWD_DEFINED__
#define __ICompositeObject_FWD_DEFINED__
typedef interface ICompositeObject ICompositeObject;

#endif 	/* __ICompositeObject_FWD_DEFINED__ */


#ifndef __ICompositeMeasureGroup_FWD_DEFINED__
#define __ICompositeMeasureGroup_FWD_DEFINED__
typedef interface ICompositeMeasureGroup ICompositeMeasureGroup;

#endif 	/* __ICompositeMeasureGroup_FWD_DEFINED__ */


#ifndef __ICompositeSupport_FWD_DEFINED__
#define __ICompositeSupport_FWD_DEFINED__
typedef interface ICompositeSupport ICompositeSupport;

#endif 	/* __ICompositeSupport_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __CalibrationTypeLib_LIBRARY_DEFINED__
#define __CalibrationTypeLib_LIBRARY_DEFINED__

/* library CalibrationTypeLib */
/* [helpstring][version][uuid] */ 

typedef LONG_PTR TPOS;


enum copy_flags
    {
        cf_selected	= 0,
        cf_all	= 1,
        cf_grid	= 2
    } ;

enum EMeasObjType
    {
        emotUndefined	= 0,
        emotLine	= ( emotUndefined + 1 ) ,
        emotBrokenLine	= ( emotLine + 1 ) ,
        emotSpline	= ( emotBrokenLine + 1 ) ,
        emotCycle3	= ( emotSpline + 1 ) ,
        emotCycle2	= ( emotCycle3 + 1 ) ,
        emotAngle2Line	= ( emotCycle2 + 1 ) ,
        emotAngle3Point	= ( emotAngle2Line + 1 ) ,
        emotCount	= ( emotAngle3Point + 1 ) ,
        emotFreeLine	= ( emotCount + 1 ) ,
        emotParallelLines	= ( emotFreeLine + 1 ) 
    } ;
struct ParameterDescriptor
    {
    long cbSize;
    BSTR bstrName;
    BSTR bstrUserName;
    BSTR bstrDefFormat;
    long lKey;
    long lEnabled;
    TPOS pos;
    BSTR bstrUnit;
    double fCoeffToUnits;
    DWORD dwPanesMask;
    DWORD adwPhasesMask[ 4 ];
    long lTableOrder;
    } ;

enum ShiftMode
    {
        esmNextParam	= 0,
        esmNextObject	= 1,
        esmFillObject	= 2
    } ;

enum ParamType
    {
        etUndefined	= 0,
        etLinear	= ( etUndefined + 1 ) ,
        etCounter	= ( etLinear + 1 ) ,
        etAngle	= ( etCounter + 1 ) 
    } ;
struct ParameterContainer
    {
    long cbSize;
    long lpos;
    IMeasParamGroup *pGroup;
    struct ParameterDescriptor *pDescr;
    enum ParamType type;
    BSTR bstrAction;
    int cxWidth;
    } ;
struct ParameterAdditionalInfo
    {
    byte byPane;
    byte byPhase;
    } ;
struct CCompositeInfo
    {
    int m_nCount;
    int *p_nClass;
    int m_nVersion;
    BOOL m_bActual;
    } ;

EXTERN_C const IID LIBID_CalibrationTypeLib;

#ifndef __ICalibr_INTERFACE_DEFINED__
#define __ICalibr_INTERFACE_DEFINED__

/* interface ICalibr */
/* [object][uuid] */ 


EXTERN_C const IID IID_ICalibr;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("384B4AA8-CDCA-4739-8B15-38823C87874C")
    ICalibr : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetCalibration( 
            /* [out] */ double *pfPixelPerMeter,
            /* [out] */ GUID *pguidC) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetCalibration( 
            /* [in] */ double fPixelPerMeter,
            /* [in] */ REFGUID guidC) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ICalibrVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICalibr * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICalibr * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICalibr * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetCalibration )( 
            ICalibr * This,
            /* [out] */ double *pfPixelPerMeter,
            /* [out] */ GUID *pguidC);
        
        HRESULT ( STDMETHODCALLTYPE *SetCalibration )( 
            ICalibr * This,
            /* [in] */ double fPixelPerMeter,
            /* [in] */ REFGUID guidC);
        
        END_INTERFACE
    } ICalibrVtbl;

    interface ICalibr
    {
        CONST_VTBL struct ICalibrVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICalibr_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICalibr_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICalibr_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICalibr_GetCalibration(This,pfPixelPerMeter,pguidC)	\
    ( (This)->lpVtbl -> GetCalibration(This,pfPixelPerMeter,pguidC) ) 

#define ICalibr_SetCalibration(This,fPixelPerMeter,guidC)	\
    ( (This)->lpVtbl -> SetCalibration(This,fPixelPerMeter,guidC) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICalibr_INTERFACE_DEFINED__ */


#ifndef __IMeasureObject_INTERFACE_DEFINED__
#define __IMeasureObject_INTERFACE_DEFINED__

/* interface IMeasureObject */
/* [object][uuid] */ 


EXTERN_C const IID IID_IMeasureObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6D849A65-8C44-11d3-A686-0090275995FE")
    IMeasureObject : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetImage( 
            IUnknown *punkImage) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetImage( 
            IUnknown **ppunkImage) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UpdateParent( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetChildByParam( 
            long lParamKey,
            IUnknown **ppunkChild) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetDrawingNumber( 
            long nNum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDrawingNumber( 
            long *pnNum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetNumberPos( 
            POINT point) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNumberPos( 
            POINT *ppoint) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IMeasureObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMeasureObject * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMeasureObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMeasureObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetImage )( 
            IMeasureObject * This,
            IUnknown *punkImage);
        
        HRESULT ( STDMETHODCALLTYPE *GetImage )( 
            IMeasureObject * This,
            IUnknown **ppunkImage);
        
        HRESULT ( STDMETHODCALLTYPE *UpdateParent )( 
            IMeasureObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetChildByParam )( 
            IMeasureObject * This,
            long lParamKey,
            IUnknown **ppunkChild);
        
        HRESULT ( STDMETHODCALLTYPE *SetDrawingNumber )( 
            IMeasureObject * This,
            long nNum);
        
        HRESULT ( STDMETHODCALLTYPE *GetDrawingNumber )( 
            IMeasureObject * This,
            long *pnNum);
        
        HRESULT ( STDMETHODCALLTYPE *SetNumberPos )( 
            IMeasureObject * This,
            POINT point);
        
        HRESULT ( STDMETHODCALLTYPE *GetNumberPos )( 
            IMeasureObject * This,
            POINT *ppoint);
        
        END_INTERFACE
    } IMeasureObjectVtbl;

    interface IMeasureObject
    {
        CONST_VTBL struct IMeasureObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMeasureObject_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMeasureObject_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMeasureObject_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMeasureObject_SetImage(This,punkImage)	\
    ( (This)->lpVtbl -> SetImage(This,punkImage) ) 

#define IMeasureObject_GetImage(This,ppunkImage)	\
    ( (This)->lpVtbl -> GetImage(This,ppunkImage) ) 

#define IMeasureObject_UpdateParent(This)	\
    ( (This)->lpVtbl -> UpdateParent(This) ) 

#define IMeasureObject_GetChildByParam(This,lParamKey,ppunkChild)	\
    ( (This)->lpVtbl -> GetChildByParam(This,lParamKey,ppunkChild) ) 

#define IMeasureObject_SetDrawingNumber(This,nNum)	\
    ( (This)->lpVtbl -> SetDrawingNumber(This,nNum) ) 

#define IMeasureObject_GetDrawingNumber(This,pnNum)	\
    ( (This)->lpVtbl -> GetDrawingNumber(This,pnNum) ) 

#define IMeasureObject_SetNumberPos(This,point)	\
    ( (This)->lpVtbl -> SetNumberPos(This,point) ) 

#define IMeasureObject_GetNumberPos(This,ppoint)	\
    ( (This)->lpVtbl -> GetNumberPos(This,ppoint) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMeasureObject_INTERFACE_DEFINED__ */


#ifndef __IMeasureObject2_INTERFACE_DEFINED__
#define __IMeasureObject2_INTERFACE_DEFINED__

/* interface IMeasureObject2 */
/* [object][uuid] */ 


EXTERN_C const IID IID_IMeasureObject2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1DB4EFA0-4859-446e-996B-AD03059B9495")
    IMeasureObject2 : public IMeasureObject
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetZOrder( 
            int nZOrder) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetZOrder( 
            int *pnZOrder) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IMeasureObject2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMeasureObject2 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMeasureObject2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMeasureObject2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetImage )( 
            IMeasureObject2 * This,
            IUnknown *punkImage);
        
        HRESULT ( STDMETHODCALLTYPE *GetImage )( 
            IMeasureObject2 * This,
            IUnknown **ppunkImage);
        
        HRESULT ( STDMETHODCALLTYPE *UpdateParent )( 
            IMeasureObject2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetChildByParam )( 
            IMeasureObject2 * This,
            long lParamKey,
            IUnknown **ppunkChild);
        
        HRESULT ( STDMETHODCALLTYPE *SetDrawingNumber )( 
            IMeasureObject2 * This,
            long nNum);
        
        HRESULT ( STDMETHODCALLTYPE *GetDrawingNumber )( 
            IMeasureObject2 * This,
            long *pnNum);
        
        HRESULT ( STDMETHODCALLTYPE *SetNumberPos )( 
            IMeasureObject2 * This,
            POINT point);
        
        HRESULT ( STDMETHODCALLTYPE *GetNumberPos )( 
            IMeasureObject2 * This,
            POINT *ppoint);
        
        HRESULT ( STDMETHODCALLTYPE *SetZOrder )( 
            IMeasureObject2 * This,
            int nZOrder);
        
        HRESULT ( STDMETHODCALLTYPE *GetZOrder )( 
            IMeasureObject2 * This,
            int *pnZOrder);
        
        END_INTERFACE
    } IMeasureObject2Vtbl;

    interface IMeasureObject2
    {
        CONST_VTBL struct IMeasureObject2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMeasureObject2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMeasureObject2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMeasureObject2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMeasureObject2_SetImage(This,punkImage)	\
    ( (This)->lpVtbl -> SetImage(This,punkImage) ) 

#define IMeasureObject2_GetImage(This,ppunkImage)	\
    ( (This)->lpVtbl -> GetImage(This,ppunkImage) ) 

#define IMeasureObject2_UpdateParent(This)	\
    ( (This)->lpVtbl -> UpdateParent(This) ) 

#define IMeasureObject2_GetChildByParam(This,lParamKey,ppunkChild)	\
    ( (This)->lpVtbl -> GetChildByParam(This,lParamKey,ppunkChild) ) 

#define IMeasureObject2_SetDrawingNumber(This,nNum)	\
    ( (This)->lpVtbl -> SetDrawingNumber(This,nNum) ) 

#define IMeasureObject2_GetDrawingNumber(This,pnNum)	\
    ( (This)->lpVtbl -> GetDrawingNumber(This,pnNum) ) 

#define IMeasureObject2_SetNumberPos(This,point)	\
    ( (This)->lpVtbl -> SetNumberPos(This,point) ) 

#define IMeasureObject2_GetNumberPos(This,ppoint)	\
    ( (This)->lpVtbl -> GetNumberPos(This,ppoint) ) 


#define IMeasureObject2_SetZOrder(This,nZOrder)	\
    ( (This)->lpVtbl -> SetZOrder(This,nZOrder) ) 

#define IMeasureObject2_GetZOrder(This,pnZOrder)	\
    ( (This)->lpVtbl -> GetZOrder(This,pnZOrder) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMeasureObject2_INTERFACE_DEFINED__ */


#ifndef __IMeasureView_INTERFACE_DEFINED__
#define __IMeasureView_INTERFACE_DEFINED__

/* interface IMeasureView */
/* [object][uuid] */ 


EXTERN_C const IID IID_IMeasureView;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C3A156EF-EFD4-4a10-BA37-6D6825970C93")
    IMeasureView : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE AlignByImage( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CopyToClipboard( 
            DWORD dwFlags) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IMeasureViewVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMeasureView * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMeasureView * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMeasureView * This);
        
        HRESULT ( STDMETHODCALLTYPE *AlignByImage )( 
            IMeasureView * This);
        
        HRESULT ( STDMETHODCALLTYPE *CopyToClipboard )( 
            IMeasureView * This,
            DWORD dwFlags);
        
        END_INTERFACE
    } IMeasureViewVtbl;

    interface IMeasureView
    {
        CONST_VTBL struct IMeasureViewVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMeasureView_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMeasureView_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMeasureView_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMeasureView_AlignByImage(This)	\
    ( (This)->lpVtbl -> AlignByImage(This) ) 

#define IMeasureView_CopyToClipboard(This,dwFlags)	\
    ( (This)->lpVtbl -> CopyToClipboard(This,dwFlags) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMeasureView_INTERFACE_DEFINED__ */


#ifndef __IManualMeasureObject_INTERFACE_DEFINED__
#define __IManualMeasureObject_INTERFACE_DEFINED__

/* interface IManualMeasureObject */
/* [object][uuid] */ 


EXTERN_C const IID IID_IManualMeasureObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("47B615DD-0D57-4c57-887D-45586529A69E")
    IManualMeasureObject : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetParamInfo( 
            long *plParamKey,
            DWORD *pdwType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetParamInfo( 
            long *plParamKey,
            DWORD *pdwType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetObjectType( 
            DWORD *pdwType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetObjectType( 
            DWORD dwType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetValue( 
            double fVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetValue( 
            double *pfVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CalcValue( 
            double *pfVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ExchangeData( 
            DWORD *pdwData,
            BOOL bRead) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UpdateParent( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE StartStopMeasure( 
            BOOL bStart) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsProcessMeasure( 
            BOOL *pbRun) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetOffset( 
            POINT pt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CrossPoint( 
            POINT pt,
            BOOL *pbReturn) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IManualMeasureObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IManualMeasureObject * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IManualMeasureObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IManualMeasureObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetParamInfo )( 
            IManualMeasureObject * This,
            long *plParamKey,
            DWORD *pdwType);
        
        HRESULT ( STDMETHODCALLTYPE *SetParamInfo )( 
            IManualMeasureObject * This,
            long *plParamKey,
            DWORD *pdwType);
        
        HRESULT ( STDMETHODCALLTYPE *GetObjectType )( 
            IManualMeasureObject * This,
            DWORD *pdwType);
        
        HRESULT ( STDMETHODCALLTYPE *SetObjectType )( 
            IManualMeasureObject * This,
            DWORD dwType);
        
        HRESULT ( STDMETHODCALLTYPE *SetValue )( 
            IManualMeasureObject * This,
            double fVal);
        
        HRESULT ( STDMETHODCALLTYPE *GetValue )( 
            IManualMeasureObject * This,
            double *pfVal);
        
        HRESULT ( STDMETHODCALLTYPE *CalcValue )( 
            IManualMeasureObject * This,
            double *pfVal);
        
        HRESULT ( STDMETHODCALLTYPE *ExchangeData )( 
            IManualMeasureObject * This,
            DWORD *pdwData,
            BOOL bRead);
        
        HRESULT ( STDMETHODCALLTYPE *UpdateParent )( 
            IManualMeasureObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *StartStopMeasure )( 
            IManualMeasureObject * This,
            BOOL bStart);
        
        HRESULT ( STDMETHODCALLTYPE *IsProcessMeasure )( 
            IManualMeasureObject * This,
            BOOL *pbRun);
        
        HRESULT ( STDMETHODCALLTYPE *SetOffset )( 
            IManualMeasureObject * This,
            POINT pt);
        
        HRESULT ( STDMETHODCALLTYPE *CrossPoint )( 
            IManualMeasureObject * This,
            POINT pt,
            BOOL *pbReturn);
        
        END_INTERFACE
    } IManualMeasureObjectVtbl;

    interface IManualMeasureObject
    {
        CONST_VTBL struct IManualMeasureObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IManualMeasureObject_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IManualMeasureObject_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IManualMeasureObject_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IManualMeasureObject_GetParamInfo(This,plParamKey,pdwType)	\
    ( (This)->lpVtbl -> GetParamInfo(This,plParamKey,pdwType) ) 

#define IManualMeasureObject_SetParamInfo(This,plParamKey,pdwType)	\
    ( (This)->lpVtbl -> SetParamInfo(This,plParamKey,pdwType) ) 

#define IManualMeasureObject_GetObjectType(This,pdwType)	\
    ( (This)->lpVtbl -> GetObjectType(This,pdwType) ) 

#define IManualMeasureObject_SetObjectType(This,dwType)	\
    ( (This)->lpVtbl -> SetObjectType(This,dwType) ) 

#define IManualMeasureObject_SetValue(This,fVal)	\
    ( (This)->lpVtbl -> SetValue(This,fVal) ) 

#define IManualMeasureObject_GetValue(This,pfVal)	\
    ( (This)->lpVtbl -> GetValue(This,pfVal) ) 

#define IManualMeasureObject_CalcValue(This,pfVal)	\
    ( (This)->lpVtbl -> CalcValue(This,pfVal) ) 

#define IManualMeasureObject_ExchangeData(This,pdwData,bRead)	\
    ( (This)->lpVtbl -> ExchangeData(This,pdwData,bRead) ) 

#define IManualMeasureObject_UpdateParent(This)	\
    ( (This)->lpVtbl -> UpdateParent(This) ) 

#define IManualMeasureObject_StartStopMeasure(This,bStart)	\
    ( (This)->lpVtbl -> StartStopMeasure(This,bStart) ) 

#define IManualMeasureObject_IsProcessMeasure(This,pbRun)	\
    ( (This)->lpVtbl -> IsProcessMeasure(This,pbRun) ) 

#define IManualMeasureObject_SetOffset(This,pt)	\
    ( (This)->lpVtbl -> SetOffset(This,pt) ) 

#define IManualMeasureObject_CrossPoint(This,pt,pbReturn)	\
    ( (This)->lpVtbl -> CrossPoint(This,pt,pbReturn) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IManualMeasureObject_INTERFACE_DEFINED__ */


#ifndef __IManualMeasureObject2_INTERFACE_DEFINED__
#define __IManualMeasureObject2_INTERFACE_DEFINED__

/* interface IManualMeasureObject2 */
/* [object][uuid] */ 


EXTERN_C const IID IID_IManualMeasureObject2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C6F353C6-A63A-400b-9BEB-8581F9E26B3B")
    IManualMeasureObject2 : public IManualMeasureObject
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetParametersCount( 
            long *plParamsCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetParametersCount( 
            long lParamsCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetParamKey( 
            long lParam,
            long *plParamKey) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetParamKey( 
            long lParam,
            long lParamKey) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetValue2( 
            long lParam,
            double fVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetValue2( 
            long lParam,
            double *pfVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CalcAllValues( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IManualMeasureObject2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IManualMeasureObject2 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IManualMeasureObject2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IManualMeasureObject2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetParamInfo )( 
            IManualMeasureObject2 * This,
            long *plParamKey,
            DWORD *pdwType);
        
        HRESULT ( STDMETHODCALLTYPE *SetParamInfo )( 
            IManualMeasureObject2 * This,
            long *plParamKey,
            DWORD *pdwType);
        
        HRESULT ( STDMETHODCALLTYPE *GetObjectType )( 
            IManualMeasureObject2 * This,
            DWORD *pdwType);
        
        HRESULT ( STDMETHODCALLTYPE *SetObjectType )( 
            IManualMeasureObject2 * This,
            DWORD dwType);
        
        HRESULT ( STDMETHODCALLTYPE *SetValue )( 
            IManualMeasureObject2 * This,
            double fVal);
        
        HRESULT ( STDMETHODCALLTYPE *GetValue )( 
            IManualMeasureObject2 * This,
            double *pfVal);
        
        HRESULT ( STDMETHODCALLTYPE *CalcValue )( 
            IManualMeasureObject2 * This,
            double *pfVal);
        
        HRESULT ( STDMETHODCALLTYPE *ExchangeData )( 
            IManualMeasureObject2 * This,
            DWORD *pdwData,
            BOOL bRead);
        
        HRESULT ( STDMETHODCALLTYPE *UpdateParent )( 
            IManualMeasureObject2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *StartStopMeasure )( 
            IManualMeasureObject2 * This,
            BOOL bStart);
        
        HRESULT ( STDMETHODCALLTYPE *IsProcessMeasure )( 
            IManualMeasureObject2 * This,
            BOOL *pbRun);
        
        HRESULT ( STDMETHODCALLTYPE *SetOffset )( 
            IManualMeasureObject2 * This,
            POINT pt);
        
        HRESULT ( STDMETHODCALLTYPE *CrossPoint )( 
            IManualMeasureObject2 * This,
            POINT pt,
            BOOL *pbReturn);
        
        HRESULT ( STDMETHODCALLTYPE *GetParametersCount )( 
            IManualMeasureObject2 * This,
            long *plParamsCount);
        
        HRESULT ( STDMETHODCALLTYPE *SetParametersCount )( 
            IManualMeasureObject2 * This,
            long lParamsCount);
        
        HRESULT ( STDMETHODCALLTYPE *GetParamKey )( 
            IManualMeasureObject2 * This,
            long lParam,
            long *plParamKey);
        
        HRESULT ( STDMETHODCALLTYPE *SetParamKey )( 
            IManualMeasureObject2 * This,
            long lParam,
            long lParamKey);
        
        HRESULT ( STDMETHODCALLTYPE *SetValue2 )( 
            IManualMeasureObject2 * This,
            long lParam,
            double fVal);
        
        HRESULT ( STDMETHODCALLTYPE *GetValue2 )( 
            IManualMeasureObject2 * This,
            long lParam,
            double *pfVal);
        
        HRESULT ( STDMETHODCALLTYPE *CalcAllValues )( 
            IManualMeasureObject2 * This);
        
        END_INTERFACE
    } IManualMeasureObject2Vtbl;

    interface IManualMeasureObject2
    {
        CONST_VTBL struct IManualMeasureObject2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IManualMeasureObject2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IManualMeasureObject2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IManualMeasureObject2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IManualMeasureObject2_GetParamInfo(This,plParamKey,pdwType)	\
    ( (This)->lpVtbl -> GetParamInfo(This,plParamKey,pdwType) ) 

#define IManualMeasureObject2_SetParamInfo(This,plParamKey,pdwType)	\
    ( (This)->lpVtbl -> SetParamInfo(This,plParamKey,pdwType) ) 

#define IManualMeasureObject2_GetObjectType(This,pdwType)	\
    ( (This)->lpVtbl -> GetObjectType(This,pdwType) ) 

#define IManualMeasureObject2_SetObjectType(This,dwType)	\
    ( (This)->lpVtbl -> SetObjectType(This,dwType) ) 

#define IManualMeasureObject2_SetValue(This,fVal)	\
    ( (This)->lpVtbl -> SetValue(This,fVal) ) 

#define IManualMeasureObject2_GetValue(This,pfVal)	\
    ( (This)->lpVtbl -> GetValue(This,pfVal) ) 

#define IManualMeasureObject2_CalcValue(This,pfVal)	\
    ( (This)->lpVtbl -> CalcValue(This,pfVal) ) 

#define IManualMeasureObject2_ExchangeData(This,pdwData,bRead)	\
    ( (This)->lpVtbl -> ExchangeData(This,pdwData,bRead) ) 

#define IManualMeasureObject2_UpdateParent(This)	\
    ( (This)->lpVtbl -> UpdateParent(This) ) 

#define IManualMeasureObject2_StartStopMeasure(This,bStart)	\
    ( (This)->lpVtbl -> StartStopMeasure(This,bStart) ) 

#define IManualMeasureObject2_IsProcessMeasure(This,pbRun)	\
    ( (This)->lpVtbl -> IsProcessMeasure(This,pbRun) ) 

#define IManualMeasureObject2_SetOffset(This,pt)	\
    ( (This)->lpVtbl -> SetOffset(This,pt) ) 

#define IManualMeasureObject2_CrossPoint(This,pt,pbReturn)	\
    ( (This)->lpVtbl -> CrossPoint(This,pt,pbReturn) ) 


#define IManualMeasureObject2_GetParametersCount(This,plParamsCount)	\
    ( (This)->lpVtbl -> GetParametersCount(This,plParamsCount) ) 

#define IManualMeasureObject2_SetParametersCount(This,lParamsCount)	\
    ( (This)->lpVtbl -> SetParametersCount(This,lParamsCount) ) 

#define IManualMeasureObject2_GetParamKey(This,lParam,plParamKey)	\
    ( (This)->lpVtbl -> GetParamKey(This,lParam,plParamKey) ) 

#define IManualMeasureObject2_SetParamKey(This,lParam,lParamKey)	\
    ( (This)->lpVtbl -> SetParamKey(This,lParam,lParamKey) ) 

#define IManualMeasureObject2_SetValue2(This,lParam,fVal)	\
    ( (This)->lpVtbl -> SetValue2(This,lParam,fVal) ) 

#define IManualMeasureObject2_GetValue2(This,lParam,pfVal)	\
    ( (This)->lpVtbl -> GetValue2(This,lParam,pfVal) ) 

#define IManualMeasureObject2_CalcAllValues(This)	\
    ( (This)->lpVtbl -> CalcAllValues(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IManualMeasureObject2_INTERFACE_DEFINED__ */


#ifndef __IMeasParamGroup_INTERFACE_DEFINED__
#define __IMeasParamGroup_INTERFACE_DEFINED__

/* interface IMeasParamGroup */
/* [object][uuid] */ 


EXTERN_C const IID IID_IMeasParamGroup;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("51807F95-E2B2-4f02-9A64-6FD7886C1762")
    IMeasParamGroup : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CalcValues( 
            IUnknown *punkCalcObject,
            IUnknown *punkSource) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetParamsCount( 
            long *plCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFirstPos( 
            TPOS *plPos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNextParam( 
            TPOS *plPos,
            struct ParameterDescriptor **ppDescriptior) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPosByKey( 
            long lKey,
            TPOS *plPos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetUnit( 
            long lType,
            BSTR *pbstr,
            double *pfCoeffToUnits) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE InitializeCalculation( 
            IUnknown *punkContainer) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FinalizeCalculation( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IMeasParamGroupVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMeasParamGroup * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMeasParamGroup * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMeasParamGroup * This);
        
        HRESULT ( STDMETHODCALLTYPE *CalcValues )( 
            IMeasParamGroup * This,
            IUnknown *punkCalcObject,
            IUnknown *punkSource);
        
        HRESULT ( STDMETHODCALLTYPE *GetParamsCount )( 
            IMeasParamGroup * This,
            long *plCount);
        
        HRESULT ( STDMETHODCALLTYPE *GetFirstPos )( 
            IMeasParamGroup * This,
            TPOS *plPos);
        
        HRESULT ( STDMETHODCALLTYPE *GetNextParam )( 
            IMeasParamGroup * This,
            TPOS *plPos,
            struct ParameterDescriptor **ppDescriptior);
        
        HRESULT ( STDMETHODCALLTYPE *GetPosByKey )( 
            IMeasParamGroup * This,
            long lKey,
            TPOS *plPos);
        
        HRESULT ( STDMETHODCALLTYPE *GetUnit )( 
            IMeasParamGroup * This,
            long lType,
            BSTR *pbstr,
            double *pfCoeffToUnits);
        
        HRESULT ( STDMETHODCALLTYPE *InitializeCalculation )( 
            IMeasParamGroup * This,
            IUnknown *punkContainer);
        
        HRESULT ( STDMETHODCALLTYPE *FinalizeCalculation )( 
            IMeasParamGroup * This);
        
        END_INTERFACE
    } IMeasParamGroupVtbl;

    interface IMeasParamGroup
    {
        CONST_VTBL struct IMeasParamGroupVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMeasParamGroup_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMeasParamGroup_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMeasParamGroup_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMeasParamGroup_CalcValues(This,punkCalcObject,punkSource)	\
    ( (This)->lpVtbl -> CalcValues(This,punkCalcObject,punkSource) ) 

#define IMeasParamGroup_GetParamsCount(This,plCount)	\
    ( (This)->lpVtbl -> GetParamsCount(This,plCount) ) 

#define IMeasParamGroup_GetFirstPos(This,plPos)	\
    ( (This)->lpVtbl -> GetFirstPos(This,plPos) ) 

#define IMeasParamGroup_GetNextParam(This,plPos,ppDescriptior)	\
    ( (This)->lpVtbl -> GetNextParam(This,plPos,ppDescriptior) ) 

#define IMeasParamGroup_GetPosByKey(This,lKey,plPos)	\
    ( (This)->lpVtbl -> GetPosByKey(This,lKey,plPos) ) 

#define IMeasParamGroup_GetUnit(This,lType,pbstr,pfCoeffToUnits)	\
    ( (This)->lpVtbl -> GetUnit(This,lType,pbstr,pfCoeffToUnits) ) 

#define IMeasParamGroup_InitializeCalculation(This,punkContainer)	\
    ( (This)->lpVtbl -> InitializeCalculation(This,punkContainer) ) 

#define IMeasParamGroup_FinalizeCalculation(This)	\
    ( (This)->lpVtbl -> FinalizeCalculation(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMeasParamGroup_INTERFACE_DEFINED__ */


#ifndef __ICalcObjectContainer_INTERFACE_DEFINED__
#define __ICalcObjectContainer_INTERFACE_DEFINED__

/* interface ICalcObjectContainer */
/* [object][uuid] */ 


EXTERN_C const IID IID_ICalcObjectContainer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ACB5D40E-8A91-44d4-AF7F-F63022428A68")
    ICalcObjectContainer : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetCurentPosition( 
            LONG_PTR *plpos,
            long *plKey) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetCurentPosition( 
            LONG_PTR lpos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Move( 
            long lDirection) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFirstParameterPos( 
            LONG_PTR *plpos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLastParameterPos( 
            LONG_PTR *plpos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNextParameter( 
            LONG_PTR *plpos,
            struct ParameterContainer **ppContainer) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPrevParameter( 
            LONG_PTR *plpos,
            struct ParameterContainer **ppContainer) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MoveParameterAfter( 
            LONG_PTR lpos,
            struct ParameterContainer *p) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DefineParameter( 
            long lKey,
            enum ParamType type,
            IMeasParamGroup *pGroup,
            LONG_PTR *plpos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DefineParameterFull( 
            struct ParameterContainer *p,
            LONG_PTR *plpos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveParameter( 
            long lKey) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveParameterAt( 
            LONG_PTR lpos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ParamDefByKey( 
            long lKey,
            struct ParameterContainer **ppContainer) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetParameterCount( 
            long *pnCount) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ICalcObjectContainerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICalcObjectContainer * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICalcObjectContainer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICalcObjectContainer * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetCurentPosition )( 
            ICalcObjectContainer * This,
            LONG_PTR *plpos,
            long *plKey);
        
        HRESULT ( STDMETHODCALLTYPE *SetCurentPosition )( 
            ICalcObjectContainer * This,
            LONG_PTR lpos);
        
        HRESULT ( STDMETHODCALLTYPE *Move )( 
            ICalcObjectContainer * This,
            long lDirection);
        
        HRESULT ( STDMETHODCALLTYPE *GetFirstParameterPos )( 
            ICalcObjectContainer * This,
            LONG_PTR *plpos);
        
        HRESULT ( STDMETHODCALLTYPE *GetLastParameterPos )( 
            ICalcObjectContainer * This,
            LONG_PTR *plpos);
        
        HRESULT ( STDMETHODCALLTYPE *GetNextParameter )( 
            ICalcObjectContainer * This,
            LONG_PTR *plpos,
            struct ParameterContainer **ppContainer);
        
        HRESULT ( STDMETHODCALLTYPE *GetPrevParameter )( 
            ICalcObjectContainer * This,
            LONG_PTR *plpos,
            struct ParameterContainer **ppContainer);
        
        HRESULT ( STDMETHODCALLTYPE *MoveParameterAfter )( 
            ICalcObjectContainer * This,
            LONG_PTR lpos,
            struct ParameterContainer *p);
        
        HRESULT ( STDMETHODCALLTYPE *DefineParameter )( 
            ICalcObjectContainer * This,
            long lKey,
            enum ParamType type,
            IMeasParamGroup *pGroup,
            LONG_PTR *plpos);
        
        HRESULT ( STDMETHODCALLTYPE *DefineParameterFull )( 
            ICalcObjectContainer * This,
            struct ParameterContainer *p,
            LONG_PTR *plpos);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveParameter )( 
            ICalcObjectContainer * This,
            long lKey);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveParameterAt )( 
            ICalcObjectContainer * This,
            LONG_PTR lpos);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            ICalcObjectContainer * This);
        
        HRESULT ( STDMETHODCALLTYPE *ParamDefByKey )( 
            ICalcObjectContainer * This,
            long lKey,
            struct ParameterContainer **ppContainer);
        
        HRESULT ( STDMETHODCALLTYPE *GetParameterCount )( 
            ICalcObjectContainer * This,
            long *pnCount);
        
        END_INTERFACE
    } ICalcObjectContainerVtbl;

    interface ICalcObjectContainer
    {
        CONST_VTBL struct ICalcObjectContainerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICalcObjectContainer_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICalcObjectContainer_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICalcObjectContainer_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICalcObjectContainer_GetCurentPosition(This,plpos,plKey)	\
    ( (This)->lpVtbl -> GetCurentPosition(This,plpos,plKey) ) 

#define ICalcObjectContainer_SetCurentPosition(This,lpos)	\
    ( (This)->lpVtbl -> SetCurentPosition(This,lpos) ) 

#define ICalcObjectContainer_Move(This,lDirection)	\
    ( (This)->lpVtbl -> Move(This,lDirection) ) 

#define ICalcObjectContainer_GetFirstParameterPos(This,plpos)	\
    ( (This)->lpVtbl -> GetFirstParameterPos(This,plpos) ) 

#define ICalcObjectContainer_GetLastParameterPos(This,plpos)	\
    ( (This)->lpVtbl -> GetLastParameterPos(This,plpos) ) 

#define ICalcObjectContainer_GetNextParameter(This,plpos,ppContainer)	\
    ( (This)->lpVtbl -> GetNextParameter(This,plpos,ppContainer) ) 

#define ICalcObjectContainer_GetPrevParameter(This,plpos,ppContainer)	\
    ( (This)->lpVtbl -> GetPrevParameter(This,plpos,ppContainer) ) 

#define ICalcObjectContainer_MoveParameterAfter(This,lpos,p)	\
    ( (This)->lpVtbl -> MoveParameterAfter(This,lpos,p) ) 

#define ICalcObjectContainer_DefineParameter(This,lKey,type,pGroup,plpos)	\
    ( (This)->lpVtbl -> DefineParameter(This,lKey,type,pGroup,plpos) ) 

#define ICalcObjectContainer_DefineParameterFull(This,p,plpos)	\
    ( (This)->lpVtbl -> DefineParameterFull(This,p,plpos) ) 

#define ICalcObjectContainer_RemoveParameter(This,lKey)	\
    ( (This)->lpVtbl -> RemoveParameter(This,lKey) ) 

#define ICalcObjectContainer_RemoveParameterAt(This,lpos)	\
    ( (This)->lpVtbl -> RemoveParameterAt(This,lpos) ) 

#define ICalcObjectContainer_Reset(This)	\
    ( (This)->lpVtbl -> Reset(This) ) 

#define ICalcObjectContainer_ParamDefByKey(This,lKey,ppContainer)	\
    ( (This)->lpVtbl -> ParamDefByKey(This,lKey,ppContainer) ) 

#define ICalcObjectContainer_GetParameterCount(This,pnCount)	\
    ( (This)->lpVtbl -> GetParameterCount(This,pnCount) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICalcObjectContainer_INTERFACE_DEFINED__ */


#ifndef __IMeasureManager_INTERFACE_DEFINED__
#define __IMeasureManager_INTERFACE_DEFINED__

/* interface IMeasureManager */
/* [object][uuid] */ 


EXTERN_C const IID IID_IMeasureManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ABCCD4CB-A816-4afb-BD67-81621BF70DCB")
    IMeasureManager : public IUnknown
    {
    public:
    };
    
    
#else 	/* C style interface */

    typedef struct IMeasureManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMeasureManager * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMeasureManager * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMeasureManager * This);
        
        END_INTERFACE
    } IMeasureManagerVtbl;

    interface IMeasureManager
    {
        CONST_VTBL struct IMeasureManagerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMeasureManager_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMeasureManager_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMeasureManager_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMeasureManager_INTERFACE_DEFINED__ */


#ifndef __IUnitManager_INTERFACE_DEFINED__
#define __IUnitManager_INTERFACE_DEFINED__

/* interface IUnitManager */
/* [uuid] */ 


EXTERN_C const IID IID_IUnitManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6BE5AD64-5DF7-48f1-B40E-2AB97F01BEFF")
    IUnitManager
    {
    public:
        BEGIN_INTERFACE
        virtual HRESULT STDMETHODCALLTYPE GetCurrentUnit( 
            const GUID guidSys,
            GUID *pguidUnit) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetCurrentUnit( 
            const GUID guidSys,
            const GUID guidUnit) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFirstSysUnitPos( 
            long *plpos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNextSysUnit( 
            GUID *pguidSys,
            long *plpos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFirstUnitPos( 
            long lposSys,
            long *plPos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNextUnitPos( 
            long lposSys,
            GUID *pguidUnit,
            long *plPos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveUnit( 
            const GUID guidUnit) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetUnitInfo( 
            const GUID guidUnit,
            const GUID guidSys,
            BSTR bstrName,
            double fUnitToSysUnit,
            BSTR bstrFormat) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetUnitInfo( 
            const GUID guidUnit,
            GUID *pguidSys,
            BSTR *pbstrName,
            double *pfUnitToSysUnit,
            BSTR *pbstrFormat) = 0;
        
        END_INTERFACE
    };
    
    
#else 	/* C style interface */

    typedef struct IUnitManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *GetCurrentUnit )( 
            IUnitManager * This,
            const GUID guidSys,
            GUID *pguidUnit);
        
        HRESULT ( STDMETHODCALLTYPE *SetCurrentUnit )( 
            IUnitManager * This,
            const GUID guidSys,
            const GUID guidUnit);
        
        HRESULT ( STDMETHODCALLTYPE *GetFirstSysUnitPos )( 
            IUnitManager * This,
            long *plpos);
        
        HRESULT ( STDMETHODCALLTYPE *GetNextSysUnit )( 
            IUnitManager * This,
            GUID *pguidSys,
            long *plpos);
        
        HRESULT ( STDMETHODCALLTYPE *GetFirstUnitPos )( 
            IUnitManager * This,
            long lposSys,
            long *plPos);
        
        HRESULT ( STDMETHODCALLTYPE *GetNextUnitPos )( 
            IUnitManager * This,
            long lposSys,
            GUID *pguidUnit,
            long *plPos);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveUnit )( 
            IUnitManager * This,
            const GUID guidUnit);
        
        HRESULT ( STDMETHODCALLTYPE *SetUnitInfo )( 
            IUnitManager * This,
            const GUID guidUnit,
            const GUID guidSys,
            BSTR bstrName,
            double fUnitToSysUnit,
            BSTR bstrFormat);
        
        HRESULT ( STDMETHODCALLTYPE *GetUnitInfo )( 
            IUnitManager * This,
            const GUID guidUnit,
            GUID *pguidSys,
            BSTR *pbstrName,
            double *pfUnitToSysUnit,
            BSTR *pbstrFormat);
        
        END_INTERFACE
    } IUnitManagerVtbl;

    interface IUnitManager
    {
        CONST_VTBL struct IUnitManagerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUnitManager_GetCurrentUnit(This,guidSys,pguidUnit)	\
    ( (This)->lpVtbl -> GetCurrentUnit(This,guidSys,pguidUnit) ) 

#define IUnitManager_SetCurrentUnit(This,guidSys,guidUnit)	\
    ( (This)->lpVtbl -> SetCurrentUnit(This,guidSys,guidUnit) ) 

#define IUnitManager_GetFirstSysUnitPos(This,plpos)	\
    ( (This)->lpVtbl -> GetFirstSysUnitPos(This,plpos) ) 

#define IUnitManager_GetNextSysUnit(This,pguidSys,plpos)	\
    ( (This)->lpVtbl -> GetNextSysUnit(This,pguidSys,plpos) ) 

#define IUnitManager_GetFirstUnitPos(This,lposSys,plPos)	\
    ( (This)->lpVtbl -> GetFirstUnitPos(This,lposSys,plPos) ) 

#define IUnitManager_GetNextUnitPos(This,lposSys,pguidUnit,plPos)	\
    ( (This)->lpVtbl -> GetNextUnitPos(This,lposSys,pguidUnit,plPos) ) 

#define IUnitManager_RemoveUnit(This,guidUnit)	\
    ( (This)->lpVtbl -> RemoveUnit(This,guidUnit) ) 

#define IUnitManager_SetUnitInfo(This,guidUnit,guidSys,bstrName,fUnitToSysUnit,bstrFormat)	\
    ( (This)->lpVtbl -> SetUnitInfo(This,guidUnit,guidSys,bstrName,fUnitToSysUnit,bstrFormat) ) 

#define IUnitManager_GetUnitInfo(This,guidUnit,pguidSys,pbstrName,pfUnitToSysUnit,pbstrFormat)	\
    ( (This)->lpVtbl -> GetUnitInfo(This,guidUnit,pguidSys,pbstrName,pfUnitToSysUnit,pbstrFormat) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUnitManager_INTERFACE_DEFINED__ */


#ifndef __ICalcObject_INTERFACE_DEFINED__
#define __ICalcObject_INTERFACE_DEFINED__

/* interface ICalcObject */
/* [object][uuid] */ 


EXTERN_C const IID IID_ICalcObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("87D67370-50AA-4a2b-A04C-9370A9D3BD86")
    ICalcObject : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetValue( 
            long lParamKey,
            double *pfValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetValue( 
            long lParamKey,
            double fValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveValue( 
            long lParamKey) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ClearValues( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFirstValuePos( 
            LONG_PTR *plPos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNextValue( 
            LONG_PTR *plPos,
            long *plKey,
            double *pfValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetValuePos( 
            long lKey,
            LONG_PTR *plPos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveValueByPos( 
            LONG_PTR lPos) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ICalcObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICalcObject * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICalcObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICalcObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetValue )( 
            ICalcObject * This,
            long lParamKey,
            double *pfValue);
        
        HRESULT ( STDMETHODCALLTYPE *SetValue )( 
            ICalcObject * This,
            long lParamKey,
            double fValue);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveValue )( 
            ICalcObject * This,
            long lParamKey);
        
        HRESULT ( STDMETHODCALLTYPE *ClearValues )( 
            ICalcObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetFirstValuePos )( 
            ICalcObject * This,
            LONG_PTR *plPos);
        
        HRESULT ( STDMETHODCALLTYPE *GetNextValue )( 
            ICalcObject * This,
            LONG_PTR *plPos,
            long *plKey,
            double *pfValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetValuePos )( 
            ICalcObject * This,
            long lKey,
            LONG_PTR *plPos);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveValueByPos )( 
            ICalcObject * This,
            LONG_PTR lPos);
        
        END_INTERFACE
    } ICalcObjectVtbl;

    interface ICalcObject
    {
        CONST_VTBL struct ICalcObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICalcObject_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICalcObject_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICalcObject_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICalcObject_GetValue(This,lParamKey,pfValue)	\
    ( (This)->lpVtbl -> GetValue(This,lParamKey,pfValue) ) 

#define ICalcObject_SetValue(This,lParamKey,fValue)	\
    ( (This)->lpVtbl -> SetValue(This,lParamKey,fValue) ) 

#define ICalcObject_RemoveValue(This,lParamKey)	\
    ( (This)->lpVtbl -> RemoveValue(This,lParamKey) ) 

#define ICalcObject_ClearValues(This)	\
    ( (This)->lpVtbl -> ClearValues(This) ) 

#define ICalcObject_GetFirstValuePos(This,plPos)	\
    ( (This)->lpVtbl -> GetFirstValuePos(This,plPos) ) 

#define ICalcObject_GetNextValue(This,plPos,plKey,pfValue)	\
    ( (This)->lpVtbl -> GetNextValue(This,plPos,plKey,pfValue) ) 

#define ICalcObject_GetValuePos(This,lKey,plPos)	\
    ( (This)->lpVtbl -> GetValuePos(This,lKey,plPos) ) 

#define ICalcObject_RemoveValueByPos(This,lPos)	\
    ( (This)->lpVtbl -> RemoveValueByPos(This,lPos) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICalcObject_INTERFACE_DEFINED__ */


#ifndef __IClassObject_INTERFACE_DEFINED__
#define __IClassObject_INTERFACE_DEFINED__

/* interface IClassObject */
/* [object][uuid] */ 


EXTERN_C const IID IID_IClassObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DE4A00ED-8CB5-4af5-96AF-913113B7925B")
    IClassObject : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetColor( 
            DWORD *pdwColor) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetColor( 
            DWORD dwColor) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetParamLimits( 
            long lParamKey,
            double fLo,
            double fHi) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetParamLimits( 
            long lParamKey,
            double *pfLo,
            double *pfHi) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveParamLimits( 
            long lParamKey) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EmptyAllParamsLimits( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFirstParamLimitsPos( 
            LONG_PTR *plPos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNextParamLimits( 
            LONG_PTR *plPos,
            long *plParamKey,
            double *pfLo,
            double *pfHi) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IClassObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IClassObject * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IClassObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IClassObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetColor )( 
            IClassObject * This,
            DWORD *pdwColor);
        
        HRESULT ( STDMETHODCALLTYPE *SetColor )( 
            IClassObject * This,
            DWORD dwColor);
        
        HRESULT ( STDMETHODCALLTYPE *SetParamLimits )( 
            IClassObject * This,
            long lParamKey,
            double fLo,
            double fHi);
        
        HRESULT ( STDMETHODCALLTYPE *GetParamLimits )( 
            IClassObject * This,
            long lParamKey,
            double *pfLo,
            double *pfHi);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveParamLimits )( 
            IClassObject * This,
            long lParamKey);
        
        HRESULT ( STDMETHODCALLTYPE *EmptyAllParamsLimits )( 
            IClassObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetFirstParamLimitsPos )( 
            IClassObject * This,
            LONG_PTR *plPos);
        
        HRESULT ( STDMETHODCALLTYPE *GetNextParamLimits )( 
            IClassObject * This,
            LONG_PTR *plPos,
            long *plParamKey,
            double *pfLo,
            double *pfHi);
        
        END_INTERFACE
    } IClassObjectVtbl;

    interface IClassObject
    {
        CONST_VTBL struct IClassObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IClassObject_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IClassObject_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IClassObject_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IClassObject_GetColor(This,pdwColor)	\
    ( (This)->lpVtbl -> GetColor(This,pdwColor) ) 

#define IClassObject_SetColor(This,dwColor)	\
    ( (This)->lpVtbl -> SetColor(This,dwColor) ) 

#define IClassObject_SetParamLimits(This,lParamKey,fLo,fHi)	\
    ( (This)->lpVtbl -> SetParamLimits(This,lParamKey,fLo,fHi) ) 

#define IClassObject_GetParamLimits(This,lParamKey,pfLo,pfHi)	\
    ( (This)->lpVtbl -> GetParamLimits(This,lParamKey,pfLo,pfHi) ) 

#define IClassObject_RemoveParamLimits(This,lParamKey)	\
    ( (This)->lpVtbl -> RemoveParamLimits(This,lParamKey) ) 

#define IClassObject_EmptyAllParamsLimits(This)	\
    ( (This)->lpVtbl -> EmptyAllParamsLimits(This) ) 

#define IClassObject_GetFirstParamLimitsPos(This,plPos)	\
    ( (This)->lpVtbl -> GetFirstParamLimitsPos(This,plPos) ) 

#define IClassObject_GetNextParamLimits(This,plPos,plParamKey,pfLo,pfHi)	\
    ( (This)->lpVtbl -> GetNextParamLimits(This,plPos,plParamKey,pfLo,pfHi) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IClassObject_INTERFACE_DEFINED__ */


#ifndef __IPriority_INTERFACE_DEFINED__
#define __IPriority_INTERFACE_DEFINED__

/* interface IPriority */
/* [object][uuid] */ 


EXTERN_C const IID IID_IPriority;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9036F30D-6128-43ac-9997-0B5D0225FFC1")
    IPriority : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetPriority( 
            long *plPriority) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IPriorityVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPriority * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPriority * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPriority * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetPriority )( 
            IPriority * This,
            long *plPriority);
        
        END_INTERFACE
    } IPriorityVtbl;

    interface IPriority
    {
        CONST_VTBL struct IPriorityVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPriority_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPriority_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPriority_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPriority_GetPriority(This,plPriority)	\
    ( (This)->lpVtbl -> GetPriority(This,plPriority) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPriority_INTERFACE_DEFINED__ */


#ifndef __ICalcObject2_INTERFACE_DEFINED__
#define __ICalcObject2_INTERFACE_DEFINED__

/* interface ICalcObject2 */
/* [object][uuid] */ 


EXTERN_C const IID IID_ICalcObject2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C00ACB28-0022-4fb9-BD85-1C5D7C364387")
    ICalcObject2 : public ICalcObject
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetValueEx( 
            long lParamKey,
            struct ParameterAdditionalInfo *pAddInfo,
            double *pfValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetValueEx( 
            long lParamKey,
            struct ParameterAdditionalInfo *pAddInfo,
            double fValue) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ICalcObject2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICalcObject2 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICalcObject2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICalcObject2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetValue )( 
            ICalcObject2 * This,
            long lParamKey,
            double *pfValue);
        
        HRESULT ( STDMETHODCALLTYPE *SetValue )( 
            ICalcObject2 * This,
            long lParamKey,
            double fValue);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveValue )( 
            ICalcObject2 * This,
            long lParamKey);
        
        HRESULT ( STDMETHODCALLTYPE *ClearValues )( 
            ICalcObject2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetFirstValuePos )( 
            ICalcObject2 * This,
            LONG_PTR *plPos);
        
        HRESULT ( STDMETHODCALLTYPE *GetNextValue )( 
            ICalcObject2 * This,
            LONG_PTR *plPos,
            long *plKey,
            double *pfValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetValuePos )( 
            ICalcObject2 * This,
            long lKey,
            LONG_PTR *plPos);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveValueByPos )( 
            ICalcObject2 * This,
            LONG_PTR lPos);
        
        HRESULT ( STDMETHODCALLTYPE *GetValueEx )( 
            ICalcObject2 * This,
            long lParamKey,
            struct ParameterAdditionalInfo *pAddInfo,
            double *pfValue);
        
        HRESULT ( STDMETHODCALLTYPE *SetValueEx )( 
            ICalcObject2 * This,
            long lParamKey,
            struct ParameterAdditionalInfo *pAddInfo,
            double fValue);
        
        END_INTERFACE
    } ICalcObject2Vtbl;

    interface ICalcObject2
    {
        CONST_VTBL struct ICalcObject2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICalcObject2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICalcObject2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICalcObject2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICalcObject2_GetValue(This,lParamKey,pfValue)	\
    ( (This)->lpVtbl -> GetValue(This,lParamKey,pfValue) ) 

#define ICalcObject2_SetValue(This,lParamKey,fValue)	\
    ( (This)->lpVtbl -> SetValue(This,lParamKey,fValue) ) 

#define ICalcObject2_RemoveValue(This,lParamKey)	\
    ( (This)->lpVtbl -> RemoveValue(This,lParamKey) ) 

#define ICalcObject2_ClearValues(This)	\
    ( (This)->lpVtbl -> ClearValues(This) ) 

#define ICalcObject2_GetFirstValuePos(This,plPos)	\
    ( (This)->lpVtbl -> GetFirstValuePos(This,plPos) ) 

#define ICalcObject2_GetNextValue(This,plPos,plKey,pfValue)	\
    ( (This)->lpVtbl -> GetNextValue(This,plPos,plKey,pfValue) ) 

#define ICalcObject2_GetValuePos(This,lKey,plPos)	\
    ( (This)->lpVtbl -> GetValuePos(This,lKey,plPos) ) 

#define ICalcObject2_RemoveValueByPos(This,lPos)	\
    ( (This)->lpVtbl -> RemoveValueByPos(This,lPos) ) 


#define ICalcObject2_GetValueEx(This,lParamKey,pAddInfo,pfValue)	\
    ( (This)->lpVtbl -> GetValueEx(This,lParamKey,pAddInfo,pfValue) ) 

#define ICalcObject2_SetValueEx(This,lParamKey,pAddInfo,fValue)	\
    ( (This)->lpVtbl -> SetValueEx(This,lParamKey,pAddInfo,fValue) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICalcObject2_INTERFACE_DEFINED__ */


#ifndef __ICompositeObject_INTERFACE_DEFINED__
#define __ICompositeObject_INTERFACE_DEFINED__

/* interface ICompositeObject */
/* [object][uuid] */ 


EXTERN_C const IID IID_ICompositeObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D237B552-5247-4332-831A-B1277BD32B5C")
    ICompositeObject : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetPurpose( 
            int purpose) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPurpose( 
            int *purpose) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE BuildTree( 
            /* [defaultvalue] */ LONG binaryCount = -1) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CalculateComposite( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsComposite( 
            BOOL *bC) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetCompositeFlag( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMainLevelObjects( 
            IUnknown **unk) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTreeNotion( 
            void **tn) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLevel( 
            int level,
            IUnknown **unkLevel) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLevelbyClass( 
            int cls,
            IUnknown **unkLevel) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DelayRemove( 
            BOOL delay,
            IUnknown *pUnkLevel) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetElderLevel( 
            int cls,
            IUnknown **unkLevel) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RestoreTreeByGUIDs( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPlaneCount( 
            long *count) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetPlaneCount( 
            long count) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetCompositeInfo( 
            struct CCompositeInfo *ci) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCompositeInfo( 
            struct CCompositeInfo **ci) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ICompositeObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICompositeObject * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICompositeObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICompositeObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetPurpose )( 
            ICompositeObject * This,
            int purpose);
        
        HRESULT ( STDMETHODCALLTYPE *GetPurpose )( 
            ICompositeObject * This,
            int *purpose);
        
        HRESULT ( STDMETHODCALLTYPE *BuildTree )( 
            ICompositeObject * This,
            /* [defaultvalue] */ LONG binaryCount);
        
        HRESULT ( STDMETHODCALLTYPE *CalculateComposite )( 
            ICompositeObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *IsComposite )( 
            ICompositeObject * This,
            BOOL *bC);
        
        HRESULT ( STDMETHODCALLTYPE *SetCompositeFlag )( 
            ICompositeObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetMainLevelObjects )( 
            ICompositeObject * This,
            IUnknown **unk);
        
        HRESULT ( STDMETHODCALLTYPE *GetTreeNotion )( 
            ICompositeObject * This,
            void **tn);
        
        HRESULT ( STDMETHODCALLTYPE *GetLevel )( 
            ICompositeObject * This,
            int level,
            IUnknown **unkLevel);
        
        HRESULT ( STDMETHODCALLTYPE *GetLevelbyClass )( 
            ICompositeObject * This,
            int cls,
            IUnknown **unkLevel);
        
        HRESULT ( STDMETHODCALLTYPE *DelayRemove )( 
            ICompositeObject * This,
            BOOL delay,
            IUnknown *pUnkLevel);
        
        HRESULT ( STDMETHODCALLTYPE *GetElderLevel )( 
            ICompositeObject * This,
            int cls,
            IUnknown **unkLevel);
        
        HRESULT ( STDMETHODCALLTYPE *RestoreTreeByGUIDs )( 
            ICompositeObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetPlaneCount )( 
            ICompositeObject * This,
            long *count);
        
        HRESULT ( STDMETHODCALLTYPE *SetPlaneCount )( 
            ICompositeObject * This,
            long count);
        
        HRESULT ( STDMETHODCALLTYPE *SetCompositeInfo )( 
            ICompositeObject * This,
            struct CCompositeInfo *ci);
        
        HRESULT ( STDMETHODCALLTYPE *GetCompositeInfo )( 
            ICompositeObject * This,
            struct CCompositeInfo **ci);
        
        END_INTERFACE
    } ICompositeObjectVtbl;

    interface ICompositeObject
    {
        CONST_VTBL struct ICompositeObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICompositeObject_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICompositeObject_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICompositeObject_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICompositeObject_SetPurpose(This,purpose)	\
    ( (This)->lpVtbl -> SetPurpose(This,purpose) ) 

#define ICompositeObject_GetPurpose(This,purpose)	\
    ( (This)->lpVtbl -> GetPurpose(This,purpose) ) 

#define ICompositeObject_BuildTree(This,binaryCount)	\
    ( (This)->lpVtbl -> BuildTree(This,binaryCount) ) 

#define ICompositeObject_CalculateComposite(This)	\
    ( (This)->lpVtbl -> CalculateComposite(This) ) 

#define ICompositeObject_IsComposite(This,bC)	\
    ( (This)->lpVtbl -> IsComposite(This,bC) ) 

#define ICompositeObject_SetCompositeFlag(This)	\
    ( (This)->lpVtbl -> SetCompositeFlag(This) ) 

#define ICompositeObject_GetMainLevelObjects(This,unk)	\
    ( (This)->lpVtbl -> GetMainLevelObjects(This,unk) ) 

#define ICompositeObject_GetTreeNotion(This,tn)	\
    ( (This)->lpVtbl -> GetTreeNotion(This,tn) ) 

#define ICompositeObject_GetLevel(This,level,unkLevel)	\
    ( (This)->lpVtbl -> GetLevel(This,level,unkLevel) ) 

#define ICompositeObject_GetLevelbyClass(This,cls,unkLevel)	\
    ( (This)->lpVtbl -> GetLevelbyClass(This,cls,unkLevel) ) 

#define ICompositeObject_DelayRemove(This,delay,pUnkLevel)	\
    ( (This)->lpVtbl -> DelayRemove(This,delay,pUnkLevel) ) 

#define ICompositeObject_GetElderLevel(This,cls,unkLevel)	\
    ( (This)->lpVtbl -> GetElderLevel(This,cls,unkLevel) ) 

#define ICompositeObject_RestoreTreeByGUIDs(This)	\
    ( (This)->lpVtbl -> RestoreTreeByGUIDs(This) ) 

#define ICompositeObject_GetPlaneCount(This,count)	\
    ( (This)->lpVtbl -> GetPlaneCount(This,count) ) 

#define ICompositeObject_SetPlaneCount(This,count)	\
    ( (This)->lpVtbl -> SetPlaneCount(This,count) ) 

#define ICompositeObject_SetCompositeInfo(This,ci)	\
    ( (This)->lpVtbl -> SetCompositeInfo(This,ci) ) 

#define ICompositeObject_GetCompositeInfo(This,ci)	\
    ( (This)->lpVtbl -> GetCompositeInfo(This,ci) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICompositeObject_INTERFACE_DEFINED__ */


#ifndef __ICompositeMeasureGroup_INTERFACE_DEFINED__
#define __ICompositeMeasureGroup_INTERFACE_DEFINED__

/* interface ICompositeMeasureGroup */
/* [object][uuid] */ 


EXTERN_C const IID IID_ICompositeMeasureGroup;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("03542F3F-1651-4858-9F68-A1B90D32959A")
    ICompositeMeasureGroup : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE UpdateClassName( 
            struct ParameterContainer *pc) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ICompositeMeasureGroupVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICompositeMeasureGroup * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICompositeMeasureGroup * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICompositeMeasureGroup * This);
        
        HRESULT ( STDMETHODCALLTYPE *UpdateClassName )( 
            ICompositeMeasureGroup * This,
            struct ParameterContainer *pc);
        
        END_INTERFACE
    } ICompositeMeasureGroupVtbl;

    interface ICompositeMeasureGroup
    {
        CONST_VTBL struct ICompositeMeasureGroupVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICompositeMeasureGroup_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICompositeMeasureGroup_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICompositeMeasureGroup_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICompositeMeasureGroup_UpdateClassName(This,pc)	\
    ( (This)->lpVtbl -> UpdateClassName(This,pc) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICompositeMeasureGroup_INTERFACE_DEFINED__ */


#ifndef __ICompositeSupport_INTERFACE_DEFINED__
#define __ICompositeSupport_INTERFACE_DEFINED__

/* interface ICompositeSupport */
/* [object][uuid] */ 


EXTERN_C const IID IID_ICompositeSupport;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1F88A4A6-83BE-4a5d-BBC0-45734BB8B551")
    ICompositeSupport : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE IsSyncronized( 
            long *flag) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetSyncronizedFlag( 
            long flag) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLevel( 
            long *lvl) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetLevel( 
            long lvl) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPixCount( 
            long *pixCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetPixCount( 
            long pixCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetParentKey( 
            GUID *key) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetParentKey( 
            const GUID key) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTreeKey( 
            GUID *key) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetTreeKey( 
            const GUID key) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetGrandParent( 
            IUnknown **unkParent) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CopyAllCompositeData( 
            IUnknown *destObject) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ICompositeSupportVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICompositeSupport * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICompositeSupport * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICompositeSupport * This);
        
        HRESULT ( STDMETHODCALLTYPE *IsSyncronized )( 
            ICompositeSupport * This,
            long *flag);
        
        HRESULT ( STDMETHODCALLTYPE *SetSyncronizedFlag )( 
            ICompositeSupport * This,
            long flag);
        
        HRESULT ( STDMETHODCALLTYPE *GetLevel )( 
            ICompositeSupport * This,
            long *lvl);
        
        HRESULT ( STDMETHODCALLTYPE *SetLevel )( 
            ICompositeSupport * This,
            long lvl);
        
        HRESULT ( STDMETHODCALLTYPE *GetPixCount )( 
            ICompositeSupport * This,
            long *pixCount);
        
        HRESULT ( STDMETHODCALLTYPE *SetPixCount )( 
            ICompositeSupport * This,
            long pixCount);
        
        HRESULT ( STDMETHODCALLTYPE *GetParentKey )( 
            ICompositeSupport * This,
            GUID *key);
        
        HRESULT ( STDMETHODCALLTYPE *SetParentKey )( 
            ICompositeSupport * This,
            const GUID key);
        
        HRESULT ( STDMETHODCALLTYPE *GetTreeKey )( 
            ICompositeSupport * This,
            GUID *key);
        
        HRESULT ( STDMETHODCALLTYPE *SetTreeKey )( 
            ICompositeSupport * This,
            const GUID key);
        
        HRESULT ( STDMETHODCALLTYPE *GetGrandParent )( 
            ICompositeSupport * This,
            IUnknown **unkParent);
        
        HRESULT ( STDMETHODCALLTYPE *CopyAllCompositeData )( 
            ICompositeSupport * This,
            IUnknown *destObject);
        
        END_INTERFACE
    } ICompositeSupportVtbl;

    interface ICompositeSupport
    {
        CONST_VTBL struct ICompositeSupportVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICompositeSupport_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICompositeSupport_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICompositeSupport_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICompositeSupport_IsSyncronized(This,flag)	\
    ( (This)->lpVtbl -> IsSyncronized(This,flag) ) 

#define ICompositeSupport_SetSyncronizedFlag(This,flag)	\
    ( (This)->lpVtbl -> SetSyncronizedFlag(This,flag) ) 

#define ICompositeSupport_GetLevel(This,lvl)	\
    ( (This)->lpVtbl -> GetLevel(This,lvl) ) 

#define ICompositeSupport_SetLevel(This,lvl)	\
    ( (This)->lpVtbl -> SetLevel(This,lvl) ) 

#define ICompositeSupport_GetPixCount(This,pixCount)	\
    ( (This)->lpVtbl -> GetPixCount(This,pixCount) ) 

#define ICompositeSupport_SetPixCount(This,pixCount)	\
    ( (This)->lpVtbl -> SetPixCount(This,pixCount) ) 

#define ICompositeSupport_GetParentKey(This,key)	\
    ( (This)->lpVtbl -> GetParentKey(This,key) ) 

#define ICompositeSupport_SetParentKey(This,key)	\
    ( (This)->lpVtbl -> SetParentKey(This,key) ) 

#define ICompositeSupport_GetTreeKey(This,key)	\
    ( (This)->lpVtbl -> GetTreeKey(This,key) ) 

#define ICompositeSupport_SetTreeKey(This,key)	\
    ( (This)->lpVtbl -> SetTreeKey(This,key) ) 

#define ICompositeSupport_GetGrandParent(This,unkParent)	\
    ( (This)->lpVtbl -> GetGrandParent(This,unkParent) ) 

#define ICompositeSupport_CopyAllCompositeData(This,destObject)	\
    ( (This)->lpVtbl -> CopyAllCompositeData(This,destObject) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICompositeSupport_INTERFACE_DEFINED__ */

#endif /* __CalibrationTypeLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


