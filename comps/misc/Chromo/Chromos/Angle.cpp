#include "StdAfx.h"
#include "Cromos.h"
#include "ChromParam.h"
#include "ChildObjects.h"
#include "PUtils.h"
#include <math.h>
#include "debug.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

double FindWidth(double fAngle, double &dx, Contour *pCntr)
{
	double x1,y1;
	double fCos = ::cos(fAngle);
	double fSin = ::sin(fAngle);
	int nCount = pCntr->nContourSize;
	double x = pCntr->ppoints[0].x;
	double y = pCntr->ppoints[0].y;
	double dMinY = y*fCos-x*fSin;
	double dMaxY = dMinY;
	double dMinX = x*fCos+y*fSin;
	double dMaxX = dMinX;
	for (int i = 1; i < nCount; i++)
	{
		x = pCntr->ppoints[i].x;
		y = pCntr->ppoints[i].y;
		x1 = x*fCos+y*fSin;
		y1 = y*fCos-x*fSin;
		if (x1 < dMinX)
			dMinX = x1;
		else if (x1 > dMaxX)
			dMaxX = x1;
		if (y1 < dMinY)
			dMinY = y1;
		else if (y1 > dMaxY)
			dMaxY = y1;
	}
	dx = dMaxX - dMinX;
	return dMaxY - dMinY;
}


double FindAvWidth(double fAngle, Contour *pCntr)
{
	double x1,y1;
	double fCos = ::cos(fAngle);
	double fSin = ::sin(fAngle);
	int nCount = pCntr->nContourSize;
	double x = pCntr->ppoints[0].x;
	double y = pCntr->ppoints[0].y;
	double dMinX = x*fCos+y*fSin;
	double dMaxX = dMinX;
	for (int i = 1; i < nCount; i++)
	{
		x = pCntr->ppoints[i].x;
		y = pCntr->ppoints[i].y;
		x1 = x*fCos+y*fSin;
		if (x1 < dMinX)
			dMinX = x1;
		else if (x1 > dMaxX)
			dMaxX = x1;
	}
	int nMinX = int(dMinX);
	int nMaxX = int(dMaxX);
	int nDiap = nMaxX-nMinX+1;
	CArray<double,double&> arrMin,arrMax;
	arrMin.SetSize(nDiap);
	arrMax.SetSize(nDiap);
	for (i = 0; i < nDiap; i++)
	{
		arrMin[i] = CMAX;
		arrMax[i] = -CMAX;
	}
	for (i = 0; i < nCount; i++)
	{
		x = pCntr->ppoints[i].x;
		y = pCntr->ppoints[i].y;
		x1 = x*fCos+y*fSin;
		y1 = y*fCos-x*fSin;
		int nx = int(x1)-nMinX;
		if (y1 < arrMin[nx])
			arrMin[nx] = y1;
		if (y1 > arrMax[nx])
			arrMax[nx] = y1;
	}
	ASSERT(arrMax[0]>-CMAX/2);
	ASSERT(arrMin[0]<CMAX/2);
	ASSERT(arrMax[nDiap-1]>-CMAX/2);
	ASSERT(arrMin[nDiap-1]<CMAX/2);
	for (i = 1; i < nDiap-1; i++)
	{
		if (arrMin[i] > CMAX/2)
		{
			ASSERT(arrMin[i-1] < CMAX/2);
			ASSERT(arrMin[i+1] < CMAX/2);
			arrMin[i] = (arrMin[i-1]+arrMin[i+1])/2;
		}
		if (arrMax[i] < -CMAX/2)
		{
			ASSERT(arrMax[i-1] > -CMAX/2);
			ASSERT(arrMax[i+1] > -CMAX/2);
			arrMax[i] = (arrMax[i-1]+arrMax[i+1])/2;
		}
	}
	CArray<double,double&> arr;
	for (i = 0; i < nDiap; i++)
	{
		y1 = arrMax[i]-arrMin[i];
		ASSERT(y1 >= 0);
		if (y1 <= 0) continue;
		for (int j = 0; j < arr.GetSize(); j++)
		{
			if (y1 < arr[j])
			{
				arr.InsertAt(j, y1);
				break;
			}
		}
		if (j >= arr.GetSize())
			arr.Add(y1);
	}
	if (arr.GetSize() > 0)
	{
		int n = arr.GetSize()/2;
		return arr[n];
	}
	else
		return 0;
}

static double CalcAngle(CImageWrp& pImgWrp)
{
	double fSumX = 0., fSumY = 0.;
	double fSumXX = 0., fSumYY = 0., fSumXY = 0.;
	double fCalibrX = 1., fCalibrY = 1.;
	int iNum = 0;
	int nWidth = pImgWrp.GetWidth();
	int nHeight = pImgWrp.GetHeight();
	for (int y = 0; y < nHeight; y++)
	{
		byte *pbMask = pImgWrp.GetRowMask(y);
		for (int x = 0; x < nWidth; x++)
		{
			if (pbMask[x])
			{
				fSumX += x;
				fSumY += y;
				fSumXX += (double)x*x;
				fSumYY += (double)y*y;
				fSumXY += (double)x*y;
				iNum++;
			}
		}
	}
	double fCenterX = fSumX/iNum;
	double fCenterY = fSumY/iNum;
	double fMx  = ( fSumYY - 2*fCenterY*fSumY + fCenterY*fCenterY*iNum )
				   *fCalibrY*fCalibrY*fCalibrY*fCalibrX;
	double fMy  = ( fSumXX - 2*fCenterX*fSumX + fCenterX*fCenterX*iNum )
				   *fCalibrX*fCalibrX*fCalibrX*fCalibrY;
	double fMxy = ( fSumXY - fCenterY*fSumX - fCenterX*fSumY + fCenterX*fCenterY*iNum )
				   *fCalibrX*fCalibrY*fCalibrX*fCalibrY;
	if (!fMx) fMx = 1;
	if (!fMy) fMy = 1;
	if (!fMxy) fMxy = 1;
	double fAngle = ::atan2( 2*fMxy, fMy-fMx )/PI*90;
//	if( fAngle < 0 )
//		fAngle += 180;
	return fAngle;
}

CImageWrp RotateImage(CImageWrp &imgSrc, CPoint ptCenterOCS, double dAngle);

void CChromParam::CalcObjAngle()
{
	ROTATEPARAMS Rotate;
	m_sptrChromos->GetRotateParams(&Rotate);
	if (Rotate.bManual)
		dAngle = Rotate.dAngle;
	if (g_ShortAxisAlg == 1)
	{
		if (bOk) bOk = wo->Compare(COMPARE_ORIGOBJECT, 0, (INTERVALIMAGE *)pIntvl)==S_OK;
		if (!Rotate.bManual)
		{
			CChromoPerimeter *pPerim = new CChromoPerimeter;
			pPerim->MakeByInterval(pIntvl);
			if (g_RotateMode == 1)
			{
				delete pPerim->m_pChords;
				pPerim->m_pChords = new CChord[pPerim->m_nSize-1];
				for (int i = 0; i < pPerim->m_nSize-1; i++)
				{
					pPerim->m_pChords[i].acon = pPerim->m_ppt[i+1].y-pPerim->m_ppt[i].y;
					pPerim->m_pChords[i].bcon = pPerim->m_ppt[i+1].x-pPerim->m_ppt[i].x;
					pPerim->m_pChords[i].cl = _hypot(pPerim->m_pChords[i].acon, pPerim->m_pChords[i].bcon);
				}
			}
			int n1,n2,s,c;
			pPerim->CalcAngle(n1,n2,s,c);
			delete pPerim;
			if (c == 0)
			{
				c = 1;
				s *= 1000;
			}
			Rotate.dAngle = dAngle = atan((double)s / c);
		}
		CPoint ptImgOffs(pImgWrp.GetOffset());
		Rotate.ptOffs = CPoint(0,0);//pImgWrp.GetOffset();
#if 0
		m_ptCenter = CPoint(pIntvl->ptOffset.x+(pIntvl->szInt.cx-1)/2,pIntvl->ptOffset.y+(pIntvl->szInt.cy-1)/2);
		CImageWrp ImgWrp1 = RotateImage(pImgWrp, CPoint(m_ptCenter.x-ptImgOffs.x,m_ptCenter.y-ptImgOffs.y), dAngle);
		CIntervalImage *pIntvl1 = new CIntervalImage();
		pIntvl1->MakeByImage(ImgWrp1);
#else
		CImageWrp ImgWrp1;
		CIntervalImage *pIntvl1 = NULL;
		rottrans(pImgWrp,pIntvl,dAngle,ImgWrp1,pIntvl1,&m_ptCenter);
#endif
		Dump_Interval_Image(_T("Not-rotated image"), pIntvl);
		Dump_Interval_Image(_T("Rotated image"), pIntvl1);
		Rotate.ptCenter = CPoint(m_ptCenter.x-ptImgOffs.x,m_ptCenter.y-ptImgOffs.y);
		delete pIntvl;
		pIntvl = pIntvl1;
		pImgWrp = ImgWrp1;
		bRotated = true;
		if (bOk) bOk = wo->Compare(COMPARE_OBJECT, 0, (INTERVALIMAGE *)pIntvl)==S_OK;
		CPoint ptImgOffsR(pImgWrp.GetOffset());
		Rotate.ptOffsR = CPoint(ptImgOffsR.x-ptImgOffs.x,ptImgOffsR.y-ptImgOffs.y);
		m_sptrChromos->SetRotatedImage(NULL);
		m_sptrChromos->SetEqualizedRotatedImage(NULL);
	}
	else
	{
		if (!Rotate.bManual)
		{
			dAngle = CalcAngle(pImgWrp);
			dAngle = Rotate.dAngle = (dAngle/180)*PI;
		}
		Rotate.ptOffs = CPoint(0,0);//pImgWrp.GetOffset();
		Rotate.ptCenter.x = pImgWrp.GetWidth()/2;
		Rotate.ptCenter.y = pImgWrp.GetHeight()/2;
		Rotate.ptOffsR.x = Rotate.ptOffsR.y = -1;
		FindWidth(dAngle, dLen, pCntr);
		if (g_PtsDist > 1)
		{
			nPtsDist = g_PtsDist;
			nPoints = dLen/nPtsDist+1;
		}
		else if (g_PtsDist == 1)
		{
			nPoints = 10;
			nPtsDist = dLen/9.;
		}
		else
		{
			double d2 = FindAvWidth(dAngle, pCntr);
			nPtsDist = max(d2/4,6);
			nPoints = dLen/nPtsDist+1;
		}
	}
	m_sptrChromos->SetRotateParams(&Rotate);
	m_sptrChromos->SetOffset(Rotate.ptOffs);
}
