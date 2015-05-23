// Pyramid.cpp: implementation of the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PyramidSegmentation.h"
#include "misc_utils.h"
#include <math.h>

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "ImagePane.h"
#include "PyrImage.h"
#include "PyrAreaPoint.h"
#include "PyrImageSmartPointer.h"

#define MaxColor color(-1)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//
//		Pyramid
//
//
/////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg	CPyramidInfo::s_pargs[] = 
{
	{"Param",	szArgumentTypeInt, "1", true, false },
	{"Img",		szArgumentTypeImage, 0, true, true },
	{"Pyramid",	szArgumentTypeImage, 0, false, true },	
	{0, 0, 0, false, false },
};

bool CPyramidFilter::InvokeFilter()
{

	IImage3Ptr	ptrSrcImage( GetDataArgument() );
	IImage3Ptr	ptrDestImage( GetDataResult() );
	CImagePaneCache paneCache( ptrSrcImage );
	
	if( ptrSrcImage == NULL || ptrDestImage == NULL )
		return false;
	{	
		ICompatibleObjectPtr ptrCO( ptrDestImage );
		
		if( ptrCO == NULL )
			return false;

		if( S_OK != ptrCO->CreateCompatibleObject( ptrSrcImage, NULL, 0 ) )
		{			
			return false;
		}		
	}

	int nPaneCount = ::GetImagePaneCount( ptrSrcImage );
	SIZE size;
	
	ptrSrcImage->GetSize((int*)&size.cx,(int*)&size.cy);

	int nParam = GetArgLong("Param");

	long yTotal = 0;
	long nTotalCount=0;

	CPyrImageSmartPointer smartPyrImages(nParam+1);
	CPyrImage* pPyrImages = smartPyrImages.ptr;
	
	
	for(int nPane=0;nPane<nPaneCount;nPane++)
	{
		nTotalCount=0;
		if( !paneCache.IsPaneEnable( nPane ) ) 
		{			
			continue;
		}

		color *pSrc=0;
		CPyrAreaPoint *pCurPoint,*pAvg,*pTempPoint;
		long x,y;
		SIZE fixsize;
		int oversize = (int)pow(2.,nParam+1);
		fixsize.cx = size.cx+2*oversize;
		fixsize.cy = size.cy+2*oversize;
		yTotal = (long)(fixsize.cy*(1-1/pow(2.,nParam)));
		StartNotification(2*(yTotal+nParam),nPaneCount);
		pPyrImages[0].CreateNewPyrImage(fixsize,0);
		// Izobrazhenie nulevogo urovnya s uchetom rasshireniya razmera dlya ustraneniya iskazheniy
		pCurPoint = pPyrImages[0].m_pPoints + fixsize.cx*oversize;
		for(y=oversize; y<fixsize.cy-oversize; y++)
		{
			ptrSrcImage->GetRow(y-oversize,nPane,&pSrc);
			pCurPoint+=oversize;
			for(x=oversize; x<fixsize.cx-oversize; x++,pSrc++)
			{
				pCurPoint->m_nColor = *pSrc;
				pCurPoint++;
			}
			pCurPoint+=oversize;
		}
		pCurPoint = pPyrImages[0].m_pPoints + fixsize.cx*oversize;
		for(y=oversize; y<fixsize.cy-oversize; y++)
		{
			ptrSrcImage->GetRow(y-oversize,nPane,&pSrc);
			pCurPoint+=oversize-1;
			for(x=oversize-1; x>=0; x--)
			{
				pCurPoint->m_nColor = *pSrc;
				pCurPoint--;
			}
			
			pCurPoint+=oversize+size.cx+1;
			pSrc+=size.cx-1;
			for(x=oversize+size.cx; x<2*oversize+size.cx; x++)
			{
				pCurPoint->m_nColor = *pSrc;
				pCurPoint++;
			}
		}

		for(y=oversize; y>0; y--)
		{
			pCurPoint = pPyrImages[0].m_pPoints + fixsize.cx*y;
			pTempPoint = pCurPoint-fixsize.cx;
			for(x=0; x<fixsize.cx; x++)
			{
				pTempPoint->m_nColor = pCurPoint->m_nColor;
				pCurPoint++;
				pTempPoint++;
			}
		}

		for(y=oversize+size.cy-1; y<fixsize.cy-1; y++)
		{
			pCurPoint = pPyrImages[0].m_pPoints + fixsize.cx*y;
			pTempPoint = pCurPoint+fixsize.cx;
			for(x=0; x<fixsize.cx; x++)
			{
				pTempPoint->m_nColor = pCurPoint->m_nColor;
				pCurPoint++;
				pTempPoint++;
			}
		}

		int k;
		//Formiruem izobrazheniya
//==========================================================
		for(k=1; k<=nParam; k++)
		{
			SIZE newsize,oldsize;
			oldsize = pPyrImages[k-1].m_size;
			newsize.cx = (int) ((oldsize.cx+1)/2);
			newsize.cy = (int) ((oldsize.cy+1)/2);

			if(newsize.cx==0 || newsize.cy == 0)
			{
				nParam = k;
				break;
			}

			long nSum=0,nCount=0;
			
			pPyrImages[k].CreateNewPyrImage(newsize,k);
			CPyrAreaPoint *pCurPoint=0;

// ====================== opredelyaem srednee iz 4x4 ==================
			
			for(y=0; y<oldsize.cy; y+=2)
			{
				pAvg = pPyrImages[k].m_pPoints + (int)(y/2)*newsize.cx;
				for(x=0; x<oldsize.cx; x+=2)
				{
					int i,to;
					nSum=0;
					nCount=0;

					pCurPoint = pPyrImages[k-1].m_pPoints + (y)*oldsize.cx + x;
					to = min(x+4,oldsize.cx);
					for(i=x; i<to; i++,pCurPoint++)
					{
						nSum+=pCurPoint->m_nColor;
						nCount++;
					}

					if(y+1<oldsize.cy)
					{
						pCurPoint = pPyrImages[k-1].m_pPoints + (y+1)*oldsize.cx + x;
						to = min(x+4,oldsize.cx);
						for(i=x; i<to; i++,pCurPoint++)
						{
							nSum+=pCurPoint->m_nColor;
							nCount++;
						}
					}

					if(y+2<oldsize.cy)
					{
						pCurPoint = pPyrImages[k-1].m_pPoints + (y+2)*oldsize.cx + x;
						to = min(x+4,oldsize.cx);
						for(i=x; i<to; i++,pCurPoint++)
						{
							nSum+=pCurPoint->m_nColor;
							nCount++;
						}
					}

					if(y+3<oldsize.cy)
					{
						pCurPoint = pPyrImages[k-1].m_pPoints + (y+3)*oldsize.cx + x;
						to = min(x+4,oldsize.cx);
						for(i=x; i<to; i++,pCurPoint++)
						{
							nSum+=pCurPoint->m_nColor;
							nCount++;
						}
					}

					pAvg->m_nColor = (color) (nSum/nCount);
					pAvg++;
				}
				nTotalCount++;
				Notify(nTotalCount);
			}
			
//========================     Svyazyvaem nizhniy uroven' s verxnim ============================
			bool bNotify = false;

			for(y=0; y<oldsize.cy; y++)
			{
								
				pCurPoint = pPyrImages[k-1].m_pPoints + y*oldsize.cx;
				long new_y = (long) y/2;

				for(x=0; x<oldsize.cx; x++,pCurPoint++)
				{
					long cur_color = pCurPoint->m_nColor;

					int y_from,y_to,x_from,x_to;
					long new_x = (long) x/2;
					
					int offset = 1;
					y_from = max(new_y-1,0);
					y_to = new_y;
					x_from = max(new_x-1,0);
					x_to = new_x;
					int idx=0;
					long min=10000000;
					CPyrAreaPoint* minpP=0;
					for( int m=y_from; m<=y_to; m++)
						for( int l=x_from; l<=x_to; l++)
						{
							CPyrAreaPoint* tempP = pPyrImages[k].m_pPoints + newsize.cx*m + l;
							long temp = labs(cur_color - tempP->m_nColor);
							if(temp<min)
							{
								minpP = tempP;
								min = temp;
							}
						}

					pCurPoint->m_ConnectedPoint = minpP;
					pCurPoint->m_ConnectedPoint->m_nSum+=(color)cur_color;
					pCurPoint->m_ConnectedPoint->m_nCount++;
					pCurPoint->m_ConnectedPoint->m_ConnectedPoint=0;


				}
				if(bNotify)
				{
					nTotalCount++;
					Notify(nTotalCount);
					bNotify = false;
				}
				else
					bNotify = true;
			}
			
// ========================= Usrednyaem po svyaznosti ======================
			pCurPoint = pPyrImages[k].m_pPoints;
			for(y=0; y<newsize.cy; y++)
			{
				for(x=0; x<newsize.cx; x++,pCurPoint++)
				{
					if(pCurPoint->m_nCount)
						pCurPoint->m_nColor = (color)(pCurPoint->m_nSum/pCurPoint->m_nCount);
				}
			}

		}

		color *pDst=0;
		byte *pmask=0;
		for(y=0; y<size.cy; y++)
		{
			pCurPoint = pPyrImages[0].m_pPoints+(y+oversize)*fixsize.cx+oversize;
			ptrDestImage->GetRow(y,nPane,&pDst);
			ptrSrcImage->GetRowMask(y,&pmask);
			for(x=0; x<size.cx; x++,pDst++)
			{
				for(k=0,pTempPoint=pCurPoint; k<nParam; k++)
				{
					if(pTempPoint->m_ConnectedPoint)
						pTempPoint = pTempPoint->m_ConnectedPoint;
				}

				if(*pmask==255)
					*pDst = pTempPoint->m_nColor;
				pCurPoint++;
			}
		}

		NextNotificationStage();
    }

	FinishNotification();
	return true;
}

/////////////////////////////////////////////////////////////////////////////


