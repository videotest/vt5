#include "stdafx.h"
#include "binimageint.h"
#include "morph1.h"
#include "nameconsts.h"

/////////////////////////////////////////////////////////////////////////////////////////
//morpfology image - image with bytes data
CMorphImage::CMorphImage()
{
	m_pbin = 0;
	m_cx = m_cy = m_cx4 = 0;
	m_x = m_y = 0;
}

CMorphImage::~CMorphImage()
{
	Free();
}

HRESULT CMorphImage::Free()
{
	if( m_pbin )delete m_pbin;
	m_pbin = 0;
	m_cx = m_cy = m_cx4 = 0;
	m_x = m_y = 0;
	return S_OK;
}


HRESULT CMorphImage::InitFromBin( IBinaryImage *pbinImage )
{
	int	cx, cy;
	HRESULT		hr;
	hr = pbinImage->GetSizes( &cx, &cy );
	
	if( hr != S_OK )
	{
		//_trace( "Error getting size\n" );
		return hr;
	}

	hr = CreateNew( cx, cy );
	if( hr != S_OK )
	{
		//_trace( "Error creating image\n" );
		return hr;
	}

	byte	*pbinC, *pbin;
	int	x, y, bit;

	for( y = 0; y < cy; y++ )
	{
		pbinImage->GetRow( &pbinC, y, 0 );
		GetRow( &pbin, y, 0 );

		for( x = 0, bit = 7; x < cx; x++, pbin++, bit-- )
		{
			if( *pbinC & ( 1<<bit ) )
				*pbin = 0xFF;
			else
				*pbin = 0;
			if( bit == 0 )
			{
				bit = 8;
				pbinC++;
			}
		}
	}
	return S_OK;
}

HRESULT CMorphImage::StoreToBin( IBinaryImage *pbinImage )
{
	HRESULT		
	
	hr = pbinImage->CreateNew( m_cx, m_cy );

	if( hr != S_OK )
	{
		//_trace( "Error creaing binary\n" );
		return hr;
	}
	byte	*pbinC, *pbin;
	int	x, y, bit;

	for( y = 0; y < m_cy; y++ )
	{
		pbinImage->GetRow( &pbinC, y, 0 );
		GetRow( &pbin, y, 0 );

		//init first as '0'
		*pbinC = 0;

		for( x = 0, bit = 7; x < m_cx; x++, pbin++, bit-- )
		{
			if( *pbin )*pbinC |= ( 1<<bit );
			
			if( bit == 0 )
			{
				bit = 8;
				pbinC++;
				*pbinC = 0;
			}
		}
	}
	return S_OK;
}


HRESULT CMorphImage::GetRow( byte **prow, int nRow, BOOL bWithDWORDBorders )
{
	*prow = 0;
	if( !m_pbin )return E_POINTER;
	*prow = m_pbin+nRow*m_cx4;
	return S_OK;
}

HRESULT CMorphImage::GetSizes( int *pcx, int *pcy )
{
	*pcx = m_cx;
	*pcy = m_cy;
	return S_OK;
}

HRESULT CMorphImage::CreateNew( int cx, int cy )
{
	Free();

	m_cx = cx;
	m_cy = cy;
	m_cx4 = (cx+3)/4*4;
	m_pbin =  new byte[m_cx4*m_cy];

	return m_pbin ? S_OK:E_OUTOFMEMORY;
}

HRESULT CMorphImage::GetOffset( POINT *pt )
{
	pt->x = m_x;
	pt->y = m_y;

	return S_OK;
}

HRESULT CMorphImage::SetOffset( POINT pt, BOOL bMoveImage )
{
	m_x = pt.x;
	m_y = pt.y;

	return S_OK;
}

