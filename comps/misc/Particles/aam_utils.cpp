#include "stdafx.h"
#include "image5.h"
#include "aam_utils.h"

void CalcHist(color **src, int cx, int cy, int *hist)
//в hist должно быть NC свободных ячеек
{
	ZeroMemory(hist, NC*sizeof(int));
	for(int y=0; y<cy; y++)
	{
		color *pSrc=src[y];
		for(int x=0; x<cx; x++)
		{
			hist[(*pSrc++)/(65536/NC)]++;
		}
	}
	for(int i=1; i<NC; i++)
		hist[i]=hist[i]+int((hist[i-1]-hist[i])*hist_smooth_level);
	for(i=NC-2; i>=0; i--)
		hist[i]=hist[i]+int((hist[i+1]-hist[i])*hist_smooth_level);
}

void CalcHistMasked(color **src, byte **bin, byte bin_val, int cx, int cy, int *hist)
//в hist должно быть NC свободных ячеек
{
	ZeroMemory(hist, NC*sizeof(int));
	for(int y=0; y<cy; y++)
	{
		color *pSrc=src[y];
		for(int x=0; x<cx; x++)
		{
			if(bin[y][x]==bin_val) hist[(*pSrc++)/(65536/NC)]++;
		}
	}
	for(int i=1; i<NC; i++)
		hist[i]=hist[i]+int((hist[i-1]-hist[i])*hist_smooth_level);
	for(i=NC-2; i>=0; i--)
		hist[i]=hist[i]+int((hist[i+1]-hist[i])*hist_smooth_level);
}

void CalcHistMaskedSqr(color **src, byte **bin, byte bin_val, int cx, int cy, int *hist, int x0, int y0, int x1, int y1)
//в hist должно быть NC свободных ячеек
{
	ZeroMemory(hist, NC*sizeof(int));
	for(int y=y0; y<y1; y++)
	{
		for(int x=x0; x<x1; x++)
		{
			if(bin[y][x]==bin_val) hist[(src[y][x])/(65536/NC)]++;
		}
	}
	for(int i=1; i<NC; i++)
		hist[i]=hist[i]+int((hist[i-1]-hist[i])*hist_smooth_level);
	for(i=NC-2; i>=0; i--)
		hist[i]=hist[i]+int((hist[i+1]-hist[i])*hist_smooth_level);
}

//получить 'rows' для вырезанного куска - левый верхний угол (x0,y0), cy строк
//{for(int y=0; y<cy; y++) clip[y]=src[y+y0]+x0;}

int OrderValue(int *x, int n, int order, int xmin, int xmax)
{
	while(xmax-xmin>1)
	{
		int xmid=(xmax+xmin)/2;
		int count=0;
		for(int i=0; i<n; i++)
		{
			if(x[i]<=xmid) count++;
		}
		if(count<order) xmin=xmid;
		else xmax=xmid;
	}
	return(xmin);
}

