#pragma once

#include "DrvPreview.h"
#include "DllInfo.h"
#include "fgdrv.h"

#define std_dll_c	extern "C" __declspec(dllexport) 


void _addsubdir( char *pszResName, const char *szName, const char *szSubdirName);
void _deletesubdir( char *pszResName, const char *szName, const int iCount );
bool _isfileexist( const char *szFileName );
void AjustHelpFileName( CWinApp *papp );
void DoGrabImage(CDriverPreview *pDrv, CInvokeStruct *pInvokeStruct, void **pArguments, void *pResult, void **pErrInfo);

class CDriver;
class CVT4Drv : public CFGDrvAVIBase
{
	CDriver *m_pDrvVT5;
public:
	static CVT4Drv *s_pDrv;
	CVT4Drv();
	virtual ~CVT4Drv();

	virtual BOOL fgInit();
	virtual const char *GetFunctionName();
	virtual BOOL fgIsHdwPresent();
	virtual UINT fgGetHdwFlags();
	// Returns information, saved during capability negotiation.
	virtual int fgGetFormatDescr( FGFormatDescr **fd ); // returns number of formats
	virtual int fgGetFormat();
	virtual void fgSetFormat( int iNum );
	virtual BYTE * fgGetLine( short LineNum );
	virtual BOOL fgGetBrContrLimits( int *iBrMin, int *iBrMax, int *iCntrMin, int *iCntrMax );
	virtual BOOL fgSetBrContr( int iBr, int iContr, BOOL bAuto );
	virtual BOOL fgGetBrContr( int *iBr, int *iContr );
	virtual void fgSetExposTime( int iTime );
	virtual int fgGetExposTime();
	virtual void fgForse8bit( BOOL bForce );
	virtual BOOL fgIsForsed8bit();
	virtual LPCSTR fgGetDeviceName();
	virtual void fgSetUIType(int nType);
	virtual void fgSetXferType(int nType);
	virtual BOOL fgIsReverseBits();

	// Only if GetAquisMech has return Permanently, else setup
	// dialog box already will be created on begin of function.
	// Data source select dialog box will be created inside fgInit.
	virtual HWND fgCreateSetupDialog( HWND hWndParent, BOOL bModal );
	virtual int CaptureAVI(char *pszNameBuffer, int nBuffSize);
	// Here both DS and DSM will be closed.
	virtual void fgExit();
	virtual void fgRelease();


	virtual CDriver *GetDriverVT5();
	virtual CDriver *CreateDriverVT5() {return NULL;}

	static BOOL s_bVT5Settings;
};

extern CIntFuncInfo FInfo;
extern CImageArg res1;
extern CErrorInfo errUserBreak;

#define DEFINE_VT4_FUNCTIONS() std_dll_c	unsigned DllGetFunctionsCount()\
{\
	return 1;\
}\
\
std_dll_c	unsigned DllGetFunctionInfo(unsigned nFuncNo, CIntFuncInfo *pBuffer)\
{\
	if (nFuncNo>=1) return 0xFFFFFFFF;\
	if (!pBuffer||pBuffer->m_nSize!=sizeof(CIntFuncInfo)) return 0;\
\
	memcpy(pBuffer, &FInfo, pBuffer->m_nSize );\
\
	pBuffer->m_nSize = sizeof(CIntFuncInfo);\
	return 0;\
}\
\
std_dll_c	unsigned DllGetFuncArgsCount(unsigned nFuncNo)\
{\
	return 0;\
}\
\
std_dll_c	CArgInfo *DllGetFuncArgInfo(unsigned nFuncNo, unsigned nArgNo)\
{\
	return NULL;\
}\
\
std_dll_c	CArgInfo *DllGetFuncResultInfo(unsigned nFuncNo)\
{\
	return (CArgInfo *)&res1;\
}\
\
extern "C" BOOL _declspec(dllexport) fgInit(HWND hWnd, CFGDrvBase **ptab, int *num,	int mx)\
{\
	if (*num >= mx) return FALSE;\
	ptab[*num] = CVT4Drv::s_pDrv;\
	(*num)++;\
	return TRUE;\
}\
\
extern "C" void _declspec(dllexport) fgExit()\
{\
}



