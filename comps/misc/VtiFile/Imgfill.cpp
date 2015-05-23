#include "StdAfx.h"
#include "AppDefs.h"
#include "ImgFill.h"
#include "math.h"
#include "object.h"

#pragma optimize( "g", off )

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CObservable,CPtrArray);
IMPLEMENT_DYNAMIC(CInteger,CObject);

double FDist(CDblPoint pt1, CDblPoint pt2)
{
	return sqrt(double((pt1.x-pt2.x)*(pt1.x-pt2.x)+(pt1.y-pt2.y)*(pt1.y-pt2.y)));
}

double FDistLine(CDblPoint pt, CDblPoint ptLin1, CDblPoint ptLin2)
{
	double d1 = FDist(pt,ptLin1);
	double d2 = FDist(pt,ptLin2);
	double l = FDist(ptLin1,ptLin2);
	if (l == 0.) return d1;
	if (d1 < d2)
	{
		double t = (d2*d2+l*l-d1*d1)/(2*l);
		double s =  sqrt(d2*d2-t*t);
		return s;
	}
	else
	{
		double t = (d1*d1+l*l-d2*d2)/(2*l);
		double s = sqrt(d1*d1-t*t);
		return s;
	}
}

void FDistLine(CDblPoint pt, CDblPoint ptLin1, CDblPoint ptLin2, double *pd)
{
	double d1 = FDist(pt,ptLin1);
	double d2 = FDist(pt,ptLin2);
	double l = FDist(ptLin1,ptLin2);
	if (l == 0.)
	{
		*pd = d1;
		return;
	}
	if (d1 < d2)
	{
		double t = (d2*d2+l*l-d1*d1)/(2*l);
		double s =  sqrt(d2*d2-t*t);
		*pd = s;
	}
	else
	{
		double t = (d1*d1+l*l-d2*d2)/(2*l);
		double s = sqrt(d1*d1-t*t);
		*pd = s;
	}
}

void Rotate(CDblPoint ptCen, CPoint *pt, int nPoints,
	double sinf, double cosf)
{
	for (int i = 0; i < nPoints; i++)
	{
		int x = (int)floor(ptCen.x + (pt[i].x - ptCen.x) * cosf - (pt[i].y - ptCen.y) * sinf);
		int y = (int)floor(ptCen.y + (pt[i].x - ptCen.x) * sinf + (pt[i].y - ptCen.y) * cosf);
		pt[i].x = x;
		pt[i].y = y;
	}
}

void Rotate(CDblPoint ptCen, CshortPoint *pt, int nPoints,
	double sinf, double cosf)
{
	for (int i = 0; i < nPoints; i++)
	{
		int x = (int)floor(ptCen.x + (pt[i].x - ptCen.x) * cosf - (pt[i].y - ptCen.y) * sinf);
		int y = (int)floor(ptCen.y + (pt[i].x - ptCen.x) * sinf + (pt[i].y - ptCen.y) * cosf);
		pt[i].x = x;
		pt[i].y = y;
	}
}

void Resize(CPoint TopLeft, CshortPoint *pt, int nPoints, double fScaleX, double fScaleY)
{
	for (int i = 0; i < nPoints; i++)
	{
		int x = TopLeft.x + (int)floor((pt[i].x - TopLeft.x) * fScaleX);
		int y = TopLeft.y + (int)floor((pt[i].y - TopLeft.y) * fScaleY);
		pt[i].x = x;
		pt[i].y = y;
	}
}

void CheckRect(CPoint *pt, CRect rcInit)
{
	if (abs(pt[0].y-rcInit.top) <= 1 &&
		abs(pt[1].y-rcInit.top) <= 1 &&
		abs(pt[2].y-rcInit.bottom) <= 1 &&
		abs(pt[3].y-rcInit.bottom) <= 1 &&
		abs(pt[3].x-rcInit.left) <= 1 &&
		abs(pt[0].x-rcInit.left) <= 1 &&
		abs(pt[1].x-rcInit.right) <= 1 &&
		abs(pt[2].x-rcInit.right) <= 1)
	{
		pt[0].y = rcInit.top;
		pt[1].y = rcInit.top;
		pt[2].y = rcInit.bottom;
		pt[3].y = rcInit.bottom;
		pt[0].x = rcInit.left;
		pt[1].x = rcInit.right;
		pt[2].x = rcInit.right;
		pt[3].x = rcInit.left;
	}
}

void InitPoints(CRect rc, CPoint *pt)
{
	pt[0].x = rc.left;
	pt[0].y = rc.top;
	pt[1].x = rc.right;
	pt[1].y = rc.top;
	pt[2].x = rc.right;
	pt[2].y = rc.bottom;
	pt[3].x = rc.left;
	pt[3].y = rc.bottom;
	pt[4].x = rc.left;
	pt[4].y = rc.top;
}

double AngleFromPt(CDblPoint pt)
{
	double l = sqrt((double)(pt.x*pt.x+pt.y*pt.y));
	double x0 = ((double)pt.x)/l; // cos
	double y0 = ((double)pt.y)/l; // sin
	ASSERT(x0 <= 1.0 && x0 >= -1.0);
	if (x0 > 1.0) x0 = 1.0;
	if (x0 < -1.0) x0 = -1.0;
	ASSERT(y0 <= 1.0 && y0 >= -1.0);
	if (y0 > 1.0) y0 = 1.0;
	if (y0 < -1.0) y0 = -1.0;
	double fAngle = acos(x0);
	if (y0 < 0)
		fAngle = 2 * PI - fAngle;
	return fAngle;
}

double AngleFromSin(double fSin, double fCos)
{
	ASSERT(fCos <= 1.0 && fCos >= -1.0);
	ASSERT(fSin <= 1.0 && fSin >= -1.0);
	double fAngle = acos(fCos);
	if (fSin < 0)
		fAngle = 2 * PI - fAngle;
	return fAngle;
}


BOOL IsClockwise(CDblPoint ptCen, CDblPoint pt1, CDblPoint pt2)
{
	CDblPoint pt1d(pt1.x-ptCen.x,pt1.y-ptCen.y);
	CDblPoint pt2d(pt2.x-ptCen.x,pt2.y-ptCen.y);
	double fAng1 = AngleFromPt(pt1d);
	double fAng2 = AngleFromPt(pt2d);
	if (fAng2 > fAng1)
		return fAng2 - fAng1 > PI;
	else return fAng1 - fAng2 < PI;
}

void CalcAngle(CDblPoint ptCen, CPoint pt1, CPoint pt2,
	double &sinf, double &cosf)
{
	double r1 = FDist(ptCen,pt1);
	double r2 = FDist(ptCen,pt2);
	double d  = FDist(pt1,pt2);
	cosf = (r1*r1+r2*r2-d*d)/(2*r1*r2);
	ASSERT(cosf<=1.0&&cosf>=-1.0);
	if (cosf < -1.0) cosf = -1.0;
	if (cosf > 1.0) cosf = 1.0;
	sinf = sqrt(1-(cosf*cosf));
	if (IsClockwise(ptCen,pt1,pt2))
		sinf = -sinf;
	ASSERT(sinf<=1.0&&sinf>=-1.0);
	if (sinf < -1.0) sinf = -1.0;
	if (sinf > 1.0) sinf = 1.0;
}

CPoint CalcCenter(CPoint *pt)
{
	return CPoint((pt[0].x+pt[2].x)/2,(pt[0].y+pt[2].y)/2);
}

void CFill::DoFill(C8Image &img, CRect &rcFill, int x, int yc, int nTestCol,
	int nSetCol)
{
	int	x1, x2, x1o, x2o, y, xc, x1c, x2c;
	CRect rcResult;

	x1= x2 = x;
	x2 = x+1;
	y = yc;
			//Определяем x1
	while(1)
	{
		if(x1 < rcFill.left || img[y][x1] != nTestCol)
		{
			x1++;
			break;
		}

		img[y][x1] = nSetCol;
		x1--;
	}
	rcResult.left = x1;
	
				//Определяем x2
	while(1)
	{
		if(x2 > rcFill.right || img[y][x2] != nTestCol)
		{
			x2--;
			break;
		}

		img[y][x2] = nSetCol;
		x2++;
	}
	rcResult.right = x2;

	x1c =  x1;
	x2c = x2;
				//поднимаемся вверх по y
	while(1)
	{
		y--;
		x1o = x1;
		x2o = x2;

		if(y < rcFill.top)
			break;

		for(xc = max(x1o-1, 0); xc<min(x2o+2, rcFill.right+1); xc++)
			if(img[y][xc] == nTestCol)
				break;
		if(xc==min(x2o+2, rcFill.right+1))
			break;

		x1 = xc;
		x2 = xc+1;

				//Определяем x1
		while(1)
		{
			if(x1 < rcFill.left || img[y][x1] != nTestCol)
			{
				x1++;
				break;
			}

			img[y][x1] = nSetCol;
			x1--;
		}
		if (x1 < rcResult.left)
			rcResult.left = x1;
	
				//Определяем x2
		while(1)
		{
			if(x2 > rcFill.right || img[y][x2] != nTestCol)
			{
				x2--;
				break;
			}

			img[y][x2] = nSetCol;
			x2++;
		}
		if (x2 > rcResult.right)
			rcResult.right = x2;
				//сканируем ответвления вниз 
		for(xc = max(x1-1, 0); xc <min(x2+2, rcFill.right+1); xc++)
			if(img[y+1][xc] == nTestCol)
				DoFill(img, rcFill, xc, y+1, nTestCol, nSetCol);
						//сканируем ответвления вверх 

		for(xc = max(x1o-1, 0); xc <min(x2o+2, rcFill.right+1); xc++)
			if(img[y][xc] == nTestCol)
				DoFill(img, rcFill, xc, y, nTestCol, nSetCol);
	}
	rcResult.top = y + 1;

	x1 = x1c;
	x2 = x2c;
	y = yc;

					//спускаемся вниз по y
	while(1)
	{
		x1o = x1;
		x2o = x2;
		y++;
		if(y > rcFill.bottom)
			break;

		for(xc = max(x1o-1, 0); xc<min(x2o+2, rcFill.right+2); xc++)
			if(img[y][xc] == nTestCol)
				break;
		if( xc == min(x2o+2, rcFill.right+2))
			break;

		x1 = xc;
		x2 = xc+1;

				//Определяем x1
		while(1)
		{
			if(x1 < rcFill.left || img[y][x1] != nTestCol)
			{
				x1++;
				break;
			}

			img[y][x1] = nSetCol;
			x1--;
		}
		if (x1 < rcResult.left)
			rcResult.left = x1;
	
				//Определяем x2
		while(1)
		{
			if(x2 > rcFill.right || img[y][x2] != nTestCol)
			{
				x2--;
				break;
			}

			img[y][x2] = nSetCol;
			x2++;
		}
		if (x2 > rcResult.right)
			rcResult.right = x2;
				//сканируем ответвления вниз на текущей строке
		for(xc = max(x1o-1, rcFill.left); xc <min(x2o+2, rcFill.right+1); xc++)
			if(img[y][xc] == nTestCol)
				DoFill(img, rcFill, xc, y, nTestCol, nSetCol);
						//сканируем ответвления вверх на предъидущей строке
		for(xc = max(x1-1, rcFill.left); xc <min(x2+2, rcFill.right+1); xc++)
			if(img[y-1][xc] == nTestCol)
				DoFill(img, rcFill, xc, y-1, nTestCol, nSetCol);
	}
	rcResult.bottom = y - 1;
	CRect rc(m_rcMark);
	if (rc == NORECT)
		m_rcMark = rcResult;
	else
		m_rcMark = CRect(min(rc.left,rcResult.left),
			min(rc.top,rcResult.top), max(rc.right,rcResult.right),
			max(rc.bottom,rcResult.bottom));
//	m_rcMark = UnionRect(&rc,&rcResult);
}


void CFill::FastFloodFill(C8Image &img, CRect &rcFill, int x, int y, int nTestClr, int nSetClr)
{
	DoFill(img,rcFill,x,y,nTestClr,nSetClr);
}

CPoint CFill::FindFirstPoint(C8Image &img, CRect rcFill, int nTestCol)
{
	for (int y = rcFill.top; y <= rcFill.bottom; y++)
	{
		int num = 0;
		int x,xp = -10;
		int xb = -1;
		for (x = rcFill.left; x <= rcFill.right; x++)
		{
			int nCur = img[y][x];
			if (nCur == nTestCol)
			{
				if (x > xp + 1)
				{
					if (num == 0)
						xb = x;
					num++;
				}
				xp = x;
			}
		}
		if (num == 2)
		{
			return CPoint((xb+xp)/2,y);
		}
	}
	return CPoint(-1,-1);
}

void CFill::DoBordFill(C8Image &img, CRect &rcFill, int x, int yc, int nTestCol,
	int nSetCol)
{
}

void CFill::FastBordFill(C8Image &img, CRect &rcFill, int nTestClr, int nSetClr)
{
	CPoint pt(FindFirstPoint(img,rcFill,nTestClr));
	if (pt == CPoint(-1,-1)) return;
	DoBordFill(img,rcFill,pt.x,pt.y,nTestClr,nSetClr);
}

#define BIN_BACKGROUND        0   // Background in binary image
#define BIN_TEMP_CONT1        253 // Used for draw inside Fill contour
#define BIN_TEMP_CONT2        254 // Used for draw inside Fill contour
#define BIN_CONTOUR           255 // Contour in binary image

static int ChangePel(int nPel, int nOp)
{
	if (nPel == BIN_TEMP_CONT1)
	{
		if (nOp == BIN_TEMP_CONT1)
			return BIN_TEMP_CONT2;
		else return BIN_TEMP_CONT1;
	}
	else return nOp;
}

void FillContour(C8Image &img, CContour *c, CPoint ptOffs, BYTE nFill, BYTE nContour)
{
	CContour Cont;
	Cont.Attach(*c);
	CRect rc = Cont.GetRect();
	ASSERT(rc.left >= ptOffs.x);
	ASSERT(rc.top >= ptOffs.y);
	CPoint ptFirst(Cont[0].x,Cont[0].y);
	Cont.AddSegment(ptFirst);
	int nSize = Cont.GetSize();
	CPoint ptPrev(Cont[nSize-1].x,Cont[nSize-1].y);
	for (int i = 0; i < nSize; i++)
	{
		CPoint pt(Cont[i].x,Cont[i].y);
		ASSERT(rc.PtInRect(pt));
		if (pt.y == ptPrev.y)
			img[pt.y-ptOffs.y][pt.x-ptOffs.x] = ChangePel(img[pt.y-ptOffs.y][pt.x-ptOffs.x],
				BIN_TEMP_CONT2);
		else if (pt.y > ptPrev.y)
			img[pt.y-ptOffs.y][pt.x-ptOffs.x] = ChangePel(img[pt.y-ptOffs.y][pt.x-ptOffs.x],
				BIN_TEMP_CONT1);
		else
		{
			img[ptPrev.y-ptOffs.y][ptPrev.x-ptOffs.x] = ChangePel(img[ptPrev.y-ptOffs.y]
				[ptPrev.x-ptOffs.x], BIN_TEMP_CONT1);
			img[pt.y-ptOffs.y][pt.x-ptOffs.x] = ChangePel(img[pt.y-ptOffs.y][pt.x-ptOffs.x],
				BIN_TEMP_CONT2);
		}
		ptPrev = pt;
	}
	BOOL bContext = FALSE;
	for (int y = rc.top-ptOffs.y; y < rc.bottom-ptOffs.y; y++)
	{
//		ASSERT( !bContext );
		bContext = FALSE;
		for (int x = rc.left-ptOffs.x; x < rc.right-ptOffs.x; x++)
		{
			BYTE byVal = img[y][x];
			if (byVal == BIN_TEMP_CONT1)
			{
				bContext = !bContext;
				img[y][x] = nFill;
			}
			else if (byVal == BIN_TEMP_CONT2)
				img[y][x] = nFill;
			else if (bContext)
				img[y][x] = nFill;
		}
	}
	for (i = 0; i < nSize; i++)
	{
		CPoint pt(Cont[i].x,Cont[i].y);
		ASSERT(rc.PtInRect(pt));
		img[pt.y-ptOffs.y][pt.x-ptOffs.x] = nContour;
	}
}

template<class COLORREPR> void FillByMask(CImage<COLORREPR> &img,
	C8Image &imgMask, C8Image &imgBigMask, COLORREF Color, CRect rc, CPoint ptOffsMask)
{
	if (img.IsTrueColor())
	{
		for (int y = rc.top; y < rc.bottom; y++)
		{
			COLORREPR *pDst = img[y]+rc.left*img.GetIncr();
			BYTE *pMask = imgMask[y+ptOffsMask.y-rc.top]+ptOffsMask.x;
			BYTE *pBigMask = imgBigMask.IsEmpty()?NULL:
				imgBigMask[y-rc.top];
			for (int x = rc.left; x < rc.right; x++)
			{
				if (*pMask == 255 && (!pBigMask || *pBigMask != 255))
				{
					pDst[0] = CImage<COLORREPR>::Convert((BYTE)GetBValue(Color));
					pDst[1] = CImage<COLORREPR>::Convert((BYTE)GetGValue(Color));
					pDst[2] = CImage<COLORREPR>::Convert((BYTE)GetRValue(Color));
				}
				pMask++;
				if (pBigMask) pBigMask++;
				pDst += 3;
			}
		}
	}
	else
	{
		for (int y = rc.top; y < rc.bottom; y++)
		{
			COLORREPR *pDst = img[y]+rc.left;
			BYTE *pMask = imgMask[y+ptOffsMask.y-rc.top]+ptOffsMask.x;
			BYTE *pBigMask = imgBigMask.IsEmpty()?NULL:imgBigMask[y-rc.top];
			for (int x = rc.left; x < rc.right; x++)
			{
				if (*pMask == 255 && (!pBigMask || *pBigMask != 255))
					*pDst = CImage<COLORREPR>::Convert((BYTE)Color);
				pMask++;
				if (pBigMask) pBigMask++;
				pDst++;
			}
		}
	}
}

void FillByMask(CImageBase *pimg, C8Image &imgMask, C8Image &imgBigMask, COLORREF Color,
	CRect rc, CPoint ptOffsMask)
{
	if (pimg->Is16BitColor())
		FillByMask(*(C16Image *)pimg, imgMask, imgBigMask, Color, rc, ptOffsMask);
	else
		FillByMask(*(C8Image *)pimg, imgMask, imgBigMask, Color, rc, ptOffsMask);
}

void CObservable::RemoveObserver(CObserver *pObserver)
{
	for (int i = 0; i < GetSize(); i++)
	{
		CObserver *p = (CObserver *)GetAt(i);
		if (p == pObserver)
		{
			RemoveAt(i);
			return;
		}
	}

	ASSERT( FALSE );
}

void CObservable::Update(int nHint, CObject *pObjHint)
{
	for (int i = 0; i < GetSize(); i++)
	{
		CObserver *p = (CObserver *)GetAt(i);
		p->OnUpdateObserver(this,nHint,pObjHint);
	}
}

CDblRect::operator CRect()
{
	return CRect((int)floor(left),(int)floor(top),
		(int)ceil(right),(int)ceil(bottom));
}

CAccumulate::CAccumulate()
{
	m_nImagesNum = 0;
}

bool CAccumulate::AddImage(C8Image &NewImg)
{
	if (NewImg.Is16BitColor())
		return false;
	if (m_nImagesNum == 0)
		m_img.Create(NewImg.GetCX(), NewImg.GetCY(), NewImg.IsTrueColor()?CImageBase::TrueColor:
			CImageBase::GrayScale); // Sets data as 0
	else if (m_img.GetCX() != NewImg.GetCX() || m_img.GetCY() != NewImg.GetCY() ||
		m_img.IsTrueColor() != NewImg.IsTrueColor())
		return false;
	int cy = m_img.GetCY();
	int cx = m_img.GetCols();
	for (int y = 0; y < cy; y++)
	{
		WORD *pDst = m_img[y];
		BYTE *pSrc = NewImg[y];
		for (int x = 0; x < cx; x++)
			*pDst++ += *pSrc++;
	}
	m_nImagesNum++;
	return true;
}

bool CAccumulate::GetImage(C8Image &NewImg)
{
	if (m_nImagesNum == 0)
		return false;
	NewImg.Create(m_img.GetCX(), m_img.GetCY(), m_img.IsTrueColor()?CImageBase::TrueColor:
		CImageBase::GrayScale); // Sets data as 0
	int cy = m_img.GetCY();
	int cx = m_img.GetCols();
	for (int y = 0; y < cy; y++)
	{
		BYTE *pDst = NewImg[y];
		WORD *pSrc = m_img[y];
		for (int x = 0; x < cx; x++)
			*pDst++ += (BYTE)((*pSrc++)/m_nImagesNum);
	}
	return true;
}

CString _GetProfileString(const char *pSect, const char *pEntry, const char *pDef)
{
	CString s = AfxGetApp()->GetProfileString(pSect, pEntry);
	if (s.IsEmpty())
	{
		AfxGetApp()->WriteProfileString(pSect, pEntry, pDef);
		s = pDef;
	}
	return s;
}

int _GetProfileInt(const char *pSect, const char *pEntry, int nDef)
{
	int n = AfxGetApp()->GetProfileInt(pSect, pEntry, -1);
	if (n == -1)
	{
		if (nDef >= 0)
			AfxGetApp()->WriteProfileInt(pSect, pEntry, nDef);
		n = nDef;
	}
	return n;
}

