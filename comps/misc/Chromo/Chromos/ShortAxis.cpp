#include "StdAfx.h"
#include "Cromos.h"
#include "ChromParam.h"
#include "ChildObjects.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

int _xrange(CChromoAxis *pAxis)
{
	if (pAxis->m_Type == IntegerAxis)
	{
		int nMinX = pAxis->m_iptAxis[0].x;
		int nMaxX = nMinX;
		for (unsigned i = 0; i < pAxis->m_nSize; i++)
		{
			if (pAxis->m_iptAxis[i].x < nMinX)
				nMinX = pAxis->m_iptAxis[i].x;
			if (pAxis->m_iptAxis[i].x > nMaxX)
				nMaxX = pAxis->m_iptAxis[i].x;
		}
		return nMaxX - nMinX;
	}
	return 0;
}

int _xexcursion(CChromoAxis *pAxis)
{
	if (pAxis->m_Type == IntegerAxis)
	{
		int xexc = 0;
		for (unsigned i=1; i < pAxis->m_nSize; i++)
		{
			int xd = pAxis->m_iptAxis[i-1].x-pAxis->m_iptAxis[i].x;
			if (xd < 0)
				xd = -xd;
			xexc += xd;
		}
		return(xexc);
	}
	return 0;
}

int _axoutside(CIntervalImage *pIntImg, CChromoAxis *pAxis)
{
	if (pAxis->m_Type == IntegerAxis)
	{
		for (unsigned i = 2; i < pAxis->m_nSize-2; i++)
		{
			int nRow = pAxis->m_iptAxis[i].y-pIntImg->ptOffset.y;
			int nInts = pIntImg->prows[nRow].nints;
			if (nInts == 0)
				continue;
			if (pAxis->m_iptAxis[i].x < pIntImg->prows[nRow].pints[0].left+pIntImg->ptOffset.x ||
				pAxis->m_iptAxis[i].x > pIntImg->prows[nRow].pints[nInts-1].right+pIntImg->ptOffset.x)
				return 1;
		}
	}
	return 0;
}

double yrange(double *pfSegX, double *pfSegY, int nPoints)
{
	double dMinY = pfSegY[0];
	double dMaxY = pfSegY[0];
	for (int i = 1; i < nPoints; i++)
	{
		if (pfSegY[i] < dMinY) dMinY = pfSegY[i];
		if (pfSegY[i] > dMaxY) dMaxY = pfSegY[i];
	}
	return dMaxY-dMinY;
}

double yexcursion(double *pfSegX, double *pfSegY, int nPoints)
{
	double dSum = 0.;
	for (int i = 0; i < nPoints-1; i++)
		dSum += fabs(pfSegY[i]-pfSegY[i+1]);
	return dSum;
}

bool axisoutside(CImageWrp &pImgWrp, double *pfSegX, double *pfSegY, int nPoints)
{
	for (int i = 1; i < nPoints-1; i++)
	{
		int x = (int)pfSegX[i];
		int y = (int)pfSegY[i];
		byte *p = pImgWrp.GetRowMask(y);
		if (!p[x])
			return true;
	}
	return false;
}

CAxisContext::~CAxisContext()
{
	if (pfSegX) delete pfSegX;
	if (pfSegY) delete pfSegY;
}

void CAxisContext::Init(CImageWrp &pImgWrp, Contour *pCntr, CIntervalImage *pIntvl, int nPoints, int nPtsDist, double fAngle, double dLen)
{
	this->pImgWrp = pImgWrp;
	this->nPtsDist = nPtsDist;
	this->fAngle = fAngle;
	this->dLen = dLen;
	this->nPoints = nPoints;
//	nPoints = dLen/nPtsDist+1;
	this->pCntr = pCntr;//pImgWrp.GetContour(0);
	this->pIntvl = pIntvl;
	nCount = pCntr->nContourSize;
	pfSegX = new double[nPoints];
	pfSegY = new double[nPoints];
	ZeroMemory(pfSegX,sizeof(double)*nPoints);
	ZeroMemory(pfSegY,sizeof(double)*nPoints);
	fCos = ::cos(fAngle);
	fSin = ::sin(fAngle);
}

#define YPAR 6
#define XPAR 40
#define YEXCPAR	0.3

bool CAxisContext::IsCurve()
{
	if (g_AxisCurveRecognition == 1)
	{
		double a1,a2,a1_,a2_,da;
		a1 = atan2( pfSegY[0]-pfSegY[1], pfSegX[0]-pfSegX[1] )+fAngle;
		a1 = check_angle(a1);
		for (int i = 1; i < nPoints-1; i++)
		{
			a2 = atan2( pfSegY[i]-pfSegY[i+1], pfSegX[i]-pfSegX[i+1] )+fAngle;
			a2 = check_angle(a2);
			a1_ = min(a1,a2);
			a2_ = max(a1,a2);
			da = min(a2_-a1_,2*PI+a1_-a2_);
			if (da > PI/3) return true;
			a1 = a2;
		}
		return false;
	}
	else if (g_AxisCurveRecognition == 2)
	{
		double *pfSegX1 = new double[nPoints];
		double *pfSegY1 = new double[nPoints];
		for (int i = 0; i < nPoints; i++)
		{
			pfSegX1[i] = pfSegX[i]*fCos+pfSegY[i]*fSin;
			pfSegY1[i] = pfSegY[i]*fCos-pfSegX[i]*fSin;
		}
		bool bCurve = (dLen > XPAR && yrange(pfSegX1,pfSegY1,nPoints) > YPAR) || yexcursion(pfSegX1,pfSegY1,nPoints)/dLen > YEXCPAR;
		delete pfSegX1;
		delete pfSegY1;
		if (!bCurve) bCurve = axisoutside(pImgWrp,pfSegX,pfSegY,nPoints);
		return bCurve;
	}
	else return false;
}

/*
static void _LeftAndRight(CImageWrp &pImgWrp, int y, int &x1, int &x2)
{
	int nHeight = pImgWrp.GetHeight();
	int nWidth = pImgWrp.GetWidth();
	byte *pRowMask = pImgWrp.GetRowMask(y);
	x1 = -1;
	x2 = -1;
	for (int x = 0; x < nWidth; x++)
	{
		if (pRowMask[x])
		{
			x1 = x;
			break;
		}
	}
	for (; x < nWidth; x++)
	{
		if (!pRowMask[x])
		{
			x2 = x-1;
			break;
		}
	}
	if (x1 >= 0 && x2 == -1)
		x2 = nWidth-1;
}
*/
static bool _LeftAndRight(CIntervalImage *pIntvls, int y, int &x1, int &x2)
{
//	ASSERT(y < pIntvls->nrows);
	if (y >= pIntvls->nrows)
		return false;
	INTERVALROW *pIntRow = &pIntvls->prows[y];
	if (pIntRow->nints == 0)
		return false;
	x1 = pIntRow->pints[0].left;
	x2 = pIntRow->pints[pIntRow->nints-1].right;
	return true;
}

#define TIP 6
#define STEP 3
#define TIPSTRAIGHT 3

void CAxisContext::PrimaryAxis()
{
	int i;
	double fMinX,fMaxX,fMinY,fMaxY,fX,fY;
	// Find minimum and maximum X
	POINT pt;
	pt.x = pCntr->ppoints[0].x;
	pt.y = pCntr->ppoints[0].y;
	fMinX = fMaxX=pt.x*fCos+pt.y*fSin;
	fMinY = fMaxY=pt.y*fCos-pt.x*fSin;
	for(i = 1;i < nCount; i++)
	{
		pt.x = pCntr->ppoints[i].x;
		pt.y = pCntr->ppoints[i].y;
		fX = pt.x*fCos+pt.y*fSin;
		fY = pt.y*fCos-pt.x*fSin;
		if(fX<fMinX) fMinX=fX;
		if(fX>fMaxX) fMaxX=fX;
		if(fY<fMinY) fMinY=fY;
		if(fY>fMaxY) fMaxY=fY;
	}
#if defined(_DEBUG)
{
	int nHeight = pImgWrp.GetHeight();
	int nWidth = pImgWrp.GetWidth();
	for (int y = 0; y < nHeight; y++)
	{
		byte *pRowMask = pImgWrp.GetRowMask(y);
		int x1 = -1;
		int x2 = -1;
		for (int x = 0; x < nWidth; x++)
		{
			if (pRowMask[x])
			{
				x1 = x;
				break;
			}
		}
		for (; x < nWidth; x++)
		{
			if (!pRowMask[x])
			{
				x2 = x-1;
				break;
			}
		}
		if (x1 >= 0 && x2 == -1)
			x2 = nWidth-1;
		int x3 = (x1+x2)/2;
	}
	
	int nMinX = (int)(floor(fMinX));
	int nMaxX = (int)(ceil(fMaxX));
	for (int x = nMinX; x < nMaxX; x++)
	{
		double y1,y2;
		BOOL bFound=FALSE;
		for(int ii=0; ii < nCount; ii++)
		{
			int nx,ny;
			ContourGetPoint(pCntr,ii,nx,ny);
			pt.x=nx;
			pt.y=ny;
			fX = pt.x*fCos+pt.y*fSin;
			if(::fabs(fX-x)>1) continue;  // аналог != с округлением до 1
			fY = pt.y*fCos-pt.x*fSin;
			if(!bFound)
			{
				y1=y2=fY;
				bFound=TRUE;
			}
			if(fY<y1) y1=fY;
			if(fY>y2) y2=fY;
		}
		ASSERT(bFound);
	}
}
#endif
	if (g_CalcMethod == 2 || g_CalcMethod == 5 || g_CalcMethod == 6)
	{
		CPoint ptOffsInt = pIntvl->ptOffset;
		int nHeight = pIntvl->nrows;//pImgWrp.GetHeight();
		int nWidth = pImgWrp.GetWidth();
		int tip = (nHeight-1)/4;
		if (tip < 2)
			tip = 2;
		if (tip > TIP)
			tip = TIP;
		int step = STEP;
		int tipstraight = TIPSTRAIGHT;
		nPoints = 3 + (nHeight - 1 - 2*tip + step/2)/step;
		if (nPoints <= 3)
			nPoints = 4;
		if (nPoints < 2*(tipstraight+1))
			nPoints &= 0xfffe;
		nPoints &= 0xfffffffe;
		delete pfSegX;
		delete pfSegY;
		pfSegX = new double[nPoints];
		pfSegY = new double[nPoints];
		int i1 = 1; // vtx = pdom->vtx + 1;
		int nPoints2 = nPoints/2 - 1;
		int i2 = i1+nPoints-3;// wtx = vtx + nl - 3;
		for (i=0; i < nPoints2; i++)
		{
			int y1 = tip + i*step; 
			int y2 = nHeight - 1 - y1;
			int x1,x2;
			if (_LeftAndRight(pIntvl, y1, x1, x2))
			{
				pfSegX[i1]   = ptOffsInt.x+(x1+x2)/2;
				pfSegY[i1++] = ptOffsInt.y+y1;
			}
			if (_LeftAndRight(pIntvl, y2, x1, x2))
			{
				pfSegX[i2]   = ptOffsInt.x+(x1+x2)/2;
				pfSegY[i2--] = ptOffsInt.y+y2;
			}
			if (i == 0)
			{
				i += tipstraight-1;
				i1 += tipstraight-1;
				i2 -= tipstraight-1;
			}
		}
		int count = nPoints - i2 - 1;
		int gap = i2 - i1 + 1;
		if (gap > 0)
		{
			nPoints -= gap;
			i2++;
			while (count--)
			{
				pfSegX[i1] = pfSegY[i2];
				pfSegY[i1] = pfSegY[i2];
				i1++;
				i2++;
			}
		}
		nPoints2 = nPoints/2 - 1;
		int ts = tipstraight;
		if (nPoints2 < tipstraight)
			ts = nPoints2;
		i1 = 0; //vtx = pdom->vtx;
		i2 = 1; //wtx = vtx+1;
		int ix = i2+tipstraight; //xtx = wtx+tipstraight;
		if (nPoints2 <= tipstraight)
			ix = i2+nPoints-3;
		for (i = -1; i<ts; i++,i1++)
		{
			int l = i<0?ptOffsInt.y-2:int(pfSegY[i2]+i*step);
			//vtx->vtX = wtx->vtX + (l-wtx->vtY) * (wtx->vtX-xtx->vtX) / (wtx->vtY-xtx->vtY);
			pfSegX[i1] = pfSegX[i2]+int(l-pfSegY[i2])*int(pfSegX[i2]-pfSegX[ix])/int(pfSegY[i2]-pfSegY[ix]);
			pfSegY[i1] = l;//vtx->vtY = l;
		}
		i1 = nPoints-1;//vtx = pdom->vtx + nl - 1;
		i2 = i1-1;//wtx = vtx-1;
		ix = i2-tipstraight;//xtx = wtx-tipstraight;
		if (nPoints2 <= tipstraight) //if (nl2 <= tipstraight)
			ix = i2-nPoints+3;//xtx = wtx-nl+3;
		for (i = -1; i<ts; i++,i1--) //for (i = -1; i<ts; i++,vtx--)
		{
			int l = i<0?ptOffsInt.y+nHeight-1+2: int(pfSegY[i2]-i*step); //l = i<0? idom->lastln+2: wtx->vtY-i*step;
			//vtx->vtX = wtx->vtX + (l-wtx->vtY) * (wtx->vtX-xtx->vtX) / (wtx->vtY-xtx->vtY);
			pfSegX[i1] = pfSegX[i2] + int(l-pfSegY[i2])*int(pfSegX[i2]-pfSegX[ix])/int(pfSegY[i2]-pfSegY[ix]);
			pfSegY[i1] = l;//vtx->vtY = l;
		}
/*		CPoint ptOffsImg = pImgWrp.GetOffset();
		for (i = 0; i < nPoints; i++)
		{
			pfSegX[i] = int(pfSegX[i])-ptOffsImg.x;
			pfSegY[i] = int(pfSegY[i])-ptOffsImg.y;
		}
*/
		return;
	}
	// Define points of primary axis itself
	double fWidth = 0;
	int iWCount = 0;
	int nPoints1 = g_EndsAlg==4?nPoints:nPoints-1;
	for(i=0; i < nPoints; i++)
	{
		BOOL bFound=FALSE;
		double y1,y2;
		pfSegX[i] = fMinX + (fMaxX-fMinX)/(nPoints1)*i;
		for(int ii=0; ii < nCount; ii++)
		{
			int nx,ny;
			ContourGetPoint(pCntr,ii,nx,ny);
			pt.x=nx;
			pt.y=ny;
			fX = pt.x*fCos+pt.y*fSin;
			if(::fabs(fX-pfSegX[i])>1) continue;  // аналог != с округлением до 1
			fY = pt.y*fCos-pt.x*fSin;
			if(!bFound)
			{
				y1=y2=fY;
				bFound=TRUE;
			}
			if(fY<y1) y1=fY;
			if(fY>y2) y2=fY;
		}
		if(bFound==FALSE) TRACE("not found %i\n",ii);
		if(!bFound) return;
		pfSegY[i]=(y1+y2)/2;
		fWidth += fabs(y1-y2);
		iWCount++;
	}		
	// usually iWCount == nPoints but can be traps on bFound
	fWidth /= iWCount;
	//rotate axis
	for(i = 0; i<nPoints; i++ )
	{
		double	x = pfSegX[i];
		double	y = pfSegY[i];
		pfSegX[i] = x*fCos-y*fSin-pImgWrp.GetOffset().x;
		pfSegY[i] = y*fCos+x*fSin-pImgWrp.GetOffset().y;
	}
}

void CAxisContext::SecondaryAxis()
{
	if (g_AxisCorrect == 0)
	{
		int	iOptSeg = -1;
		double a1;
		double *pfSegX1 = new double[nPoints];
		double *pfSegY1 = new double[nPoints];
		for (int i=0; i<nPoints; i++)
		{
			pfSegX1[i] = pfSegX[i];
			pfSegY1[i] = pfSegY[i];
		}
		for (i=0; i<nPoints-2; i++)
		{
			double x,y;
			BOOL bRight,bLeft;
			CPoint  ptL,ptR;
			BYTE* colStr;
			BYTE ucPixel;
			a1 = atan2( pfSegY1[i]-pfSegY1[i+2], pfSegX1[i]-pfSegX1[i+2] );
			x=pfSegX1[i+1];
			y=pfSegY1[i+1];
			bLeft  = GetPointDir(pImgWrp,x,y,a1+PI/2,ptL);
			bRight = GetPointDir(pImgWrp,x,y,a1-PI/2,ptR);
			colStr = pImgWrp.GetRowMask((int)y);
			ucPixel = colStr[(int)x];
			if(ucPixel==0)
			{
				int nXt,nYt,nOffset;
				nOffset=int(y-tan(a1+PI/2)*x);
				double fDist=1000,fX=0,fY=0;
				for(nXt=0;nXt<pImgWrp.GetWidth()-1;nXt++)
				{
					nYt=int(nXt*tan(a1+PI/2)+nOffset);
					nYt=min(max(0,nYt),pImgWrp.GetHeight()-1);
					colStr = pImgWrp.GetRowMask(nYt);
					ucPixel = colStr[nXt];
					if(ucPixel!=0) 
					{
						if(fDist>GetLen(x-nXt,y-nYt))
							{ 
								fDist=GetLen(x-(double)nXt,y-(double)nYt); 
								fX=nXt;
								fY=nYt;
							}
					}
				}
				if(fDist<1000)
				{
					x=pfSegX[i+1]=fX;
					y=pfSegY[i+1]=fY;
					bLeft  = GetPointDir(pImgWrp,x,y,a1+PI/2,ptL);
					bRight = GetPointDir(pImgWrp,x,y,a1-PI/2,ptR);
				}

			}
			if(bLeft==TRUE&&bRight==TRUE)
			{
				pfSegX[i+1]=(ptR.x+ptL.x)/2;
				pfSegY[i+1]=(ptR.y+ptL.y)/2;
			}
		}
		delete pfSegX1;
		delete pfSegY1;
	}
	else if (g_AxisCorrect == 1)
	{
		// Immitate ingeger
		int i,j;
		for (i = 0; i < g_AxisIterations; i++)
		{
			double dPrevX = pfSegX[0];
			double dPrevY = pfSegY[0];
			for (j = 1; j < nPoints-1; j++)
			{
				double dCurX = pfSegX[j];
				pfSegX[j] = (dPrevX+2.*dCurX+pfSegX[j+1])/4.;
				dPrevX = dCurX;
				double dCurY = pfSegY[j];
				pfSegY[j] = (dPrevY+2.*dCurY+pfSegY[j+1])/4.;
				dPrevY = dCurY;
			}
		}
		for (j = 1; j < nPoints-1; j++)
		{
			int t;
			t = int(pfSegX[j]*8.);
			pfSegX[j] = double(t)/8.;
			t = int(pfSegY[j]*8.);
			pfSegY[j] = double(t)/8.;
		}
	};
}

static bool PrimaryAxis(CIntervalImage *pIntvl, CPoint ptOffsInt, int nHeight, int nWidth, CChromoAxis &ChromoAxis)
{
	int tip = (nHeight-1)/4,i;
	if (tip < 2)
		tip = 2;
	if (tip > TIP)
		tip = TIP;
	int step = STEP;
	int tipstraight = TIPSTRAIGHT;
	int nPoints = 3 + (nHeight - 1 - 2*tip + step/2)/step;
	if (nPoints <= 3)
		nPoints = 4;
	if (nPoints < 2*(tipstraight+1))
		nPoints &= 0xfffe;
	nPoints &= 0xfffffffe;
	POINT *ppt = new POINT[nPoints];
//	for (i = 0; i < nPoints; i++) ppt[i] = ptOffsInt;
	for (i = 0; i < nPoints; i++) ppt[i].x = ppt[i].y = -10000;
	int i1 = 1; // vtx = pdom->vtx + 1;
	int nPoints2 = nPoints/2 - 1;
	int i2 = i1+nPoints-3;// wtx = vtx + nl - 3;
	int y1 = tip, y2 = nHeight-1;
	for (i = 0; i < nPoints2; i++)
	{
		y1 = tip + i*step; 
		y2 = nHeight - 1 - y1;
		int x1,x2;
		if (_LeftAndRight(pIntvl, y1, x1, x2))
		{
			ppt[i1].x   = ptOffsInt.x+(x1+x2)/2;
			ppt[i1++].y = ptOffsInt.y+y1;
			if (i1 == 2)
			{
				i += tipstraight-1;
				i1 += tipstraight-1;
			}
		}
		if (_LeftAndRight(pIntvl, y2, x1, x2))
		{
			ppt[i2].x   = ptOffsInt.x+(x1+x2)/2;
			ppt[i2--].y = ptOffsInt.y+y2;
			if (i2 == nPoints-3)
			{
				i2 -= tipstraight-1;
			}
		}
/*		if (i == 0)
		{
			i += tipstraight-1;
			i1 += tipstraight-1;
			i2 -= tipstraight-1;
		}*/
	}
/*	{
		POINT *ppt1 = new POINT[nPoints];
		{
		POINT *ppt = ppt1;
		for (i = 0; i < nPoints; i++) ppt[i].x = ppt[i].y = -10000;
		int i1 = 1; // vtx = pdom->vtx + 1;
		int nPoints2 = nPoints/2 - 1;
		int i2 = i1+nPoints-3;// wtx = vtx + nl - 3;
		int y1 = tip, y2 = nHeight-1;
		for (i = 0; i < nPoints2; i++)
		{
			y1 = tip + i*step; 
			y2 = nHeight - 1 - y1;
			int x1,x2;
			if (_LeftAndRight(pIntvl, y1, x1, x2))
			{
				ppt[i1].x   = ptOffsInt.x+(x1+x2)/2;
				ppt[i1++].y = ptOffsInt.y+y1;
			}
			if (_LeftAndRight(pIntvl, y2, x1, x2))
			{
				ppt[i2].x   = ptOffsInt.x+(x1+x2)/2;
				ppt[i2--].y = ptOffsInt.y+y2;
			}
			if (i == 0)
			{
				i += tipstraight-1;
				i1 += tipstraight-1;
				i2 -= tipstraight-1;
			}
		}
		}
		int iFound = -1;
		for (i = 0; i <nPoints; i++)
			iFound = ppt[i].x==ppt1[i].x&&ppt[i].y==ppt1[i].y?iFound:i;
		ASSERT(iFound==-1);
		delete ppt1;

	}*/
	int count = nPoints - i2 - 1;
	int gap = i2 - i1 + 1;
	if (gap > 0)
	{
		nPoints -= gap;
		i2++;
		while (count--)
		{
			ppt[i1] = ppt[i2];
			i1++;
			i2++;
		}
	}
	nPoints2 = nPoints/2 - 1;
	int ts = tipstraight;
	if (nPoints2 < tipstraight)
		ts = nPoints2;
	i1 = 0; //vtx = pdom->vtx;
	i2 = 1; //wtx = vtx+1;
	int ix = i2+tipstraight; //xtx = wtx+tipstraight;
	if (nPoints2 <= tipstraight)
		ix = i2+nPoints-3;
	if (ppt[i2].x == -10000 || ppt[i2].y == -10000 || ppt[ix].x == -10000 || ppt[ix].y == -10000)
	{
		delete ppt;
		return false;
	}
	for (i = -1; i<ts; i++,i1++)
	{
		int l = i<0?ptOffsInt.y-2:ppt[i2].y+i*step;
		if (ppt[i2].y == ppt[ix].y)
			ppt[i1].x = ppt[i2].x;
		else
			ppt[i1].x = ppt[i2].x+(l-ppt[i2].y)*(ppt[i2].x-ppt[ix].x)/(ppt[i2].y-ppt[ix].y);
		ppt[i1].y = l;
	}
	i1 = nPoints-1;//vtx = pdom->vtx + nl - 1;
	i2 = i1-1;//wtx = vtx-1;
	ix = i2-tipstraight;//xtx = wtx-tipstraight;
	if (nPoints2 <= tipstraight) //if (nl2 <= tipstraight)
		ix = i2-nPoints+3;//xtx = wtx-nl+3;
	if (ppt[i2].x == -10000 || ppt[i2].y == -10000 || ppt[ix].x == -10000 || ppt[ix].y == -10000)
	{
		delete ppt;
		return false;
	}
	for (i = -1; i<ts; i++,i1--) //for (i = -1; i<ts; i++,vtx--)
	{
		int l = i<0?ptOffsInt.y+nHeight-1+2: ppt[i2].y-i*step;
		if (ppt[i2].y==ppt[ix].y)
			ppt[i1].x = ppt[i2].x;
		else
			ppt[i1].x = ppt[i2].x + (l-ppt[i2].y)*(ppt[i2].x-ppt[ix].x)/(ppt[i2].y-ppt[ix].y);
		ppt[i1].y = l;
	}
	ChromoAxis.Init(IntegerAxis, nPoints, ppt);
	delete ppt;
	ChromoAxis.m_ptOffset = CPoint(0,0);
	ChromoAxis.m_nScale = 1;
	return true;
}

static void SecondaryAxis(CChromoAxis &ChromoAxis)
{
	if (ChromoAxis.m_Type == IntegerAxis)
	{
		int i,j;
		int nPoints = ChromoAxis.m_nSize;
		for (i = 0; i < g_AxisIterations; i++)
		{
			POINT ptPrev = ChromoAxis.m_iptAxis[0];
			ChromoAxis.m_iptAxis[0].x <<= 2;
			ChromoAxis.m_iptAxis[0].y <<= 2;
			for (j = 1; j < nPoints-1; j++)
			{
				POINT ptCur = ChromoAxis.m_iptAxis[j];
				ChromoAxis.m_iptAxis[j].x = ptPrev.x+2*ptCur.x+ChromoAxis.m_iptAxis[j+1].x;
				ChromoAxis.m_iptAxis[j].y = ptPrev.y+2*ptCur.y+ChromoAxis.m_iptAxis[j+1].y;
				ptPrev = ptCur;
			}
			ChromoAxis.m_iptAxis[nPoints-1].x <<= 2;
			ChromoAxis.m_iptAxis[nPoints-1].y <<= 2;
		}
		int n = 2*g_AxisIterations-3;
		for (j = 0; j < nPoints; j++)
		{
			ChromoAxis.m_iptAxis[j].x >>= n;
			ChromoAxis.m_iptAxis[j].y >>= n;
		}
		ChromoAxis.m_nScale = 8;
	}
}

static void InitDrawAxis(IChromosome *pChromo, CChromoAxis &ShortAxis, CChromParam *pParam, bool bSkel)
{
	CChromoAxis AxisR(ShortAxis);
	AxisR.Scale(1./double(ShortAxis.m_nScale));
	FPOINT fpCen,fpOrg,fpOrgR;
	fpOrgR.x = -pParam->pImgWrp.GetOffset().x;
	fpOrgR.y = -pParam->pImgWrp.GetOffset().y;
	fpOrg.x = -pParam->m_ptOffs.x;
	fpOrg.y = -pParam->m_ptOffs.y;
	fpCen.x = pParam->pIntvl->ptOffset.x+(pParam->pIntvl->szInt.cx-1)/2;
	fpCen.y = pParam->pIntvl->ptOffset.y+(pParam->pIntvl->szInt.cy-1)/2;
	CChromoAxis Axis(AxisR);
	AxisR.Offset(fpOrgR); // Now AxisR in rotated object coordinates
	pChromo->SetAxis(pParam->bRotated?AT_ShortAxis|AT_Rotated:AT_ShortAxis, AxisR.m_nSize, AxisR.m_iptAxis, FALSE);
//	Axis.Rotate(fpCen,-pParam->dAngle);
//	Axis.Offset(fpOrg); // Axis in object coordinates
//	pChromo->SetAxis(AT_ShortAxis, Axis.m_nSize, Axis.m_iptAxis, FALSE);
	if (g_ShowSkelAxis)
	{
		if (bSkel)
			pChromo->SetColor(RGB(0, 128, 0), 0);
		else
			pChromo->SetColor(RGB(255, 128, 0), 0);
	}
}

CChromoAxis *SkelAxis(CIntervalImage *pSrc, CImageWrp &pImgWrp);

#define _XPAR 6
#define _YPAR 40
#define _XEXCPAR	30


bool CChromParam::CalcShortAxis()
{
	// If manual axis exists, use it
	BOOL bShortAxisManual;
	long nShortAxisSize;
//	m_sptrChromos->GetAxisSize(bRotated?AT_ShortAxis|AT_Rotated:AT_ShortAxis, &nShortAxisSize, &bShortAxisManual);
	m_sptrChromos->GetAxisSize(AT_ShortAxis, &nShortAxisSize, &bShortAxisManual);
	if (bShortAxisManual && nShortAxisSize)
	{
		m_sptrChromos->GetAxis2(AT_ShortAxis, &m_ShortAxis, NULL);
		m_ShortAxis.Offset(m_ShortAxis.GetOffset()); // Will be needed for profile calculation
		if (bRotated) m_ShortAxis.Rotate(CFPoint(m_ptCenter), dAngle);
		m_ShortAxis.Scale(8/m_ShortAxis.m_nScale);
		m_ShortAxis.Convert(IntegerAxis);
		return true;
	}
	// If not, calculate it.
	if (g_ShortAxisAlg == 1)
	{
		bool bSkel = false;
		if (!PrimaryAxis(pIntvl,pIntvl->ptOffset,pIntvl->nrows,pImgWrp.GetWidth(),m_ShortAxis))
			return false;
		if (g_AxisCurveRecognition == 1)
		{
			int yr = pIntvl->szInt.cy-1;
			if ((yr > _YPAR && _xrange(&m_ShortAxis) > _XPAR) ||
					100*_xexcursion(&m_ShortAxis)/yr > _XEXCPAR ||
					_axoutside(pIntvl,&m_ShortAxis) )
			{
				CChromoAxis *pSkel = SkelAxis(pIntvl, pImgWrp);
				if (pSkel)
				{
					m_ShortAxis.Init(pSkel->m_Type, pSkel->m_nSize, pSkel->m_iptAxis);
					bSkel = true;
					delete pSkel;
				}
			}
		}
		if (bOk) bOk = wo->Compare(COMPARE_SHORTAXIS, 1, (CHROMOAXIS *)&m_ShortAxis)==S_OK;
		SecondaryAxis(m_ShortAxis);
		bOk = bOk?wo->Compare(COMPARE_SMOOTHSHORTAXIS, 1, (CHROMOAXIS *)&m_ShortAxis)==S_OK:false;
		InitDrawAxis(m_sptrChromos,m_ShortAxis,this,bSkel);
		return true;
	}
	// Old short axis algorythms: VT4, etc.
	CAxisContext c;
	c.Init(pImgWrp,pCntr,pIntvl,nPoints,nPtsDist,dAngle,dLen);
	c.PrimaryAxis();
	if (g_AxisCorrectPrimEnds) c.CorrectEnds(CAxisContext::CEPrimary);
	c.SecondaryAxis();
	if (g_AxisCorrectSecEnds) c.CorrectEnds(CAxisContext::CESecondary);
	c.m_bCurve = c.IsCurve();
	if (c.m_bCurve)
	{
		c.SkeletonAxis();
		if (g_AxisCorrectSkelEnds && g_AxisCorrectPrimEnds) c.CorrectEnds(CAxisContext::CESkeleton); // Now correct ends of new axis
		c.SecondaryAxis();
		if (g_AxisCorrectSkelEnds && g_AxisCorrectSecEnds) c.CorrectEnds(CAxisContext::CESkeleton); // Now correct ends of new axis
	}
	// Save data for drawing
	m_ShortAxis.Init(IntegerAxis, c.nPoints);
	for(int i = 0; i < c.nPoints; i++)
	{
		if (g_EndsAlg != 4 && g_EndsAlg != 5)
		{
			c.pfSegX[i] = max(0,c.pfSegX[i]);
			c.pfSegX[i] = min(pImgWrp.GetWidth()-1,c.pfSegX[i]);
			c.pfSegY[i] = max(0,c.pfSegY[i]);
			c.pfSegY[i] = min(pImgWrp.GetHeight()-1,c.pfSegY[i]);
		}
		m_ShortAxis.m_iptAxis[i].x = int((c.pfSegX[i]+pImgWrp.GetOffset().x)*8);
		m_ShortAxis.m_iptAxis[i].y = int((c.pfSegY[i]+pImgWrp.GetOffset().y)*8);
	}
	m_ShortAxis.m_nScale = 8;
	InitDrawAxis(m_sptrChromos,m_ShortAxis,this,c.m_bCurve);
	return true;
}

