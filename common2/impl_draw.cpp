#include "stdafx.h"
#include "impl_draw.h"

/////////////////////////////////////////////////////////////////////////////////////////
//CDrawObjectImpl
CDrawObjectImpl::CDrawObjectImpl()
{
}

CDrawObjectImpl::~CDrawObjectImpl()
{
}

HRESULT CDrawObjectImpl::Paint( HDC hDC, RECT rectDraw, IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache, BYTE *pdibCache )
{
	return S_OK;
}

HRESULT CDrawObjectImpl::GetRect( RECT *prect )
{
	if( prect )ZeroMemory( prect, sizeof( RECT ) );
	return E_NOTIMPL;
}

HRESULT CDrawObjectImpl::SetTargetWindow( IUnknown *punkTarget )
{
	return E_NOTIMPL;
}

