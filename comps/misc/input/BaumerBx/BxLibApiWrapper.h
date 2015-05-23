/*****************************************************************************\
*    Program:             BxLib                                               *
*    Version:                                                                 *
*    Module:              BxLibApiWrapper.h                                   *
*    Compiler:            MS Visual C++ 6.0                                   *
*    Operating System:    Windows NT/2000/XP & Windows 98                     *
*    Purpose:             Use for explicite link with fxlib.dll               *
*                                                                             *
*    Notes:                                                                   *
*    Created:             08.04.2002                                          *
*    Last modified:       24.05.2002                                          *
*    Author:              M.Pester                                            *
*    Copyright:           © Baumer Optronic GmbH, Radeberg                    *
\*****************************************************************************/
// mp 24.05.2002 wrapBX_GetBmpDirect  add allocMemSize - Parameter 

#ifndef _BXLIB_API_WRAPPER_H_
#define _BXLIB_API_WRAPPER_H_

#include "Windows.h"
#ifdef	CXLIBINTERN
#undef	CXLIBINTERN
#define RESETCXLIBINTERN
#endif

#include "BxLibApi.h"

//#if _DEBUG
//#define  BXDLL_NAME_TO_WRAP _T("BxLibD.dll")
//#else
#define  BXDLL_NAME_TO_WRAP _T("BxLib.dll")
//#endif

int AttachBxLib(); 
int DetachBxLib(); 

typedef __declspec(dllimport) unsigned char * ( __stdcall  *pBX_GetVersion)( );
unsigned char * wrapBX_GetVersion( );

typedef __declspec(dllimport)void (__stdcall *pBX_SetMsgOutput)( int flags ); 
void wrapBX_SetMsgOutput( int flags );

typedef __declspec(dllimport)int (__stdcall *pBX_GetErrorCode)( );
int wrapBX_GetErrorCode( );

typedef __declspec(dllimport)int (__stdcall *pBX_CameraTable)(  tDcName ** table );
int wrapBX_CameraTable(  tDcName ** table );

typedef __declspec(dllimport)int (__stdcall *pBX_CameraTableInstalled)(  tDcName ** table );
int wrapBX_CameraTableInstalled(  tDcName ** table );

typedef __declspec(dllimport)int (__stdcall *pBX_RegCameraByDesc)( int iCamAnz, ptDefineDigitalCamera pDefCam, int bClearRegistry );
int wrapBX_RegCameraByDesc( int iCamAnz, ptDefineDigitalCamera pDefCam, int bClearRegistry );

typedef __declspec(dllimport)int (__stdcall *pBX_CameraSystemManager)(  CamSysInfoProc pCallback );
int wrapBX_CameraSystemManager( CamSysInfoProc pCallback );

typedef __declspec(dllimport)int (__stdcall *pBX_InitCamSys)(   );    
int wrapBX_InitCamSys(   ); 
   
typedef __declspec(dllimport)int (__stdcall *pBX_DeInitLibrary)(   );    
int wrapBX_DeInitLibrary(   );    

typedef __declspec(dllimport)int (__stdcall *pBX_InitCamByNr)( int nr  );
int wrapBX_InitCamByNr( int nr  );

typedef __declspec(dllimport)int (__stdcall *pBX_InitCamByNrAndType)( int nr, int type  );    
int wrapBX_InitCamByNrAndType( int nr, int type  );    

typedef __declspec(dllimport)int (__stdcall *pBX_EnumeratePciDevices)( tpBOIFCDEV DevAsk );    
int wrapBX_EnumeratePciDevices( tpBOIFCDEV DevAsk );     

typedef __declspec(dllimport)int (__stdcall *pBX_EnumerateCamSystem)( int * piCamCount, ptDefineDigitalCamera * ppCam);   
int wrapBX_EnumerateCamSystem( int * piCamCount, ptDefineDigitalCamera * ppCam); 

typedef __declspec(dllimport)int (__stdcall *pBX_SetScanMode)( int fId, int sm,  PVOID pvmem  );  
int wrapBX_SetScanMode( int fId, int sm,  PVOID pvmem  );

typedef __declspec(dllimport)int (__stdcall *pBX_StopScanMode)( int fId ); 
int wrapBX_StopScanMode( int fId );

typedef __declspec(dllimport)int (__stdcall *pBX_LoopControl)( int fId, PLVMEM buffers, int freeBufNr );
int wrapBX_LoopControl( int fId, PLVMEM buffers, int freeBufNr );

typedef __declspec(dllimport)int (__stdcall *pBX_GetActiveLoopBuffer)( int fId  );
int wrapBX_GetActiveLoopBuffer( int fId  );

typedef __declspec(dllimport)int (__stdcall *pBX_LoopControlSMem)( int fId, PSVMEM buffers, int freeBufNr );
int wrapBX_LoopControlSMem( int fId, PSVMEM buffers, int freeBufNr );

typedef __declspec(dllimport)int (__stdcall *pBX_GetActiveLoopBufferSMem)( int fId  );
int wrapBX_GetActiveLoopBufferSMem( int fId  );

typedef __declspec(dllimport)int (__stdcall *pBX_GetCameraInfo)( int cLabel, tPBxCameraType T, int sizeT, tPBxCameraStatus S, int sizeS  );
int wrapBX_GetCameraInfo(int cLabel, tPBxCameraType T, int sizeT, tPBxCameraStatus S, int sizeS );

typedef __declspec(dllimport)int (__stdcall *pBX_SetDataWidth)( int cLabel, ePIXWIDTH type );
int wrapBX_SetDataWidth( int cLabel, ePIXWIDTH type );

typedef __declspec(dllimport)int (__stdcall *pBX_DmaImFormat)(  int cLabel, ptFormat pDmaImFormat );
int wrapBX_DmaImFormat( int cLabel, ptFormat pDmaImFormat );

typedef __declspec(dllimport)int (__stdcall *pBX_ResetCamera)( int cLabel );
int wrapBX_ResetCamera( int cLabel );

typedef __declspec(dllimport)int (__stdcall *pBX_SetBlackOffset)( int cLabel, int offset, ptBLOFFSET bloffset );
int wrapBX_SetBlackOffset( int cLabel, int offset, ptBLOFFSET bloffset);

typedef __declspec(dllimport)int (__stdcall *pBX_SetShutterTime)( int cLabel, int tms, ptSHT sht );
int wrapBX_SetShutterTime( int cLabel, int tms, ptSHT sht);

typedef __declspec(dllimport)int (__stdcall *pBX_SetGainFactor)( int cLabel, double factor, ptGAIN gain );
int wrapBX_SetGainFactor( int cLabel, double factor, ptGAIN gain);

typedef __declspec(dllimport)int (__stdcall *pBX_SetTriggerMode)( int cLabel, BOOL set, tpCmdDesc c );
int wrapBX_SetTriggerMode( int cLabel, BOOL set, tpCmdDesc c );

typedef __declspec(dllimport)int (__stdcall *pBX_EnableCamArbiter)( int cLabel, int set );
int wrapBX_EnableCamArbiter( int cLabel, int set );

typedef __declspec(dllimport)int (__stdcall *pBX_DoTrigger)( int cLabel );
int wrapBX_DoTrigger( int cLabel );

typedef __declspec(dllimport)int (__stdcall *pBX_CamStart)( int cLabel, int start );
int wrapBX_CamStart( int cLabel, int start );

typedef __declspec(dllimport)int (__stdcall *pBX_SetBinningMode)( int cLabel, BOOL bSet, tpCmdDesc pCmdExt  );
int wrapBX_SetBinningMode( int cLabel, BOOL bSet, tpCmdDesc pCmdExt  );

typedef __declspec(dllimport)int (__stdcall *pBX_SetPartialScan)( int cLabel, BOOL enable, PRECT prect   );
int wrapBX_SetPartialScan( int cLabel, BOOL enable, PRECT prect   );

typedef __declspec(dllimport)int (__stdcall *pBX_SetExtImFormat)( int cLabel, int mode, PVOID todo );
int wrapBX_SetExtImFormat( int cLabel, int mode, PVOID todo );

typedef __declspec(dllimport)int (__stdcall *pBX_CamAuxSettings)( int cLabel, tpCmdDesc pCmdExt );
int wrapBX_CamAuxSettings( int cLabel, tpCmdDesc pCmdExt );

typedef __declspec(dllimport)int (__stdcall *pBX_CamIoSettings)( int cLabel, tpCmdDesc pCmdExt );
int wrapBX_CamIoSettings( int cLabel, tpCmdDesc pCmdExt );

typedef __declspec(dllimport)int (__stdcall *pBX_CamFlashSettings)( int cLabel, int enable, tpCmdDesc pCmdExt );
int wrapBX_CamFlashSettings( int cLabel,  int enable,tpCmdDesc pCmdExt );

typedef __declspec(dllimport)int (__stdcall *pBX_SetFrameCounter)( int cLabel, int enable, int set, WORD counter );
int wrapBX_SetFrameCounter( int cLabel, int enable, int set, WORD counter );

typedef __declspec(dllimport)int (__stdcall *pBX_ControlCameraByCmdDesc)(  int cLabel, tpCmdDesc collect );
int wrapBX_ControlCameraByCmdDesc( int cLabel, tpCmdDesc collect );

typedef __declspec(dllimport)int (__stdcall *pBX_SwitchToCamera)( int cLabel  ); 
int wrapBX_SwitchToCamera( int cLabel  ); 

typedef __declspec(dllimport)int (__stdcall *pBX_EnableIfcArbiter)( int fId, int set );
int wrapBX_EnableIfcArbiter( int fId, int set );

typedef __declspec(dllimport)int (__stdcall *pBX_IfcIo)( int fId, tpIfcCmdDesc collect  );   
int wrapBX_IfcIo( int fId, tpIfcCmdDesc collect  );   

typedef __declspec(dllimport)int (__stdcall *pBX_IfcFlashSignal)( int fId, int enable, int invert  ); 
int wrapBX_IfcFlashSignal( int fId, int enable, int invert  ); 

typedef __declspec(dllimport)HANDLE (__stdcall *pBX_ShowIfcMonitor)( int bShow );
HANDLE wrapBX_ShowIfcMonitor( int bShow );

typedef __declspec(dllimport)int (__stdcall *pBX_SetAutoExposure)( int cLabel, int start, int bright );
int wrapBX_SetAutoExposure( int cLabel, int start, int bright );

typedef __declspec(dllimport)int (__stdcall *pBX_DoWhiteBalance)(  int cLabel, int enable, double * fr, double * fg, double * fb, RECT roi  );
int wrapBX_DoWhiteBalance( int cLabel, int enable, double * fr, double * fg, double * fb, RECT roi  );

typedef __declspec(dllimport)int (__stdcall *pBX_SetWhiteBalance)(  int cLabel, int enable, int set, double * fr, double * fg, double * fb  );
int wrapBX_SetWhiteBalance(int cLabel, int enable, int set, double * fr, double * fg, double * fb  );

typedef __declspec(dllimport)int (__stdcall *pBX_SetRgbLut)(  int cLabel, int enable, int set, double * fr, double * fg, double * fb );
int wrapBX_SetRgbLut( int cLabel, int enable, int set, double * fr, double * fg, double * fb);

typedef __declspec(dllimport)int (__stdcall *pBX_SetGammaLut)(  int cLabel, BOOL bEnable, WORD awRed[4096], WORD awGreen[4096], WORD awBlue[4096] );
int wrapBX_SetGammaLut( int cLabel, BOOL bEnable, WORD awRed[4096], WORD awGreen[4096], WORD awBlue[4096] );

typedef __declspec(dllimport)int (__stdcall *pBX_SetMacbethColors)(  int cLabel, COLORREF acrMacbeth[24], double adblMatrix[4][4] );
int wrapBX_SetMacbethColors(int cLabel, COLORREF acrMacbeth[24], double adblMatrix[4][4] );

typedef __declspec(dllimport)int (__stdcall *pBX_SetTransformationMatrix)(  int cLabel, double adblMatrix[4][4] );
int wrapBX_SetTransformationMatrix(int cLabel, double adblMatrix[4][4] );

typedef __declspec(dllimport)int (__stdcall *pBX_EnableTransformationMatrix)(  int cLabel, int enable );
int wrapBX_EnableTransformationMatrix(int cLabel, int enable );

typedef __declspec(dllimport)int (__stdcall *pBX_DefineImageNotificationEvent)( int src, int cLabel, HANDLE event  );
int wrapBX_DefineImageNotificationEvent( int src, int cLabel, HANDLE event  );

typedef __declspec(dllimport)int (__stdcall *pBX_GetCurrentImNumber)( int src, int cLabel );
int wrapBX_GetCurrentImNumber( int src, int cLabel  );

typedef __declspec(dllimport)PBYTE (__stdcall *pBX_GetDataPtr)( int fId );
PBYTE wrapBX_GetDataPtr( int fId );

typedef __declspec(dllimport)PBYTE (__stdcall *pBX_GetDataPtrByIndex)( int fId, int iIndex );
PBYTE wrapBX_GetDataPtrByIndex( int fId, int iIndex  );

typedef __declspec(dllimport)int (__stdcall *pBX_GetBmpDirect)( int cLabel, PBITMAPFILEHEADER pBmf, PBITMAPINFOHEADER pBmi, PBYTE  pBmp, DWORD allocMemSize, int modus);
int wrapBX_GetBmpDirect( int cLabel, PBITMAPFILEHEADER pBmf, PBITMAPINFOHEADER pBmi, PBYTE  pBmp, DWORD allocMemSize, int modus );

typedef __declspec(dllimport)int (__stdcall *pBX_GetBmp)( int cLabel, PVOID bp, int modus );
int wrapBX_GetBmp( int cLabel, PVOID bp, int modus );

typedef __declspec(dllimport)HGLOBAL (__stdcall *pBX_GetBmpHandle)( int cLabel, int modus );
HGLOBAL wrapBX_GetBmpHandle( int cLabel, int modus);

typedef __declspec(dllimport)HGLOBAL (__stdcall *pBX_GetSnapShotBmp)( int cLabel, DWORD CamSpecImFormatTyp,  tpCmdDesc cmdSnapSpec, ProgressProc pCallback, DWORD dwData );
HGLOBAL wrapBX_GetSnapShotBmp( int cLabel, DWORD CamSpecImFormatTyp,  tpCmdDesc cmdSnapSpec, ProgressProc pCallback, DWORD dwData );

typedef __declspec(dllimport)int (__stdcall *pBX_GetHistogram)(int cLabel, int aiHistogram[4096] );
int wrapBX_GetHistogram(int cLabel, int aiHistogram[4096] ); 

typedef __declspec(dllimport)int (__stdcall *pBX_SetCooler)( int cLabel, int enable );
int wrapBX_SetCooler( int cLabel, int enable ); 

typedef __declspec(dllimport)int (__stdcall *pBX_SetExtImFormat)( int cLabel, int mode, PVOID todo );
int wrapBX_SetExtImFormat( int cLabel, int mode, PVOID todo  ); 

typedef __declspec(dllimport)int (__stdcall *pBX_EnumerateImFormats)( int cLabel, int * piResCount, ptDefineImageFormat * ppRes );
int wrapBX_EnumerateImFormats( int cLabel, int * piResCount, ptDefineImageFormat * ppRes  ); 

typedef __declspec(dllimport)int (__stdcall *pBX_EnumerateBmpImFormats)( int cLabel, int * piResCount, ptDefineImageFormat * ppRes );
int wrapBX_EnumerateBmpImFormats( int cLabel, int * piResCount, ptDefineImageFormat * ppRes  ); 

typedef __declspec(dllimport)int (__stdcall *pBX_ValidateCamSystem)( int * piCamCount, ptDefineDigitalCamera * ppCam );
int wrapBX_ValidateCamSystem(  int * piCamCount, ptDefineDigitalCamera * ppCam);

typedef __declspec(dllimport)int (__stdcall *pBX_ValidateCamSystemSD)( int * piCamCount, ptDefineDigitalCamera * ppCam );
int wrapBX_ValidateCamSystemSD(  int * piCamCount, ptDefineDigitalCamera * ppCam);

int wrapBX_WriteShadingTable( IN int cLabel, IN tpShading psShading );
typedef __declspec(dllimport)int (__stdcall *pBX_WriteShadingTable)( IN int cLabel, IN tpShading psShading  );

int wrapBX_ReadShadingTable( IN int cLabel, OUT tpShading psShading );
typedef __declspec(dllimport)int (__stdcall *pBX_ReadShadingTable)( IN int cLabel, OUT tpShading psShading );

int wrapBX_SetShadingTable( int cLabel, int enable, tpSHD shading  );
typedef __declspec(dllimport)int (__stdcall *pBX_SetShadingTable)( int cLabel, int enable, tpSHD shading  );

int wrapBX_DeviceInfo( int cLabel,  tpBOBXCAM_INFO pDevInfo  );  
typedef __declspec(dllimport)int (__stdcall *pBX_DeviceInfo)( int cLabel,  tpBOBXCAM_INFO pDevInfo );


int wrapBX_SetDigOutput( int fId,  WORD output );
typedef __declspec(dllimport)int (__stdcall *pBX_SetDigOutput)( int ifcLabel,  WORD  output );

int wrapBX_GetDigInput(  int fId,  WORD* input );
typedef __declspec(dllimport)int (__stdcall *pBX_GetDigInput)( int ifcLabel,  WORD* output );


#ifdef	RESETCXLIBINTERN
#define CXLIBINTERN
#endif

#endif // _BXLIB_API_WRAPPER_H_