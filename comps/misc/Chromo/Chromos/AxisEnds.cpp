#include "StdAfx.h"
#include "Cromos.h"
#include "ChromParam.h"
#include "ChildObjects.h"
#include "ChrUtils.h"
#include <math.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

double CalcAsymmetry(CImageWrp &pImgWrp, Contour *pCntr, double dX0, double dY0, CPoint ptEnd, int nOffsX, int nOffsY,
	int iEnd)
{
	double a0 = atan2(ptEnd.y-dY0, ptEnd.x-dX0);
	double fSin = sin(a0);
	double fCos = cos(a0);
	int nXMax = (int)((ptEnd.x-dX0)*fCos+(ptEnd.y-dY0)*fSin);
#if defined(_DEBUG)
	int nYMax = (int)((ptEnd.y-dY0)*fCos-(ptEnd.x-dX0)*fSin);
	ASSERT(nYMax == 0);
#endif
	ASSERT(nXMax > 0);
	if (nXMax <= 0) return 0;
	double *pdMins = new double[nXMax+1];
	double *pdMaxs = new double[nXMax+1];
#if defined(_DEBUG)
	POINT *aptMins = new POINT[nXMax+1];
	POINT *aptMaxs = new POINT[nXMax+1];
#endif
	for (int i = 0; i <= nXMax; i++)
	{
		pdMins[i] = CMAX;
		pdMaxs[i] = -CMAX;
	}
	pdMins[nXMax] = pdMaxs[nXMax] = 0.;
	int nPoints = pCntr->nContourSize;
	for (i = iEnd==nPoints-1?0:iEnd+1; i != iEnd; i = i==nPoints-1?0:i+1)
	{
		int nx,ny;
		ContourGetPoint(pCntr,i,nx,ny);
		double fX = (nx-nOffsX-dX0)*fCos+(ny-nOffsY-dY0)*fSin;
		double fY = (ny-nOffsY-dY0)*fCos-(nx-nOffsX-dX0)*fSin;
		if (fX < -2) break;
		int nX = (int)fX;
		if (nX >= 0 && nX <= nXMax)
		{
			if (fY < pdMins[nX])
			{
				pdMins[nX] = fY;
#if defined(_DEBUG)
				aptMins[nX].x = nx;
				aptMins[nX].y = ny;
#endif
			}
			if (fY > pdMaxs[nX])
			{
				pdMaxs[nX] = fY;
#if defined(_DEBUG)
				aptMaxs[nX].x = nx;
				aptMaxs[nX].y = ny;
#endif
			}
		}
	}
	for (i = iEnd==0?nPoints-1:iEnd-1; i != iEnd; i = i==0?nPoints-1:i-1)
	{
		int nx,ny;
		ContourGetPoint(pCntr,i,nx,ny);
		double fX = (nx-nOffsX-dX0)*fCos+(ny-nOffsY-dY0)*fSin;
		double fY = (ny-nOffsY-dY0)*fCos-(nx-nOffsX-dX0)*fSin;
		if (fX < -2) break;
		int nX = (int)fX;
		if (nX >= 0 && nX <= nXMax)
		{
			if (fY < pdMins[nX])
			{
				pdMins[nX] = fY;
#if defined(_DEBUG)
				aptMins[nX].x = nx;
				aptMins[nX].y = ny;
#endif
			}
			if (fY > pdMaxs[nX])
			{
				pdMaxs[nX] = fY;
#if defined(_DEBUG)
				aptMaxs[nX].x = nx;
				aptMaxs[nX].y = ny;
#endif
			}
		}
	}
	ASSERT(pdMins[0] < CMAX/2);
	ASSERT(pdMaxs[0] > -CMAX/2);
	ASSERT(pdMins[nXMax] < CMAX/2);
	ASSERT(pdMaxs[nXMax] > -CMAX/2);
	for (i = 1; i < nXMax; i++)
	{
		if (pdMins[i] > CMAX/2)
		{
			ASSERT(pdMins[i-1] < CMAX/2);
			ASSERT(pdMins[i+1] < CMAX/2);
			pdMins[i] = (pdMins[i-1]+pdMins[i+1])/2;
		}
		if (pdMaxs[i] < -CMAX/2)
		{
			ASSERT(pdMaxs[i-1] > -CMAX/2);
			ASSERT(pdMaxs[i+1] > -CMAX/2);
			pdMaxs[i] = (pdMaxs[i-1]+pdMaxs[i+1])/2;
		}
	}
	double dAssym = 0;
	int nPts = 0;
	for (i = 0; i <= nXMax; i++)
	{
		if (pdMins[i] < 0. && pdMaxs[i] > 0.)
		{
			dAssym += fabs(fabs(pdMaxs[i])-fabs(pdMins[i]));
			nPts++;
		}
#if defined(_DEBUG)
		if (ptEnd.x+nOffsX==56 && ptEnd.y+nOffsY==27 /*|| ptEnd.x+nOffsX==305 && ptEnd.y+nOffsY==73*/)
		{
			TRACE2(" CalcAssymetry: (%d,%d)", aptMins[i].x, aptMins[i].y);
			TRACE2(", (%d,%d)\n", aptMaxs[i].x, aptMaxs[i].y);
		}
#endif
	}
	delete pdMaxs;
	delete pdMins;
#if defined(_DEBUG)
	delete aptMins;
	delete aptMaxs;
#endif
	return dAssym/nPts;
}

#if defined(_DEBUG)
extern int nObj;
#endif
void CAxisContext::CorrectEnd2(double daDef, double daLim, bool bFirst)
{
	TRACE("CorrectEnd2, %d\n", bFirst);
	double aDef,a1,a2,a3,aMin1,aMax1,aMin2,aMax2,aMin3,aMax3;
	int nx,ny,idx,idx1,idx2,idxEnd;
	bool bClosestFound,bPrevOnEnd;
	if (nPoints >= 4)
	{
		int nPrevIdx,nCurIdx;
		int nPrevIdx1;
		bool bCorrected = false;
		if (bFirst)
		{
			aDef = atan2(pfSegY[1]-pfSegY[2], pfSegX[1]-pfSegX[2])+fAngle;
			a1 = atan2(pfSegY[2]-pfSegY[3], pfSegX[2]-pfSegX[3])+fAngle;
			a2 = fabs(aDef-a1);
			a3 = fabs(aDef-2*PI-a1);
			a1 = fabs(aDef+2*PI-a1);
			a1 = min(a1,a2);
			a1 = min(a1,a3);
			nPrevIdx = 1;
			nCurIdx = 0;
			nPrevIdx1 = a1<PI/6||m_bCurve?2:1;
		}
		else 
		{
			aDef = atan2(pfSegY[nPoints-2]-pfSegY[nPoints-3], pfSegX[nPoints-2]-pfSegX[nPoints-3] )+fAngle;
			a1 = atan2(pfSegY[nPoints-3]-pfSegY[nPoints-4], pfSegX[nPoints-3]-pfSegX[nPoints-4])+fAngle;
			a2 = fabs(aDef-a1);
			a3 = fabs(aDef-2*PI-a1);
			a1 = fabs(aDef+2*PI-a1);
			a1 = min(a1,a2);
			a1 = min(a1,a3);
			nPrevIdx = nPoints-2;
			nCurIdx = nPoints-1;
			nPrevIdx1 = a1<PI/6||m_bCurve?nPoints-3:nPoints-2;
		}
		aDef = check_angle(aDef);
		aMin1 = aDef-daDef;
		aMax1 = aDef+daDef;
		aMin2 = aDef-PI/12;
		aMax2 = aDef+PI/12;
		aMin3 = aDef-daLim;
		aMax3 = aDef+daLim;
		int nOffsX = pImgWrp.GetOffset().x;
		int nOffsY = pImgWrp.GetOffset().y;
		for(idx = 0; idx < nCount; idx++)
		{
			ContourGetPoint(pCntr,idx,nx,ny);
			a1 = atan2(ny-nOffsY-pfSegY[nPrevIdx], nx-nOffsX-pfSegX[nPrevIdx])+fAngle;
//			a = check_angle(a);
			a2 = a1-2*PI;
			a3 = a1+2*PI;
			bool bOnEnd = a1>=aMin1&&a1<=aMax1||a2>=aMin1&&a2<=aMax1||a3>=aMin1&&a3<=aMax1;
			if (!bOnEnd) break;
		}
		if (idx == nCount) return;
		bPrevOnEnd = false;
		bClosestFound = false;
		idxEnd = idx;
		for (idx = idxEnd==nCount-1?0:idxEnd+1; idx != idxEnd; idx = idx==nCount-1?0:idx+1)
		{
			ContourGetPoint(pCntr,idx,nx,ny);
			a1 = atan2(ny-nOffsY-pfSegY[nPrevIdx], nx-nOffsX-pfSegX[nPrevIdx])+fAngle;
//			a = check_angle(a);
			a2 = a1-2*PI;
			a3 = a1+2*PI;
			bool bOnEnd = a1>=aMin1&&a1<=aMax1||a2>=aMin1&&a2<=aMax1||a3>=aMin1&&a3<=aMax1;
			if (bOnEnd)
			{
				bool bClosest = a1>=aMin2&&a1<=aMax2||a2>=aMin2&&a2<=aMax2||a3>=aMin2&&a3<=aMax2;
				if (bClosest) bClosestFound = true;
				if (!bPrevOnEnd) idx1 = idx;
			}
			else if (bPrevOnEnd && bClosestFound)
			{
				idx2 = idx;
				break;
			}
			bPrevOnEnd = bOnEnd;
		}
		if (bPrevOnEnd && idx == idxEnd)
			idx2 = idxEnd;
		if (bClosestFound && idx1 != idx2)
		{
			double dAssym = 100000000;
			int nAssym = -1;
			int nSize = idx2>idx1?idx2-idx1:idx2+nCount-idx1;
			for(idx = idx1; idx != idx2; idx = idx==nCount-1?0:idx+1)
			{
				ContourGetPoint(pCntr,idx,nx,ny);
				double d = CalcAsymmetry(pImgWrp, pCntr, pfSegX[nPrevIdx1], pfSegY[nPrevIdx1], CPoint(nx-nOffsX,ny-nOffsY),
					nOffsX, nOffsY, idx);
#if defined(_DEBUG)
				if (nObj == 4) TRACE3("%d,%d Assymetry: %f\n", nx, ny, d);
#endif
				if (d < dAssym)
				{
					dAssym = d;
					nAssym = idx;
				}
			}
			if (nAssym != -1)
			{
				ContourGetPoint(pCntr,nAssym,nx,ny);
				pfSegX[nCurIdx] = nx-nOffsX;
				pfSegY[nCurIdx] = ny-nOffsY;
				bCorrected = true;
			}
		}
		if (!bCorrected)
		{
			double fL1,fL2;
			if (bFirst)
			{
				a1 = atan2(pfSegY[1]-pfSegY[2], pfSegX[1]-pfSegX[2])+fAngle;
				fL1 = GetLen(pfSegX[1]-pfSegX[0], pfSegY[1]-pfSegY[0]);
				a1 = a1-fAngle;
				pfSegX[0] = pfSegX[1]+fL1*cos( a1 );
				pfSegY[0] = pfSegY[1]+fL1*sin( a1 );
				CPoint pt;
				if (GetPointDir(pImgWrp, pfSegX[1], pfSegY[1], a1, pt))
				{
					pfSegX[0] = pt.x;
					pfSegY[0] = pt.y;
				}
			}
			else
			{
				a2 = atan2(pfSegY[(int)nPoints-2]-pfSegY[(int)nPoints-3], pfSegX[(int)nPoints-2]-pfSegX[(int)nPoints-3])+fAngle;
				fL2 = GetLen(pfSegX[nPoints-2]-pfSegX[nPoints-1], pfSegY[nPoints-2]-pfSegY[nPoints-1]);
				a2 = a2-fAngle;
				pfSegX[(int)nPoints-1] = pfSegX[(int)nPoints-2]+fL2*cos( a2 );
				pfSegY[(int)nPoints-1] = pfSegY[(int)nPoints-2]+fL2*sin( a2 );
				CPoint pt;
				if (GetPointDir(pImgWrp, pfSegX[nPoints-2], pfSegY[nPoints-2], a2, pt))
				{
					pfSegX[nPoints-1] = pt.x;
					pfSegY[nPoints-1] = pt.y;
				}
			}
		}
	}
	else if (nPoints == 3)
		CorrectEnd1(daDef, daLim, bFirst);
}

void CAxisContext::CorrectEnds(double daDef)
{
	double fL1,fL2,da1,da2;
	double a1,a2/*,a3,a4*/;
	if (nPoints >= 4)
	{
		a1 = atan2( pfSegY[1]-pfSegY[2], pfSegX[1]-pfSegX[2] )+fAngle;
		a1 = check_angle(a1);
		a2 = atan2( pfSegY[(int)nPoints-2]-pfSegY[(int)nPoints-3], pfSegX[(int)nPoints-2]-pfSegX[(int)nPoints-3] )+fAngle;
		a2 = check_angle(a2);
		if (nPoints >= 6)
		{
			da1 = daDef;
			da2 = daDef;
			/*
			a3 = atan2(pfSegY[2]-pfSegY[3], pfSegX[2]-pfSegX[3])+fAngle;
			a3 = check_angle(a3);
			a4 = atan2(pfSegY[(int)nPoints-3]-pfSegY[(int)nPoints-4], pfSegX[(int)nPoints-3]-pfSegX[(int)nPoints-4])+fAngle;
			a4 = check_angle(a4);
			da1 = fabs(a1-a3);
			if (da1 > PI) da1 = fabs(da1-2*PI);
			da1 = min(da1,PI/6);
			da1 = max(da1,PI/6);
			da2 = fabs(a2-a4);
			if (da2 > PI) da2 = fabs(da2-2*PI);
			da2 = min(da2,PI/6);
			da2 = max(da2,PI/6);
			*/
		}
		else
		{
			da1 = daDef/2;
			da2 = daDef/2;
		}
//			a3 = atan2( pfSegY[2]-pfSegY[3], pfSegX[2]-pfSegX[3] )+fAngle;
//			a3 = check_angle(a3);
//			a4 = atan2( pfSegY[(int)nPoints-3]-pfSegY[(int)nPoints-4], 
//					    pfSegX[(int)nPoints-3]-pfSegX[(int)nPoints-4] )+fAngle;
//			a4 = check_angle(a4);
//			if((a1-a3)>PI) a3=a3 + 2*PI;
//			if((a1-a3)<-PI) a3=a3 - 2*PI;
		double a1_,a1x_,a2_,a2x_,a3_,a3x_,a4_,a4x_;
		a1_ = a1-da1;//min(a1,a3)-PI/12;
		a3_ = a1+da1;//max(a1,a3)+PI/12;
		if (a3_ < 0)
		{
			a1x_ = a1_+2*PI;
			a3x_ = a3_+2*PI;
		}
		else
		{
			a1x_ = a1_-2*PI;
			a3x_ = a3_-2*PI;
		}
//			if((a2-a4)>PI) a4=a4 + 2*PI;
//			if((a2-a4)<-PI) a4=a4 - 2*PI;
		a2_ = a2-da2;//min(a2,a4)-PI/36;
		a4_ = a2+da2;//max(a2,a4)+PI/36;
		if (a4_ < 0)
		{
			a2x_ = a2_+2*PI;
			a4x_ = a4_+2*PI;
		}
		else
		{
			a2x_ = a2_-2*PI;
			a4x_ = a4_-2*PI;
		}
			double dMinDist0 = 10000.;
			double dMinDist1 = 10000.;
			CPoint pt0,pt1,pt2,pt3,pt4;
			CPoint ptFnd0, ptFnd1;
			double dDist0,dDist1,dDist2,dDist3,dDist4,dDist0l,dDist1l,dDist2l,dDist3l,dDist4l,a;
			int nOffsX = pImgWrp.GetOffset().x;
			int nOffsY = pImgWrp.GetOffset().y;
			int nx,ny;
			ContourGetPoint(pCntr,nCount-4,nx,ny);
			pt0 = CPoint(nx,ny);
			dDist0 = ::GetLen(pfSegX[1]+nOffsX-pt0.x, pfSegY[1]+nOffsY-pt0.y);
			dDist0l = ::GetLen(pfSegX[nPoints-2]+nOffsX-pt0.x, pfSegY[nPoints-2]+nOffsY-pt0.y);
			ContourGetPoint(pCntr,nCount-3,nx,ny);
			pt1 = CPoint(nx,ny);
			dDist1 = ::GetLen(pfSegX[1]+nOffsX-pt1.x, pfSegY[1]+nOffsY-pt1.y);
			dDist1l = ::GetLen(pfSegX[nPoints-2]+nOffsX-pt1.x, pfSegY[nPoints-2]+nOffsY-pt1.y);
			ContourGetPoint(pCntr,nCount-2,nx,ny);
			pt2 = CPoint(nx,ny);
			dDist2 = ::GetLen(pfSegX[1]+nOffsX-pt2.x, pfSegY[1]+nOffsY-pt2.y);
			dDist2l = ::GetLen(pfSegX[nPoints-2]+nOffsX-pt2.x, pfSegY[nPoints-2]+nOffsY-pt2.y);
			ContourGetPoint(pCntr,nCount-1,nx,ny);
			pt3 = CPoint(nx,ny);
			dDist3 = ::GetLen(pfSegX[1]+nOffsX-pt3.x, pfSegY[1]+nOffsY-pt3.y);
			dDist3l = ::GetLen(pfSegX[nPoints-2]+nOffsX-pt3.x, pfSegY[nPoints-2]+nOffsY-pt3.y);
			for(int idx = 0; idx < nCount; idx++)
			{
				ContourGetPoint(pCntr,idx,nx,ny);
				pt4 = CPoint(nx, ny);
				dDist4 = ::GetLen(pfSegX[1]+nOffsX-pt4.x, pfSegY[1]+nOffsY-pt4.y);
				dDist4l = ::GetLen(pfSegX[nPoints-2]+nOffsX-pt4.x, pfSegY[nPoints-2]+nOffsY-pt4.y);
				a = atan2(pt2.y-nOffsY-pfSegY[1], pt2.x-nOffsX-pfSegX[1])+fAngle;
				a = check_angle(a);
				if (dDist2 < dMinDist0 && (a >= a1_ && a <= a3_ || a >= a1x_ && a <= a3x_) && dDist2 <= dDist1 &&
					dDist2 <= dDist3 &&	dDist2 <= dDist0 && dDist2 <= dDist4)
				{
					dMinDist0 = dDist2;
					ptFnd0 = pt2;
				}
				a = atan2(pt2.y-nOffsY-pfSegY[nPoints-2], pt2.x-nOffsX-pfSegX[nPoints-2])+fAngle;
				a = check_angle(a);
				if (dDist2l < dMinDist1 && (a >= a2_ && a <= a4_ || a >= a2x_ && a <= a4x_) && dDist2l <= dDist1l &&
					dDist2l <= dDist3l && dDist2l <= dDist0l && dDist2l <= dDist4l)
				{
					dMinDist1 = dDist2l;
					ptFnd1 = pt2;
				}
				dDist0  = dDist1;
				dDist1  = dDist2;
				dDist2  = dDist3;
				dDist3  = dDist4;
				dDist0l = dDist1l;
				dDist1l = dDist2l;
				dDist2l = dDist3l;
				dDist3l = dDist4l;
				pt0 = pt1;
				pt1 = pt2;
				pt2 = pt3;
				pt3 = pt4;
			}
			if (dMinDist0 < 1000.)
			{
				pfSegX[0] = ptFnd0.x-nOffsX;
				pfSegY[0] = ptFnd0.y-nOffsY;
			}
			else
			{
				a1 = atan2(pfSegY[1]-pfSegY[2], pfSegX[1]-pfSegX[2])+fAngle;
				fL1 = GetLen(pfSegX[1]-pfSegX[0], pfSegY[1]-pfSegY[0]);
				a1 = a1-fAngle;
				pfSegX[0] = pfSegX[1]+fL1*cos( a1 );
				pfSegY[0] = pfSegY[1]+fL1*sin( a1 );
				CPoint pt;
				if (GetPointDir(pImgWrp, pfSegX[1], pfSegY[1], a1, pt))
				{
					pfSegX[0] = pt.x;
					pfSegY[0] = pt.y;
				}
			}
			if (dMinDist1 < 1000.)
			{
				pfSegX[nPoints-1] = ptFnd1.x-nOffsX;
				pfSegY[nPoints-1] = ptFnd1.y-nOffsY;
			}
			else
			{
				a2 = atan2(pfSegY[(int)nPoints-2]-pfSegY[(int)nPoints-3], pfSegX[(int)nPoints-2]-pfSegX[(int)nPoints-3])+fAngle;
				fL2 = GetLen(pfSegX[nPoints-2]-pfSegX[nPoints-1], pfSegY[nPoints-2]-pfSegY[nPoints-1]);
				a2 = a2-fAngle;
				pfSegX[(int)nPoints-1] = pfSegX[(int)nPoints-2]+fL2*cos( a2 );
				pfSegY[(int)nPoints-1] = pfSegY[(int)nPoints-2]+fL2*sin( a2 );
				CPoint pt;
				if (GetPointDir(pImgWrp, pfSegX[nPoints-2], pfSegY[nPoints-2], a2, pt))
				{
					pfSegX[nPoints-1] = pt.x;
					pfSegY[nPoints-1] = pt.y;
				}
			}
	}
}

void CAxisContext::CorrectEnd1(double daDef, double daLim, bool bFirst)
{
	double aDef,a1,a2,a3,aMin1,aMax1,aMin2,aMax2,aMin3,aMax3;
	int nx,ny,idx,idx1,idx2,idxEnd,i;
	bool bClosestFound,bPrevOnEnd;
	if (nPoints >= 4)
	{
		int nPrevIdx,nCurIdx;
		bool bCorrected = false;
		if (bFirst)
		{
			aDef = atan2(pfSegY[1]-pfSegY[2], pfSegX[1]-pfSegX[2])+fAngle;
			nPrevIdx = 1;
			nCurIdx = 0;
		}
		else 
		{
			aDef = atan2(pfSegY[nPoints-2]-pfSegY[nPoints-3], pfSegX[nPoints-2]-pfSegX[nPoints-3] )+fAngle;
			nPrevIdx = nPoints-2;
			nCurIdx = nPoints-1;
		}
		aDef = check_angle(aDef);
		aMin1 = aDef-daDef;
		aMax1 = aDef+daDef;
		aMin2 = aDef-PI/12;
		aMax2 = aDef+PI/12;
		aMin3 = aDef-daLim;
		aMax3 = aDef+daLim;
		int nOffsX = pImgWrp.GetOffset().x;
		int nOffsY = pImgWrp.GetOffset().y;
		for(idx = 0; idx < nCount; idx++)
		{
			ContourGetPoint(pCntr,idx,nx,ny);
			a1 = atan2(ny-nOffsY-pfSegY[nPrevIdx], nx-nOffsX-pfSegX[nPrevIdx])+fAngle;
//			a = check_angle(a);
			a2 = a1-2*PI;
			a3 = a1+2*PI;
			bool bOnEnd = a1>=aMin1&&a1<=aMax1||a2>=aMin1&&a2<=aMax1||a3>=aMin1&&a3<=aMax1;
			if (!bOnEnd) break;
		}
		if (idx == nCount) return;
		bPrevOnEnd = false;
		bClosestFound = false;
		idxEnd = idx;
		for (idx = idxEnd==nCount-1?0:idxEnd+1; idx != idxEnd; idx = idx==nCount-1?0:idx+1)
		{
			ContourGetPoint(pCntr,idx,nx,ny);
			a1 = atan2(ny-nOffsY-pfSegY[nPrevIdx], nx-nOffsX-pfSegX[nPrevIdx])+fAngle;
//			a = check_angle(a);
			a2 = a1-2*PI;
			a3 = a1+2*PI;
			bool bOnEnd = a1>=aMin1&&a1<=aMax1||a2>=aMin1&&a2<=aMax1||a3>=aMin1&&a3<=aMax1;
			if (bOnEnd)
			{
				bool bClosest = a1>=aMin2&&a1<=aMax2||a2>=aMin2&&a2<=aMax2||a3>=aMin2&&a3<=aMax2;
				if (bClosest) bClosestFound = true;
				if (!bPrevOnEnd) idx1 = idx;
			}
			else if (bPrevOnEnd && bClosestFound)
			{
				idx2 = idx;
				break;
			}
			bPrevOnEnd = bOnEnd;
		}
		if (bPrevOnEnd && idx == idxEnd)
			idx2 = idxEnd;
		if (bClosestFound && idx1 != idx2)
		{
			CArray<double, double &> arrDists;
			CUIntArray arrFlags;
			int nSize = idx2>idx1?idx2-idx1:idx2+nCount-idx1;
			arrDists.SetSize(nSize);
			arrFlags.SetSize(nSize);
			for(i = 0, idx = idx1; idx != idx2; idx = idx==nCount-1?0:idx+1, i++)
			{
				ContourGetPoint(pCntr,idx,nx,ny);
				double dDist = ::GetLen(pfSegX[nPrevIdx]+nOffsX-nx, pfSegY[nPrevIdx]+nOffsY-ny);
				arrDists[i] = dDist;
				a1 = atan2(ny-nOffsY-pfSegY[nPrevIdx], nx-nOffsX-pfSegX[nPrevIdx])+fAngle;
				a2 = a1-2*PI;
				a3 = a1+2*PI;
				if (a1>=aMin3&&a1<=aMax3||a2>=aMin3&&a2<=aMax3||a3>=aMin3&&a3<=aMax3)
					arrFlags[i] = 1;
				else
					arrFlags[i] = 0;
			}
			CArray<double, double&> arrLeft,arrRight;
			arrLeft.SetSize(nSize);
			arrRight.SetSize(nSize);
			double dMax = 0;
			for (i = 0; i < nSize; i++)
			{
				dMax = max(dMax, arrDists[i]);
				arrLeft[i] = dMax;
			}
			dMax = 0;
			for (i = nSize-1; i >= 0; i--)
			{
				dMax = max(dMax, arrDists[i]);
				arrRight[i] = dMax;
			}
			dMax = 0;
			int iMax = -1;
			for (i = 0; i < nSize; i++)
			{
				double d1 = arrLeft[i] - arrDists[i];
				double d2 = arrRight[i] - arrDists[i];
				d1 = min(d1,d2);
				if (d1 > dMax && arrFlags[i])
				{
					dMax = d1;
					iMax = i;
				}
			}
			if (iMax > 0 && dMax > 1)
			{
				idx = iMax+idx1>=nCount?iMax+idx1-nCount:iMax+idx1;
				ContourGetPoint(pCntr,idx,nx,ny);
				pfSegX[nCurIdx] = nx-nOffsX;
				pfSegY[nCurIdx] = ny-nOffsY;
				bCorrected = true;
			}
		}
		if (!bCorrected)
		{
			double fL1,fL2;
			if (bFirst)
			{
				a1 = atan2(pfSegY[1]-pfSegY[2], pfSegX[1]-pfSegX[2])+fAngle;
				fL1 = GetLen(pfSegX[1]-pfSegX[0], pfSegY[1]-pfSegY[0]);
				a1 = a1-fAngle;
				pfSegX[0] = pfSegX[1]+fL1*cos( a1 );
				pfSegY[0] = pfSegY[1]+fL1*sin( a1 );
				CPoint pt;
				if (GetPointDir(pImgWrp, pfSegX[1], pfSegY[1], a1, pt))
				{
					pfSegX[0] = pt.x;
					pfSegY[0] = pt.y;
				}
			}
			else
			{
				a2 = atan2(pfSegY[(int)nPoints-2]-pfSegY[(int)nPoints-3], pfSegX[(int)nPoints-2]-pfSegX[(int)nPoints-3])+fAngle;
				fL2 = GetLen(pfSegX[nPoints-2]-pfSegX[nPoints-1], pfSegY[nPoints-2]-pfSegY[nPoints-1]);
				a2 = a2-fAngle;
				pfSegX[(int)nPoints-1] = pfSegX[(int)nPoints-2]+fL2*cos( a2 );
				pfSegY[(int)nPoints-1] = pfSegY[(int)nPoints-2]+fL2*sin( a2 );
				CPoint pt;
				if (GetPointDir(pImgWrp, pfSegX[nPoints-2], pfSegY[nPoints-2], a2, pt))
				{
					pfSegX[nPoints-1] = pt.x;
					pfSegY[nPoints-1] = pt.y;
				}
			}
		}
	}
}

void CAxisContext::CorrectEnds3()
{
	double a1,a2,a3,a4;
	//finding head and tail
	//first and last point determine as max. point along axix 1,2 and 6,5 
		// их заносим в pfSeg[0] and pfSeg[MAX-1]
	a1 = atan2( pfSegY[1]-pfSegY[2], pfSegX[1]-pfSegX[2] )+fAngle;
	a2 = atan2( pfSegY[(int)nPoints-2]-pfSegY[(int)nPoints-3], 
				pfSegX[(int)nPoints-2]-pfSegX[(int)nPoints-3] )+fAngle;

	a3 = atan2( pfSegY[2]-pfSegY[3], pfSegX[2]-pfSegX[3] )+fAngle;
	a4 = atan2( pfSegY[(int)nPoints-3]-pfSegY[(int)nPoints-4], 
				pfSegX[(int)nPoints-3]-pfSegX[(int)nPoints-4] )+fAngle;
	if((a1-a3)>PI) a3=a3 + 2*PI;
	if((a1-a3)<-PI) a3=a3 - 2*PI;
	if((a2-a4)>PI) a4=a4 + 2*PI;
	if((a2-a4)<-PI) a4=a4 - 2*PI;
	a1 = (a1+a3)/2.0;
	a2 = (a2+a4)/2.0;
	double	fL1, fL2;
	fL1 = GetLen(pfSegX[1]-pfSegX[0], pfSegY[1]-pfSegY[0]);
	fL2 = GetLen(pfSegX[nPoints-2]-pfSegX[nPoints-1], pfSegY[nPoints-2]-pfSegY[nPoints-1]);
	a1 = a1-fAngle;//-(a1-);
	a2 = a2-fAngle;//-(a2-fAngle);
	/*pfSegX[0] = pfSegX[1]+fL1*cos( a1 );
	pfSegY[0] = pfSegY[1]+fL1*sin( a1 );
	pfSegX[(int)nPoints-1] = pfSegX[(int)nPoints-2]+fL2*cos( a2 );
	pfSegY[(int)nPoints-1] = pfSegY[(int)nPoints-2]+fL2*sin( a2 ); */
	CPoint pt;
	if (GetPointDir(pImgWrp, pfSegX[1], pfSegY[1], a1, pt))
	{
		pfSegX[0] = pt.x;
		pfSegY[0] = pt.y;
	}
	if (GetPointDir(pImgWrp, pfSegX[nPoints-2], pfSegY[nPoints-2], a2, pt))
	{
		pfSegX[nPoints-1] = pt.x;
		pfSegY[nPoints-1] = pt.y;
	}
}

void CAxisContext::CorrectEnds4()
{
	double a1,a2;
	//finding head and tail
	//first and last point determine as max. point along axix 1,2 and 6,5 
		// их заносим в pfSeg[0] and pfSeg[MAX-1]
	a1 = atan2( pfSegY[1]-pfSegY[2], pfSegX[1]-pfSegX[2] );//+fAngle;
	a2 = atan2( pfSegY[(int)nPoints-2]-pfSegY[(int)nPoints-3], 
				pfSegX[(int)nPoints-2]-pfSegX[(int)nPoints-3] );//+fAngle;
	CPoint ptOffs = pImgWrp.GetOffset();
	double	fL1, fL2;
	double m1, m2;
	bool bFirstTime = true;
	int idx1 = 0, idx2 = 0;
	double fX;
	int nx,ny;
	for(int idx = 0; idx < nCount; idx++ )
	{
//		pCntr->GetPoint( idx, &pt );
		ContourGetPoint(pCntr, idx, nx, ny);
		if( bFirstTime )
		{
			m1 = nx*cos(a1)+ny*sin(a1);
			m2 = nx*cos(a2)+ny*sin(a2);
			bFirstTime = false;
		}
		fX = nx*cos(a1)+ny*sin(a1);
		if (fX > m1)
		{
			idx1 = idx;
			m1 = fX;
		}
		fX = nx*cos(a2)+ny*sin(a2);
		if (fX > m2)
		{
			idx2 = idx;
			m2 = fX;
		}
	}
	ContourGetPoint(pCntr, idx1, nx, ny);
	fL1 = ::GetLen(pfSegX[1]+ptOffs.x-nx, pfSegY[1]+ptOffs.y-ny);
	ContourGetPoint(pCntr, idx2, nx, ny);
	fL2 = ::GetLen(pfSegX[nPoints-2]+ptOffs.x-nx, pfSegY[nPoints-2]+ptOffs.y-ny);
//	a1 = a1-fAngle;
//	a2 = a2-fAngle;
	pfSegX[0] = pfSegX[1]+fL1*cos(a1);
	pfSegY[0] = pfSegY[1]+fL1*sin(a1);
	pfSegX[nPoints-1] = pfSegX[nPoints-2]+fL2*cos(a2);
	pfSegY[nPoints-1] = pfSegY[nPoints-2]+fL2*sin(a2);;
}

void CAxisContext::CorrectEnds5()
{
	int ts = 3;
	int st = 3;
	if (ts > nPoints/2)
		ts = nPoints/2;
	int n1 = 1;
	int n2 = ts+1;
	double x1 = pfSegX[n1]*fCos+pfSegY[n1]*fSin;;
	double y1 = pfSegY[n1]*fCos-pfSegX[n1]*fSin;
	double x2 = pfSegX[n2]*fCos+pfSegY[n2]*fSin;;
	double y2 = pfSegY[n2]*fCos-pfSegX[n2]*fSin;
	double d  = (y2-y1)/(x2-x1);
	for (int i = -1; i < ts; i++)
	{
		double x = x1+i*st;
		double y = y1+i*st*d;
		pfSegX[i+1] = x*fCos-y*fSin;
		pfSegY[i+1] = y*fCos+x*fSin;
	}
	n1 = nPoints-2;
	n2 = nPoints-ts-2;
	x1 = pfSegX[n1]*fCos+pfSegY[n1]*fSin;;
	y1 = pfSegY[n1]*fCos-pfSegX[n1]*fSin;
	x2 = pfSegX[n2]*fCos+pfSegY[n2]*fSin;;
	y2 = pfSegY[n2]*fCos-pfSegX[n2]*fSin;
	d  = (y2-y1)/(x2-x1);
	for (i = -1; i < ts; i++)
	{
		double x = x1-i*st;
		double y = y1-i*st*d;
		pfSegX[nPoints-2-i] = x*fCos-y*fSin;
		pfSegY[nPoints-2-i] = y*fCos+x*fSin;
	}
	CorrectEnds3();
}

void CAxisContext::CorrectEnds(int nStage)
{
	double daLim = m_bCurve?PI/4:PI/6;
	if (g_EndsAlg == 0)
		CorrectEnds(PI/6);
	else if (g_EndsAlg == 1)
	{
		CorrectEnd1(PI/2, daLim, true);
		CorrectEnd1(PI/2, daLim, false);
	}
	else if (g_EndsAlg == 2)
	{
		CorrectEnd2(PI/2, daLim, true);
		CorrectEnd2(PI/2, daLim, false);
	}
	else if (g_EndsAlg == 3)
		CorrectEnds3();
	else if (g_EndsAlg == 4)
	{
		if (nStage == CESkeleton)
			CorrectEnds3();
		else
			CorrectEnds4();
	}
	else if (g_EndsAlg == 5)
	{
		if (nStage == CESkeleton || nPtsDist != 3)
			CorrectEnds3();
		else
			CorrectEnds5();
	}
};

