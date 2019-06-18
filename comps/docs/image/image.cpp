#include "stdafx.h"
#include "image.h"
#include "misc_calibr.h"

CBinaryImage::CBinaryImage()
{
	m_ppRows = 0;
	m_cx = m_cy = m_cx4 = 0;
	ZeroMemory(&m_ptOffset, sizeof(POINT));
	m_guidC = GUID_NULL;m_fCalibr = 1;

	//load type lib && type info
	m_pi_type_lib	= 0;
	m_pi_type_info	= 0;
	{
		char sz_path[MAX_PATH];	sz_path[0] = 0;
		::GetModuleFileName( 0, sz_path, sizeof(sz_path) );
		char* psz_last = strrchr( sz_path, '\\' );
		if( psz_last )
		{
			psz_last++;
			strcpy( psz_last, "comps\\BinaryImage.tlb" );
			HRESULT hr = ::LoadTypeLib( _bstr_t( sz_path ), &m_pi_type_lib );
			dbg_assert( S_OK == hr );
			if( m_pi_type_lib )
			{
				hr = m_pi_type_lib->GetTypeInfoOfGuid( IID_IBinaryImageDisp, &m_pi_type_info );
				dbg_assert( S_OK == hr );
			}
		}		
	}
}

CBinaryImage::~CBinaryImage()
{
	if( m_pi_type_lib )
		m_pi_type_lib->Release(); m_pi_type_lib = 0;

	if( m_pi_type_info )
		m_pi_type_info->Release(); m_pi_type_info = 0;

	Free();
}

HRESULT CBinaryImage::SetRect(RECT rc)
{
	POINT	point;
	point.x = rc.left;
	point.y = rc.top;
	return Move( point );
}

HRESULT CBinaryImage::GetRect(RECT* prc)
{
	::SetRect(prc, m_ptOffset.x, m_ptOffset.y, m_ptOffset.x+m_cx, m_ptOffset.y+m_cy );
	return S_OK;
}

HRESULT CBinaryImage::HitTest( POINT point, long *plHitTest )
{
	*plHitTest = 0;
	if( point.x >= m_ptOffset.x &&
		point.x < m_ptOffset.x+m_cx &&
		point.y >= m_ptOffset.y &&
		point.y < m_ptOffset.y+m_cy )

	{
		byte	*p = 0;
		GetRow( &p, point.y - m_ptOffset.y, 0 );
		if( !p )return E_FAIL;
		p+=(point.x - m_ptOffset.x);
		if( *p )
			*plHitTest = 1;
	}

	return S_OK;
}

HRESULT CBinaryImage::Move( POINT point )
{
	m_ptOffset.x = point.x;
	m_ptOffset.y = point.y;
	return S_OK;
}



HRESULT CBinaryImage::GetCalibration( double *pfPixelPerMeter, GUID *pguidC )
{
	*pfPixelPerMeter = m_fCalibr;
	*pguidC = m_guidC;
	return S_OK;
}
HRESULT CBinaryImage::SetCalibration( double fPixelPerMeter, GUID *pguidC )
{
	m_fCalibr = fPixelPerMeter;
	m_guidC = *pguidC;
	return S_OK;
}

HRESULT CBinaryImage::Free()
{
	if(m_ppRows)
	{
		// A.M. fix BT4787 - allocating single memory block for the all rows.
		delete m_ppRows[0];
//		for(long y = 0; y < m_cy; y++)
//			delete m_ppRows[y];
		delete m_ppRows;
		m_ppRows = 0;
	}

	m_cx = m_cy = m_cx4 = 0;
	ZeroMemory(&m_ptOffset, sizeof(POINT));
	m_guidC = GUID_NULL;m_fCalibr = 1;
	return S_OK;
}

HRESULT CBinaryImage::GetRow( byte **prow, int nRow, BOOL bWithDWORDBorders )
{
	if(nRow < 0) nRow = 0;
	if(nRow >= m_cy) nRow = m_cy - 1;

	if(prow)
	{
		*prow = m_ppRows[nRow];
		/*if(bWithDWORDBorders == FALSE)
			*prow += sizeof(DWORD);*/
		return S_OK;
	}
	else
		return E_INVALIDARG;
}

HRESULT CBinaryImage::GetSizes( int *pcx, int *pcy )
{
	*pcx = m_cx;
	*pcy = m_cy;
	return S_OK;
}

HRESULT CBinaryImage::CreateNew( int cx, int cy )
{
	Free();

	::GetCalibration( Unknown(), &m_fCalibr, &m_guidC );


	m_cx = cx;
	m_cy = cy;
	m_cx4 = (cx+3)/4*4;

	m_ppRows = new byte*[cy];
	// A.M. fix BT4787 - allocating single memory block for the all rows.
	byte *p = new byte[m_cx4*cy];
	ZeroMemory(p, m_cx4*cy);

	for(long y = 0; y < cy; y++)
	{
		m_ppRows[y] = p+y*m_cx4;
		/*m_ppRows[y] = new byte[m_cx4];
		ZeroMemory(m_ppRows[y], m_cx4);*/
	}

	return m_ppRows ? S_OK:E_OUTOFMEMORY;
}

HRESULT CBinaryImage::GetOffset( POINT *pt )
{
	if(pt)
	{
		memcpy(pt, &m_ptOffset, sizeof(POINT));
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT CBinaryImage::SetOffset( POINT pt, BOOL bMoveImage )
{
	memcpy(&m_ptOffset, &pt, sizeof(POINT));

	if(bMoveImage == TRUE)
	{
	}

	return S_OK;
}

HRESULT CBinaryImage::Load( IStream *pStream, SerializeParams *pparams )
{
	if(!pStream)
		return E_INVALIDARG;

	Free();

	ULONG nRead = 0;
	int	nVersion = 1;

	pStream->Read(&m_cx, sizeof(int), &nRead);
	if( m_cx < 0 )
	{
		pStream->Read(&nVersion, sizeof(nVersion), &nRead);
		pStream->Read(&m_cx, sizeof(int), &nRead);
	}
	pStream->Read(&m_cy, sizeof(int), &nRead);
	pStream->Read(&m_cx4, sizeof(int), &nRead);
	pStream->Read(&m_ptOffset.x, sizeof(int), &nRead);
	pStream->Read(&m_ptOffset.y, sizeof(int), &nRead);

	if(m_cy > 0)
	{
		m_ppRows = new byte*[m_cy];

		//long nRowSize = ((m_cx+7)/8)+4 + sizeof(DWORD)*2;
		// A.M. fix 2292
		byte *p = new byte[m_cx4*m_cy];
		ZeroMemory(p, m_cx4*m_cy);

		for(long y = 0; y < m_cy; y++)
		{
			m_ppRows[y] = p+y*m_cx4;
//			m_ppRows[y] = new byte[m_cx4];
			pStream->Read(m_ppRows[y], m_cx4, &nRead);
		}
	}

	if( nVersion >= 2 )
	{
		pStream->Read(&m_guidC, sizeof(m_guidC), &nRead);
		pStream->Read(&m_fCalibr, sizeof(m_fCalibr), &nRead);
	}

	
	return CObjectBase::Load(pStream, 0);
}

HRESULT CBinaryImage::Store( IStream *pStream, SerializeParams *pparams )
{
	if(!pStream)
		return E_INVALIDARG;
	
	int	nVersion = -1;
	ULONG nWritten = 0;
	pStream->Write(&nVersion, sizeof(nVersion), &nWritten);
	nVersion = 2;
	pStream->Write(&nVersion, sizeof(nVersion), &nWritten);
	pStream->Write(&m_cx, sizeof(int), &nWritten);
	pStream->Write(&m_cy, sizeof(int), &nWritten);
	pStream->Write(&m_cx4, sizeof(int), &nWritten);
	pStream->Write(&m_ptOffset.x, sizeof(int), &nWritten);
	pStream->Write(&m_ptOffset.y, sizeof(int), &nWritten);

	for(long y = 0; y < m_cy; y++)
		pStream->Write(m_ppRows[y], m_cx4, &nWritten);
	
	pStream->Write(&m_guidC, sizeof(m_guidC), &nWritten);
	pStream->Write(&m_fCalibr, sizeof(m_fCalibr), &nWritten);

	return CObjectBase::Store(pStream, 0);
}

HRESULT CBinaryImage::IsBaseObject(BOOL * pbFlag)
{
	* pbFlag = false;
	return S_OK;
}



/////////////////////////////////////////////////
//interface CBinaryImage (dispatch)
//////////////////////////////////////////////////////////////////////
HRESULT CBinaryImage::GetTypeInfoCount( 
            /* [out] */ UINT *pctinfo)
{
	if( !pctinfo )
		return E_POINTER;

	*pctinfo = 1;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CBinaryImage::GetTypeInfo( 
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo)
{
	if( !ppTInfo )
		return E_POINTER;

	if( !iTInfo )
		return E_INVALIDARG;

	if( !m_pi_type_info )
		return E_FAIL;

	m_pi_type_info->AddRef();

	(*ppTInfo) = m_pi_type_info;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CBinaryImage::GetIDsOfNames( 
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId)
{
	if( !m_pi_type_info )
		return E_FAIL;

	return m_pi_type_info->GetIDsOfNames( rgszNames, cNames, rgDispId );
}


//////////////////////////////////////////////////////////////////////
HRESULT CBinaryImage::Invoke( 
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr)
{
	if( !m_pi_type_info )
		return E_FAIL;

	return m_pi_type_info->Invoke(	(IUnknown*)(IBinaryImageDisp*)this, dispIdMember, wFlags, 
		pDispParams, pVarResult, pExcepInfo, puArgErr );
}

HRESULT CBinaryImage::get_Name(BSTR *pbstr )
{
	INamedObject2Ptr sptrO = Unknown();

	if( sptrO )
		sptrO->GetName(	pbstr  );

	return S_OK;
}

HRESULT CBinaryImage::put_Name(BSTR bstr )
{
	INamedObject2Ptr sptrO = Unknown();

	if( sptrO )
		sptrO->SetName(	bstr  );

	return S_OK;
}

HRESULT CBinaryImage::GetType( /*[out, retval]*/ BSTR *pbstrType )
{
	_bstr_t bstrType(szArgumentTypeBinaryImage);
	*pbstrType = bstrType.Detach();
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////////////////
//wrapper
CBinaryBitImage::CBinaryBitImage()
{
	m_cx = m_cy = 0;
	m_ppRows = 0;
	ZeroMemory(&m_ptOffset, sizeof(POINT));
}

CBinaryBitImage::~CBinaryBitImage()
{
	Free();
}

HRESULT CBinaryBitImage::InitFromBin( IBinaryImage *pbinImage )
{
//convert to bits
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


  for( y = 0; y < m_cy; y++ )
	{
		GetRow( &pbinC, y, 0 );
		pbinImage->GetRow( &pbin, y, 0 );

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

HRESULT CBinaryBitImage::StoreToBin( IBinaryImage *pbinImage )
{
//convert to bytes
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
		GetRow( &pbinC, y, 0 );
		pbinImage->GetRow( &pbin, y, 0 );

		for( x = 0, bit = 7; x < m_cx; x++, pbin++, bit-- )
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


HRESULT CBinaryBitImage::CreateNew( int cx, int cy )
{
	Free();

	int	cx4 = ((cx+7)/8)+8;

	m_cx = cx;
	m_cy = cy;
	
	m_ppRows = new byte*[cy];

	for(long y = 0; y < cy; y++)
	{
		long nRowSize = cx4 + sizeof(DWORD)*2;
		m_ppRows[y] = new byte[nRowSize];
		ZeroMemory(m_ppRows[y], nRowSize);
	}

	return S_OK;
}

HRESULT CBinaryBitImage::GetSizes( int *pcx, int *pcy )
{
	*pcx = m_cx;
	*pcy = m_cy;
	return S_OK;
}

HRESULT CBinaryBitImage::Free()
{
	if(m_ppRows)
	{
		for(long y = 0; y < m_cy; y++)
			delete m_ppRows[y];
		delete m_ppRows;
		m_ppRows = 0;
	}

	m_cx = m_cy = 0;

	m_ptOffset.x = 0;
	m_ptOffset.y = 0;

	return S_OK;
}

HRESULT CBinaryBitImage::GetRow(byte **prow, int nRow, BOOL bWithDWORDBorders)
{
	if(nRow < 0) nRow = 0;
	if(nRow >= m_cy) nRow = m_cy - 1;

	if(prow)
	{
		*prow = m_ppRows[nRow];
		if(bWithDWORDBorders == FALSE)
			*prow += sizeof(DWORD);
		return S_OK;
	}
	else
		return E_INVALIDARG;
}

HRESULT CBinaryBitImage::GetOffset( POINT *pt )
{
	if(pt)
	{
		memcpy(pt, &m_ptOffset, sizeof(POINT));
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT CBinaryBitImage::SetOffset( POINT pt, BOOL bMoveImage )
{
	memcpy(&m_ptOffset, &pt, sizeof(POINT));

	if(bMoveImage == TRUE)
	{
	}

	return S_OK;
}

void CBinaryBitImage::ClearBorders()
{
	int	cx4 = ((m_cx+7)/8);
	int nBit = (m_cx+7) % 8;
	byte bitMask = 128 >> nBit;
	for (int y = 0; y < m_cy; y++)
	{
		byte *pRow;
		GetRow(&pRow, y, TRUE);
		memset(pRow, 0, 4);
		memset(pRow+cx4+4, 0, 4);
		pRow[cx4+3] &= -bitMask;
	}
}

void CBinaryBitImage::FillBorders()
{
	int	cx4 = ((m_cx+7)/8);
	int nBit = (m_cx+7) % 8;
	byte bitMask = 128 >> nBit;
	for (int y = 0; y < m_cy; y++)
	{
		byte *pRow;
		GetRow(&pRow, y, TRUE);
		memset(pRow, 255, 4);
		memset(pRow+cx4+4, 255, 4);
		pRow[cx4+3] |= bitMask-1;
	}
}

void CBinaryBitImage::ExpandBorders()
{
	int	cx4 = (m_cx+7) / 8;
	int nBit = (m_cx+7) % 8;
	byte bitMask = 128 >> nBit;

	for (int y = 0; y < m_cy; y++)
	{
		byte *pRow;
		GetRow(&pRow, y, TRUE);
		if(pRow[4]&128)
			memset(pRow, 255, 4);
		else
			memset(pRow, 0, 4);
		if( pRow[cx4+3] & bitMask )
		{
			memset(pRow+cx4+4, 255, 4);
			pRow[cx4+3] |= bitMask-1;
		}
		else
		{
			memset(pRow+cx4+4, 0, 4);
			pRow[cx4+3] &= -bitMask;
		}
	}
}
 
/*HRESULT CBinaryBitImage::Load( IStream *pStream, SerializeParams *pparams )
{
	if(!pStream)
		return E_INVALIDARG;

	Free();

	ULONG nRead = 0;
	pStream->Read(&m_cx, sizeof(int), &nRead);
	pStream->Read(&m_cy, sizeof(int), &nRead);
	pStream->Read(&m_ptOffset.x, sizeof(int), &nRead);
	pStream->Read(&m_ptOffset.y, sizeof(int), &nRead);

	if(m_cy > 0)
	{
		m_ppRows = new byte*[m_cy];

		long nRowSize = ((m_cx+7)/8)+4 + sizeof(DWORD)*2;
		
		for(long y = 0; y < m_cy; y++)
		{
			m_ppRows[y] = new byte[nRowSize];
			pStream->Read(m_ppRows[y], nRowSize, &nRead);
		}
	}

	
	return CObjectBase::Load(pStream);
}

HRESULT CBinaryBitImage::Store( IStream *pStream, SerializeParams *pparams )
{
	if(!pStream)
		return E_INVALIDARG;
	
	ULONG nWritten = 0;
	pStream->Write(&m_cx, sizeof(int), &nWritten);
	pStream->Write(&m_cy, sizeof(int), &nWritten);
	pStream->Write(&m_ptOffset.x, sizeof(int), &nWritten);
	pStream->Write(&m_ptOffset.y, sizeof(int), &nWritten);

	long nRowSize = ((m_cx+7)/8)+4 + sizeof(DWORD)*2;
	
	for(long y = 0; y < m_cy; y++)
	{
		pStream->Write(m_ppRows[y], nRowSize, &nWritten);
	}
	
	return CObjectBase::Store(pStream);
}

HRESULT CBinaryBitImage::IsBaseObject(BOOL * pbFlag)
{
	* pbFlag = false;
	return S_OK;
}
*/