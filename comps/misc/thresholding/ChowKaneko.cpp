// ChowKaneko.cpp: implementation of the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ChowKaneko.h"
#include "misc_utils.h"
#include <math.h>
#include "core5.h"

#include "nameconsts.h"
#include "ImagePane.h"
#include "binimageint.h"

#define MaxColor color(-1)
#define EDGETYPE_PEAKS		0
#define EDGETYPE_ENTROPY	1
#define EDGETYPE_MINBINARY	2
#define EDGETYPE_ITERATIVE  3
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//
//		ChowKaneko
//
//
/////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg	CChowKanekoInfo::s_pargs[] = 
{
	{"Param",	szArgumentTypeInt, "64", true, false },
	{"Type",	szArgumentTypeInt, "0", true, false },
	{"Img",		szArgumentTypeImage, 0, true, true },
	{"ChowKaneko",szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};

bool CChowKanekoFilter::InvokeFilter()
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

	m_seg_size = GetArgLong("Param");
	m_EdgeType = GetArgLong("Type");
	m_seg_offset = m_seg_size/2;

	_ptr_t2<long> smartHist(256);
	pHist = smartHist.ptr;
	sizeEdges.cx = (int)(size.cx/m_seg_size+1+1);
	sizeEdges.cy = (int)(size.cy/m_seg_size+1+1);
	_ptr_t2<long> smartEdges(sizeEdges.cx*sizeEdges.cy);
	pEdges = smartEdges.ptr;
	
	byte *pmask =0;
	long x,i;

	try
	{
		StartNotification( sizeEdges.cy*sizeEdges.cx, 1 );
		long cur_idx=0;
		for( long y = 0; y < sizeEdges.cy; y++ )
		{
			for( x = 0; x < sizeEdges.cx; x++ )
			{
				color edge = 0;
				InitHist(ptrSrcImage,x,y);
				long v = SelectEdge();
				long N1=0,N2=0,lsum=0;
				double sigma1=0,sigma2=0,P1=0,P2=0,dsum=0,M1=0,M2=0;

				for(i=0; i<=v; i++)
				{
					N1+=pHist[i];
					M1+=i*pHist[i];
				}
				M1 = N1 ? M1/N1 : 0;

				for(i=v+1; i<256; i++)
				{
					N2+=pHist[i];
					M2+=i*pHist[i];
				}
				M2 = N2 ? M2/N2 : 0;

				for(i=0,lsum=0; i<=v; i++)
				{
					lsum+=pHist[i]*(i-(long)M1)*(i-(long)M1);
				}
				sigma1 = N1 ? sqrt(double(lsum)/N1) : 0.;

				for(i=v+1,lsum=0; i<256; i++)
				{
					lsum+=pHist[i]*(i-(long)M2)*(i-(long)M2);
				}
				sigma2 = N2 ? sqrt(double(lsum)/N2) : 0.;

				if(sigma1)
				{
					for(i=0,dsum=0; i<=v; i++)
					{
						dsum+=1/exp( ((i-M1)*(i-M1))/(2*sigma1*sigma1) );
					}
					P1 = sigma1*N1/dsum;
				}
				else
					P1 = 0;

				if(sigma2)
				{
					for(i=v,dsum=0; i<256; i++)
					{
						dsum+=1/exp( ((i-M2)*(i-M2))/(2*sigma2*sigma2) );
					}
					P2 = sigma2*N2/dsum;
				}
				else
					P2 = 0;

				bool criteria1=false,criteria2=false,criteria3=false;

				if(M2-M1>4)
					criteria1=true;

				if(sigma2)
				{
					if(sigma1/sigma2<2 && sigma1/sigma2>0.05)
						criteria2=true;
				}
				
				long minH=pHist[(long)M1];
				for(i=(long)M1; i<(long)M2; i++)
				{
					if(pHist[i] && pHist[i]<minH)
					{
						minH=pHist[i];
					}
				}

				if(pHist[(long)M1] && pHist[(long)M2])
					if(minH/min(pHist[(long)M1],pHist[(long)M2])<0.8)
						criteria3=true;

				if(criteria1 && criteria2 && criteria3)
				{
					double a=1/(sigma2*sigma2)+1/(sigma1*sigma1);
					//double a = 2/(sigma2*sigma2);
					double b=2*(M2/(sigma2*sigma2)-M1/(sigma1*sigma1));
					double c=2*log10((P2*sigma1)/(P1*sigma2));
					double D = b*b-4*a*c;
			/*		if(D>=0)
					{
						double t1 = fabs((-b + sqrt(D))/(2*a));
						double t2 = fabs((-b - sqrt(D))/(2*a));
						pEdges[cur_idx] = max(t1,t2);
					}
					else*/
						
					pEdges[cur_idx]=v;
				}
				else
				{
					//pEdges[cur_idx]=pEdges[cur_idx-1];
					pEdges[cur_idx]=-1;
				}
			//	pEdges[cur_idx]=(color)v;
				Notify( cur_idx );
				cur_idx++;
			}
		}

		cur_idx=0;
		for( y = 0; y < sizeEdges.cy; y++ )
		{
			for( x = 0; x < sizeEdges.cx; x++ )
			{
				if(pEdges[cur_idx]==-1)
				{
					long loc_ofset=0;
					bool stop=false;
					while(pEdges[cur_idx]==-1)
					{
						long y_to,y_from,x_to,x_from;
						loc_ofset++;
						y_from = max(0,y-loc_ofset);
						y_to = min(sizeEdges.cy-1,y+loc_ofset);
						x_from = max(0,x-loc_ofset);
						x_to = min(sizeEdges.cx-1,x+loc_ofset);
						long nSum=0,nCount=0;

						for( long Y=y_from; Y<=y_to && !stop; Y++ )
						{ 
																	
							for( long X = x_from; X <= x_to && !stop; X++)
							{
								int edge = pEdges[Y*sizeEdges.cx+X];
								if( edge != -1  )
								{
									nSum+=edge;
									nCount++;
									//pEdges[cur_idx]  = edge;
									//stop = true;
								}
							}
						}
						pEdges[cur_idx] = nCount ? (int)(nSum/nCount) : -1;
						if(loc_ofset>sizeEdges.cx && loc_ofset>sizeEdges.cy)
						{
							InitHist(ptrSrcImage,x,y);
							pEdges[cur_idx] = SelectEdge();
						}
					}
 				}
				cur_idx++;
			}
		}

		for( y=0;y<size.cy;y++ )
		{
			ptrSrcImage->GetRowMask( y ,&pmask );
			color* pSrc;
			ptrSrcImage->GetRow(y , GetWorkPane( ) , &pSrc);
			byte* pDst;

			ptrBin->GetRow(&pDst,y,0);

			for( long x = 0; x < size.cx; x++, pmask++, pSrc++, pDst++ )
			{
				if( *pmask==255  )
				{
					POINT p1,p2,p3,p4;
					p1.x = x/m_seg_size;
					p1.y = y/m_seg_size;
					p2.x = min(sizeEdges.cx-1,p1.x+1);
					p2.y = p1.y;
					p3.x = p1.x;
					p3.y = min(sizeEdges.cy-1,p1.y+1);
					p4.x = p2.x;
					p4.y = p3.y;
					// Korrektirovka nepostavlenyx porogov
					int a = y-p1.y*m_seg_size;
					int b;
					if(p3.y<sizeEdges.cy-1)
						b = p3.y*m_seg_size-y;
					else
						b = size.cy-1-y;
					int c = x-p1.x*m_seg_size;
					int d;
					if(p4.x<sizeEdges.cx-1)
						d = p2.x*m_seg_size-x;
					else
						d = size.cx-1-x;
					double Ta,Tb,Tc,Td;
					long temp = p1.y*sizeEdges.cx+p1.x;
					Ta = pEdges[temp];
					Tb = pEdges[temp+1];
					Tc = pEdges[temp+sizeEdges.cx];
					Td = pEdges[temp+sizeEdges.cx+1];

					color T;
					T = (color)((b*d*Ta+b*c*Tb+a*d*Tc+a*c*Td)/((a+b)*(c+d)));
					
					if((*pSrc>>8)>=T)
						*pDst = 0xFF;
					else
						*pDst = 0;
				}
				
			}			

//			Notify( y );
		}

		FinishNotification();
		return true;
	}
	catch( ... )
	{
		return false;
	}
}

/////////////////////////////////////////////////////////////////////////////

void CChowKanekoFilter::InitHist(IImage3 *ptrSrcImage, long X, long Y)
{
	long y_to,y_from,x_to,x_from;
	y_from = max(0,Y*m_seg_size-m_seg_offset);
	y_to = min(size.cy-1,Y*m_seg_size+m_seg_offset);
	x_from = max(0,X*m_seg_size-m_seg_offset);
	x_to = min(size.cx-1,X*m_seg_size+m_seg_offset);
	ZeroMemory(pHist,sizeof(long)*256);
	byte *pmask;

	for( long y=y_from; y<y_to; y++ )
	{ 
		ptrSrcImage->GetRowMask( y ,&pmask );
		color* pSrc;
		ptrSrcImage->GetRow(y , GetWorkPane( ) , &pSrc);
		pSrc+=x_from;
		pmask+=x_from;

					
		for( long x = x_from; x < x_to; x++, pmask++, pSrc++)
		{
			if( *pmask==255  )
			{
				pHist[*pSrc>>8]++;
			}
		}
	}
	pHist[0] = (3*pHist[0]+2*pHist[1]+pHist[2])/6;
	pHist[1] = (2*pHist[0]+3*pHist[1]+2*pHist[2]+pHist[3])/8;
	for(int i=2; i<254; i++)
		pHist[i] = (pHist[i-2]+2*pHist[i-1]+3*pHist[i]+2*pHist[i+1]+pHist[i+2])/9;
	pHist[254] = (pHist[252]+2*pHist[253]+3*pHist[254]+2*pHist[255])/8;
	pHist[255] = (pHist[253]+2*pHist[254]+3*pHist[255])/6;
}

long CChowKanekoFilter::SelectEdge()
{
	long j = 0,i=0;
	color T;

	switch(m_EdgeType)
	{
		case EDGETYPE_PEAKS:
			{
			/* Find the first peak */
				for (i=0; i<256; i++)
				  if (pHist[i] > pHist[j]) j = i;

			/* Find the second peak */
				long k = 0;
				for(k=0; k<256 && pHist[k]==0; k++);

				for (i=1; i<256; i++)
				  if (pHist[i] && pHist[i-1]<=pHist[i] && pHist[i+1]<=pHist[i])
					if ((k-j)*(k-j)*pHist[k] < (i-j)*(i-j)*pHist[i])
						k = i;

				T = (color) j;
				if (j<k)
				{
				  for (i=j; i<k; i++)
					  if (pHist[i] && pHist[i] < pHist[T])
						  T = (color)i;
				}
				else
				{
				  for (i=k; i<j; i++)
					if (pHist[i] && pHist[i] < pHist[T])
						T = (color)i;
				}
			}
		break;

		case EDGETYPE_ENTROPY:
			{
				_ptr_t2<double> smartP(256);
				double *pP=smartP.ptr;
				int i,t;
				__int64 PixTotal=0;

				PixTotal=m_seg_size*m_seg_size;
				for(i=0; i<256; i++)
				{
					pP[i]=(double)(pHist[i])/(double)(PixTotal);
				}
				double min=10000000;
				color minT=0;
				double Cur=0;
				double Sb=0,Sw=0;
				double sum=0;

				for(t=1; t<256; t++)
				{
					Sb=0;
					Sw=0;
					sum=0;
					for(i=0; i<t; i++)
						sum+=pP[i];
					if(sum && sum!=pP[t] && pP[t])
						Sb=log10(sum)+(E(pP[t])+E(sum-pP[t]))/sum;
					else
						Sb=1000000;
				
					sum=0;
					for(i=t; i<256; i++)
						sum+=pP[i];
					if(sum && sum!=pP[t] && pP[t])
						Sw=log10(sum)+(E(pP[t])+E(sum-pP[t]))/sum;
					else
						Sw=1000000;

					Cur = Sb + Sw;
						
					if(Cur<min)
					{
						min=Cur;
						minT=t;
					}

				}
				T = minT;
			}
			break;
		case EDGETYPE_MINBINARY:
			{
				int i=0,j=0;
				double PixTotal = m_seg_size*m_seg_size;
				_ptr_t2<double> smartP1(256);
				double *pP1 = smartP1.ptr;
				_ptr_t2<double> smartMju1(256);
				double *pMju1 = smartMju1.ptr;
				_ptr_t2<double> smartMju2(256);
				double *pMju2 = smartMju2.ptr;

				double pSigma1=0;
				double pSigma2=0;
				double Jcur=0,Jmin=1000000000;
				T=0;

				double sum1=0,sum2=0,sum3=0,sum4=0,sum5=0;
				for(i=0; i<256; i++)
				{
					
					sum1+=pHist[i];
					pP1[i] = sum1;

					if(pP1[i])
					{
						sum2+=pHist[i]*i;
						pMju1[i] = sum2/pP1[i];
					}
					else
						pMju1[i]=0;

					if(PixTotal-pP1[i])
					{
						sum3+=pHist[255-i]*(255-i);
						pMju2[i] = sum3/(PixTotal-pP1[i]);
					}
					else
						pMju2[i]=0;

				}

				for(i=0; i<256; i++)
				{
					sum4=0;
					if(pP1[i])
					{
						for(j=0; j<=i; j++)
							sum4+=pHist[j]*(j-pMju1[i])*(j-pMju1[i]);
						pSigma1=sum4/pP1[i];
					}
					else
						pSigma1=0;

					sum5=0;
					if(PixTotal-pP1[i])
					{
						for(j=i+1; j<256; j++)
							sum5+=pHist[j]*(j-pMju2[i])*(j-pMju2[i]);
						pSigma2=sum5/(PixTotal-pP1[i]);
					}
					else
						pSigma2=0;

					if(pSigma1 && pSigma2 && pP1[i] && (PixTotal-pP1[i]))
						Jcur = 1+2*( pP1[i]*log10(pSigma1)+(PixTotal-pP1[i])*log10(pSigma2) ) - 2*( pP1[i]*log10(pP1[i])+(PixTotal-pP1[i])*log10(PixTotal-pP1[i]) );
					else
						Jcur = 100000000;

					if(Jcur<Jmin)
					{
						Jmin = Jcur;
						T =i;
					}
				}
			}

			break;
		case EDGETYPE_ITERATIVE:
			{
				color Tk=0;
				long i;
				__int64 sum1=0,sum2=0;
				for(i=0; i<256; i++)
					sum1+=i*pHist[i];
				Tk = (color)(sum1/(m_seg_size*m_seg_size));
				double newT=0,temp=0;
				while(fabs((double)Tk-temp)>1)
				{
					temp=Tk;
					sum1=0;
					sum2=0;
					
					for(i=0;i<Tk; i++)
					{
						sum1+=i*pHist[i];
						sum2+=pHist[i];
					}
					
					if(sum2)
						newT = (double)(sum1)/(double)(2*sum2);
					
					for(i=Tk;i<256; i++)
					{
						sum1+=i*pHist[i];
						sum2+=pHist[i];
					}
					
					if(sum2)
						newT += (double)(sum1)/(double)(2*sum2);

					Tk=(color)newT;

				}
				T=(color)newT;
			}
			break;
	}

	return T;
}

bool CChowKanekoFilter::CanDeleteArgument( CFilterArgument *pa )
{
	return false;
}

double CChowKanekoFilter::E(double X)
{
	return -X*log10(X);
}
