// LocalMin.cpp: implementation of the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LocalMax.h"
#include "misc_utils.h"
#include <math.h>

#include "nameconsts.h"
#include "ImagePane.h"
#include "binimageint.h"
#include "core5.h"
#include "ConnectedAreasFactory.h"

#define MaxColor color(-1)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//
//		LocalMin
//
//
/////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg	CLocalMinInfo::s_pargs[] = 
{
	{"Img",		szArgumentTypeImage, 0, true, true },
	{"LocalMin",szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};

bool CLocalMinFilter::InvokeFilter()
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

	byte *pmask =0;
	long y;
	long *pMapCur=0;

	try
	{
		StartNotification( size.cy, 1 );
		CConnectedAreasFactory factory;
		factory.CreateConnectedArea( GetWorkPane( ), ptrSrcImage);
		pMapCur = factory.pMap;

		for( y=0;y<size.cy;y++ )
		{
			ptrSrcImage->GetRowMask( y ,&pmask );
			color* pSrc,*pPredRow=0,*pNextRow=0;;
			ptrSrcImage->GetRow(y , GetWorkPane( ) , &pSrc);
			if(y>0)
				pPredRow = pSrc - size.cx;
			if(y+1<size.cy)
				pNextRow = pSrc + size.cx;

			for( long x = 0; x < size.cx; x++, pmask++, pSrc++,pPredRow++,pNextRow++)
			{
				if( *pmask==255  )
				{
					if(y>0)
					{
						int from = x-1>0 ? -1 : 0;
						int to = x+1<size.cx ? 1 : 0;
						for(int i=from; i<=to; i++)
						{
							if(*pSrc>*(pPredRow+i))
								factory.areas[*pMapCur].m_Deleted=false;
						}
					}
					if(y+1<size.cy)
					{
						int from = x-1>0 ? -1 : 0;
						int to = x+1<size.cx ? 1 : 0;
						for(int i=from; i<=to; i++)
						{
							if(*pSrc>*(pNextRow+i))
								factory.areas[*pMapCur].m_Deleted=false;
						}
					}
					if(x>0)
					{
						if(*pSrc>*(pSrc-1))
							factory.areas[*pMapCur].m_Deleted=false;
					}
					if(x+1<size.cx)
					{
						if(*pSrc>*(pSrc+1))
							factory.areas[*pMapCur].m_Deleted=false;
					}
				}

				pMapCur++;
			}			

			Notify( y );
		}

		pMapCur = factory.pMap;

		for( y=0;y<size.cy;y++ )
		{
			ptrSrcImage->GetRowMask( y ,&pmask );
			byte* pDst;

			ptrBin->GetRow(&pDst,y,0);

			for( long x = 0; x < size.cx; x++, pmask++, pDst++)
			{
				if( *pmask==255  )
				{
					if(factory.areas[*pMapCur].m_Deleted)
					{
						*pDst = 0xFF;
					}
					else
					{
						*pDst = 0;
					}
				}
				pMapCur++;
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

/////////////////////////////////////////////////////////////////////////////



bool CLocalMinFilter::CanDeleteArgument( CFilterArgument *pa )
{
	return false;
}
