#pragma once
#include "GDIPlusManager.h"

enum ImageDrawFlags
{
	idfUseAlpha = 1 << 0,
	idfScalingImage = 1 << 1,
};

class CImageDrawer
{
	static glib_utils::CGDIPlusManager *m_pgdipman;
	static long m_ldrawer_count;

	float			m_falpha;
	bool			m_benable_draw;

public:
	CImageDrawer(void);
	virtual ~CImageDrawer(void);

	void	SetAlpha( float fValue ) { m_falpha = fValue; }
	float	GetAlpha( ) { return m_falpha; }

	void	SetEnableDraw( bool bValue ) { m_benable_draw = bValue; }
	bool	GetEnableDraw( ) { return m_benable_draw; }

	bool	DrawImage( HDC hDC, HBITMAP hBitmap, RECT rcDrawRect, DWORD dwFlags = 0 );
	bool	DrawImage( HDC hDC, HICON hIcon, RECT rcDrawRect, DWORD dwFlags = 0 );

	bool	DrawImage( Graphics *pGraphics, Bitmap *pBitmap, RectF rcfDrawRect, DWORD dwFlags = 0, ImageAttributes *pImageAttr = 0 );
};
