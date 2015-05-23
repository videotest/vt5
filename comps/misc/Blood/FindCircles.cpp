#include "stdafx.h"
#include "findcircles.h"

#include "misc_utils.h"
#include <math.h>
#include "alloc.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "ImagePane.h"

#define MaxColor color(-1)

_ainfo_base::arg        CFindCirclesInfo::s_pargs[] = 
{       
	{"Radius",      szArgumentTypeInt, "25", true, false },
	{"Points",  szArgumentTypeInt, "16", true, false },
	{"Img",       szArgumentTypeImage, 0, true, true },
	{"FindCircles",    szArgumentTypeImage, 0, false, true },	
	{0, 0, 0, false, false },
};

static inline int iround(double x)
{
	return int(floor(x+0.5));
}

/////////////////////////////////////////////////////////////////////////////
bool CFindCircles::InvokeFilter()
{
	IImage3Ptr	ptrSrcImage( GetDataArgument() );
	IImage3Ptr	ptrDstImage( GetDataResult() );
	
	if(ptrSrcImage == NULL || ptrDstImage == NULL )
		return false;

	long nRadius = GetArgLong("Radius");
	long nPoints=GetArgLong("Points");

	ICompatibleObjectPtr ptrCO( ptrDstImage );
	if( ptrCO == NULL ) return false;
	if( S_OK != ptrCO->CreateCompatibleObject( ptrSrcImage, NULL, 0 ) )
	{			
		return false;
	}

	int nPaneCount = ::GetImagePaneCount( ptrSrcImage );
	int cx,cy;
	ptrSrcImage->GetSize(&cx,&cy);

	smart_alloc(srcRows, color *, cy);
	smart_alloc(dstRows, color *, cy);

	double h=6;
	smart_alloc(dx1,int,nPoints);
	smart_alloc(dy1,int,nPoints);
	smart_alloc(dx2,int,nPoints);
	smart_alloc(dy2,int,nPoints);
	for(int i=0; i<nPoints; i++)
	{
		dx1[i] = iround( cos(i*2*PI/nPoints)*(nRadius+h/2) );
		dy1[i] = iround( sin(i*2*PI/nPoints)*(nRadius+h/2) );
		dx2[i] = iround( cos(i*2*PI/nPoints)*(nRadius-h/2) );
		dy2[i] = iround( sin(i*2*PI/nPoints)*(nRadius-h/2) );
	}

	StartNotification(cy,nPaneCount);
	for( int nPane=0;nPane<nPaneCount;nPane++ )
	{
		for(int y=0; y<cy; y++)
		{
			ptrSrcImage->GetRow(y, nPane, &srcRows[y]);
			ptrDstImage->GetRow(y, nPane, &dstRows[y]);
		}

		for(int y=0; y<cy; y++)
		{
			for(int x=0; x<cx; x++)
			{
				int s = 0;
				for(int i=0; i<nPoints; i++)
				{
					int x1 = x+dx1[i], y1 = y+dy1[i];
					int x2 = x+dx2[i], y2 = y+dy2[i];
					if( x1>=0 && y1>=0 && x1<cx && y1<cy &&
						x2>=0 && y2>=0 && x2<cx && y2<cy )
					{
						int n = srcRows[y1][x1];
						n -= srcRows[y2][x2]; // внешнее минус внутреннее
						//n -= max(srcRows[y2][x2],srcRows[y][x]); // внешнее минус внутреннее
						if(n>0) s += n;
					}
				}
				dstRows[y][x] = color(s/nPoints);
			}
			Notify(y);
		}

		/*
		for(int y=0; y<cy; y++)
		{
			for(int x=0; x<cx; x++)
			{
				dstRows[y][x] = 0;
			}
		}
		for(int y=5; y<cy-5; y++)
		{
			for(int x=5; x<cx-5; x++)
			{
				double gx = srcRows[y][x+3] - srcRows[y][x-3];
				double gy = srcRows[y+3][x] - srcRows[y-3][x];
				double g = _hypot(gx,gy);
				if(g>256)
				{
					int x1 = x - round(gx/g*nRadius);
					int y1 = y - round(gy/g*nRadius);
					if( x1>=0 && y1>=0 && x1<cx && y1<cy )
						dstRows[y1][x1] = min(65535,g+dstRows[y1][x1]/nPoints);
				}
			}
			Notify(y);
		}
		*/

		NextNotificationStage();
	}

	FinishNotification();

	return true;
}
