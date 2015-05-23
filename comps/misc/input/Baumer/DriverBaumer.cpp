// DriverBaumer.cpp: implementation of the CDriverBaumer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Baumer.h"
#include "DriverBaumer.h"
#include "PreviewDlg.h"
#include "VT5Profile.h"
#include "Accumulate.h"
#include "CxLibWork.h"
#include "Image5.h"
#include "BaumUtils.h"
#include "misc_utils.h"
#include <vfw.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*static inline int Bright ( int b, int g, int r )	//функция для получения яркости
{
	return (r*60+g*118+b*22)/200;
}*/


static bool AttachDIBBits(IImage3 *pimg, BITMAPINFOHEADER *pbi, byte *pbyteDIBBits)
{
	ASSERT( pbi );

	int	cx = pbi->biWidth;
	int	cy = pbi->biHeight;
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
	/*else if( nBitCount == 15 )
		cx4 = (cx*2+3)/4*4;
	else if( nBitCount == 16 )
		cx4 = (cx*2+3)/4*4;*/
	else if( nBitCount == 24 )
		cx4 = (cx*3+3)/4*4;
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
	bool	bColor = true;
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
	sptrIColorConvertor	sptrC( punkCC );

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

		byte	*pRaw = pbyteDIBData+nRow*cx4;
		byte	*pRGB = 0;
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
			ASSERT( bColor );
			pRGB = new byte[nLineLength];
			bFreeFlag = true;

			for( nCol = 0, nColorCol = 0; nCol < cx; nCol++, nColorCol+=3 )
			{
				WORD	wVal = ((WORD*)pRaw)[nCol];

				byte	r = (wVal & 0xF800)>>6;
				byte	g = (wVal & 0x0760)>>3;
				byte	b = (wVal & 0x001F)<<3;

				pRGB[nColorCol+2] = b;
				pRGB[nColorCol+1] = g;
				pRGB[nColorCol+0] = r;
			}

		case 24:		//24 bit per pixel image
			ASSERT(bColor);
			pRGB = pRaw;
			break;
		}
		
		
		for( long nColor = 0; nColor < nColorsCount; nColor++ )
		{
			 pimg->GetRow( cy-nRow-1, nColor, &ppcolorBuf[nColor] );
		}

		if( sptrC->ConvertDIBToImage( pRGB, ppcolorBuf, cx, bColor ) != S_OK )
			return false;

		if( bFreeFlag )
			delete pRGB;
	}

	if( ppcolorBuf )delete ppcolorBuf;
	if( pbyteRed )delete pbyteRed;
	if( pbyteGreen )delete pbyteGreen;
	if( pbyteBlue )delete pbyteBlue;

	return true;
}

void AttachDIB2Image(IImage3 *pimg, LPBITMAPINFOHEADER lpbi)
{
	if (DIBBps(lpbi) == 2)
	{
		if (DIBIsGrayScale(lpbi))
			pimg->CreateImage(lpbi->biWidth, lpbi->biHeight, _bstr_t("Gray"),-1);
		else
			pimg->CreateImage(lpbi->biWidth, lpbi->biHeight, _bstr_t("RGB"),-1);
		int nRow = DIBRowSize(lpbi)/2;
		LPWORD pSrc = (LPWORD)DIBData(lpbi);
		int nShift = DIBShift(lpbi);
		if (DIBIsGrayScale(lpbi))
		{
			for (int i = lpbi->biHeight-1; i >= 0; i--)
			{
				LPWORD pDst;
				pimg->GetRow(i,0,&pDst);
				for (int j = 0; j < lpbi->biWidth; j++)
					pDst[j] = pSrc[j]<<nShift;
				pSrc += nRow;
			}
		}
		else
		{
			for (int i = lpbi->biHeight-1; i >= 0; i--)
			{
				LPWORD pRed,pGreen,pBlue;
				pimg->GetRow(i,0,&pRed);
				pimg->GetRow(i,1,&pGreen);
				pimg->GetRow(i,2,&pBlue);
				for (int j = 0; j < lpbi->biWidth; j++)
				{
					pRed[j] = (pSrc[3*j+2])<<nShift;
					pGreen[j] = (pSrc[3*j+1])<<nShift;
					pBlue[j] = (pSrc[3*j+0])<<nShift;
				}
				pSrc += nRow;
			}
		}
	}
	else
		AttachDIBBits(pimg, lpbi, (LPBYTE)DIBData(lpbi)); // use g_CxLibWork.m_lpbi
}


class CDibDraw
{
public:
	HDRAWDIB	hdd;

	CDibDraw()
	{ hdd = ::DrawDibOpen();}
	~CDibDraw()
	{ DrawDibClose( hdd );}
};

CDibDraw	hdd;

static int CalcShift(LPBITMAPINFOHEADER lpbi)
{
	int nShift = 8;
	switch(lpbi->biBitCount)
	{
	case 10: case 30: nShift = 6; break;
	case 12: case 36: nShift = 4; break;
	case 16: case 48: nShift = 0; break;
	}
	return nShift;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//IMPLEMENT_DYNCREATE(CDriverBaumer, CDriverBase)

CDriverBaumer::CDriverBaumer()
{
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	AfxOleLockApp();
	m_bDataInBuffer = false;
}

CDriverBaumer::~CDriverBaumer()
{
	if (m_bDataInBuffer)
		g_CxLibWork.Deinit();
	// To terminate the application when all objects created with
	// 	with OLE automation, the destructor calls AfxOleUnlockApp.
	AfxOleUnlockApp();
}

IUnknown *CDriverBaumer::DoGetInterface( const IID &iid )
{
	if (iid == IID_IDriver)
		return (IDriver *)this;
	else if (iid == IID_IDriver2)
		return (IDriver2 *)this;
	else if (iid == IID_IDriver3)
		return (IDriver3 *)this;
	else if (iid == IID_IDriver4)
		return (IDriver3 *)this;
	else if (iid == IID_IInputPreview)
		return (IInputPreview *)this;
	else
		return ComObjectBase::DoGetInterface(iid);
}


HRESULT CDriverBaumer::BeginPreview(IInputPreviewSite *pSite)
{
	HRESULT hr = StartImageAquisition(0,TRUE);
	if (FAILED(hr)) return hr;
	return AddDriverSite(0,pSite);
}

HRESULT CDriverBaumer::EndPreview(IInputPreviewSite *pSite)
{
	RemoveDriverSite(0,pSite);
	return StartImageAquisition(0,FALSE);
}

HRESULT CDriverBaumer::GetSize(short *pdx, short *pdy)
{
	int cx,cy;
	if (g_CxLibWork.IsInitedOk())
	{
		BOOL bPreviewAllFrame = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("PreviewAllFrame"), FALSE);
		if (bPreviewAllFrame || !g_CxLibWork.m_bEnableMargins)
		{
			cx = g_CxLibWork.m_cx;
			cy = g_CxLibWork.m_cy;
		}
		else
		{
			CRect rcMargins;
			if (g_CxLibWork.m_CameraStatus.bBinning)
				rcMargins = CRect(g_CxLibWork.m_rcMargins.left/2, g_CxLibWork.m_rcMargins.top/2,
					g_CxLibWork.m_rcMargins.right/2, g_CxLibWork.m_rcMargins.bottom/2);
			else
				rcMargins = g_CxLibWork.m_rcMargins;
			cx = g_CxLibWork.m_cx-rcMargins.left-rcMargins.right;
			cy = g_CxLibWork.m_cy-rcMargins.top-rcMargins.bottom;
		}
		if (g_CxLibWork.m_CameraStatus.bBinning)
		{
			cx *= 2;
			cy *= 2;
		}
	}
	else
	{
		cx = 0;
		cy = 0;
	}
	*pdx = (short)cx;
	*pdy = (short)cy;
	return S_OK;
}

HRESULT CDriverBaumer::GetPreviewFreq(DWORD *pdwFreq)
{
	*pdwFreq = 0;
//	dwFreq = g_BaumerProfile.GetProfileInt(NULL, _T("RedrawFrequency"), 100, true);
	return S_OK;
}


HRESULT CDriverBaumer::DrawRect(HDC hDC, LPRECT lpRectSrc, LPRECT lpRectDst)
{
	if (g_CxLibWork.IsImageCaptured())
	{
//		BOOL bGrayScale = g_CxLibWork.m_nGrayMode!=NoneGrayMode;
//		LPBITMAPINFOHEADER lpbi = bGrayScale?g_CxLibWork.m_lpbiGray:g_CxLibWork.m_lpbi;
		LPBITMAPINFOHEADER lpbi = g_CxLibWork.m_lpbi;
		LPBYTE lpData = (LPBYTE)(((RGBQUAD*)(lpbi+1))+lpbi->biClrUsed);
		BOOL bBinning = g_CxLibWork.m_CameraStatus.bBinning;
		CRect rcMargins;
		if (bBinning)
			rcMargins = CRect(g_CxLibWork.m_rcMargins.left/2, g_CxLibWork.m_rcMargins.top/2, g_CxLibWork.m_rcMargins.right/2,
				g_CxLibWork.m_rcMargins.bottom/2);
		else
			rcMargins = g_CxLibWork.m_rcMargins;
		BOOL bPreviewAllFrame = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("PreviewAllFrame"), FALSE);
		if (bPreviewAllFrame || !g_CxLibWork.m_bEnableMargins) rcMargins = CRect(0,0,0,0);
		CSize sz(lpbi->biWidth-rcMargins.left-rcMargins.right,lpbi->biHeight-rcMargins.top-rcMargins.bottom);
		CRect rcSrc = bBinning?CRect(lpRectSrc->left/2,lpRectSrc->top/2,lpRectSrc->right/2,lpRectSrc->bottom/2):CRect(*lpRectSrc);
		CRect rcDst(*lpRectDst);
		if (rcSrc == CRect(0,0,0,0))
			rcSrc = CRect(0,0,sz.cx,sz.cy);
		rcSrc.OffsetRect(rcMargins.left, rcMargins.top);
//		bool bStretch = rcSrc.Width() != rcDst.Width() || rcSrc.Height() != rcDst.Height() ||
//			rcMargins != CRect(0,0,0,0);
		if (rcSrc.Width() > sz.cx)
		{
//			rcDst.right = rcDst.left + rcDst.Width()*sz.cx/rcSrc.Width();
			rcSrc.right = rcSrc.left + sz.cx;
		}
		if (rcSrc.Height() > sz.cy)
		{
//			rcDst.bottom = rcDst.top + rcDst.Height()*sz.cy/rcSrc.Height();
			rcSrc.bottom = sz.cy;
		}
		bool bStretch = rcSrc.Width() != rcDst.Width() || rcSrc.Height() != rcDst.Height() ||
			rcMargins != CRect(0,0,0,0);
		if (bStretch)
//			StretchDIBits(hDC, lpRectDst->left, lpRectDst->top, rcDst.Width(), rcDst.Height(),
//				rcSrc.left, rcSrc.top, rcSrc.Width(), rcSrc.Height(),
//				lpData, (BITMAPINFO *)lpbi, DIB_RGB_COLORS, SRCCOPY);
			DrawDibDraw(hdd.hdd, hDC, lpRectDst->left, lpRectDst->top, rcDst.Width(), rcDst.Height(), (BITMAPINFOHEADER *)lpbi,
				lpData, rcSrc.left, rcSrc.top, rcSrc.Width(), rcSrc.Height(), 0);
		else
			SetDIBitsToDevice(hDC, lpRectDst->left-rcSrc.left, lpRectDst->top-rcSrc.top, sz.cx, sz.cy, 0,
				0, 0, sz.cy, lpData, (BITMAPINFO *)lpbi, DIB_RGB_COLORS);
	}
	else
	{
		HBRUSH br = ::CreateSolidBrush(RGB(0,0,0));
		FillRect(hDC, lpRectDst, br);
		DeleteObject(br);
	}
	return S_OK;
}

#if 0
bool CDriverBaumer::OnInitOverlay()
{
	return g_CxLibWork.Init();
}

void CDriverBaumer::OnDeinitOverlay()
{
}

void CDriverBaumer::OnGetOvrSize(short &cx, short &cy)
{
	OnGetSize(cx,cy);
}

bool CDriverBaumer::OnStartOverlay(HWND hwndDraw, LPRECT lprcSrc, LPRECT lprcDst)
{
	CRect rcSrc(*lprcSrc);
	CRect rcDst(*lprcDst);
	m_nScaler = rcSrc==rcDst?0:1;
	g_CxLibWork.StartDisplay(TRUE, m_nScaler, hwndDraw, rcDst);
	return true;
}

void CDriverBaumer::OnStopOverlay(HWND hwndDraw)
{
	g_CxLibWork.StartDisplay(FALSE, 1, hwndDraw, NULL);
}

void CDriverBaumer::OnSetDstRect(HWND hwndDraw, LPRECT lprcDst)
{
	g_CxLibWork.StartDisplay(TRUE, m_nScaler, hwndDraw, *lprcDst);
}

void CDriverBaumer::OnSetSrcRect(HWND hwndDraw, LPRECT lprcSrc)
{
}
#endif

HRESULT CDriverBaumer::SetValue(int nCurDev, BSTR bstrName, VARIANT Value)
{
	CString s(bstrName);
	int n= -1;
	g_CxLibWork.Init();
	if (s.CompareNoCase(_T("Shutter"))==0)
	{
		if (Value.vt == VT_I2)
			n = Value.iVal;
		else if (Value.vt == VT_I4)
			n = Value.lVal;
		if (n > 0 && g_CxLibWork.IsInitedOk())
		{
			n = Range(n,g_CxLibWork.m_sht.timeMin,g_CxLibWork.m_sht.timeMax);
			g_BaumerProfile.WriteProfileInt(_T("Settings"), _T("ShutterMcs"), n);
			tSHT sht;
			SetShutterTime(n, &sht);
		}
	}
	else if (s.CompareNoCase(_T("Gain"))==0)
	{
		if (Value.vt == VT_I2)
			n = Value.iVal;
		else if (Value.vt == VT_I2)
			n = Value.lVal;
		if (n > 0 && g_CxLibWork.IsInitedOk())
		{
			n = Range(n, g_CxLibWork.m_CameraType.vAmplMin, g_CxLibWork.m_CameraType.vAmplMax);
			g_BaumerProfile.WriteProfileInt(_T("Settings"), _T("Gain"), n);
			SetGain(n);
		}
	}
	else
		g_BaumerProfile.WriteProfileValue(NULL, s, Value);
	return S_OK;
}

HRESULT CDriverBaumer::GetValue(int nCurDev, BSTR bstrName, VARIANT *pValue)
{
	CString s(bstrName);
	g_CxLibWork.Init();
	if (s.CompareNoCase(_T("Shutter"))==0)
	{
		int nShutterMcs = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("ShutterMcs"), 1000);
		pValue->vt = VT_I4;
		pValue->lVal = nShutterMcs;
	}
	else if (s.CompareNoCase(_T("Gain"))==0)
	{
		int nGain = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("Gain"), -1);
		if (nGain == -1)
			nGain = g_CxLibWork.m_CameraStatus.vAmplif;
		pValue->vt = VT_I4;
		pValue->lVal = nGain;
	}
	else if (s.CompareNoCase(_T("IsShutter"))==0)
	{
		BOOL bAutoShutter = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("AutoShutter"), FALSE);
		pValue->vt = VT_I4;
		pValue->lVal = bAutoShutter?0:1;
	}
	else  if (s.CompareNoCase(_T("IsGain"))==0)
	{
		pValue->vt = VT_I4;
		pValue->lVal = 1;
	}
	else if (s.CompareNoCase(_T("ShutterMin"))==0)
	{
		pValue->vt = VT_I4;
		pValue->lVal = 1;
	}
	else if (s.CompareNoCase(_T("ShutterMax"))==0)
	{
		pValue->vt = VT_I4;
		pValue->lVal = 1000000;
	}
	else if (s.CompareNoCase(_T("GainMin"))==0)
	{
		pValue->vt = VT_I4;
		pValue->lVal = g_CxLibWork.m_CameraType.vAmplMin;
	}
	else if (s.CompareNoCase(_T("GainMax"))==0)
	{
		pValue->vt = VT_I4;
		pValue->lVal = g_CxLibWork.m_CameraType.vAmplMax;
	}
	else if (_tcsnicmp(s, _T("Is"), 2))
	{
		pValue->vt = VT_I4;
		pValue->lVal = 0;
	}
	else
		*pValue = g_BaumerProfile.GetProfileValue(NULL, s);
	return S_OK;
}

static void AttachImage(IImage3 *pimg)
{
//	BOOL bGrayScale = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("GrayScale"), FALSE);
//	int nPlane = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("Plane"), TRUE);
	if (g_CxLibWork.m_nBitsMode != W8BIT && g_CxLibWork.m_nConversion == NoConversion) // Input 1x-bit, use m_lpBuff16
	{
		ASSERT(g_CxLibWork.m_bpp == 2);
		BOOL bBinning = g_CxLibWork.m_CameraStatus.bBinning;
		CRect rcMargins;
		if (!g_CxLibWork.m_bEnableMargins)
			rcMargins = CRect(0,0,0,0);
		else if (bBinning)
			rcMargins = CRect(g_CxLibWork.m_rcMargins.left/2, g_CxLibWork.m_rcMargins.top/2, g_CxLibWork.m_rcMargins.right/2,
				g_CxLibWork.m_rcMargins.bottom/2);
		else
			rcMargins = g_CxLibWork.m_rcMargins;
		int cx = g_CxLibWork.m_lpbi->biWidth-rcMargins.left-rcMargins.right;
		int cy = g_CxLibWork.m_lpbi->biHeight-rcMargins.top-rcMargins.bottom;
		if (/*bGrayScale ||*/ g_CxLibWork.m_lpbi16->biBitCount<=16)
			pimg->CreateImage(cx, cy, _bstr_t("Gray"),-1);
		else
			pimg->CreateImage(cx, cy, _bstr_t("RGB"),-1);
		int cx1 = g_CxLibWork.m_lpbi16->biWidth;
		int nRow = g_CxLibWork.m_lpbi16->biBitCount<=16?cx1:cx1*3;
		LPWORD pSrc = ((LPWORD)(g_CxLibWork.m_lpbi16+1))+rcMargins.bottom*nRow;
		int nShift = CalcShift(g_CxLibWork.m_lpbi16);
		if (g_CxLibWork.m_lpbi16->biBitCount<=16)
		{
			for (int i = cy-1; i >= 0; i--)
			{
				LPWORD pDst;
				pimg->GetRow(i,0,&pDst);
				for (int j = 0; j < cx; j++)
					pDst[j] = pSrc[rcMargins.left+j+2]<<nShift;
				pSrc += nRow;
			}
		}
		/*else if (bGrayScale)
		{
			if (nPlane == GrayPlane)
			{
				for (int i = cy-1; i >= 0; i--)
				{
					LPWORD pDst;
					pimg->GetRow(i,0,&pDst);
					for (int j = 0; j < cx; j++)
						pDst[j] = Bright(pSrc[3*(rcMargins.left+j)+0], pSrc[3*(rcMargins.left+j)+1],
							pSrc[3*(rcMargins.left+j)+2])<<nShift;
					pSrc += nRow;
				}
			}
			else if (nPlane == RedPlane)
			{
				for (int i = cy-1; i >= 0; i--)
				{
					LPWORD pDst;
					pimg->GetRow(i,0,&pDst);
					for (int j = 0; j < cx; j++)
						pDst[j] = pSrc[3*(rcMargins.left+j)+2]<<nShift;
					pSrc += nRow;
				}
			}
			else if (nPlane == GreenPlane)
			{
				for (int i = cy-1; i >= 0; i--)
				{
					LPWORD pDst;
					pimg->GetRow(i,0,&pDst);
					for (int j = 0; j < cx; j++)
						pDst[j] = pSrc[3*(rcMargins.left+j)+1]<<nShift;
					pSrc += nRow;
				}
			}
			else //if (nPlane == BluePlane)
			{
				for (int i = cy-1; i >= 0; i--)
				{
					LPWORD pDst;
					pimg->GetRow(i,0,&pDst);
					for (int j = 0; j < cx; j++)
						pDst[j] = pSrc[3*(rcMargins.left+j)+0]<<nShift;
					pSrc += nRow;
				}
			}
		}*/
		else
		{
			for (int i = cy-1; i >= 0; i--)
			{
				LPWORD pRed,pGreen,pBlue;
				pimg->GetRow(i,0,&pRed);
				pimg->GetRow(i,1,&pGreen);
				pimg->GetRow(i,2,&pBlue);
				for (int j = 0; j < cx; j++)
				{
					int j1 = rcMargins.left+j;
//					ASSERT((pSrc[3*j1+0]&0xFC00)==0);
//					ASSERT((pSrc[3*j1+1]&0xFC00)==0);
//					ASSERT((pSrc[3*j1+2]&0xFC00)==0);
					pRed[j] = (pSrc[3*j1+2])<<nShift;
					pGreen[j] = (pSrc[3*j1+1])<<nShift;
					pBlue[j] = (pSrc[3*j1+0])<<nShift;
				}
				pSrc += nRow;
			}
		}
	}
	else if ((!g_CxLibWork.m_bEnableMargins || g_CxLibWork.m_rcMargins == CRect(0,0,0,0)) /*&& !bGrayScale*/)
		AttachDIBBits(pimg, g_CxLibWork.m_lpbi, g_CxLibWork.m_lpData); // use g_CxLibWork.m_lpbi
	else // Some conversion required
	{
		BOOL bBinning = g_CxLibWork.m_CameraStatus.bBinning;
		CRect rcMargins;
		if (!g_CxLibWork.m_bEnableMargins)
			rcMargins = CRect(0,0,0,0);
		else if (bBinning)
			rcMargins = CRect(g_CxLibWork.m_rcMargins.left/2, g_CxLibWork.m_rcMargins.top/2, g_CxLibWork.m_rcMargins.right/2,
				g_CxLibWork.m_rcMargins.bottom/2);
		else
			rcMargins = g_CxLibWork.m_rcMargins;
//		LPBITMAPINFOHEADER lpbiSrc = bGrayScale?g_CxLibWork.m_lpbiGray:g_CxLibWork.m_lpbi;
		LPBITMAPINFOHEADER lpbiSrc = g_CxLibWork.m_lpbi;
		int cx = lpbiSrc->biWidth-rcMargins.left-rcMargins.right;
		int cy = lpbiSrc->biHeight-rcMargins.top-rcMargins.bottom;
		int bpp = lpbiSrc->biBitCount;
		int nRow = ((cx*bpp/8+3)>>2)<<2;
		DWORD dwSizeHdr = sizeof(BITMAPINFOHEADER)+lpbiSrc->biClrUsed*sizeof(RGBQUAD);
		DWORD dwSizeData = nRow*cy;
		LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)new BYTE[dwSizeHdr+dwSizeData];
		memcpy(lpbi, lpbiSrc, dwSizeHdr);
		lpbi->biWidth = cx;
		lpbi->biHeight = cy;
		lpbi->biSizeImage = 0;
		LPBYTE pDst = ((LPBYTE)lpbi)+dwSizeHdr;
		LPBYTE pDst1 = pDst;
		int nRowSrc = ((lpbiSrc->biWidth*bpp/8+3)>>2)<<2;
		DWORD dwOffset = rcMargins.left*bpp/8;
		LPBYTE pSrc = ((LPBYTE)lpbiSrc)+dwSizeHdr+nRowSrc*rcMargins.bottom+dwOffset;
		for (int i = 0; i < cy; i++)
		{
			memcpy(pDst, pSrc, nRow);
			pDst += nRow;
			pSrc += nRowSrc;
		}
		AttachDIBBits(pimg, lpbi, pDst1);
		delete lpbi;
	}
}

HRESULT CDriverBaumer::InputNative(IUnknown *pUnkImg, IUnknown *punkTarget, int nDevice, BOOL bUseSettings)
{
	CWaitCursor wc;
	bool bOk2;
	BOOL bAccum = g_BaumerProfile.GetProfileInt(NULL, _T("Accumulate"), FALSE);
	if (m_bDataInBuffer&&(bAccum||!bUseSettings))
		m_bDataInBuffer = false;
	int nPlane = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("Plane"), TRUE);
	if (bAccum)
	{
		// If grab in process, then probably preview view is active. Use data from preview view.
		g_CxLibWork.Init();
		bOk2 = g_CxLibWork.StartGrab();
		if (bOk2)
		{
			LPBITMAPINFOHEADER lpbiSrc = g_CxLibWork.m_lpbi;
			int nImagesNum = g_BaumerProfile.GetProfileInt(NULL, _T("AccumulateImagesNum"), FALSE);
			CAccumulate Accum;
			for (int i = 0; i < nImagesNum; i++)
			{
				g_CxLibWork.WaitForFrame(0);
				Accum.Add(lpbiSrc);
			}
			g_CxLibWork.StopGrab();
			Accum.GetResult(lpbiSrc, true);
			IImage3Ptr img(pUnkImg);
			AttachImage(img);
		}
		g_CxLibWork.Deinit();
	}
	else
	{
		IImage3Ptr img(pUnkImg);
		if (m_bDataInBuffer)
		{
			// Use data in buffer
			AttachImage(img);
			m_bDataInBuffer = false;
		}
		else if (g_CxLibWork.IsStarted())
		{
			// No data has beed saved, but grab in process. Probably preview view is active. Use data from
			// preview view.
			g_CxLibWork.SetForCapture(TRUE);
			AttachImage(img);
			g_CxLibWork.SetForCapture(FALSE);
		}
		else
		{
			// Grab mode is initialized. It needed to start it. Need to start grab.
			g_CxLibWork.Init();
			g_CxLibWork.SetForCapture(TRUE);
			bOk2 = g_CxLibWork.StartGrab();
			if (bOk2)
			{
				g_CxLibWork.WaitForFrame(0);
				g_CxLibWork.StopGrab();
				AttachImage(img);
			}
			g_CxLibWork.SetForCapture(FALSE);
			g_CxLibWork.Deinit();
		}
	}
	return S_OK;
}

HRESULT CDriverBaumer::InputDIB(IDIBProvider *pDIBPrv, IUnknown *punkTarget, int nDevice, BOOL bFromSettings)
{
	return E_NOTIMPL;
}

HRESULT CDriverBaumer::InputVideoFile(BSTR *pbstrVideoFile, int nDevice, int nMode)
{
	return E_NOTIMPL;
}


static LPBITMAPINFOHEADER AttachImage()
{
//	BOOL bGrayScale = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("GrayScale"), FALSE);
//	int nPlane = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("Plane"), TRUE);
//	LPBITMAPINFOHEADER lpbiSrc = bGrayScale?g_CxLibWork.m_lpbiGray:g_CxLibWork.m_lpbi;
	LPBITMAPINFOHEADER lpbiSrc = g_CxLibWork.m_lpbi;
	LPBITMAPINFOHEADER lpbi;
	if (g_CxLibWork.m_nBitsMode != W8BIT && g_CxLibWork.m_nConversion == NoConversion) // Input 1x-bit, use m_lpBuff16
	{
		ASSERT(g_CxLibWork.m_bpp == 2);
		BOOL bBinning = g_CxLibWork.m_CameraStatus.bBinning;
		CRect rcMargins;
		if (!g_CxLibWork.m_bEnableMargins)
			rcMargins = CRect(0,0,0,0);
		else if (bBinning)
			rcMargins = CRect(g_CxLibWork.m_rcMargins.left/2, g_CxLibWork.m_rcMargins.top/2, g_CxLibWork.m_rcMargins.right/2,
				g_CxLibWork.m_rcMargins.bottom/2);
		else
			rcMargins = g_CxLibWork.m_rcMargins;
		int cx = g_CxLibWork.m_lpbi->biWidth-rcMargins.left-rcMargins.right;
		int cy = g_CxLibWork.m_lpbi->biHeight-rcMargins.top-rcMargins.bottom;
		int cx1 = g_CxLibWork.m_lpbi16->biWidth;//GetImFrameX();
		int nRow = g_CxLibWork.m_lpbi16->biBitCount<=16?cx1:cx1*3;
		BOOL bGrayScale = g_CxLibWork.m_lpbi16->biBitCount<=16;
		int nRowDst = bGrayScale?cx:cx*3;
		DWORD dwSize = sizeof(BITMAPINFOHEADER)+nRowDst*2*cy;
		if (bGrayScale) dwSize += 256*sizeof(RGBQUAD);
		lpbi = (LPBITMAPINFOHEADER)malloc(dwSize);
		memset(lpbi, 0, dwSize);
		lpbi->biSize = sizeof(BITMAPINFOHEADER);
		lpbi->biBitCount = bGrayScale?16:48;
		lpbi->biWidth = cx;
		lpbi->biHeight = cy;
		lpbi->biPlanes = 1;
		LPWORD pSrc = ((LPWORD)(g_CxLibWork.m_lpbi16+1))+rcMargins.bottom*nRow;
		LPWORD pDst = (LPWORD)(lpbi+1);
		int nShift = CalcShift(g_CxLibWork.m_lpbi16);
		if (g_CxLibWork.m_lpbi16->biBitCount<=16)
		{
			for (int i = cy-1; i >= 0; i--)
			{
				for (int j = 0, j1 = rcMargins.left; j < cx; j++,j1++)
					pDst[j] = (pSrc[j1])<<nShift;
				pSrc += nRow;
				pDst += nRowDst;
			}
		}
		/*else if (bGrayScale)
		{
			if (nPlane == GrayPlane)
			{
				for (int i = cy-1; i >= 0; i--)
				{
					for (int j = 0, j1 = rcMargins.left; j < cx; j++,j1++)
						pDst[j] = Bright((pSrc[3*j1+0])<<nShift,(pSrc[3*j1+1])<<nShift,(pSrc[3*j1+2])<<nShift);
					pSrc += nRow;
					pDst += nRowDst;
				}
			}
			else if (nPlane == BluePlane)
			{
				for (int i = cy-1; i >= 0; i--)
				{
					for (int j = 0, j1 = rcMargins.left; j < cx; j++,j1++)
						pDst[j] = (pSrc[3*j1+0])<<nShift;
					pSrc += nRow;
					pDst += nRowDst;
				}
			}
			else if (nPlane == GreenPlane)
			{
				for (int i = cy-1; i >= 0; i--)
				{
					for (int j = 0, j1 = rcMargins.left; j < cx; j++,j1++)
						pDst[j] = (pSrc[3*j1+1])<<nShift;
					pSrc += nRow;
					pDst += nRowDst;
				}
			}
			else //if (nPlane == RedPlane)
			{
				for (int i = cy-1; i >= 0; i--)
				{
					for (int j = 0, j1 = rcMargins.left; j < cx; j++,j1++)
						pDst[j] = (pSrc[3*j1+2])<<nShift;
					pSrc += nRow;
					pDst += nRowDst;
				}
			}
		}*/
		else
		{
			for (int i = cy-1; i >= 0; i--)
			{
				for (int j = 0; j < cx; j++)
				{
					int j1 = rcMargins.left+j;
					pDst[3*j+0] = (pSrc[3*j1+0])<<nShift;
					pDst[3*j+1] = (pSrc[3*j1+1])<<nShift;
					pDst[3*j+2] = (pSrc[3*j1+2])<<nShift;
				}
				pSrc += nRow;
				pDst += nRowDst;
			}
		}
	}
	else
	{
		BOOL bBinning = g_CxLibWork.m_CameraStatus.bBinning;
		CRect rcMargins;
		if (!g_CxLibWork.m_bEnableMargins)
			rcMargins = CRect(0,0,0,0);
		else if (bBinning)
			rcMargins = CRect(g_CxLibWork.m_rcMargins.left/2, g_CxLibWork.m_rcMargins.top/2, g_CxLibWork.m_rcMargins.right/2,
				g_CxLibWork.m_rcMargins.bottom/2);
		else
			rcMargins = g_CxLibWork.m_rcMargins;
//		LPBITMAPINFOHEADER lpbiSrc = bGrayScale?g_CxLibWork.m_lpbiGray:g_CxLibWork.m_lpbi;
		LPBITMAPINFOHEADER lpbiSrc = g_CxLibWork.m_lpbi;
		int cx = lpbiSrc->biWidth-rcMargins.left-rcMargins.right;
		int cy = lpbiSrc->biHeight-rcMargins.top-rcMargins.bottom;
		int bpp = lpbiSrc->biBitCount;
		int nRow = ((cx*bpp/8+3)>>2)<<2;
		DWORD dwSizeHdr = sizeof(BITMAPINFOHEADER)+lpbiSrc->biClrUsed*sizeof(RGBQUAD);
		DWORD dwSizeData = nRow*cy;
		lpbi = (LPBITMAPINFOHEADER)malloc(dwSizeHdr+dwSizeData);
		memcpy(lpbi, lpbiSrc, dwSizeHdr);
		lpbi->biWidth = cx;
		lpbi->biHeight = cy;
		lpbi->biSizeImage = 0;
		LPBYTE pDst = ((LPBYTE)lpbi)+dwSizeHdr;
		LPBYTE pDst1 = pDst;
		int nRowSrc = ((lpbiSrc->biWidth*bpp/8+3)>>2)<<2;
		DWORD dwOffset = rcMargins.left*bpp/8;
		LPBYTE pSrc = ((LPBYTE)lpbiSrc)+dwSizeHdr+nRowSrc*rcMargins.bottom+dwOffset;
		for (int i = 0; i < cy; i++)
		{
			memcpy(pDst, pSrc, nRow);
			pDst += nRow;
			pSrc += nRowSrc;
		}
	}
	return lpbi;
}

LPBITMAPINFOHEADER CDriverBaumer::OnInputImage(BOOL bUseSettings)
{
	CWaitCursor wc;
	bool bOk1,bOk2;
	LPBITMAPINFOHEADER lpbi = NULL;
	BOOL bAccum = g_BaumerProfile.GetProfileInt(NULL, _T("Accumulate"), FALSE);
	if (m_bDataInBuffer&&(bAccum||!bUseSettings))
		m_bDataInBuffer = false;
	if (bAccum)
	{
		bool bStarted = g_CxLibWork.IsStarted();
		// If grab in process, then probably preview view is active. Use data from preview view.
		bOk2 = g_CxLibWork.StartGrab();
		if (bOk2)
		{
			int nImagesNum = g_BaumerProfile.GetProfileInt(NULL, _T("AccumulateImagesNum"), FALSE);
			CAccumulate Accum;
			for (int i = 0; i < nImagesNum; i++)
			{
				g_CxLibWork.WaitForFrame(0);
				Accum.Add(g_CxLibWork.m_lpbi);
			}
			g_CxLibWork.StopGrab();
			Accum.GetResult(g_CxLibWork.m_lpbi, true);
			lpbi = AttachImage();
		}
	}
	else
	{
		if (m_bDataInBuffer)
		{
			// Use data in buffer
			lpbi = AttachImage();
			m_bDataInBuffer = false;
			g_CxLibWork.Deinit();
		}
		else if (g_CxLibWork.IsStarted())
		{
			// No data has beed saved, but grab in process. Probably preview view is active. Use data from
			// preview view.
			g_CxLibWork.SetForCapture(TRUE);
			lpbi = AttachImage();
			g_CxLibWork.SetForCapture(FALSE);
		}
		else
		{
			// Grab mode is initialized. It needed to start it. Need to start grab.
			g_CxLibWork.SetForCapture(TRUE);
			bOk1 = g_CxLibWork.Init();
			if (bOk1)
			{
				bOk2 = g_CxLibWork.StartGrab();
				if (bOk2)
				{
					g_CxLibWork.WaitForFrame(0);
					g_CxLibWork.StopGrab();
					lpbi = AttachImage();
				}
			}
			g_CxLibWork.SetForCapture(FALSE);
		}
	}
	return lpbi;
}


HRESULT CDriverBaumer::ExecuteSettings(HWND hwndParent, IUnknown *punkTarget, int nDevice, int nMode, BOOL bFirst, BOOL bForInput)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (nMode == 0)
		return S_OK;
	else
	{
		CPreviewDlg dlg((IInputPreview *)this);
		int r = dlg.DoModal();
		if (bForInput && r == IDOK)
		{
			if (!dlg.m_bOvrMode && !g_BaumerProfile.GetProfileInt(_T("Settings"), _T("Binning"), FALSE) &&
				(g_CxLibWork.m_nBitsMode == W8BIT || g_CxLibWork.m_nConversion != NoConversion)
				&& !(IsDC300(g_CxLibWork.m_nCamera)&&g_CxLibWork.m_nRom!=DC300_FULLFRAME))
				m_bDataInBuffer = true; // Buffer is needed for input. Buffer will be deallocated lately.
		}
		return r == IDOK ? S_OK : S_FALSE;
	}
}

HRESULT CDriverBaumer::GetFlags(DWORD *pdwFlags)
{
	*pdwFlags = FG_INPUTFRAME|FG_ACCUMULATION;
	BOOL bBinning = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("Binning"), FALSE);
	if (!bBinning)
		*pdwFlags |= FG_TRIGGERMODE|FG_SOFTWARETRIGGER;
	return S_OK;
}

HRESULT CDriverBaumer::GetShortName(BSTR *pbstrShortName)
{
	CString strDriverName("Baumer");
	*pbstrShortName = strDriverName.AllocSysString();
	return S_OK;
}

HRESULT CDriverBaumer::GetLongName(BSTR *pbstrLongName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString strDriverFullName;
	strDriverFullName.LoadString(IDS_BAUMER);
	*pbstrLongName = strDriverFullName.AllocSysString();
	return S_OK;
}


HRESULT CDriverBaumer::ExecuteDriverDialog(int nDev, LPCTSTR lpstrName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	g_CxLibWork.Init();
	CSettingsSheet sheet(NULL, (IUnknown *)DoGetInterface(IID_IDriver3));
	sheet.DoModal();
	g_CxLibWork.Deinit();
	return E_NOTIMPL;
}

HRESULT CDriverBaumer::GetDevicesCount(int *pnCount)
{
	*pnCount = 1;
	return S_OK;
}

HRESULT CDriverBaumer::GetDeviceNames(int nDev, BSTR *pbstrShortName, BSTR *pbstrLongName, BSTR *pbstrCategory)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (nDev != 0) return E_INVALIDARG;
	CString strDriverName("Baumer");
	CString strDriverFullName,strCategory;
	strDriverFullName.LoadString(IDS_BAUMER);
	strCategory.LoadString(IDS_INPUT);
	*pbstrShortName = strDriverName.AllocSysString();
	*pbstrLongName = strDriverFullName.AllocSysString();
	*pbstrCategory = strCategory.AllocSysString();
	return S_OK;
}



HRESULT CDriverBaumer::GetImage(int nCurDev, LPVOID *lplpData, DWORD *pdwSize)
{
	if (!g_CxLibWork.IsInited())
	{
		if (lplpData) *lplpData = NULL;
		if (pdwSize) *pdwSize = 0;
		return S_FALSE;
	}
	LPBITMAPINFOHEADER lpbi;
	DWORD dwSize;
	if (g_CxLibWork.m_nBitsMode != W8BIT && g_CxLibWork.m_nConversion == NoConversion)
	{
		lpbi = g_CxLibWork.m_lpbi16;
		int bpp = lpbi->biBitCount<=16?2:6;
		dwSize = sizeof(BITMAPINFOHEADER)+(((lpbi->biWidth*bpp+3)>>2)<<2)*lpbi->biHeight;
	}
	else
	{
		lpbi = g_CxLibWork.m_lpbi;
		int bpp = lpbi->biBitCount==8?1:3;
		dwSize = sizeof(BITMAPINFOHEADER)+lpbi->biClrUsed*sizeof(RGBQUAD)+(((lpbi->biWidth*bpp+3)>>2)<<2)*lpbi->biHeight;
	}
	if (lplpData) *lplpData = lpbi;
	if (*pdwSize) *pdwSize = dwSize;
	return S_OK;
}

HRESULT CDriverBaumer::ConvertToNative(int nCurDev, LPVOID lpData, DWORD dwSize, IUnknown *punkImg)
{
	LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)lpData;
	IImage3Ptr ptrImage(punkImg);
	AttachDIB2Image(ptrImage, lpbi);
	return S_OK;
}

HRESULT CDriverBaumer::GetTriggerMode(int nCurDev, BOOL *pgTriggerMode)
{
	if (g_CxLibWork.IsInited())
		*pgTriggerMode = g_CxLibWork.m_CameraStatus .bTriggerEn;
	else
		*pgTriggerMode = g_BaumerProfile.GetProfileInt(_T("Trigger"), _T("TriggerMode"), FALSE, true);
	return S_OK;
}

HRESULT CDriverBaumer::SetTriggerMode(int nCurDev, BOOL bSet)
{
	BOOL bBinning = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("Binning"), FALSE);
	if (bBinning && bSet == TRUE)
		return E_UNEXPECTED;
	if (g_CxLibWork.IsInited())
	{
		BOOL bDischarge = g_BaumerProfile.GetProfileInt(_T("Trigger"), _T("Discharge"), FALSE, true);
		EnterCriticalSection(&g_CritSectionCamera);
		::SetTriggerMode(bSet, bDischarge);
		GetCameraInfo(&g_CxLibWork.m_CameraType, sizeof(tCameraType), &g_CxLibWork.m_CameraStatus , sizeof(tCameraStatus));
		LeaveCriticalSection(&g_CritSectionCamera);
		Sleep(g_BaumerProfile.GetProfileInt(_T("Trigger"), _T("Timeout"), 0, true));
	}
	g_BaumerProfile.WriteProfileInt(_T("Trigger"), _T("TriggerMode"), bSet);
	return S_OK;
}

HRESULT CDriverBaumer::DoSoftwareTrigger(int nCurDev)
{
	EnterCriticalSection(&g_CritSectionCamera);
	::DoTrigger();
	LeaveCriticalSection(&g_CritSectionCamera);
	return S_OK;
}


HRESULT CDriverBaumer::StartImageAquisition(int nCurDev, BOOL bStart)
{
	if (bStart)
	{
		if (!g_CxLibWork.Init()) return S_FALSE;
		if (!g_CxLibWork.StartGrab())
		{
			g_CxLibWork.Deinit();
			return S_FALSE;
		}
	}
	else
		g_CxLibWork.StopGrab();
	return S_OK;
}

HRESULT CDriverBaumer::IsImageAquisitionStarted(int nCurDev, BOOL *pbAquisition)
{
	*pbAquisition = g_CxLibWork.IsStarted()?TRUE:FALSE;
	return S_OK;
}

HRESULT CDriverBaumer::AddDriverSite(int nCurDev, IDriverSite *pSite)
{
	EnterCriticalSection(&g_CritSectionSite);
	bool bFound = false;
	for (int i = 0; i < g_CxLibWork.m_arrSites.GetSize(); i++)
	{
		if (g_CxLibWork.m_arrSites[i] == pSite)
		{
			bFound = true;
			break;
		}
	}
	if (!bFound)
	{
		pSite->AddRef();
		g_CxLibWork.m_arrSites.Add(pSite);
	}
	LeaveCriticalSection(&g_CritSectionSite);
	return S_OK;
}

HRESULT CDriverBaumer::RemoveDriverSite(int nCurDev, IDriverSite *pSite)
{
	EnterCriticalSection(&g_CritSectionSite);
	for (int i = 0; i < g_CxLibWork.m_arrSites.GetSize(); i++)
	{
		if (g_CxLibWork.m_arrSites[i] == pSite)
		{
			g_CxLibWork.m_arrSites.RemoveAt(i);
			pSite->Release();
		}
	}
	LeaveCriticalSection(&g_CritSectionSite);
	return S_OK;
}





