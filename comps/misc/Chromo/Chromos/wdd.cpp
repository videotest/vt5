#include "stdafx.h"
#include "ChromParam.h"
#include "PUtils.h"
#include "ChromoSubs.h"
#include "Settings.h"
 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// coefficient of variation (standard deviation / mean) of profile.
double _cvdd(int *h, int l)
{
	int s = 0,ss = 0,ll = l;
	for (; l>0; l--)
	{
		s += *h;
		ss += *h * *h;
		h++;
	}
	if (g_FloatArith == 1)
	{
		double ss1 = ss, s1 = s;
		if (s == 0) // safeguard against zero profile
			return 0.;
		else
			return sqrt(ll*ss1-s1*s1)/double(s1);
	}
	else
	{
		ss >>= 8;  s >>= 4;	// prevent overflow 
		if(s == 0)	// safeguard against zero profile
			return (0.);
		else
			return (1024 * isqrt(ll*ss - s*s) / s);
	}
}

// Granum's NSSD of profile -
// "normalised root of the sum of squared density differences"
double _nssd(int *h, int l)
{
	int ll=l,s=0,ss,sd=0;
	for (; l>0; l--)
	{
		s += *h;
		ss = *h++;
		ss -= *h;
		sd += ss*ss;
	}
	if(s == 0)			// Safeguard against zero profile
		return(0);
	else if (g_FloatArith == 1)
		return sqrt(double(sd))/s;
	else
		return(1024 * isqrt8(sd) / s);
}

// v returns the wdd values
void _wdd(int *hdom, int npoints, short *v)
{
	int p,i,l,u,*h,*g;
	int ll,td,w;
	int wdd1,wdd2,wdd3,wdd4,wdd5,wdd6;
	l = npoints;
	ll = l*2;
	// wdd2 - take care with central point if odd number of points
	// wdd5 - derive from wdd4 computation
	h = hdom;
	g = h+l-1;
	td = wdd1 = wdd2 = wdd3 = wdd4 = wdd5 = wdd6 = 0;
	for (i=1; i<l; i+=2)
	{
		w = *h - *g;
		u = *h++ + *g--;
		td += u;
		p = 3072*i/l;
		wdd1 += w * (p - 3072);
		wdd2 += u * (p - 1536);
		if (i*3 <= ll)
		{
			wdd3 += w * (p - 1024);
		}
		else
		{
			wdd3 += w * (3072 - p);
		}
		if (i*4 <= ll)
		{
			wdd4 += u * (p - 768);
			wdd5 += w * (p - 768);
		}
		else
		{
			wdd4 += u * (2304 - p);
			wdd5 += w * (2304 - p);
		}
		if (i*6 <= ll)
			wdd6 += u * (p - 512);
		else if (i*3 > ll)
			wdd6 += u * (p - 2560);
		else
			wdd6 += u * (1536 - p);
	}
	// convert back to range [-256,256]
	wdd1 /= 12;
	wdd2 /= 6;
	wdd3 /= 4;
	wdd4 /= 3;
	wdd5 /= 3;
	wdd6 /= 2;
	// If an odd number of points in histogram, deal suitable with middle
	if (l&1 == 1)
	{
		td += *h;
		u = *h * 256;
		wdd2 += u;
		wdd4 -= u;
		wdd6 += u;
	}
	// normalise by profile total density
	// it is possible for 1 pixel wide objects that the profile is
	// zero everywhere.  Safeguard here against zero division
	td /= 10;
	if(td == 0)
	{
		*v = 0;
		*++v = 0;
		*++v = 0;
		*++v = 0;
		*++v = 0;
		*++v = 0;
	}
	else
	{
		*v = wdd1/td;
		*++v = wdd2/td;
		*++v = wdd3/td;
		*++v = wdd4/td;
		*++v = wdd5/td;
		*++v = wdd6/td;
	}
}

int *gradprof(int *hv, int npoints, int *pnpoints1)
{
	int i, v, *h, *g, *gv;
	*pnpoints1 = npoints-1;
	gv = new int[npoints-1];
	memset(gv, 0, (npoints-1)*sizeof(int));
	h = hv;
	g = gv;
	for (i=0; i<*pnpoints1; i++)
	{
		v = *h++;
		v -= *h;
		if (v<0)
			v = -v;
		*g++ = v;
	}
	return gv;
}


