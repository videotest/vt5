#include "stdafx.h"
#include "image.h"
#include "resource.h"		// main symbols

#include "compressint.h"
#include "afxpriv2.h"

#include "ImagePane.h"
#include "\vt5\common2\misc_calibr.h"
#include "misc_utils.h"
#include "ImageSignature.h"
#include "\vt5\AWIN\profiler.h"

/////////////////////////////////////////////////////////////////////////////////////////
//misc utils
class CImageRowHelper
{
public:
	byte	*m_pdummyRow;

	CImageRowHelper()
	{
		m_pdummyRow = new byte[65536];
		memset( m_pdummyRow, 255, 65536 );
	}
	~CImageRowHelper()
	{
		delete []m_pdummyRow;
	}

	static CImageRowHelper	CImageRowHelper::s_helper;
};

CImageRowHelper	CImageRowHelper::s_helper;


static void _GetBackgroundColor(CImageWrp &imageSrc, color *colorsBacks)
{
	CString sCC = imageSrc.GetColorConvertor();
	if (sCC == _T("GRAY"))
	{
		COLORREF	cr = ::GetValueColor( GetAppUnknown(), "\\Image", "Background", RGB( 255, 255, 255 ) );
		int br = Bright(GetBValue(cr), GetGValue(cr), GetRValue(cr));
		colorsBacks[0] = ByteAsColor(br);
	}
	else
	{
		COLORREF	cr = ::GetValueColor( GetAppUnknown(), "\\Image", "Background", RGB( 255, 255, 255 ) );
		byte	dib[3];
		dib[0] = (byte)GetBValue(cr); 
		dib[1] = (byte)GetGValue(cr);
		dib[2] = (byte)GetRValue(cr);
		color	*ppcolor[10];
		for( int i = 0; i < 10; i++ )
			ppcolor[i] = &colorsBacks[i];
		IUnknown* punk;
		imageSrc->GetColorConvertor(&punk);
		sptrIColorConvertor5 sptrCCScr(punk);
		punk->Release();
		sptrCCScr->ConvertDIBToImageEx( dib, ppcolor, 1, true, imageSrc.GetColorsCount() );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//implements

IMPLEMENT_DYNCREATE(CImage, CDataObjectBase)

// {19C18616-43F4-11d3-A611-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CImage, "ImageDoc.Image", 
0x19c18616, 0x43f4, 0x11d3, 0xa6, 0x11, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);

// {C3AA6C52-4430-11d3-A611-0090275995FE}
static const GUID clsidImageDataObjectInfo = 
{ 0xc3aa6c52, 0x4430, 0x11d3, { 0xa6, 0x11, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };


DATA_OBJECT_INFO(IDS_IMAGE_TYPE, CImage, CImage::c_szType, clsidImageDataObjectInfo, IDI_THUMBNAIL_IMAGE)

BEGIN_INTERFACE_MAP(CImage, CDataObjectBase)
	INTERFACE_PART(CImage, IID_IImage, Image)
	INTERFACE_PART(CImage, IID_IImage2, Image)
	INTERFACE_PART(CImage, IID_IImage3, Image)
	INTERFACE_PART(CImage, IID_IImage4, Image)
	INTERFACE_PART(CImage, IID_IClipboard, Clipboard)
	INTERFACE_PART(CImage, IID_ICompatibleObject, CompatibleObject )
	INTERFACE_PART(CImage, IID_ICompatibleObject2, CompatibleObject )
	INTERFACE_PART(CImage, IID_ICompatibleObject3, CompatibleObject )
	INTERFACE_PART(CImage, IID_IPaintInfo, Info)
	INTERFACE_PART(CImage, IID_ICalibr, Calibr)
	INTERFACE_PART(CImage, IID_IRectObject, Rect)
END_INTERFACE_MAP()

BEGIN_DISPATCH_MAP(CImage, CDataObjectBase)
	//{{AFX_DISPATCH_MAP(CImage)
	DISP_FUNCTION(CImage, "GetWidth", GetWidth, VT_I4, VTS_NONE)
	DISP_FUNCTION(CImage, "GetHeight", GetHeight, VT_I4, VTS_NONE)
	DISP_FUNCTION(CImage, "CreateNew", CreateNew, VT_EMPTY, VTS_I4 VTS_I4 VTS_BSTR)
	DISP_FUNCTION(CImage, "CreateNewEx", CreateNewEx, VT_EMPTY, VTS_I4 VTS_I4 VTS_BSTR VTS_I4)
	DISP_FUNCTION(CImage, "GetPosX", GetPosX, VT_I4, VTS_NONE)
	DISP_FUNCTION(CImage, "GetPosY", GetPosY, VT_I4, VTS_NONE)
	DISP_FUNCTION(CImage, "SetOffset", SetOffset, VT_EMPTY, VTS_I4 VTS_I4 VTS_BOOL)
	DISP_FUNCTION(CImage, "GetColorSystem", GetColorSystem, VT_BSTR, VTS_NONE)
	DISP_FUNCTION(CImage, "GetColorsCount", GetColorsCount, VT_I4, VTS_NONE)
	DISP_FUNCTION(CImage, "GetCalibration", GetCalibration, VT_EMPTY, VTS_PVARIANT VTS_PVARIANT)
	DISP_FUNCTION(CImage, "SetCalibration", SetCalibration, VT_EMPTY, VTS_R8 VTS_BSTR)
	DISP_FUNCTION(CImage, "CalcMinMaxBrightness", CalcMinMaxBrightness, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CImage, "GetMinBrightness", GetMinBrightness, VT_I4, VTS_NONE)
	DISP_FUNCTION(CImage, "GetMaxBrightness", GetMaxBrightness, VT_I4, VTS_NONE)
	DISP_FUNCTION(CImage, "GetAverageBrightness", GetAverageBrightness, VT_I4, VTS_NONE)
	DISP_FUNCTION(CImage, "SetProperty", SetBagProperty, VT_BOOL, VTS_BSTR VTS_VARIANT)
	DISP_FUNCTION(CImage, "GetProperty", GetBagProperty, VT_VARIANT, VTS_BSTR )
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()
	//{{AFX_MSG_MAP(CImage)
	//}}AFX_MSG_MAP


IMPLEMENT_UNKNOWN(CImage, Image)
IMPLEMENT_UNKNOWN(CImage, CompatibleObject)
IMPLEMENT_UNKNOWN(CImage, Info);
IMPLEMENT_UNKNOWN(CImage, Calibr);
IMPLEMENT_UNKNOWN(CImage, Rect);

HRESULT CImage::XInfo::GetInvalidRect( RECT* pRect )
{
	METHOD_PROLOGUE(CImage, Info)
	if(pRect)
	{
		*pRect = CRect(pThis->m_ptOffset, CSize(pThis->m_cx, pThis->m_cy));
		return S_OK;
	}
	else return E_INVALIDARG;
}


char *CImage::c_szType = szTypeImage;


/////////////////////////////////////////////////////////////////////////////////////////
//CImage implementation
CImage::CImage()
{
	m_cx = m_cy = 0;
	m_pcolors = 0;
	m_pbytes = 0;
	m_iPanesCount = 0;
	m_ppRows = 0;
	m_ppRowMasks = 0;
	m_ptOffset = CPoint( 0, 0 );
	m_bVirtual = false;

	//init clipboard support
	m_arrAvailFormats.Add(CF_DIB);

	//paul commented 22.09.2003, cos this func call in create new
	//::GetDefaultCalibration( &m_fCalibr, &m_guidC );
	m_bCalibrOk = false;
	m_bMinMaxBriInit = false;
	m_byMinBri = m_byMaxBri = m_byAvrBri = 0;
}

CImage::~CImage()
{
	DeInit();
}

void CImage::InitCalibr(bool bForce)
{
	if (!m_bCalibrOk || bForce)
	{
		::GetDefaultCalibration( &m_fCalibr, &m_guidC );
		m_bCalibrOk = true;
		::GetCalibration( GetControllingUnknown(), &m_fCalibr, &m_guidC );
	}
}

HRESULT CImage::XCalibr::GetCalibration( double *pfPixelPerMeter, GUID *pguidC )
{
	METHOD_PROLOGUE_EX(CImage, Calibr);
	pThis->InitCalibr();
	if( pfPixelPerMeter )*pfPixelPerMeter = pThis->m_fCalibr;
	if( pguidC )*pguidC =pThis->m_guidC;
	return S_OK;
}
HRESULT CImage::XCalibr::SetCalibration( double fPixelPerMeter, GUID *guidC )
{
	METHOD_PROLOGUE_EX(CImage, Calibr);
	pThis->m_fCalibr = fPixelPerMeter;
	pThis->m_guidC = *guidC;
	return S_OK;
}

//interface IImage implementation
HRESULT CImage::XImage::CreateImage( int cx, int cy, BSTR bstrCCNamed, int nPaneNum )
{
	_try_nested(CImage, Image, CreateImage)
	{
		_bstr_t	bstrCC( bstrCCNamed );

		if( !pThis->_CreateNew( cx, cy, bstrCC, nPaneNum ) )
			return E_INVALIDARG;

		
		return S_OK;
	}
	_catch_nested;
}

HRESULT CImage::XImage::FreeImage()
{
	_try_nested(CImage, Image, CreateImage)
	{
		pThis->_FreeImage();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CImage::XImage::GetColorConvertor( IUnknown **ppunk )
{
	METHOD_PROLOGUE_EX(CImage, Image);
	{
		*ppunk = pThis->m_sptrCC2;

		if( *ppunk )
			(*ppunk)->AddRef();

		return S_OK;
	}
}

HRESULT CImage::XImage::GetRow( int nRow, int nPane, color **ppcolor )
{
	//_try_nested(CImage, Image, GetRow)
	METHOD_PROLOGUE_EX(CImage, Image);
	{
		*ppcolor = pThis->_GetRow( nRow, nPane );
		return S_OK;
	}
	//_catch_nested;
}

HRESULT CImage::XImage::GetSize( int *pnCX, int *pnCY )
{
	METHOD_PROLOGUE_EX(CImage, Image);
	{
		if( pnCX )	*pnCX = pThis->m_cx;
		if( pnCY )	*pnCY = pThis->m_cy;

		return S_OK;
	}
}

HRESULT CImage::XImage::CreateVirtual( RECT rect )
{
	_try_nested(CImage, Image, CreateVirtual)
	{
		pThis->_CreateVirtual( rect );
		return S_OK;
	}
	_catch_nested;
}


HRESULT CImage::XImage::GetImageFlags( DWORD *pdwFlags )
{
	_try_nested(CImage, Image, GetImageFlags)
	{
		*pdwFlags = 0;

		if( pThis->IsVirtual() )
			*pdwFlags |= ifVirtual;
		if( pThis->m_ppRowMasks != 0 )
			*pdwFlags |= ifMasked;
		if( pThis->m_arrContours.GetSize() )
			*pdwFlags |= ifContoured;

		return S_OK;
	}
	_catch_nested;
}

HRESULT CImage::XImage::GetOffset( POINT *pt )
{
	METHOD_PROLOGUE_EX(CImage, Image);
	{
		//if( pThis->m_pParent )
			*pt = pThis->m_ptOffset;
		/*else
			*pt = CPoint( 0, 0 );*/
		return S_OK;
	}
}


HRESULT CImage::XImage::SetOffset( POINT pt, BOOL bMoveImage )
{
	_try_nested(CImage, Image, SetOffset)
	{
		pThis->_SetOffset( pt, bMoveImage == TRUE );
		return S_OK;
	}
	_catch_nested;
}

HRESULT CImage::XImage::GetRowMask( int nRow, BYTE **ppByte )
{
	//_try_nested(CImage, Image, GetRowMask)
	METHOD_PROLOGUE_EX(CImage, Image);
	{
		*ppByte = pThis->_GetRowMask( nRow );
		return S_OK;
	}
	//_catch_nested;
}

HRESULT CImage::XImage::InitRowMasks()
{
	_try_nested(CImage, Image, GetRowMask)
	{
		pThis->_InitRowMasks();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CImage::XImage::InitContours()
{
	_try_nested(CImage, Image, GetRowMask)
	{
		pThis->_InitContours();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CImage::XImage::FreeContours()
{
	_try_nested(CImage, Image, FreeContours)
	{
		pThis->_FreeContours();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CImage::XImage::GetContoursCount( int *piCount )
{
	METHOD_PROLOGUE_EX(CImage, Image);
	{
		*piCount = pThis->m_arrContours.GetSize();
		return S_OK;
	}
}

HRESULT CImage::XImage::GetContour( int nPos, Contour **ppContour )
{
	METHOD_PROLOGUE_EX(CImage, Image);
	{
		*ppContour = (Contour *)pThis->m_arrContours[nPos];
		return S_OK;
	}
}

HRESULT CImage::XImage::SetContour( int nPos, Contour *pContour )
{
	METHOD_PROLOGUE_EX(CImage, Image);
	{
		pThis->_InsertContour( nPos, pContour );
		return S_OK;
	}
}

HRESULT CImage::XImage::AddContour( Contour *pContour )
{
	METHOD_PROLOGUE_EX(CImage, Image);
	{
		pThis->m_arrContours.Add( pContour );
		return S_OK;
	}
}

HRESULT CImage::XImage::AddPane(int nPane )
{
	_try_nested(CImage, Image, AddPane)
	{
		return pThis->_AddPane( nPane );
	}
	_catch_nested;
}

HRESULT CImage::XImage::RemovePane(int nPane )
{
	_try_nested(CImage, Image, RemovePane)
	{
		return pThis->_RemovePane( nPane );
	}
	_catch_nested;
}

HRESULT CImage::XImage::GetPanesCount(int *nSz )
{
	_try_nested(CImage, Image, RemovePane)
	{
		if( !nSz ) 
			return S_FALSE;

		return pThis->_GetPanesCount( nSz );
	}
	_catch_nested;
}


//de-initialize the image. Free the memory
void CImage::DeInit( bool bRemoveMask )
{
	if( m_pcolors )
		delete m_pcolors;
	

	if( bRemoveMask )
	{
		if( m_pbytes )delete m_pbytes;
		if( m_ppRowMasks )delete m_ppRowMasks;
		
		
		m_ppRowMasks = 0;
		m_pbytes = 0;

		_FreeContours();
	}

	if( m_ppRows )delete m_ppRows;
	m_ppRows = 0;

	m_bVirtual = false;
	m_cx = m_cy = 0;
	
	m_pcolors = 0;
	m_iPanesCount = 0;
	m_sptrCC2 = 0;
}

//create a new image with specified size and color ussage
bool CImage::_CreateNew( int cx, int cy, const char *szColorConvertorName, int nPaneNum, bool bGetCalibr )
{
	VERIFY( strlen(szColorConvertorName) );
	
	DeInit( !(m_cx == cx && m_cy == cy) );

	//paul 22.09.2003, cos read on serialize
	if( bGetCalibr )
		InitCalibr(true);

	m_sptrCC2 = GetCCByName(szColorConvertorName,false);

	m_cx = cx;
	m_cy = cy;
	m_iPanesCount = 0;

	if (m_sptrCC2 == 0)
		return false;

	if( !nPaneNum )
		return false;

	if( nPaneNum == -1 )
		m_sptrCC2->GetColorPanesDefCount( &m_iPanesCount );
	else
		m_iPanesCount = nPaneNum;

	//sergey 29/11/06
    // m_pcolors = new color[cx-1*cy-2*m_iPanesCount];
	m_pcolors = new color[cx*cy*m_iPanesCount];
	//end

	if( !m_pcolors )
		return false;

	ASSERT( !m_ppRows );

	//sergey 29/11/06
	//m_ppRows = new color*[m_cy-2*m_iPanesCount];
	m_ppRows = new color*[m_cy*m_iPanesCount];
	//end

	if( !m_ppRows )
	{
		delete m_pcolors;
		return false;
	}

	//sergey 29/11/06
	//int j=0;
	//for( int i = 0; i < m_cy-2*m_iPanesCount; i++ )
	//{
	//	m_ppRows[i] = m_pcolors+m_cx-1*i;
	//	j++;
	//}

	for( int i = 0; i < m_cy*m_iPanesCount; i++ )
		m_ppRows[i] = m_pcolors+m_cx*i;

	//for( int i = 0; i < m_cy*m_iPanesCount; i++ )
		//m_ppRows[i] = m_pcolors+m_cx*i;
	//end
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
//create a virtual image
bool CImage::_CreateVirtual( CRect rc, bool bGetCalibr )
{
	DeInit( false );

	//paul 22.09.2003, cos read on serialize
	if( bGetCalibr )
		InitCalibr(true);

	m_bVirtual = true;

	m_ptOffset = rc.TopLeft();
	m_cx = rc.Size().cx;
	m_cy = rc.Size().cy;

	if( m_pParent )
	{
		CImageWrp	source( m_pParent );

		ASSERT( m_pParent != 0 );
		int	nMaxWidth = source.GetWidth();
		int	nMaxHeight = source.GetHeight();

		rc.left = max( 0, min( nMaxWidth, rc.left ) );
		rc.right = max( 0, min( nMaxWidth, rc.right ) );
		rc.top = max( 0, min( nMaxHeight, rc.top ) );
		rc.bottom = max( 0, min( nMaxHeight, rc.bottom ) );

		m_ptOffset = rc.TopLeft();
		m_cx = rc.Size().cx;
		m_cy = rc.Size().cy;

		m_iPanesCount = source.GetColorsCount();
		if(m_iPanesCount<1) 
			return false;

		if( source.IsEmpty() )
			return false;

		if( rc.IsRectEmpty() )
			return false;

		source->GetColorConvertor( (IUnknown**)&m_sptrCC2 );

		m_ppRows = new color*[m_cy*m_iPanesCount];

		if( !m_ppRows )
			return false;

		int	nRow = 0, i, c;

		for( i = 0; i < m_cy; i++ )
			for( c = 0; c < m_iPanesCount; c++ )
				m_ppRows[nRow++] = source.GetRow( i+m_ptOffset.y, c )+m_ptOffset.x;

		if( m_pbytes )
			return true;
	}

	/*m_ppRowMasks = new byte*[m_cy];
	for( int i = 0; i < m_cy; i++ )
		m_ppRowMasks[i] = m_pbytes+m_cx*i;*/

	return true;
	
}

//free the image
void CImage::_FreeImage()
{
	DeInit();
}

//return the image row
color *CImage::_GetRow( int nRow, int nPane )
{
	if( !m_ppRows )
		return 0;
	nPane = max(0, min(nPane, m_iPanesCount-1));
	return m_ppRows[nRow*m_iPanesCount+nPane];
}

#define DUMP_RL \
{ \
	*(pCompressed++)=old; \
	*(pCompressed++)=len; \
	old = *pData; len=0; \
	nCompressed+=2; \
}
static int CompressRLE(byte **ppData, long *pnDataRest,
					   byte *pCompressed, long nCompressedBufSize)
{	// пожать буфер и сдвинуть указатели
	byte *pData=*ppData; int nDataRest=*pnDataRest;
	int nCompressed=0;
	int old=*pData, len=0;
	while(nDataRest)
	{
		if(*pData!=old || len==255) DUMP_RL;
		pData++; nDataRest--; len++;
		if(nCompressed>=nCompressedBufSize-2) break;
	}
	if(len!=0) DUMP_RL;
	*ppData = pData; *pnDataRest=nDataRest;
	return nCompressed;
}

static void DecompressRLE(byte **ppData, long *pnDataRest,
						 byte *pCompressed, long nCompressed)
{	// распаковать буфер и сдвинуть указатели
	byte *pData=*ppData; int nDataRest=*pnDataRest;
	int old=0, len=0;
	while(nDataRest)
	{
		if(len==0)
		{
			if(nCompressed>=2)
			{
				old=*(pCompressed++);
				len=*(pCompressed++);
				nCompressed -= 2;
			}
			else break;
		}
		*(pData++)=old; nDataRest--;
		len--;
	}
	*ppData = pData; *pnDataRest=nDataRest;
}

LPSTREAM AFXAPI _AfxCreateMemoryStream()
{
	LPSTREAM lpStream = NULL;

	// Create a stream object on a memory block.
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, 0);
	if (hGlobal != NULL)
	{
		if (FAILED(CreateStreamOnHGlobal(hGlobal, TRUE, &lpStream)))
		{
			TRACE0("CreateStreamOnHGlobal failed.\n");
			GlobalFree(hGlobal);
			return NULL;
		}

		ASSERT_POINTER(lpStream, IStream);
	}
	else
	{
		TRACE0("Failed to allocate memory for stream.\n");
		return NULL;
	}

	return lpStream;
}

//store/restore the image to archive
bool CImage::SerializeObject( CStreamEx &ar, SerializeParams *pparams )
{
	PROFILE_TEST( "CImage::SerializeObject" )		

	BOOL	bSaveRows = (pparams->flags & sf_OnlyMaskOfImage)==0;
	BOOL	bEnableCompression = (pparams->flags & sf_ImageCompressed)==sf_ImageCompressed;

#ifdef DEMOVERSION
		bEnableCompression = false; // полный запрет сжатия для Demo-версии
#endif

	if( !CheckInterface( GetAppUnknown(), IID_ICompressionManager ) )
		bEnableCompression = false;

	BSTR	bstr = 0;
	if( m_sptrCC2 != 0 )m_sptrCC2->GetCnvName( &bstr );
	CString	strCCName = bstr;
	::SysFreeString( bstr );

	if( strCCName != "RGB" &&
//		strCCName != "YUV" &&
		strCCName != "GRAY" &&
		strCCName != "CGH")
		bEnableCompression = false;

	



	enum ImageFileFlags
	{
		iffEmpty = 0,
		iffHasRows = 1,
		iffHasMasks = 2,
		iffHasOwnRows = 4,
		iffHasOwnMasks = 8,
		iffHasContours = 16,
		iffVersionControl = 32,
		iffVirtual = 64,
		iffCompressMasks = 128
	};
	
	CDataObjectBase::SerializeObject( ar, pparams );

	if( ar.IsStoring() )
	{
		long	lInitState = iffEmpty;

		BOOL	bParentKilled=FALSE;
		INamedDataObject2Ptr ptrParentNDO(m_pParent);
		if(ptrParentNDO!=0)
		{
			IUnknown *punkData=0;
			ptrParentNDO->GetData(&punkData);
			if(punkData==0) bParentKilled=TRUE;
			if(punkData!=0) punkData->Release();
		}

		bool bDetachParent = (pparams->flags & sf_DetachParent) || bParentKilled;

		IUnknown	*punkP = 0;
		if( bDetachParent && m_pParent )
		{
			punkP = m_pParent;
			m_xDataObj.SetParent( 0, sdfCopyParentData );
		}


		if( m_ppRows && bSaveRows )
			lInitState |= iffHasRows;
		if( m_pcolors && bSaveRows )
		{
			lInitState |= iffHasOwnRows;
		}
		else
		{
			bEnableCompression = false;
		}

		if( !bSaveRows )
			lInitState |= iffVirtual;

		if( m_ppRowMasks )
			lInitState |= iffHasMasks;
		if( m_pbytes )
			lInitState |= iffHasOwnMasks;
		if( m_arrContours.GetSize() )
			lInitState |= iffHasContours;
		if(m_pbytes && bEnableCompression)
			lInitState |= iffCompressMasks;

#ifdef DEMOVERSION
		if(lInitState & iffHasOwnRows)
		{			
			IImage2Ptr	ptrImage( GetControllingUnknown() );
			vlong signature = DEMOVALUE;
			StoreVLong(ptrImage, signature);
		}
#endif

		

		if( lInitState == iffEmpty )
		{
			ar<<lInitState;
			return true;
		}

		lInitState |= iffVersionControl;		
		ar<<lInitState;

		long lImageVer = 9;
		ar<<lImageVer;
		ar<<bEnableCompression;

		bool bUpdateSignature = ::GetValueInt( ::GetAppUnknown(), "\\Signature","UpdateSignature", 0 ) != 0;
		if( bUpdateSignature && (lInitState & iffHasOwnRows) )
		{ // если надо - обновим подпись
			IImage2Ptr	ptrImage( GetControllingUnknown() );
			byte digest_updated[16] = {0};
			CalcSignature(ptrImage, digest_updated, sizeof(digest_updated));
			// установка
			vlong v1=0;
			unsigned* p = (unsigned*)digest_updated;
			for(int i=0; i<16/sizeof(unsigned); i++) v1.set_data(i,p[i]);
			_bstr_t bstrKeyPath = "\\vt5\\crypto\\private_key.h";
			// bstrKeyPath = ::MakeAppPathName( "private_key.h" );
			bstrKeyPath = ::GetValueString( ::GetAppUnknown(), "\\Signature", "PrivateKeyFile", bstrKeyPath );
			private_key key2 = read_key(bstrKeyPath);
			vlong v_new = 0;
			if(key2.p==0) ::MessageBox(0, "Private key not found", "Error", MB_OK | MB_ICONERROR );
			else v_new = key2.decrypt(v1);
			StoreVLong(ptrImage, v_new);
		}

		if( bEnableCompression )
		{
			ICompressionManager2Ptr ptrCompress( ::GetAppUnknown() );
			if( ptrCompress )
			{
				CMemFile file;
				CArchive arch( &file, CArchive::store );
				CArchiveStream stream( &arch );

				IImage2Ptr	ptrImage( GetControllingUnknown() );
				vlong signature = ExtractVLong(ptrImage);

				bool bHideSignature = ::GetValueInt( ::GetAppUnknown(), "\\ImageCompress","HideSignature", 1 ) != 0;
				if(bHideSignature)
				{
					byte zero[2048/8] = {0};
					StoreSignature(ptrImage, zero, sizeof(zero));
				}

				long lCompressVersion = 1;
				HRESULT hr = ptrCompress->Compress2( GetControllingUnknown(), &stream, &lCompressVersion ); 
				if( hr == S_OK )
				{
					arch.Flush();	
					file.Flush();
					DWORD dwCompSize = (DWORD)file.GetLength();
					if( dwCompSize > 0 )
					{
						BYTE* pBuf = file.Detach();	
						ar<<lCompressVersion;
						ar<<m_ptOffset.x<<m_ptOffset.y;

						ar<<(DWORD)dwCompSize;
						ar.Write( pBuf, (UINT)dwCompSize );						

						arch.Close();
						file.Close();

						{ // сохранение подписи
							//bool bUpdateSignature = ::GetValueInt( ::GetAppUnknown(), "\\ImageCompress","UpdateSignature", 0 ) != 0;
							if(bUpdateSignature)
							{ // если надо - обновим подпись
								CMemFile file(pBuf, (UINT)dwCompSize );
								CArchive arch( &file, CArchive::load );
								CArchiveStream stream( &arch );

								HRESULT hr = ptrCompress->DeCompress2( GetControllingUnknown(), &stream, lCompressVersion ); 

								arch.Close();
								file.Close();

								byte digest_updated[16] = {0};
								CalcSignature(ptrImage, digest_updated, sizeof(digest_updated));
								// установка
								vlong v1=0;
								unsigned* p = (unsigned*)digest_updated;
								for(int i=0; i<16/sizeof(unsigned); i++) v1.set_data(i,p[i]);
								_bstr_t bstrKeyPath = "\\vt5\\crypto\\private_key.h";
								// bstrKeyPath = ::MakeAppPathName( "private_key.h" );
								bstrKeyPath = ::GetValueString( ::GetAppUnknown(), "\\Signature", "PrivateKeyFile", bstrKeyPath );
								private_key key2 = read_key(bstrKeyPath);
								if(key2.p==0) ::MessageBox(0, "Private key not found", "Error", MB_OK | MB_ICONERROR );
								else signature = key2.decrypt(v1);
							}
							DWORD dwLen = signature.get_data_size();
							unsigned *pData = signature.get_data_buf();
							ar << dwLen;
							ASSERT ( dwLen <= 2048/8/sizeof(unsigned) );
							dwLen = min( dwLen, 2048/8/sizeof(unsigned) );
							ar.Write( pData, dwLen*sizeof(unsigned) );
						}

						delete pBuf;	pBuf = 0;
					}
				}
				else
				{
					ASSERT( FALSE );
				}
			}
		}
		else
		{
			ar<<m_cx<<m_cy<<m_ptOffset.x<<m_ptOffset.y;
			ar<<strCCName;
			ar << m_iPanesCount;

			if( lInitState & iffHasOwnRows )
			{
				ar.Write( m_pcolors, m_cx*m_cy*m_iPanesCount*sizeof( color ) );
			}
		}
		if( lInitState & iffHasOwnMasks )
		{
			bool bCompressMasks = (lInitState & iffCompressMasks)!=0;
			if(bCompressMasks)
			{
				byte CompressBuf[16384];
				byte *pData=m_pbytes;
				long nBytesRest=m_cx*m_cy;
				while(nBytesRest)
				{
					long nBytesCompressed = 
						CompressRLE(&pData, &nBytesRest,
						CompressBuf, sizeof(CompressBuf));
					if(nBytesCompressed)
					{
						ar << nBytesCompressed; // пишем длину блока
						ar.Write( CompressBuf, nBytesCompressed );
					}
				}
			}
			else ar.Write( m_pbytes, m_cx*m_cy );
		}

		if( lInitState & iffHasContours )
		{
			long	nCount = m_arrContours.GetSize();
			ar << nCount;
			for( int i = 0; i < nCount; i++ )
			{
				Contour	*pc = (Contour	*)m_arrContours[i];
				if( !pc )
				{
					ar << 0;
					continue;
				}
				ar<<pc->nContourSize;
				ar<<pc->lFlags;
				ar<<pc->lReserved;
				if( pc->nContourSize )
					ar.Write( pc->ppoints, sizeof( ContourPoint )*pc->nContourSize );
			}
		}
		InitCalibr();
		ar<<m_fCalibr<<m_guidC;

		IStream *punkStream = _AfxCreateMemoryStream();
		if( punkStream )
		{
			((INamedPropBagSer*)this)->Store( punkStream );

			COleStreamFile sfile( punkStream );
			DWORD dwLen = (DWORD)sfile.GetLength();
			if( dwLen )
			{
				BYTE *lpBuf = new BYTE[dwLen];

				sfile.Seek( 0, CFile::begin );
				sfile.Read( (void*)lpBuf, dwLen );
							
				ar << dwLen;
				ar.Write( lpBuf, dwLen );

				delete []lpBuf;
			}
			punkStream->Release();
		}

		if( punkP )
			m_xDataObj.SetParent( punkP, sdfCopyLocalData|sdfAttachParentData );
	}
	else
	{
		DeInit();

		long	lInitState = 0;
		CString	strColorCnv;

		ar>>lInitState;

		if( lInitState == iffEmpty )
			return true;

	
		if( lInitState & iffVirtual && m_pParent )
			m_xDataObj.SetParent( 0, 0 );

		long lImageVer = 0;

		if( lInitState & iffVersionControl )
			ar>>lImageVer;

		if( lImageVer == 3 )
		{
			ar>>m_fCalibr>>m_guidC;
			m_bCalibrOk = true; // BT5070
		}

		BOOL bImageWasCompress = FALSE;

		if( lImageVer >= 2 )
			ar>>bImageWasCompress;

		if( bImageWasCompress )
		{
			ICompressionManager2Ptr ptrCompress( ::GetAppUnknown() );
			if( ptrCompress == NULL )
			{
				AfxMessageBox( IDS_WARNING_NO_COMPRESSOR, MB_ICONSTOP );
				return false;
			}

			long lCompressVersion = 1;
			ar>>lCompressVersion;
			ar>>m_ptOffset.x>>m_ptOffset.y;

			DWORD dwSize = 0;
			ar>>(DWORD)dwSize;
			if( dwSize <=0 )
				return false;

			BYTE* pBuf = new BYTE[dwSize];			

			if( !pBuf )
				return false;

			ar.Read( pBuf, (UINT)dwSize );

			CMemFile file(pBuf, (UINT)dwSize );
			CArchive arch( &file, CArchive::load );
			CArchiveStream stream( &arch );
						
			HRESULT hr = ptrCompress->DeCompress2( GetControllingUnknown(), &stream, lCompressVersion ); 

			arch.Close();
			file.Close();

			delete pBuf;	pBuf = 0;

			if( lImageVer < 5 )
				return false;

			if( hr != S_OK )
				return false;

			if(lImageVer >= 8)
			{ // прочитать подпись и запихать ее в image
				IImage2Ptr	ptrImage( GetControllingUnknown() );
				vlong signature = 0;
				DWORD dwLen=0;
				ar >> dwLen;
				ASSERT ( dwLen <= 2048/8/sizeof(unsigned) );
				dwLen = min( dwLen, 2048/8/sizeof(unsigned) );
				smart_alloc(buf, unsigned, dwLen);
				ar.Read( buf, dwLen*sizeof(unsigned) );
				for(unsigned i=0; i<dwLen; i++) signature.set_data(i, buf[i]);
				StoreVLong(ptrImage, signature);
			}
		}
		else
		{
			ar>>m_cx>>m_cy>>m_ptOffset.x>>m_ptOffset.y;
			ar>>strColorCnv;

			m_iPanesCount = -1;
			if( lImageVer >= 6 )
				ar >> m_iPanesCount;


			//if image has own rows, load it

			if( lInitState & iffHasOwnRows )
			{
				if( !_CreateNew( m_cx, m_cy, strColorCnv, m_iPanesCount, false ) )
					return false;

				if( lInitState & iffHasOwnRows )
					ar.Read( m_pcolors, m_cx*m_cy*m_iPanesCount*sizeof( color ) );
				else
					ZeroMemory( m_pcolors, m_cx*m_cy*m_iPanesCount*sizeof( color ) );
			}
			else	//else image is virtual
				if( !_CreateVirtual( CRect( m_ptOffset, CSize( m_cx, m_cy ) ), false ) )
					return false;
		}

		if( lInitState & iffHasMasks )
			_InitRowMasks();
		//if object has own row masks
		if( lInitState & iffHasOwnMasks )
		{
			ASSERT(m_pbytes);
			bool bCompressMasks = (lInitState & iffCompressMasks)!=0;
			if(bCompressMasks)
			{
				byte CompressBuf[16384];
				byte *pData=m_pbytes;
				long nBytesRest=m_cx*m_cy;
				while(nBytesRest)
				{
					long nBytesCompressed = 0;
					ar >> nBytesCompressed; // читаем длину блока
					ar.Read( CompressBuf, nBytesCompressed );
					DecompressRLE(&pData, &nBytesRest,
						CompressBuf, nBytesCompressed);
				}
			}
			else ar.Read( m_pbytes, m_cx*m_cy );
		}

		if( lInitState & iffHasContours )
		{
			long	nCount;
			ar >> nCount;
			for( int i = 0; i < nCount; i++ )
			{
				Contour	*pc = ContourCreate();
				m_arrContours.Add( pc );
				
				ar>>pc->nContourSize;

				if( !pc->nContourSize )
					continue;

				ar>>pc->lFlags;
				ar>>pc->lReserved;

				pc->nAllocatedSize=pc->nContourSize;
				pc->ppoints = new ContourPoint[pc->nContourSize];

				if( !pc->ppoints )
					return false;


				ar.Read( pc->ppoints, sizeof( ContourPoint )*pc->nContourSize );
			}
		}
		if( lImageVer >= 4 )
		{
			ar>>m_fCalibr>>m_guidC;
			m_bCalibrOk = true; // BT5070
		}

#ifdef DEMOVERSION
		bool bDontCheckSignature = (pparams->flags & sf_DontCheckSignature) != 0;
		if( pparams->flags & sf_DontCheckSignatureInBig )
		{
			if( (m_cx<200) && (m_cy<200) ) bDontCheckSignature=true;
		}
        
		if( !bDontCheckSignature )
		{
			vlong v = ExtractVLong(IImage2Ptr(GetControllingUnknown()));
			if(v != DEMOVALUE) 
				CheckImageSignature(GetControllingUnknown());
		}
#endif
	
		IStream *punkStream = _AfxCreateMemoryStream();
		if( lImageVer >= 9 && punkStream )
		{
			DWORD dwLen = 0;

			ar >> dwLen;
			if( dwLen )
			{
				BYTE *lpBuf = new BYTE[dwLen];

				ar.Read( (LPVOID)lpBuf, dwLen );

				COleStreamFile sfile( punkStream );
				sfile.Write( (void*)lpBuf, dwLen );
				sfile.Seek( 0, CFile::begin );

				((INamedPropBagSer*)this)->Load( punkStream );
				delete []lpBuf;
			}
		}

		if( punkStream )
			punkStream->Release();
	}
	return true;
}


//return the mask of specified row
byte *CImage::_GetRowMask( int nRow )
{
	if( !m_pbytes )
		return CImageRowHelper::s_helper.m_pdummyRow;
	ASSERT( m_ppRowMasks[nRow] );
	return m_ppRowMasks[nRow];

}


//initialze the row masks
void CImage::_InitRowMasks()
{
	if( m_pbytes )
		return;
//free all row data (if exist)
	if( m_ppRowMasks )
		delete m_ppRowMasks;
	if( m_pbytes )
		delete m_pbytes;
//alloc the rows structure
	m_ppRowMasks = new byte*[ m_cy ];

	if( !m_ppRowMasks )
		 return;

	//if( !IsVirtual() )
	{
		m_pbytes = new byte[m_cx*m_cy];

		if( !m_pbytes )
			return ;
		memset( m_pbytes, 255, m_cx*m_cy );

		for( int i = 0; i < m_cy; i++ )
			m_ppRowMasks[i] = m_pbytes+m_cx*i;

	}
	/*else
	{
		CImageWrp	src( m_pParent );

		for( int i = 0; i < m_cy; i++ )
			m_ppRowMasks[i] = src.GetRowMask( i+m_ptOffset.x )+m_ptOffset.y;
	}*/
}

void CImage::_SetOffset( CPoint pt, bool bMove )
{
	if( bMove )
		Detach();

	CPoint	ptDelta;
	ptDelta.x = pt.x-m_ptOffset.x;
	ptDelta.y = pt.y-m_ptOffset.y;
	m_ptOffset = pt;

	for( int i = 0; i < m_arrContours.GetSize(); i++ )
		::ContourOffset( (Contour*)m_arrContours[i], ptDelta );
}


bool CImage::ExchangeDataWithParent( DWORD flags )
{
	int	i, j, c;

	bool	bMaskOnlyImage = m_ppRows == 0;
	bool	bAttachParentData = (flags & sdfAttachParentData) != 0 || bMaskOnlyImage;
	bool	bCopyParentData = (flags & sdfCopyParentData) != 0;
	bool	bCopyLocalData = (flags & sdfCopyLocalData) != 0;
	bool	bClean = (flags & sdfClean) != 0;

	CImageWrp	imageDest( m_pParent );
	CImageWrp	imageSrc( GetControllingUnknown() );


	if( imageDest.IsEmpty()&&!imageSrc.IsEmpty() )
	{
		CRect	rect = imageSrc.GetRect();
		if( !imageDest.CreateNew( rect.right, rect.bottom, imageSrc.GetColorConvertor() ) )
			return false;
	}

	if( !bMaskOnlyImage && !imageSrc.IsEmpty()&&(imageDest.GetColorConvertor() != imageSrc.GetColorConvertor()) )
	{
		ASSERT( !imageSrc.IsVirtual() );

		CImageWrp	imageNew;
		imageNew.CreateCompatibleImage( imageSrc, true );
		long	colors = imageSrc.GetColorsCount();
		long	cx = imageSrc.GetWidth(), cy = imageSrc.GetHeight();

		long	y, c;

		for( y = 0; y < cy; y++ )
		{
			for( c = 0; c < colors; c++ )
				memcpy( imageNew.GetRow( y, c ), imageSrc.GetRow( y, c ), cx*sizeof( color ) );
			//memcpy( imageNew.GetRowMask( y ), imageSrc.GetRowMask( y ), cx*sizeof( byte ) );
		}
		imageSrc.CreateCompatibleImage( imageNew, false, NORECT, imageDest.GetColorConvertor() );

		colors = max(imageNew.GetColorsCount(), imageSrc.GetColorsCount());
		double*		pLab= new double[cx*3];

		if( !pLab )
			return 0;

		IUnknown* punk;
		imageNew->GetColorConvertor(&punk);
		sptrIColorConvertor3 sptrCCScr(punk);
		punk->Release();
		imageSrc->GetColorConvertor(&punk);
		sptrIColorConvertor3 sptrCCDest(punk);
		punk->Release();

		color	**ppcolorscr = new color*[colors];

		if( !ppcolorscr )
		{
			delete pLab;
			return 0;
		}

		color	**ppcolordest = new color*[colors];

		if( !ppcolordest )
		{
			delete pLab;
			delete ppcolorscr;
			return 0;
		}

		for( y = 0; y < cy; y++ )
		{
			for( c = 0; c  < colors; c++ )
			{
				ppcolorscr[c] = imageNew.GetRow( y, c );
				ppcolordest[c] = imageSrc.GetRow( y, c );
			}
			sptrCCScr->ConvertImageToLAB(ppcolorscr, pLab, cx);
			sptrCCDest->ConvertLABToImage(pLab, ppcolordest, cx);
		}

		delete pLab;
		delete ppcolorscr;
		delete ppcolordest;
	}

	CRect	rectDest = imageDest.GetRect();
	CRect	rectOperation;

	CPoint	pointOffestDest = rectDest.TopLeft();
	CPoint	pointOffestSrc = m_ptOffset;

	rectOperation.left = max( rectDest.left, min( m_ptOffset.x, rectDest.right ) );
	rectOperation.top = max( rectDest.top, min( m_ptOffset.y, rectDest.bottom ) );
	rectOperation.right = max( rectDest.left, min( m_ptOffset.x+m_cx, rectDest.right ) );
	rectOperation.bottom = max( rectDest.top, min( m_ptOffset.y+m_cy, rectDest.bottom ) );


	bool	bWasVirtual = false;

	if( imageSrc.IsVirtual() && bCopyParentData )
	{
		if( !imageSrc.CreateNew( m_cx, m_cy, imageSrc.GetColorConvertor(), imageSrc.GetColorsCount() ) )
			return false;
		imageSrc.SetOffset( pointOffestSrc );

		bWasVirtual = true;
	}


	if( bCopyParentData || bCopyLocalData || bClean )
	{
		color	colorsBacks[10];
		if( bClean )
		{
			_GetBackgroundColor(imageSrc, colorsBacks);
			TRACE("CImage::Exchange... 0x%p\n", this);
			/*COLORREF	cr = ::GetValueColor( GetAppUnknown(), "\\Editor", "Back", RGB( 0, 0, 0 ) );
			byte	dib[3];
			dib[0] = (byte)GetBValue(cr); 
			dib[1] = (byte)GetGValue(cr);
			dib[2] = (byte)GetRValue(cr);

			color	*ppcolor[10];
			for( int i = 0; i < 10; i++ )
				ppcolor[i] = &colorsBacks[i];

			IUnknown* punk;
			imageSrc->GetColorConvertor(&punk);
			sptrIColorConvertor5 sptrCCScr(punk);
			punk->Release();

			sptrCCScr->ConvertDIBToImageEx( dib, ppcolor, 1, true, imageSrc.GetColorsCount() );*/
		}

		for( c = 0; c < imageSrc.GetColorsCount(); c++ )
		{
	//current background
			color	colorBack = colorsBacks[c];

			for( j = rectOperation.top; j < rectOperation.bottom; j++ )
			{
				color	*pdest = imageDest.GetRow( j-pointOffestDest.y, c )-pointOffestDest.x;
				color	*psrc = imageSrc.GetRow( j-pointOffestSrc.y, c )-pointOffestSrc.x;
				byte	*pmask = imageSrc.GetRowMask( j-pointOffestSrc.y )-pointOffestSrc.x;

				for( i = rectOperation.left; i < rectOperation.right; i++ )
				{
					if( pmask[i] == 0xFF )
					{
	//preform operation here
						color	colorParent = pdest[i];
						color	colorLocal = psrc[i];

						if( bCopyParentData )
							psrc[i] = colorParent;
						if( bCopyLocalData )
							pdest[i] = colorLocal;
						if( bClean )
						{
							if( bWasVirtual )
								pdest[i] = colorBack;
							else
								psrc[i] = colorBack;
								
						}
					}
					else
					{
						//AAM - при копировании из парента захватим и пустые области
						//оттестить!!!!!!!!!!
						if( bCopyParentData )
							psrc[i] = pdest[i];
					}
				}
			}
		}
	}

	if( bAttachParentData )
		_CreateVirtual( imageSrc.GetRect() );


	//notify parent
	if( (flags & apfNotifyNamedData) != 0 )
	{
		INamedDataObject2Ptr	ptrParent( m_pParent );
		if( ptrParent != 0 )
		{
			IUnknown	*punkData = 0;
			ptrParent->GetData( &punkData );

			if( punkData != 0 )
			{
				::FireEvent( punkData, szNeedUpdateView, punkData, GetControllingUnknown() );
				punkData->Release();
			}
		}
	}
	return true;
}

//return the data object flags
DWORD CImage::GetNamedObjectFlags()
{
	if( !IsVirtual() )
		return nofHasData|CDataObjectBase::GetNamedObjectFlags();
	else
		return CDataObjectBase::GetNamedObjectFlags();
}


//initialize the contour array, walk the image
void CImage::_InitContours()
{
	_FreeContours();

	CWalkFillInfo	info( GetControllingUnknown() );

	info.InitContours();
	for( int i = 0; i < info.GetContoursCount(); i++ )
	{
		Contour	*pcontour = info.GetContour( i );
		ContourCompact(pcontour);
		_InsertContour( i, pcontour );
	}
	info.DetachContours();
}
//reset the contour array
void CImage::_FreeContours()
{
	for( int i = 0; i < m_arrContours.GetSize(); i++ )
		_FreeContour( i );

	m_arrContours.RemoveAll();

}
//insert contour to the specified location
void CImage::_InsertContour( long nPos, Contour *p )
{
	if( m_arrContours.GetSize() <= nPos )
		m_arrContours.SetSize( nPos+1 );
	else
		_FreeContour( nPos );

	m_arrContours[nPos] = p;
}
//free the specified contour
void CImage::_FreeContour( long nPos )
{
	Contour *p = (Contour*)m_arrContours[nPos];
	if( !p )
		return;
	if( p->ppoints )
		delete p->ppoints;

	delete p;
}

void CImage::Detach()
{
	//TRACE( "CImage::Detach()\n" );
	CDataObjectBase::Detach();
}


bool CImage::Copy(UINT nFormat, HANDLE* pHandle)
{
	if(!pHandle)
		return false;
	if(nFormat == CF_DIB)
	{
		CImageWrp image(GetControllingUnknown());
		CRect rc(image.GetRect());
		BITMAPINFOHEADER	*pbi = image.ExtractDIBBits(rc);

		if( !pbi )
			return false;

		HGLOBAL	hGlobal = ::GlobalAlloc( GMEM_MOVEABLE|GMEM_DDESHARE, pbi->biSizeImage+pbi->biSize );
		void	*pdata = ::GlobalLock( hGlobal );

		if( !pdata )
		{
			delete pbi;
			return false;
		}

		::memcpy( pdata, pbi, pbi->biSizeImage+pbi->biSize );

		delete pbi;

		::GlobalUnlock( hGlobal );

		*pHandle = hGlobal;
		return true;
	}

	return false;
}

bool CImage::Paste(UINT nFormat, HANDLE Handle)
{
	if(!Handle)
		return false;
	if(nFormat == CF_DIB)
	{
		CImageWrp image(GetControllingUnknown());

		BITMAPINFOHEADER* pbi = (BITMAPINFOHEADER*)::GlobalLock(Handle);

		bool bRet = image.AttachDIBBits(pbi);
	
		::GlobalUnlock(Handle);

		return bRet;
	}
	return false;
}

long CImage::GetWidth() 
{
	return m_cx;
}

long CImage::GetHeight() 
{
	return m_cy;
}

void CImage::CreateNew(long cx, long cy, LPCTSTR szColor) 
{
	VERIFY( strlen(szColor) );

	if( !_CreateNew( cx, cy, szColor, -1 ) )
		return;


	color	colorsBacks[10];
	COLORREF	cr = ::GetValueColor( GetAppUnknown(), "\\Editor", "Back", RGB( 0, 0, 0 ) );
	byte	dib[3];
	dib[0] = (byte)GetBValue(cr); 
	dib[1] = (byte)GetGValue(cr);
	dib[2] = (byte)GetRValue(cr);

	color	*ppcolor[10];
	for( int i = 0; i < 10; i++ )
		ppcolor[i] = &colorsBacks[i];

	sptrIColorConvertor5 sptrCCScr(m_sptrCC2);
	if( sptrCCScr == 0 )
		return;

	sptrCCScr->ConvertDIBToImageEx( dib, ppcolor, 1, true, m_iPanesCount );

	int	x, y, c;
	int	colors = m_iPanesCount;

	for( c = 0; c < colors; c++ )
	{
		for( y = 0; y < cy; y++ )
		{
			color	*p = _GetRow( y, c );
			for( x = 0; x < cx; x++, p++ )
				*p = colorsBacks[c];
		}
	}

	/*if( m_punkNamedData )
		::UpdateDataObject( m_punkNamedData, GetControllingUnknown() );*/
}

void CImage::CreateNewEx(long cx, long cy, LPCTSTR szColor, long lPaneNum) 
{
	if( !_CreateNew( cx, cy, szColor, lPaneNum ) )
		return;


	color	colorsBacks[10];
	COLORREF	cr = ::GetValueColor( GetAppUnknown(), "\\Editor", "Back", RGB( 0, 0, 0 ) );
	byte	dib[3];
	dib[0] = (byte)GetBValue(cr); 
	dib[1] = (byte)GetGValue(cr);
	dib[2] = (byte)GetRValue(cr);

	color	*ppcolor[10];
	for( int i = 0; i < 10; i++ )
		ppcolor[i] = &colorsBacks[i];

	sptrIColorConvertor5 sptrCCScr(m_sptrCC2);
	sptrCCScr->ConvertDIBToImageEx( dib, ppcolor, 1, true, m_iPanesCount );

	int	x, y, c;
	int	colors = m_iPanesCount;

	for( c = 0; c < colors; c++ )
	{
		for( y = 0; y < cy; y++ )
		{
			color	*p = _GetRow( y, c );
			for( x = 0; x < cx; x++, p++ )
				*p = colorsBacks[c];
		}
	}
}


long CImage::GetPosX() 
{
	return m_ptOffset.x;
}

long CImage::GetPosY() 
{
	return m_ptOffset.y;
}

void CImage::SetOffset(long cx, long cy, BOOL bMove) 
{
	_SetOffset( CPoint( cx, cy ), bMove ? true : false );
}

/////////////////////////////////////////////////////////////////////////////
//Object clone
HRESULT CImage::CreateCompatibleObject( IUnknown *punkObjSource, void *pData, unsigned uDataSize )
{
	IImage3Ptr imgSource( punkObjSource );
	if( imgSource == NULL )
		return E_FAIL;

	ICalibrPtr	ptrCalibrS( punkObjSource );
	if( ptrCalibrS!= 0  )ptrCalibrS->GetCalibration( &m_fCalibr, &m_guidC );


	CString strSourceCCName;
	BOOL	bSourceIsVirtual	= FALSE;
	BOOL	bSourceIsMasked		= TRUE;
	BOOL	bSourceIsContoured	= FALSE; 

	int		colors = 0;


	{
		IImage2Ptr ptrSource( punkObjSource );
		if( ptrSource != NULL )
		{
			DWORD dwFlags = 0;
			ptrSource->GetImageFlags( &dwFlags );

			bSourceIsVirtual	= ( dwFlags & ifVirtual );
			bSourceIsMasked		= ( dwFlags & ifMasked );			
			bSourceIsContoured	= ( dwFlags & ifContoured );			
		}

		IUnknown* punkCC = NULL;
														
		IImage2Ptr imgSource2( punkObjSource );
		if( imgSource2 != 0 )
			imgSource2->GetColorConvertor( &punkCC );			

		

		IColorConvertor2Ptr ptrCC( punkCC );

		if( punkCC )
			punkCC->Release();

		if( ptrCC == NULL )
			return E_FAIL;

		{				

			ptrCC->GetColorPanesDefCount( &colors );
			BSTR bstr = 0;
			ptrCC->GetCnvName( &bstr );					  
			strSourceCCName = bstr;
			::SysFreeString( bstr );			
		}
	}

	colors = ::GetImagePaneCount( punkObjSource );

	// [vanek] 12.12.2003: поумолчанию копируем все паны: 3540 и 3541
	DWORD dwCopyData		= ICD_COPY_MASK | ICD_COPY_CONTOURS | ICD_COPY_IMGDATA /*| ICD_COPY_PANE_SELECT*/; 
	CRect rect				= NORECT;
	const char *szCCName	= NULL;

	ImageCloneData* picd = (ImageCloneData*)pData;

	if( picd != NULL )
	{
		dwCopyData		= picd->dwCopyFlags;
		rect			= picd->rect;						  
		szCCName		= picd->szCCName;
	}	
	

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

	if( CheckInterface( imgSource, IID_INamedData ) )
	{
		INamedDataObject2Ptr	ptr( GetControllingUnknown() );
		ptr->InitAttachedData();

		CopyNamedData( ptr, imgSource );
	}


	IUnknown	*punkParent = NULL;

	{															 
		sptrINamedDataObject2	sptrN( imgSource );
		sptrN->GetParent( &punkParent );
	}
	{
		sptrINamedDataObject2	sptrN( GetControllingUnknown() );		
		sptrN->SetParent( punkParent, apfNotifyNamedData );
	}
	if( punkParent )
		punkParent->Release();

	
	if( bSourceIsVirtual )
	{
		_CreateVirtual( rect );
		_InitRowMasks();
		_InitContours();
	}
	else
	{
		CString	strCC;
		if(szCCName == 0)
			strCC = strSourceCCName;
		else
			strCC = szCCName;
		if( !_CreateNew( cx, cy, strCC, ::GetImagePaneCount( punkObjSource ) ) )
			return false;
		_SetOffset( ptOffset, true );
	}

	int	x, y, c;

	if( bSourceIsMasked )
	{
		_InitRowMasks();

		if( dwCopyData & ICD_COPY_MASK )
		{
		
			if( cx == cxOriginal && cy == cyOriginal )
			{
				for( int i = 0; i < cy; i++ )
				{
					BYTE *pSource = NULL;
					BYTE *pTarget = _GetRowMask( i );

					imgSource->GetRowMask( i, &pSource );

					memcpy( pTarget, pSource, cx );
				}					
			}
		}
		
	}

	if( bSourceIsContoured )
	{
		if( dwCopyData & ICD_COPY_CONTOURS )
			_InitContours( );
	}

	if( dwCopyData & ICD_COPY_IMGDATA )
	{
		CImagePaneCache cache( GetControllingUnknown(), (dwCopyData & ICD_COPY_PANE_SELECT) != 0 );		
		for( c = 0; c < colors; c++ )
		{
			if( dwCopyData & ICD_COPY_PANE_SELECT )
				if( !cache.IsPaneEnable( c ) )
					continue;

			for( y = 0; y < cy; y++ )
			{
				
				color	*psrc = 0, *pdest = 0;

				pdest = _GetRow( y, c );
				imgSource->GetRow( y, c, &psrc );			

				BYTE *pMask = _GetRowMask( y );
				for( x = 0; x < cx; x++, pdest++, psrc++ )
				{
					*pdest = *psrc;
				}
			}
			
		}
	}

	CopyObjectNamedData( punkObjSource, GetControllingUnknown(), "\\", true );
	CopyPropertyBag(punkObjSource, GetControllingUnknown(), true);

	return S_OK;
	
}
HRESULT CImage::Synchronize( )
{
	IImage3Ptr ptrImage( GetControllingUnknown() );
	
	if( ptrImage == NULL )
		return E_FAIL;

	DWORD dwFlags = 0;

	ptrImage->GetImageFlags( &dwFlags );

	BOOL	bIsVirtual		= FALSE;
	BOOL	bIsMasked		= FALSE;
	BOOL	bIsContoured	= FALSE; 

	bIsVirtual		= ( dwFlags & ifVirtual );
	bIsMasked		= ( dwFlags & ifMasked );			
	bIsContoured	= ( dwFlags & ifContoured );			
	
	if( bIsContoured )	
		_InitContours();

	if( bIsMasked )
		_InitRowMasks();	


	return S_OK;
}




//interface ICompatibleObject2 implementation
HRESULT CImage::XCompatibleObject::CreateCompatibleObject( IUnknown *punkObjSource, void *pData, unsigned uDataSize )
{
	_try_nested(CImage, CompatibleObject, CreateCompatibleObject)
	{
		return pThis->CreateCompatibleObject( punkObjSource, pData, uDataSize );
	}
	_catch_nested;
}


HRESULT CImage::XCompatibleObject::Synchronize( )
{
	_try_nested(CImage, CompatibleObject, Synchronize)
	{
		return pThis->Synchronize( );
	}
	_catch_nested;
}

HRESULT CImage::XCompatibleObject::IsEqual( IUnknown *punkObjSource  )
{
	_try_nested(CImage, CompatibleObject, IsEqual)
	{
		return pThis->IsEqualImage( punkObjSource );
	}
	_catch_nested;
}


BSTR CImage::GetColorSystem() 
{
	if( m_sptrCC2 != 0 )
	{
		BSTR	bstr;

		m_sptrCC2->GetCnvName( &bstr );
		return bstr;
	}

	CString strResult;
	return strResult.AllocSysString();
}

HRESULT CImage::XRect::SetRect(RECT rc)
{
	METHOD_PROLOGUE_EX(CImage, Rect);
	return E_NOTIMPL;
}

HRESULT CImage::XRect::GetRect(RECT* prc)
{
	METHOD_PROLOGUE_EX(CImage, Rect);
	prc->left = pThis->m_ptOffset.x;
	prc->top = pThis->m_ptOffset.y;
	prc->right = prc->left+pThis->m_cx;
	prc->bottom = prc->top+pThis->m_cy;
	return S_OK;
}

HRESULT CImage::XRect::HitTest( POINT	point, long *plHitTest )
{
	METHOD_PROLOGUE_EX(CImage, Rect);
	*plHitTest = 0;
	if( point.x >= pThis->m_ptOffset.x && point.y >= pThis->m_ptOffset.y &&
		point.x < pThis->m_ptOffset.x + pThis->m_cx && point.y < pThis->m_ptOffset.y + pThis->m_cy )
	{
		byte	*p = pThis->_GetRowMask( point.y - pThis->m_ptOffset.y );
		if( p )*plHitTest = *(p+point.x-pThis->m_ptOffset.x) >= 128;
	}

	return S_OK;
}

HRESULT CImage::XRect::Move( POINT point )
{
	METHOD_PROLOGUE_EX(CImage, Rect)
	pThis->_SetOffset( point, false );
	return S_OK;
}

long CImage::GetColorsCount() 
{
	return m_iPanesCount;
}

void CImage::GetCalibration(VARIANT FAR* varCalibr, VARIANT FAR* varGUID) 
{
	InitCalibr();
	_variant_t	var = m_fCalibr, varG;

	BSTR	bstr;
	::StringFromCLSID( m_guidC, &bstr );
	varG = bstr;
	::CoTaskMemFree( bstr );

	*varCalibr = var.Detach();
	*varGUID = varG.Detach();
}

void CImage::SetCalibration( double fPixelPerMeter, LPCTSTR bstr_guid )
{
	_bstr_t bstr = bstr_guid;
	CLSIDFromString( bstr, &m_guidC );
	m_fCalibr = fPixelPerMeter;
}

bool CImage::IsObjectEmpty()
{
	return !( m_cx > 0 && m_cy > 0 );
}

HRESULT CImage::_AddPane(int nPane )
{
	if( IsObjectEmpty() )
		return S_FALSE;

	if( nPane >= m_iPanesCount )
		return S_FALSE;

	int cx = m_cx;
	int cy = m_cy;

	color *tmp_color = m_pcolors;
	color **tmp_pprow = m_ppRows;
	m_iPanesCount++;

	m_pcolors = new color[cx*cy*m_iPanesCount];

	if( !m_pcolors )
		return S_FALSE;

	m_ppRows = new color*[ cy * m_iPanesCount ];

	if( !m_ppRows )
	{
		delete []m_pcolors;
		return S_FALSE;
	}

	for( int i = 0; i < cy*m_iPanesCount; i++ )
		m_ppRows[i] = m_pcolors + cx * i;


	if( nPane == -1 )
	{
		for( int y = 0; y < cy ;y++  )
		{
			for( int p = 0; p < m_iPanesCount ;p++  )
			{
				if( p + 1 != m_iPanesCount )
					CopyMemory( m_ppRows[y * m_iPanesCount + p], tmp_pprow[y * ( m_iPanesCount - 1) + p], sizeof( color ) * cx );
				else
					ZeroMemory( m_ppRows[y * m_iPanesCount + p], sizeof( color ) * cx );
			}
		}
	}
	else
	{
		for( int y = 0; y < cy ;y++  )
		{
			for( int p = 0; p < m_iPanesCount ;p++  )
			{
				if( p != nPane )
				{
					if( p < nPane)
						CopyMemory( m_ppRows[y * m_iPanesCount + p], tmp_pprow[y * ( m_iPanesCount - 1) + p], sizeof( color ) * cx );
					else
						CopyMemory( m_ppRows[y * m_iPanesCount + p], tmp_pprow[y * ( m_iPanesCount - 1) + p - 1], sizeof( color ) * cx );
				}
				else
					ZeroMemory( m_ppRows[y * m_iPanesCount + p], sizeof( color ) * cx );
			}
		}
	}

	delete []tmp_pprow;
	delete []tmp_color;

	return S_OK;
}
HRESULT CImage::_RemovePane( int nPane )
{
	if( IsObjectEmpty() )
		return S_FALSE;

	if( nPane >= m_iPanesCount || nPane < 0 )
		return S_FALSE;

	int cx = m_cx;
	int cy = m_cy;

	color *tmp_color = m_pcolors;
	color **tmp_pprow = m_ppRows;

	m_iPanesCount--;
	m_pcolors = new color[cx*cy*m_iPanesCount];

	if( !m_pcolors )
		return S_FALSE;

	m_ppRows = new color*[ cy * m_iPanesCount ];

	if( !m_ppRows )
	{
		delete []m_pcolors;
		return S_FALSE;
	}

	for( int i = 0; i < cy*m_iPanesCount; i++ )
		m_ppRows[i] = m_pcolors + cx * i;

	for( int y = 0; y < cy ;y++  )
	{
		for( int p = 0; p < m_iPanesCount+1 ;p++  )
		{
			if( p != nPane )
			{
				if( p < nPane)
					CopyMemory( m_ppRows[y * m_iPanesCount + p], tmp_pprow[y * ( m_iPanesCount + 1) + p], sizeof( color ) * cx );
				else
					CopyMemory( m_ppRows[y * m_iPanesCount + p - 1], tmp_pprow[y * ( m_iPanesCount + 1) + p], sizeof( color ) * cx );
			}
		}
	}

	delete []tmp_pprow;
	delete []tmp_color;

	return S_OK;
}
HRESULT CImage::_GetPanesCount(int *nSz )
{
	*nSz = m_iPanesCount;
	return S_OK;
}

struct _CALCMINMAXSUMINFO
{
	color m_clrMin,m_clrMax;
	int m_nPoints;
	double m_dSum;
	_CALCMINMAXSUMINFO()
	{
		m_nPoints = 0;
		m_clrMin = m_clrMax = 0;
		m_dSum = 0.;
	}
	void AddPoint(color clrBri)
	{
		if (m_nPoints++ == 0)
			m_clrMin = m_clrMax = clrBri;
		else
		{
			if (clrBri < m_clrMin) m_clrMin = clrBri;
			if (clrBri > m_clrMax) m_clrMax = clrBri;
		}
		m_dSum += clrBri;
	}
};

void CImage::CalcMinMaxBrightness()
{
	if (m_sptrCC2 == 0) return;
	BSTR bstrName;
	m_sptrCC2->GetCnvName(&bstrName);
	CString sName(bstrName);
	::SysFreeString(bstrName);
	_CALCMINMAXSUMINFO MinMaxSumInfo;
	if (sName.CompareNoCase(_T("Gray")))
	{
		for (int y = 0; y < m_cy; y++)
		{
			color *pRow = _GetRow(y, 0);
			byte *pRowMask = _GetRowMask(y);
			for (int x = 0; x < m_cx; x++)
				if (pRowMask[x] != 0)
					MinMaxSumInfo.AddPoint(pRow[x]);
		}
	}
	else if (sName.CompareNoCase(_T("RGB")))
	{
		for (int y = 0; y < m_cy; y++)
		{
			color *pRowR = _GetRow(y, 0);
			color *pRowG = _GetRow(y, 1);
			color *pRowB = _GetRow(y, 2);
			byte *pRowMask = _GetRowMask(y);
			for (int x = 0; x < m_cx; x++)
				if (pRowMask[x] != 0)
					MinMaxSumInfo.AddPoint(Bright(pRowB[x],pRowG[x],pRowR[x]));
		}
	}
	m_byMinBri = (byte)(MinMaxSumInfo.m_clrMin>>8);
	m_byMaxBri = (byte)(MinMaxSumInfo.m_clrMax>>8);
	m_byAvrBri = (byte)(((color)(MinMaxSumInfo.m_dSum/MinMaxSumInfo.m_nPoints))>>8);
	m_bMinMaxBriInit = true;
}

long CImage::GetMinBrightness()
{
	if (!m_bMinMaxBriInit) CalcMinMaxBrightness();
	return m_byMinBri;
}

long CImage::GetMaxBrightness()
{
	if (!m_bMinMaxBriInit) CalcMinMaxBrightness();
	return m_byMaxBri;
}

long CImage::GetAverageBrightness()
{
	if (!m_bMinMaxBriInit) CalcMinMaxBrightness();
	return m_byAvrBri;
}

HRESULT CImage::IsEqualImage( IUnknown *punkObjSource )
{
	IImage3Ptr imgSource = punkObjSource;
	IImage3Ptr imgDest   = punkObjSource;
	
	if( imgSource == 0 )
		return E_FAIL;

	if( imgDest == 0 )
		return E_FAIL;

	ICalibrPtr	ptrCalibrS = punkObjSource;
	
	double fCalibr = 0;
	GuidKey guidC;

	if( ptrCalibrS != 0  )
		ptrCalibrS->GetCalibration( &fCalibr, &guidC );

	if( m_fCalibr != fCalibr )
		return E_FAIL;

	if( m_guidC != guidC )
		return E_FAIL;


	int		colors = 0;

	{
		DWORD dwFlags = 0;
		imgSource->GetImageFlags( &dwFlags );

		DWORD dwFlags2 = 0;
		imgDest->GetImageFlags( &dwFlags2 );

		if( dwFlags != dwFlags2 )
			return E_FAIL;

		IUnknown* punkCC = 0;
		imgSource->GetColorConvertor( &punkCC );			

		IUnknown* punkCC2 = 0;
		imgDest->GetColorConvertor( &punkCC2 );			

		IColorConvertor2Ptr ptrCC = punkCC;
		IColorConvertor2Ptr ptrCC2 = punkCC2;

		if( punkCC )
			punkCC->Release();

		if( punkCC2 )
			punkCC2->Release();

		if( ptrCC == 0 )
			return E_FAIL;

		if( ptrCC2 == 0 )
			return E_FAIL;

		_bstr_t bstrCCName, bstrCCName2;

		ptrCC->GetCnvName( bstrCCName.GetAddress() );
		ptrCC2->GetCnvName( bstrCCName2.GetAddress() );

		if( bstrCCName != bstrCCName2 )
			return E_FAIL;

	}
	colors = ::GetImagePaneCount( punkObjSource );
	
	if( colors != ::GetImagePaneCount( GetControllingUnknown() ) )
		return E_FAIL;

	int	cx = 0;
	int	cy = 0;
	CPoint	ptOffset;

	int	cx2 = 0;
	int	cy2 = 0;
	CPoint	ptOffset2;

	imgSource->GetSize( &cx, &cy );
	imgSource->GetOffset( &ptOffset );

	imgDest->GetSize( &cx2, &cy2 );
	imgDest->GetOffset( &ptOffset2 );

	if( cx != cx2 || cy != cy2 )
		return E_FAIL;

	if( ptOffset != ptOffset2 )
		return E_FAIL;

	if( CheckInterface( imgSource, IID_INamedData ) != CheckInterface( imgDest, IID_INamedData ) )
		return E_FAIL;


	if( m_ppRowMasks != 0 )
	{
		for( int y = 0; y < cy; y++ )
		{
			BYTE *pSource = 0;
			BYTE *pTarget = _GetRowMask( y );

			imgSource->GetRowMask( y, &pSource );

			if( memcmp( pTarget, pSource, sizeof( BYTE ) * cx ) != 0 )
				return E_FAIL;
		}					
	}

	int nCnt = 0;
	imgSource->GetContoursCount( &nCnt );

	if( m_arrContours.GetSize() != nCnt )
		return E_FAIL;

	for( int i = 0; i < nCnt; i++ )
	{
		Contour *pDest = 0;
		Contour *pScr = 0;

		imgSource->GetContour( i, &pScr );
		imgDest->GetContour( i, &pDest );

		if( pDest == 0 || pScr == 0 )
			return E_FAIL;

		if( pScr->nContourSize != pDest->nContourSize )
			return E_FAIL;

		if( pScr->nAllocatedSize != pDest->nAllocatedSize )
			return E_FAIL;

		if( pScr->lFlags != pDest->lFlags )
			return E_FAIL;

		if( pScr->lReserved != pDest->lReserved )
			return E_FAIL;

		if( memcmp( pScr->ppoints, pDest->ppoints, sizeof( ContourPoint ) * pScr->nContourSize ) != 0 )
			return E_FAIL;
	}


	for( int c = 0; c < colors; c++ )
	{
		for( int y = 0; y < cy; y++ )
		{
			color	*psrc = 0, *pdest = 0;

			pdest = _GetRow( y, c );
			imgSource->GetRow( y, c, &psrc );			

			if( memcmp( psrc, pdest, sizeof( color ) * cx ) != 0 )
				return E_FAIL;
		}
		
	}

	return S_OK;
}

bool CImage::SetBagProperty( LPCTSTR strName, const VARIANT FAR&var )
{
	INamedPropBagPtr sptrB = GetControllingUnknown();
	
	if( sptrB == 0 )
		return false;

	if( S_OK != sptrB->SetProperty( _bstr_t( strName ), var ) )
		return false;

	return true;
}

VARIANT CImage::GetBagProperty( LPCTSTR strName )
{
	INamedPropBagPtr sptrB = GetControllingUnknown();
	
	_variant_t ret_var;

	if( sptrB == 0 )
		return ret_var;

	sptrB->GetProperty( _bstr_t( strName ), &ret_var );
	return ret_var;
}