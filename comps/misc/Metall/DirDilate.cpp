#include "stdafx.h"
#include "DirDilate.h"
#include "math.h"
#include <stdio.h>
#include "measure5.h"
#include "alloc.h"
#include "aam_utils.h"
#include <limits.h>

_ainfo_base::arg CDirDilateInfo::s_pargs[] =
{
	{"DetectorSize",	szArgumentTypeInt, "10", true, false },
	{"Anisotropy",	szArgumentTypeInt, "1", true, false },
	{"BinSrc",	szArgumentTypeBinaryImage, 0, true, true },
	{"DirDilate",	szArgumentTypeBinaryImage, 0, false, true },
	{"DistanceMap",	szArgumentTypeImage, 0, false, true },
	{0, 0, 0, false, false },
};

/////////////////////////////////////////////////////////////////////////////
CDirDilate::CDirDilate()
{
}

//---------------------------------------------------------------------------
CDirDilate::~CDirDilate()
{
}

#define round(x) int(floor(x+0.5))

//---------------------------------------------------------------------------
bool CDirDilate::InvokeFilter()
{
	IBinaryImagePtr pBinSrc( GetDataArgument() );
	IBinaryImagePtr pBinDst( GetDataResult("DirDilate") );
	IImage4Ptr pImgDst( GetDataResult("DistanceMap") );

	int nDetectorSize = GetArgLong("DetectorSize");
	int nAnisotropy = GetArgLong("Anisotropy");

	//int nDetectorSize=10;
	
	if( pBinSrc==NULL || pBinDst==NULL || pImgDst==0 )
		return false;
	
	int cx,cy;
	pBinSrc->GetSizes(&cx,&cy);
	POINT ptOffset;
	pBinSrc->GetOffset(&ptOffset);

	StartNotification( cy, 3 );

	pBinDst->CreateNew(cx, cy);
	pBinDst->SetOffset( ptOffset, TRUE);

	pImgDst->CreateImage(cx, cy, _bstr_t("GRAY"),-1);
	pImgDst->SetOffset(ptOffset, TRUE);

	smart_alloc(srcRows, byte *, cy);
	smart_alloc(dstRows, byte *, cy);
	smart_alloc(distanceRows, color *, cy);
	for(int y=0; y<cy; y++)
	{
		pBinSrc->GetRow(&srcRows[y], y, false);
		pBinDst->GetRow(&dstRows[y], y, false);
		pImgDst->GetRow(y, 0, &distanceRows[y]);
	}

	const int nDirs=64;
	int pdx[nDirs], pdy[nDirs];
	for(int i=0; i<nDirs; i++)
	{
		pdx[i] = round( nDetectorSize*cos(i * PI*2/nDirs) );
		pdy[i] = round( nDetectorSize*sin(i * PI*2/nDirs) );
	}

	for(int y=nDetectorSize; y<cy-nDetectorSize; y++)
	{
		for(int x=nDetectorSize; x<cx-nDetectorSize; x++)
		{
			if(srcRows[y][x])
			{
				if(	1|| !srcRows[y+1][x] ||
					!srcRows[y-1][x] ||
					!srcRows[y][x+1] ||
					!srcRows[y][x-1] ||
					!srcRows[y-1][x-1] ||
					!srcRows[y-1][x+1] ||
					!srcRows[y+1][x-1] ||
					!srcRows[y+1][x+1] )
				{
					int xsum=0, ysum=0, n=0;
					for(int i=0; i<nDirs; i++)
					{
						if(srcRows[y+pdy[i]][x+pdx[i]])
						{
							n++;
							xsum+=pdx[i];
							ysum+=pdy[i];
						}
					}
					if(n>0)
					{
						xsum /= n;
						ysum /= n;
					}
					double r=_hypot(xsum,ysum)/nDetectorSize;
					//r=max(0, r-2/PI)/(1-2/PI);
					dstRows[y][x]=1+int(r*1.9);
					dstRows[y][x]=255;
				}
			}
		}
		Notify(y);
	}

	NextNotificationStage();

	smart_alloc(pfdx,float,cx*cy);
	smart_alloc(pfdy,float,cx*cy);
	smart_alloc(pfr,float,cx*cy);

	for(int y=0; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			if(srcRows[y][x]) distanceRows[y][x] = 0;
			else distanceRows[y][x] = 65535;
			pfdx[y*cx+x]=0;
			pfdy[y*cx+x]=0;
			pfr[y*cx+x]=0;
		}
	}

	for(int y=nDetectorSize; y<cy-nDetectorSize; y++)
	{
		for(int x=nDetectorSize; x<cx-nDetectorSize; x++)
		{
			if(srcRows[y][x])
			{
				if(	!srcRows[y+1][x] ||
					!srcRows[y-1][x] ||
					!srcRows[y][x+1] ||
					!srcRows[y][x-1] ||
					!srcRows[y-1][x-1] ||
					!srcRows[y-1][x+1] ||
					!srcRows[y+1][x-1] ||
					!srcRows[y+1][x+1] )
				{
					dstRows[y][x]=255;
					int xsum=0, ysum=0, n=0;
					for(int i=0; i<nDirs; i++)
					{
						if(srcRows[y+pdy[i]][x+pdx[i]])
						{
							n++;
							xsum+=pdx[i];
							ysum+=pdy[i];
						}
					}
					if(n>0)
					{
						xsum /= n;
						ysum /= n;
					}
					double fdx = double(xsum)/nDetectorSize;
					double fdy = double(ysum)/nDetectorSize;
					double r=_hypot(fdx,fdy);
					if(r>0) { fdx/=r; fdy/=r; }
					r=max(0,(r-0.64)/(1-0.64));
					pfdx[y*cx+x]=-fdx;
					pfdy[y*cx+x]=-fdy;
					pfr[y*cx+x]=r;
				}
			}
		}
		Notify(y);
	}

	NextNotificationStage();

#define CHECKPFR(dx,dy) \
	if(pfr[y*cx+x]!=0.0) \
	{ \
		if(pfr[y*cx+x]<pfr[(y+dy)*cx+x+dx]) \
		{ \
			pfdx[y*cx+x] = pfdx[(y+dy)*cx+x+dx]; \
			pfdy[y*cx+x] = pfdy[(y+dy)*cx+x+dx]; \
			pfr[y*cx+x] = pfr[(y+dy)*cx+x+dx]*0.9999; \
		} \
	}

	for(int y=1; y<cy; y++)
	{
		for(int x=1; x<cx; x++)
		{
			CHECKPFR(-1,-1);
			CHECKPFR(0,-1);
			CHECKPFR(1,-1);
			CHECKPFR(-1,0);
		}
	}
	for(int y=cy-2; y>=0; y--)
	{
		for(int x=cx-2; x>=0; x--)
		{
			CHECKPFR(1,0);
			CHECKPFR(-1,1);
			CHECKPFR(0,1);
			CHECKPFR(1,1);
		}
	}


	for(int y=nDetectorSize; y<cy-nDetectorSize; y++)
	{
		for(int x=nDetectorSize; x<cx-nDetectorSize; x++)
		{
			if(dstRows[y][x])
			{
				double r=pfr[y*cx+x];
				if(	r>0 &&
					r>=pfr[(y+1)*cx+x] &&
					r>=pfr[(y-1)*cx+x] &&
					r>=pfr[y*cx+x+1] &&
					r>=pfr[y*cx+x-1] &&
					r>=pfr[(y-1)*cx+x-1] &&
					r>=pfr[(y-1)*cx+x+1] &&
					r>=pfr[(y+1)*cx+x-1] &&
					r>=pfr[(y+1)*cx+x+1] )
				{
					double fdx = min(1,2*r)*pfdx[y*cx+x];
					double fdy = min(1,2*r)*pfdy[y*cx+x];
					for(int j=1; j<50; j++)
					{
						int x1 = x + round(fdx*j);
						int y1 = y + round(fdy*j);
						if(x1<=0 || x1>=cx-1 || y1<=0 || y1>=cy-1) continue;
						if((j>=2) && srcRows[y1][x1]) break; // дошли до края - стоп
						for(int k=-j/2; k<=j/2; k++)
						{
							int x1 = x + round(fdx*j-fdy*k);
							int y1 = y + round(fdy*j+fdx*k);
							if(x1<=0 || x1>=cx-1 || y1<=0 || y1>=cy-1) continue;
							double f=k*2.0/j;
							f=1-f*f; f=max(f,0.001);
							if(srcRows[y1][x1]==0)
							distanceRows[y1][x1] = min(distanceRows[y1][x1], j*1024/f/nAnisotropy);
						}
					}
				}
			}
			else
			{
				pfdx[y*cx+x]=0.0;
				pfdy[y*cx+x]=0.0;
				pfr[y*cx+x]=0.0;
			}
		}
		Notify(y);
	}

#define DIFF(dx,dy,step) distanceRows[y][x] = min(distanceRows[y][x], distanceRows[y+dy][x+dx]+step)

	for(int y=1; y<cy-1; y++)
	{
		for(int x=1; x<cx-1; x++)
		{
			DIFF(-1,0,1024*2);
			DIFF(0,-1,1000*2);
			DIFF(-1,-1,1448*2);
			DIFF(-1,1,1448*2);
		}
	}
	for(int y=cy-2; y>0; y--)
	{
		for(int x=cx-2; x>0; x--)
		{
			DIFF(1,0,1024*2);
			DIFF(0,1,1024*2);
			DIFF(1,-1,1448*2);
			DIFF(1,1,1448*2);
		}
	}

	FinishNotification();
	return true;
}
