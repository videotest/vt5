// ThumbnailManager.cpp : implementation file
//

#include "stdafx.h"
#include "ThumbMan.h"
#include "ThumbnailManager.h"

#include "constant.h"
#include <math.h>
#include "ThumbnailDlg.h"
#include "wmf.h"
#include "PropBag.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class CActionThumbnailGenerate : public CActionBase
{
	DECLARE_DYNCREATE(CActionThumbnailGenerate)
	GUARD_DECLARE_OLECREATE(CActionThumbnailGenerate)

public:
	CActionThumbnailGenerate(){}
	virtual ~CActionThumbnailGenerate(){}

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible(){ return true;};
	virtual bool IsChecked(){ return false;};

};


IMPLEMENT_DYNCREATE(CActionThumbnailGenerate, CCmdTargetEx);

// {AC8EACE3-44F4-46a5-883F-82167F192CFC}
GUARD_IMPLEMENT_OLECREATE(CActionThumbnailGenerate, "ThumbnailManager.GenerateThumbnail",
0xac8eace3, 0x44f4, 0x46a5, 0x88, 0x3f, 0x82, 0x16, 0x7f, 0x19, 0x2c, 0xfc);

// {E185716D-7F2D-4213-8741-62E293C2728F}
static const GUID guidThumbnailGenerate = 
{ 0xe185716d, 0x7f2d, 0x4213, { 0x87, 0x41, 0x62, 0xe2, 0x93, 0xc2, 0x72, 0x8f } };



ACTION_INFO_FULL(CActionThumbnailGenerate, IDS_SHOW_THUMBNAIL, -1, -1, guidThumbnailGenerate);
ACTION_TARGET(CActionThumbnailGenerate, "anydoc");

CRect g_rcTarget = CRect(100,100,200,200);
CSize g_sizeImage = CSize(100,100);
CString g_strObject;

bool CActionThumbnailGenerate::Invoke()
{
	CThumbnailDlg dlg;

	dlg.m_strObjectName = g_strObject;

	dlg.m_nGenerateWidth = g_sizeImage.cx;
	dlg.m_nGenerateHeight = g_sizeImage.cy;

	dlg.m_nX		= g_rcTarget.left;
	dlg.m_nY		= g_rcTarget.top;
	dlg.m_nWidth	= g_rcTarget.Width();
	dlg.m_nHeight	= g_rcTarget.Height();


	
	dlg.SetNamedData( m_punkTarget );
	if( dlg.DoModal() != IDOK) 
		return false;


	g_strObject		= dlg.m_strObjectName;

	g_sizeImage.cx = dlg.m_nGenerateWidth;
	g_sizeImage.cy = dlg.m_nGenerateHeight;

	g_rcTarget.left = dlg.m_nX;
	g_rcTarget.top  = dlg.m_nY;
	g_rcTarget.right  = dlg.m_nX + dlg.m_nWidth;
	g_rcTarget.bottom = dlg.m_nY + dlg.m_nHeight;


	IUnknown* punkObject = ::GetObjectByName( m_punkTarget, dlg.m_strObjectName, 0 );
	if( punkObject == NULL )
		return false;

	IUnknownPtr ptrObject = punkObject;
	punkObject->Release();

	if( ptrObject == NULL )
		return false;


	sptrIThumbnailManager spThumbMan( GetAppUnknown() );
	if( spThumbMan == NULL )
		return false;


	IUnknown* punkRenderObject = NULL;

	spThumbMan->GetRenderObject( ptrObject, &punkRenderObject );
	if( punkRenderObject == NULL )
		return false;


	sptrIRenderObject spRenderObject( punkRenderObject );
	punkRenderObject->Release();

	if( spRenderObject == NULL ) 
		return false;


	BYTE* pbi = NULL;

	short nSupported;
	spRenderObject->GenerateThumbnail( ptrObject, 24, dlg.m_nFlags, dlg.m_lParam, 
				CSize(dlg.m_nGenerateWidth,dlg.m_nGenerateHeight), 
				&nSupported, &pbi, 0 );

	if( pbi )
	{
		CClientDC dc(0);
		::ThumbnailDraw( pbi, &dc, 
				CRect(	dlg.m_nX, dlg.m_nY,
						dlg.m_nX + dlg.m_nWidth, dlg.m_nY + dlg.m_nHeight
						), 0
					);
		::ThumbnailFree( pbi );

		INumeredObjectPtr sptrNO( ptrObject );
		if( sptrNO )
		{			
			GUID guid;
			::ZeroMemory( &guid, sizeof( guid ) );
			sptrNO->GetKey( &guid );

			typedef struct _GUID {          // size is 16
				DWORD Data1;
				WORD   Data2;
				WORD   Data3;
				BYTE  Data4[8];
			} GUID;

			CString strKey;
			strKey.Format( "%x-%x-%x-%x%x%x%x%x%x%x%x", guid.Data1, guid.Data2, guid.Data3, 
				guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
				guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7] );
			dc.TextOut( dlg.m_nX, dlg.m_nY, strKey );			
				
		}

		
		pbi = NULL;
	}





	
	return true;
}

//


/////////////////////////////////////////////////////////////////////////////
// CThumbnailManager

IMPLEMENT_DYNCREATE(CThumbnailManager, CCmdTargetEx);

// {F426CE0E-25E1-46c2-897E-7BA87219029B}
GUARD_IMPLEMENT_OLECREATE(CThumbnailManager, szThumbnailManagerProgID,
0xf426ce0e, 0x25e1, 0x46c2, 0x89, 0x7e, 0x7b, 0xa8, 0x72, 0x19, 0x2, 0x9b);


/////////////////////////////////////////////////////////////////////////////
BEGIN_INTERFACE_MAP(CThumbnailManager, CCmdTargetEx)
	INTERFACE_PART(CThumbnailManager, IID_IThumbnailManager, ThumbnailManager)	
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CThumbnailManager, ThumbnailManager);


/////////////////////////////////////////////////////////////////////////////
CThumbnailManager::CThumbnailManager()
{
	_OleLockApp( this );
	EnableAggregation();

	m_thumbnailManager.AttachComponentGroup( szPluginRender );
	m_thumbnailManager.Init();


	//Register Default Render
	InitDefaultRender();

}

bool CThumbnailManager::InitDefaultRender()
{
	CString	s = szDefaultRenderProgID;

	BSTR	bstr = s.AllocSysString();
	CLSID	clsid;

	if (::CLSIDFromProgID(bstr, &clsid) != S_OK)
		return false;

	IUnknown	*punk = 0;

	::SysFreeString(bstr);


	HRESULT hresult = ::CoCreateInstance(clsid, NULL, 
		CLSCTX_INPROC_SERVER, IID_IUnknown, (void **)&punk);

	if( hresult != S_OK )
		return false;
	

	m_thumbnailManager.AddComponent(punk);
	punk->Release();

	return true;

}


/////////////////////////////////////////////////////////////////////////////
CThumbnailManager::~CThumbnailManager()
{	
	
	_OleUnlockApp( this );
}

	
/////////////////////////////////////////////////////////////////////////////
HRESULT CThumbnailManager::XThumbnailManager::GetRenderObject( 
									IUnknown* punkDataObject, IUnknown** ppunkRenderObject )
{
	_try_nested(CThumbnailManager, ThumbnailManager, GetRenderObject)
	{
		*ppunkRenderObject = NULL;

		for( int i=0;i<pThis->m_thumbnailManager.GetComponentCount();i++ )
		{
			IUnknown* punk = pThis->m_thumbnailManager.GetComponent( i, true );
			if( punk == NULL )
				continue;

			sptrIRenderObject sptrRender( punk );
			punk->Release( );

			if( sptrRender == NULL )
				continue;

			short nRenderSupport = -1;

			sptrRender->Support( punkDataObject, &nRenderSupport );



			if( (RenderSupport)nRenderSupport == rsFull )
			{
				sptrRender->AddRef();
				*ppunkRenderObject = sptrRender;
				return S_OK;
			}
		}

		return S_OK;
	}
	_catch_nested;
}


/////////////////////////////////////////////////////////////////////////////
HRESULT CThumbnailManager::XThumbnailManager::ThumbnailDraw( BYTE* pbi, HDC hdc, RECT rcTarget, IUnknown* punkDataObject )
{
	_try_nested(CThumbnailManager, ThumbnailManager, ThumbnailDraw)
	{
		bool bRes = ::ThumbnailDraw( pbi, CDC::FromHandle( hdc ), rcTarget, punkDataObject );
		return bRes ? S_OK : E_FAIL;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CThumbnailManager::XThumbnailManager::ThumbnailGetSize( BYTE* pbi, DWORD* pdwSize )
{
	_try_nested(CThumbnailManager, ThumbnailManager, ThumbnailGetSize)
	{
		*pdwSize = ::ThumbnailGetSize( pbi );
		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CThumbnailManager::XThumbnailManager::ThumbnailAllocate( BYTE** ppbi, SIZE sizeThumbnail )
{
	_try_nested(CThumbnailManager, ThumbnailManager, ThumbnailAllocate)
	{
		bool bRes = ::ThumbnailAllocate( ppbi, sizeThumbnail );
		return bRes ? S_OK : E_FAIL;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CThumbnailManager::XThumbnailManager::ThumbnailFree( BYTE* pbi )
{
	_try_nested(CThumbnailManager, ThumbnailManager, ThumbnailFree)
	{
		bool bRes = ::ThumbnailFree( pbi );
		return bRes ? S_OK : E_FAIL;
	}
	_catch_nested;
}


/////////////////////////////////////////////////////////////////////////////
// CImageRender

IMPLEMENT_DYNCREATE(CImageRender, CCmdTargetEx);

// {038BF230-7A14-4a98-AB13-37D629BA66BE}
GUARD_IMPLEMENT_OLECREATE(CImageRender, szImageRenderProgID,
0x38bf230, 0x7a14, 0x4a98, 0xab, 0x13, 0x37, 0xd6, 0x29, 0xba, 0x66, 0xbe);


/////////////////////////////////////////////////////////////////////////////
BEGIN_INTERFACE_MAP(CImageRender, CCmdTargetEx)
	INTERFACE_PART(CImageRender, IID_IRenderObject, RenderObject)	
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CImageRender, RenderObject);


/////////////////////////////////////////////////////////////////////////////
CImageRender::CImageRender()
{
	_OleLockApp( this );

	EnableAggregation();

}

/////////////////////////////////////////////////////////////////////////////
CImageRender::~CImageRender()
{
	
	_OleUnlockApp( this );
}



/////////////////////////////////////////////////////////////////////////////
HRESULT CImageRender::XRenderObject::Support( IUnknown* punkDataObject, short* pnRenderSupport )
{
	_try_nested(CImageRender, RenderObject, Support)
	{
		*pnRenderSupport = rsNone;
		

		CImageWrp image = punkDataObject;
		if( image == NULL )
			return S_FALSE;

		*pnRenderSupport = (short)rsFull;
		return S_OK;
	}
	_catch_nested;

}


/////////////////////////////////////////////////////////////////////////////
CSize GetRenderSize( CSize sizeThumb, CSize sizeImage )
{
	double fThumbWidth		= (double)sizeThumb.cx;
	double fThumbHeight	= (double)sizeThumb.cy;
	double fThumbZoom		= fThumbWidth / fThumbHeight;

	double fImageWidth	= (double)sizeImage.cx;
	double fImageHeight	= (double)sizeImage.cy;	
	double fImageZoom	= fImageWidth / fImageHeight;


	double fZoom = min( fThumbWidth / fImageWidth, fThumbHeight / fImageHeight );

	double fWidth, fHeight;

	fWidth = fThumbWidth;
	fHeight = fThumbHeight;
	
	if( fWidth / fImageZoom > fThumbHeight )
	{		
		fWidth = fThumbHeight * fImageZoom;
		fHeight = fThumbHeight;
	}

	if( fHeight * fImageZoom > fThumbWidth )
	{
		fHeight = fThumbWidth / fImageZoom;
		fWidth = fThumbWidth;
	}

	
	int nWidth, nHeight;

	double fConvZoom = fHeight / fImageHeight;
	int nConvZoom = int( fConvZoom*256+.5 );

	double fNewConvZoom = (double)nConvZoom / 256.0;

	int nNewZ = int( fNewConvZoom*256+.5 );

	ASSERT( nNewZ == nConvZoom );

	nWidth  = int( fImageWidth  * fNewConvZoom + .5);
	nHeight = int( fImageHeight * fNewConvZoom + .5);	

	return CSize( nWidth, nHeight );
}


#define THUMBNAIL_SECTION "\\Thumbnail"
#define THUMBNAIL_ADVANCED_MODE "AdvancedMode"

#define THUMBNAIL_MIN_SIZE_4_Icon "ImageRenderMinSize4Icon"


#define THUMBNAIL_USE_FILE "ImageRenderUseFile"
#define THUMBNAIL_FILENAME "ImageRenderFileName"

	
/////////////////////////////////////////////////////////////////////////////
HRESULT CImageRender::XRenderObject::GenerateThumbnail( 
					/*[in]*/	IUnknown* punkDataObject,
					/*[in]*/	int	nBitsCount,
					/*[in]*/	DWORD dwFlag,
					/*[in]*/	LPARAM lParam,
					/*[in]*/	SIZE sizeThumbnail, 
					/*[out]*/	short* pnRenderSupport,
					/*[out]*/	BYTE** ppbi,
					/*[in]*/	IUnknown* punkBag
					)

{
	_try_nested(CImageRender, RenderObject, GenerateThumbnail)
	{

		CSize sizeImage;
		CSize sizeRender;
		
		
		*pnRenderSupport = (short)rsNone;

		CImageWrp image = punkDataObject;
		if( image == NULL )
			return S_FALSE;

		*pnRenderSupport = (short)rsFull;


		sizeImage.cx = image.GetWidth();
		sizeImage.cy = image.GetHeight();

		int nMinSize = ::GetValueInt( ::GetAppUnknown(), THUMBNAIL_SECTION, THUMBNAIL_MIN_SIZE_4_Icon, 32 );

		if( image.IsEmpty() )
		{
			if( sizeThumbnail.cx <= nMinSize || sizeThumbnail.cy <= nMinSize )
				return pThis->DrawICO( punkDataObject, dwFlag, lParam,	sizeThumbnail, pnRenderSupport, ppbi );
			else
				return pThis->DrawWMF( dwFlag, lParam,	sizeThumbnail, pnRenderSupport, ppbi );
		}

		sizeRender = ::GetRenderSize/*ThumbnailGetRatioSize*/( sizeThumbnail, sizeImage );		

		IUnknown * punkColorCnv = 0;
		image->GetColorConvertor(&punkColorCnv);
		if( punkColorCnv == NULL )
		{
			//ASSERT( false );
			return E_FAIL;
		}

		sptrIColorConvertorEx spCCEx( punkColorCnv );
		punkColorCnv->Release();

		if( spCCEx == NULL )
			return S_FALSE;

		*pnRenderSupport = (short)rsFull;
				
		BITMAPINFOHEADER bmih;
		ZeroMemory(&bmih, sizeof(bmih));

		bmih.biBitCount = 24;//nBitsCount;
		bmih.biWidth = sizeRender.cx;
		bmih.biHeight = sizeRender.cy;		
		bmih.biSize = sizeof(bmih);
		bmih.biPlanes = 1;


		DWORD dwSize = ::ThumbnailGetSize( (BYTE*)&bmih );

		if( !::ThumbnailAllocate( ppbi, CSize( bmih.biWidth, bmih.biHeight ) ) )
			return S_FALSE;

		//[AY]
		CRect rectPaint = CRect( 0, 0, sizeRender.cx, sizeRender.cy );
		
		//Copy BITMAPINFOHEADER
		memcpy( *ppbi, &bmih, sizeof(BITMAPINFOHEADER) );
		
		//Copy TumbnailInfo
		TumbnailInfo tumbnailInfo;
		::ZeroMemory( &tumbnailInfo, sizeof( tumbnailInfo ) );
		tumbnailInfo.dwIcon = FALSE;
		tumbnailInfo.dwFlags = tfVarStretch;

		memcpy( (*ppbi+((BYTE)sizeof(BITMAPINFOHEADER))), &tumbnailInfo, sizeof(tumbnailInfo) );

		BYTE* pDibBits = (*ppbi) + ((BYTE)sizeof(BITMAPINFOHEADER)) + 
							((BYTE)sizeof(TumbnailInfo));
		//Copy DibBits
		//rectPaint.right	+= 1;
		//rectPaint.bottom	+= 1;

		double fZoom = (double)sizeRender.cx/sizeImage.cx;
		DWORD dwCnvFlags = cidrFillBackByColor;
		INamedPropBagPtr bag(punkBag);
		if(bag!=0)
		{
			_variant_t	var;
			bag->GetProperty(_bstr_t("Object.ShowBackground"), &var);
			if(var.vt != VT_EMPTY)
			{
				if(var.lVal)
				{
					dwCnvFlags &= ~cidrFillBackByColor;
					dwCnvFlags |= cidrIgnoreMask;
				}
			}
		}
		spCCEx->ConvertImageToDIBRect( &bmih, pDibBits, rectPaint.TopLeft(),
			image, rectPaint, 
			CPoint(0,0),
			fZoom, 
			CPoint(0, 0), lParam, dwCnvFlags, punkBag );

		
		/*
		if( CheckInterface( punkDataObject, IID_IAviImage ) )
		{

				AFX_MANAGE_STATE(AfxGetStaticModuleState());
				HICON hAviIco = AfxGetApp()->LoadIcon( IDI_AVI_ADD );
				if( hAviIco )
				{
					CDC	dcMemory;
					{
						CClientDC dcScreen( 0 );						
						dcMemory.CreateCompatibleDC( &dcScreen );			
					}

					HBITMAP	hDIBSection = ::CreateDIBSection( 
										dcMemory.m_hDC, 
										(BITMAPINFO*)&bmih, DIB_RGB_COLORS, (void**)&pDibBits, 0, 0 );

					CBitmap *psrcBitmap = CBitmap::FromHandle( hDIBSection  );					

					//propare the memory DC to drawing
					CBitmap *poldBmp = dcMemory.SelectObject( psrcBitmap );

					::DrawIcon( dcMemory.m_hDC, 0, 0, hAviIco );

					dcMemory.SelectObject( poldBmp );

					::DeleteObject(hDIBSection);	hDIBSection = 0;
					
					::DestroyIcon( hAviIco );
				}			
		}
		*/

		return S_OK;
	}
	_catch_nested;
}



HRESULT CImageRender::DrawWMF(
			/*[in]*/	DWORD dwFlag,
			/*[in]*/	LPARAM lParam,
			/*[in]*/	SIZE sizeThumbnail, 
			/*[out]*/	short* pnRenderSupport,
			/*[out]*/	BYTE** ppbi )
{

	CString strResource = "WMF_NO_IMAGE";
	long lAdvMode = ::GetValueInt( ::GetAppUnknown(), THUMBNAIL_SECTION, THUMBNAIL_ADVANCED_MODE, 0 );
	if( lAdvMode == 1 )
		strResource = "WMF_NO_IMAGE_ADV";

	long lUseFile = ::GetValueInt( ::GetAppUnknown(), THUMBNAIL_SECTION, THUMBNAIL_USE_FILE, 0 );
	CString strFileName = ::GetValueString( ::GetAppUnknown(), THUMBNAIL_SECTION, THUMBNAIL_FILENAME, "" );


	if( lUseFile )
	{
		CFileFind ff;
		if( !ff.FindFile( strFileName ) )
			lUseFile = 0;

	}

	
	int nMetaWidth, nMetaHeight;
	nMetaWidth = nMetaHeight = 0;	

	HENHMETAFILE hmf = NULL;
	if( !lUseFile )
	{
	
		AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
		CWinApp* pApp = AfxGetApp();
		if( pApp )
		{
			HRSRC hrSrc = ::FindResource( pApp->m_hInstance, (LPCSTR)strResource, "WMFDATA" );
			if( hrSrc )
			{
				HGLOBAL handle = ::LoadResource( pApp->m_hInstance, hrSrc );
				if( handle )
				{
					DWORD dwDataSize = ::SizeofResource( pApp->m_hInstance, hrSrc );
					if( dwDataSize > 0 )
					{
						BYTE* pMetaData = (BYTE*)GlobalLock( handle );	
						if( pMetaData )
						{

							CWmf wmf( pMetaData, dwDataSize );		
							hmf = wmf.CreateMetaFile( nMetaWidth, nMetaHeight );
							GlobalUnlock( handle );	
							FreeResource( handle );
						}
					}
				}
			}
		}
	}
	else//Use file
	{
		CFile ff;
		if( ff.Open( strFileName, CFile::modeRead|CFile::typeBinary ) )
		{
			UINT uiDataSize = (UINT)ff.GetLength();
			if( uiDataSize > 0 )
			{
				BYTE* pBuf = new BYTE[uiDataSize];
				if( pBuf )
				{
					ff.Read( pBuf, uiDataSize );
					CWmf wmf( pBuf, uiDataSize );		
					hmf = wmf.CreateMetaFile( nMetaWidth, nMetaHeight );
					delete pBuf;
				}
			}
		}
	}


	int nMin = min( sizeThumbnail.cx, sizeThumbnail.cy );

	CSize sizeImage( nMin, nMin );
	CSize sizeRender( nMin, nMin );

	if( nMetaWidth > 0 && nMetaHeight > 0 )
		sizeImage = CSize( nMetaWidth, nMetaHeight );

	sizeRender = ::GetRenderSize( sizeThumbnail, sizeImage );
	
	
	BITMAPINFOHEADER bmih;
	::ZeroMemory( &bmih, sizeof( bmih )  );
	bmih.biBitCount = 24;
	bmih.biHeight = sizeRender.cy;
	bmih.biWidth = sizeRender.cx;
	bmih.biSize = sizeof( bmih );
	bmih.biPlanes = 1;


	DWORD dwSize = ::ThumbnailGetSize( (BYTE*)&bmih );

	if( !::ThumbnailAllocate( ppbi, CSize( bmih.biWidth, bmih.biHeight ) ) )
		return S_FALSE;
	

	CDC	dcMemory;
	{
		CClientDC dcScreen( 0 );						
		dcMemory.CreateCompatibleDC( &dcScreen );			
	}

	byte	*pdibBits = 0;
	
	HBITMAP	hDIBSection = ::CreateDIBSection( 
						dcMemory.m_hDC, 
						(BITMAPINFO*)&bmih, DIB_PAL_COLORS, (void**)&pdibBits, 0, 0 );

	if( pdibBits == NULL || hDIBSection == NULL )
		return S_FALSE;
	

	ASSERT( hDIBSection );

	CBitmap *psrcBitmap = CBitmap::FromHandle( hDIBSection  );

	ASSERT( psrcBitmap );

	//propare the memory DC to drawing
	CBitmap *poldBmp = dcMemory.SelectObject( psrcBitmap );

	CRect rcPaint = CRect(0,0,sizeRender.cx, sizeRender.cy);

	dcMemory.FillRect( &rcPaint, &CBrush( RGB(255,255,255) ) );

	
	if( hmf )
	{
		::PlayEnhMetaFile( dcMemory.m_hDC, hmf, &rcPaint );		
		::DeleteEnhMetaFile( hmf );
	}
	

	BYTE* pSource = pdibBits;
	BYTE* pTarget = (*ppbi) + ((BYTE)sizeof(BITMAPINFOHEADER)) +
									((BYTE)sizeof(TumbnailInfo));

	//Copy BITMAPINFOHEADER
	memcpy( *ppbi, &bmih, sizeof(BITMAPINFOHEADER) );

	//Copy TumbnailInfo
	TumbnailInfo tumbnailInfo;
	::ZeroMemory( &tumbnailInfo, sizeof( tumbnailInfo ) );
	tumbnailInfo.dwIcon = 0;
	tumbnailInfo.dwFlags = tfVarStretch;

	memcpy( (*ppbi+((BYTE)sizeof(BITMAPINFOHEADER))), &tumbnailInfo, sizeof(tumbnailInfo) );

	//Copy DIB bits
	memcpy( pTarget, pSource, dwSize - sizeof(BITMAPINFOHEADER) - ((BYTE)sizeof(TumbnailInfo)) );

	dcMemory.SelectObject( poldBmp );

	::DeleteObject(hDIBSection);	hDIBSection = 0;

	return S_OK;

}


HRESULT CImageRender::DrawICO(
			/*[in]*/	IUnknown* punkDataObject,
			/*[in]*/	DWORD dwFlag,
			/*[in]*/	LPARAM lParam,
			/*[in]*/	SIZE sizeThumbnail, 
			/*[out]*/	short* pnRenderSupport,
			/*[out]*/	BYTE** ppbi )
{
	*ppbi = NULL;	
	
	*pnRenderSupport = (short)rsFull;


	if( sizeThumbnail.cx < 1 || sizeThumbnail.cy < 1 )
		return S_FALSE;


	HICON hDefaultIcon	= NULL;
	HICON hObjectIcon	= NULL;
	
	
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	hDefaultIcon = AfxGetApp()->LoadIcon( IDI_DEFAULT_ICON );

	long lAdvMode = ::GetValueInt( ::GetAppUnknown(), THUMBNAIL_SECTION, THUMBNAIL_ADVANCED_MODE, 0 );
	if( !lAdvMode )
	{
		sptrINamedDataObject spNO( punkDataObject );
		if( spNO )
		{
			IUnknown* punkType = NULL;
			spNO->GetDataInfo( &punkType );
			if( punkType )
			{
				sptrINamedDataInfo spNDInfo( punkType );
				punkType->Release();
				if( spNDInfo )
				{
					spNDInfo->GetObjectIcon( &hObjectIcon, 0 );
				}
			}
		}		
	}
	else
	{
		hObjectIcon = AfxGetApp()->LoadIcon( IDI_IMAGE_ADV );
	}


	BITMAPINFOHEADER bmih;
	::ZeroMemory( &bmih, sizeof( bmih )  );
	bmih.biBitCount = 24;
	bmih.biHeight = 32;
	bmih.biWidth = 32;
	bmih.biSize = sizeof( bmih );
	bmih.biPlanes = 1;


	DWORD dwSize = ::ThumbnailGetSize( (BYTE*)&bmih );

	if( !::ThumbnailAllocate( ppbi, CSize( bmih.biWidth, bmih.biHeight ) ) )
		return S_FALSE;
	

	CDC	dcMemory;
	{
		CClientDC dcScreen( 0 );						
		dcMemory.CreateCompatibleDC( &dcScreen );			
	}




	byte	*pdibBits = 0;
	
	HBITMAP	hDIBSection = ::CreateDIBSection( 
						dcMemory.m_hDC, 
						(BITMAPINFO*)&bmih, DIB_PAL_COLORS, (void**)&pdibBits, 0, 0 );

	if( pdibBits == NULL || hDIBSection == NULL )
		return S_FALSE;
	

	ASSERT( hDIBSection );

	CBitmap *psrcBitmap = CBitmap::FromHandle( hDIBSection  );

	ASSERT( psrcBitmap );

	//propare the memory DC to drawing
	CBitmap *poldBmp = dcMemory.SelectObject( psrcBitmap );

	//At first fill background
	if( dwFlag == RF_NODEFINE )
	{
		dcMemory.FillRect( &CRect(0,0,sizeThumbnail.cx, sizeThumbnail.cy), 
			&CBrush( ::GetSysColor( COLOR_BTNFACE ) ) );			
	}
	else
	if( dwFlag == RF_BACKGROUND )
	{
		dcMemory.FillRect( &CRect(0,0,sizeThumbnail.cx, sizeThumbnail.cy), 
			&CBrush( (COLORREF)lParam ) );			
	}

	::DrawIcon( dcMemory.m_hDC, 0, 0, (hObjectIcon != NULL ? hObjectIcon : hDefaultIcon ) );

	

	BYTE* pSource = pdibBits;
	BYTE* pTarget = (*ppbi) + ((BYTE)sizeof(BITMAPINFOHEADER)) +
									((BYTE)sizeof(TumbnailInfo));

	//Copy BITMAPINFOHEADER
	memcpy( *ppbi, &bmih, sizeof(BITMAPINFOHEADER) );

	//Copy TumbnailInfo
	TumbnailInfo tumbnailInfo;
	::ZeroMemory( &tumbnailInfo, sizeof( tumbnailInfo ) );
	tumbnailInfo.dwIcon = TRUE;
	tumbnailInfo.dwFlags = tfCanStretch2_1;

	memcpy( (*ppbi+((BYTE)sizeof(BITMAPINFOHEADER))), &tumbnailInfo, sizeof(tumbnailInfo) );

	//Copy DIB bits
	memcpy( pTarget, pSource, dwSize - sizeof(BITMAPINFOHEADER) - ((BYTE)sizeof(TumbnailInfo)) );

	dcMemory.SelectObject( poldBmp );

	::DeleteObject(hDIBSection);


	if( hObjectIcon )
		::DestroyIcon( hObjectIcon );

	if( hDefaultIcon )
		::DestroyIcon( hDefaultIcon );


	return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// CDefaultRender

IMPLEMENT_DYNCREATE(CDefaultRender, CCmdTargetEx);

// {2AFBDF31-39E5-46f8-84C0-6CF3067DBB7B}
GUARD_IMPLEMENT_OLECREATE(CDefaultRender, szDefaultRenderProgID,
0x2afbdf31, 0x39e5, 0x46f8, 0x84, 0xc0, 0x6c, 0xf3, 0x6, 0x7d, 0xbb, 0x7b);



/////////////////////////////////////////////////////////////////////////////
BEGIN_INTERFACE_MAP(CDefaultRender, CCmdTargetEx)
	INTERFACE_PART(CDefaultRender, IID_IRenderObject, RenderObject)	
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CDefaultRender, RenderObject);


/////////////////////////////////////////////////////////////////////////////
CDefaultRender::CDefaultRender()
{
	_OleLockApp( this );
	EnableAggregation();

    m_binit_icons = false;
}

/////////////////////////////////////////////////////////////////////////////
CDefaultRender::~CDefaultRender()
{
	
	_OleUnlockApp( this );
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDefaultRender::XRenderObject::Support( IUnknown* punkDataObject, short* pnRenderSupport )
{
	_try_nested(CDefaultRender, RenderObject, Support)
	{
		*pnRenderSupport = (short)rsFull;

		return S_OK;
	}
	_catch_nested;

}
	
/////////////////////////////////////////////////////////////////////////////
HRESULT CDefaultRender::XRenderObject::GenerateThumbnail( 
					/*[in]*/	IUnknown* punkDataObject,
					/*[in]*/	int	nBitsCount,
					/*[in]*/	DWORD dwFlag,
					/*[in]*/	LPARAM lParam,
					/*[in]*/	SIZE sizeThumbnail, 
					/*[out]*/	short* pnRenderSupport,
					/*[out]*/	BYTE** ppbi,
					/*[in]*/	IUnknown* punkBag
					)

{
	_try_nested(CDefaultRender, RenderObject, GenerateThumbnail)
	{

		*ppbi = NULL;
		
		
		*pnRenderSupport = (short)rsFull;


		if( sizeThumbnail.cx < 1 || sizeThumbnail.cy < 1 )
			return S_FALSE;

		// [vanek] : init ovedloaded icons - 06.12.2004
		if( !pThis->m_binit_icons )
		{
			if( 0 != ::GetValueInt( GetAppUnknown(), "\\General", "UseOverloadedIcons", 0 ) )
			{
				CString str_path( _T("") );
				str_path = ::GetValueString( GetAppUnknown(), "\\Paths", "OverloadedIcons", str_path );
				if( !str_path.IsEmpty() )
				{
					CString str_filename( _T("") );
					str_filename = ::GetValueString( GetAppUnknown(), "OverloadedIcons", "VTObjectsIcons", str_filename );
					if( !str_filename.IsEmpty() )
					{
						str_filename = str_path + str_filename;
						pThis->m_icons_overloaded.Load( str_filename );
					}
                }
			}
			pThis->m_binit_icons = true;
		}


		HICON hDefaultIcon	= NULL;
		HICON hObjectIcon	= NULL;
		
		
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		hDefaultIcon = AfxGetApp()->LoadIcon( IDI_DEFAULT_ICON );

		sptrINamedDataObject spNO( punkDataObject );
		if( spNO )
		{
			// [vanek] : trying to get overload icon - 06.12.2004
			_bstr_t bstr_type;
			spNO->GetType( bstr_type.GetAddress() );
			if( !pThis->m_icons_overloaded.GetIcon( sizeThumbnail, bstr_type, &hObjectIcon ) )
			{   
				// none overload icon - get own icon
				IUnknown* punkType = NULL;
				spNO->GetDataInfo( &punkType );
				if( punkType )
				{
					sptrINamedDataInfo spNDInfo( punkType );
					punkType->Release();
					if( spNDInfo )
					{
						DWORD	dwFlags = 0;
						if( sizeThumbnail.cx <= 16 )
							dwFlags |= oif_SmallIcon;
						spNDInfo->GetObjectIcon( &hObjectIcon, dwFlags );
					}
				}
			}
		}		


		BITMAPINFOHEADER bmih;
		::ZeroMemory( &bmih, sizeof( bmih )  );
		bmih.biBitCount = 24;
		bmih.biHeight = sizeThumbnail.cx;
		bmih.biWidth = sizeThumbnail.cy;
		bmih.biSize = sizeof( bmih );
		bmih.biPlanes = 1;


		DWORD dwSize = ::ThumbnailGetSize( (BYTE*)&bmih );

		if( !::ThumbnailAllocate( ppbi, CSize( bmih.biWidth, bmih.biHeight ) ) )
			return S_FALSE;
		

		CDC	dcMemory;
		{
			CClientDC dcScreen( 0 );						
			dcMemory.CreateCompatibleDC( &dcScreen );			
		}




		byte	*pdibBits = 0;
		
		HBITMAP	hDIBSection = ::CreateDIBSection( 
							dcMemory.m_hDC, 
							(BITMAPINFO*)&bmih, DIB_PAL_COLORS, (void**)&pdibBits, 0, 0 );

		if( pdibBits == NULL || hDIBSection == NULL )
			return S_FALSE;
		

		ASSERT( hDIBSection );

		CBitmap *psrcBitmap = CBitmap::FromHandle( hDIBSection  );

		ASSERT( psrcBitmap );

		//propare the memory DC to drawing
		CBitmap *poldBmp = dcMemory.SelectObject( psrcBitmap );

		//At first fill background
		if( dwFlag == RF_NODEFINE )
		{
			dcMemory.FillRect( &CRect(0,0,bmih.biWidth, bmih.biHeight/*sizeThumbnail.cx, sizeThumbnail.cy*/), 
				&CBrush( ::GetSysColor( COLOR_BTNFACE ) ) );			
		}
		else
		if( dwFlag == RF_BACKGROUND )
		{
			dcMemory.FillRect( &CRect(0,0,bmih.biWidth, bmih.biHeight/*sizeThumbnail.cx, sizeThumbnail.cy*/), 
				&CBrush( (COLORREF)lParam ) );			
		}

		::DrawIconEx( dcMemory.m_hDC, 
			0, 0, 
			(hObjectIcon != NULL ? hObjectIcon : hDefaultIcon ),
			bmih.biWidth, bmih.biHeight,
			0, 0, DI_NORMAL );

		

		BYTE* pSource = pdibBits;
		BYTE* pTarget = (*ppbi) + ((BYTE)sizeof(BITMAPINFOHEADER)) +
										((BYTE)sizeof(TumbnailInfo));

		//Copy BITMAPINFOHEADER
		memcpy( *ppbi, &bmih, sizeof(BITMAPINFOHEADER) );

		//Copy TumbnailInfo
		TumbnailInfo tumbnailInfo;
		::ZeroMemory( &tumbnailInfo, sizeof( tumbnailInfo ) );
		tumbnailInfo.dwIcon = TRUE;
		tumbnailInfo.dwFlags = tfCanStretch2_1;

		memcpy( (*ppbi+((BYTE)sizeof(BITMAPINFOHEADER))), &tumbnailInfo, sizeof(tumbnailInfo) );

		//Copy DIB bits
		memcpy( pTarget, pSource, dwSize - sizeof(BITMAPINFOHEADER) - ((BYTE)sizeof(TumbnailInfo)) );

		dcMemory.SelectObject( poldBmp );

		::DeleteObject(hDIBSection);


		if( hObjectIcon )
			::DestroyIcon( hObjectIcon );

		if( hDefaultIcon )
			::DestroyIcon( hDefaultIcon );


		return S_OK;
	}
	_catch_nested;
}

