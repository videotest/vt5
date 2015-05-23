#include "stdafx.h"
#include "TopHat.h"
#include "misc_utils.h"
#include <math.h>
#include "alloc.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "ImagePane.h"
#include "PointList.h"

#define MaxColor color(-1)

_ainfo_base::arg        CTopHatInfo::s_pargs[] = 
{       
	{"Mask",      szArgumentTypeInt, "15", true, false },
	{"FindDark",  szArgumentTypeInt, "1", true, false },
	{"Img",       szArgumentTypeImage, 0, true, true },
	{"TopHat",    szArgumentTypeImage, 0, false, true },	
	{0, 0, 0, false, false },
};

/////////////////////////////////////////////////////////////////////////////
static void dilate(color **srcRows, color **dstRows, color **tmpRows, int cx, int cy, int mask)
{
#define CMP max
#include "dilate.inl"
#undef CMP
}

static void erode(color **srcRows, color **dstRows, color **tmpRows, int cx, int cy, int mask)
{
#define CMP min
#include "dilate.inl"
#undef CMP
}

static void smooth(color **srcRows, color **dstRows, int cx, int cy, int mask)
{
	int m = mask;
	int m1 = (mask-1)/2;
	int m2 = mask-m1;
	int mm = m*m;
	int *buf=new int[cx+m];

	int *b=buf+m1;
	for(int ix=0; ix<cx; ix++, b++) 
	{
		*b=0;
		for (int iy=0; iy<m; iy++)
		{
			if (iy-m1<0) 
				*b+=srcRows[0][ix];
			else
				*b+=srcRows[iy-m1][ix];
		}
	}
	for(ix=0; ix<m1; ix++) buf[ix]=buf[m1];
	for(ix=0; ix<m2; ix++) buf[m1+cx+ix]=buf[m1+cx-1];

	for(int iy=0; iy<cy; iy++)
	{
		int sum=0;
		for (int ix=0; ix<m; ix++)	sum+=buf[ix];
		for (ix=0; ix<cx; ix++)
		{
			dstRows[iy][ix] =sum/mm;
			int x1=ix-m1, x2=x1+m;
			if(x1<0) x1=0;
			if(x2>=cx) x2=cx-1;
			sum -= buf[x1+m1];
			sum += buf[x2+m1];
		}

		int y1=iy-m1, y2=y1+m;
		if(y1<0) y1=0;
		if(y2>=cy) y2=cy-1;
		int *b=buf+m1;
		for(ix=0; ix<cx; ix++, b++)
		{
			*b -= srcRows[y1][ix]++;
			*b += srcRows[y2][ix]++;
		}
		for(ix=0; ix<m1; ix++) buf[ix]=buf[m1];
		for(ix=0; ix<m2; ix++) buf[m1+cx+ix]=buf[m1+cx-1];
	}
	delete buf;
}


/////////////////////////////////////////////////////////////////////////////
bool CTopHatFilter::InvokeFilter()
{
	IImage3Ptr	ptrSrcImage( GetDataArgument() );
	IImage3Ptr	ptrDstImage( GetDataResult() );
	
	if(ptrSrcImage == NULL || ptrDstImage == NULL )
		return false;

	long nMask = GetArgLong("Mask");
	nMask = max(3,nMask) | 1; // нечетное число
	long nFindDark=GetArgLong("FindDark");

	ICompatibleObjectPtr ptrCO( ptrDstImage );
	if( ptrCO == NULL ) return false;
	if( S_OK != ptrCO->CreateCompatibleObject( ptrSrcImage, NULL, 0 ) )
	{			
		return false;
	}

	// copy seed to result image
	int nPaneCount = ::GetImagePaneCount( ptrSrcImage );
	int cx,cy;
	ptrSrcImage->GetSize(&cx,&cy);

	StartNotification(cy,nPaneCount);

	smart_alloc(srcRows, color *, cy);
	smart_alloc(dstRows, color *, cy);
	smart_alloc(tmpRows, color *, cy);
	smart_alloc(tmpbuf, color, cx*cy);

	for( int nPane=0;nPane<nPaneCount;nPane++ )
	{
		for(int y=0; y<cy; y++)
		{
			ptrSrcImage->GetRow(y, nPane, &srcRows[y]);
			ptrDstImage->GetRow(y, nPane, &dstRows[y]);
			tmpRows[y]=tmpbuf+cx*y;
		}
		if(nFindDark==2)
		{ // это уже не tophat, просто экспериментальный фильтр для повышения резкости
			smart_alloc(tmpRows2, color *, cy);
			smart_alloc(tmpbuf2, color, cx*cy);
			for(int y=0; y<cy; y++)
				tmpRows2[y]=tmpbuf2+cx*y;

			dilate(srcRows,tmpRows,dstRows, cx,cy,nMask); // tmpRows - результат dilate
			erode (srcRows,tmpRows2,dstRows, cx,cy,nMask); // tmpRows2 - результат erode
			for(int y=0; y<cy; y++)
			{
				for(int x=0; x<cx; x++)
				{
					if( tmpRows[y][x]-srcRows[y][x] <= srcRows[y][x]-tmpRows2[y][x] )
						dstRows[y][x] = tmpRows[y][x];
					else
						dstRows[y][x] = tmpRows2[y][x];
				}
			}
		}
		else if(nFindDark==3)
		{ // тоже не tophat, просто экспериментальный фильтр для повышения резкости
			smart_alloc(tmpRows2, color *, cy);
			smart_alloc(tmpbuf2, color, cx*cy);
			for(int y=0; y<cy; y++)
				tmpRows2[y]=tmpbuf2+cx*y;

			dilate(srcRows,tmpRows,dstRows, cx,cy,nMask); // tmpRows - результат dilate
			//erode(tmpRows,tmpRows,dstRows, cx,cy,nMask); // tmpRows - результат close
			erode (srcRows,tmpRows2,dstRows, cx,cy,nMask); // tmpRows2 - результат erode
			//dilate (tmpRows2,tmpRows2,dstRows, cx,cy,nMask); // tmpRows2 - результат open
			for(int y=0; y<cy; y++)
			{
				for(int x=0; x<cx; x++)
				{
					double d = double(srcRows[y][x]-tmpRows2[y][x])
						/ max(1, tmpRows[y][x]-tmpRows2[y][x]);
					d = d*2-1;
					d = d>=0 ? sqrt(d) : -sqrt(-d);
					d = (d+1)/2;
					dstRows[y][x] = tmpRows2[y][x] + (tmpRows[y][x]-tmpRows2[y][x])*d;
				}
			}
		}
		else if(nFindDark==4)
		{ // Проба алгоритма чистки
			smart_alloc(tmpRows2, color *, cy);
			smart_alloc(tmpbuf2, color, cx*cy);
			for(int y=0; y<cy; y++)
				tmpRows2[y]=tmpbuf2+cx*y;
			smart_alloc(tmpRows3, color *, cy);
			smart_alloc(tmpbuf3, color, cx*cy);
			for(int y=0; y<cy; y++)
				tmpRows3[y]=tmpbuf3+cx*y;
			smooth(srcRows,tmpRows, cx,cy,nMask);
			erode(tmpRows,tmpRows2,dstRows, cx,cy,nMask);
			dilate(tmpRows,tmpRows3,dstRows, cx,cy,nMask);
			for(int y=0; y<cy; y++)
			{
				for(int x=0; x<cx; x++)
				{
					dstRows[y][x] = min(tmpRows3[y][x],max(tmpRows2[y][x],srcRows[y][x]));
				}
			}
		}
		else if(nFindDark==1)
		{
			dilate(srcRows,dstRows,tmpRows, cx,cy,nMask);
			erode (dstRows,dstRows,tmpRows, cx,cy,nMask);
			for(int y=0; y<cy; y++)
			{
				for(int x=0; x<cx; x++)
				{
					dstRows[y][x] -= srcRows[y][x];
				}
			}
		}
		else if(nFindDark==0)
		{
			erode (srcRows,dstRows,tmpRows, cx,cy,nMask);
			dilate(dstRows,dstRows,tmpRows, cx,cy,nMask);
			for(int y=0; y<cy; y++)
			{
				for(int x=0; x<cx; x++)
				{
					dstRows[y][x] = srcRows[y][x] - dstRows[y][x];
				}
			}
		}
		NextNotificationStage();
	}
	FinishNotification();

	return true;
}
