// RelaxationEdge.cpp: implementation of the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Relaxation.h"
#include "misc_utils.h"
#include <math.h>
#include "core5.h"

#include "nameconsts.h"

#define MaxColor color(-1)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//
//		RelaxationEdge
//
//
/////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg	CRelaxationEdgeInfo::s_pargs[] = 
{
	{"Param",		szArgumentTypeInt, "100", true, false },
	{"Img",		szArgumentTypeImage, 0, true, true },
	{"Relaxation",szArgumentTypeBinaryImage, 0, false, true },	
	{0, 0, 0, false, false },
};


bool CRelaxationEdgeFilter::CanDeleteArgument( CFilterArgument *pa )
{
	return false;
}

/////////////////////////////////////////////////////////////////////////////

bool CRelaxationEdgeFilter::InvokeFilter()
{
	IImage3Ptr	ptrSrcImage( GetDataArgument() );
	if( !TestImage( ptrSrcImage ) )
		return false;

	IBinaryImagePtr	ptrBin = GetDataResult();
			
	if( ptrSrcImage == NULL || ptrBin==NULL )
		return false;

	ptrSrcImage->GetSize((int*)&size.cx,(int*)&size.cy);
	ptrBin->CreateNew(size.cx,size.cy);
	POINT point;
	ptrSrcImage->GetOffset(&point);
	ptrBin->SetOffset(point,true);

	_ptr_t2<long> smartHist(MaxColor+1);
	long *pHist = smartHist.ptr;
	_ptr_t2<long> smartAprHist(256);
	long *pAprHist = smartAprHist.ptr;
	CreateHistogram(ptrSrcImage,0,pHist,pAprHist);
	m_Edge=SelectEdge(ptrSrcImage,pHist,pAprHist);

	byte *pmask=0;
	_ptr_t2<double> smartP(size.cx*size.cy);
	pP = smartP.ptr;
	_ptr_t2<double> smartQ(size.cx*size.cy);
	pQ = smartQ.ptr;
	double *curP=0,*curQ=0;

	long nParam = GetArgLong( "Param" );
	double NotifyCount = 0;

	try
	{
		StartNotification( 2*size.cy, 1 ,1);
//  ===============  Naxodim P i Q =================================
		long x,y;
		for( y=0;y<size.cy;y++ )
		{
			ptrSrcImage->GetRowMask( y ,&pmask );
			color* pSrc;
			ptrSrcImage->GetRow(y , GetWorkPane( ) , &pSrc);
			curP = pP + y*size.cx;
			curQ = pQ + y*size.cx;

			for( x = 0; x < size.cx; x++, pmask++, pSrc++, curP++, curQ++)
			{
				if( *pmask==255  )
				{
					if(*pSrc>=m_Edge)
					{
						*curP = 0.5 + 0.5*(*pSrc-m_Edge)/(MaxColor - m_Edge);
						*curQ = 1 - *curP;
					}
					else
					{
						*curQ = 0.5 + 0.5*(m_Edge-*pSrc)/(m_Edge - 0);
						*curP = 1 - *curQ;
					}
					
				}
			}			

		}

		double Sum;
		long iter = 1;
		double inc=1;

		do
		{
			Sum = 0;
			for( y=0;y<size.cy;y++ )
			{ 
				ptrSrcImage->GetRowMask( y ,&pmask );
				curP = pP + y*size.cx;
				curQ = pQ + y*size.cx;

				for( x = 0; x < size.cx; x++, pmask++, curP++, curQ++)
				{
					if( *pmask==255  )
					{
						double qb = 1 + Q(x,y,0);
						double qw = 1 + Q(x,y,1);
						double pk = *curP*qw + *curQ*qb;
						double temp1 = *curP;
						double temp2 = *curQ;
						if(pk!=0)
						{
							*curP = *curP*qw/pk;
							*curQ = *curQ*qb/pk;
							Sum+=fabs(*curP-temp1) + fabs(*curQ-temp2);
						}
					}
				}
				Notify( (int)NotifyCount );
				NotifyCount+=inc;
			}
			iter++;
			inc = inc/2;
		}
		while(Sum>nParam);

		
		for( y=0;y<size.cy;y++ )
		{
			ptrSrcImage->GetRowMask( y ,&pmask );
			byte* pDst;
			ptrBin->GetRow(&pDst,y,0);
			curP = pP + y*size.cx;
			curQ = pQ + y*size.cx;
			
			for( long x = 0; x < size.cx; x++, pmask++, pDst++ ,curP++,curQ++)
			{
				if( *pmask==255  )
				{
					if(*curP>*curQ)
						*pDst=0xFF;
					else
						*pDst = 0;
				}
				else
					*pDst = 0;
			}			

			Notify( (int)NotifyCount );
			NotifyCount+=inc;
		}

		FinishNotification();
		return true;
	}
	catch( ... )
	{
		return false;
	}
}

color CRelaxationEdgeFilter::SelectEdge(IImage3* pSource, long* pHist, long* pAprHist)
{
	long j = 0,i=0;
	_ptr_t2<long> smart64Hist(64);
	long *p64Hist = smart64Hist.ptr;
	ZeroMemory(p64Hist,sizeof(long)*64);

	for(j=0; j<256; j++)
		p64Hist[(int)(j/4)]+=pAprHist[j];
/* Find the first peak */
	j=0;
	for (i=0; i<64; i++)
	  if (p64Hist[i] > p64Hist[j]) j = i;

/* Find the second peak */
	long k = 0;
	for (i=1; i<64; i++)
	  if (p64Hist[i-1]<=p64Hist[i] && p64Hist[i+1]<=p64Hist[i])
	    if ((k-j)*(k-j)*p64Hist[k] < (i-j)*(i-j)*p64Hist[i])
			k = i;

	color T = (color) j;
	if (j<k)
	{
	  for (i=j; i<k; i++)
		  if (p64Hist[i] < p64Hist[T])
			  T = (color)i;
	}
	else
	{
	  for (i=k; i<j; i++)
	    if (p64Hist[i] < p64Hist[T])
			T = (color)i;
	}

	return (T<<10);
}

double CRelaxationEdgeFilter::C(double c1, double c2)
{
	if(c1==c2)
		return 0.9;
	return -0.9;
}

double CRelaxationEdgeFilter::Q(long x, long y, double c)
{
	double Q=0;
	int i;
	double *curP=0,*curQ=0;

	if(y-1>=0)
	{
		curP = pP + (y-1)*size.cx + (x-1);
		curQ = pQ + (y-1)*size.cx + (x-1);
		int from = max(0,x-1);
		int to = min(size.cx-1,x+1);
		for(i=from; i<=to; i++,curP++,curQ++)
			Q+=C(c,1)*(*curP)+C(c,0)*(*curQ);
	}

	if(y+1<size.cy)
	{
		curP = pP + (y+1)*size.cx + (x-1);
		curQ = pQ + (y+1)*size.cx + (x-1);
		int from = max(0,x-1);
		int to = min(size.cx-1,x+1);
		for(i=from; i<=to; i++,curP++,curQ++)
			Q+=C(c,1)*(*curP)+C(c,0)*(*curQ);
	}

	if(x-1>0)
	{
		curP = pP + y*size.cx + (x-1);
		curQ = pQ + y*size.cx + (x-1);
		Q+=C(c,1)*(*curP)+C(c,0)*(*curQ);
	}


	if(x+1<size.cx)
	{
		curP = pP + y*size.cx + (x+1);
		curQ = pQ + y*size.cx + (x+1);
		Q+=C(c,1)*(*curP)+C(c,0)*(*curQ);
	}

	return Q/8;
}

