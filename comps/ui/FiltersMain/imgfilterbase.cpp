#include "stdafx.h"
#include "imgfilterbase.h"
#include "\vt5\common2\class_utils.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "imagefilterbase.inl"

BEGIN_INTERFACE_MAP(CThresholdingActionBase, CFilterBase)
	INTERFACE_PART(CThresholdingActionBase, IID_IDrawObject, Draw)
END_INTERFACE_MAP()


CThresholdingActionBase::CThresholdingActionBase()
{
	m_rect = NORECT;
	m_bPreviewAttached = false;
	m_pwfi = 0;
	m_dwOldFlags = 0;
	m_nPane = -1;
}

CThresholdingActionBase::~CThresholdingActionBase()
{
	if( m_bPreviewAttached )
	{
		IWindow2Ptr	ptrWindow( m_ptrTargetView );
		ptrWindow->DetachDrawing( GetControllingUnknown() );
		m_bPreviewAttached = false;

		IImageViewPtr	ptrIV( m_ptrTargetView );
		ptrIV->SetImageShowFlags( m_dwOldFlags );


		//InvalidateTargetWindow();
	}

	delete m_pwfi;
}

bool CThresholdingActionBase::InvokeFilter()
{
	m_image.Attach( GetDataArgument() );
	m_objects.Attach( GetDataResult() );

	m_rect = m_image.GetRect();
	m_pwfi = new CWalkFillInfo( m_rect );

	if( IsPreviewMode() )
		StartNotification( m_rect.Height() );
	else
		StartNotification( m_rect.Height(), 2 );

	if( !MakeBinaryImage() )
		return false;

	if( IsPreviewMode() )
		FinishNotification();
	else
		NextNotificationStage();

	if( !IsPreviewMode() )
	{
		if( !MeasureBinaryImage() )
			return false;
		FinishNotification();
	}
		

	return true;
}

void CThresholdingActionBase::DoInitializePreview()
{
	if( m_bPreviewAttached  )
		return;

	IApplicationPtr	ptrApp( GetAppUnknown() );
	IUnknown		*punkDoc = 0;
	ptrApp->GetActiveDocument( &punkDoc );

	if( !punkDoc )
		return;

	IDocumentSitePtr	ptrDocument( punkDoc );
	punkDoc->Release();
	
	IUnknown	*punkView = 0;
	ptrDocument->GetActiveView( &punkView );
	m_ptrTargetView = punkView;
	punkView->Release();

	if( !CheckInterface( m_ptrTargetView, IID_IImageView ) )
		return;

	
	IWindow2Ptr	ptrWindow( m_ptrTargetView );
	ptrWindow->AttachDrawing( GetControllingUnknown() );
	m_bPreviewAttached = true;

	IImageViewPtr	ptrIV( m_ptrTargetView );
	ptrIV->GetImageShowFlags( &m_dwOldFlags );
	ptrIV->SetImageShowFlags( isfSourceImage );
	ptrIV->SetSelectionMode( 1 );
}

void CThresholdingActionBase::DoFinalizePreview()
{
	if( m_bPreviewAttached )
	{
		IWindow2Ptr	ptrWindow( m_ptrTargetView );
		ptrWindow->DetachDrawing( GetControllingUnknown() );
		m_bPreviewAttached = false;

		IImageViewPtr	ptrIV( m_ptrTargetView );
		ptrIV->SetImageShowFlags( m_dwOldFlags );

		//InvalidateTargetWindow();

		m_ptrTargetView = 0;
	}

	MeasureBinaryImage();
}

COLORREF CThresholdingActionBase::GetFillColor()
{
	COLORREF	crFill = RGB( 0, 0, 0 );
	if( (m_dwOldFlags & isfPseudoImage) || (m_dwOldFlags & isfFilledImageExceptObjects) )
		crFill = ::GetValueColor( GetAppUnknown(), "Colors", "Background", RGB( 128, 128, 255 ) );
	else
		crFill = get_class_color( -1 );

	return crFill;
}

void CThresholdingActionBase::DoDraw( CDC &dc )
{
	COLORREF	crFill = GetFillColor();

	byte	red = GetRValue( crFill );
	byte	green = GetGValue( crFill );
	byte	blue = GetBValue( crFill );


	CRect	rectClip = m_rcInvalid;
	
	CPoint	pointScroll = GetScrollPos( m_ptrTargetView );
	double fZoom = GetZoom( m_ptrTargetView );

	CPoint	pointOffsetRgn = rectClip.TopLeft();
//convert clip to the client coordinates
	rectClip+=pointScroll;

	rectClip.left = (int)::floor( rectClip.left/fZoom );
	rectClip.top = (int)::floor( rectClip.top/fZoom );
	rectClip.right = (int)::ceil( rectClip.right/fZoom );
	rectClip.bottom = (int)::ceil( rectClip.bottom/fZoom );

//get the image rect
	CRect	rectImage;
	CRect	rectDraw;

	rectImage = m_image.GetRect();
//ajust rectangle
	rectImage.left = max( rectClip.left, rectImage.left );
	rectImage.top = max( rectClip.top, rectImage.top );
	rectImage.right = min( rectClip.right, rectImage.right );
	rectImage.bottom = min( rectClip.bottom, rectImage.bottom );

	if( rectImage.left >= rectImage.right ||
		rectImage.top >= rectImage.bottom )
		return;

	rectDraw.left = (int)::floor( rectImage.left*fZoom-pointScroll.x );
	rectDraw.top = (int)::floor( rectImage.top*fZoom-pointScroll.y );
	rectDraw.right = (int)::ceil( rectImage.right*fZoom-pointScroll.x );
	rectDraw.bottom = (int)::ceil( rectImage.bottom*fZoom-pointScroll.y );
	rectDraw-=pointOffsetRgn;


	int	cxDIB = m_pbi->biWidth;
	int	cyDIB = m_pbi->biHeight;
	int	cx4DIB = (cxDIB*3+3)/4*4;

	CPoint	point = m_image.GetOffset();

	int	cx = m_image.GetWidth();
	int	cy = m_image.GetHeight();

	CRect	rectDest = rectDraw;
	CRect	rect = rectImage;

	int	xStart = rectDest.left;
	int	yStart = rectDest.top;
	int	xEnd = rectDest.right;
	int	yEnd = rectDest.bottom;

	xStart = max( 0, min( m_pbi->biWidth, xStart ) );
	xEnd = max( 0, min( m_pbi->biWidth, xEnd ) );
	yStart = max( 0, min( m_pbi->biHeight, yStart ) );
	yEnd = max( 0, min( m_pbi->biHeight, yEnd ) );

	int x, y, xsrc, ysrc;


	int		cxDest = rectDest.right-rectDest.left;
	int		cyDest = rectDest.bottom-rectDest.top;
	int		cxSrc = rect.right-rect.left;
	int		cySrc = rect.bottom-rect.top;

	int	colors = m_image.GetColorsCount();

	byte	*pmask = 0;

	int	nImageColorsOffest = rect.left-point.x-rectDest.left*cxSrc/cxDest;
	int	nImageOffestCY = rect.top-point.y-rectDest.top*cySrc/cyDest;
	int	nInitialFlag = (cxDest>=cxSrc)?cxDest+(rectDest.left%(cxDest/cxSrc))*cxSrc:0;

	/*IUnknown	*punk = 0;
	m_image->GetColorConvertor( &punk );
	sptrIColorConvertor	sptrC( punk );
	punk->Release();*/

	for( y = yStart; y < yEnd; y++ )
	{
		ysrc = (y-rectDest.top)*cySrc/cyDest+rect.top-point.y;

		pmask = m_pwfi->GetRowMask( ysrc )+nImageColorsOffest;

		byte	*pdib = m_pdib+cx4DIB*(cyDIB-y-1)+xStart*3;

		if( cxDest >= cxSrc )
		{
			long	lFlag = nInitialFlag;
			pmask += xStart*cxSrc/cxDest;
			//pdibSrc += xStart*cxSrc/cxDest;

			for( x = xStart; x < xEnd; x++ )
			{										
				lFlag	-= cxSrc;
				if( lFlag < 0 )
				{
					lFlag+=cxDest;
					pmask++;
				}

				//xsrc = x*cxSrc/cxDest;
				if( !*pmask )
				{
					pdib++;
					pdib++;
					pdib++;
					continue;
				}

				{
					*pdib = blue;
					pdib++;
					*pdib = green;
					pdib++;
					*pdib = red;
					pdib++;
				}
			}
		}
		else
		{
			for( x = xStart; x < xEnd; x++ )
			{
				xsrc = (x-xStart)*cxSrc/cxDest;

				if( !pmask[xsrc] )
				{
					pdib++;
					pdib++;
					pdib++;
					continue;
				}
				
				{
					*pdib = blue;
					pdib++;
					*pdib = green;
					pdib++;
					*pdib = red;
					pdib++;
				}
			}
		}
	}
}


bool CThresholdingActionBase::MakeBinaryImage()
{
	AfxMessageBox( "CThresholdingActionBase::MakeBinaryImage() must be overrided" );
	return false;
}

bool CThresholdingActionBase::MeasureBinaryImage()
{
	CListLockUpdate	lock( m_objects );

	ASSERT( m_pwfi );


	CPoint	pointStart = CPoint( -1, -1 );
	CRect	rectObject;


	
	IUnknown* punkImage = 0;
	while( m_pwfi->InitContours(true, pointStart, &pointStart, NORECT, m_image, &punkImage) )
	{
		CImageWrp	image(punkImage, false);
		IUnknownPtr punkCC;
		image->GetColorConvertor(&punkCC);
		IColorConvertor6Ptr sptrCC(punkCC);
		if (sptrCC != 0)
		{
			INIT_OBJECT_DATA InitData;
			InitData.m_nSize = sizeof(INIT_OBJECT_DATA);
			InitData.m_nPane = m_nPane;
			sptrCC->InitObjectAfterThresholding(punkImage, m_image, &InitData);
		}

		CObjectWrp	object( ::CreateTypedObject( szTypeObject ), false );
		INamedDataObject2Ptr	ptrObject( object );
		ptrObject->SetParent( m_objects, 0 );

		for( int nCtr = 0; nCtr < m_pwfi->GetContoursCount(); nCtr++ )
		{
			Contour	*pc = m_pwfi->GetContour( nCtr );
			image->AddContour( pc );
		}

		m_pwfi->DetachContours();

		object.SetImage( image );
	}

	return true;
}

void CThresholdingActionBase::InvalidateTargetWindow()
{
	CWnd	*pwnd = GetWindowFromUnknown( m_ptrTargetView );

	if( pwnd && pwnd->GetSafeHwnd() )
		pwnd->Invalidate();
}


/////////////////////////////////////////////////////////////////////////////////////////
CImageFilterBase::CImageFilterBase()
{
	m_cx = m_cy = m_colors = 0;
	m_bImageInitialized = false;
	m_bMaskInitialized = false;
	m_nCurrentHistSum = 0;
	m_nCurrentHistArea = 0;
	m_pRowCache = 0;
	m_pCurrentHist  =0;
	m_pCurrentHistAppr = 0;
	m_pCurrentJumpUp = 0;
	m_pCurrentJumpDown = 0;
	m_nCurrentMedian = 0;

	m_pMask = 0;
	m_pMaskFull = 0;
	m_nCurrentX = m_nCurrentY = 0;
	m_nCurrentMaskSize = 0;
	m_nMedianPos = 0;
}

CImageFilterBase::~CImageFilterBase()
{
	delete m_pRowCache;
	delete m_pCurrentHist;
	delete m_pCurrentHistAppr;
	delete m_pCurrentJumpUp;
	delete m_pCurrentJumpDown;

	_DeleteMask( m_pMask );
	_DeleteMask( m_pMaskFull );

}
bool CImageFilterBase::InitImageFilter( int nStargeCount )
{
	m_image = GetDataArgument();

	if( m_image == 0 )
		return false;
	m_imageNew = GetDataResult();
	if( m_imageNew == 0 )
		return false;

	m_cx = m_image.GetWidth();
	m_cy = m_image.GetHeight();
	m_colors = m_image.GetColorsCount();

	if( m_cy == 0 )
		return false;

	InitImageResult();

	StartNotification( m_cy, nStargeCount );

	m_bImageInitialized = true;

	return true;
}

void CImageFilterBase::FinalizeImageFilter()
{
	m_image = m_imageNew = 0;
	FinishNotification();
	m_bImageInitialized = false;
}	

void CImageFilterBase::NotifyImage( int yPos, int cPos )
{
	ASSERT( m_bImageInitialized );

	int	nPos = (yPos+cPos*m_cy)/m_colors;
	Notify( nPos );
}

void CImageFilterBase::DoDrawMaskShape( CDC *pdc, int xCenter, int yCenter, int nMaskSize )
{
	pdc->Ellipse( CRect( xCenter-nMaskSize/2, yCenter-nMaskSize/2, 
						xCenter-nMaskSize/2+nMaskSize, yCenter-nMaskSize/2+nMaskSize ) );
}

bool CImageFilterBase::InitHistMask( int nMaskSize )
{
	ASSERT(m_bImageInitialized);

	m_nCurrentMaskSize = nMaskSize;
	m_nCurrentHistArea = 0;

	m_bMaskInitialized = true;

	ASSERT(!m_pRowCache);
	m_pRowCache = new color*[m_cy+nMaskSize*2+1];

	int	cx = nMaskSize+2;
	int	cy = nMaskSize+2;

	int xCenter = cx/2;
	int yCenter = cy/2;

	int	x, y;

	CRect	rect( 0, 0, cx, cy );
	CWalkFillInfo	info( rect );
	CDC	*pdc = info.GetDC();
	pdc->SelectStockObject( WHITE_PEN );
	pdc->SelectStockObject( WHITE_BRUSH );

	DoDrawMaskShape( pdc, xCenter, yCenter, nMaskSize );
	
	//pdc->Rectangle( CRect( xCenter-nMaskSize/2, yCenter-nMaskSize/2, 
	//					xCenter-nMaskSize/2+nMaskSize, yCenter-nMaskSize/2+nMaskSize ) );

	m_pCurrentHist = new long[colorMax	+ 1];
	m_pCurrentJumpUp = new int[colorMax	+ 1];
	m_pCurrentJumpDown = new int[colorMax	+ 1];
	m_pCurrentHistAppr = new long[256];

	for( int i = 0; i <= colorMax; i++ )
	{
		m_pCurrentJumpUp[i] = m_pCurrentJumpDown[i] = -1;
		m_pCurrentHist[i] = 0;
	}
	ZeroMemory( m_pCurrentHistAppr, sizeof( long )*256 );
	m_nCurrentMin = -1;
	m_nCurrentMax = -1;


	MaskCol		*pOldMask = 0;
	MaskCol		*pCurrentMask = 0;
	//init the full mask
	for( x = 0; x <= cx; x++ )
	{
		for( y = 0; y <= cy; y++ )
		{
			byte	byteCurrentPixel = info.GetPixel( x, y );

			if( pCurrentMask && byteCurrentPixel == 0 )	//remove last mask if nessesary
				pCurrentMask = 0;

			if( byteCurrentPixel  )	//start label
			{
				if( !pCurrentMask )
				{
					pCurrentMask = new MaskCol;
					pCurrentMask->xPos = x-xCenter;
					pCurrentMask->yPos = y-yCenter;
					pCurrentMask->bAdd = true;
					pCurrentMask->pNext = 0;
					pCurrentMask->nPointCount = 0;
					if( pOldMask )
						pOldMask->pNext = pCurrentMask;

					pOldMask = pCurrentMask;

					if( !m_pMaskFull )
						m_pMaskFull = pCurrentMask;
				}
				pCurrentMask->nPointCount++;
				m_nCurrentHistArea++;
			}
		}

		ASSERT( !pCurrentMask );
	}

	m_nMedianPos = m_nCurrentHistArea/2;

	//label the bytes
	for( y = 0; y < cy; y++ )
	{
		byte	byteOld = 0;
		byte	byteCur = 0;
		for( x = 0; x <= cx; x++ )
		{
			byteOld = byteCur;
			byteCur = info.GetPixel( x, y );

			if( byteOld == 0 && byteCur == 255 )
				info.SetPixel( x, y, 88 );	//start label
			if( byteOld == 255 && byteCur == 0 )
				info.SetPixel( x, y, 77 );	//end label
		}
	}
	

	pOldMask = 0;
	//init the mask cols
	for( x = 0; x < cx; x++ )
	{
		for( y = 0; y <= cy; y++ )
		{
			byte	byteCurrentPixel = info.GetPixel( x, y );

			if( pCurrentMask )	//remove last mask if nessesary
			{
				if( byteCurrentPixel == 0 || byteCurrentPixel == 0xFF ||
					(byteCurrentPixel == 77 &&!pCurrentMask->bAdd ) ||
					(byteCurrentPixel == 88 && pCurrentMask->bAdd ) )
				{
					pCurrentMask = 0;
				}
			}

			if( byteCurrentPixel == 77|| byteCurrentPixel == 88 )	//start label
			{
				if( !pCurrentMask )
				{
					pCurrentMask = new MaskCol;
					pCurrentMask->xPos = x-xCenter;
					pCurrentMask->yPos = y-yCenter;
					pCurrentMask->bAdd = byteCurrentPixel == 77;
					pCurrentMask->pNext = 0;
					pCurrentMask->nPointCount = 0;
					if( pOldMask )
						pOldMask->pNext = pCurrentMask;
					pOldMask = pCurrentMask;

					if( !m_pMask )
						m_pMask = pCurrentMask;
				}
				pCurrentMask->nPointCount++;
			}
		}

		ASSERT( !pCurrentMask );
	}

	return true;
}

void CImageFilterBase::InitColorCache( int nColor )
{
	ASSERT(m_pRowCache);
	for( int i = 0; i < m_cy+m_nCurrentMaskSize*2; i++ )
		m_pRowCache[i] = m_image.GetRow( max( 0, min( m_cy-1, i-m_nCurrentMaskSize ) ), nColor );
}

void CImageFilterBase::InitMask( int nRow )
{
	ASSERT(m_bMaskInitialized);

	m_nCurrentHistSum = 0;
	m_nCurrentX = 0;
	m_nCurrentY = nRow;

	m_nCurrentMin = -1;
	m_nCurrentMax = -1;

	MoveMaskRight( true );
}

void CImageFilterBase::InitApprHistMask( int nRow )
{
	ASSERT(m_bMaskInitialized);

	m_nCurrentHistSum = 0;
	m_nCurrentX = 0;
	m_nCurrentY = nRow;

	ZeroMemory( m_pCurrentHistAppr, sizeof( long )*256 );
	m_nCurrentMin = -1;
	m_nCurrentMax = -1;

	MoveApprHistMaskRight( true );
}

void CImageFilterBase::_DeleteMask( MaskCol *pMask )
{
	if( !pMask )
		return;
	_DeleteMask( pMask->pNext );
	delete pMask;
}

void CImageFilterBase::MoveMaskRight( bool bInit )
{
	MaskCol	*pCurMask = bInit?m_pMaskFull:m_pMask;

	int	x, y, i;

	while( pCurMask )
	{
		x = max( 0, min( m_cx-1, m_nCurrentX+pCurMask->xPos ) );
		y = m_nCurrentY+pCurMask->yPos+m_nCurrentMaskSize;

		if( pCurMask->bAdd )
			for( i = 0; i < pCurMask->nPointCount; i++ )
			{
				color	colorVal = *(m_pRowCache[i+y]+x);
				m_nCurrentHistSum+=colorVal;
			}
		else
			for( i = 0; i < pCurMask->nPointCount; i++ )
			{
				color	colorVal = *(m_pRowCache[i+y]+x);
				m_nCurrentHistSum-=colorVal;
			}
		pCurMask = pCurMask->pNext;
	}

	if( !bInit )
		m_nCurrentX++;
}


void CImageFilterBase::MoveApprHistMaskRight( bool bInit )
{
	MaskCol	*pCurMask = bInit?m_pMaskFull:m_pMask;
	int	x, y, i;

	while( pCurMask )
	{
		x = max( 0, min( m_cx-1, m_nCurrentX+pCurMask->xPos ) );
		y = m_nCurrentY+pCurMask->yPos+m_nCurrentMaskSize;

		
		for( i = 0; i < pCurMask->nPointCount; i++ )
		{
			color	colorVal = *(m_pRowCache[i+y]+x);
			
			if( pCurMask->bAdd )
				m_pCurrentHistAppr[::ColorAsByte( colorVal )]++;
			else
				m_pCurrentHistAppr[::ColorAsByte( colorVal )]--;
		}
	
		pCurMask = pCurMask->pNext;
	}

	if( !bInit )
		m_nCurrentX++;
}

void CImageFilterBase::InitImageResult()
{
	ICompatibleObjectPtr ptrCO( m_imageNew );
	if( ptrCO == NULL )
	{
		ASSERT( FALSE );
		return;
	}

	if( S_OK != ptrCO->CreateCompatibleObject( m_image, NULL, 0 ) )
	{	
		ASSERT( FALSE );
		return;
	}		

}

bool CImageFilterBase::IsAvaible()
{
	bool bres = CFilterBase::IsAvaible();
	/*
	if( bres )
	{
		IDocumentSitePtr ptrDocSite( m_punkTarget );
		if( ptrDocSite )
		{
			IUnknown* punkView = 0;
			ptrDocSite->GetActiveView( &punkView );
			if( punkView )
			{
				if( !CheckInterface( punkView, IID_IImageView ) )
					bres = false;

				punkView->Release();
				punkView = 0;
			}
		}
	}
	*/
	
	return bres;

}

