#include "stdafx.h"
#include "watermarkfilter.h"

#define MaxColor color(-1)

_ainfo_base::arg        CWatermarkFilterInfo::s_pargs[] = 
{       
	{"Mark",      szArgumentTypeInt, "1", true, false },
	{"ImgSrc",       szArgumentTypeImage, 0, true, true },
	{"ImgMark",      szArgumentTypeImage, 0, true, true },
	{"ImgResult",    szArgumentTypeImage, 0, false, true },	
	{0, 0, 0, false, false },
};

class CRnd
{
public:
	CRnd(DWORD dwSeed=0) {m_dwSeed=dwSeed;}
	void Randomize(DWORD dwSeed) {m_dwSeed=dwSeed;}
	DWORD Next(DWORD dwRange)
	{
		m_dwSeed = 1664525L * m_dwSeed + 1013904223L;
		return ( (m_dwSeed>>16) | (m_dwSeed<<16) )  % dwRange;
	}
	DWORD m_dwSeed;
};

CWatermarkFilter::CWatermarkFilter(void)
{
}

CWatermarkFilter::~CWatermarkFilter(void)
{
}

bool CWatermarkFilter::InvokeFilter()
{
	IImage3Ptr	ptrSrcImage( GetDataArgument( "ImgSrc" ) );
	IImage3Ptr	ptrMarkImage( GetDataArgument( "ImgMark" ) );

	IImage3Ptr	ptrDstImage( GetDataResult() );
	
	if( ptrSrcImage == 0 || ptrDstImage == 0 || ptrMarkImage == 0 )
		return false;

	ICompatibleObjectPtr ptrCO = ptrDstImage;
	
	if( ptrCO == 0 ) 
		return false;
	
	if( ptrCO->CreateCompatibleObject( ptrSrcImage, 0, 0 ) != S_OK )
		return false;

	int nPaneCount = ::GetImagePaneCount( ptrSrcImage );
 	int nPaneCount2 = ::GetImagePaneCount( ptrMarkImage );

	if( nPaneCount != nPaneCount2 )
		return false;

	int cxSrc = 0, cySrc = 0;
	ptrSrcImage->GetSize( &cxSrc, &cySrc );

	int cxMark = 0, cyMark = 0;
	ptrMarkImage->GetSize( &cxMark, &cyMark );

	if( cxMark != 64 || cyMark != 64 )
		return false;

	int nMarkCountX = cxSrc / cxMark;  
	int nMarkCountY = cySrc / cyMark;  

	StartNotification( nMarkCountY * cyMark, nPaneCount );

	CRnd rnd;
	long cPrev=0;

	bool bCypher = true; // маркировать или снимать марку?
	bCypher = GetArgLong("Mark") != 0;

	double fMarkSum = 0.0; // сумма, показывающая уровень сходства с маркой
	long nMarkSum = 0; // счетчик

	for( int nPane = 0; nPane < nPaneCount; nPane++ )
	{
		for( int z = 0; z < nMarkCountY; z++ )
		{
			for( int y = 0; y < cyMark; y++ )
			{
				if( z * cyMark + y > cySrc )
					break;

				color *pColorMark = 0;
				ptrMarkImage->GetRow( y, nPane, &pColorMark );

				color *pColorDst = 0;
				ptrDstImage->GetRow( z * cyMark + y, nPane, &pColorDst );

				for( int k = 0; k < nMarkCountX; k++ )
				{
					for( int x = 0; x < cxMark; x++ )
					{
						if( k * cxMark + x > cxSrc )
							break; 

						long lMark = 0;

      					if( pColorMark[x] & 0x8000 )
							lMark = 1;
						else
							lMark = 0;
						lMark ^= rnd.Next(2);
						lMark ^= cPrev >> 12;
						lMark = (lMark&1) ? 1 : -1;

 						long c = pColorDst[k * cxMark + x];

						fMarkSum += c*lMark;
						nMarkSum ++;

						if(bCypher)
						{
							cPrev = c; // cPrev - берем с незамаркированного
							c += lMark*256*1;
						}
						else
						{
							c -= lMark*256*1;
							cPrev = c; // cPrev - берем с незамаркированного
						}

						pColorDst[k * cxMark + x] = min(65535,max(0,c));
					}
				}
				Notify( z * cyMark + y );
			}
		}
		NextNotificationStage();
	}
	FinishNotification();
	fMarkSum /= nMarkSum;

	return true;
}

color CWatermarkFilter::_convert_color( color clValue, int x, int y )
{
   	BYTE hBt = HIBYTE( clValue );
	BYTE lBt = LOBYTE( clValue );

	short shParts[12] = { hBt >> 4,
						hBt & 0xF,
						lBt >> 4,
						lBt & 0xF } ;

	shParts[4] = shParts[0];
	shParts[5] = shParts[1];
	shParts[6] = shParts[2];
	shParts[7] = shParts[3];

	shParts[8] = shParts[0];
	shParts[9] = shParts[1];
	shParts[10] = shParts[2];
	shParts[11] = shParts[3];

	int nOf = ( x & y ) % 8;

	color clRet = 0;

	long lMask = 0xF00;
	for( int i = 0; i < 4; i++ )
		clRet += ( shParts[i + nOf] << ( 4 - i + 1 ) ) & ( lMask >> ( i * 4 ) );

	return clRet;
}

color CWatermarkFilter::_inverse_convert_color( color clValue, int x, int y )
{
	BYTE hBt = HIBYTE( clValue );
	BYTE lBt = LOBYTE( clValue );

	short shParts[12] = { hBt >> 4,
						hBt & 0xF,
						lBt >> 4,
						lBt & 0xF } ;

	shParts[4] = shParts[0];
	shParts[5] = shParts[1];
	shParts[6] = shParts[2];
	shParts[7] = shParts[3];

	shParts[8] = shParts[0];
	shParts[9] = shParts[1];
	shParts[10] = shParts[2];
	shParts[11] = shParts[3];

	int nOf = ( x & y ) % 8;
	nOf += ( nOf / 4 + 1 ) * 4 - nOf;

	color clRet = 0;

	long lMask = 0xF00;
	for( int i = 0; i < 4; i++ )
		clRet += ( shParts[i + nOf] << ( 4 - i + 1 ) ) & ( lMask >> ( i * 4 ) );

	return clRet;
}
