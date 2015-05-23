#include "stdafx.h"
#include "binimageint.h"
#include "morph1.h"
#include "nameconsts.h"

/////////////////////////////////////////////////////////////////////////////////////////
//morphology filter base class (work with MorpImage
CMorphFilterBase::CMorphFilterBase()
{
	m_psrc = m_pnew = 0;
	m_ppcurrent = 0;

	m_nBodyArea = 0;
	m_nBackArea = 0;
	m_nTotalArea = 0;

	m_nCurrentX = 0;

	m_nMaskCenter = 0;	//index of current row in list;
	m_nMaskSize = 0;
	m_cx = 0; 
	m_cy = 0;
}

CMorphFilterBase::~CMorphFilterBase()
{
  if(m_ppcurrent)
    delete m_ppcurrent;
  m_ppcurrent = 0;
}

bool CMorphFilterBase::Initialize( int nMaskSize )
{
	m_psrc = GetDataArgument();
	m_pnew = GetDataResult();

	if(m_pnew == 0 || m_psrc == 0)
	{
		//_trace( "Fault to create source or destination wrapper" );
		return false;
	}
	
	m_psrc->GetSizes( &m_cx, &m_cy );

	if( m_pnew->CreateNew( m_cx, m_cy ) != S_OK )
	{
		//_trace( "Fault to initialize dest wrapper" );
		return false;
	}
	// A.M. fix, BT4415.
	POINT ptOffset;
	m_psrc->GetOffset(&ptOffset);
	m_pnew->SetOffset(ptOffset, FALSE);

  if(m_ppcurrent)
    delete m_ppcurrent;
	m_ppcurrent = new byte*[nMaskSize];
	if( !m_ppcurrent )return false;

	m_nMaskSize = nMaskSize;
	m_nTotalArea = m_nMaskSize*m_nMaskSize;
	m_nMaskCenter = nMaskSize/2;
	m_nBodyArea = 0;
	m_nBackArea = 0;
	m_nCurrentX = 0;

	return true;
}


void CMorphFilterBase::InitCurrentRow( int nRow )
{
	int		i, j;
	byte	*pbin;
	int nLeftArea = m_nMaskCenter;

	m_nBodyArea = 0;
	m_nBackArea = 0;
	m_nCurrentX = 0;

	for( j = 0; j < m_nMaskSize; j++ )
	{
		//get row from image
		m_psrc->GetRow( &pbin, max( 0, min( m_cy, nRow+j-m_nMaskCenter ) ), 0 );
		m_ppcurrent[j] = pbin;

		if( *pbin )m_nBodyArea+=m_nMaskCenter-1;
		else m_nBackArea+=m_nMaskCenter-1;

		for( i = m_nMaskCenter; i <= m_nMaskSize; i++, pbin++ )
		{
			if( *pbin )m_nBodyArea++;
			else m_nBackArea++;
		}
	}

	dbg_assert( m_nBodyArea+m_nBackArea == m_nTotalArea );

}
void CMorphFilterBase::MoveCurrentRowRight()
{
	register int	left = max( 0, min( m_cx-1, m_nCurrentX-m_nMaskSize+m_nMaskCenter ) );
	register int	right = max( 0, min( m_cx-1, m_nCurrentX+m_nMaskCenter+1 ) );
	m_nCurrentX++;
	for( int i = 0; i < m_nMaskSize; i++ )
	{
		if( m_ppcurrent[i][left] )m_nBodyArea--;
		else m_nBackArea--;
		if( m_ppcurrent[i][right] )m_nBodyArea++;
		else m_nBackArea++;
	}
}


/////////////////////////////////////////////////////////////////////////////////////////
//some morphology filters
_ainfo_base::arg	CMorphSmoothInfo::s_pargs[] = 
{
	{"MaskSize",szArgumentTypeInt, "9", true, false },
	{"Level",szArgumentTypeInt, "3", true, false },
	{"BinImage",szArgumentTypeBinaryImage, 0, true, true },
	{"BinSmooth",szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};


bool CMorphSmooth::InvokeFilter()
{
	Initialize( GetArgLong( "MaskSize" ) );
	int	nLevel = GetArgLong( "Level" );

	try
	{
		StartNotification( m_cy );

		int	x, y;
		byte	*pdst;

		for( y = 0; y < m_cy; y++ )
		{
			InitCurrentRow( y );
			m_pnew->GetRow( &pdst, y, 0 );

			for( x = 0; x < m_cx; x++, pdst++ )
			{
				*pdst = m_ppcurrent[m_nMaskCenter][x];
				if( *pdst && m_nBodyArea < nLevel )*pdst = 0;
				if( !*pdst && m_nBackArea < nLevel )*pdst = 0xFF;
				MoveCurrentRowRight();
			}
			Notify( y );
		}

		FinishNotification();
		
		return true;
	}
	catch( ... )
	{
		return false;
	}
}