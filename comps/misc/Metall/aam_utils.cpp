#include "stdafx.h"
#include "image5.h"
#include "aam_utils.h"
#include "alloc.h"
#include <math.h>

double hist_smooth_level=0.5;

void CalcHist(color **src, int cx, int cy, int *hist)
//в hist должно быть NC свободных €чеек
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
//в hist должно быть NC свободных €чеек
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
//в hist должно быть NC свободных €чеек
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

//получить 'rows' дл€ вырезанного куска - левый верхний угол (x0,y0), cy строк
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

// [vanek]
BOOL FormingFRVString( double fkoeff, double *pfFRVchord, int nsize, double fCalibr, CString *pstr_out )
{
    if( !pfFRVchord || !pstr_out)
		return FALSE;

	// прореживание ‘–¬ в соответствии с заданным коэффициентом fkoeff
	int nidxs_count = 0;
	if( fkoeff > 0. )
		nidxs_count = (int)(1. / fkoeff + 0.5);
	else
		return FALSE;

	smart_alloc( idxs, int, nidxs_count );
	::ZeroMemory( idxs, nidxs_count * sizeof( int ) );
	
	int nidx_begin = 0, nidx_end = 0, nidx = 0;
	while( ( nidx_begin < nsize ) && ( nidx < nidxs_count ) )
	{
		double	f0 = pfFRVchord[ nidx_begin ],
				fx = pfFRVchord[ nidx_end ];

		idxs[ nidx ++ ] = nidx_begin;
		do
		{
            fx = pfFRVchord[ nidx_end ++ ];
		}
		while( ( (fx - f0) < fkoeff ) && ( nidx_end < nsize ) );

		nidx_begin = nidx_end;
	}

	// формирование выходной строки
	pstr_out->Empty();
	for( int nidx = 0; nidx < nidxs_count; nidx ++ )
	{
		if( !nidx || (nidx && idxs[nidx] > idxs[nidx - 1]) )
		{
			CString sval( _T("") );
			sval.Format( "%g:%g;", fCalibr * idxs[ nidx ], pfFRVchord[ idxs[ nidx ] ] );
			*pstr_out += sval;
		}      
	}
    
	return TRUE;    
}

BOOL ParseFRVString( CString *pstrFRV, double *pfFRVchord, int *pnsize )
{
	if( !pstrFRV )
		return FALSE;

	if( !pfFRVchord )
	{
		if( !pnsize )
			return FALSE;

		*pnsize = 0;

		int nidx = 0;
		while( (nidx = pstrFRV->Find( _T(";"), ++nidx)) != - 1 )
			(*pnsize)++;	

		(*pnsize) *= 2;
	}
	else
	{
		int nidx_start = 0, nidx_separator = 0, nidx_pair = 0;
		while( (nidx_separator = pstrFRV->Find( _T(";"), nidx_start)) != - 1 )
		{
			int nidx_inner_separator = pstrFRV->Find( _T(":"), nidx_start );
			if( nidx_inner_separator < nidx_separator )
			{
				CString str( _T("") );
				str = pstrFRV->Mid( nidx_start, nidx_inner_separator - nidx_start );
				pfFRVchord[ nidx_pair ] = atof( str );
				str = pstrFRV->Mid( nidx_inner_separator + 1, nidx_separator - (nidx_inner_separator + 1) );
				pfFRVchord[ nidx_pair + 1 ] = atof( str );
				nidx_pair += 2;
			}

			nidx_start = nidx_separator + 1;
		}

	}

    return TRUE;
}
