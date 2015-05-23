// LookupImpl.cpp : implementation file
//

#include "stdafx.h"
#include "LookupImpl.h"
#include "math.h"
#include "timetest.h"


/////////////////////////////////////////////////////////////////////////////
// CLookupImpl

CLookupImpl::CLookupImpl()
{
	m_pLookupTableB = 0;
	m_pLookupTableG = 0;
	m_pLookupTableR = 0;
}

CLookupImpl::~CLookupImpl()
{
	DeleteLookup();
}

IMPLEMENT_UNKNOWN_BASE(CLookupImpl, CnvLookup)

HRESULT CLookupImpl::XCnvLookup::SetLookupTable(byte* pTable, int cx, int cy)
{
	METHOD_PROLOGUE_BASE(CLookupImpl, CnvLookup)
	pThis->SetLookupTable(pTable, cx, cy);
	return S_OK;
}

HRESULT CLookupImpl::XCnvLookup::KillLookup()
{
	METHOD_PROLOGUE_BASE(CLookupImpl, CnvLookup)
	pThis->DeleteLookup();
	return S_OK;
}

void CLookupImpl::SetLookupTable(byte* pTable, int cx, int cy)
{
	DeleteLookup();

	double	fGamma = 1-(::GetValueDouble( ::GetAppUnknown(), "\\DrawImage", "Gamma", 1.0 )-1)/10;
	int		nBitNumber = ::GetValueInt( ::GetAppUnknown(), "\\DrawImage", "HiBitNum", 16 )-8;


	if( !pTable && fGamma == 1.0 && nBitNumber == 8 )
		return;

	m_pLookupTableR = new byte[65536];
	m_pLookupTableG = new byte[65536];
	m_pLookupTableB = new byte[65536];

	double fCnvMul0;
	//fGamma0 = pow( 10.,  fGamma );
	fCnvMul0 = 65535/pow( 65535., fGamma );

	int	nColor, nOldColor = -1;;
	double	fcolor = 0;
	byte	byteColor = 0;

	for( int c = 0; c <= 65535; c++ )
	{
		nColor = (c>>nBitNumber);
		if( nColor !=  nOldColor )
		{
			fcolor = max( 0, min(255, fCnvMul0 * pow((double)nColor, fGamma)));
			byteColor = (byte)fcolor;
			nOldColor = nColor;
		}
		
		if( pTable )
		{
			m_pLookupTableR[c] = pTable[byteColor];
			m_pLookupTableG[c] = pTable[byteColor+256];
			m_pLookupTableB[c] = pTable[byteColor+512];
		}
		else
		{
			m_pLookupTableR[c] = byteColor;
			m_pLookupTableG[c] = byteColor;
			m_pLookupTableB[c] = byteColor;
		}
	}
}


void CLookupImpl::DeleteLookup()
{
	if(m_pLookupTableR)delete m_pLookupTableR;m_pLookupTableR = 0;
	if(m_pLookupTableG)delete m_pLookupTableG;m_pLookupTableG = 0;
	if(m_pLookupTableB)delete m_pLookupTableB;m_pLookupTableB = 0;
}

