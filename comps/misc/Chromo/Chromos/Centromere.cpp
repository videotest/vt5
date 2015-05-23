// Centromere.cpp: implementation of the centromere calculation.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Cromos.h"
#include "ChromParam.h"
#include "math.h"
#include "resource.h"
#include "ChildObjects.h"
#include "ChromoSubs.h"
#include "Settings.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#if defined(_DEBUG)
extern int nObj;
#endif

#define	TEST_SUBSEGMENT	10

BOOL GetPointDirVT4( CImageWrp& ImgWrp,  double oX, double oY,	 double fAngle, CPoint &pt )
{	
	double	dx, dy;
	int	iRow, iCol;//, iCol4;
//	BYTE	*pImage;
	color	ucPixel;
	BYTE* colStr;
	double X,Y;
	int iX,iY;
	X=oX;
	Y=oY;
	iRow = ImgWrp.GetHeight();
	iCol = ImgWrp.GetWidth();
	dx =::cos(fAngle);
	dy =::sin(fAngle);
	iX = int(X+0.5);
	iY = int(Y+0.5);
	if( iX < 0 || iY < 0 || iX >= iCol-1 || iY >= iRow-1 )
		return FALSE;
	colStr = ImgWrp.GetRowMask(iY);
	ucPixel = colStr[iX];
	if( !ucPixel )
		return FALSE;
	while (1)
	{                  
		X+=dx;         
		Y+=dy;
		if(X<0||Y<0||X>iCol-1||Y>iRow-1)
			return FALSE;
		iX = int(X+0.5);
		iY = int(Y+0.5);
//		if(iX<0||iY<0||iX>iCol-1||iY>iRow-1)
//			return FALSE;
		colStr = ImgWrp.GetRowMask(iY);
		ucPixel = colStr[iX];
		if(ucPixel==0)	
		{
			pt.x = int(X+0.5);
			pt.y = int(Y+0.5);
			return TRUE;
		}
	}
	AfxMessageBox("Error GetPointDir ");
	return FALSE;
}

double _xcoord(double x, double y, double a)
{
	return x*cos(a)+y*sin(a);
}

#define PEMAX	30		/* maximum number of extrema in a profile */
struct profextremum {
	int pos;	/* position */
	int minmax;	/* 1 if max, -1 if min */
	int val;	/* profile value */
};

static int *mirrorprof(int *p, int npoints, int *o, int *e, int *np)
{
//	struct object *np, *makemain();
//	struct histogramdomain *hdom, *ndom, *makehistodmn();
	int i,ovlap,n,*hv,*nv;

//	hdom = (struct histogramdomain *) p->idom;
	ovlap = npoints >> 2;
	if (ovlap > 8)
		ovlap = 8;
	*o = npoints - ovlap;
	*e = *o + npoints - 1;
	int npoints1 = 3*npoints - 2*ovlap;
	int *ndom = new int[npoints1];
	memset(ndom, 0, npoints1*sizeof(int));
//	ndom = makehistodmn(1,3*hdom->npoints - 2*ovlap);
//	ndom->l = hdom->l - *o;
//	ndom->k = hdom->k;
//	ndom->r = hdom->r;
//	np = makemain(13,ndom,0,0,p);
	n = npoints;
	hv = p;
	nv = ndom + *o;
	for (i = 0; i < n; i++)
		*nv++ = *hv++;
	hv = p;
	nv = ndom + *o + ovlap;
	for (i = 0; i < n; i++)
		*nv-- += *hv++;
	hv = p;
	nv = ndom + npoints1 - 1;
	for (i = 0; i < n; i++)
		*nv-- += *hv++;
	/*
	 * increment the profile outside original bounds
	 * to ensure that minima near ends are within original bounds
	 */
	ovlap >>= 1;
	n = *o+ovlap;
	nv = ndom;
	for (i = 0; i < n; i++)
		(*(nv++))++;
	n = npoints1;
	nv = ndom + *e - ovlap;
	for (i = *e - ovlap; i < n; i++)
		(*(nv++))++;

	*np = npoints1;
	return ndom;
}


// Piper-style centromere location
static int cmerelocation(int *prof, int npoints, int quantum, int mirror)
{
	int *hv;
	int i,m,ne,max;
	int curmin, curmax, pt, orig, end;
	struct profextremum pe[PEMAX];
	struct profextremum *PE;
	if (mirror)
	{
		prof = mirrorprof(prof,npoints,&orig,&end,&npoints);
	}
	else
	{
		orig = 0;
		end = npoints;
	}
	/*
	 * find max value and use to determine quantisation of profile
	 */
	hv = prof;
	max = 0;
	for (i = 0; i < npoints; i++)
	{
		if (*hv > max)
			max = *hv;
		hv++;
	}
	quantum = max/quantum;
	/*
	 * search profile for extrema
	 */
	m = 1;
	ne = 0;
	curmax = 0;
	hv = prof;
	PE = pe;
	for (i=0; i < npoints && ne < PEMAX; i++)
	{
		switch (m)
		{
		case 1:			/* looking for maximum */
			if (*hv > curmax) {
				curmax = *hv;
				pt = i;
			}
			else if (*hv < curmax - quantum)
			{
				PE->pos = pt;
				PE->minmax = m;
				PE->val = curmax;
				m = -m;
				ne++;
				PE++;
				curmin = *hv;
				pt = i;
			}
			break;
		case -1:		/* looking for minimum */
			if (*hv < curmin)
			{
				curmin = *hv;
				pt = i;
			}
			else if (*hv > curmin + quantum)
			{
				PE->pos = pt;
				PE->minmax = m;
				PE->val = curmin;
				m = -m;
				ne++;
				PE++;
				curmax = *hv;
				pt = i;
			}
			break;
		}
		hv++;
	}
	if (mirror)
		delete prof;
	/*
	 * search extremum list for lowest minimum in original profile length,
	 * return -1 otherwise
	 */
	curmin = max;
	PE = pe;
	for (i=0; i<ne; i++)
	{
		if (PE->minmax < 0)
			if (PE->val < curmin && PE->pos >= orig && PE->pos <= end)
			{
				curmin = PE->val;
				pt = PE->pos;
			}
		PE++;
	}
	if (curmin == max)
		return(-1);
	else
		return(pt-orig);
}

static int ncentromere(POINT *ptaxis, int *prof, int npoints, POINT &pt)
{
	int ipt = cmerelocation(prof, npoints, 10, 1);
	if (ipt == -1)
		pt = INVPOINT;
	else
	{
		pt.x = (ptaxis[ipt].x+4)>>3;
		pt.y = (ptaxis[ipt].y+4)>>3;
	}
	return ipt;
};

/*bool _calcdrawpoints(int nCen, CPoint &pt1, CPoint &pt2, CChromoAxis &LongAxis, CImageWrp &pImgWrp)
{
	int nShf = 3;
	int n1 = max(nCen-nShf,0);
	int n2 = min(nCen+nShf,LongAxis.m_nSize-1);
	FPOINT fpt0,fpt1,fpt2;
	if (LongAxis.m_Type == IntegerAxis && LongAxis.m_nScale == 8)
	{
		fpt0.x = (LongAxis.m_iptAxis[nCen].x+4)>>3;
		fpt0.y = (LongAxis.m_iptAxis[nCen].y+4)>>3;
		fpt1.x = (LongAxis.m_iptAxis[n1].x+4)>>3;
		fpt1.y = (LongAxis.m_iptAxis[n1].y+4)>>3;
		fpt2.x = (LongAxis.m_iptAxis[n2].x+4)>>3;
		fpt2.y = (LongAxis.m_iptAxis[n2].y+4)>>3;
	}
	else
	{
		fpt0 = LongAxis.GetFPoint(nCen);
		fpt1 = LongAxis.GetFPoint(n1);
		fpt2 = LongAxis.GetFPoint(n2);
	}
	double fAlpha = ::atan2(fpt2.y-fpt1.y, fpt2.x-fpt1.x);
	if (!GetPointDir(pImgWrp, fpt0.x-pImgWrp.GetOffset().x, fpt0.y-pImgWrp.GetOffset().y,
		fAlpha+PI/2, pt1))
		return false;
	if (!GetPointDir(pImgWrp, fpt0.x-pImgWrp.GetOffset().x, fpt0.y-pImgWrp.GetOffset().y,
		fAlpha-PI/2, pt2))
		return false;
	pt1 += pImgWrp.GetOffset();
	pt2 += pImgWrp.GetOffset();
	return true;
}*/

bool _calcdrawpoints(CENTROMERE *pCen, CChromoAxis *pLongAxis, CImageWrp &pImgWrp)
{
	int dx,dy;
	double fAlpha;
	FPOINT fpt0,fpt1,fpt2;
	if (pImgWrp.IsEmpty()) return false;
	if (pLongAxis)
	{
		if (pLongAxis->GetType() != IntegerAxis && pLongAxis->GetType() != FloatAxis)
			return false;
		int nShf = 3;
		int nCen = pCen->nCentromere;
		int n1 = max(nCen-nShf,0);
		int n2 = min(nCen+nShf,(int)pLongAxis->m_nSize-1);
		if (pLongAxis->m_Type == IntegerAxis && pLongAxis->m_nScale == 8)
		{
			fpt0.x = (pLongAxis->m_iptAxis[nCen].x+4)>>3;
			fpt0.y = (pLongAxis->m_iptAxis[nCen].y+4)>>3;
			fpt1.x = (pLongAxis->m_iptAxis[n1].x+4)>>3;
			fpt1.y = (pLongAxis->m_iptAxis[n1].y+4)>>3;
			fpt2.x = (pLongAxis->m_iptAxis[n2].x+4)>>3;
			fpt2.y = (pLongAxis->m_iptAxis[n2].y+4)>>3;
		}
		else
		{
			fpt0 = pLongAxis->GetFPoint(nCen);
			fpt1 = pLongAxis->GetFPoint(n1);
			fpt2 = pLongAxis->GetFPoint(n2);
		}
		dx = pImgWrp.GetOffset().x-pLongAxis->m_ptOffset.x;
		dy = pImgWrp.GetOffset().y-pLongAxis->m_ptOffset.y;
		fAlpha = ::atan2(fpt2.y-fpt1.y, fpt2.x-fpt1.x);
		if (IsInvAngle(pCen->dAngle))
			pCen->dAngle = fAlpha;
	}
	else
	{
		fAlpha = pCen->dAngle;
		fpt0 = pCen->ptCen;
		dx = pImgWrp.GetOffset().x-pCen->ptOffset.x;
		dy = pImgWrp.GetOffset().y-pCen->ptOffset.y;
	}
	CPoint pt1,pt2;
	if (!GetPointDir(pImgWrp, fpt0.x-dx, fpt0.y-dy, fAlpha+PI/2, pt1))
		return false;
	if (!GetPointDir(pImgWrp, fpt0.x-dx, fpt0.y-dy,	fAlpha-PI/2, pt2))
		return false;
	pt1 += CPoint(dx,dy);
	pt2 += CPoint(dx,dy);
	if (IsInvPoint(pCen->aptCenDraw[0])) pCen->aptCenDraw[0] = CFPoint(pt1.x+0.5,pt1.y+0.5);
	if (IsInvPoint(pCen->aptCenDraw[1])) pCen->aptCenDraw[1] = CFPoint(pt2.x+0.5,pt2.y+0.5);
	return true;
}

int _FindCentromereByPoint(CFPoint ptCen, CChromoAxis &LongAxis)
{
	int nSize = LongAxis.GetSize();
	int cmere = 0;
	if (LongAxis.GetType() != IntegerAxis && LongAxis.GetType() != FloatAxis)
		return -1;
	CFPoint fpt = LongAxis.GetFPoint(0);
	double dMinDist = sqrt((ptCen.x-fpt.x)*(ptCen.x-fpt.x)+(ptCen.y-fpt.y)*(ptCen.y-fpt.y));
	for (int i = 0; i < nSize; i++)
	{
		CFPoint fpt = LongAxis.GetFPoint(i);
		double dDist = sqrt((ptCen.x-fpt.x)*(ptCen.x-fpt.x)+(ptCen.y-fpt.y)*(ptCen.y-fpt.y));
		if (dDist < dMinDist)
		{
			dMinDist = dDist;
			cmere = i;
		}
	}
	return cmere;
}


int _FindCentromereByPoint(CPoint ptCentromere, int nSize, POINT *ppt, int nScale)
{
//	ptCentromere.x *= 8;
//	ptCentromere.y *= 8;
	double dScale = nScale<=0?nScale:1./nScale;
	double dMinDist = sqrt((ppt[0].x*dScale-ptCentromere.x)*(ppt[0].x*dScale-ptCentromere.x)+
		(ppt[0].y*dScale-ptCentromere.y)*(ppt[0].y*dScale-ptCentromere.y));
	int iMinPos = 0;
	for (int i = 1; i < nSize; i++)
	{
		double dDist = sqrt((ppt[i].x*dScale-ptCentromere.x)*(ppt[i].x*dScale-ptCentromere.x)+
			(ppt[i].y*dScale-ptCentromere.y)*(ppt[i].y*dScale-ptCentromere.y));
		if (dDist < dMinDist)
		{
			dMinDist = dDist;
			iMinPos = i;
		}
	}
	return iMinPos;
}

//#define _SDEBUG

void CChromParam::CalcCentromere()
{
	BOOL bCenManual;
	CPoint ptCenManual;
//	m_sptrChromos->GetCentromereCoord(bRotated?AT_Rotated:0, &ptCenManual, NULL, &bCenManual);
	m_sptrChromos->GetCentromereCoord(0, &ptCenManual, NULL, &bCenManual);
	if (bCenManual)
	{
		CPoint ptOffs(pImgWrpOrig.GetOffset());
		ptCentromere = CPoint(ptCenManual.x+ptOffs.x, ptCenManual.y+ptOffs.y);
		if (bRotated)
			ptCentromere = rotatepoint(m_ptCenter, ptCentromere, dAngle);
		nCmerePos = _FindCentromereByPoint(ptCentromere, m_LongAxis.m_nSize, m_LongAxis.m_iptAxis,
			m_LongAxis.m_nScale);
		dLenghtCI = double(nCmerePos)/double(m_LongAxis.m_nSize);
		bPolarity = nCmerePos<=(int)m_LongAxis.m_nSize/2;
		return;
	}
	CPoint ptOffs(pImgWrp.GetOffset());
	int iMin = -1;
	int nAxisPts1 = m_LongAxis.m_nSize;
	int nAxisNodes = m_ShortAxis.m_nSize;
	FPOINT *pAxisNodes = new FPOINT[nAxisNodes];
	for (int i = 0; i < nAxisNodes; i++)
	{
		pAxisNodes[i] = m_ShortAxis.GetFPoint(i);
		pAxisNodes[i].x = pAxisNodes[i].x-pImgWrp.GetOffset().x;
		pAxisNodes[i].y = pAxisNodes[i].y-pImgWrp.GetOffset().y;
	}
	ptCentromere = INVPOINT;
	if (g_CentromereAlg == 0)
	{
		BOOL	bFound = FALSE;
		CPoint	pt1, pt2, pt1min, pt2min;
		double	fX, fY;
		double	fLen, fPrevLen;
		int	iStart, iEnd;
		double	fAlpha;
		fPrevLen = 0;
		int	iSeg, iOptSeg = -1;
		double	fOfs/*, fOptOfs*/;
		for (int i = TEST_SUBSEGMENT; i<TEST_SUBSEGMENT*(nAxisNodes-1); i++)
		{
			iSeg = i/TEST_SUBSEGMENT;
			fOfs = 1./TEST_SUBSEGMENT*(i%TEST_SUBSEGMENT);
			fAlpha = ::atan2(pAxisNodes[iSeg+1].y-pAxisNodes[iSeg].y, pAxisNodes[iSeg+1].x-pAxisNodes[iSeg].x);
			fX = pAxisNodes[iSeg].x+(pAxisNodes[iSeg+1].x-pAxisNodes[iSeg].x)*fOfs;
			fY = pAxisNodes[iSeg].y+(pAxisNodes[iSeg+1].y-pAxisNodes[iSeg].y)*fOfs;
			if (!GetPointDirVT4(pImgWrp, fX, fY, fAlpha+PI/2, pt1)) continue;
			if (!GetPointDirVT4(pImgWrp, fX, fY, fAlpha-PI/2, pt2)) continue;
			fLen = ::GetLen( pt1.x-pt2.x, pt1.y-pt2.y );
			if( fLen < fPrevLen )
				break;
			fPrevLen = fLen;
		}
		iStart = i;
		fPrevLen = 0;
		for (i = TEST_SUBSEGMENT*(nAxisNodes-1)-1; i>=TEST_SUBSEGMENT; i--)
		{
			iSeg = i/TEST_SUBSEGMENT;
			fOfs = 1./TEST_SUBSEGMENT*(i%TEST_SUBSEGMENT);
			fAlpha = ::atan2(pAxisNodes[iSeg+1].y-pAxisNodes[iSeg].y, pAxisNodes[iSeg+1].x-pAxisNodes[iSeg].x);
			fX = pAxisNodes[iSeg].x+(pAxisNodes[iSeg+1].x-pAxisNodes[iSeg].x)*fOfs;
			fY = pAxisNodes[iSeg].y+(pAxisNodes[iSeg+1].y-pAxisNodes[iSeg].y)*fOfs;
			if (!GetPointDirVT4(pImgWrp, fX, fY, fAlpha+PI/2, pt1)) continue;
			if (!GetPointDirVT4(pImgWrp, fX, fY, fAlpha-PI/2, pt2)) continue;
			fLen = ::GetLen(pt1.x-pt2.x, pt1.y-pt2.y);
			if (fLen < fPrevLen)
				break;
			fPrevLen = fLen;
		}
		iEnd = i;
		fPrevLen=1e50;
		for( i = iStart; i<iEnd; i++ )
		{
			iSeg = i/TEST_SUBSEGMENT;
			fOfs = 1./TEST_SUBSEGMENT*(i%TEST_SUBSEGMENT);
			fAlpha = ::atan2(pAxisNodes[iSeg+1].y-pAxisNodes[iSeg].y, pAxisNodes[iSeg+1].x-pAxisNodes[iSeg].x);
			fX = pAxisNodes[iSeg].x+(pAxisNodes[iSeg+1].x-pAxisNodes[iSeg].x)*fOfs;
			fY = pAxisNodes[iSeg].y+(pAxisNodes[iSeg+1].y-pAxisNodes[iSeg].y)*fOfs;
			if (!GetPointDirVT4(pImgWrp, fX, fY, fAlpha+PI/2, pt1)) continue;
			if (!GetPointDirVT4(pImgWrp, fX, fY, fAlpha-PI/2, pt2)) continue;
			fLen = ::GetLen(pt1.x-pt2.x, pt1.y-pt2.y);
			if (fLen < fPrevLen)
			{
				iMin = (int)(double(i)*double(nAxisPts1)/double(TEST_SUBSEGMENT*(nAxisNodes-1)));
				fPrevLen = fLen;
				pt1min = pt1;
				pt2min = pt2;
			}
		}
	}
	else if (g_CentromereAlg == 1)
	{
		BOOL	bFound = FALSE;
		CPoint	pt1, pt2, pt1min, pt2min;
		double  xs,xe;
		double	fX, fY;
		double	fLen, fPrevLen;
		int	iStart, iEnd;
		double	fAlpha, fAStart, fAEnd;
		fPrevLen = 0;
		int	iSeg, iOptSeg = -1;
		double	fOfs/*, fOptOfs*/;
		int iStartScan = TEST_SUBSEGMENT*(nAxisNodes-1)*15/100;
		int iEndScan = TEST_SUBSEGMENT*(nAxisNodes-1)*85/100;
		for (int i = iStartScan; i < iEndScan; i++)
		{
			iSeg = i/TEST_SUBSEGMENT;
			fOfs = 1./TEST_SUBSEGMENT*(i%TEST_SUBSEGMENT);
			fAlpha = ::atan2(pAxisNodes[iSeg+1].y-pAxisNodes[iSeg].y, pAxisNodes[iSeg+1].x-pAxisNodes[iSeg].x);
			fX = pAxisNodes[iSeg].x+(pAxisNodes[iSeg+1].x-pAxisNodes[iSeg].x)*fOfs;
			fY = pAxisNodes[iSeg].y+(pAxisNodes[iSeg+1].y-pAxisNodes[iSeg].y)*fOfs;
			if (!GetPointDirVT4(pImgWrp, fX, fY, fAlpha+PI/2, pt1)) continue;
			if (!GetPointDirVT4(pImgWrp, fX, fY, fAlpha-PI/2, pt2)) continue;
			fLen = ::GetLen( pt1.x-pt2.x, pt1.y-pt2.y );
			if (fLen < fPrevLen)
			{
				fAStart = fAlpha;
				xs = _xcoord(pt1.x, pt1.y, fAlpha);;
				break;
			}
			fPrevLen = fLen;
		}
		iStart = i;
		fPrevLen = 0;
		for (i = iEndScan-1; i >= iStartScan; i--)
		{
			iSeg = i/TEST_SUBSEGMENT;
			fOfs = 1./TEST_SUBSEGMENT*(i%TEST_SUBSEGMENT);
			fAlpha = ::atan2(pAxisNodes[iSeg+1].y-pAxisNodes[iSeg].y, pAxisNodes[iSeg+1].x-pAxisNodes[iSeg].x);
			fX = pAxisNodes[iSeg].x+(pAxisNodes[iSeg+1].x-pAxisNodes[iSeg].x)*fOfs;
			fY = pAxisNodes[iSeg].y+(pAxisNodes[iSeg+1].y-pAxisNodes[iSeg].y)*fOfs;
			if (!GetPointDirVT4(pImgWrp, fX, fY, fAlpha+PI/2, pt1)) continue;
			if (!GetPointDirVT4(pImgWrp, fX, fY, fAlpha-PI/2, pt2)) continue;
			fLen = ::GetLen(pt1.x-pt2.x, pt1.y-pt2.y);
			if (fLen < fPrevLen)
			{
				fAEnd = fAlpha;
				xe = _xcoord(pt1.x, pt1.y, fAlpha);;
				break;
			}
			fPrevLen = fLen;
		}
		iEnd = i;
		fPrevLen=1e50;
		for( i = iStart; i<iEnd; i++ )
		{
			iSeg = i/TEST_SUBSEGMENT;
			fOfs = 1./TEST_SUBSEGMENT*(i%TEST_SUBSEGMENT);
			fAlpha = ::atan2(pAxisNodes[iSeg+1].y-pAxisNodes[iSeg].y, pAxisNodes[iSeg+1].x-pAxisNodes[iSeg].x);
			fX = pAxisNodes[iSeg].x+(pAxisNodes[iSeg+1].x-pAxisNodes[iSeg].x)*fOfs;
			fY = pAxisNodes[iSeg].y+(pAxisNodes[iSeg+1].y-pAxisNodes[iSeg].y)*fOfs;
			if (!GetPointDirVT4(pImgWrp, fX, fY, fAlpha+PI/2, pt1)) continue;
			if (!GetPointDirVT4(pImgWrp, fX, fY, fAlpha-PI/2, pt2)) continue;
			fLen = ::GetLen(pt1.x-pt2.x, pt1.y-pt2.y);
			if (fLen < fPrevLen)
			{
				iMin = (int)(double(i)*double(nAxisPts1)/double(TEST_SUBSEGMENT*(nAxisNodes-1)));
				fPrevLen = fLen;
				pt1min = pt1;
				pt2min = pt2;
			}
			double fAlpha1 = fAlpha-PI/6.;
			double fAlpha2 = fAlpha+PI/6.;
			double fDAlpha = PI/30.;
			for (double a = fAlpha1; a < fAlpha2; a += fDAlpha)
			{
				if (!GetPointDirVT4(pImgWrp, fX, fY, a+PI/2, pt1)) continue;
				if (!GetPointDirVT4(pImgWrp, fX, fY, a-PI/2, pt2)) continue;
				fLen = ::GetLen(pt1.x-pt2.x, pt1.y-pt2.y);
				double x1s = _xcoord(pt1.x, pt1.y, fAStart);
				double x2s = _xcoord(pt2.x, pt2.y, fAStart);
				double x1e = _xcoord(pt1.x, pt1.y, fAEnd);
				double x2e = _xcoord(pt2.x, pt2.y, fAEnd);
				if (fLen < fPrevLen && x1s > xs && x2s > xs && x1e < xe && x2e < xe)
				{
					iMin = (int)(double(i)*double(nAxisPts1)/double(TEST_SUBSEGMENT*(nAxisNodes-1)));
					fPrevLen = fLen;
					pt1min = pt1;
					pt2min = pt2;
				}
			}
		}
	}
	else
	{
		ASSERT(CPoint(m_LongAxis.m_ptOffset) == CPoint(0,0));
		ASSERT(CPoint(m_Prof2.m_ptOffset) == CPoint(0,0));
		iMin = ncentromere(m_LongAxis.m_iptAxis,m_Prof2.m_pnProf,m_LongAxis.m_nSize,ptCentromere);
	}
	delete pAxisNodes;
	CCentromere Cen;
	if (iMin != -1)
	{
		// Init data
		nCmerePos = iMin;
		if (ptCentromere.x < 0)
		{
			int nplus = m_LongAxis.m_nScale/2;
			int ndiv = m_LongAxis.m_nScale;
			ptCentromere.x = (m_LongAxis.m_iptAxis[iMin].x+nplus)/ndiv;
			ptCentromere.y = (m_LongAxis.m_iptAxis[iMin].y+nplus)/ndiv;
		}
		dLenghtCI = double(nCmerePos)/double(m_LongAxis.m_nSize);
		bPolarity = nCmerePos<=(int)m_LongAxis.m_nSize/2;
		// Init draw object
		Cen.nCentromere = nCmerePos;
	}
	m_sptrChromos->SetCentromereCoord2(0, &Cen, FALSE);
}

int _cindexa(INTERVALIMAGE *pinvls, int y)
{
	int s1 = 0, s2 = 0, s3 = 0;
	for (int i = 0; i < pinvls->nrows; i++)
	{
		int c = 0;
		for (int j = 0; j < pinvls->prows[i].nints; j++)
		{
			c += pinvls->prows[i].pints[j].right-pinvls->prows[i].pints[j].left+1;
		}
		if (i < y)
			s1 += c;
		else if (i == y)
			s2 += c;
		else
			s3 += c;
	}
	return ((100*s3 + 50*s2)/(s1+s2+s3));
}

class CGrayRow
{
public:
	bool m_bNeedFree;
	color *m_pGrayVals;
	CGrayRow(CImageWrp &pImgWrp, int y)
	{
		CString s = pImgWrp.GetColorConvertor();
		m_bNeedFree = false;
		if (s.CompareNoCase(_T("Gray"))==0)
			m_pGrayVals = pImgWrp.GetRow(y, 0);
		else if (s.CompareNoCase(_T("RGB"))==0)
		{
			int nWidth = pImgWrp.GetWidth();
			m_pGrayVals = new color[nWidth];
			color *pred = pImgWrp.GetRow(y, 0);
			color *pgreen = pImgWrp.GetRow(y, 1);
			color *pblue = pImgWrp.GetRow(y, 2);
			for (int i = 0; i < nWidth; i++)
			{
				m_pGrayVals[i] = Bright(pblue[i], pgreen[i], pred[i]);
			}
			m_bNeedFree = true;
		}
		else if (s.CompareNoCase(_T("CGH"))==0)
			m_pGrayVals = pImgWrp.GetRow(y, g_CGHPane);
		else
			m_pGrayVals = pImgWrp.GetRow(y, 0);
	}
	~CGrayRow()
	{
		if (m_bNeedFree)
		{
			if (m_pGrayVals)
				delete m_pGrayVals;
		}
	}
};

int _cindexm(CImageWrp &pImgWrp, INTERVALIMAGE *pinvls, int y)
{
	int dy = pinvls->ptOffset.y-pImgWrp.GetOffset().y;
	int s1 = 0, s2 = 0, s3 = 0;
	for (int i = 0; i < pinvls->nrows; i++)
	{
		CGrayRow GrayRow(pImgWrp,i+dy);
		int c = 0;
		for (int j = 0; j < pinvls->prows[i].nints; j++)
			for (int k = pinvls->prows[i].pints[j].left; k <= pinvls->prows[i].pints[j].right; k++)
				c += (GrayRow.m_pGrayVals[k])>>8;
		if (i < y)
			s1 += c;
		else if (i == y)
			s2 += c;
		else
			s3 += c;
	}

	//paul 6.09.2002
	if( s1+s2+s3 == 0 )
		return 0;

	return ((100*s3 + 50*s2)/(s1+s2+s3));
}

int _cindexl(POINT *ppt, int npts, int y)
{
	y <<= 3;
	int i = 0;
	if (ppt[0].y <= ppt[npts-1].y)
	{
		while ((ppt->y < y) && (++i < npts))
			ppt++;
		return (100 * (npts - i) / npts);
	}
	else
	{
		while ((ppt->y > y) && (++i < npts))
			ppt++;
		return (100 * (i) / npts);
	}
}




