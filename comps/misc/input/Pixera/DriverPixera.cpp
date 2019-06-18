// DriverPixera.cpp: implementation of the CDriverPixera class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Pixera.h"
#include "DriverPixera.h"
#include "PreviewDialog.h"
#include "Image5.h"
#include <vfw.h>
#include "debug.h"
#include "PixSdk.h"
#include "Settings.h"
#include "Vt5Profile.h"
#include "WaitDlg.h"
#include "core5.h"
#include "misc_utils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


class CDibDraw
{
public:
	HDRAWDIB	hdd;

	CDibDraw()
	{ hdd = ::DrawDibOpen();}
	~CDibDraw()
	{ DrawDibClose( hdd );}
};

CDibDraw	dd;

static bool AttachDIBBits(IImage3 *pimg, BITMAPINFOHEADER *pbi, byte *pbyteDIBBits)
{
	ASSERT( pbi );

	int	cx = pbi->biWidth;
	int	cy = pbi->biHeight<0?-pbi->biHeight:pbi->biHeight;
	bool bHorzMirror = pbi->biHeight<0;
	int	cx4;
	
	long	nBitCount = pbi->biBitCount;
//calc line length
	if( nBitCount == 1 )
		cx4 = ((cx+7)/8+3)/4*4;
	else if( nBitCount == 2 )
		cx4 = ((cx+3)/4+3)/4*4;
	else if( nBitCount == 4 )
		cx4 = ((cx+1)/2+3)/4*4;
	else if( nBitCount == 8 )
		cx4 = (cx+3)/4*4;
	else if( nBitCount == 16 ) // 16-bit grayscale
		cx4 = (cx*2+3)/4*4;
	else if( nBitCount == 24 )
		cx4 = (cx*3+3)/4*4;
	else if( nBitCount == 48 )
		cx4 = (cx*6+3)/4*4;
	else
		return false;

	
	long	nPalSize = (nBitCount <= 8) ? 1<<nBitCount : 0;
	
	byte	*pbytePalette = nPalSize?((byte*)pbi)+pbi->biSize:0;
	byte	*pbyteDIBData = ((byte*)pbi)+pbi->biSize+nPalSize*4;

	if( pbyteDIBBits )
		pbyteDIBData = pbyteDIBBits;

	byte	*pbyteRed = 0;
	byte	*pbyteGreen = 0;
	byte	*pbyteBlue = 0;
//is color image
	bool	bColor = nBitCount!=16;
//if image has palette, check it is gray scale
	if( nPalSize > 0 )
	{
		ASSERT( pbytePalette );
		bool	bGrayScale = true;

		pbyteRed = new byte[nPalSize];
		pbyteGreen = new byte[nPalSize];
		pbyteBlue = new byte[nPalSize];

		for( long nEntry = 0; nEntry < nPalSize; nEntry++ )
		{
			long	nPalOffset = nEntry*4;

			if( pbytePalette[nPalOffset] != pbytePalette[nPalOffset+1] ||
				pbytePalette[nPalOffset] != pbytePalette[nPalOffset+2] )
				bGrayScale = false;
			pbyteRed[nEntry] = pbytePalette[nPalOffset];
			pbyteGreen[nEntry] = pbytePalette[nPalOffset+1];
			pbyteBlue[nEntry] = pbytePalette[nPalOffset+2];
		}

		if( bGrayScale )
			bColor = false;
	}
//create a new image
	_bstr_t strCC = bColor?"RGB":"Gray";

	if (pimg->CreateImage( cx, cy, strCC, -1 ) != S_OK)
		return false;
//find the color convertor
	IUnknown	*punkCC = 0;
	pimg->GetColorConvertor( &punkCC );
	sptrIColorConvertor5	sptrC( punkCC );

	if( !punkCC )
		return false;
	punkCC->Release();
//alloc the buffer

	int	nColorsCount = ::GetImagePaneCount( pimg );
	color	**ppcolorBuf = new color*[nColorsCount];

	long	nLineLength = bColor?((cx*3+3)/4*4):((cx+3)/4*4);
	long	nColorCol, nCol, nRow;
//convert image rows
	for( nRow = 0; nRow < cy; nRow++ )
	{
		bool	bFreeFlag = false;
		color	*pRow,*pRowR,*pRowG,*pRowB; 
		bool    bConverted = false;
		byte	*pRaw = pbyteDIBData+nRow*cx4;
		byte	*pRGB = 0;
		int nDestRow = bHorzMirror?nRow:cy-nRow-1;
//convert row from raw to RGB DIB
		switch( nBitCount )
		{
		case 1:		//1 bit per pixel image
			pRGB = new byte[nLineLength];
			bFreeFlag = true;

			ASSERT(pbyteRed);
			ASSERT(pbyteGreen);
			ASSERT(pbyteBlue);

			for( nCol = 0, nColorCol = 0; nCol < cx; nCol++, nColorCol+=3 )
			{
				long	nByte = nCol / 8;
				long	nOffset = nCol % 8;
				long	nOffset1 = 7-nOffset;

				byte	byteVal = (pRaw[nByte] & (128>>nOffset))>>nOffset1;

				if( bColor )
				{
					pRGB[nColorCol+2] = pbyteBlue[byteVal];
					pRGB[nColorCol+1] = pbyteGreen[byteVal];
					pRGB[nColorCol+0] = pbyteRed[byteVal];
				}
				else
					pRGB[nCol] = pbyteBlue[byteVal];
			}
			break;
		case 2:		//2 bit per pixel image
			pRGB = new byte[nLineLength];
			bFreeFlag = true;

			ASSERT(pbyteRed);
			ASSERT(pbyteGreen);
			ASSERT(pbyteBlue);

			for( nCol = 0, nColorCol = 0; nCol < cx; nCol++, nColorCol+=3 )
			{
				long	nByte = nCol / 4; 
				long	nOffset = nCol % 4;
				long	nOffset1 = 4-nOffset;

				byte	byteVal = (pRaw[nByte] & (192>>nOffset))>>nOffset1;

				if( bColor )
				{
					pRGB[nColorCol+2] = pbyteBlue[byteVal];
					pRGB[nColorCol+1] = pbyteGreen[byteVal];
					pRGB[nColorCol+0] = pbyteRed[byteVal];
				}
				else
					pRGB[nCol] = pbyteBlue[byteVal];
			}
			break;
		case 4:		//4 bit per pixel image
			pRGB = new byte[nLineLength];
			bFreeFlag = true;

			ASSERT(pbyteRed);
			ASSERT(pbyteGreen);
			ASSERT(pbyteBlue);

			for( nCol = 0, nColorCol = 0; nCol < cx; nCol++, nColorCol+=3 )
			{
				long	nByte = nCol / 2; 
				long	nOffset = 4*(nCol % 2);
				long	nOffset1 = 4-nOffset;

				byte	byteVal = (pRaw[nByte] & (240>>nOffset))>>nOffset1;

				if( bColor )
				{
					pRGB[nColorCol+2] = pbyteBlue[byteVal];
					pRGB[nColorCol+1] = pbyteGreen[byteVal];
					pRGB[nColorCol+0] = pbyteRed[byteVal];
				}
				else
					pRGB[nCol] = pbyteBlue[byteVal];
			}
			break;
		case 8:		//8 bit per pixel image
			if( bColor )
			{
				pRGB = new byte[nLineLength];
				bFreeFlag = true;

				ASSERT(pbyteRed);
				ASSERT(pbyteGreen);
				ASSERT(pbyteBlue);

				for( nCol = 0, nColorCol = 0; nCol < cx; nCol++, nColorCol+=3 )
				{
					byte	byteVal = pRaw[nCol];

					pRGB[nColorCol+2] = pbyteBlue[byteVal];
					pRGB[nColorCol+1] = pbyteGreen[byteVal];
					pRGB[nColorCol+0] = pbyteRed[byteVal];
				}
			}
			else
				pRGB = pRaw;

			break;
		case 16:
			ASSERT( !bColor );
			pimg->GetRow( nDestRow, 0, &pRow );
			for (nCol = 0; nCol < cx; nCol++)
				pRow[nCol] = ((WORD*)pRaw)[nCol];
			bConverted = true;
			break;

		case 24:		//24 bit per pixel image
			ASSERT(bColor);
			pimg->GetRow( nDestRow, 0, &pRowR );
			pimg->GetRow( nDestRow, 1, &pRowG );
			pimg->GetRow( nDestRow, 2, &pRowB );
			for (nCol = 0; nCol < cx; nCol++)
			{
				pRowB[nCol] = ((WORD)pRaw[3*nCol])<<8;
				pRowG[nCol] = ((WORD)pRaw[3*nCol+1])<<8;
				pRowR[nCol] = ((WORD)pRaw[3*nCol+2])<<8;
			}
			bConverted = true;
			break;

		case 48:
			ASSERT( bColor );
			pimg->GetRow( nDestRow, 0, &pRowR );
			pimg->GetRow( nDestRow, 1, &pRowG );
			pimg->GetRow( nDestRow, 2, &pRowB );
			for (nCol = 0; nCol < cx; nCol++)
			{
				pRowR[nCol] = ((WORD*)pRaw)[3*nCol];
				pRowG[nCol] = ((WORD*)pRaw)[3*nCol+1];
				pRowB[nCol] = ((WORD*)pRaw)[3*nCol+2];
			}
			bConverted = true;
			break;

		}

		if (!bConverted && sptrC != 0)
		{
			for( long nColor = 0; nColor < nColorsCount; nColor++ )
			{
				 pimg->GetRow( nDestRow, nColor, &ppcolorBuf[nColor] );
			}
			if( sptrC->ConvertDIBToImageEx( pRGB, ppcolorBuf, cx, bColor, nColorsCount ) != S_OK )
				return false;
		}
		if( bFreeFlag )
			delete pRGB;
	}

	if( ppcolorBuf )delete ppcolorBuf;
	if( pbyteRed )delete pbyteRed;
	if( pbyteGreen )delete pbyteGreen;
	if( pbyteBlue )delete pbyteBlue;

	return true;
}

static DWORD s_dwTickCountLast = 0;
static DWORD s_dwPeriod = 0;
static bool  s_bConfChanged = false;
void ImageFunc(void*,void*)
{
	DWORD dwTickCount = GetTickCount();
	if (s_dwTickCountLast > 0)
		s_dwPeriod = dwTickCount - s_dwTickCountLast;
	s_dwTickCountLast = dwTickCount;
	EnterCriticalSection(&CDriverPixera::m_csSites);
	for (int i = 0; i < CDriverPixera::m_arrSites.GetSize(); i++)
		CDriverPixera::m_arrSites[i]->Invalidate();
	LeaveCriticalSection(&CDriverPixera::m_csSites);
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//IMPLEMENT_DYNCREATE(CDriverBaumer, CDriverBase)

CArray<IDriverSite *,IDriverSite *&> CDriverPixera::m_arrSites;
CRITICAL_SECTION CDriverPixera::m_csSites;
bool CDriverPixera::m_bCSSitesInited = false;


CDriverPixera::CDriverPixera()
{
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	AfxOleLockApp();
	m_nDrvState = 0;
	m_nGrabCount = 0;
	m_hPrv = NULL;
	if (!m_bCSSitesInited)
	{
		m_bCSSitesInited = true;
		InitializeCriticalSection(&m_csSites);
	}
	m_RectEditMode = None;
	m_RectEditZone = RectNone;
	m_bInited = false;
	m_nPreviewMode = Preview_Default;

	//sergey 24/01/06
	//m_nIdNewMeth = IDC_BUTTON_NEW_SETTINGS;
	//CMethNameDlg::s_idd = IDD_DIALOG_SETTING_NAME;
	//end
}

CDriverPixera::~CDriverPixera()
{
	if (CAM_IsDriverLoaded())
	{
		CAM_Exit();
		CAM_UnloadDriver();
	}
	// To terminate the application when all objects created with
	// 	with OLE automation, the destructor calls AfxOleUnlockApp.
	AfxOleUnlockApp();
}

IUnknown *CDriverPixera::DoGetInterface( const IID &iid )
{
	if (iid == IID_IDriver)
		return (IDriver *)this;
	else if (iid == IID_IDriver2)
		return (IDriver2 *)this;
	else if (iid == IID_IDriver3)
		return (IDriver3 *)this;
	else if (iid == IID_IDriver4)
		return (IDriver4 *)this;
	else if (iid == IID_IDriver5)
		return (IDriver5 *)this;
	else if (iid == IID_IDriver6)
		return (IDriver6 *)this;
	else if (iid == IID_IInputPreview2)
		return (IInputPreview2 *)this;
	else if (iid == IID_IInputPreview3)
		return (IInputPreview3 *)this;
	else if (iid == IID_IInputPreview)
		return (IInputPreview *)this;
	else
		return ComObjectBase::DoGetInterface(iid);
}

static int s_nAdditionalInit = 0;
static int s_nTimerId = 0;
void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	static bool s_bInside = false;
	if (s_bInside) return;
	s_bInside = true;
//	AfxMessageBox("111");
	if ((int)idEvent == s_nTimerId && CAM_IsDriverLoaded() && CAM_IsConnected())
	{
		bool bWBShow;
		PxRectEx WBrect;
		UCHAR byWBR,byWBG,byWBB;
		if (CAM_WB_GetMode() == kAutoWB)
		{
			//CAM_WB_SetFactors(1.,1.,1.);			
			// Sleep(15);
			//CAM_WB_SetMode(kAutoWB);

			CAM_WB_GetRegion(&bWBShow,&WBrect,&byWBR,&byWBG,&byWBB);
			CAM_WB_SetRegion(bWBShow,&WBrect,byWBR,byWBG,byWBB);
		}
		else if (CAM_WB_GetMode() == kManualWB)
		{
			double dWBRed,dWBGreen,dWBBlue;
			CAM_WB_GetFactors(&dWBRed,&dWBGreen,&dWBBlue);
			//sergey 11/05/06
			CAM_WB_SetFactors(1.,1.,1.);
			
//          Sleep(200);
			//Sleep(100);
			CAM_WB_SetFactors(dWBRed,dWBGreen,dWBBlue);
			TRACE("TimerProc, factors are %f,%f,%f\n", dWBRed, dWBGreen, dWBBlue);
			Sleep(300);
			//end
		}
		if (s_nAdditionalInit >= 1)
		{
			if (CStdProfileManager::m_pMgr->GetProfileInt(_T("BlackBalance"), _T("UseBlackBalance"), 0))
			{
				CAM_BB_Start();
				bool bBBShow;
				PxRectEx BBrect;
				UCHAR byBBR,byBBG,byBBB;
//				CAM_BB_GetRegion(&bBBShow,&BBrect,&byBBR,&byBBG,&byBBB);
				RestoreRect(_T("BlackBalance\\Rect"), bBBShow, BBrect, byBBR, byBBG, byBBB);
				CAM_BB_SetRegion(bBBShow,&BBrect,byBBR,byBBG,byBBB);
			}
		}
		s_nAdditionalInit++;
		if (s_nAdditionalInit >= CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("AdditionalInitCount"), 5, true))
		{
			KillTimer(NULL, s_nTimerId);
			s_nTimerId = 0;
		}
	}
//	AfxMessageBox("222");
	s_bInside = false;
}


bool CDriverPixera::Init()
{
	if(!CAM_IsDriverLoaded())
	{
		if (!CAM_LoadDriver())
		{
			dprintf("CAM_LoadDriver failed\n");
			return false;
		}
		if (!CAM_IsConnected())
			dprintf("CAM_IsConnected returned 0\n");
		BOOL bColorCamera = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("ColorCamera"), TRUE, true, true);
		CAM_Init(bColorCamera?kColorCamColor:kMonochromeCam);
		//sergey 19.05.06
		::RestoreSettings();
		//end
		m_bInited = true;
		s_nAdditionalInit = 0;
		if (CAM_WB_GetMode() == kAutoWB || CAM_WB_GetMode() == kManualWB || CStdProfileManager::m_pMgr->GetProfileInt(_T("BlackBalance"), _T("UseBlackBalance"), 0))
			s_nTimerId = SetTimer(NULL, 974, 1000, TimerProc);
		m_Curve.Load();
	}
	return true;
}

bool CDriverPixera::InitMIA()
{
	if (!Init())
		return false;
	m_hPrv = NULL;

	//	CAM_Init();
	CAM_MIA_Start(&m_hPrv);
	//sergey 19.05.06
	//::RestoreSettings();
	//Sleep(150);
	//end
	CAM_MIA_SetPreDrawFunc(ImageFunc);
	//sergey 11.05.06	
	RestoreSettingsLite();	
	//::RestoreSettings();
	//end

	if (s_bConfChanged && s_nTimerId == 0)
	{
		s_nAdditionalInit = 0;
		if (CAM_WB_GetMode() == kAutoWB || CAM_WB_GetMode() == kManualWB || CStdProfileManager::m_pMgr->GetProfileInt(_T("BlackBalance"), _T("UseBlackBalance"), 0))
			s_nTimerId = SetTimer(NULL, 974, 1000, TimerProc);
	}
	s_bConfChanged = false;

	
	return true;
}

void CDriverPixera::DeinitMIA()
{
	CAM_MIA_SetPreDrawFunc(NULL);
	CAM_MIA_Stop();
//	CAM_Exit();
}

HRESULT CDriverPixera::BeginPreview(int nDev, IInputPreviewSite *pSite)
{
	HRESULT hr = StartImageAquisition(0,TRUE);
	if (FAILED(hr)) return hr;
	return AddDriverSite(0,pSite);
}

HRESULT CDriverPixera::EndPreview(int nDev, IInputPreviewSite *pSite)
{
	RemoveDriverSite(0,pSite);
	return StartImageAquisition(0,FALSE);
}

HRESULT CDriverPixera::GetSize(int nDev, short *pdx, short *pdy)
{
	int cx = 0,cy = 0;
	PreviewResolutionEx Rsl;
	PxSizeEx Size;
	CAM_MIA_GetResolution(&Rsl, &Size);
	*pdx = (short)Size.cx;
	*pdy = (short)Size.cy;
	return S_OK;
}

HRESULT CDriverPixera::GetPreviewFreq(int nDev, DWORD *pdwFreq)
{
	*pdwFreq = 0;
	return S_OK;
}

static void _DrawFrameOnImage24(LPBITMAPINFOHEADER lpbi, PxRectEx WBRect, UCHAR byWBR,
	UCHAR byWBG, UCHAR byWBB)
{
	if (lpbi->biBitCount != 24) return;
	LPBYTE lpData = (LPBYTE)(lpbi+1);
	DWORD dwRow = (lpbi->biWidth*3+3)/4*4;
	LPBYTE lp = lpData+dwRow*(lpbi->biHeight-1-WBRect.top);
	for (int x = WBRect.left; x <= WBRect.right; x++)
	{
		lp[3*x] = byWBB;
		lp[3*x+1] = byWBG;
		lp[3*x+2] = byWBR;
	}
	lp = lpData+dwRow*(lpbi->biHeight-1-WBRect.bottom);
	for (int x = WBRect.left; x <= WBRect.right; x++)
	{
		lp[3*x] = byWBB;
		lp[3*x+1] = byWBG;
		lp[3*x+2] = byWBR;
	}
	for (int y = WBRect.top; y <= WBRect.bottom; y++)
	{
		lp = lpData+dwRow*(lpbi->biHeight-1-y);
		lp[3*WBRect.left] = byWBB;
		lp[3*WBRect.left+1] = byWBG;
		lp[3*WBRect.left+2] = byWBR;
		lp[3*WBRect.right] = byWBB;
		lp[3*WBRect.right+1] = byWBG;
		lp[3*WBRect.right+2] = byWBR;
	}
}


HRESULT CDriverPixera::DrawRect(int nDev, HDC hDC, LPRECT prcSrc, LPRECT prcDst)
{
	bool bDraw = false;
	if (m_hPrv)
	{
		LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)GlobalLock(m_hPrv);
		if (lpbi && lpbi->biSize >= sizeof(BITMAPINFOHEADER))
		{
			if (m_Curve.m_bCurve)
				m_Curve.Convert(lpbi);
			if (m_nPreviewMode == Preview_Default)
			{
				bool bWBShow;
				PxRectEx WBrect;
				UCHAR byWBR,byWBG,byWBB;
				CAM_WB_GetRegion(&bWBShow,&WBrect,&byWBR,&byWBG,&byWBB);
				if (bWBShow)
				{
					OrientationEx Orient = CAM_IP_GetOrientation();
					if (Orient==kFlipHorizontal||Orient==kRotate180)
					{
						int l = lpbi->biWidth-1-WBrect.right, r = lpbi->biWidth-1-WBrect.left;
						WBrect.left = min(l,r);
						WBrect.right = max(l,r);
					}
					if (Orient==kFlipVertical||Orient==kRotate180)
					{
						int t = lpbi->biHeight-1-WBrect.top, b = lpbi->biHeight-1-WBrect.bottom;
						WBrect.top = min(t,b);
						WBrect.bottom = max(t,b);
					}
					_DrawFrameOnImage24(lpbi, WBrect, byWBR, byWBG, byWBB);
				}
			}
			if (prcSrc->bottom-prcSrc->top == prcDst->bottom-prcDst->top &&
				prcSrc->right-prcSrc->left == prcDst->right-prcDst->left)
			{
				int nWidth = prcDst->right-prcDst->left;
				int nHeight = prcDst->bottom-prcDst->top;
				SetDIBitsToDevice(hDC, prcDst->left, prcDst->top, nWidth, nHeight,
					prcSrc->left,lpbi->biHeight-nHeight-prcSrc->top,0,lpbi->biHeight,
					lpbi+1,(LPBITMAPINFO)lpbi,DIB_RGB_COLORS);
				
				
			}
			else
				DrawDibDraw(dd.hdd, hDC, prcDst->left, prcDst->top, prcDst->right-prcDst->left, prcDst->bottom-prcDst->top,
					lpbi, lpbi+1, prcSrc->left,prcSrc->top,prcSrc->right-prcSrc->left,prcSrc->bottom-prcSrc->top, 0);
			bDraw = true;
		}
		GlobalUnlock(m_hPrv);
	}
	if (!bDraw)
	{
		HBRUSH br = ::CreateSolidBrush(RGB(0,0,0));
		FillRect(hDC, prcDst, br);
		DeleteObject(br);
	}
	return S_OK;
}

HRESULT CDriverPixera::GetPeriod(int nDevice, int nMode, DWORD *pdwPeriod)
{
	*pdwPeriod = s_dwPeriod;
	return S_OK;
}

HRESULT CDriverPixera::GetPreviewMode(int nDevice, int *pnMode)
{
	*pnMode = m_nPreviewMode;
	return S_OK;
}

HRESULT CDriverPixera::SetPreviewMode(int nDevice, int nMode)
{
	if (nMode == Preview_FastCapture) return E_FAIL;
	if (m_nPreviewMode == Preview_Default && nMode == Preview_FastCaptureSmall)
	{
		CAM_AE_GetSpotSize(&m_AutoExpSaved.bDraw, &m_AutoExpSaved.rect, &m_AutoExpSaved.r,
			&m_AutoExpSaved.g, &m_AutoExpSaved.b);
		CAM_WB_GetRegion(&m_WBSaved.bDraw, &m_WBSaved.rect, &m_WBSaved.r, &m_WBSaved.g,
			&m_WBSaved.b);
		CAM_FF_GetRegion(&m_FocusSaved.bDraw, &m_FocusSaved.rect, &m_FocusSaved.r,
			&m_FocusSaved.g, &m_FocusSaved.b);
		CAM_BB_GetRegion(&m_BBSaved.bDraw, &m_BBSaved.rect, &m_BBSaved.r, &m_BBSaved.g,
			&m_BBSaved.b);
		if (m_AutoExpSaved.bDraw)
			CAM_AE_SetSpotSize(false, &m_AutoExpSaved.rect, m_AutoExpSaved.r,
				m_AutoExpSaved.g, m_AutoExpSaved.b);
		if (m_FocusSaved.bDraw)
			CAM_FF_SetRegion(false, &m_FocusSaved.rect, m_FocusSaved.r, m_FocusSaved.g,
				m_FocusSaved.b);
		if (m_BBSaved.bDraw)
			CAM_BB_SetRegion(false, &m_BBSaved.rect, m_BBSaved.r, m_BBSaved.g, m_BBSaved.b);
	}
	else if (m_nPreviewMode == Preview_FastCaptureSmall && nMode == Preview_Default)
	{
		if (m_AutoExpSaved.bDraw)
			CAM_AE_SetSpotSize(true, &m_AutoExpSaved.rect, m_AutoExpSaved.r,
				m_AutoExpSaved.g, m_AutoExpSaved.b);
		if (m_FocusSaved.bDraw)
			CAM_FF_SetRegion(true, &m_FocusSaved.rect, m_FocusSaved.r, m_FocusSaved.g,
				m_FocusSaved.b);
		if (m_BBSaved.bDraw)
			CAM_BB_SetRegion(true, &m_BBSaved.rect, m_BBSaved.r, m_BBSaved.g, m_BBSaved.b);
	}
	m_nPreviewMode = nMode;
	return S_OK;
}


HRESULT CDriverPixera::SetValue(int nCurDev, BSTR bstrName, VARIANT Value)
{
	CString s(bstrName);
	((CVT5ProfileManager*)CStdProfileManager::m_pMgr)->WriteProfileValue(NULL, s, Value);

	return S_OK;
}

HRESULT CDriverPixera::GetValue(int nCurDev, BSTR bstrName, VARIANT *pValue)
{
	CString s(bstrName);
	*pValue = ((CVT5ProfileManager*)CStdProfileManager::m_pMgr)->GetProfileValue(NULL, s);
	return S_OK;
}

void CDriverPixera::DoProcessNativeOrDIB(LPBITMAPINFOHEADER lpbi, IImage3 *pImg, IDIBProvider *pDIBPrv)
{
	if (m_Curve.m_bCurve)
		m_Curve.Convert(lpbi);
	if (pImg)
		AttachDIBBits(pImg, lpbi, NULL);
	else if (pDIBPrv)
	{
		bool bReverse = lpbi->biHeight<0;
		if (lpbi->biHeight<0)
			lpbi->biHeight = -lpbi->biHeight;
		DWORD dwRow = (((lpbi->biWidth*lpbi->biBitCount/8+3)>>2)<<2);
		DWORD dwSize = sizeof(BITMAPINFOHEADER)+lpbi->biClrUsed*sizeof(RGBQUAD)+dwRow*lpbi->biHeight;
		LPBITMAPINFOHEADER lpbiDest = (LPBITMAPINFOHEADER)pDIBPrv->AllocMem(dwSize);
		memcpy(lpbiDest, lpbi, dwSize);
		if (bReverse)
		{
			int nClrCount = lpbi->biBitCount==8?256:0;
			LPBYTE lpSrc = ((LPBYTE)(lpbi+1))+nClrCount*sizeof(RGBQUAD);
			LPBYTE lpDst = ((LPBYTE)(lpbiDest+1))+nClrCount*sizeof(RGBQUAD)+dwRow*(lpbi->biHeight-1);
			for (int y = 0; y < lpbi->biHeight; y++)
			{
				memcpy(lpDst, lpSrc, dwRow);
				lpSrc += dwRow;
				lpDst -= dwRow;
			}
		}
		if (lpbi->biBitCount == 48) // VT4 problem
		{
			BYTE *p = (BYTE*)(lpbiDest+1);
			for (int y = 0; y < lpbi->biHeight; y++)
			{
				WORD *p1 = (WORD*)(p+dwRow*y);
				for (int x = 0; x < lpbi->biWidth; x++)
				{
					WORD w = p1[3*x];
					p1[3*x] = p1[3*x+2];
					p1[3*x+2] = w;
				}
			}
		}
	}
}

HRESULT CDriverPixera::InputNativeOrDIB(IImage3 *pImg, IDIBProvider *pDIBPrv)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!Init())
		return E_UNEXPECTED;
	//sergey 11/05/06
	//BOOL bGetImageFromPreview = CStdProfileManager::m_pMgr->GetProfileInt(_T("Image"), _T("CaptureFromPreview"), 0, true,true);
	BOOL bGetImageFromPreview = CStdProfileManager::m_pMgr->GetProfileInt(_T("Image"), _T("CaptureFromPreview"), 0, TRUE);
	//end
	if (bGetImageFromPreview || m_nPreviewMode == Preview_FastCaptureSmall)
	{
		if (m_nGrabCount == 0)
			InitMIA();
		DWORD dwTick = GetTickCount();
		while(m_hPrv == 0)
		{
			if (GetTickCount()-dwTick>2000) break;
			Sleep(20);
		}
		HRESULT hr = E_FAIL;
		if (m_hPrv)
		{
			LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)GlobalLock(m_hPrv);
			if (lpbi!= 0)
			{
				DoProcessNativeOrDIB(lpbi,pImg,pDIBPrv);
				hr = S_OK;
			}
		}
		if (m_nGrabCount == 0)
			DeinitMIA();
		return hr;
	}
	HWND hwndOwner = 0;
/*	if (((CVT5ProfileManager*)CStdProfileManager::m_pMgr)->IsVT5Profile())
	{
		IApplicationPtr sptrApp(GetAppUnknown());
		if(sptrApp != 0)
			sptrApp->GetMainWindowHandle(&hwndOwner);
	}
	else
		hwndOwner = ::GetForegroundWindow();*/
	EnableWindow(hwndOwner, FALSE);
	CWaitDlg dlg;
	dlg.Create(IDD_WAIT);
	if (m_nGrabCount > 0)
		DeinitMIA();
	HANDLE hDIB = NULL;
	if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("UseSIASequence"), TRUE, true, true))
	{
		if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("Input16Bit"), FALSE, true, true))
		{
			if (CAM_IP_IsColorCapture() == kColorCamColor)
				CAM_SIA_Prep48Bit(&hDIB);
			else
				CAM_SIA_Prep16Bit(&hDIB);
		}
		else
		{
			if (CAM_IP_IsColorCapture() == kColorCamColor)
				CAM_SIA_Prep24Bit(&hDIB);
			else
				CAM_SIA_Prep8Bit(&hDIB);
		}
		CAM_SIA_CaptureExpose();
		CAM_SIA_CaptureProcess();
	}
	else
	{
		if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("Input16Bit"), FALSE, true, true))
		{
			if (CAM_IP_IsColorCapture() == kColorCamColor)
				CAM_SIA_Start48Bit(&hDIB);
			else
				CAM_SIA_Start16Bit(&hDIB);
		}
		else
		{
			if (CAM_IP_IsColorCapture() == kColorCamColor)
				CAM_SIA_Start24Bit(&hDIB);
			else
				CAM_SIA_Start8Bit(&hDIB);
		}
	}
	MSG msg;
	while (!dlg.m_bClosed && GetMessage(&msg, NULL, 0, 0))
	{
		if (msg.message == WM_PIX_CAPTURE_DONE)
		{
			LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)hDIB;
			if (lpbi) DoProcessNativeOrDIB(lpbi,pImg,pDIBPrv);
			EnableWindow(hwndOwner, TRUE);
			dlg.DestroyWindow();
			break;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (CAM_SIA_IsRunning())
		CAM_SIA_Stop();
	if (m_nGrabCount > 0)
		InitMIA();
	return S_OK;
}

HRESULT CDriverPixera::InputNative(IUnknown *pUnkImg, IUnknown *punkTarget, int nDevice, BOOL bUseSettings)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IImage3Ptr sptrImg(pUnkImg);
	return InputNativeOrDIB(sptrImg, NULL);
}

HRESULT CDriverPixera::InputDIB(IDIBProvider *pDIBPrv, IUnknown *punkTarget, int nDevice, BOOL bFromSettings)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return InputNativeOrDIB(NULL, pDIBPrv);
}

HRESULT CDriverPixera::InputVideoFile(BSTR *pbstrVideoFile, int nDevice, int nMode)
{
	return E_NOTIMPL;
}

HRESULT CDriverPixera::ExecuteSettings(HWND hwndParent, IUnknown *punkTarget, int nDevice, int nMode, BOOL bFirst, BOOL bForInput)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (nMode == 0)
		return S_OK;
	else
	{
		CPreviewDialog dlg((IInputPreview *)this,nDevice,true);
		return dlg.DoModal() == IDOK ? S_OK : S_FALSE;
	}
}

HRESULT CDriverPixera::GetFlags(DWORD *pdwFlags)
{
	*pdwFlags = FG_ACCUMULATION|FG_INPUTFRAME|FG_FOCUSINDICATOR;
	return S_OK;
}

HRESULT CDriverPixera::GetShortName(BSTR *pbstrShortName)
{
	CString strDriverName("Pixera");
	*pbstrShortName = strDriverName.AllocSysString();
	return S_OK;
}

HRESULT CDriverPixera::GetLongName(BSTR *pbstrLongName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString strDriverFullName;
	strDriverFullName.LoadString(IDS_PIXERA);
	*pbstrLongName = strDriverFullName.AllocSysString();
	return S_OK;
}


HRESULT CDriverPixera::ExecuteDriverDialog(int nDev, LPCTSTR lpstrName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CPreviewDialog dlg((IInputPreview *)this,nDev,false);
	HRESULT r = dlg.DoModal() == IDOK ? S_OK : S_FALSE;
	if (r == S_FALSE) // cancel pressed and RestoreSettings called - need to initialize rectangle
	{ // and factors for white balance
		s_nAdditionalInit = 0;
		if (CAM_WB_GetMode() == kAutoWB || CAM_WB_GetMode() == kManualWB || CStdProfileManager::m_pMgr->GetProfileInt(_T("BlackBalance"), _T("UseBlackBalance"), 0))
			s_nTimerId = SetTimer(NULL, 974, 1000, TimerProc);
	}
	return r;
}

HRESULT CDriverPixera::GetDevicesCount(int *pnCount)
{
	*pnCount = 1;
	return S_OK;
}

HRESULT CDriverPixera::GetDeviceNames(int nDev, BSTR *pbstrShortName, BSTR *pbstrLongName, BSTR *pbstrCategory)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (nDev != 0) return E_INVALIDARG;
	CString strDriverName("Pixera");
	CString strDriverFullName,strCategory;
	strDriverFullName.LoadString(IDS_PIXERA);
	strCategory.LoadString(IDS_INPUT);
	*pbstrShortName = strDriverName.AllocSysString();
	*pbstrLongName = strDriverFullName.AllocSysString();
	*pbstrCategory = strCategory.AllocSysString();
	return S_OK;
}



HRESULT CDriverPixera::GetImage(int nCurDev, LPVOID *lplpData, DWORD *pdwSize)
{
	*lplpData = m_hPrv;
	*pdwSize = GlobalSize(m_hPrv);
	return S_OK;
}

HRESULT CDriverPixera::ConvertToNative(int nCurDev, LPVOID lpData, DWORD dwSize, IUnknown *punkImg)
{
	IImage3Ptr sptrImg(punkImg);
	LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)lpData;
	int nColor = lpbi->biClrUsed==0?lpbi->biBitCount==8?256:0:lpbi->biClrUsed;
	LPBYTE lpBytes = (LPBYTE)(((RGBQUAD*)(lpbi+1))+nColor);
	AttachDIBBits(sptrImg,lpbi,lpBytes);
	return S_OK;
}

HRESULT CDriverPixera::StartImageAquisition(int nCurDev, BOOL bStart)
{
	if (bStart)
	{
		if (m_nGrabCount == 0)
		{
			if (!InitMIA())
				return E_UNEXPECTED;
		}
		m_nGrabCount++;
	}
	else
	{
		if (--m_nGrabCount == 0)
			DeinitMIA();
	}
	return S_OK;
}

HRESULT CDriverPixera::IsImageAquisitionStarted(int nCurDev, BOOL *pbAquisition)
{
	*pbAquisition = m_nGrabCount > 0;
	return S_OK;
}

HRESULT CDriverPixera::AddDriverSite(int nCurDev, IDriverSite *pSite)
{
	EnterCriticalSection(&m_csSites);
	bool bFound = false;
	for (int i = 0; i < m_arrSites.GetSize(); i++)
	{
		if (m_arrSites[i] == pSite)
		{
			bFound = true;
			break;
		}
	}
	if (!bFound)
	{
		pSite->AddRef();
		m_arrSites.Add(pSite);
	}
	LeaveCriticalSection(&m_csSites);
	return S_OK;
}

HRESULT CDriverPixera::RemoveDriverSite(int nCurDev, IDriverSite *pSite)
{
	EnterCriticalSection(&m_csSites);
	for (int i = 0; i < m_arrSites.GetSize(); i++)
	{
		if (m_arrSites[i] == pSite)
		{
			m_arrSites.RemoveAt(i);
			pSite->Release();
		}
	}
	LeaveCriticalSection(&m_csSites);
	return S_OK;
}

HRESULT CDriverPixera::SetConfiguration(BSTR bstrName)
{
	HRESULT hr = CDriver::SetConfiguration(bstrName);
	if (SUCCEEDED(hr))
	
		if(CAM_IsDriverLoaded() && CAM_IsConnected())
		{
			//sergey 19/05/06
			//::RestoreSettings();
			//end
			s_bConfChanged = true;
		}
		
	
	return hr;
}


RectEditZone CDriverPixera::GetRectEditZone(CPoint pt, CRect rc)
{
	int n = 3;
	bool bLeft = pt.x >= rc.left-n && pt.x <= rc.left+n;
	bool bRight = pt.x >= rc.right-n && pt.x <= rc.right+n;
	bool bTop = pt.y >= rc.top-n && pt.y <= rc.top+n;
	bool bBottom = pt.y >= rc.bottom-n && pt.y <= rc.bottom+n;
	if (bLeft)
	{
		if (bTop)
			return RectTopLeft;
		else if (bBottom)
			return RectBottomLeft;
		else
			return RectLeft;
	}
	else if (bRight)
	{
		if (bTop)
			return RectTopRight;
		else if (bBottom)
			return RectBottomRight;
		else
			return RectRight;
	}
	else
	{
		if (bTop)
			return RectTop;
		else if (bBottom)
			return RectBottom;
		else if (rc.PtInRect(pt))
		{
			return RectMove;
		}
	}
	return RectNone;
}

bool CDriverPixera::GetRectEditZoneEx(RectEditZone &Zone, CPoint pt, PxRectEx Rect, PxRectEx &RectOrig)
{
	RectEditZone Zone1 = GetRectEditZone(pt, CRect(Rect.left,Rect.top,Rect.right,Rect.bottom));
	if (Zone == RectNone || Zone1 != RectMove && Zone1 != RectNone)
	{
		Zone = Zone1;
		RectOrig = Rect;
		return true;
	}
	else
		return false;
}

RectEditMode CDriverPixera::GetRectEditMode(CPoint pt, RectEditZone *pZone, PxRectEx &RectOrig)
{
	RectEditMode Mode = None;
	RectEditZone Zone = RectNone;
	bool bFocusDrawingOnOff;
	PxRectEx RectFocus;
	UCHAR byFocR, byFocG, byFocB;
	CAM_FF_GetRegion(&bFocusDrawingOnOff, &RectFocus, &byFocR, &byFocG, &byFocB);
	if (bFocusDrawingOnOff)
	{
		if (GetRectEditZoneEx(Zone, pt, RectFocus, RectOrig))
			Mode = Focus;
	}
	bool bWBShow;
	PxRectEx WBrect;
	UCHAR byWBR,byWBG,byWBB;
	CAM_WB_GetRegion(&bWBShow,&WBrect,&byWBR,&byWBG,&byWBB);
	//CAM_WB_SetMode(kAutoWB);
	if (bWBShow)
	{
		if (GetRectEditZoneEx(Zone, pt, WBrect, RectOrig))
			Mode = ColorBalance;
	}
	BOOL bUseBB = CStdProfileManager::m_pMgr->GetProfileInt(_T("BlackBalance"), _T("UseBlackBalance"), 0);
	bool bBBShow;
	PxRectEx BBrect;
	UCHAR byBBR,byBBG,byBBB;
	CAM_BB_GetRegion(&bBBShow,&BBrect,&byBBR,&byBBG,&byBBB);
	if (bBBShow && bUseBB)
	{
		if (GetRectEditZoneEx(Zone, pt, BBrect, RectOrig))
			Mode = BlackBalance;
	}
	AutoExposureModeEx AEMode = CAM_AE_GetMode();
	bool bAERect;
	PxRectEx AERect;
	UCHAR byAER, byAEG, byAEB;
	CAM_AE_GetSpotRegion(&bAERect, &AERect, &byAER, &byAEG, &byAEB);
	if (bAERect && AEMode != kManualExposure)
	{
		if (GetRectEditZoneEx(Zone, pt, AERect, RectOrig))
			Mode = Exposure;
	}


	*pZone = Zone;
	return Mode;
}

static void add_left(PxRectEx &RectFocus, int dx)
{
	RectFocus.left += dx;
	if (RectFocus.left > RectFocus.right - nMinRect)
		RectFocus.left = RectFocus.right - nMinRect;
}

static void add_top(PxRectEx &RectFocus, int dy)
{
	RectFocus.top += dy;
	if (RectFocus.top > RectFocus.bottom - nMinRect)
		RectFocus.top = RectFocus.bottom - nMinRect;
}

static void add_right(PxRectEx &RectFocus, int dx)
{
	RectFocus.right += dx;
	if (RectFocus.right < RectFocus.left + nMinRect)
		RectFocus.right = RectFocus.left + nMinRect;
}

static void add_bottom(PxRectEx &RectFocus, int dy)
{
	RectFocus.bottom += dy;
	if (RectFocus.bottom < RectFocus.top + nMinRect)
		RectFocus.bottom = RectFocus.top + nMinRect;
}

static void change_rect(int dx, int dy, RectEditZone Zone, PxRectEx &RectFocus, PxSizeEx szMax)
{
	if (Zone == RectTopLeft)
	{
		add_left(RectFocus, dx);
		add_top(RectFocus, dy);
	}
	else if (Zone == RectTopRight)
	{
		add_right(RectFocus, dx);
		add_top(RectFocus, dy);
	}
	else if (Zone == RectBottomLeft)
	{
		add_left(RectFocus, dx);
		add_bottom(RectFocus, dy);
	}
	else if (Zone == RectBottomRight)
	{
		add_right(RectFocus, dx);
		add_bottom(RectFocus, dy);
	}
	else if (Zone == RectLeft)
		add_left(RectFocus, dx);
	else if (Zone == RectRight)
		add_right(RectFocus, dx);
	else if (Zone == RectTop)
		add_top(RectFocus, dy);
	else if (Zone == RectBottom)
		add_bottom(RectFocus, dy);
	else if (Zone == RectMove)
	{
		RectFocus.left += dx;
		RectFocus.top += dy;
		RectFocus.right += dx;
		RectFocus.bottom += dy;
	}
	RectFocus.left = min(max(RectFocus.left,0),szMax.cx-1);
	RectFocus.right = min(max(RectFocus.right,0),szMax.cx-1);
	RectFocus.top = min(max(RectFocus.top,0),szMax.cy-1);
	RectFocus.bottom = min(max(RectFocus.bottom,0),szMax.cy-1);
}

static bool ConvertPoint(POINT &pt)
{
	OrientationEx Orient = CAM_IP_GetOrientation();
	if (Orient == kNoTransform) return false;
	PreviewResolutionEx Rsl;
	PxSizeEx Size;
	CAM_MIA_GetResolution(&Rsl, &Size);
	if (Orient == kFlipHorizontal)
		pt.x = Size.cx-pt.x;
	else if (Orient == kFlipVertical)
		pt.y = Size.cy-pt.y;
	else if (Orient == kRotate180)
	{
		pt.x = Size.cx-pt.x;
		pt.y = Size.cy-pt.y;
	}
	return Orient != kRotate180;
}

HRESULT CDriverPixera::MouseMove(UINT nFlags, POINT pt)
{
	if (!m_bInited)	return S_FALSE;
	if ((nFlags&MK_LBUTTON) && m_RectEditZone != RectNone)
	{
		ConvertPoint(pt);
		PreviewResolutionEx Rsl;
		PxSizeEx Size;
		CAM_MIA_GetResolution(&Rsl, &Size);
		if (m_RectEditMode == Focus)
		{
			bool bFocusDrawingOnOff;
			PxRectEx RectFocus;
			UCHAR byFocR, byFocG, byFocB;
			CAM_FF_GetRegion(&bFocusDrawingOnOff, &RectFocus, &byFocR, &byFocG, &byFocB);
			RectFocus = m_RectOrig;
			change_rect(pt.x-m_ptDrag1.x,pt.y-m_ptDrag1.y,m_RectEditZone,RectFocus,Size);
			CAM_FF_SetRegion(bFocusDrawingOnOff, &RectFocus, byFocR, byFocG, byFocB);
		}
		if (m_RectEditMode == ColorBalance)
		{
			bool bWBShow;
			PxRectEx WBrect;
			UCHAR byWBR,byWBG,byWBB;
			CAM_WB_GetRegion(&bWBShow,&WBrect,&byWBR,&byWBG,&byWBB);
			WBrect = m_RectOrig;
			change_rect(pt.x-m_ptDrag1.x,pt.y-m_ptDrag1.y,m_RectEditZone,WBrect,Size);
//			TRACE("WBRect: %d-%d, %d-%d\n", WBrect.left, WBrect.right, WBrect.top, WBrect.bottom);
			CAM_WB_SetMode(kAutoWB);
			CAM_WB_SetRegion(bWBShow,&WBrect,byWBR,byWBG,byWBB);
		}
		if (m_RectEditMode == BlackBalance)
		{
			bool bBBShow;
			PxRectEx BBrect;
			UCHAR byBBR,byBBG,byBBB;
			CAM_BB_GetRegion(&bBBShow,&BBrect,&byBBR,&byBBG,&byBBB);
			BBrect = m_RectOrig;
			change_rect(pt.x-m_ptDrag1.x,pt.y-m_ptDrag1.y,m_RectEditZone,BBrect,Size);
//			TRACE("BBRect: %d-%d, %d-%d\n", BBrect.left, BBrect.right, BBrect.top, BBrect.bottom);
			CAM_BB_SetRegion(bBBShow,&BBrect,byBBR,byBBG,byBBB);
		}
		if (m_RectEditMode == Exposure)
		{
			bool bAERect;
			PxRectEx AERect;
			UCHAR byAER, byAEG, byAEB;
			CAM_AE_GetSpotRegion(&bAERect, &AERect, &byAER, &byAEG, &byAEB);
			AERect = m_RectOrig;
			change_rect(pt.x-m_ptDrag1.x,pt.y-m_ptDrag1.y,m_RectEditZone,AERect,Size);
			CAM_AE_SetSpotPosition(bAERect, &AERect, byAER, byAEG, byAEB);
			SaveRect(_T("Exposure\\Rect"), bAERect, AERect, byAER, byAEG, byAEB);
		}
	}
	return S_OK;
}

HRESULT CDriverPixera::LButtonDown(UINT nFlags, POINT pt)
{
	if (!m_bInited)	return S_FALSE;
	ConvertPoint(pt);
	m_RectEditMode = GetRectEditMode(pt,&m_RectEditZone,m_RectOrig);
	m_ptDrag1 = pt;
	return S_OK;
}

HRESULT CDriverPixera::LButtonUp(UINT nFlags, POINT pt)
{
	if (!m_bInited)	return S_FALSE;
	m_RectEditMode = None;
	m_RectEditZone = RectNone;
	return S_OK;
}

HRESULT CDriverPixera::SetCursor(UINT nFlags, POINT pt)
{
	if (!m_bInited)	return S_FALSE;
	bool bUpDown = ConvertPoint(pt);
	RectEditZone Zone;
	if (m_RectEditZone == RectNone)
	{
		PxRectEx RectOrig;
		GetRectEditMode(pt,&Zone,RectOrig);
	}
	else
		Zone = m_RectEditZone;
	char *nNESW = bUpDown?IDC_SIZENWSE:IDC_SIZENESW;
	char *nNWSE = bUpDown?IDC_SIZENESW:IDC_SIZENWSE;
	HCURSOR hCur = NULL;
	if (Zone == RectTopLeft)
		hCur = LoadCursor(NULL, nNWSE);
	else if (Zone == RectTopRight)
		hCur = LoadCursor(NULL, nNESW);
	else if (Zone == RectBottomLeft)
		hCur = LoadCursor(NULL, nNESW);
	else if (Zone == RectBottomRight)
		hCur = LoadCursor(NULL, nNWSE);
	else if (Zone == RectLeft)
		hCur = LoadCursor(NULL, IDC_SIZEWE);
	else if (Zone == RectRight)
		hCur = LoadCursor(NULL, IDC_SIZEWE);
	else if (Zone == RectTop)
		hCur = LoadCursor(NULL, IDC_SIZENS);
	else if (Zone == RectBottom)
		hCur = LoadCursor(NULL, IDC_SIZENS);
	else if (Zone == RectMove)
		hCur = LoadCursor(NULL, IDC_HAND);
	else 
		hCur = LoadCursor(NULL, IDC_ARROW);
	if (hCur)
		::SetCursor(hCur);

	return S_OK;
}

HRESULT CDriverPixera::GetFocusMode(BOOL *pbFocusMode)
{
	if (!m_bInited)
		*pbFocusMode = CStdProfileManager::m_pMgr->GetProfileInt(_T("Focus"), _T("Enable"), FALSE);
	else
		*pbFocusMode = CAM_FF_IsRunning();
	return S_OK;
}

HRESULT CDriverPixera::SetFocusMode(BOOL bFocus)
{
	if (bFocus)
		CAM_FF_Start();
	else
		CAM_FF_Stop();
	return S_OK;
}

HRESULT CDriverPixera::GetFocusIndicator(int *pnFocusIndicator)
{
	int nCurrent,nPeak;
	CAM_FF_GetEvalValue(&nCurrent,&nPeak);
	*pnFocusIndicator = nCurrent;
	return S_OK;
}

#if 0
HRESULT CDriverPixera::GetValue(int nCurDev, BSTR bstrSec, BSTR bstrEntry, BOOL bPreview, VARIANT *pValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString sSec(bstrSec);
	CString sEntry(bstrEntry);
	if (bPreview)
	{
		if (_tcscmp(sSec, _T("Settings")) == 0)
		{
			if (_tcscmp(sEntry, _T("AutoExposure")) == 0)
			{
				AutoExposureModeEx Mode = CAM_AE_GetMode();
				pValue->vt = VT_I4;
				pValue->lVal = Mode==kManualExposure?0:1;
			}
			else if (_tcscmp(sEntry, _T("AutoExposureLocked")) == 0)
			{
				AutoExposureModeEx Mode = CAM_AE_GetMode();
				pValue->vt = VT_I4;
				pValue->lVal = Mode==kAELocked?1:0;
			}
			else if (_tcscmp(sEntry, _T("AutoExposureState")) == 0)
			{
				AutoExposureStatusEx State = CAM_AE_GetStatus();
				pValue->vt = VT_BSTR;
				CString s;
				if (State == kAEUnderExposed)
					s.LoadString(IDS_AE_STATE_UNDER);
				else if (State == kAEOverExposed)
					s.LoadString(IDS_AE_STATE_OVER);
				else if (State == kAEGoodExposure)
					s.LoadString(IDS_AE_STATE_NORMAL);
				else
					s.LoadString(IDS_AE_STATE_NOTFUNCTIONAL);
				pValue->bstrVal = s.AllocSysString();
			}
			else if (_tcscmp(sEntry, _T("AutoExposureAdjust")) == 0)
			{
				int nAdj = CAM_AE_GetAdjust();
				pValue->vt = VT_I4;
				pValue->lVal = nAdj+6;
			}
		}
	}
	return S_OK;
}

HRESULT CDriverPixera::SetValue(int nCurDev, BSTR bstrSec, BSTR bstrEntry, BOOL bPreview, VARIANT Value)
{
	if (bPreview)
	{
		m_saSecs.Add(CString(bstrSec));
		m_saVals.Add(CString(bstrEntry));
		m_naDevs.Add(nCurDev);
	}
	return S_OK;
}

HRESULT CDriverPixera::BeginPreviewSettings()
{
	return S_OK;
}

HRESULT CDriverPixera::EndPreviewSettings()
{
	return S_OK;
}

HRESULT CDriverPixera::CancelChanges()
{
	m_saSecs.RemoveAll();
	m_saVals.RemoveAll();
	m_naDevs.RemoveAll();
	return S_OK;
}

HRESULT CDriverPixera::ApplyChanges()
{
	for (int i = 0; i < m_saVals.GetSize(); i++)
	{
		_bstr_t sSec(m_saSecs[i]);
		_bstr_t sVal(m_saVals[i]);
		int nDev = m_naDevs[i];
		COleVariant var;
		GetValue(nDev, sSec, sVal, TRUE, &var);
		if (var.vt != VT_EMPTY)
			SetValue(nDev, sSec, sVal, FALSE, var);
	}
	return S_OK;
}

HRESULT CDriverPixera::GetValueInfo(int nCurDev, BSTR bstrSec, BSTR bstrEntry, int *pnType, void *pInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString sSec(bstrSec);
	CString sEntry(bstrEntry);
	if (_tcscmp(sSec, _T("Settings")) == 0)
	{
		if (_tcscmp(sEntry, _T("AutoExposureAdjust")) == 0)
		{
		}
	}
	return S_OK;
}
#endif



