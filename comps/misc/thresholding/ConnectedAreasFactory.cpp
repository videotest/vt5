// ConnectedAreasFactory.cpp: implementation of the CConnectedAreasFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConnectedAreasFactory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConnectedAreasFactory::CConnectedAreasFactory()
{
	areas = 0;
	pMap = 0;
	pPoints = 0;
	m_nAreaCounter = 0;
}

CConnectedAreasFactory::~CConnectedAreasFactory()
{
	if(areas)
		delete [] areas;
	if(pMap)
		delete [] pMap;
	if(pPoints)
		delete [] pPoints;
}

void CConnectedAreasFactory::CreateConnectedArea(int nPane,IImage2* pSource)
{
	SIZE size;
	pSource->GetSize((int*)&size.cx,(int*)&size.cy);

	areas=new CConnectedArea[size.cx*size.cy];
	pMap=new long[size.cx*size.cy];
	FillMemory(pMap,sizeof(long)*size.cx*size.cy,-1);
	pPoints=new POINT[size.cx*size.cy];

	color* pSrc;
	long* pMapCur;
	long x,y;
	long nFilledCount=0;
	long AreaCounter=0;
	
	for( y=0;y<size.cy;y++)
	{
		pSource->GetRow(y,nPane,&pSrc);
		pMapCur=pMap+y*size.cx;
		for( x = 0; x < size.cx; x++, pMapCur++,pSrc++)
		{
			if(*pMapCur==-1) // still not added
			{
				long nCurrentCount = nFilledCount;
				long start=nFilledCount;

				color CurrentColor=*pSrc;
				AddFilledPoint( pSource,nPane,x, y, pMap, nFilledCount, pPoints, size,CurrentColor,AreaCounter);

				while( nCurrentCount < nFilledCount )
				{
					int	xCurrent = pPoints[nCurrentCount].x;
					int	yCurrent = pPoints[nCurrentCount].y;
					//check 8 neightbords
					AddFilledPoint( pSource,nPane,xCurrent-1, yCurrent, pMap, nFilledCount, pPoints, size ,CurrentColor,AreaCounter);
					AddFilledPoint( pSource,nPane,xCurrent+1, yCurrent, pMap, nFilledCount, pPoints, size ,CurrentColor,AreaCounter);
					AddFilledPoint( pSource,nPane,xCurrent, yCurrent-1, pMap, nFilledCount, pPoints, size ,CurrentColor,AreaCounter);
					AddFilledPoint( pSource,nPane,xCurrent, yCurrent+1, pMap, nFilledCount, pPoints, size ,CurrentColor,AreaCounter);
					AddFilledPoint( pSource,nPane,xCurrent-1, yCurrent-1, pMap, nFilledCount, pPoints, size ,CurrentColor,AreaCounter);
					AddFilledPoint( pSource,nPane,xCurrent-1, yCurrent+1, pMap, nFilledCount, pPoints, size ,CurrentColor,AreaCounter);
					AddFilledPoint( pSource,nPane,xCurrent+1, yCurrent-1, pMap, nFilledCount, pPoints, size ,CurrentColor,AreaCounter);
					AddFilledPoint( pSource,nPane,xCurrent+1, yCurrent+1, pMap, nFilledCount, pPoints, size ,CurrentColor,AreaCounter);
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
				AreaCounter++;
			}
	
		} 
	}
	m_nAreaCounter = AreaCounter;
}

void CConnectedAreasFactory::AddFilledPoint(IImage2* pSource, int nPane, long x, long y,  long* pMap, long &nFilledCount, POINT* points, SIZE size, color CurrentColor, long lIndex)
{
	if( x < 0 || y < 0 || x >= size.cx || y >= size.cy )
		return;

	if( *(pMap+y*size.cx+x) != -1 )
		return;

	color temp_color;
	color* ptemp;
	pSource->GetRow(y,nPane,&ptemp);
	temp_color=*(ptemp+x);
	
	if(temp_color!=CurrentColor)
		return;

	*(pMap+y*size.cx+x) = lIndex;
	points[nFilledCount].x = x;
	points[nFilledCount].y = y;
	nFilledCount++;
}
