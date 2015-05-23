#include "StdAfx.h"
#include "Mv5Drv.h"
#include "DllInfo.h"
#include "MV500Grab.h"
#include "PreviewDialog.h"
#include "StdProfile.h"
#include "FgDrv.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

class CMV500Drv : public CFGDrvAVIBase  
{
public:
	CMV500Drv();
	virtual ~CMV500Drv();

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
};

FGFormatDescr fds[] =
{
	{640,480,TRUE,"NTSC"},
	{768,576,TRUE,"PAL"},
	{0,0,FALSE,""},
};

CMV500Drv g_MV500Drv;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMV500Drv::CMV500Drv()
{

}

CMV500Drv::~CMV500Drv()
{

}


extern "C" BOOL _declspec(dllexport) fgInit(HWND hWnd, CFGDrvBase **ptab, int *num,
	int mx)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (*num >= mx) return FALSE;
	if (!CMV500Grab::s_pGrab->TestHardware()) return FALSE;
	CMV500Grab::s_pGrab->Preinit();
	ptab[*num] = &g_MV500Drv;
	(*num)++;
	return TRUE;
}

extern "C" void _declspec(dllexport) fgExit()
{
}


BOOL CMV500Drv::fgInit()
{
	return TRUE;
}

void CMV500Drv::fgExit()
{
//	DeinitHardware();
}

BOOL CMV500Drv::fgIsHdwPresent()
{
//	InitHardware();
	return TRUE;//m_bHdwOK;
}

UINT CMV500Drv::fgGetHdwFlags()
{
	return FG_HASBRCONTR|FG_HASSETUPDLG|FG_UICONTROLLABLE|FG_SUPPORTSAVI;
}

int CMV500Drv::fgGetFormatDescr( FGFormatDescr **fd )
{
	*fd = fds;
	return 2;
}

void CMV500Drv::fgSetFormat(int iNum)
{
	if (iNum == 0 || iNum == 1)
		CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("VideoStandard"), iNum);
}

BYTE *CMV500Drv::fgGetLine( short LineNum )
{
	return NULL;
}

HWND CMV500Drv::fgCreateSetupDialog( HWND hWndParent, BOOL bModal )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_AFX_THREAD_STATE *pThreadState = AfxGetThreadState();
	pThreadState->m_pRoutingFrame = NULL;
	CPreviewDialog dlg(0);
	dlg.DoModal();
	return NULL;
}

BOOL CMV500Drv::fgGetBrContrLimits( int *iBrMin, int *iBrMax, int *iCntrMin, int *iCntrMax )
{
	*iBrMin = *iCntrMin = 0;
	*iBrMax = *iCntrMax = 255;
	return TRUE;
}

BOOL CMV500Drv::fgGetBrContr( int *iBr, int *iContr )
{
	*iBr = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("Brightness"), 128);
	*iContr = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("Contrast"), 128);
	return FALSE;
}

BOOL CMV500Drv::fgSetBrContr( int iBr, int iContr, BOOL bAuto )
{
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("Brightness"), iBr);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("Contrast"), iContr);
	return FALSE;
}

LPCSTR CMV500Drv::fgGetDeviceName()
{
	return "Mutech MV-500/510";
}

int CMV500Drv::fgGetFormat()
{
	BOOL bPal = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"),_T("VideoStandard"),0);
	return bPal;
}

void CMV500Drv::fgSetExposTime( int iTime )
{
}

int CMV500Drv::fgGetExposTime()
{
	return 0;
}

void CMV500Drv::fgForse8bit( BOOL bForce )
{
}

BOOL CMV500Drv::fgIsForsed8bit()
{
	return TRUE;
}

void CMV500Drv::fgSetUIType(int nType)
{
}

void CMV500Drv::fgSetXferType(int nType)
{
}

BOOL CMV500Drv::fgIsReverseBits()
{
	return FALSE;
}

void CMV500Drv::fgRelease()
{
}

const char *CMV500Drv::GetFunctionName()
{
	return "MV500GrabImage";
}

int CMV500Drv::CaptureAVI(char *pszNameBuffer, int nBuffSize)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_AFX_THREAD_STATE *pThreadState = AfxGetThreadState();
	pThreadState->m_pRoutingFrame = NULL;
	CPreviewDialog dlg(0);
	dlg.m_WindowType = CPreviewDialog::ForAVICapture;
	int r = dlg.DoModal();
	if (r == IDOK)
		strncpy(pszNameBuffer, (const char *)dlg.m_strAVI, nBuffSize);
	return r==IDOK?0:-1;
}

static LPBITMAPINFOHEADER s_lpbiLast;
std_dll_c void MV500GrabImage( CInvokeStruct *pInvokeStruct, void **pArguments, void *pResult, void **pErrInfo )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CDIBArgument *pOutImage = (CDIBArgument *)pResult;
	pOutImage->m_fsOpts = 0;
	if (s_lpbiLast)
	{
		free(s_lpbiLast);
		s_lpbiLast = NULL;
	}
	bool bSilent = true;
	if (pInvokeStruct->m_pszTextParams)
		bSilent = atoi(pInvokeStruct->m_pszTextParams)!=0;
	LPBITMAPINFOHEADER lpbi = NULL;
	if (!bSilent)
	{
		_AFX_THREAD_STATE *pThreadState = AfxGetThreadState();
		pThreadState->m_pRoutingFrame = NULL;
		CPreviewDialog dlg(0);
		dlg.m_WindowType = CPreviewDialog::ForImageCapture;
		if (dlg.DoModal() == IDCANCEL)
			return;
		lpbi = CMV500Grab::s_pGrab->Grab(0,true);
	}
	else
	{
		int nTimeout = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("GrabTimeout"), 1000, true, true);
		if (CMV500Grab::s_pGrab->InitGrab(CMV500Grab::GM_ForGrab,false))
		{
			if (CMV500Grab::s_pGrab->StartGrab())
			{
				Sleep(nTimeout);
				CMV500Grab::s_pGrab->StopGrab();
				lpbi = CMV500Grab::s_pGrab->CopyGrabbedImage(0);
			}
			CMV500Grab::s_pGrab->DeinitGrab();
		}
	}
	if (lpbi)
	{
		pOutImage->m_lpbi = lpbi;
		s_lpbiLast = lpbi;
		pInvokeStruct->NotifyFunc( 0x80000000, pInvokeStruct->NotifyLParam );
		return;
	}
	BOOL bPal = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"),_T("VideoStandard"),0);
	int cx,cy;
	if (bPal)
	{
		cx = 768;
		cy = 576;
	}
	else
	{
		cx = 640;
		cy = 480;
	}
	lpbi = (LPBITMAPINFOHEADER)malloc(sizeof(BITMAPINFOHEADER)+cx*cy*3);
	if( !lpbi )
	{
		::MessageBox( NULL, "Memory Low!", NULL, MB_SYSTEMMODAL|MB_OK|MB_ICONSTOP ); 
		return;
	}
	pOutImage->m_lpbi = lpbi;
	memset(lpbi, 0, sizeof(BITMAPINFOHEADER)+cy*cx*3);
	lpbi->biSize = sizeof(BITMAPINFOHEADER);
	lpbi->biWidth = cx;
	lpbi->biHeight = cy;
	lpbi->biBitCount = 24;
	lpbi->biCompression = BI_RGB;
	lpbi->biPlanes = 1;
	lpbi->biSizeImage = 0;
	lpbi->biClrUsed = lpbi->biClrImportant = 0;
	BYTE *pData = (BYTE*)(lpbi+1);
	pInvokeStruct->NotifyFunc( 0x80000000, pInvokeStruct->NotifyLParam );
	for (int y = 0; y < cy; y++)
		for (int x = 0; x < cx; x++, pData+=3)
		{
			pData[0] = (BYTE)y;
			pData[1] = (BYTE)x;
			pData[2] = (BYTE)(x+y);
		}
}

///////////////////////////////////////////////////////////////////////////////
//
// Information - retrieving functions


const CIntFuncInfo FInfo=
	{0, 0, 6, 0,"MV500GrabImage","MV500GrabImage",0};

HANDLE hInst;

CImageArg res1 =
{
	{sizeof(CImageArg),
	iptImage,
	"Result image of function MV500GrabImage",
	"Image on output"},
	NULL
};


std_dll_c	unsigned DllGetFunctionsCount()
{
	return 1;
}

std_dll_c	unsigned DllGetFunctionInfo(unsigned nFuncNo, CIntFuncInfo *pBuffer)
{
	if (nFuncNo>=1) return 0xFFFFFFFF;
	if (!pBuffer||pBuffer->m_nSize!=sizeof(CIntFuncInfo)) return 0;

	memcpy(pBuffer, &FInfo, pBuffer->m_nSize );

	pBuffer->m_nSize = sizeof(CIntFuncInfo);
	return 0;
}

std_dll_c	unsigned DllGetFuncArgsCount(unsigned nFuncNo)
{
	return 0;
}

std_dll_c	CArgInfo *DllGetFuncArgInfo(unsigned nFuncNo, unsigned nArgNo)
{
//	if (nFuncNo>=FuncNo) return NULL;
//	if (nArgNo>=arrArgsCount[nFuncNo]) return NULL;
//	CArgInfo **ppArgInfo = args[nFuncNo];
//	return ppArgInfo[nArgNo];
	return NULL;
}

std_dll_c	CArgInfo *DllGetFuncResultInfo(unsigned nFuncNo)
{
	return (CArgInfo *)&res1;
}

// Work functions

CErrorInfo errUserBreak =
{
	sizeof(CErrorInfo),
	3,
	svFatal,
	"User break"
};



