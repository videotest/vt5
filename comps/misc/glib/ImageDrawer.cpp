#include "StdAfx.h"
#include "imagedrawer.h"
#include "GDIPlusManager.h"
#include "misc_dbg.h"


glib_utils::CGDIPlusManager *CImageDrawer::m_pgdipman = 0;
long	CImageDrawer::m_ldrawer_count = 0;

CImageDrawer::CImageDrawer(void)
{			 
	m_falpha = 0.5;
	m_benable_draw = true;

	// if( !m_pgdipman )
	//	m_pgdipman = new  glib_utils::CGDIPlusManager;

	m_ldrawer_count ++;
}

CImageDrawer::~CImageDrawer(void)
{
	m_ldrawer_count --;
	if( m_pgdipman && !m_ldrawer_count )
	{
		delete m_pgdipman;
		m_pgdipman = 0;
	}
}

bool	CImageDrawer::DrawImage( HDC hDC, HBITMAP hBitmap, RECT rcDrawRect, DWORD dwFlags /*= 0*/ )
{
	if( !m_pgdipman )
		m_pgdipman = new  glib_utils::CGDIPlusManager;

	// [vanek] - 08.03.2005
	if( !m_benable_draw || !hDC || !hBitmap ) 
		return false;

	return DrawImage( &Graphics(hDC), &Bitmap(hBitmap, 0), RectF((REAL)(rcDrawRect.left),(REAL)(rcDrawRect.top),
		(REAL)(rcDrawRect.right - rcDrawRect.left), (REAL)(rcDrawRect.bottom - rcDrawRect.top)), dwFlags );
}

bool	CImageDrawer::DrawImage( HDC hDC, HICON hIcon, RECT rcDrawRect, DWORD dwFlags /*= 0*/ )
{
	if( !m_pgdipman )
		m_pgdipman = new  glib_utils::CGDIPlusManager;

	// [vanek] - 08.03.2005
	if( !m_benable_draw || !hDC || !hIcon ) 
		return false;

	return  DrawImage( &Graphics(hDC), &Bitmap(hIcon), RectF((REAL)(rcDrawRect.left),(REAL)(rcDrawRect.top),
		(REAL)(rcDrawRect.right - rcDrawRect.left), (REAL)(rcDrawRect.bottom - rcDrawRect.top)), dwFlags );
}


bool	CImageDrawer::DrawImage( Graphics *pGraphics, Bitmap *pBitmap, RectF rcfDrawRect, DWORD dwFlags /*= 0*/, ImageAttributes *pImageAttr /*= 0*/ )
{
	if( !m_pgdipman )
		m_pgdipman = new  glib_utils::CGDIPlusManager;

	if( !m_benable_draw || !pGraphics || !pBitmap )
		return false;

	ImageAttributes *pim_attr = 0;
	if( pImageAttr )
		pim_attr = pImageAttr;
	else
		pim_attr = new ImageAttributes();

	if( !pim_attr )
	{
		_assert( false );
		return false;
	}
    
	REAL falpha = 1.f;
	if( dwFlags & idfUseAlpha )
		falpha = (REAL)(m_falpha);

	ColorMatrix color_matrix = {  
			1.0f,	0.0f,	0.0f,	0.0f,	0.0f,
			0.0f,	1.0f,	0.0f,	0.0f,	0.0f,
			0.0f,	0.0f,	1.0f,	0.0f,	0.0f,
			0.0f,	0.0f,	0.0f,	falpha,	0.0f,
			0.0f,	0.0f,	0.0f,	0.0f,	1.0f};

			pim_attr->SetColorMatrix( &color_matrix, ColorMatrixFlagsDefault, ColorAdjustTypeDefault);

	bool bret_value = true;

	// save clipping region
	Region old_clipRegion;
	pGraphics->GetClip(&old_clipRegion);
	// set new clipping rectangle
	pGraphics->SetClip( rcfDrawRect, CombineModeReplace );

    RectF rcf_drawrect = rcfDrawRect;
    if( dwFlags & idfScalingImage )
		pGraphics->SetInterpolationMode( InterpolationModeHighQuality );
	else
	{
		rcf_drawrect.Width = (REAL)pBitmap->GetWidth();
		rcf_drawrect.Height = (REAL)pBitmap->GetHeight();
	}

	// draw image
	bret_value = Ok == pGraphics->DrawImage( pBitmap, rcf_drawrect, 0.f, 0.f, (REAL)pBitmap->GetWidth(), 
		(REAL)pBitmap->GetHeight(), UnitPixel, pim_attr, 0, 0);

	// restore clipping region
	pGraphics->SetClip( &old_clipRegion, CombineModeReplace );

	if( !pImageAttr && pim_attr )
		delete pim_attr;
	pim_attr = 0;

	return bret_value;
}
