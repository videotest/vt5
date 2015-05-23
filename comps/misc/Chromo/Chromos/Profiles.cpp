#include "StdAfx.h"
#include "Cromos.h"
#include "ChromParam.h"
#include "ChildObjects.h"
#include "PUtils.h"
#include "Settings.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*
 * compute estimate of slope of polygon vertices between lf and lr
 * using a gradient-finding convolution independently in X and Y.
 * Express as 256*sin, 256*cos of slope.
 */
static void axisslope(POINT *p, int lf, int lr, int *sinsl, int *cossl)
{
	POINT *pt1, *pt2;
	int i,s,c,sscc;
	s = 0;
	c = 0;
	pt1 = p + lf - 1;
	pt2 = p + lr - 1;
	lf = (lr-lf+1)>>1;
	for (i=0; i<lf; i++)
	{
		c += pt2->y - pt1->y;
		s += pt1->x - pt2->x;
		pt2--;
		pt1++;
	}
	sscc = int(8*sqrt(double(s*s + c*c)));
	if (sscc == 0)
	{
		if (s > c)
		{
			*sinsl = 256;
			*cossl = 0;
		}
		else
		{
			*sinsl = 0;
			*cossl = 256;
		}
	}
	else
	{
		*sinsl = (s<<11)/sscc;
		*cossl = (c<<11)/sscc;
	}
}

#define HALF_SLOPE_NHD	5

static void setnextpoint(POINT *p, int nPoints, int *xx, int *yy, int *sini, int *cosi, int sln)
{
	int int1,int2;
//	fv = p->vtx + sln - 1;
	*xx = p[sln-1].x;
	*yy = p[sln-1].y;
	int1 = sln-HALF_SLOPE_NHD;
	int2 = sln+HALF_SLOPE_NHD;
	if (int1 < 1)
	{
		int1=1;
		int2++;
	}
	if (int2 > nPoints)
	{
		int2 = nPoints;
		if (int1 > 1)
			int1--;
	}
	axisslope(p,int1,int2,sini,cosi);
}

static void profslice(CImageWrp &pImgWrp, CIntervalImage *pIntvl,int x2, int y2, int sin2, int cos2, int halfwidth, color *g)
{
	int i;
	int xxx,yyy,xinc,yinc;
	int ix3,iy3,ixfrac,iyfrac;
	int ixx,iyy,jxx,jyy;
	color g4[4];
//	POINT ptOffs = pImgWrp.GetOffset();
	ix3= x2 >> 3;
	iy3= y2 >> 3;
	// fractional remainder, out of 256
	ixfrac = (x2<<5) - (ix3<<8);
	iyfrac = (y2<<5) - (iy3<<8);
	_CGrayVal gval(pImgWrp,pIntvl);
	// everything will now be done centered round ix3,iy3.
	for (i = -halfwidth; i <= halfwidth; i++)
	{
		// coordinates, minus ix3, iy3 respectively, *256
		xxx = ixfrac+i*cos2;
		yyy = iyfrac+i*sin2;
		ixx = xxx>>8;
		iyy = yyy>>8;
		xinc = xxx-(ixx<<8);
		if (xinc < 0)
		{
			xinc += 256;
			ixx--;
		}
		yinc = yyy-(iyy<<8);
		if (yinc < 0)
		{
			yinc += 256;
			iyy--;
		}
		// xinc, xbar, yinc, ybar should now all be non-negative
		jxx = ix3+ixx;
		jyy = iy3+iyy;
		gval.Gray4Val(jyy,jxx,g4);
		// interpolated grey value
		*g++ = interp(g4,yinc,xinc);
	}
}

/*
 * make multiple profiles as specified in array "moments", which should be
 * monotonic increasing, minimum value >= -1.
 * This saves considerable time in the extraction of slices perpendicular
 * to medial axis.  The maximum number of simultaneous profiles is MAXSPROF.
 *
 * "moment" selects profile type :
 *	-2	max density
 *	-1	mean density (width defined as sum of non-zero points)
 *	 0	integrated density
 *	 1	first moment (sigma |md| / sigma m) (cf. centroid)
 *	 2	second moment (sigma mdd / sigma m) (radius of gyration)
 *
 * The mid-points polygon is assumed to be integer, scaled up by a factor
 * of 8, to avoid floating point wherever possible.  Similarly, sintheta
 * and costheta are represented by numbers in [-256,256].
 */
#define MAXSPROF	5
#define MAXHALFWIDTH	80
void multiprofiles(CImageWrp &pImgWrp, CIntervalImage *pIntvl, POINT *pAxisPts, int nAxisPts, int nArea, int **prof, int *moments, int nmoments)
{
	register int d,g,i,sum,maxg;
	int j, k, mlines, sline, sumd[MAXSPROF], sump, sumq, mom;
	int halfwidth, *hv[MAXSPROF];
	color rbuf[2*MAXHALFWIDTH+1];
	int x,y,sintheta,costheta;

	/*
	 * approximate 1.5 mean width of object
	 * to provide a limiting distance from axis for profile extraction
	 * Safeguard against a duff polygon
	 */
	if ((mlines = nAxisPts) != 0)//if ((mlines = p->nvertices) != 0)
	{
	
		halfwidth = 2*nArea;
		halfwidth /= (3*mlines);
		if (halfwidth > MAXHALFWIDTH)
			halfwidth = MAXHALFWIDTH;
		if (halfwidth > (int)g_MaxChromoWidth)
			halfwidth = g_MaxChromoWidth;
		/*
	 	 * a long narrow object could obtain halfwidth of zero.  This will
	 	 * subsequently give a zero profile of this object.
	 	 */
		if (halfwidth == 0)
			halfwidth = 1;
	}
	for (i=0; i < nmoments; i++)
	{
		hv[i] = prof[i];
	}
	for (sline = 1; sline <= mlines; sline++)
	{
		/*
	 	* get next column, line (x,y) in unit-spaced steps
	 	* along smoothed mid-points polygon, and local orientation.
	 	* sline is line number in straightened object
	 	*/
		setnextpoint(pAxisPts,nAxisPts,&x,&y,&sintheta,&costheta,sline);
		/*
		 * rotate into buffer.
		 */
		profslice(pImgWrp,pIntvl,x,y,sintheta,costheta,halfwidth,rbuf);
		/*
		 * compute density profile value
		 */
		maxg = 0;
		sum = 0;
		sump = 0;
		for (i = 0; i < nmoments; i++)
			sumd[i] = 0;
		for (k = 0; k <= halfwidth*2; k++)
		{
			g = rbuf[k];
			if (g > maxg)
				maxg = g;
			sum += g;
			if (g > 0)
				sump++;
			mom = g;
			d = halfwidth - k;
			if (d < 0)
				d = -d;
			i = 0;
			for (j=0; j<nmoments; j++)
			{
				for (; i<moments[j]; i++)
					mom *= d;
				sumd[j] += mom;
			}
		}
		sumq = sum;
		if (moments[0] == -1 && sump > 0)
			sum /= sump;
		for (i=0; i < nmoments; i++)
		{
			*hv[i] = (moments[i] == -2? maxg: (moments[i] <= 0? sum: (sumq==0? 0: 10*sumd[i]/sumq)));
			hv[i]++;
		}
	}
	return;
}

/*
 * using density profile, shorten tips of mid points polygon,
 * density profile and moment profile
 * until within chromosome body (density at least 1% of peak).
 */
void shorten(int *prof, int l, int *beg, int *end)
{
	int i,maxh;
	int *g = prof;
	maxh = 0;
	for (i = 0; i < l; i++)
	{
		if (*g > maxh)
			maxh = *g;
		g++;
	}
	maxh /= 100;
	*end = l;
	while (*--g < maxh)
	{
		(*end)--;
	}
	g = prof;
	*beg = 0;
	while (*g < maxh)
	{
		g++;
		(*beg)++;
	}
}

/*
 * smooth a histogram by applying a low pass filter.
 * But preserve the end points.
 * protection added to check supplied object. if probelm found then this will
 * casue an immediate return. The routine should cope with the effects of
 * an empty domain or iterations of <1 .  bdp 7/5/87
 */
void histosmooth(int *hv, int npoints, int iterations)
{
	int i;
	int *iv;
	int k, kwas, n;
	/*
	 * iterate 1-2-1 smoothing, leaving tips alone
	 */
	for (n=0; n<iterations; n++)
	{
		iv = hv;
		kwas = *iv;
		*iv = kwas * 4;
		iv++;
		for (i=2; i < npoints; i++)
		{
			k = *iv;
			*iv = kwas + 2 * k + *(iv+1);
			iv++;
			kwas = k;
		}
		*iv = *iv * 4;
	}
	/*
	 * divide vertex values by 4 ** iterations
	 */
	k = 4;
	for (n = 1; n < iterations; n++)
		k = k*4;
	iv = hv;
	for (i = 0; i < npoints; i++)
	{
		*iv = *iv/k;
		iv++;
	}
}


#define PROFSMOOTH 1
static int whichprof[2] = {0,2};

void CChromParam::CalcProfiles()
{
	int *anProfs[2];
	m_Prof0.Init(IntegerProfile, m_LongAxis.m_nSize);
	m_Prof2.Init(IntegerProfile, m_LongAxis.m_nSize);
	anProfs[0] = m_Prof0.m_pnProf;
	anProfs[1] = m_Prof2.m_pnProf;
	ASSERT(CPoint(m_LongAxis.m_ptOffset) == CPoint(0,0));
	multiprofiles(pImgWrp, pIntvl, m_LongAxis.m_iptAxis, m_LongAxis.m_nSize, (int)dArea, anProfs, whichprof, 2);
	if (bOk) bOk = wo->Compare(COMPARE_PROFILE0, m_Prof0.m_nSize, m_Prof0.m_pnProf)==S_OK;
	if (bOk) bOk = wo->Compare(COMPARE_PROFILE1, m_Prof2.m_nSize, m_Prof2.m_pnProf)==S_OK;
	int beg,end;
	shorten(m_Prof0.m_pnProf, m_Prof0.m_nSize, &beg, &end);
	m_LongAxis.Shorten(beg,end);
//	BOOL bLongAxisManual;
//	long nLongAxisSize;
//	m_sptrChromos->GetAxisSize(AT_LongAxis, &nLongAxisSize, &bLongAxisManual);
//	if (!bLongAxisManual) // If manual long axis exist, do not change it.
	{
		CChromoAxis LongAxis(m_LongAxis); // this axis will be saved in child object
		LongAxis.Convert(FloatAxis);
		LongAxis.m_ptOffset = CPoint(0,0);
		LongAxis.Offset(CPoint(-pImgWrp.GetOffset().x,-pImgWrp.GetOffset().y));
		m_sptrChromos->SetAxis2(bRotated?AT_LongAxis|AT_Rotated:AT_LongAxis, &LongAxis, FALSE);
	}
	m_Prof0.Shorten(beg,end);
	m_Prof2.Shorten(beg,end);
	m_Prof2.m_pnProf[0] = m_Prof2.m_pnProf[m_Prof2.m_nSize-1] = 0; 
	histosmooth(m_Prof0.m_pnProf, m_Prof0.m_nSize, PROFSMOOTH);
	histosmooth(m_Prof2.m_pnProf, m_Prof2.m_nSize, PROFSMOOTH+2);
	if (bOk) bOk = wo->Compare(COMPARE_SPROFILE0, m_Prof0.m_nSize, m_Prof0.m_pnProf)==S_OK;
	if (bOk) bOk = wo->Compare(COMPARE_SPROFILE1, m_Prof2.m_nSize, m_Prof2.m_pnProf)==S_OK;
}



