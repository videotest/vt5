/*****************************************************************************\
*    Program:             BxLib2M                                             *
*    Version:                                                                 *
*    Module:              BxLibApiWrapper.cpp                                 *
*    Compiler:            MS Visual C++ 6.0                                   *
*    Operating System:    Windows NT/2000/XP & Windows 98                     *
*    Purpose:                                                                 *
*                                                                             *
*    Notes:                                                                   *
*    Created:             08.04.2002                                          *
*    Last modified:       24.05.2002                                          *
*    Author:              M.Pester											  *
*    Copyright:           © Baumer Optronic GmbH, Radeberg                    *
\*****************************************************************************/
// mp 24.05.2002 wrapBX_GetBmpDirect  add allocMemSize - Parameter 

#include "StdAfx.h"
#include "BxLibApiWrapper.h"

static HINSTANCE hDLL = NULL;       // Handle to DLL 

int AttachBxLib() 
{
	if (hDLL == NULL )
		hDLL = LoadLibrary(BXDLL_NAME_TO_WRAP);
	return hDLL ? TRUE : FALSE;
}

int DetachBxLib()
{	
	int ret = FreeLibrary( hDLL ); 
	hDLL = NULL;
	return ret; 
}

unsigned char * wrapBX_GetVersion( )
{ 
	pBX_GetVersion lpF = (pBX_GetVersion)GetProcAddress( hDLL, "BX_GetVersion");
	if( lpF )	return lpF( );
	else		return NULL;
}

void  wrapBX_SetMsgOutput( int flags )
{ 
	pBX_SetMsgOutput lpF = (pBX_SetMsgOutput)GetProcAddress( hDLL, "BX_SetMsgOutput");
	if( lpF )	lpF( flags );
}

int   wrapBX_GetErrorCode( )
{ 
	pBX_GetErrorCode lpF = (pBX_GetErrorCode)GetProcAddress( hDLL, "BX_GetErrorCode");
	if( lpF )	return lpF( );
	else		return 0;
}

int   wrapBX_CameraTable(  tDcName ** table )
{ 
	pBX_CameraTable lpF = (pBX_CameraTable)GetProcAddress( hDLL, "BX_CameraTable");
	if( lpF )	return lpF( table );
	else		return 0;
}

int   wrapBX_CameraTableInstalled(  tDcName ** table )
{ 
	pBX_CameraTable lpF = (pBX_CameraTableInstalled)GetProcAddress( hDLL, "BX_CameraTableInstalled");
	if( lpF )	return lpF( table );
	else		return 0;
}

int   wrapBX_RegCameraByDesc(  int iCamAnz, ptDefineDigitalCamera pDefCam, int bClearRegistry )
{ 
	pBX_RegCameraByDesc lpF = (pBX_RegCameraByDesc)GetProcAddress( hDLL, "BX_RegCameraByDesc");
	if( lpF )	return lpF( iCamAnz,  pDefCam, bClearRegistry );
	else		return 0;
}

int   wrapBX_CameraSystemManager( CamSysInfoProc pCallback  )
{ 
	pBX_CameraSystemManager lpF = (pBX_CameraSystemManager)GetProcAddress( hDLL, "BX_CameraSystemManager");
	if( lpF )	return lpF( pCallback );
	else		return 0;
}

int   wrapBX_InitCamSys(  )
{ 
	pBX_InitCamSys lpF = (pBX_InitCamSys)GetProcAddress( hDLL, "BX_InitCamSys");
	if( lpF )	return lpF( );
	else		return 0;
}

int   wrapBX_DeInitLibrary(  )
{ 
	pBX_DeInitLibrary lpF = (pBX_DeInitLibrary)GetProcAddress( hDLL, "BX_DeInitLibrary");
	if( lpF )	return lpF( );
	else		return 0;
}

int   wrapBX_InitCamByNr( int nr )
{ 
	pBX_InitCamByNr lpF = (pBX_InitCamByNr)GetProcAddress( hDLL, "BX_InitCamByNr");
	if( lpF )	return lpF( nr );
	else		return 0;
}

int   wrapBX_InitCamByNrAndType( int nr, int type )
{ 
	pBX_InitCamByNrAndType lpF = (pBX_InitCamByNrAndType)GetProcAddress( hDLL, "BX_InitCamByNrAndType");
	if( lpF )	return lpF( nr, type );
	else		return 0;
}

#include <stdio.h>
int wrapBX_EnumeratePciDevices( tpBOIFCDEV DevAsk )
{ 
	pBX_EnumeratePciDevices lpF = (pBX_EnumeratePciDevices)GetProcAddress( hDLL, "BX_EnumeratePciDevices");
	if( lpF ) {
		int ret = lpF( DevAsk );
		return ret;
	}
	else		return 0;
}

int wrapBX_EnumerateCamSystem( int * piCamCount, ptDefineDigitalCamera * ppCam )
{ 
	pBX_EnumerateCamSystem lpF = (pBX_EnumerateCamSystem)GetProcAddress( hDLL, "BX_EnumerateCamSystem");
	if( lpF ) {
		int ret = lpF( piCamCount, ppCam );
		return ret;
	}
	else		return 0;
}

int wrapBX_SetScanMode( int fId, int sm,  PVOID pvmem  )
{ 
	pBX_SetScanMode lpF = (pBX_SetScanMode)GetProcAddress( hDLL, "BX_SetScanMode");
	if( lpF ) {
		int ret = lpF( fId, sm,  pvmem  );
		return ret;
	}
	else		return 0;
}

int wrapBX_StopScanMode( int fId )
{ 
	pBX_StopScanMode lpF = (pBX_StopScanMode)GetProcAddress( hDLL, "BX_StopScanMode");
	if( lpF ) {
		int ret = lpF( fId );
		return ret;
	}
	else		return 0;
}

int wrapBX_LoopControl( int fId, PLVMEM buffers, int freeBufNr )
{ 
	pBX_LoopControl lpF = (pBX_LoopControl)GetProcAddress( hDLL, "BX_LoopControl");
	if( lpF ) {
		int ret = lpF( fId, buffers, freeBufNr );
		return ret;
	}
	else		return 0;
}

int wrapBX_GetActiveLoopBuffer( int fId )
{ 
	pBX_GetActiveLoopBuffer lpF = (pBX_GetActiveLoopBuffer)GetProcAddress( hDLL, "BX_GetActiveLoopBuffer");
	if( lpF ) {
		int ret = lpF( fId );
		return ret;
	}
	else		return 0;
}

int wrapBX_LoopControlSMem( int fId, PSVMEM buffers, int freeBufNr )
{ 
	pBX_LoopControlSMem lpF = (pBX_LoopControlSMem)GetProcAddress( hDLL, "BX_LoopControlSMem");
	if( lpF ) {
		int ret = lpF( fId, buffers, freeBufNr );
		return ret;
	}
	else		return 0;
}

int wrapBX_GetActiveLoopBufferSMem( int fId )
{ 
	pBX_GetActiveLoopBufferSMem lpF = (pBX_GetActiveLoopBufferSMem)GetProcAddress( hDLL, "BX_GetActiveLoopBufferSMem");
	if( lpF ) {
		int ret = lpF( fId );
		return ret;
	}
	else		return 0;
}

int wrapBX_GetCameraInfo(int cLabel, tPBxCameraType T, int sizeT, tPBxCameraStatus S, int sizeS )
{ 
	pBX_GetCameraInfo lpF = (pBX_GetCameraInfo)GetProcAddress( hDLL, "BX_GetCameraInfo");
	if( lpF )	return lpF( cLabel, T,  sizeT,  S, sizeS  );
	else		return 0;
}

int wrapBX_SetDataWidth( int cLabel, ePIXWIDTH type )
{ 
	pBX_SetDataWidth lpF = (pBX_SetDataWidth)GetProcAddress( hDLL, "BX_SetDataWidth");
	if( lpF )	return lpF( cLabel, type );
	else		return 0;
}

int wrapBX_DmaImFormat( int cLabel, ptFormat pDmaImFormat ) 
{ 
	pBX_DmaImFormat lpF = (pBX_DmaImFormat)GetProcAddress( hDLL, "BX_DmaImFormat");
	if( lpF )	return lpF( cLabel, pDmaImFormat  );
	else		return 0;
}

int wrapBX_ResetCamera( int cLabel )
{ 
	pBX_ResetCamera lpF = (pBX_ResetCamera)GetProcAddress( hDLL, "BX_ResetCamera"); 
	if( lpF )	return lpF( cLabel );
	else		return 0;
}

int wrapBX_SwitchToCamera( int cLabel )
{ 
	pBX_SwitchToCamera lpF = (pBX_SwitchToCamera)GetProcAddress( hDLL, "BX_SwitchToCamera"); 
	if( lpF )	return lpF( cLabel );
	else		return 0;
}

int wrapBX_SetAutoExposure( int cLabel, int start, int bright )
{ 
	pBX_SetAutoExposure lpF = (pBX_SetAutoExposure)GetProcAddress( hDLL, "BX_SetAutoExposure"); 
	if( lpF )	return lpF( cLabel, start, bright );
	else		return 0;
}

int wrapBX_SetBlackOffset( int cLabel, int offset, ptBLOFFSET bloffset)
{ 
	pBX_SetBlackOffset lpF = (pBX_SetBlackOffset)GetProcAddress( hDLL, "BX_SetBlackOffset");
	if( lpF )	return lpF( cLabel, offset, bloffset );
	else		return 0;
}

int wrapBX_SetShutterTime( int cLabel, int tms, ptSHT sht)
{ 
	pBX_SetShutterTime lpF = (pBX_SetShutterTime)GetProcAddress( hDLL, "BX_SetShutterTime");
	if( lpF )	return lpF( cLabel, tms,  sht );
	else		return 0;
}

int wrapBX_SetGainFactor( int cLabel, double factor, ptGAIN gain)
{ 
	pBX_SetGainFactor lpF = (pBX_SetGainFactor)GetProcAddress( hDLL, "BX_SetGainFactor");
	if( lpF )	return lpF( cLabel, factor, gain );
	else		return 0;
}

int wrapBX_SetTriggerMode( int cLabel, BOOL set, tpCmdDesc c )
{ 
	pBX_SetTriggerMode lpF = (pBX_SetTriggerMode)GetProcAddress( hDLL, "BX_SetTriggerMode");
	if( lpF )	return lpF( cLabel, set, c );
	else		return 0;
}

int wrapBX_DoTrigger( int cLabel )
{ 
	pBX_DoTrigger lpF = (pBX_DoTrigger)GetProcAddress( hDLL, "BX_DoTrigger");
	if( lpF )	return lpF( cLabel );
	else		return 0;
}

int wrapBX_CamStart( int cLabel, int start  )
{ 
	pBX_CamStart lpF = (pBX_CamStart)GetProcAddress( hDLL, "BX_CamStart");
	if( lpF )	return lpF( cLabel, start  );
	else		return 0;
}

int wrapBX_SetPartialScan( int cLabel, BOOL enable, PRECT prect   )
{ 
	pBX_SetPartialScan lpF = (pBX_SetPartialScan)GetProcAddress( hDLL, "BX_SetPartialScan");
	if( lpF )	return lpF(  cLabel, enable, prect   );
	else		return 0;
}

int wrapBX_SetBinningMode( int cLabel, BOOL bSet, tpCmdDesc pCmdExt  )
{ 
	pBX_SetBinningMode lpF = (pBX_SetBinningMode)GetProcAddress( hDLL, "BX_SetBinningMode");
	if( lpF )	return lpF( cLabel,  bSet, pCmdExt  );
	else		return 0;
}

int wrapBX_CamAuxSettings( int cLabel, tpCmdDesc pCmdExt )
{ 
	pBX_CamAuxSettings lpF = (pBX_CamAuxSettings)GetProcAddress( hDLL, "BX_CamAuxSettings");
	if( lpF )	return lpF( cLabel, pCmdExt );
	else		return 0;
}

int wrapBX_CamIoSettings( int cLabel, tpCmdDesc pCmdExt )
{ 
	pBX_CamIoSettings lpF = (pBX_CamIoSettings)GetProcAddress( hDLL, "BX_CamIoSettings");
	if( lpF )	return lpF( cLabel, pCmdExt );
	else		return 0;
}

int wrapBX_CamFlashSettings( int cLabel,  int enable,tpCmdDesc pCmdExt )
{ 
	pBX_CamFlashSettings lpF = (pBX_CamFlashSettings)GetProcAddress( hDLL, "BX_CamFlashSettings");
	if( lpF )	return lpF( cLabel, enable, pCmdExt );
	else		return 0;
}

int wrapBX_SetFrameCounter( int cLabel, int enable, int set, WORD counter )
{ 
	pBX_SetFrameCounter lpF = (pBX_SetFrameCounter)GetProcAddress( hDLL, "BX_SetFrameCounter");
	if( lpF )	return lpF( cLabel, enable, set, counter );
	else		return 0;
}

int wrapBX_ControlCameraByCmdDesc( int cLabel, tpCmdDesc collect )
{ 
	pBX_ControlCameraByCmdDesc lpF = (pBX_ControlCameraByCmdDesc)GetProcAddress( hDLL, "BX_ControlCameraByCmdDesc");
	if( lpF )	return lpF( cLabel,  collect  );
	else		return 0;
}

int wrapBX_DoWhiteBalance( int cLabel, int enable, double * fr, double * fg, double * fb, RECT roi  )
{ 
	pBX_DoWhiteBalance lpF = (pBX_DoWhiteBalance)GetProcAddress( hDLL, "BX_DoWhiteBalance");
	if( lpF )	return lpF(  cLabel, enable, fr, fg, fb, roi  );
 	else		return 0;
}

int wrapBX_SetWhiteBalance(int cLabel, int enable, int set, double * fr, double * fg, double * fb  )
{ 
	pBX_SetWhiteBalance lpF = (pBX_SetWhiteBalance)GetProcAddress( hDLL, "BX_SetWhiteBalance");
	if( lpF )	return lpF( cLabel, enable, set, fr,  fg, fb );
 	else		return 0;
}

int wrapBX_SetRgbLut( int cLabel, int enable, int set, double * fr, double * fg, double * fb)
{ 
	pBX_SetRgbLut lpF = (pBX_SetRgbLut)GetProcAddress( hDLL, "BX_SetRgbLut");
	if( lpF )	return lpF( cLabel, enable,  set,  fr,  fg, fb  );
	else		return 0;
}

int wrapBX_SetGammaLut( int cLabel, BOOL bEnable, WORD awRed[4096], WORD awGreen[4096], WORD awBlue[4096] )
{ 
	pBX_SetGammaLut lpF = (pBX_SetGammaLut)GetProcAddress( hDLL, "BX_SetGammaLut");
	if( lpF )	return lpF(  cLabel,  bEnable,  awRed,  awGreen,  awBlue );
	else		return 0;
}

int wrapBX_SetMacbethColors(int cLabel, COLORREF acrMacbeth[24], double adblMatrix[4][4] ) 
{ 
	pBX_SetMacbethColors lpF = (pBX_SetMacbethColors)GetProcAddress( hDLL, "BX_SetMacbethColors");
	if( lpF )	return lpF( cLabel, acrMacbeth, adblMatrix );
 	else		return 0;
}

int wrapBX_SetTransformationMatrix(int cLabel, double adblMatrix[4][4] )
{ 
	pBX_SetTransformationMatrix lpF = (pBX_SetTransformationMatrix)GetProcAddress( hDLL, "BX_SetTransformationMatrix");
	if( lpF )	return lpF( cLabel, adblMatrix );
 	else		return 0;
}

int wrapBX_EnableTransformationMatrix(int cLabel, int enable )
{ 
	pBX_EnableTransformationMatrix lpF = (pBX_EnableTransformationMatrix)GetProcAddress( hDLL, "BX_EnableTransformationMatrix");
	if( lpF )	return lpF(  cLabel, enable );
 	else		return 0;
}

int wrapBX_GetCurrentImNumber( int src, int cLabel )
{ 
	pBX_GetCurrentImNumber lpF = (pBX_GetCurrentImNumber)GetProcAddress( hDLL, "BX_GetCurrentImNumber");
	if( lpF )	return lpF( src, cLabel );
	else		return 0;
}

PBYTE wrapBX_GetDataPtr( int cLabel  )
{ 
	pBX_GetDataPtr lpF = (pBX_GetDataPtr)GetProcAddress( hDLL, "BX_GetDataPtr");
	if( lpF )	return lpF(  cLabel );
	else		return 0;
}

PBYTE wrapBX_GetDataPtrByIndex( int cLabel, int iIndex  )
{ 
	pBX_GetDataPtrByIndex lpF = (pBX_GetDataPtrByIndex)GetProcAddress( hDLL, "BX_GetDataPtrByIndex");
	if( lpF )	return lpF(  cLabel, iIndex );
	else		return 0;
}

int wrapBX_GetBmpDirect( int cLabel, PBITMAPFILEHEADER pBmf, PBITMAPINFOHEADER pBmi, PBYTE  pBmp, DWORD allocMemSize, int modus )
{ 
	pBX_GetBmpDirect lpF = (pBX_GetBmpDirect)GetProcAddress( hDLL, "BX_GetBmpDirect");
	if( lpF )	return lpF( cLabel, pBmf, pBmi, pBmp, allocMemSize, modus );
	else		return 0;
}

int wrapBX_DefineImageNotificationEvent( int src, int cLabel, HANDLE event  )
{ 
	pBX_DefineImageNotificationEvent lpF = (pBX_DefineImageNotificationEvent)GetProcAddress( hDLL, "BX_DefineImageNotificationEvent");
	if( lpF )	return lpF( src, cLabel, event  );
	else		return 0;
}

int wrapBX_GetBmp( int cLabel, PVOID bp, int modus )
{ 
	pBX_GetBmp lpF = (pBX_GetBmp)GetProcAddress( hDLL, "BX_GetBmp");
	if( lpF )	return lpF( cLabel, bp, modus );
	else		return 0;
}

HGLOBAL wrapBX_GetBmpHandle( int cLabel, int modus)
{ 
	pBX_GetBmpHandle lpF = (pBX_GetBmpHandle)GetProcAddress( hDLL, "BX_GetBmpHandle");
	if( lpF )	return lpF( cLabel, modus);
	else		return NULL;
}
HGLOBAL wrapBX_GetSnapShotBmp( int cLabel, DWORD CamSpecImFormatTyp,  tpCmdDesc cmdSnapSpec, ProgressProc pCallback, DWORD dwData )
{
	pBX_GetSnapShotBmp lpF = (pBX_GetSnapShotBmp)GetProcAddress( hDLL, "BX_GetSnapShotBmp");
	if( lpF )	return lpF( cLabel, CamSpecImFormatTyp, cmdSnapSpec,  pCallback, dwData);
	else		return NULL;
}

int wrapBX_EnableCamArbiter( int cLabel, int set )
{
	pBX_EnableCamArbiter lpF = (pBX_EnableCamArbiter)GetProcAddress( hDLL, "BX_EnableCamArbiter");
	if( lpF )	return lpF( cLabel, set );
	else		return 0;
}

int wrapBX_EnableIfcArbiter( int fId, int set )
{
	pBX_EnableIfcArbiter lpF = (pBX_EnableIfcArbiter)GetProcAddress( hDLL, "BX_EnableIfcArbiter");
	if( lpF )	return lpF( fId, set );
	else		return 0;
}

int wrapBX_IfcIo( int fId, tpIfcCmdDesc collect  )
{
	pBX_IfcIo lpF = (pBX_IfcIo)GetProcAddress( hDLL, "BX_IfcIo");
	if( lpF )	return lpF( fId, collect  );
	else		return 0;
}

int wrapBX_IfcFlashSignal( int fId, int enable, int invert )
{
	pBX_IfcFlashSignal lpF = (pBX_IfcFlashSignal)GetProcAddress( hDLL, "BX_IfcFlashSignal");
	if( lpF )	return lpF( fId, enable, invert );
	else		return 0;
}

int wrapBX_SetCooler( int cLabel, int enable )
{
	pBX_SetCooler lpF = (pBX_SetCooler)GetProcAddress( hDLL, "BX_SetCooler");
	if( lpF )	return lpF( cLabel, enable );
	else		return 0;
}
int wrapBX_SetExtImFormat( int cLabel, int mode, PVOID todo )
{
	pBX_SetExtImFormat lpF = (pBX_SetExtImFormat)GetProcAddress( hDLL, "BX_SetExtImFormat");
	if( lpF )	return lpF( cLabel, mode, todo  );
	else		return 0;
}

int wrapBX_GetHistogram(int cLabel, int aiHistogram[4096] )
{
	pBX_GetHistogram lpF = (pBX_GetHistogram)GetProcAddress( hDLL, "BX_GetHistogram");
	if( lpF )	return lpF( cLabel, aiHistogram  );
	else		return 0;
}

int wrapBX_EnumerateImFormats( int cLabel, int * piResCount, ptDefineImageFormat * ppRes  )
{
	pBX_EnumerateImFormats lpF = (pBX_EnumerateImFormats)GetProcAddress( hDLL, "BX_EnumerateImFormats");
	if( lpF )	return lpF( cLabel, piResCount, ppRes  );
	else		return 0;
}

int wrapBX_EnumerateBmpImFormats( int cLabel, int * piResCount, ptDefineImageFormat * ppRes  )
{
	pBX_EnumerateBmpImFormats lpF = (pBX_EnumerateBmpImFormats)GetProcAddress( hDLL, "BX_EnumerateBmpImFormats");
	if( lpF )	return lpF( cLabel, piResCount, ppRes  );
	else		return 0;
}

HANDLE wrapBX_ShowIfcMonitor( int show  )
{
	pBX_ShowIfcMonitor lpF = (pBX_ShowIfcMonitor)GetProcAddress( hDLL, "BX_ShowIfcMonitor");
	if( lpF )	return lpF( show );
	else		return NULL;
}

int wrapBX_ValidateCamSystem(  int * piCamCount, ptDefineDigitalCamera * ppCam)
{
	pBX_ValidateCamSystem lpF = (pBX_ValidateCamSystem)GetProcAddress( hDLL, "BX_ValidateCamSystem");
	if( lpF )	return lpF( piCamCount, ppCam  );
	else		return 0;
}

int wrapBX_ValidateCamSystemSD(  int * piCamCount, ptDefineDigitalCamera * ppCam)
{
	pBX_ValidateCamSystemSD lpF = (pBX_ValidateCamSystemSD)GetProcAddress( hDLL, "BX_ValidateCamSystemSD");
	if( lpF )	return lpF( piCamCount, ppCam  );
	else		return 0;
}

int wrapBX_WriteShadingTable( IN int cLabel, IN tpShading psShading )
{
	pBX_WriteShadingTable lpF = (pBX_WriteShadingTable)GetProcAddress( hDLL, "BX_WriteShadingTable");
	if( lpF )	return lpF( cLabel, psShading );
	else		return 0;
}

int wrapBX_ReadShadingTable( IN int cLabel, OUT tpShading psShading )
{
	pBX_ReadShadingTable lpF = (pBX_ReadShadingTable)GetProcAddress( hDLL, "BX_ReadShadingTable");
	if( lpF )	return lpF( cLabel, psShading  );
	else		return 0;
}

int wrapBX_SetShadingTable( int cLabel, int enable, tpSHD shading )
{
	pBX_SetShadingTable lpF = (pBX_SetShadingTable)GetProcAddress( hDLL, "BX_SetShadingTable");
	if( lpF )	return lpF( cLabel, enable, shading  );
	else		return 0;
}

int wrapBX_DeviceInfo( int cLabel,  tpBOBXCAM_INFO pDevInfo  )
{
	pBX_DeviceInfo lpF = (pBX_DeviceInfo)GetProcAddress( hDLL, "BX_DeviceInfo");
	if( lpF )	return lpF( cLabel,  pDevInfo  );
	else		return 0;
}

int wrapBX_SetDigOutput( int fId,  WORD output )
{
    pBX_SetDigOutput lpF = (pBX_SetDigOutput)GetProcAddress( hDLL, "BX_SetDigOutput");
	if( lpF )	return lpF( fId,   output );
 	else		return 0;
}

int wrapBX_GetDigInput(  int fId,  WORD* input )
{
    pBX_GetDigInput lpF = (pBX_GetDigInput)GetProcAddress( hDLL, "BX_GetDigInput");
	if( lpF )	return lpF( fId,   input );
 	else		return 0;
}






