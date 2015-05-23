

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sun Apr 26 01:42:55 2015
 */
/* Compiler settings for Charts.idl:
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

#ifndef __Charts_h_h__
#define __Charts_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IChartAttributes_FWD_DEFINED__
#define __IChartAttributes_FWD_DEFINED__
typedef interface IChartAttributes IChartAttributes;

#endif 	/* __IChartAttributes_FWD_DEFINED__ */


#ifndef __IChartDrawer_FWD_DEFINED__
#define __IChartDrawer_FWD_DEFINED__
typedef interface IChartDrawer IChartDrawer;

#endif 	/* __IChartDrawer_FWD_DEFINED__ */


#ifndef __ISmartChartDrawer_FWD_DEFINED__
#define __ISmartChartDrawer_FWD_DEFINED__
typedef interface ISmartChartDrawer ISmartChartDrawer;

#endif 	/* __ISmartChartDrawer_FWD_DEFINED__ */


#ifndef __IChartMiscHelper_FWD_DEFINED__
#define __IChartMiscHelper_FWD_DEFINED__
typedef interface IChartMiscHelper IChartMiscHelper;

#endif 	/* __IChartMiscHelper_FWD_DEFINED__ */


#ifndef __ChartDrawer_FWD_DEFINED__
#define __ChartDrawer_FWD_DEFINED__

#ifdef __cplusplus
typedef class ChartDrawer ChartDrawer;
#else
typedef struct ChartDrawer ChartDrawer;
#endif /* __cplusplus */

#endif 	/* __ChartDrawer_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_Charts_0000_0000 */
/* [local] */ 

typedef /* [helpstring][v1_enum][uuid] */  DECLSPEC_UUID("07BB0478-1A9C-4f59-B56A-41F347515210") 
enum ChartColors
    {
        ccfChartColor	= ( 1 << 0 ) ,
        ccfAxisColor	= ( 1 << 1 ) ,
        ccfAxisText	= ( 1 << 2 ) ,
        ccfGistFillColor	= ( 1 << 3 ) ,
        ccfHintTextColor	= ( 1 << 4 ) 
    } 	ChartColors;

typedef /* [helpstring][v1_enum][uuid] */  DECLSPEC_UUID("E39FC74B-8902-4735-90D6-CEF9EE63445D") 
enum CommonChartColors
    {
        ccfBackColor	= ( 1 << 0 ) ,
        ccfTargetColor	= ( 1 << 1 ) ,
        ccfGridColor	= ( 1 << 2 ) 
    } 	CommonChartColors;

typedef /* [helpstring][v1_enum][uuid] */  DECLSPEC_UUID("2C4233C6-9F24-4766-BB5C-58D0F4E92B28") 
enum ChartLineStyles
    {
        clfChartStyle	= ( 1 << 0 ) ,
        clfAxisStyle	= ( 1 << 1 ) 
    } 	ChartLineStyles;

typedef /* [helpstring][v1_enum][uuid] */  DECLSPEC_UUID("3D21EDE6-3FCF-49be-9A89-D50F12941F74") 
enum CommonChartLineStyles
    {
        clfTargetStyle	= ( 1 << 0 ) 
    } 	CommonChartLineStyles;

typedef /* [helpstring][v1_enum][uuid] */  DECLSPEC_UUID("E960EDE6-9F85-4703-B59B-28E7256D404B") 
enum ChartLineThickness
    {
        ctfChartThick	= ( 1 << 0 ) ,
        ctfAxisThick	= ( 1 << 1 ) 
    } 	ChartLineThickness;

typedef /* [helpstring][v1_enum][uuid] */  DECLSPEC_UUID("985DDEB5-5147-4bad-8F9A-9275E939FD3B") 
enum CommonChartLineThickness
    {
        ctfTargetThick	= ( 1 << 0 ) 
    } 	CommonChartLineThickness;

typedef /* [helpstring][v1_enum][uuid] */  DECLSPEC_UUID("344963EF-9AF7-4fa0-B279-225B246A4F48") 
enum ChartLineSmooth
    {
        csfMovingAverage	= ( 1 << 0 ) ,
        csfExp	= ( 1 << 1 ) ,
        csfMediane	= ( 1 << 2 ) ,
        csfSpline2	= ( 1 << 3 ) ,
        csfSpline3	= ( 1 << 4 ) 
    } 	ChartLineSmooth;

typedef /* [helpstring][v1_enum][uuid] */  DECLSPEC_UUID("3BFDBAC2-D26A-4aed-9220-C1F8894BBA65") 
enum ChartViewParam
    {
        cfAxisX	= ( 1 << 0 ) ,
        cfAxisY	= ( 1 << 1 ) ,
        cfAxisArrowX	= ( 1 << 2 ) ,
        cfAxisArrowY	= ( 1 << 3 ) ,
        cfAxisNameX	= ( 1 << 4 ) ,
        cfAxisNameY	= ( 1 << 5 ) ,
        cfAxisDigitsX	= ( 1 << 6 ) ,
        cfAxisDigitsY	= ( 1 << 7 ) ,
        cfAxisScaleX	= ( 1 << 8 ) ,
        cfAxisScaleY	= ( 1 << 9 ) ,
        cfHatch	= ( 1 << 10 ) ,
        cfAll	= ( ( ( ( ( ( ( ( ( cfAxisScaleY | cfAxisScaleX )  | cfAxisDigitsY )  | cfAxisDigitsX )  | cfAxisNameY )  | cfAxisNameX )  | cfAxisArrowY )  | cfAxisArrowX )  | cfAxisY )  | cfAxisX ) 
    } 	ChartViewParam;

typedef /* [helpstring][v1_enum][uuid] */  DECLSPEC_UUID("C787B687-59F0-49ba-8D21-7B66B301DBC0") 
enum CommonChartViewParam
    {
        cfBack	= ( 1 << 0 ) ,
        cfTargetVert	= ( 1 << 1 ) ,
        cfTargetHorz	= ( 1 << 2 ) ,
        cfAxisOnBorder	= ( 1 << 3 ) ,
        cfAbsolute	= ( 1 << 4 ) ,
        cf90CW	= ( 1 << 5 ) ,
        cf90CCW	= ( 1 << 6 ) ,
        cf180CW	= ( 1 << 7 ) ,
        cf180CCW	= ( 1 << 8 ) 
    } 	CommonChartViewParam;

typedef /* [helpstring][v1_enum][uuid] */  DECLSPEC_UUID("8E74CC24-4107-46e7-A448-98ECBECB5582") 
enum ChartDataType
    {
        cdfDataX	= ( 1 << 0 ) ,
        cdfDataY	= ( 1 << 1 ) ,
        cdfDataGist	= ( 1 << 2 ) 
    } 	ChartDataType;

typedef /* [helpstring][v1_enum][uuid] */  DECLSPEC_UUID("6914F5BE-D89E-4684-A2E0-FC9729598087") 
enum ChartRange
    {
        crfRangeX	= ( 1 << 0 ) ,
        crfRangeY	= ( 1 << 1 ) 
    } 	ChartRange;

typedef /* [helpstring][v1_enum][uuid] */  DECLSPEC_UUID("282DEA9C-B33B-4ded-8746-B08AFAAD8615") 
enum ChartAxis
    {
        cafAxisX	= ( 1 << 0 ) ,
        cafAxisY	= ( 1 << 1 ) 
    } 	ChartAxis;

typedef /* [helpstring][v1_enum][uuid] */  DECLSPEC_UUID("277C2844-9C23-4461-ACBF-02DE53D96DA8") 
enum ChartFont
    {
        cfnDigit	= ( 1 << 0 ) ,
        cfnAxisName	= ( 1 << 1 ) 
    } 	ChartFont;

typedef /* [helpstring][v1_enum][uuid] */  DECLSPEC_UUID("8C03CD61-E69F-453a-8909-19FE150EB945") 
enum ChartTextFormat
    {
        cffDigitsX	= ( 1 << 0 ) ,
        cffDigitsY	= ( 1 << 1 ) 
    } 	ChartTextFormat;

typedef /* [helpstring][v1_enum][uuid] */  DECLSPEC_UUID("7A73C1B6-F5C1-4ff7-91CF-F6313FAAF4DF") 
enum CommonChartConst
    {
        ccfAxisOffsetX	= ( 1 << 0 ) ,
        ccfAxisOffsetY	= ( 1 << 1 ) ,
        ccfArrowLength	= ( 1 << 2 ) ,
        ccfScaleHalfSize	= ( 1 << 3 ) ,
        ccfScaleCountX	= ( 1 << 4 ) ,
        ccfScaleCountY	= ( 1 << 5 ) ,
        ccfDigitCountX	= ( 1 << 7 ) ,
        ccfDigitCountY	= ( 1 << 8 ) ,
        ccfLBorder	= ( 1 << 9 ) ,
        ccfRBorder	= ( 1 << 10 ) ,
        ccfTBorder	= ( 1 << 11 ) ,
        ccfBBorder	= ( 1 << 12 ) ,
        ccfScaleDirectionX	= ( 1 << 13 ) ,
        ccfScaleDirectionY	= ( 1 << 14 ) ,
        ccfTextOffsetX	= ( 1 << 15 ) ,
        ccfTextOffsetY	= ( 1 << 16 ) 
    } 	CommonChartConst;

typedef /* [helpstring][v1_enum][uuid] */  DECLSPEC_UUID("7BEC68C6-E78C-48bd-994B-547DDCB2A655") 
enum CommonChartGrid
    {
        ccgXGrid	= ( 1 << 0 ) ,
        ccgYGrid	= ( 1 << 1 ) ,
        ccgXEndGrid	= ( 1 << 2 ) ,
        ccgYEndGrid	= ( 1 << 3 ) 
    } 	CommonChartGrid;

typedef /* [helpstring][v1_enum][uuid] */  DECLSPEC_UUID("B8FEDE44-E998-4af8-ADF5-EEB922F1EB74") 
enum ChartMarker
    {
        cmCross	= ( 1 << 0 ) ,
        cmRect	= ( 1 << 1 ) ,
        cmCircle	= ( 1 << 2 ) 
    } 	ChartMarker;



extern RPC_IF_HANDLE __MIDL_itf_Charts_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_Charts_0000_0000_v0_0_s_ifspec;

#ifndef __IChartAttributes_INTERFACE_DEFINED__
#define __IChartAttributes_INTERFACE_DEFINED__

/* interface IChartAttributes */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IChartAttributes;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7E4AA4DE-46B3-4de2-9802-72AEA184E85E")
    IChartAttributes : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetColor( 
            /* [in] */ DWORD lColor,
            /* [in] */ CommonChartColors dwFlag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetColor( 
            /* [out] */ DWORD *lColor,
            /* [in] */ CommonChartColors dwFlag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetChartColor( 
            /* [in] */ long nID,
            /* [in] */ DWORD lColor,
            /* [in] */ ChartColors dwFlag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetChartColor( 
            /* [in] */ long nID,
            /* [out] */ DWORD *lColor,
            /* [in] */ ChartColors dwFlag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetStyle( 
            /* [in] */ CommonChartViewParam dwFlag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetStyle( 
            /* [out] */ CommonChartViewParam *dwFlag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetChartStyle( 
            /* [in] */ long nID,
            /* [in] */ ChartViewParam dwFlag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetChartStyle( 
            /* [in] */ long nID,
            /* [out] */ ChartViewParam *dwFlag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetLineStyle( 
            /* [in] */ ULONG lStyle,
            /* [in] */ CommonChartLineStyles dwFlag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetLineStyle( 
            /* [out] */ ULONG *lStyle,
            /* [in] */ CommonChartLineStyles dwFlag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetChartLineStyle( 
            /* [in] */ long nID,
            /* [in] */ ULONG lStyle,
            /* [in] */ ChartLineStyles dwFlag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetChartLineStyle( 
            /* [in] */ long nID,
            /* [out] */ ULONG *lStyle,
            /* [in] */ ChartLineStyles dwFlag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetLineThick( 
            /* [in] */ long nThick,
            /* [in] */ CommonChartLineThickness dwFlag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetLineThick( 
            /* [out] */ long *nThick,
            /* [in] */ CommonChartLineThickness dwFlag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetChartLineThick( 
            /* [in] */ long nID,
            /* [in] */ long nThick,
            /* [in] */ ChartLineThickness dwFlag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetChartLineThick( 
            /* [in] */ long nID,
            /* [out] */ long *nThick,
            /* [in] */ ChartLineThickness dwFlag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetChartRange( 
            /* [in] */ long nID,
            /* [in] */ double lfMin,
            /* [in] */ double lfMax,
            /* [in] */ ChartRange dwFlag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetChartRange( 
            /* [in] */ long nID,
            /* [out] */ double *lfMin,
            /* [out] */ double *lfMax,
            /* [in] */ ChartRange dwFlag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetRangeState( 
            /* [in] */ long nID,
            /* [in] */ int bEnable,
            /* [in] */ ChartRange dwFlag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetRangeState( 
            /* [in] */ long nID,
            /* [out] */ int *bEnable,
            /* [in] */ ChartRange dwFlag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetTargetPos( 
            /* [in] */ POINT ptPos) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetTextFormat( 
            /* [in] */ long nID,
            /* [in] */ BSTR bstrFormat,
            /* [in] */ ChartTextFormat dwFlag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RemoveChart( 
            /* [in] */ long nID) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ShowChart( 
            /* [in] */ long nID,
            /* [in] */ int bShow) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetAxisText( 
            /* [in] */ long nID,
            /* [in] */ BSTR bstrText,
            /* [in] */ ChartAxis dwFlag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAxisText( 
            /* [in] */ long nID,
            /* [out] */ BSTR *bstrText,
            /* [in] */ ChartAxis dwFlag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetConstParams( 
            /* [in] */ long nVal,
            /* [in] */ CommonChartConst dwFlag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetConstParams( 
            /* [out] */ long *nVal,
            /* [in] */ CommonChartConst dwFlag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetColorizedMap( 
            /* [in] */ long nID,
            /* [in] */ DWORD lColor,
            /* [in] */ double lfMinX,
            /* [in] */ double lfMinY,
            /* [in] */ double lfMaxX,
            /* [in] */ double lfMaxY) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ClearColorized( 
            /* [in] */ long nID) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EnableGridLines( 
            /* [in] */ CommonChartGrid dwFlags) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetGridLines( 
            /* [in] */ double *fGridVals,
            /* [in] */ long lCount,
            /* [in] */ CommonChartGrid dwFlags) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetScaleLabels( 
            /* [in] */ long nID,
            /* [in] */ double *pfVals,
            /* [in] */ long lCount,
            /* [in] */ ChartAxis dwFlags) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetScaleTextLabels( 
            /* [in] */ long nID,
            /* [in] */ double *pfVals,
            /* [in] */ double *pfLabels,
            /* [in] */ long lCount,
            /* [in] */ ChartAxis dwFlags) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetAxisTextFont( 
            /* [in] */ long nID,
            /* [in] */ BYTE *plfFont,
            /* [in] */ ChartAxis dwFlags,
            /* [in] */ ChartFont dwFlag2) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RemoveHints( 
            /* [in] */ long nID) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetHintText( 
            /* [in] */ long nID,
            /* [in] */ double fX,
            /* [in] */ double fY,
            /* [in] */ BSTR bstrHint,
            /* [in] */ int nAlign,
            /* [in] */ int XOffsetInPixel,
            /* [in] */ int YOffsetInPixel) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetHintFont( 
            /* [in] */ long nID,
            /* [in] */ BYTE *plfFont) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetTextZoom( 
            /* [in] */ double fZoom) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTextZoom( 
            /* [retval][out] */ double *pfZoom) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetPixelUnit( 
            /* [in] */ int nUnit) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IChartAttributesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IChartAttributes * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IChartAttributes * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IChartAttributes * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetColor )( 
            IChartAttributes * This,
            /* [in] */ DWORD lColor,
            /* [in] */ CommonChartColors dwFlag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetColor )( 
            IChartAttributes * This,
            /* [out] */ DWORD *lColor,
            /* [in] */ CommonChartColors dwFlag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetChartColor )( 
            IChartAttributes * This,
            /* [in] */ long nID,
            /* [in] */ DWORD lColor,
            /* [in] */ ChartColors dwFlag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetChartColor )( 
            IChartAttributes * This,
            /* [in] */ long nID,
            /* [out] */ DWORD *lColor,
            /* [in] */ ChartColors dwFlag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetStyle )( 
            IChartAttributes * This,
            /* [in] */ CommonChartViewParam dwFlag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetStyle )( 
            IChartAttributes * This,
            /* [out] */ CommonChartViewParam *dwFlag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetChartStyle )( 
            IChartAttributes * This,
            /* [in] */ long nID,
            /* [in] */ ChartViewParam dwFlag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetChartStyle )( 
            IChartAttributes * This,
            /* [in] */ long nID,
            /* [out] */ ChartViewParam *dwFlag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetLineStyle )( 
            IChartAttributes * This,
            /* [in] */ ULONG lStyle,
            /* [in] */ CommonChartLineStyles dwFlag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetLineStyle )( 
            IChartAttributes * This,
            /* [out] */ ULONG *lStyle,
            /* [in] */ CommonChartLineStyles dwFlag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetChartLineStyle )( 
            IChartAttributes * This,
            /* [in] */ long nID,
            /* [in] */ ULONG lStyle,
            /* [in] */ ChartLineStyles dwFlag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetChartLineStyle )( 
            IChartAttributes * This,
            /* [in] */ long nID,
            /* [out] */ ULONG *lStyle,
            /* [in] */ ChartLineStyles dwFlag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetLineThick )( 
            IChartAttributes * This,
            /* [in] */ long nThick,
            /* [in] */ CommonChartLineThickness dwFlag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetLineThick )( 
            IChartAttributes * This,
            /* [out] */ long *nThick,
            /* [in] */ CommonChartLineThickness dwFlag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetChartLineThick )( 
            IChartAttributes * This,
            /* [in] */ long nID,
            /* [in] */ long nThick,
            /* [in] */ ChartLineThickness dwFlag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetChartLineThick )( 
            IChartAttributes * This,
            /* [in] */ long nID,
            /* [out] */ long *nThick,
            /* [in] */ ChartLineThickness dwFlag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetChartRange )( 
            IChartAttributes * This,
            /* [in] */ long nID,
            /* [in] */ double lfMin,
            /* [in] */ double lfMax,
            /* [in] */ ChartRange dwFlag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetChartRange )( 
            IChartAttributes * This,
            /* [in] */ long nID,
            /* [out] */ double *lfMin,
            /* [out] */ double *lfMax,
            /* [in] */ ChartRange dwFlag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetRangeState )( 
            IChartAttributes * This,
            /* [in] */ long nID,
            /* [in] */ int bEnable,
            /* [in] */ ChartRange dwFlag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetRangeState )( 
            IChartAttributes * This,
            /* [in] */ long nID,
            /* [out] */ int *bEnable,
            /* [in] */ ChartRange dwFlag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetTargetPos )( 
            IChartAttributes * This,
            /* [in] */ POINT ptPos);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetTextFormat )( 
            IChartAttributes * This,
            /* [in] */ long nID,
            /* [in] */ BSTR bstrFormat,
            /* [in] */ ChartTextFormat dwFlag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RemoveChart )( 
            IChartAttributes * This,
            /* [in] */ long nID);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ShowChart )( 
            IChartAttributes * This,
            /* [in] */ long nID,
            /* [in] */ int bShow);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetAxisText )( 
            IChartAttributes * This,
            /* [in] */ long nID,
            /* [in] */ BSTR bstrText,
            /* [in] */ ChartAxis dwFlag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetAxisText )( 
            IChartAttributes * This,
            /* [in] */ long nID,
            /* [out] */ BSTR *bstrText,
            /* [in] */ ChartAxis dwFlag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetConstParams )( 
            IChartAttributes * This,
            /* [in] */ long nVal,
            /* [in] */ CommonChartConst dwFlag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetConstParams )( 
            IChartAttributes * This,
            /* [out] */ long *nVal,
            /* [in] */ CommonChartConst dwFlag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetColorizedMap )( 
            IChartAttributes * This,
            /* [in] */ long nID,
            /* [in] */ DWORD lColor,
            /* [in] */ double lfMinX,
            /* [in] */ double lfMinY,
            /* [in] */ double lfMaxX,
            /* [in] */ double lfMaxY);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ClearColorized )( 
            IChartAttributes * This,
            /* [in] */ long nID);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *EnableGridLines )( 
            IChartAttributes * This,
            /* [in] */ CommonChartGrid dwFlags);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetGridLines )( 
            IChartAttributes * This,
            /* [in] */ double *fGridVals,
            /* [in] */ long lCount,
            /* [in] */ CommonChartGrid dwFlags);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetScaleLabels )( 
            IChartAttributes * This,
            /* [in] */ long nID,
            /* [in] */ double *pfVals,
            /* [in] */ long lCount,
            /* [in] */ ChartAxis dwFlags);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetScaleTextLabels )( 
            IChartAttributes * This,
            /* [in] */ long nID,
            /* [in] */ double *pfVals,
            /* [in] */ double *pfLabels,
            /* [in] */ long lCount,
            /* [in] */ ChartAxis dwFlags);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetAxisTextFont )( 
            IChartAttributes * This,
            /* [in] */ long nID,
            /* [in] */ BYTE *plfFont,
            /* [in] */ ChartAxis dwFlags,
            /* [in] */ ChartFont dwFlag2);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RemoveHints )( 
            IChartAttributes * This,
            /* [in] */ long nID);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetHintText )( 
            IChartAttributes * This,
            /* [in] */ long nID,
            /* [in] */ double fX,
            /* [in] */ double fY,
            /* [in] */ BSTR bstrHint,
            /* [in] */ int nAlign,
            /* [in] */ int XOffsetInPixel,
            /* [in] */ int YOffsetInPixel);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetHintFont )( 
            IChartAttributes * This,
            /* [in] */ long nID,
            /* [in] */ BYTE *plfFont);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetTextZoom )( 
            IChartAttributes * This,
            /* [in] */ double fZoom);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetTextZoom )( 
            IChartAttributes * This,
            /* [retval][out] */ double *pfZoom);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetPixelUnit )( 
            IChartAttributes * This,
            /* [in] */ int nUnit);
        
        END_INTERFACE
    } IChartAttributesVtbl;

    interface IChartAttributes
    {
        CONST_VTBL struct IChartAttributesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IChartAttributes_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IChartAttributes_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IChartAttributes_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IChartAttributes_SetColor(This,lColor,dwFlag)	\
    ( (This)->lpVtbl -> SetColor(This,lColor,dwFlag) ) 

#define IChartAttributes_GetColor(This,lColor,dwFlag)	\
    ( (This)->lpVtbl -> GetColor(This,lColor,dwFlag) ) 

#define IChartAttributes_SetChartColor(This,nID,lColor,dwFlag)	\
    ( (This)->lpVtbl -> SetChartColor(This,nID,lColor,dwFlag) ) 

#define IChartAttributes_GetChartColor(This,nID,lColor,dwFlag)	\
    ( (This)->lpVtbl -> GetChartColor(This,nID,lColor,dwFlag) ) 

#define IChartAttributes_SetStyle(This,dwFlag)	\
    ( (This)->lpVtbl -> SetStyle(This,dwFlag) ) 

#define IChartAttributes_GetStyle(This,dwFlag)	\
    ( (This)->lpVtbl -> GetStyle(This,dwFlag) ) 

#define IChartAttributes_SetChartStyle(This,nID,dwFlag)	\
    ( (This)->lpVtbl -> SetChartStyle(This,nID,dwFlag) ) 

#define IChartAttributes_GetChartStyle(This,nID,dwFlag)	\
    ( (This)->lpVtbl -> GetChartStyle(This,nID,dwFlag) ) 

#define IChartAttributes_SetLineStyle(This,lStyle,dwFlag)	\
    ( (This)->lpVtbl -> SetLineStyle(This,lStyle,dwFlag) ) 

#define IChartAttributes_GetLineStyle(This,lStyle,dwFlag)	\
    ( (This)->lpVtbl -> GetLineStyle(This,lStyle,dwFlag) ) 

#define IChartAttributes_SetChartLineStyle(This,nID,lStyle,dwFlag)	\
    ( (This)->lpVtbl -> SetChartLineStyle(This,nID,lStyle,dwFlag) ) 

#define IChartAttributes_GetChartLineStyle(This,nID,lStyle,dwFlag)	\
    ( (This)->lpVtbl -> GetChartLineStyle(This,nID,lStyle,dwFlag) ) 

#define IChartAttributes_SetLineThick(This,nThick,dwFlag)	\
    ( (This)->lpVtbl -> SetLineThick(This,nThick,dwFlag) ) 

#define IChartAttributes_GetLineThick(This,nThick,dwFlag)	\
    ( (This)->lpVtbl -> GetLineThick(This,nThick,dwFlag) ) 

#define IChartAttributes_SetChartLineThick(This,nID,nThick,dwFlag)	\
    ( (This)->lpVtbl -> SetChartLineThick(This,nID,nThick,dwFlag) ) 

#define IChartAttributes_GetChartLineThick(This,nID,nThick,dwFlag)	\
    ( (This)->lpVtbl -> GetChartLineThick(This,nID,nThick,dwFlag) ) 

#define IChartAttributes_SetChartRange(This,nID,lfMin,lfMax,dwFlag)	\
    ( (This)->lpVtbl -> SetChartRange(This,nID,lfMin,lfMax,dwFlag) ) 

#define IChartAttributes_GetChartRange(This,nID,lfMin,lfMax,dwFlag)	\
    ( (This)->lpVtbl -> GetChartRange(This,nID,lfMin,lfMax,dwFlag) ) 

#define IChartAttributes_SetRangeState(This,nID,bEnable,dwFlag)	\
    ( (This)->lpVtbl -> SetRangeState(This,nID,bEnable,dwFlag) ) 

#define IChartAttributes_GetRangeState(This,nID,bEnable,dwFlag)	\
    ( (This)->lpVtbl -> GetRangeState(This,nID,bEnable,dwFlag) ) 

#define IChartAttributes_SetTargetPos(This,ptPos)	\
    ( (This)->lpVtbl -> SetTargetPos(This,ptPos) ) 

#define IChartAttributes_SetTextFormat(This,nID,bstrFormat,dwFlag)	\
    ( (This)->lpVtbl -> SetTextFormat(This,nID,bstrFormat,dwFlag) ) 

#define IChartAttributes_RemoveChart(This,nID)	\
    ( (This)->lpVtbl -> RemoveChart(This,nID) ) 

#define IChartAttributes_ShowChart(This,nID,bShow)	\
    ( (This)->lpVtbl -> ShowChart(This,nID,bShow) ) 

#define IChartAttributes_SetAxisText(This,nID,bstrText,dwFlag)	\
    ( (This)->lpVtbl -> SetAxisText(This,nID,bstrText,dwFlag) ) 

#define IChartAttributes_GetAxisText(This,nID,bstrText,dwFlag)	\
    ( (This)->lpVtbl -> GetAxisText(This,nID,bstrText,dwFlag) ) 

#define IChartAttributes_SetConstParams(This,nVal,dwFlag)	\
    ( (This)->lpVtbl -> SetConstParams(This,nVal,dwFlag) ) 

#define IChartAttributes_GetConstParams(This,nVal,dwFlag)	\
    ( (This)->lpVtbl -> GetConstParams(This,nVal,dwFlag) ) 

#define IChartAttributes_SetColorizedMap(This,nID,lColor,lfMinX,lfMinY,lfMaxX,lfMaxY)	\
    ( (This)->lpVtbl -> SetColorizedMap(This,nID,lColor,lfMinX,lfMinY,lfMaxX,lfMaxY) ) 

#define IChartAttributes_ClearColorized(This,nID)	\
    ( (This)->lpVtbl -> ClearColorized(This,nID) ) 

#define IChartAttributes_EnableGridLines(This,dwFlags)	\
    ( (This)->lpVtbl -> EnableGridLines(This,dwFlags) ) 

#define IChartAttributes_SetGridLines(This,fGridVals,lCount,dwFlags)	\
    ( (This)->lpVtbl -> SetGridLines(This,fGridVals,lCount,dwFlags) ) 

#define IChartAttributes_SetScaleLabels(This,nID,pfVals,lCount,dwFlags)	\
    ( (This)->lpVtbl -> SetScaleLabels(This,nID,pfVals,lCount,dwFlags) ) 

#define IChartAttributes_SetScaleTextLabels(This,nID,pfVals,pfLabels,lCount,dwFlags)	\
    ( (This)->lpVtbl -> SetScaleTextLabels(This,nID,pfVals,pfLabels,lCount,dwFlags) ) 

#define IChartAttributes_SetAxisTextFont(This,nID,plfFont,dwFlags,dwFlag2)	\
    ( (This)->lpVtbl -> SetAxisTextFont(This,nID,plfFont,dwFlags,dwFlag2) ) 

#define IChartAttributes_RemoveHints(This,nID)	\
    ( (This)->lpVtbl -> RemoveHints(This,nID) ) 

#define IChartAttributes_SetHintText(This,nID,fX,fY,bstrHint,nAlign,XOffsetInPixel,YOffsetInPixel)	\
    ( (This)->lpVtbl -> SetHintText(This,nID,fX,fY,bstrHint,nAlign,XOffsetInPixel,YOffsetInPixel) ) 

#define IChartAttributes_SetHintFont(This,nID,plfFont)	\
    ( (This)->lpVtbl -> SetHintFont(This,nID,plfFont) ) 

#define IChartAttributes_SetTextZoom(This,fZoom)	\
    ( (This)->lpVtbl -> SetTextZoom(This,fZoom) ) 

#define IChartAttributes_GetTextZoom(This,pfZoom)	\
    ( (This)->lpVtbl -> GetTextZoom(This,pfZoom) ) 

#define IChartAttributes_SetPixelUnit(This,nUnit)	\
    ( (This)->lpVtbl -> SetPixelUnit(This,nUnit) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IChartAttributes_INTERFACE_DEFINED__ */


#ifndef __IChartDrawer_INTERFACE_DEFINED__
#define __IChartDrawer_INTERFACE_DEFINED__

/* interface IChartDrawer */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IChartDrawer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4EB92343-D990-4511-A4B9-30036B6036D2")
    IChartDrawer : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetData( 
            /* [in] */ long nID,
            /* [in] */ double *pValues,
            /* [in] */ long nCount,
            /* [in] */ ChartDataType dwFlag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetData( 
            /* [in] */ long nID,
            /* [out] */ double **pValues,
            /* [out] */ long *nCount,
            /* [in] */ ChartDataType dwFlag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetData2( 
            /* [in] */ long nID,
            /* [in] */ VARIANT PtrToFunct,
            /* [in] */ double lfMinX,
            /* [in] */ double lfMaxX,
            /* [in] */ double lfStep) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetExternData( 
            /* [in] */ long nID,
            /* [in] */ double *pValues,
            /* [in] */ long nCount,
            /* [in] */ ChartDataType dwFlag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Draw( 
            /* [in] */ DWORD hDC,
            /* [in] */ RECT rcDraw) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetToolTipValue( 
            /* [in] */ long nID,
            /* [in] */ POINT ptMiceInDC,
            /* [out] */ double *pX,
            /* [out] */ double *pY,
            /* [in] */ short bYfromX,
            /* [in] */ int nHalfHotZone) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SmoothChart( 
            /* [in] */ long nID,
            /* [in] */ double fSmoothParam,
            /* [in] */ short bYFromX,
            /* [in] */ ChartLineSmooth dwFlag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ClearAll( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LogarifmScale( 
            /* [in] */ ChartDataType dwFlag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddMarker( 
            /* [in] */ double fX,
            /* [in] */ double fY,
            /* [in] */ long nPixelInSize,
            COLORREF clColor,
            ChartMarker dwFlag) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IChartDrawerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IChartDrawer * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IChartDrawer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IChartDrawer * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetData )( 
            IChartDrawer * This,
            /* [in] */ long nID,
            /* [in] */ double *pValues,
            /* [in] */ long nCount,
            /* [in] */ ChartDataType dwFlag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetData )( 
            IChartDrawer * This,
            /* [in] */ long nID,
            /* [out] */ double **pValues,
            /* [out] */ long *nCount,
            /* [in] */ ChartDataType dwFlag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetData2 )( 
            IChartDrawer * This,
            /* [in] */ long nID,
            /* [in] */ VARIANT PtrToFunct,
            /* [in] */ double lfMinX,
            /* [in] */ double lfMaxX,
            /* [in] */ double lfStep);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetExternData )( 
            IChartDrawer * This,
            /* [in] */ long nID,
            /* [in] */ double *pValues,
            /* [in] */ long nCount,
            /* [in] */ ChartDataType dwFlag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Draw )( 
            IChartDrawer * This,
            /* [in] */ DWORD hDC,
            /* [in] */ RECT rcDraw);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetToolTipValue )( 
            IChartDrawer * This,
            /* [in] */ long nID,
            /* [in] */ POINT ptMiceInDC,
            /* [out] */ double *pX,
            /* [out] */ double *pY,
            /* [in] */ short bYfromX,
            /* [in] */ int nHalfHotZone);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SmoothChart )( 
            IChartDrawer * This,
            /* [in] */ long nID,
            /* [in] */ double fSmoothParam,
            /* [in] */ short bYFromX,
            /* [in] */ ChartLineSmooth dwFlag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ClearAll )( 
            IChartDrawer * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LogarifmScale )( 
            IChartDrawer * This,
            /* [in] */ ChartDataType dwFlag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddMarker )( 
            IChartDrawer * This,
            /* [in] */ double fX,
            /* [in] */ double fY,
            /* [in] */ long nPixelInSize,
            COLORREF clColor,
            ChartMarker dwFlag);
        
        END_INTERFACE
    } IChartDrawerVtbl;

    interface IChartDrawer
    {
        CONST_VTBL struct IChartDrawerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IChartDrawer_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IChartDrawer_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IChartDrawer_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IChartDrawer_SetData(This,nID,pValues,nCount,dwFlag)	\
    ( (This)->lpVtbl -> SetData(This,nID,pValues,nCount,dwFlag) ) 

#define IChartDrawer_GetData(This,nID,pValues,nCount,dwFlag)	\
    ( (This)->lpVtbl -> GetData(This,nID,pValues,nCount,dwFlag) ) 

#define IChartDrawer_SetData2(This,nID,PtrToFunct,lfMinX,lfMaxX,lfStep)	\
    ( (This)->lpVtbl -> SetData2(This,nID,PtrToFunct,lfMinX,lfMaxX,lfStep) ) 

#define IChartDrawer_SetExternData(This,nID,pValues,nCount,dwFlag)	\
    ( (This)->lpVtbl -> SetExternData(This,nID,pValues,nCount,dwFlag) ) 

#define IChartDrawer_Draw(This,hDC,rcDraw)	\
    ( (This)->lpVtbl -> Draw(This,hDC,rcDraw) ) 

#define IChartDrawer_GetToolTipValue(This,nID,ptMiceInDC,pX,pY,bYfromX,nHalfHotZone)	\
    ( (This)->lpVtbl -> GetToolTipValue(This,nID,ptMiceInDC,pX,pY,bYfromX,nHalfHotZone) ) 

#define IChartDrawer_SmoothChart(This,nID,fSmoothParam,bYFromX,dwFlag)	\
    ( (This)->lpVtbl -> SmoothChart(This,nID,fSmoothParam,bYFromX,dwFlag) ) 

#define IChartDrawer_ClearAll(This)	\
    ( (This)->lpVtbl -> ClearAll(This) ) 

#define IChartDrawer_LogarifmScale(This,dwFlag)	\
    ( (This)->lpVtbl -> LogarifmScale(This,dwFlag) ) 

#define IChartDrawer_AddMarker(This,fX,fY,nPixelInSize,clColor,dwFlag)	\
    ( (This)->lpVtbl -> AddMarker(This,fX,fY,nPixelInSize,clColor,dwFlag) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IChartDrawer_INTERFACE_DEFINED__ */


#ifndef __ISmartChartDrawer_INTERFACE_DEFINED__
#define __ISmartChartDrawer_INTERFACE_DEFINED__

/* interface ISmartChartDrawer */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_ISmartChartDrawer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4810C3EF-2090-4128-8D70-ED1780D223ED")
    ISmartChartDrawer : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Draw( 
            /* [in] */ DWORD hDC,
            /* [in] */ RECT rcDraw) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetToolTipValue( 
            /* [in] */ long nID,
            /* [in] */ POINT ptMiceInDC,
            /* [out] */ double *pX,
            /* [out] */ double *pY,
            /* [in] */ short bYfromX,
            /* [in] */ int nHalfHotZone) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SmoothChart( 
            /* [in] */ long nID,
            /* [in] */ double fSmoothParam,
            /* [in] */ short bYFromX,
            /* [in] */ ChartLineSmooth dwFlag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CalcValues( 
            /* [in] */ long nID,
            /* [in] */ BSTR bstrExpr,
            /* [in] */ double fMin,
            /* [in] */ double fMax,
            /* [in] */ double fStep) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ClearAll( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LogarifmScale( 
            /* [in] */ ChartDataType dwFlag) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ISmartChartDrawerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISmartChartDrawer * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISmartChartDrawer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISmartChartDrawer * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Draw )( 
            ISmartChartDrawer * This,
            /* [in] */ DWORD hDC,
            /* [in] */ RECT rcDraw);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetToolTipValue )( 
            ISmartChartDrawer * This,
            /* [in] */ long nID,
            /* [in] */ POINT ptMiceInDC,
            /* [out] */ double *pX,
            /* [out] */ double *pY,
            /* [in] */ short bYfromX,
            /* [in] */ int nHalfHotZone);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SmoothChart )( 
            ISmartChartDrawer * This,
            /* [in] */ long nID,
            /* [in] */ double fSmoothParam,
            /* [in] */ short bYFromX,
            /* [in] */ ChartLineSmooth dwFlag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CalcValues )( 
            ISmartChartDrawer * This,
            /* [in] */ long nID,
            /* [in] */ BSTR bstrExpr,
            /* [in] */ double fMin,
            /* [in] */ double fMax,
            /* [in] */ double fStep);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ClearAll )( 
            ISmartChartDrawer * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LogarifmScale )( 
            ISmartChartDrawer * This,
            /* [in] */ ChartDataType dwFlag);
        
        END_INTERFACE
    } ISmartChartDrawerVtbl;

    interface ISmartChartDrawer
    {
        CONST_VTBL struct ISmartChartDrawerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISmartChartDrawer_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISmartChartDrawer_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISmartChartDrawer_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISmartChartDrawer_Draw(This,hDC,rcDraw)	\
    ( (This)->lpVtbl -> Draw(This,hDC,rcDraw) ) 

#define ISmartChartDrawer_GetToolTipValue(This,nID,ptMiceInDC,pX,pY,bYfromX,nHalfHotZone)	\
    ( (This)->lpVtbl -> GetToolTipValue(This,nID,ptMiceInDC,pX,pY,bYfromX,nHalfHotZone) ) 

#define ISmartChartDrawer_SmoothChart(This,nID,fSmoothParam,bYFromX,dwFlag)	\
    ( (This)->lpVtbl -> SmoothChart(This,nID,fSmoothParam,bYFromX,dwFlag) ) 

#define ISmartChartDrawer_CalcValues(This,nID,bstrExpr,fMin,fMax,fStep)	\
    ( (This)->lpVtbl -> CalcValues(This,nID,bstrExpr,fMin,fMax,fStep) ) 

#define ISmartChartDrawer_ClearAll(This)	\
    ( (This)->lpVtbl -> ClearAll(This) ) 

#define ISmartChartDrawer_LogarifmScale(This,dwFlag)	\
    ( (This)->lpVtbl -> LogarifmScale(This,dwFlag) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISmartChartDrawer_INTERFACE_DEFINED__ */


#ifndef __IChartMiscHelper_INTERFACE_DEFINED__
#define __IChartMiscHelper_INTERFACE_DEFINED__

/* interface IChartMiscHelper */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IChartMiscHelper;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4C2B900F-7DE5-4da5-9113-F3C79D3F4A5B")
    IChartMiscHelper : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LPtoDP( 
            /* [in] */ DWORD hDC,
            /* [out] */ POINT *pt) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DPtoLP( 
            /* [in] */ DWORD hDC,
            /* [out] */ POINT *pt) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ConvertToDC( 
            /* [in] */ long nID,
            /* [in] */ RECT rcDraw,
            /* [in] */ double x,
            /* [in] */ double y,
            /* [retval][out] */ POINT *pt) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IChartMiscHelperVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IChartMiscHelper * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IChartMiscHelper * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IChartMiscHelper * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LPtoDP )( 
            IChartMiscHelper * This,
            /* [in] */ DWORD hDC,
            /* [out] */ POINT *pt);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DPtoLP )( 
            IChartMiscHelper * This,
            /* [in] */ DWORD hDC,
            /* [out] */ POINT *pt);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ConvertToDC )( 
            IChartMiscHelper * This,
            /* [in] */ long nID,
            /* [in] */ RECT rcDraw,
            /* [in] */ double x,
            /* [in] */ double y,
            /* [retval][out] */ POINT *pt);
        
        END_INTERFACE
    } IChartMiscHelperVtbl;

    interface IChartMiscHelper
    {
        CONST_VTBL struct IChartMiscHelperVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IChartMiscHelper_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IChartMiscHelper_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IChartMiscHelper_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IChartMiscHelper_LPtoDP(This,hDC,pt)	\
    ( (This)->lpVtbl -> LPtoDP(This,hDC,pt) ) 

#define IChartMiscHelper_DPtoLP(This,hDC,pt)	\
    ( (This)->lpVtbl -> DPtoLP(This,hDC,pt) ) 

#define IChartMiscHelper_ConvertToDC(This,nID,rcDraw,x,y,pt)	\
    ( (This)->lpVtbl -> ConvertToDC(This,nID,rcDraw,x,y,pt) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IChartMiscHelper_INTERFACE_DEFINED__ */



#ifndef __ChartsLib_LIBRARY_DEFINED__
#define __ChartsLib_LIBRARY_DEFINED__

/* library ChartsLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_ChartsLib;

EXTERN_C const CLSID CLSID_ChartDrawer;

#ifdef __cplusplus

class DECLSPEC_UUID("B7D2C875-8C05-46d4-9BE0-2B47E6B98668")
ChartDrawer;
#endif
#endif /* __ChartsLib_LIBRARY_DEFINED__ */

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


