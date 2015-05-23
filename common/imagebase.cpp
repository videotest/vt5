#include "stdafx.h"
#include "imagebase.h"
#include "database.h"
#include "vfw.h"
#include "math.h"
#include "timetest.h"
#include "nameconsts.h"
#include "calibrint.h"
#include "misc_utils.h"


/////////////////////////////////////////////////////////////////////////////////////////
//the color convertor base class. It implements IColorConvertor interface
//used for translating color values between native and DIB format
BEGIN_INTERFACE_MAP(CColorConvertorBase, CCmdTargetEx)
	INTERFACE_PART(CColorConvertorBase, IID_IColorConvertor, Cnv)
	INTERFACE_PART(CColorConvertorBase, IID_IColorConvertor2, Cnv)
	INTERFACE_PART(CColorConvertorBase, IID_IColorConvertor3, Cnv)
	INTERFACE_PART(CColorConvertorBase, IID_IColorConvertor4, Cnv)
	INTERFACE_PART(CColorConvertorBase, IID_IColorConvertor5, Cnv)
	INTERFACE_PART(CColorConvertorBase, IID_IColorConvertor6, Cnv)
	INTERFACE_PART(CColorConvertorBase, IID_INamedObject2, Name)
	INTERFACE_PART(CColorConvertorBase, IID_IColorConvertorLookup, CnvLookup)
	INTERFACE_PART(CColorConvertorBase, IID_IHumanColorConvertor, HumanCnv)
	INTERFACE_PART(CColorConvertorBase, IID_IHumanColorConvertor2, HumanCnv)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CColorConvertorBase, Cnv)
IMPLEMENT_UNKNOWN(CColorConvertorBase, HumanCnv)

CColorConvertorBase::CColorConvertorBase()
{
	_OleLockApp( this );
}

CColorConvertorBase::~CColorConvertorBase()
{
	_OleUnlockApp( this );
}


HRESULT CColorConvertorBase::XCnv::GetColorPanesDefCount( int *pnCount )
{
	METHOD_PROLOGUE_EX(CColorConvertorBase, Cnv)

	*pnCount = pThis->GetColorPanesCount();
	return S_OK;
}
HRESULT CColorConvertorBase::XCnv::ConvertDIBToImage( byte *pRGB, color **ppColor, int cx, BOOL bColor )
{
	METHOD_PROLOGUE_EX(CColorConvertorBase, Cnv)
	if( bColor )
		pThis->ConvertRGBToImage( pRGB, ppColor, cx );
	else
		pThis->ConvertGrayToImage( pRGB, ppColor, cx );

	return S_OK;
}
HRESULT CColorConvertorBase::XCnv::ConvertImageToDIB( color **ppColor, byte *pRGB, int cx, BOOL bColor )
{
	//_try_nested(CColorConvertorBase, Cnv, ConvertImageToDIB)
	METHOD_PROLOGUE_EX(CColorConvertorBase, Cnv)
	if( bColor )
		pThis->ConvertImageToRGB( ppColor, pRGB, cx );
	else
		pThis->ConvertImageToGray( ppColor, pRGB, cx );
	return S_OK;
}

HRESULT CColorConvertorBase::XCnv::GetCnvName(BSTR *pbstrName)
{
	METHOD_PROLOGUE_EX(CColorConvertorBase, Cnv)

	CString	str(pThis->GetCnvName());
	*pbstrName = str.AllocSysString();
	return S_OK;
}

HRESULT CColorConvertorBase::XCnv::GetPaneName(int numPane, BSTR *pbstrName)
{
	METHOD_PROLOGUE_EX(CColorConvertorBase, Cnv)
	CString	str(pThis->GetPaneName(numPane));
	*pbstrName = str.AllocSysString();
	return S_OK;
}

HRESULT CColorConvertorBase::XCnv::GetPaneFlags(int numPane, DWORD *pdwFlags)
{
	METHOD_PROLOGUE_EX(CColorConvertorBase, Cnv)
	*pdwFlags = pThis->GetPaneFlags(numPane);
	return S_OK;
}

HRESULT CColorConvertorBase::XCnv::ConvertLABToImage( double *pLab, color **ppColor, int cx )
{
	METHOD_PROLOGUE_EX(CColorConvertorBase, Cnv)
	pThis->ConvertLABToImage( pLab, ppColor, cx );
	return S_OK;
}

HRESULT CColorConvertorBase::XCnv::ConvertImageToLAB( color **ppColor, double *pLab, int cx )
{
	//_try_nested(CColorConvertorBase, Cnv, ConvertImageToLAB)
	METHOD_PROLOGUE_EX(CColorConvertorBase, Cnv)
	try
	{
		pThis->ConvertImageToLAB( ppColor, pLab, cx );
	}
	catch( ... )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}
	//_catch_nested;
	return S_OK;
}

HRESULT CColorConvertorBase::XCnv::GetConvertorIcon( HICON *phIcon )
{
	_try_nested(CColorConvertorBase, Cnv, GetConvertorIcon)
	{
		pThis->GetConvertorIcon( phIcon );
		return S_OK;
	}
	_catch_nested;
}

HRESULT CColorConvertorBase::XCnv::GetPaneColor(int numPane, DWORD* pdwColor)
{
	_try_nested(CColorConvertorBase, Cnv, GetPaneColor)
	{
		pThis->GetPaneColor(numPane, pdwColor);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CColorConvertorBase::XCnv::GetCorrespondPanesColorValues(int numPane, DWORD* pdwColorValues)
{
	_try_nested(CColorConvertorBase, Cnv, GetCorrespondPanesColorValues)
	{
		pThis->GetCorrespondPanesColorValues(numPane, pdwColorValues);
		return S_OK;
	
	}
	_catch_nested;
}

HRESULT CColorConvertorBase::XCnv::GetConvertorFlags(DWORD* pdwFlags)
{
	_try_nested(CColorConvertorBase, Cnv, GetConvertorFlags)
	{
		if(pdwFlags)
			*pdwFlags = pThis->GetConvertorFlags();
		return S_OK;
	
	}
	_catch_nested;
}

HRESULT CColorConvertorBase::XCnv::ConvertDIBToImageEx( byte *pRGB, color **ppColor, int cx, BOOL bColor, int nPanes )
{
	METHOD_PROLOGUE_EX(CColorConvertorBase, Cnv)
	if( bColor )
		pThis->ConvertRGBToImageEx( pRGB, ppColor, cx, nPanes );
	else
		pThis->ConvertGrayToImageEx( pRGB, ppColor, cx, nPanes );
	return S_OK;	
}

HRESULT CColorConvertorBase::XCnv::ConvertImageToDIBEx( color **ppColor, byte *pRGB, int cx, BOOL bColor, int nPanes )
{
	METHOD_PROLOGUE_EX(CColorConvertorBase, Cnv)
	if( bColor )
		pThis->ConvertImageToRGBEx( ppColor, pRGB, cx, nPanes  );
	else
		pThis->ConvertImageToGrayEx( ppColor, pRGB, cx, nPanes  );
	return S_OK;	  
}

HRESULT CColorConvertorBase::XCnv::ConvertLABToImageEx( double *pLab, color **ppColor, int cx, int nPanes )
{
    METHOD_PROLOGUE_EX(CColorConvertorBase, Cnv)
	pThis->ConvertLABToImageEx( pLab, ppColor, cx, nPanes );
	return S_OK;	  
}

HRESULT CColorConvertorBase::XCnv::ConvertImageToLABEx( color **ppColor, double *pLab, int cx, int nPanes )
{
    METHOD_PROLOGUE_EX(CColorConvertorBase, Cnv)
	pThis->ConvertImageToLABEx( ppColor, pLab, cx, nPanes );
	return S_OK;	  
}

HRESULT CColorConvertorBase::XCnv::GetUsedPanes(IUnknown *punkImage, IUnknown *punkView, bool *pbUsedPanes, int nSize)
{
    METHOD_PROLOGUE_EX(CColorConvertorBase, Cnv)
	bool b = pThis->OnGetUsedPanes(punkImage, punkView, pbUsedPanes, nSize);
	return b?S_OK:E_NOTIMPL;
}

HRESULT CColorConvertorBase::XCnv::GetEditMode(IUnknown *punkImage, int *pnColorMode)
{
    METHOD_PROLOGUE_EX(CColorConvertorBase, Cnv)
	*pnColorMode = pThis->OnGetEditMode(punkImage);
	return S_OK;
}

HRESULT CColorConvertorBase::XCnv::GetPalette(IUnknown *punkImage, IUnknown *punkView, COLORREF *pColors, int nColors)
{
    METHOD_PROLOGUE_EX(CColorConvertorBase, Cnv)
	bool b = pThis->OnGetPalette(punkImage, punkView, pColors, nColors);
	return b?S_OK:E_NOTIMPL;
}

HRESULT CColorConvertorBase::XCnv::InitObjectAfterThresholding(IUnknown *punkNewObjImg, IUnknown *punkBaseImg, INIT_OBJECT_DATA *pData)
{
    METHOD_PROLOGUE_EX(CColorConvertorBase, Cnv)
	pThis->OnInitObjectAfterThresholding(punkNewObjImg, punkBaseImg, pData);
	return S_OK;
}

HRESULT CColorConvertorBase::XCnv::ConvertImageToDIB2( color **ppColor, byte *pRGB, int cx,
	BOOL bColor, int nPane, IUnknown *punkImage, IUnknown *punkParams)
{
	METHOD_PROLOGUE_EX(CColorConvertorBase, Cnv)
	if( bColor )
		pThis->ConvertImageToRGB2( ppColor, pRGB, cx, nPane, punkImage, punkParams);
	else
		pThis->ConvertImageToGray2( ppColor, pRGB, cx, nPane, punkImage, punkParams);
	return S_OK;	  
}

HRESULT CColorConvertorBase::XHumanCnv::GetPaneShortName( int numPane, BSTR *pbstrName )
{
	_try_nested(CColorConvertorBase, HumanCnv, GetPaneShortName)
	{
		CString str = pThis->GetPaneShortName( numPane );
		*pbstrName = str.AllocSysString();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CColorConvertorBase::XHumanCnv::GetPaneShortName2(IUnknown *punkImage, IUnknown *punkParams, int numPane, BSTR *pbstrName )
{
	_try_nested(CColorConvertorBase, HumanCnv, GetPaneShortName2)
	{
		CString str = pThis->GetPaneShortName( numPane, punkImage, punkParams );
		*pbstrName = str.AllocSysString();
		return S_OK;
	}
	_catch_nested;
}


HRESULT CColorConvertorBase::XHumanCnv::Convert( color colorInternal, int numPane, color* pcolorHuman )
{
	_try_nested(CColorConvertorBase, HumanCnv, Convert)
	{
		*pcolorHuman = pThis->ConvertToHumanValue( colorInternal, numPane );
		return S_OK;
	}
	_catch_nested;
}

HRESULT CColorConvertorBase::XHumanCnv::GetColorString(IUnknown *punkImage, IUnknown *punkParams, POINT pt, BSTR *pbstr)
{
	_try_nested(CColorConvertorBase, HumanCnv, GetColorString)
	{
		CString s;
		bool b = pThis->GetColorString(punkImage, punkParams, pt, s);
		if (b)
			*pbstr = s.AllocSysString();
		return b?S_OK:S_FALSE;
	}
	_catch_nested;
}


bool CColorConvertorBase::OnGetUsedPanes(IUnknown *punkImage, IUnknown *punkView, bool *pbUsedPanes, int nSize)
{
	return false;
}

int  CColorConvertorBase::OnGetEditMode(IUnknown *punkImage)
{
	return ::GetImagePaneCount(punkImage)>1?1:0;
}

CString CColorConvertorBase::GetPaneShortName( int numPane, IUnknown *punkImage, IUnknown *punkParams)
{
	CString sPaneName = GetPaneShortName(numPane);
	return sPaneName;
}


/////////////////////////////////////////////////////////////////////////////////////////
//private class - used for DrawDib library
class CDibDraw
{
	HDRAWDIB	m_hDD;
public:
	CDibDraw()
	{
		m_hDD = ::DrawDibOpen();
	}
	~CDibDraw()
	{
		::DrawDibClose( m_hDD );
	}
	operator HDRAWDIB()
	{return m_hDD;}
};

CDibDraw	g_DrawDib;

/////////////////////////////////////////////////////////////////////////////////////////
//the image wrapper class. It creates more programmer-friendly interface for 
//IImage interface
CImageWrp::CImageWrp( IUnknown *punk/* = 0*/, bool bAddRef/* = false*/ )
{
	if( punk )
		Attach( punk, bAddRef );
}

void CImageWrp::Attach( IUnknown *punk, bool bAddRef /*= true */)
{
	sptrIImage4	sptrI( punk );
	if(sptrI)
		__super::Attach(sptrI,bAddRef);
	else{
		if (GetInterfacePtr() != 0)
			__super::Release();
	}
}


//create a new image
bool CImageWrp::CreateNew( int cx, int cy, const char *szCCName, int nPanesNum /*=-1*/ )
{
	if( *this == 0 )
		Attach( ::CreateTypedObject( _T("Image") ), false );


	_bstr_t	bstrCC( szCCName );
	return (*this)->CreateImage( cx, cy, bstrCC, nPanesNum ) == S_OK;

}

//create a compatible image
bool CImageWrp::CreateCompatibleImage( CImageWrp &imgSource, bool bCopyData, CRect rect, const char *szCCName )
{
	if( *this == 0 )
		Attach( ::CreateTypedObject( _T("Image") ), false );

	//if equal, do nothing
	int	cx = rect.Width();
	int	cy = rect.Height();
	CPoint	ptOffset = rect.TopLeft();

	int	cxOriginal;
	int	cyOriginal;

	imgSource->GetSize( &cxOriginal, &cyOriginal );

	if( rect == NORECT )
	{
		imgSource->GetSize( &cx, &cy );
		imgSource->GetOffset( &ptOffset );
		rect = CRect( ptOffset, CSize( cx, cy ) );
	}

	{
		CRect	rectCur = GetRect();
		CString	strCCDest = imgSource.GetColorConvertor();
		if( szCCName )strCCDest = szCCName;
		
		if( rect == rectCur &&
			IsVirtual() == imgSource.IsVirtual() &&
			GetColorConvertor() == strCCDest )
			return true;
	}

	if( CheckInterface( imgSource, IID_INamedData ) )
	{
		INamedDataObject2Ptr	ptr( *this );
		ptr->InitAttachedData();

		CopyNamedData( ptr, imgSource );
	}


	IUnknown	*punkParent;

	{
		sptrINamedDataObject2	sptrN( imgSource );
		sptrN->GetParent( &punkParent );
	}
	{
		sptrINamedDataObject2	sptrN( *this );
		/*if( imgSource.IsVirtual() )
			sptrN->SetParent( punkParent, apfAttachParentData|apfNotifyNamedData );
		else*/
		sptrN->SetParent( punkParent, apfNotifyNamedData );
	}
	if( punkParent )
		punkParent->Release();

	
	if( imgSource.IsVirtual() )
	{
		if( (*this)->CreateVirtual( rect ) != S_OK )
			return false;
	}
	else
	{
		CString	strCC;
		if(szCCName == 0)
			strCC = imgSource.GetColorConvertor();
		else
			strCC = szCCName;
		if( !CreateNew( cx, cy, strCC )  )
			return false;

		SetOffset( ptOffset );
	}

	if( imgSource.IsMasked() )
	{
		(*this)->InitRowMasks();

		if( bCopyData && cx == cxOriginal && cy == cyOriginal )
		{
			for( int i = 0; i < cy; i++ )
				memcpy( GetRowMask( i ), imgSource.GetRowMask( i ), cx );
		}
	}

	if( imgSource.IsCountored() )
	{
		if( bCopyData )
			(*this)->InitContours();
	}

	{
		double	fCalibr;
		GUID	guidC;
		ICalibrPtr	ptrCalibrS( imgSource ), ptrCalibrD( this );
		if( ptrCalibrS != 0 && ptrCalibrD != 0 )
		{
			ptrCalibrS->GetCalibration( &fCalibr, &guidC );
			ptrCalibrD->SetCalibration( fCalibr, &guidC );
		}
	}


	return true;
}

//draw image in specified device context with specified palette
void CImageWrp::Draw( CDC &dc, const CRect rcDest, const CRect rcSrc, CPalette *pPal )
{
	//CTimeTest	test( true, "Draw image" );	
	//test.m_bEnableFileOutput = true;

	CRect	rcSource = rcSrc, rcDestination = rcDest;

	if( rcSource == NORECT )
		rcSource = CRect( 0, 0, GetWidth(), GetHeight() );
	if( rcDestination == NORECT )
		rcDestination = CRect( 0, 0, GetWidth(), GetHeight() );

	if( IsEmpty() )
		return;

	
	BITMAPINFOHEADER	*pbi = 0;

	{
		//CTimeTest	test;	
		pbi = ExtractDIBBits( rcSource );
	}

	rcSource.right = min( rcSource.right, rcSource.left+pbi->biWidth );
	rcSource.bottom = min( rcSource.bottom, rcSource.top+pbi->biHeight );

	if( !pbi )return;

	byte	*pbits = (byte*)pbi+pbi->biSize + ((pbi->biBitCount==8||pbi->biBitCount==16)?1024:0);


	
	//bool	bEnableTransparent =  ((dc.GetDeviceCaps( CAPS1 ) & C1_TRANSPARENT) != 0;


	/*dc.SetBkMode( NEWTRANSPARENT );
	dc.SetBkColor( RGB( 255, 255, 255 ) );*/

	::DrawDibDraw( g_DrawDib, dc, rcDestination.left, rcDestination.top, rcDestination.Width(), rcDestination.Height(), 
		(BITMAPINFOHEADER*)pbi, pbits,	0, 0, rcSource.Width(), rcSource.Height(), 0 );

	

	/*::StretchDIBits( dc, rcDestination.left, rcDestination.top, 
		rcDestination.Width(), rcDestination.Height(), 
		0, 0, rcSource.Width(), rcSource.Height(), 
		pbits, (BITMAPINFO*)pbi, DIB_RGB_COLORS, SRCCOPY );

	dc.SetBkMode( TRANSPARENT );*/

	
	/*HBITMAP	hbmp = ::CreateDIBitmap( dc, pbi, CBM_INIT, pbits, (BITMAPINFO*)pbi, DIB_RGB_COLORS );

	{
		CDC	dcMem;
		dcMem.CreateCompatibleDC( &dc );
		dcMem.SelectObject( CBitmap::FromHandle( hbmp ) );

		::StretchBlt( dc,	rcDestination.left, rcDestination.top, 
			rcDestination.Width(), rcDestination.Height(), dcMem, 
			0, 0, rcSource.Width(), rcSource.Height(), SRCCOPY );
	}
	::DeleteObject( hbmp );*/

	delete pbi;
}

//convert data from DIB to image
bool CImageWrp::AttachDIBBits( BITMAPINFOHEADER *pbi, byte *pbyteDIBBits )
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
/*	else if( nBitCount == 15 )
		cx4 = (cx*2+3)/4*4;*/
	else if( nBitCount == 16 )
		cx4 = (cx*2+3)/4*4;
	else if( nBitCount == 24 )
		cx4 = (cx*3+3)/4*4;
	else if( nBitCount == 32)
		cx4 = cx*4;
	else
		return false;

	bool bBitFields = nBitCount==16?pbi->biCompression==BI_BITFIELDS:false;
	
	long	nPalSize = (nBitCount <= 8) ? 1<<nBitCount : 0;
	
	byte	*pbytePalette = nPalSize?((byte*)pbi)+pbi->biSize:0;
	byte	*pbyteDIBData = bBitFields?(byte*)(((DWORD*)(pbi+1))+3):((byte*)pbi)+pbi->biSize+nPalSize*4;

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
	CString	strCC = bColor?"RGB":"Gray";

	if( !CreateNew( cx, cy, strCC ) )
		return false;
//find the color convertor
	IUnknown	*punkCC = 0;
	(*this)->GetColorConvertor( &punkCC );
	sptrIColorConvertor5	sptrC( punkCC );

	if( !punkCC )
		return false;
	punkCC->Release();
//alloc the buffer

	long	nColorsCount = GetColorsCount();
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
				byte r,g,b;
				if (bBitFields)
				{
					r = (wVal & 0xF800)>>8;
					g = (wVal & 0x07E0)>>3;
					b = (wVal & 0x001F)<<3;
				}
				else
				{
					r = (wVal & 0x7A00)>>7;
					g = (wVal & 0x03E0)>>2;
					b = (wVal & 0x001F)<<3;
				}

				pRGB[nColorCol+0] = b;
				pRGB[nColorCol+1] = g;
				pRGB[nColorCol+2] = r;
			}
			break;

		case 24:		//24 bit per pixel image
			ASSERT(bColor);
			pRGB = pRaw;
			break;

		case 32:
			ASSERT( bColor );
			pRGB = new byte[nLineLength];
			bFreeFlag = true;
			for( nCol = 0, nColorCol = 0; nCol < cx; nCol++, nColorCol+=3 )
			{
				pRGB[nColorCol+2] = pRaw[4*nCol+2];
				pRGB[nColorCol+1] = pRaw[4*nCol+1];
				pRGB[nColorCol+0] = pRaw[4*nCol+0];
			}
		}
		
		
		for( long nColor = 0; nColor < nColorsCount; nColor++ )
			ppcolorBuf[nColor] = GetRow( cy-nRow-1, nColor );


		if( sptrC->ConvertDIBToImageEx( pRGB, ppcolorBuf, cx, bColor, nColorsCount ) != S_OK )
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

//convert data from image to DIB

BITMAPINFOHEADER *CImageWrp::ExtractDIBBits( CRect rc )
{
	if( IsEmpty() )
		return 0;

	if( rc == NORECT )
		rc = GetRect();
	else
		rc-=GetOffset();

	long	nImageHeight = GetHeight();
	long	nImageWidth = GetWidth();

	
//init variables
	bool	bColor = IsColor();

	long	cx = rc.Width();
	long	cy = rc.Height();
	long	cx4 = bColor?(cx*3+3)/4*4:(cx+3)/4*4;
	long	nPaletteSize = bColor?0:1024;
	long	nDIBSize = sizeof( BITMAPINFOHEADER )+cx4*cy+nPaletteSize;

	
///alloc memory
	byte	*pbyte = new byte[nDIBSize];
	BITMAPINFOHEADER	*pbmih = (BITMAPINFOHEADER	*)pbyte;

	ZeroMemory( pbmih, sizeof( BITMAPINFOHEADER ) );
//fill BITMAPINFOHEADER
	pbmih->biSize = sizeof( BITMAPINFOHEADER );
	pbmih->biWidth = cx;
	pbmih->biHeight = cy;
	pbmih->biPlanes = 1;
	pbmih->biBitCount = bColor?24:8;
	pbmih->biSizeImage = nDIBSize;
//fill palette
	byte	*ppal = pbyte+pbmih->biSize;

	for( long nEntry = 0; nEntry < nPaletteSize/4; nEntry++ )
	{
		long	nEntryOffset = nEntry*4;
		ppal[nEntryOffset] = (byte)nEntry;
		ppal[nEntryOffset+1] = (byte)nEntry;
		ppal[nEntryOffset+2] = (byte)nEntry;
		ppal[nEntryOffset+3] = 0;
	}

	byte	*pbyteDIBBits = ppal+nPaletteSize;

//find the color convertor
	IUnknown	*punkCC = 0;
	(*this)->GetColorConvertor( &punkCC );
	sptrIColorConvertor	sptrC( punkCC );
	punkCC->Release();
//alloc rows
	long	nColorsCount = GetColorsCount();
	color	**ppcolorBuf = new color*[nColorsCount];


//translate from color to the DIB
	sptrIColorConvertorEx	sptrCEx(sptrC);
	if(bColor && sptrCEx!=0)
	{
		POINT pt;
		pt.x=rc.left;
		pt.y=rc.top;
		sptrCEx->ConvertImageToDIBRect(pbmih, pbyteDIBBits,
			CPoint(0,0), *this, CRect(0,0,cx,cy), pt, 1,
			CPoint(0,0), 0xFFFFFF, 0, 0);

	}
	else{
		for( long nRow = 0; nRow < cy; nRow++ )
		{
			byte *pRGB = pbyteDIBBits + nRow*cx4;
			for( long nColor = 0; nColor < nColorsCount; nColor++ )
				ppcolorBuf[nColor] = GetRow( nImageHeight - nRow - 1, nColor );

			sptrC->ConvertImageToDIB( ppcolorBuf, pRGB, cx, bColor );

			byte* pmask = GetRowMask( nImageHeight - nRow - 1);

			if(bColor)
				for( long x = 0; x < cx*3; x+=3 )
				{										
					if( !*pmask )
					{
						pRGB[x] = 255;
						pRGB[x+1] = 255;
						pRGB[x+2] = 255;
					}
					pmask++;	
				}
			else
				for( long x = 0; x < cx; x++ )
				{										
					if( !*pmask )
						pRGB[x] = 255;
					pmask++;	
				}
		}
	}

	if( ppcolorBuf )delete ppcolorBuf;

	return pbmih;
}
//is image empty?
bool CImageWrp::IsEmpty()
{
	if( *this == 0 )
		return true;
	int nCX = 0, nCY = 0;
	(*this)->GetSize( &nCX, &nCY );
	return nCX== 0 || nCY == 0;
}

//and is t color?
bool	CImageWrp::IsColor()
{
	return GetColorsCount() >= 3;
}

int CImageWrp::GetEditMode()
{
	if (*this == 0) return 0;
	IUnknownPtr punkCC;
	(*this)->GetColorConvertor(&punkCC);
	IColorConvertor6Ptr sptrCC6(punkCC);
	if (sptrCC6 == 0)
		return IsColor()?1:0;
	int nEditMode = 0;
	sptrCC6->GetEditMode(*this, &nEditMode);
	return nEditMode;
}

bool CImageWrp::IsVirtual()
{
	if( *this == 0 )return false;
	DWORD	dwFlags = 0;
	(*this)->GetImageFlags( &dwFlags );
	return (dwFlags & ifVirtual) != 0;
}
bool CImageWrp::IsMasked()
{
	if( *this == 0 )return false;
	DWORD	dwFlags = 0;
	(*this)->GetImageFlags( &dwFlags );
	return (dwFlags & ifMasked) != 0;
}

bool CImageWrp::IsCountored()
{
	if( *this == 0 )return false;
	DWORD	dwFlags = 0;
	(*this)->GetImageFlags( &dwFlags );
	return (dwFlags & ifContoured) != 0;
}

//image width
int CImageWrp::GetWidth()
{
	int nCX = 0, nCY = 0;
	(*this)->GetSize( &nCX, &nCY );
	return nCX;
}

//image height
int CImageWrp::GetHeight()
{
	int nCX = 0, nCY = 0;
	(*this)->GetSize( &nCX, &nCY );
	return nCY;
}

//counter of color per pixel
int CImageWrp::GetColorsCount()
{
	int nCount = -1;
	
	(*this)->GetPanesCount( &nCount );
	return nCount;
}

//image row
color *CImageWrp::GetRow( int nRow, int nPane )
{
	color	*prow = 0;
	(*this)->GetRow( nRow, nPane, &prow );
	return prow;
}

CString	CImageWrp::GetColorConvertor()
{
	//return _T("RGB");
	IUnknown	*punkCC = 0;
	(*this)->GetColorConvertor( &punkCC );
	sptrIColorConvertor2	sptrC( punkCC );

	if( !punkCC )
		return "";
	punkCC->Release();
	ASSERT(sptrC != 0);
	BSTR bstrName;
	sptrC->GetCnvName(&bstrName);
	CString strName(bstrName);
	::SysFreeString(bstrName);
	return strName;
}


IUnknown *CImageWrp::CreateVImage( CRect rect )
{
	sptrINamedDataObject2	sptrD( this );


	CImageWrp	imageNew;

	IUnknown	*punk = 0;
	sptrD->CreateChild( &punk );

	imageNew.Attach( punk, false );
	imageNew->CreateVirtual( rect );

	return imageNew.Detach();
}

byte *CImageWrp::GetRowMask( int nRow )
{
	byte	*pmask = 0;
	(*this)->GetRowMask( nRow, &pmask );
	return pmask;
}

//return the contours count
int CImageWrp::GetContoursCount()
{
	if( IsEmpty() )
		return 0;
	int	iCount;
	(*this)->GetContoursCount( &iCount );

	return iCount;
}

//get the specified contour
Contour *CImageWrp::GetContour( int nPos )
{
	if( *this == 0 )
		return 0;

	Contour	*pContour;
	(*this)->GetContour( nPos, &pContour );

	return pContour;
}

CPoint	CImageWrp::GetOffset()
{
	CPoint	point = CPoint( 0, 0 );;

	if( *this == 0 )
		return point;
	
	(*this)->GetOffset( &point );
	return point;
}

//offset the image rect
void CImageWrp::SetOffset( const POINT pt, bool bMoveImage )
{
	(*this)->SetOffset( pt, bMoveImage );
}
//initialize the masks
void CImageWrp::InitRowMasks()
{
	(*this)->InitRowMasks();
}
//initialize the contours 
void CImageWrp::InitContours()
{
	//init the new contours
	(*this)->InitContours();
}


CRect	CImageWrp::GetRect()
{
	if( IsEmpty() )
		return NORECT;
	CRect	rect;

	rect.left = GetOffset().x;
	rect.top = GetOffset().y;
	rect.right = rect.left+GetWidth();
	rect.bottom = rect.top+GetHeight();

	return rect;
}

CRgn &CImageWrp::GetPaintRgn( IUnknown *punkZoomScrollSite )
{
	if( !punkZoomScrollSite )
		return m_clipData.m_rgn;

	m_clipData.m_rgn.DeleteObject();

	if( IsEmpty() )
		return m_clipData.m_rgn;

	double fZoom = GetZoom( punkZoomScrollSite );
	CPoint ptScroll = GetScrollPos( punkZoomScrollSite );

	CPoint	pointOffset = GetRect().CenterPoint();

	m_clipData.Init( this );
	m_clipData.CalcRotateRgn( ptScroll, fZoom, pointOffset, 1, 1, 0 );

	return m_clipData.m_rgn;
}


IUnknown *CImageWrp::operator =(IUnknown *punk)
{
	Attach( punk );
	return punk;
}

bool CImageWrp::PtInImage( CPoint point )
{
	if( IsEmpty() )
		return false;
	CRect	rect = GetRect();
	if( !rect.PtInRect( point ) )
		return false;
	int	y = point.y - rect.top;
	byte *pbytes = GetRowMask( y );

	return (pbytes[point.x-rect.left] & 0x80) == 0x80;
}

//reset the contours
void CImageWrp::ResetContours()
{
	(*this)->FreeContours();
}
//reset the masks
void CImageWrp::ResetMasks()
{
	for( int i = 0; i < GetHeight(); i++ )
		ZeroMemory( GetRowMask( i ), GetWidth() );
}

/////////////////////////////////////////////////////////////////////////////////////////
CClipData::CClipData()
{
	ZeroMemory( &m_xform, sizeof( m_xform ) );
	m_pointOffset = CPoint( 0, 0 );
	m_pimage = 0;
}

void CClipData::Init( CImageWrp *pimage )
{
	m_pimage = pimage;
	/*CTimeTest	test( true, "CClipData::Init" );
	//delete the old regoion
	m_rgn.DeleteObject();
	m_rgnSource.DeleteObject();

	m_pointOffset = pimage->GetRect().CenterPoint();

	//init the region
	int	nContourCount = pimage->GetContoursCount(), nContour;
	int	nPoint;

	bool	bInit = false;

	for( nContour = 0; nContour < nContourCount; nContour++ )
	{
		Contour	*pcontour = pimage->GetContour( nContour );

		if( !pcontour->nContourSize )
			continue;

		POINT	*ppoints = new POINT[pcontour->nContourSize];
		for( nPoint = 0; nPoint < pcontour->nContourSize; nPoint++ )
		{
			ppoints[nPoint].x = pcontour->ppoints[nPoint].x;
			ppoints[nPoint].y = pcontour->ppoints[nPoint].y;
		}

		int	nContourSize = pcontour->nContourSize;

		if( (HRGN)m_rgnSource == 0 )
		{
			ASSERT( pcontour->lFlags == cfExternal );

			m_rgnSource.CreatePolyPolygonRgn( ppoints, &nContourSize, 1, WINDING );
		}
		else
		{
			CRgn	rgn;
			rgn.CreatePolyPolygonRgn( ppoints, &nContourSize, 1, WINDING );

			if( pcontour->lFlags == cfExternal )
				m_rgnSource.CombineRgn( &m_rgnSource, &rgn, RGN_OR );
			if( pcontour->lFlags == cfInternal )
				m_rgnSource.CombineRgn( &m_rgnSource, &rgn, RGN_DIFF );
		}

		delete ppoints;
	}*/
}

void CClipData::CalcRotateRgn( CPoint pointScroll, double fZoom, 
			CPoint	pointOffsetCenter,
			double fZoomX, double fZoomY, 
			double fRotateAngle )
{
	/*
	m_rgn.DeleteObject();

	if( !(HRGN)m_rgnSource )
		return;

	long	nDataSize = m_rgnSource.GetRegionData( 0, 0 );

	if( !nDataSize )
		return;

	byte	*prgnData = new byte[nDataSize];

	m_rgnSource.GetRegionData( (RGNDATA*)prgnData, nDataSize );

	XFORM	xform;
	ZeroMemory( &xform, sizeof( XFORM ) );

	xform.eM11 = (float)fZoom;
	xform.eM22 = (float)fZoom;
	xform.eDx = (float)pointOffset.x;
	xform.eDy = (float)pointOffset.y;

 	m_rgn.CreateFromData( &xform, nDataSize, (RGNDATA*)prgnData );

	delete prgnData;*/

	//CTimeTest	test( true, "CClipData::Init" );
	//delete the old regoion
	m_rgn.DeleteObject();

	//init the region
	/*int	nContourCount = m_pimage->GetContoursCount(), nContour;
	int	nPoint;
	
	for( nContour = 0; nContour < nContourCount; nContour++ )
	{
		Contour	*pcontour = m_pimage->GetContour( nContour );

		if( !pcontour->nContourSize )
			continue;

		POINT	*ppointsTopLeft = new POINT[pcontour->nContourSize];
		POINT	*ppointsBottomRight = new POINT[pcontour->nContourSize];

		for( nPoint = 0; nPoint < pcontour->nContourSize; nPoint++ )
		{
			ppointsTopLeft[nPoint].x = (int)::floor( pcontour->ppoints[nPoint].x*fZoom-pointScroll.x );
			ppointsTopLeft[nPoint].y = (int)::floor( pcontour->ppoints[nPoint].y*fZoom-pointScroll.y );
			ppointsBottomRight[nPoint].x = (int)::ceil( pcontour->ppoints[nPoint].x*fZoom+fZoom-pointScroll.x );
			ppointsBottomRight[nPoint].y = (int)::ceil( pcontour->ppoints[nPoint].y*fZoom+fZoom-pointScroll.y );
		}

		if( (HRGN)m_rgn == 0 )
		{
			ASSERT( pcontour->lFlags == cfExternal );

			CRgn	rgnBottomRight;
			rgnBottomRight.CreatePolygonRgn( ppointsBottomRight, pcontour->nContourSize, WINDING );
			m_rgn.CreatePolygonRgn( ppointsTopLeft, pcontour->nContourSize, WINDING );
			m_rgn.CombineRgn( &m_rgn, &rgnBottomRight, RGN_OR );
		}
		else
		{
			CRgn	rgn;
			CRgn	rgnBottomRight;

			rgn.CreatePolygonRgn( ppointsTopLeft, pcontour->nContourSize, WINDING );
			rgnBottomRight.CreatePolygonRgn( ppointsBottomRight, pcontour->nContourSize, WINDING );
			rgn.CombineRgn( &rgn, &rgnBottomRight, RGN_OR );

			if( pcontour->lFlags == cfExternal )
				m_rgn.CombineRgn( &m_rgn, &rgn, RGN_OR );
			if( pcontour->lFlags == cfInternal )
				m_rgn.CombineRgn( &m_rgn, &rgn, RGN_DIFF );
		}

		delete ppointsTopLeft;
		delete ppointsBottomRight;
	}*/

	int	x, y;
	int	cx=  m_pimage->GetWidth();
	int	cy = m_pimage->GetHeight();

	int	xstart, xend;

	CRect	rect = m_pimage->GetRect();

	int	left = (int)( rect.left*fZoom );
	int	right = (int)( rect.right*fZoom );
	int	top = (int)( rect.top*fZoom );
	int	bottom = (int)( rect.bottom*fZoom );

	bool	bInit = false;
	

	for( y = 0; y < cy; y++ )
	{
		byte	*prow = m_pimage->GetRowMask( y );

		xstart = -1;	//not started here

		for( x = 0; x < cx; x++, prow++ )
		{
			if( !*prow && xstart != -1 )
			{
				xend = x-1;
				//exclude region here
				int	left = (int)( (xstart+rect.left)*fZoom );
				int	right = (int)( (xend+1+rect.left)*fZoom );
				int	top = (int)( (y+rect.top)*fZoom );
				int	bottom = (int)( (y+1+rect.top)*fZoom );

				CRgn	rgn;

				if( bInit )
				{
					rgn.CreateRectRgn( left, top, right, bottom );
					m_rgn.CombineRgn( &m_rgn, &rgn, RGN_OR );
				}
				else
				{
					m_rgn.CreateRectRgn( left, top, right, bottom );
					bInit = true;
				}
				
				
				xstart = -1;
			}
			else
				if( *prow && xstart == -1 )
					xstart = x;
		}

		if( xstart != -1 )
		{
			xend = x-1;
			//exclude region here
			int	left = (int)( (xstart+rect.left)*fZoom );
			int	right = (int)( (xend+1+rect.left)*fZoom );
			int	top = (int)( (y+rect.top)*fZoom );
			int	bottom = (int)( (y+1+rect.top)*fZoom );

			CRgn	rgn;

			if( bInit )
			{
				rgn.CreateRectRgn( left, top, right, bottom );
				m_rgn.CombineRgn( &m_rgn, &rgn, RGN_OR );
			}
			else
			{
				m_rgn.CreateRectRgn( left, top, right, bottom );
				bInit = true;
			}
		}
		
		
		xstart = -1;

	}

	if( (HRGN)m_rgn )
		m_rgn.OffsetRgn( -pointScroll.x, -pointScroll.y );

}