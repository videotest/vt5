#include "stdafx.h"
#include "morphobase.h"


CDCProvider::CDCProvider(long cx, long cy)
{
	m_cx = cx;
	m_cy = cy;

	HDC hdcScreen = ::GetDC(0);
	m_dc = CreateCompatibleDC(hdcScreen);
	ReleaseDC(0, hdcScreen);
		
	DWORD	dwMemSize = sizeof(BITMAPINFOHEADER)+1024;
	byte	*pbyteInfo = new byte[dwMemSize];
	BITMAPINFOHEADER	*pbih = (BITMAPINFOHEADER	*)pbyteInfo;

	ZeroMemory( pbih, sizeof( BITMAPINFOHEADER ) );
	pbih->biSize = sizeof( BITMAPINFOHEADER );
	pbih->biWidth = cx;
	pbih->biHeight = cy;
	pbih->biPlanes = 1;
	pbih->biBitCount = 8;

	BITMAPINFO	*pbi = (BITMAPINFO	*)pbih;

	for( int i = 0; i < 256; i++ )
	{
		pbi->bmiColors[i].rgbBlue = i;
		pbi->bmiColors[i].rgbGreen = i;
		pbi->bmiColors[i].rgbRed = i;
		pbi->bmiColors[i].rgbReserved = 0;
	}


	m_hBmp = CreateDIBSection(m_dc, pbi, DIB_RGB_COLORS, (void**)&m_pbits, NULL, 0);

	m_cx4 = (cx+3)/4*4;

	ZeroMemory(m_pbits, m_cx4*cy);

	m_hBmpOld = (HBITMAP)SelectObject(m_dc, m_hBmp);
	SelectObject(m_dc, GetStockObject(WHITE_BRUSH)); 
	SelectObject(m_dc, GetStockObject(WHITE_PEN)); 
		
	delete pbyteInfo;
}

CDCProvider::~CDCProvider()
{
	if(m_hBmp)
	{
		SelectObject(m_dc, m_hBmpOld);
		DeleteObject(m_hBmp);
		m_hBmp = 0;
	}

	if(m_dc)
	{
		DeleteDC(m_dc);
		m_dc = 0;
	}
}

CMorphoFilter::CMorphoFilter()
{
	m_cx = 0;
	m_cy = 0;
	m_nMaskSize = 0;
	m_pnMask = 0;
	m_pnWorkingMask = 0;
	m_pbinSrc = 0;
	m_pbinDst = 0;
	m_pbinIntr = 0;
}

CMorphoFilter::~CMorphoFilter()
{
	_DeInitMorpho(false);
}


bool CMorphoFilter::_InitMorpho()
{
	m_pbinDst = new CBinaryBitImage();
	m_pbinSrc = new CBinaryBitImage();

	if(m_pbinDst == 0 || m_pbinSrc == 0)
		return false;

	IBinaryImagePtr	ptrBin = GetDataArgument();
	if( m_pbinSrc->InitFromBin( ptrBin ) != S_OK )
		return false;
	
	m_pbinSrc->GetSizes( &m_cx, &m_cy );

	if( m_pbinDst->CreateNew( m_cx, m_cy ) != S_OK )
		return false;
	ptrBin = 0;

	_CreateMask();

	return true;
}

bool CMorphoFilter::_InitMorpho2()
{
	if (!_InitMorpho())
		return false;
	m_pbinIntr = new CBinaryBitImage();
	if (m_pbinIntr == NULL)
		return false;
	if( m_pbinIntr->CreateNew( m_cx, m_cy ) != S_OK )
		return false;
	return true;
}


void CMorphoFilter::_CreateMask()
{
	m_nMaskSize = GetArgLong( "MaskSize" );
	
	m_nMaskSize = max(3, min(g_nMaxMaskSize-1, m_nMaskSize));

	CDCProvider dcp(m_nMaskSize, m_nMaskSize);
	HDC dc = dcp.GetDC();
	BOOL b = Ellipse(dc, 0, 0, m_nMaskSize, m_nMaskSize);

	long nRowSize = 0;
	byte* pBits = dcp.GetBits(nRowSize);


	m_pnMask = new DDWORD[g_nMaxMaskSize];
	m_pnWorkingMask = new DDWORD[g_nMaxMaskSize];
	ZeroMemory(m_pnMask, sizeof(DDWORD)*g_nMaxMaskSize);

	byte* pRow = 0;
	int y, x;
	for(y = 0; y < m_nMaskSize; y++)
	{
		pRow = pBits + y*nRowSize;
		for(x = 0; x < m_nMaskSize; x++)
		{
			if(pRow[x])
			{
				m_pnMask[y] |= 0x8000000000000000>>x;
			}
		}
	}

}

void CMorphoFilter::_DeInitMorpho(bool bApply)
{
	if(bApply)
	{
		IBinaryImagePtr	ptrBin = GetDataResult();
		m_pbinDst->StoreToBin( ptrBin );
		// A.M. fix, BT4415.
		IBinaryImagePtr	ptrBinSrc = GetDataArgument();
		POINT ptOffset;
		ptrBinSrc->GetOffset(&ptOffset);
		ptrBin->SetOffset(ptOffset, FALSE);

		FinishNotification();
	}

	delete m_pnWorkingMask;
	m_pnWorkingMask = 0;

	delete m_pnMask;
	m_pnMask = 0;

	if(m_pbinDst) 
		m_pbinDst->Release();
	m_pbinDst = 0;

	if(m_pbinSrc)
		m_pbinSrc->Release();
	m_pbinSrc = 0;

	if(m_pbinIntr)
		m_pbinIntr->Release();
	m_pbinIntr=0;
}

void CMorphoFilter::_DoMorphology(bool bErode, CBinaryBitImage *pbinDst, CBinaryBitImage *pbinSrc)
{
	if (pbinDst == 0)
		pbinDst = m_pbinDst;
	if (pbinSrc == 0)
		pbinSrc = m_pbinSrc;

	//if(bErode) pbinSrc->FillBorders();
	//else pbinSrc->ClearBorders();
	pbinSrc->ExpandBorders();

	long nMagick = 0;
	switch(m_nMaskSize/8)
	{
	case 1:
	case 2:
		nMagick = 8;
		break;
	case 3:
		nMagick = 16;
		break;
	};
	if(m_nMaskSize%8 == 0 && m_nMaskSize != 16)
		nMagick -= 8;
	

	byte *pRowSrc = 0, *pRowDst = 0;
	for(long y = 0; y < m_cy; y++)
	{
		bool bDirForward = true;

		memcpy(m_pnWorkingMask, m_pnMask, sizeof(DDWORD)*g_nMaxMaskSize);
		
		pbinDst->GetRow(&pRowDst, y, TRUE);

		long nExtraOffset = 0;
		long offsetX = 0;
		
		offsetX = (g_nMaxMaskSize-m_nMaskSize/2)/8;
		nExtraOffset = (g_nMaxMaskSize-m_nMaskSize/2)%8;

		for(long i = 0; i < g_nMaxMaskSize; i++)
		{
			m_pnWorkingMask[i] = m_pnWorkingMask[i] >> nExtraOffset;
		}

		
		long X = -offsetX*8;
		
		bool bFirstTime = true;

		while(X < m_cx)
		{
			DDWORD ddwFilterRes = 0;
			DDWORD ddwSource = 0;

			if(bErode)
			{
				for(long k = 0; k < 64 - (nExtraOffset+m_nMaskSize/2+nMagick); k++)
				{
					DDWORD ss = 0x0000000000000001;
					ss = ss<<(k+m_nMaskSize/2);
					ddwFilterRes |= ss;
				}
			}

			for(long n = -m_nMaskSize/2; n < m_nMaskSize/2 + (m_nMaskSize&0x01); n++)
			{				
				pbinSrc->GetRow(&pRowSrc, y+n, TRUE);

				ddwSource = 0;
				for(long i = 7; i >= 0; i--)
				{
					ddwSource |= ((DDWORD)*(pRowSrc+offsetX-i+7))<<i*8;
				}
				
								
				if(bDirForward)
				{
					for(long k = 0; k < 64 - (bFirstTime ? (nExtraOffset+m_nMaskSize-1) : (nExtraOffset+m_nMaskSize/2+nMagick)); k++)
					{
						if((bErode?(~ddwSource):ddwSource)&m_pnWorkingMask[n+m_nMaskSize/2])	
						{
							DDWORD qq = 0x8000000000000000;
							qq = qq >> (k+nExtraOffset+(bFirstTime?m_nMaskSize/2: nMagick));
							if(bErode)
							{
								qq = ~qq;
								ddwFilterRes &= qq;
							}
							else
								ddwFilterRes |= qq;
						}
						if(k != 64 - (bFirstTime ? (nExtraOffset+m_nMaskSize-1) : (nExtraOffset+m_nMaskSize/2+nMagick)) -1)
							m_pnWorkingMask[n+m_nMaskSize/2] = m_pnWorkingMask[n+m_nMaskSize/2] >> 1;
					}
				}
				else
				{
					for(long k = 0; k < 64 - (nExtraOffset+m_nMaskSize/2+nMagick); k++)
					{
						if((bErode?(~ddwSource):ddwSource)&m_pnWorkingMask[n+m_nMaskSize/2])	
						{
							DDWORD ss = 0x0000000000000001;
							ss = ss<<(k+m_nMaskSize/2);
							if(bErode)
							{
								ss = ~ss;
								ddwFilterRes &= ss;
							}
							else
								ddwFilterRes |= ss;
							
						}
						if(k != 64 - (nExtraOffset+m_nMaskSize/2+nMagick) -1)
							m_pnWorkingMask[n+m_nMaskSize/2] = m_pnWorkingMask[n+m_nMaskSize/2] << 1;
					}
				}

			}
		
			for(long i = 7; i >= 0; i--)
			{
				*(pRowDst+offsetX-i+7) |= (byte)(ddwFilterRes>>i*8);
			}
			
			bDirForward = !bDirForward;

			bFirstTime = false;

			offsetX += (64-m_nMaskSize)/8;
			
			X += (64-m_nMaskSize)/8*8;
//			X += (8 - m_nMaskSize/8 - 1)*8;
			
		}

		Notify(y);
	
	}
}


	