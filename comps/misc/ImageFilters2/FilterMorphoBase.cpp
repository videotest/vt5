// FilterMorphoBase.cpp: implementation of the CFilterMorphoBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FilterMorphoBase.h"
#include "resource.h"
#include "misc_templ.h"
#include "misc_utils.h"
#include "ImageFilters2.h"
#include "ImagePane.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFilterMorphoBase::CFilterMorphoBase()
{

}

CFilterMorphoBase::~CFilterMorphoBase()
{

}
void CFilterMorphoBase::CreateConnectedArea(int nPane,long* pMap,IImage2* pSource,IImage2* pImageA1,POINT* pPoints,CConnectedArea* areas,int epsilon)
{
	color* pSrc;
		
	long* pMapCur;
	long x,y;
	long nFilledCount=0;
	long AreaCounter=0;
//	long nondeleted=0,deleted=0;
//	double avg_size=0;
	SIZE size;
	pSource->GetSize((int*)&size.cx,(int*)&size.cy);
	StartNotification(size.cy,1);
			
	for( y=0;y<size.cy;y++)
	{
		pSource->GetRow(y,nPane,&pSrc);
		color* pSrcDilated;
		pImageA1->GetRow(y,nPane,&pSrcDilated);
		pMapCur=pMap+y*size.cx;
		for( x = 0; x < size.cx; x++, pMapCur++,pSrc++,pSrcDilated++)
		{
			if(*pMapCur==-1) // still not added
			{
				long nCurrentCount = nFilledCount;
				long start=nFilledCount;

				color CurrentColor=*pSrc;
				AddFilledPoint( pSource,nPane,x, y, pMap, nFilledCount, pPoints, size,CurrentColor,AreaCounter,epsilon);

				while( nCurrentCount < nFilledCount )
				{
					int	xCurrent = pPoints[nCurrentCount].x;
					int	yCurrent = pPoints[nCurrentCount].y;
					//check 8 neightbords
					AddFilledPoint( pSource,nPane,xCurrent-1, yCurrent, pMap, nFilledCount, pPoints, size ,CurrentColor,AreaCounter,epsilon);
					AddFilledPoint( pSource,nPane,xCurrent+1, yCurrent, pMap, nFilledCount, pPoints, size ,CurrentColor,AreaCounter,epsilon);
					AddFilledPoint( pSource,nPane,xCurrent, yCurrent-1, pMap, nFilledCount, pPoints, size ,CurrentColor,AreaCounter,epsilon);
					AddFilledPoint( pSource,nPane,xCurrent, yCurrent+1, pMap, nFilledCount, pPoints, size ,CurrentColor,AreaCounter,epsilon);
					AddFilledPoint( pSource,nPane,xCurrent-1, yCurrent-1, pMap, nFilledCount, pPoints, size ,CurrentColor,AreaCounter,epsilon);
					AddFilledPoint( pSource,nPane,xCurrent-1, yCurrent+1, pMap, nFilledCount, pPoints, size ,CurrentColor,AreaCounter,epsilon);
					AddFilledPoint( pSource,nPane,xCurrent+1, yCurrent-1, pMap, nFilledCount, pPoints, size ,CurrentColor,AreaCounter,epsilon);
					AddFilledPoint( pSource,nPane,xCurrent+1, yCurrent+1, pMap, nFilledCount, pPoints, size ,CurrentColor,AreaCounter,epsilon);
					//check the next point
					nCurrentCount++;
				}

				CConnectedArea area;
				area.m_AreaColor=CurrentColor;
				area.m_lStartPoint=start;
				area.m_EndPoint=nCurrentCount;
				area.m_NeighborColor=0;
				area.m_Deleted=true;
				areas[AreaCounter]=area;
//				avg_size+=nCurrentCount-start;
				AreaCounter++;
			}
			if(*pSrc==*pSrcDilated && areas[*pMapCur].m_Deleted==true)
			{
				areas[*pMapCur].m_Deleted=false;
	//			nondeleted++;
			}
		} 
		Notify(y);
	}
	//NextNotificationStage();
	FinishNotification();
	//avg_size/=AreaCounter;
	//deleted=AreaCounter-nondeleted;
	m_nAreaCounter = AreaCounter;
}

void CFilterMorphoBase::CreateConnectedArea2(int nPane,long* pMap,IBinaryImagePtr pSource,color* pImageA1,POINT* pPoints,CConnectedArea* areas, int epsilon)
{
	byte* pSrc;
		
	long* pMapCur;
	long x,y;
	long nFilledCount=0;
	long AreaCounter=0;
	SIZE size;
	pSource->GetSizes((int*)&size.cx,(int*)&size.cy);
				
	for( y=0;y<size.cy;y++)
	{
		pSource->GetRow(&pSrc,y,0);;
		color* pSrcDilated;
		pSrcDilated = pImageA1 + y*size.cx;
		pMapCur=pMap+y*size.cx;
		for( x = 0; x < size.cx; x++, pMapCur++,pSrc++,pSrcDilated++)
		{
			if(*pMapCur==-1) // still not added
			{
				long nCurrentCount = nFilledCount;
				long start=nFilledCount;

				color CurrentColor=*pSrc;
				AddFilledPoint2( pSource,nPane,x, y, pMap, nFilledCount, pPoints, size,CurrentColor,AreaCounter,epsilon);

				while( nCurrentCount < nFilledCount )
				{
					int	xCurrent = pPoints[nCurrentCount].x;
					int	yCurrent = pPoints[nCurrentCount].y;
					//check 8 neightbords
					AddFilledPoint2( pSource,nPane,xCurrent-1, yCurrent, pMap, nFilledCount, pPoints, size ,CurrentColor,AreaCounter,epsilon);
					AddFilledPoint2( pSource,nPane,xCurrent+1, yCurrent, pMap, nFilledCount, pPoints, size ,CurrentColor,AreaCounter,epsilon);
					AddFilledPoint2( pSource,nPane,xCurrent, yCurrent-1, pMap, nFilledCount, pPoints, size ,CurrentColor,AreaCounter,epsilon);
					AddFilledPoint2( pSource,nPane,xCurrent, yCurrent+1, pMap, nFilledCount, pPoints, size ,CurrentColor,AreaCounter,epsilon);
					AddFilledPoint2( pSource,nPane,xCurrent-1, yCurrent-1, pMap, nFilledCount, pPoints, size ,CurrentColor,AreaCounter,epsilon);
					AddFilledPoint2( pSource,nPane,xCurrent-1, yCurrent+1, pMap, nFilledCount, pPoints, size ,CurrentColor,AreaCounter,epsilon);
					AddFilledPoint2( pSource,nPane,xCurrent+1, yCurrent-1, pMap, nFilledCount, pPoints, size ,CurrentColor,AreaCounter,epsilon);
					AddFilledPoint2( pSource,nPane,xCurrent+1, yCurrent+1, pMap, nFilledCount, pPoints, size ,CurrentColor,AreaCounter,epsilon);
					//check the next point
					nCurrentCount++;
				}

				CConnectedArea area;
				area.m_AreaColor=CurrentColor;
				area.m_lStartPoint=start;
				area.m_EndPoint=nCurrentCount;
				area.m_NeighborColor=0;
				area.m_Deleted=true;
				areas[AreaCounter]=area;
//				avg_size+=nCurrentCount-start;
				AreaCounter++;
			}
			if(*pSrc==*pSrcDilated && areas[*pMapCur].m_Deleted==true)
			{
				areas[*pMapCur].m_Deleted=false;
	//			nondeleted++;
			}
		} 
		Notify(y);
	}
	NextNotificationStage();
	//avg_size/=AreaCounter;
	//deleted=AreaCounter-nondeleted;
	m_nAreaCounter = AreaCounter;
}

void CFilterMorphoBase::AddFilledPoint(IImage2* pSource, int nPane, long x, long y,  long* pMap, long &nFilledCount, POINT* points, SIZE size, color CurrentColor, long lIndex, int nEpsilon)
{
	if( x < 0 || y < 0 || x >= size.cx || y >= size.cy )
		return;

	if( *(pMap+y*size.cx+x) != -1 )
		return;

	color temp_color;
	color* ptemp;
	pSource->GetRow(y,nPane,&ptemp);
	temp_color=*(ptemp+x);
	
	if(labs(temp_color-CurrentColor)>nEpsilon)
		return;

	*(pMap+y*size.cx+x) = lIndex;
	points[nFilledCount].x = x;
	points[nFilledCount].y = y;
	nFilledCount++;
}

void CFilterMorphoBase::AddFilledPoint2(IBinaryImagePtr pSource, int nPane, long x, long y,  long* pMap, long &nFilledCount, POINT* points, SIZE size, color CurrentColor, long lIndex, int nEpsilon)
{
	if( x < 0 || y < 0 || x >= size.cx || y >= size.cy )
		return;

	if( *(pMap+y*size.cx+x) != -1 )
		return;

	color temp_color;
	byte* ptemp;
	pSource->GetRow(&ptemp,y,0);
	temp_color=*(ptemp+x);
	
	if(labs(temp_color-CurrentColor)>nEpsilon)
		return;

	*(pMap+y*size.cx+x) = lIndex;
	points[nFilledCount].x = x;
	points[nFilledCount].y = y;
	nFilledCount++;
}

bool CFilterMorphoBase::OpenByReconstruction(int nMaskType, int nMaskSize, int epsilon, IImage2 *pSource, IImage2 *pResult)
{
	int k=0;
		if( !pSource || !pResult )
		return false;

	SIZE size;
	pSource->GetSize((int*)&size.cx,(int*)&size.cy);
	POINT ptOffset;
	pSource->GetOffset(&ptOffset);

	// [vanek] 12.12.2003: works with color convertors panes
	CImagePaneCache paneCache( pSource );

	int nPaneCount;
	BSTR bstrName;
	IColorConvertor2* pIClrConv;
	pSource->GetColorConvertor((IUnknown**)&pIClrConv);
	//nPaneCount = ::GetImagePaneCount( pSource );
    nPaneCount = paneCache.GetPanesCount( );
    pIClrConv->GetCnvName(&bstrName);
	pResult->CreateImage(size.cx, size.cy,bstrName, ::GetImagePaneCount( pSource ) ); 
	pResult->SetOffset( ptOffset ,TRUE);
	pResult->InitRowMasks();
	pIClrConv->Release();
	SysFreeString(bstrName);

	_ptr_t2<CConnectedArea> smartAreas(size.cx*size.cy);
	CConnectedArea *areas=smartAreas.ptr;

	_ptr_t2<color> smartImageA1(size.cx*size.cy);
	color* pImageA1=smartImageA1.ptr;

	_ptr_t2<long> smartMap(size.cx*size.cy);
	long *pMap=smartMap.ptr;
	_ptr_t2<bool> smartFlags(size.cx*size.cy);
	bool *pFlags=smartFlags.ptr;
	_ptr_t2<POINT> smartPoints(size.cx*size.cy);
	POINT *pPoints=smartPoints.ptr;

	_ptr_t2<color*> smartSrcRows(size.cy);
	color** pSrcRows=smartSrcRows.ptr;
	_ptr_t2<color*> smartDstRows(size.cy);
	color** pDstRows=smartDstRows.ptr;
	_ptr_t2<color*> smartA1Rows(size.cy);
	color** pA1Rows=smartA1Rows.ptr;

	StartNotification(size.cy,nPaneCount,5);    // erosion + connected areas + obr
	Erode(nMaskType,nMaskSize,pSource,pResult);

	for( int nPane=0;nPane<nPaneCount;nPane++ )
	{
		if( !paneCache.IsPaneEnable( nPane ) ) 
		{			
			continue;
		}
		if(IsPaneEnable(nPane))
		{
			int x,y;
			//=============== Stage 1 Creating Map and Areas structures =================
			long* pMapCur;
			bool* pFlagCur;
			byte* pmask;

// =========== Create Connectead Areas ===========================			
			FillMemory(pMap,size.cx*size.cy*sizeof(long),-1);
			CreateConnectedArea(nPane,pMap,pSource,pResult,pPoints,areas,epsilon);
// =========== Reconstruct non-erased areas immideatly ==============
			pMapCur=pMap;
			pFlagCur=pFlags;
			color *pSrc,*pDst,*pSrcA1;
			pSrcA1=pImageA1;
						
			for( y=0;y<size.cy;y++)
			{
				pSource->GetRow(y,nPane,&pSrc);
			    pResult->GetRow(y,nPane,&pDst);
				pSrcRows[y]=pSrc;
				pDstRows[y]=pDst;
				pA1Rows[y]=pSrcA1;
				pSource->GetRowMask( y, &pmask );
				for( x = 0; x < size.cx; x++)
				{
					if(areas[*pMapCur].m_Deleted==false)
					{
						*pDst=*pSrc;
						*pFlagCur=false;
					}
					else
					{
						*pFlagCur=true;
					}

					if(*pmask!=255) 
						*pFlagCur=false;
					
					pSrc++;
					pDst++;
					pMapCur++;
					pFlagCur++;
					pSrcA1++;
					pmask++;
				}
			}
  
			int iChangedRow=0;
			bool bPaneChanged=true;
						
			while( bPaneChanged )
			{ 
				k++;
			// делаем дилатацию там где можно
// ====================================================================================
				color	*p1, *p2, *p3;
				bool	bChanged = false;
				pFlagCur=pFlags;
				
				for(y=0; y<size.cy; y++)
				{
					color max_color;
					color left_max,center_max,right_max;
					p1 = pA1Rows[max( y-1, 0 )];
					p2 = pA1Rows[y];
					p3 = pA1Rows[min( y+1, size.cy-1 )];
					pSrc = pSrcRows[y];
					pDst = pDstRows[y];
					CopyMemory(p1,pDstRows[max(y-1,0)],sizeof(color)*size.cx);
					CopyMemory(p2,pDstRows[y],sizeof(color)*size.cx);
					CopyMemory(p3,pDstRows[min(y+1,size.cy-1)],sizeof(color)*size.cx);
					
					if(*pFlagCur)
					{
						max_color= max( max( max(*p1,*(p1+1)),
											 max(*p2,*(p2+1)) ),
											 max(*p3,*(p3+1)) );
						if(*pSrc<max_color)
						{
							*pDst=*pSrc;
							*pFlagCur=true;
						}
						else
							*pDst=max_color;

						if(*p2!=*pDst && !bChanged)
						{
						 	iChangedRow=y;
							bChanged=true;
						} 
					}

					p1++;
					p2++;
					p3++;
					pFlagCur++;
					pDst++;
					pSrc++;
					
					left_max=max( max( *(p1-1),*(p2-1) ), *(p3-1));
					center_max=max( max( *p1,*p2 ), *p3);
					right_max=max( max( *(p1+1),*(p2+1) ), *(p3+1));

					for(x=1; x<size.cx-1; x++,p1++,p2++,p3++,pFlagCur++,pDst++,pSrc++)
					{
						left_max=center_max;
						center_max=right_max;
						right_max=max( max( *(p1+1),*(p2+1) ), *(p3+1));

						if(*pFlagCur)
						{
							max_color=max( max(right_max,center_max),left_max);

							if(*pSrc<max_color)
							{
								*pDst=*pSrc;
								*pFlagCur=false;
							}
							else
								*pDst=max_color;

							if(*p2!=*pDst && !bChanged)
							{
						 		iChangedRow=y;
								bChanged=true;
							} 
						}
					}

					if(*pFlagCur)
					{
						max_color= max( max( max(*p1,*(p1-1)),
											 max(*p2,*(p2-1)) ),
											 max(*p3,*(p3-1)) );
						if(*pSrc<max_color)
						{
							*pDst=*pSrc;
							*pFlagCur=false;
						}
						else
							*pDst=max_color;

						if(*p2!=*pDst && !bChanged)
						{
						 	iChangedRow=y;
							bChanged=true;
						} 
					}

					pFlagCur++;
				}
				bPaneChanged=bChanged;
//   ==================== Изменилась пана? ===================
				Notify(iChangedRow);
			}  
		} 
		else
		{
			for( long y=0;y<size.cy;y++ )
			{ 
				byte* pmask;
				pSource->GetRowMask( y, &pmask );
				color *pSrc,*pDst;
				pSource->GetRow( y, nPane , &pSrc);
				pResult->GetRow( y, nPane , &pDst);
				for( long x = 0; x<size.cx; x++, pmask++, pSrc++, pDst++ )
				{
					if( *pmask==255 )
						*pSrc=*pDst;
				}
				Notify( y );
			}
		}
		NextNotificationStage();
	} 
	FinishNotification();
	return TRUE;
}

bool CFilterMorphoBase::CloseByReconstruction(int nMaskType, int nMaskSize, int epsilon, IImage2 *pSource, IImage2 *pResult)
{
	int k=0;
		if( !pSource || !pResult )
		return false;

	SIZE size;
	pSource->GetSize((int*)&size.cx,(int*)&size.cy);
	POINT ptOffset;
	pSource->GetOffset(&ptOffset);

	// [vanek] 12.12.2003: works with color convertors panes
	CImagePaneCache paneCache( pSource );

	int nPaneCount;
	BSTR bstrName;
	IColorConvertor2* pIClrConv;
	pSource->GetColorConvertor((IUnknown**)&pIClrConv);
	//nPaneCount = ::GetImagePaneCount( pSource );
	nPaneCount = paneCache.GetPanesCount( );
	pIClrConv->GetCnvName(&bstrName);
	pResult->CreateImage(size.cx, size.cy,bstrName, ::GetImagePaneCount( pSource ) ); 
	pResult->SetOffset( ptOffset ,TRUE);
	pResult->InitRowMasks();
	pIClrConv->Release();
	SysFreeString(bstrName);

	_ptr_t2<CConnectedArea> smartAreas(size.cx*size.cy);
	CConnectedArea *areas=smartAreas.ptr;

	_ptr_t2<color> smartImageA1(size.cx*size.cy);
	color* pImageA1=smartImageA1.ptr;

	_ptr_t2<long> smartMap(size.cx*size.cy);
	long *pMap=smartMap.ptr;
	_ptr_t2<bool> smartFlags(size.cx*size.cy);
	bool *pFlags=smartFlags.ptr;
	_ptr_t2<POINT> smartPoints(size.cx*size.cy);
	POINT *pPoints=smartPoints.ptr;

	_ptr_t2<color*> smartSrcRows(size.cy);
	color** pSrcRows=smartSrcRows.ptr;
	_ptr_t2<color*> smartDstRows(size.cy);
	color** pDstRows=smartDstRows.ptr;
	_ptr_t2<color*> smartA1Rows(size.cy);
	color** pA1Rows=smartA1Rows.ptr;

	StartNotification(size.cy,nPaneCount*3,5);    // dilation- + connected areas + obr
	Dilate(nMaskType,nMaskSize,pSource,pResult);
	
	for( int nPane=0;nPane<nPaneCount;nPane++ )
	{
		if( !paneCache.IsPaneEnable( nPane ) ) 
		{			
			continue;
		}
		if(IsPaneEnable(nPane))    
		{ 
			int x,y;
			//=============== Stage 1 Creating Map and Areas structures =================
			
			long* pMapCur;
			bool* pFlagCur;
			byte* pmask;

// =========== Create Connectead Areas ===========================			
			FillMemory(pMap,size.cx*size.cy*sizeof(long),-1);
			CreateConnectedArea(nPane,pMap,pSource,pResult,pPoints,areas,epsilon);
// =========== Reconstruct non-erased areas immideatly ==============
			pMapCur=pMap;
			pFlagCur=pFlags;
			color *pSrc,*pDst,*pSrcA1;
			pSrcA1=pImageA1;
						
			for( y=0;y<size.cy;y++)
			{
				pSource->GetRow(y,nPane,&pSrc);
			    pResult->GetRow(y,nPane,&pDst);
				pSrcRows[y]=pSrc;
				pDstRows[y]=pDst;
				pA1Rows[y]=pSrcA1;
				pSource->GetRowMask( y, &pmask );
				for( x = 0; x < size.cx; x++)
				{
					if(areas[*pMapCur].m_Deleted==false)
					{
						*pDst=*pSrc;
						*pFlagCur=false;
					}
					else
					{
						*pFlagCur=true;
					}
					if(*pmask!=255) 
						*pFlagCur=false;
					pSrc++;
					pDst++;
					pMapCur++;
					pFlagCur++;
					pSrcA1++;
					pmask++;
				}
			}
  
			int iChangedRow=0;
			bool bPaneChanged=true;
						
			while( bPaneChanged )
			{ 
				k++;
			// делаем дилатацию там где можно
// ====================================================================================
				color	*p1, *p2, *p3;
				bool	bChanged = false;
				pFlagCur=pFlags;
				
				for(y=0; y<size.cy; y++)
				{
					color min_color;
					color left_min,center_min,right_min;
					p1 = pA1Rows[max( y-1, 0 )];
					p2 = pA1Rows[y];
					p3 = pA1Rows[min( y+1, size.cy-1 )];
					pSrc = pSrcRows[y];
					pDst = pDstRows[y];
					CopyMemory(p1,pDstRows[max(y-1,0)],sizeof(color)*size.cx);
					CopyMemory(p2,pDstRows[y],sizeof(color)*size.cx);
					CopyMemory(p3,pDstRows[min(y+1,size.cy-1)],sizeof(color)*size.cx);

					if(*pFlagCur)
					{
						min_color= min( min( min(*p1,*(p1+1)),
											 min(*p2,*(p2+1)) ),
											 min(*p3,*(p3+1)) );
						if(*pSrc>min_color)
						{
							*pDst=*pSrc;
							*pFlagCur=true;
						}
						else
							*pDst=min_color;

						if(*p2!=*pDst && !bChanged)
						{
						 	iChangedRow=y;
							bChanged=true;
						} 
					}

					p1++;
					p2++;
					p3++;
					pFlagCur++;
					pDst++;
					pSrc++;

					left_min=min( min( *(p1-1),*(p2-1) ), *(p3-1));
					center_min=min( min( *p1,*p2 ), *p3);
					right_min=min( min( *(p1+1),*(p2+1) ), *(p3+1));

					for(x=1; x<size.cx-1; x++,p1++,p2++,p3++,pFlagCur++,pDst++,pSrc++)
					{
							left_min=center_min;
							center_min=right_min;
							right_min=min( min( *(p1+1),*(p2+1) ), *(p3+1));

						if(*pFlagCur)
						{
							min_color=min( min(right_min,center_min),left_min);

							if(*pSrc>min_color)
							{
								*pDst=*pSrc;
								*pFlagCur=false;
							}
							else
								*pDst=min_color;

							if(*p2!=*pDst && !bChanged)
							{
						 		iChangedRow=y;
								bChanged=true;
							} 
						}
					}

					if(*pFlagCur)
					{
						min_color= min( min( min(*p1,*(p1-1)),
											 min(*p2,*(p2-1)) ),
											 min(*p3,*(p3-1)) );
						if(*pSrc>min_color)
						{
							*pDst=*pSrc;
							*pFlagCur=false;
						}
						else
							*pDst=min_color;

						if(*p2!=*pDst && !bChanged)
						{
						 	iChangedRow=y;
							bChanged=true;
						} 
					}

					pFlagCur++;
				}

				bPaneChanged=bChanged;
				Notify(iChangedRow);
			}
		}
		else    //  Pane not enabled
		{
			for( long y=0;y<size.cy;y++ )
			{ 
				byte* pmask;
				pSource->GetRowMask( y, &pmask );
				color *pSrc,*pDst;
				pSource->GetRow( y, nPane , &pSrc);
				pResult->GetRow( y, nPane , &pDst);
				for( long x = 0; x<size.cx; x++, pmask++, pSrc++, pDst++ )
				{
					if( *pmask==255 )
						*pSrc=*pDst;
				}
				Notify( y );
			}
		}
	NextNotificationStage();
	} 
		  
	FinishNotification();
	return TRUE;
}

bool CFilterMorphoBase::Erode(int nMaskType, int nMaskSize, IImage2* pSource, IImage2* pResult)
{

	if( !pSource || !pResult )
		return false;

	SIZE size;
	pSource->GetSize((int*)&size.cx,(int*)&size.cy);

	// [vanek] 12.12.2003: works with color convertors panes
	CImagePaneCache paneCache( pSource );
    //int nPaneCount = ::GetImagePaneCount( pSource );
	int nPaneCount = paneCache.GetPanesCount( );

	StartNotification( size.cy, nPaneCount );
	byte *pmask;

	if(nMaskType==MASK_LINE0)
	{
		for( int nPane=0;nPane<nPaneCount;nPane++ )
		{
			if( !paneCache.IsPaneEnable( nPane ) ) 
			{			
				continue;
			}
			if(IsPaneEnable(nPane))
				for( long y=0;y<size.cy;y++ )
				{
					pSource->GetRowMask( y, &pmask );

					color *pSrc,*pDst;
					pSource->GetRow( y, nPane , &pSrc);
					pResult->GetRow( y, nPane , &pDst);

					for( long x = 0; x < size.cx; x++, pmask++, pSrc++, pDst++ )
					{
						if( *pmask==255 )
						{
							//*pDst = ~*pSrc;
							color minimum=*(pSrc);
							for(int k=-nMaskSize; k<=nMaskSize; k++)
							{
								color cur;
								if(x+k>=0 && x+k<size.cx)
									cur=*(pSrc+k);
								 else
									cur=MaxColor;
								 if(cur<minimum)
									 minimum=cur;
							}
							*pDst=minimum;
						}
					}
					Notify( y );
				}
				NextNotificationStage( );
		}

	FinishNotification();
	return TRUE;
	}
	
	if(nMaskType==MASK_LINE90)
	{
		_ptr_t2<color*> spRows(nMaskSize*2+1);
		color** rows=spRows.ptr;

		for( int nPane=0;nPane<nPaneCount;nPane++ )
		{
			if( !paneCache.IsPaneEnable( nPane ) ) 
			{			
				continue;
			}
			if(IsPaneEnable(nPane))
				for( long y=0;y<size.cy;y++ )
				{
					pSource->GetRowMask( y, &pmask );

					color *pSrc,*pDst;
					pSource->GetRow( y, nPane , &pSrc);
					pResult->GetRow( y, nPane , &pDst);
					int k;

					for(k=-nMaskSize; k<=nMaskSize; k++)
					{
						if(y+k>=0 && y+k<size.cy)
							pSource->GetRow(y+k,nPane,&rows[nMaskSize+k]);
						else
							rows[nMaskSize+k]=NULL;
					}

					for( long x = 0; x < size.cx; x++, pmask++, pSrc++, pDst++ )
					{
						if( *pmask==255 )
						{
							//*pDst = ~*pSrc;
							
							color minimum=*(pSrc);
							for(k=-nMaskSize; k<=nMaskSize; k++)
							{
								color cur;
								if(rows[nMaskSize+k])
									cur=*(rows[nMaskSize+k]);
								 else
									cur=MaxColor;
								 if(cur<minimum)
									 minimum=cur;
							}
							*pDst=minimum;
						}
					for(k=-nMaskSize; k<=nMaskSize; k++)
						if(rows[nMaskSize+k]) 
							rows[nMaskSize+k]++;
					}
					Notify( y );
				}
				NextNotificationStage( );
		}

	  FinishNotification();
	  
	return TRUE;
	}

	if(nMaskType==MASK_LINE135)
	{
		_ptr_t2<color*> spRows(nMaskSize*2+1);
		color** rows=spRows.ptr;

		for( int nPane=0;nPane<nPaneCount;nPane++ )
		{
			if( !paneCache.IsPaneEnable( nPane ) ) 
			{			
				continue;
			}
			if(IsPaneEnable(nPane))
				for( long y=0;y<size.cy;y++ )
				{
					pSource->GetRowMask( y, &pmask );

					color *pSrc,*pDst;
					pSource->GetRow( y, nPane , &pSrc);
					pResult->GetRow( y, nPane , &pDst);
					int k;

					for(k=-nMaskSize; k<=nMaskSize; k++)
					{
						if(y+k>=0 && y+k<size.cy)
							pSource->GetRow(y+k,nPane,&rows[nMaskSize+k]);
						else
							rows[nMaskSize+k]=NULL;
					}

					for(k=-nMaskSize; k<=nMaskSize; k++)   // delaem diagonal
					{
						if(rows[nMaskSize+k])
							rows[nMaskSize+k]+=k;
					}

					for( long x = 0; x < size.cx; x++, pmask++, pSrc++, pDst++ )
					{
						if( *pmask==255 )
						{
							//*pDst = ~*pSrc;
							
							color minimum=*(pSrc);
							for(k=-nMaskSize; k<=nMaskSize; k++)
							{
								color cur;
								if(rows[nMaskSize+k] && x+k>=0 && x+k<size.cx)
									cur=*(rows[nMaskSize+k]);
								 else
									cur=MaxColor;
								 if(cur<minimum)
									 minimum=cur;
							}
							*pDst=minimum;
						}
					for(k=-nMaskSize; k<=nMaskSize; k++)
						if(rows[nMaskSize+k]) 
							rows[nMaskSize+k]++;
					}
				Notify( y );
				}
				NextNotificationStage( );
		}

	FinishNotification();
	return TRUE;
	}

	if(nMaskType==MASK_LINE45)
	{
		_ptr_t2<color*> spRows(nMaskSize*2+1);
		color** rows=spRows.ptr;

		for( int nPane=0;nPane<nPaneCount;nPane++ )
		{
			if( !paneCache.IsPaneEnable( nPane ) ) 
			{			
				continue;
			}
			if(IsPaneEnable(nPane))
				for( long y=0;y<size.cy;y++ )
				{
					pSource->GetRowMask( y, &pmask );

					color *pSrc,*pDst;
					pSource->GetRow( y, nPane , &pSrc);
					pResult->GetRow( y, nPane , &pDst);
					int k;

					for(k=-nMaskSize; k<=nMaskSize; k++)
					{
						if(y+k>=0 && y+k<size.cy)
							pSource->GetRow(y+k,nPane,&rows[nMaskSize+k]);
						else
							rows[nMaskSize+k]=NULL;
					}

					for(k=-nMaskSize; k<=nMaskSize; k++)   // delaem diagonal
					{
						if(rows[nMaskSize+k])
							rows[nMaskSize+k]-=k;
					}

					for( long x = 0; x < size.cx; x++, pmask++, pSrc++, pDst++ )
					{
						if( *pmask==255 )
						{
							//*pDst = ~*pSrc;
							
							color minimum=*(pSrc);
							for(k=-nMaskSize; k<=nMaskSize; k++)
							{
								color cur;
								if(rows[nMaskSize+k] && x-k>=0 && x-k<size.cx)
									cur=*(rows[nMaskSize+k]);
								 else
									cur=MaxColor;
								 if(cur<minimum)
									 minimum=cur;
							}
							*pDst=minimum;
						}
					for(k=-nMaskSize; k<=nMaskSize; k++)
						if(rows[nMaskSize+k]) 
							rows[nMaskSize+k]++;
					}
					Notify( y );
				}
				NextNotificationStage( );
		}

	FinishNotification();

	return TRUE;
	}

	if(nMaskType==MASK_SQUARE)
	{
		_ptr_t2<color*> spRows(nMaskSize*2+1);
		color** rows=spRows.ptr;

		for( int nPane=0;nPane<nPaneCount;nPane++ )
		{
			if( !paneCache.IsPaneEnable( nPane ) ) 
			{			
				continue;
			}
			if(IsPaneEnable(nPane))
				for( long y=0;y<size.cy;y++ )
				{
					pSource->GetRowMask( y, &pmask );

					color *pSrc,*pDst;
					pSource->GetRow( y, nPane , &pSrc);
					pResult->GetRow( y, nPane , &pDst);
					int k;

					for(k=-nMaskSize; k<=nMaskSize; k++)
					{
						if(y+k>=0 && y+k<size.cy)
							pSource->GetRow(y+k,nPane,&rows[nMaskSize+k]);
						else
							rows[nMaskSize+k]=NULL;
					}

					for( long x = 0; x < size.cx; x++, pmask++, pSrc++, pDst++ )
					{
						if( *pmask==255 )
						{
							//*pDst = ~*pSrc;
							
							color minimum=*pSrc;
							
							for(k=-nMaskSize; k<=nMaskSize; k++)
							for(int j=-nMaskSize; j<=nMaskSize; j++)
							{
								color cur;
								if(rows[nMaskSize+k]!=NULL && x+j>=0 && x+j<size.cx)
									cur=*(rows[nMaskSize+k]+j);
								 else
									cur=MaxColor;
								 if(cur<minimum)
									 minimum=cur;
							}
							*pDst=minimum;
						}
						for(k=-nMaskSize; k<=nMaskSize; k++)
							if(rows[nMaskSize+k]!=NULL) 
								rows[nMaskSize+k]++;
					}
					Notify( y );
				}
				NextNotificationStage( );
		}

	FinishNotification();
	return TRUE;
	}

	return false;
}

bool CFilterMorphoBase::Dilate(int nMaskType, int nMaskSize, IImage2* pSource, IImage2* pResult)
{

	if( !pSource || !pResult )
		return false;

	SIZE size;
	pSource->GetSize((int*)&size.cx,(int*)&size.cy);
	
	// [vanek] 12.12.2003: works with color convertors panes
	CImagePaneCache paneCache( pSource );
	//int nPaneCount = ::GetImagePaneCount( pSource );
	int nPaneCount = paneCache.GetPanesCount( );

	StartNotification( size.cy, nPaneCount );

	byte *pmask;

	if(nMaskType==MASK_LINE0)
	{
		for( int nPane=0;nPane<nPaneCount;nPane++ )
		{
			if( !paneCache.IsPaneEnable( nPane ) ) 
			{			
				continue;
			}
			if(IsPaneEnable(nPane))
				for( long y=0;y<size.cy;y++ )
				{
					pSource->GetRowMask( y, &pmask );

					color *pSrc,*pDst;
					pSource->GetRow( y, nPane , &pSrc);
					pResult->GetRow( y, nPane , &pDst);

					for( long x = 0; x<size.cx; x++, pmask++, pSrc++, pDst++ )
					{
						if( *pmask==255 )
						{
							//*pDst = ~*pSrc;
							color maximum=*(pSrc);
							for(int k=-nMaskSize; k<=nMaskSize; k++)
							{
								color cur;
								if(x+k>=0 && x+k<size.cx)
									cur=*(pSrc+k);
								 else
									cur=0;
								 if(cur>maximum)
									 maximum=cur;
							}
							*pDst=maximum;
						}
					}
					Notify( y );
				}
				NextNotificationStage( );
		}

	FinishNotification();
	return TRUE;
	}
	
	if(nMaskType==MASK_LINE90)
	{
		_ptr_t2<color*> spRows(nMaskSize*2+1);
		color** rows=spRows.ptr;

		for( int nPane=0;nPane<nPaneCount;nPane++ )
		{
			if( !paneCache.IsPaneEnable( nPane ) ) 
			{			
				continue;
			}
			if(IsPaneEnable(nPane))
				for( long y=0;y<size.cy;y++ )
				{
					pSource->GetRowMask( y, &pmask );

					color *pSrc,*pDst;
					pSource->GetRow( y, nPane , &pSrc);
					pResult->GetRow( y, nPane , &pDst);
					int k;

					for(k=-nMaskSize; k<=nMaskSize; k++)
					{
						if(y+k>=0 && y+k<size.cy)
							pSource->GetRow(y+k,nPane,&rows[nMaskSize+k]);
						else
							rows[nMaskSize+k]=NULL;
					}

					for( long x = 0; x<size.cx; x++, pmask++, pSrc++, pDst++ )
					{
						if( *pmask==255 )
						{
							//*pDst = ~*pSrc;
							
							color maximum=*(pSrc);
							for(k=-nMaskSize; k<=nMaskSize; k++)
							{
								color cur;
								if(rows[nMaskSize+k])
									cur=*(rows[nMaskSize+k]);
								 else
									cur=0;
								 if(cur>maximum)
									 maximum=cur;
							}
							*pDst=maximum;
						}
					for(k=-nMaskSize; k<=nMaskSize; k++)
						if(rows[nMaskSize+k]) 
							rows[nMaskSize+k]++;
					}
					Notify( y );
				}
				NextNotificationStage( );
		}

	FinishNotification();
	
	return TRUE;
	}

	if(nMaskType==MASK_LINE135)
	{
		_ptr_t2<color*> spRows(nMaskSize*2+1);
		color** rows=spRows.ptr;

		for( int nPane=0;nPane<nPaneCount;nPane++ )
		{
			if( !paneCache.IsPaneEnable( nPane ) ) 
			{			
				continue;
			}
			if(IsPaneEnable(nPane))
				for( long y=0;y<size.cy;y++ )
				{
					pSource->GetRowMask( y, &pmask );

					color *pSrc,*pDst;
					pSource->GetRow( y, nPane , &pSrc);
					pResult->GetRow( y, nPane , &pDst);
					int k;

					for(k=-nMaskSize; k<=nMaskSize; k++)
					{
						if(y+k>=0 && y+k<size.cy)
							pSource->GetRow(y+k,nPane,&rows[nMaskSize+k]);
						else
							rows[nMaskSize+k]=NULL;
					}

					for(k=-nMaskSize; k<=nMaskSize; k++)   // delaem diagonal
					{
						if(rows[nMaskSize+k])
							rows[nMaskSize+k]+=k;
					}

					for( long x = 0; x<size.cx; x++, pmask++, pSrc++, pDst++ )
					{
						if( *pmask==255 )
						{
							//*pDst = ~*pSrc;
							
							color maximum=*(pSrc);
							for(k=-nMaskSize; k<=nMaskSize; k++)
							{
								color cur;
								if(rows[nMaskSize+k] && x+k>=0 && x+k<size.cx)
									cur=*(rows[nMaskSize+k]);
								 else
									cur=0;
								 if(cur>maximum)
									 maximum=cur;
							}
							*pDst=maximum;
						}
					for(k=-nMaskSize; k<=nMaskSize; k++)
						if(rows[nMaskSize+k]) 
							rows[nMaskSize+k]++;
					}
				Notify( y );
				}
				NextNotificationStage( );
		}

	FinishNotification();
	return TRUE;
	}

	if(nMaskType==MASK_LINE45)
	{
		_ptr_t2<color*> spRows(nMaskSize*2+1);
		color** rows=spRows.ptr;

		for( int nPane=0;nPane<nPaneCount;nPane++ )
		{
			if( !paneCache.IsPaneEnable( nPane ) ) 
			{			
				continue;
			}
			if(IsPaneEnable(nPane))
				for( long y=0;y<size.cy;y++ )
				{
					pSource->GetRowMask( y, &pmask );

					color *pSrc,*pDst;
					pSource->GetRow( y, nPane , &pSrc);
					pResult->GetRow( y, nPane , &pDst);
					int k;

					for(k=-nMaskSize; k<=nMaskSize; k++)
					{
						if(y+k>=0 && y+k<size.cy)
							pSource->GetRow(y+k,nPane,&rows[nMaskSize+k]);
						else
							rows[nMaskSize+k]=NULL;
					}

					for(k=-nMaskSize; k<=nMaskSize; k++)   // delaem diagonal
					{
						if(rows[nMaskSize+k])
							rows[nMaskSize+k]-=k;
					}

					for( long x = 0; x<size.cx; x++, pmask++, pSrc++, pDst++ )
					{
						if( *pmask==255 )
						{
							//*pDst = ~*pSrc;
							
							color maximum=*(pSrc);
							for(k=-nMaskSize; k<=nMaskSize; k++)
							{
								color cur;
								if(rows[nMaskSize+k] && x-k>=0 && x-k<size.cx)
									cur=*(rows[nMaskSize+k]);
								 else
									cur=0;
								 if(cur>maximum)
									 maximum=cur;
							}
							*pDst=maximum;
						}
					for(k=-nMaskSize; k<=nMaskSize; k++)
						if(rows[nMaskSize+k]) 
							rows[nMaskSize+k]++;
					}
					Notify( y );
				}
				NextNotificationStage( );
		}

	FinishNotification();

	return TRUE;
	}

	if(nMaskType==MASK_SQUARE)
	{
		_ptr_t2<color*> spRows(nMaskSize*2+1);
		color** rows=spRows.ptr;

		for( int nPane=0;nPane<nPaneCount;nPane++ )
		{
			if( !paneCache.IsPaneEnable( nPane ) ) 
			{			
				continue;
			}
			if(IsPaneEnable(nPane))
				for( long y=0;y<size.cy;y++ )
				{
					pSource->GetRowMask( y, &pmask );

					color *pSrc,*pDst;
					pSource->GetRow( y, nPane , &pSrc);
					pResult->GetRow( y, nPane , &pDst);
					int k;

					for(k=-nMaskSize; k<=nMaskSize; k++)
					{
						if(y+k>=0 && y+k<size.cy)
							pSource->GetRow(y+k,nPane,&rows[nMaskSize+k]);
						else
							rows[nMaskSize+k]=NULL;
					}

					for( long x = 0; x<size.cx; x++, pmask++, pSrc++, pDst++ )
					{
						if( *pmask==255 )
						{
							//*pDst = ~*pSrc;
							
							color maximum=*pSrc;
							
							for(k=-nMaskSize; k<=nMaskSize; k++)
							for(int j=-nMaskSize; j<=nMaskSize; j++)
							{
								color cur;
								if(rows[nMaskSize+k]!=NULL && x+j>=0 && x+j<size.cx)
									cur=*(rows[nMaskSize+k]+j);
								 else
									cur=0;
								 if(cur>maximum)
									 maximum=cur;
							}
							*pDst=maximum;
						}
						for(k=-nMaskSize; k<=nMaskSize; k++)
							if(rows[nMaskSize+k]!=NULL) 
								rows[nMaskSize+k]++;
					}
					Notify( y );
				}
				NextNotificationStage( );
		}

	FinishNotification();

	return TRUE;
	}

	return false;
}

bool CFilterMorphoBase::IsPaneEnable(int nPane)
{
	return true;
}
