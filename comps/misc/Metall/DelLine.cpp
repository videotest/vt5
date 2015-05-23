#include "stdafx.h"
#include "nameconsts.h"
#include "image5.h"
#include "data5.h"
#include "DocView5.h"
#include "misc_utils.h"
#include "DelLine.h"
#include <stdio.h>
#include <alloc.h>
#include <math.h>
#include "array.h"

//#define __TIME_TRACE
#if defined(__TIME_TRACE)

#define TimeTrace(p) CTimeTrace tt(p)

class CTimeTrace
{
	const char *m_pszFuncName;
	DWORD m_dwBeg;
public:
	CTimeTrace(const char *pszFuncName)
	{
		m_dwBeg = GetTickCount();
		m_pszFuncName = pszFuncName;
	}
	~CTimeTrace()
	{
		DWORD dw = GetTickCount();
		char szBuff[256];
		sprintf(szBuff, "%s : %d\n", m_pszFuncName, dw-m_dwBeg);
		OutputDebugString(szBuff);
	}
};

class CTracePoint
{
public:
	const char *m_pszFuncName;
	DWORD m_dwTotal;
	DWORD m_dwCur;
	DWORD m_dwEnd;
	CTracePoint(const char *pszFuncName)
	{
		m_pszFuncName = pszFuncName;
		m_dwTotal = m_dwCur = 0;
	}
	void start()
	{
		m_dwCur = GetTickCount();
	}
	void stop()
	{
		m_dwEnd = GetTickCount();
		m_dwTotal += m_dwEnd - m_dwCur;
	}
};

class CTracePoint1 : public CTracePoint
{
public:
	CTracePoint1 *m_pPrev;
	CTracePoint1(const char *pszFuncName, CTracePoint1 *pPrev = NULL) : 
		CTracePoint(pszFuncName)
	{
		m_dwTotal = m_dwCur = 0;
		m_pPrev = pPrev;
	}
	void start()
	{
		if (m_pPrev)
		{
			m_pPrev->stop();
			m_dwCur = m_pPrev->m_dwEnd;
		}
		else
			m_dwCur = GetTickCount();
	}
	void init()
	{
		for (CTracePoint1 *tp = this; tp; tp = tp->m_pPrev)
			tp->m_dwTotal = 0;
	}
	void print()
	{
		for (CTracePoint1 *tp = this; tp; tp = tp->m_pPrev)
		{
			char szBuff[256];
			sprintf(szBuff, "%s : %d\n", tp->m_pszFuncName, tp->m_dwTotal);
			OutputDebugString(szBuff);
			tp->m_dwTotal = 0;
		}
	}
};

class CTimeTrace2
{
	CTracePoint *m_ptp;
public:
	CTimeTrace2(CTracePoint *ptp)
	{
		m_ptp = ptp;
		m_ptp->start();
	}
	~CTimeTrace2()
	{
		m_ptp->stop();
	}
};


#define StartTracePoint(tp) tp.start()
#define StopTracePoint(tp) tp.stop()
#define PrintTracePoint(tp) tp.print()

#else
#define TimeTrace(p)
#define StartTracePoint(tp)
#define StopTracePoint(tp)
#define PrintTracePoint(tp)
#endif

/*inline int Bright(int b, int g, int r)
{
	return max(min((r*60+g*118+b*22)/200,0xFFFF),0);
}*/

inline color _ToColor(int c)
{
	return (color)(min(max(c,0),0xFFFF));
}

static double s_dMaxAngleShort = 0.15;
static double s_dMaxAngleFar = 0.15;
static double s_dMaxAngle3Far = 0.15;
static double s_dCurveCoef = 1.3;

////////////////////////////////////////////////////////////////////////////////////////////////////
//CDelLineBase
////////////////////////////////////////////////////////////////////////////////////////////////////

int _IsGrayScale(IImage3 *pSrc)
{
	IUnknownPtr punkCC;
	pSrc->GetColorConvertor(&punkCC);
	IColorConvertor2Ptr sptrCC(punkCC);
	if (sptrCC == 0) return -1;
	BSTR bstr = 0;
	sptrCC->GetCnvName(&bstr);
	_bstr_t bstrName(bstr);
	::SysFreeString(bstr);
	if (bstrName == _bstr_t("RGB"))
		return 0;
	else if (bstrName == _bstr_t("GRAY"))
		return 1;
	else
		return -1;
}

IImage3Ptr _MakeGrayscaleImage(IImage3 *pSrc, int cx, int cy, int bGS, bool bNegative)
{
	IUnknownPtr punkImage(::CreateTypedObject(_bstr_t(szArgumentTypeImage)), false);
	IImage3Ptr sptrImgGS(punkImage);
	sptrImgGS->CreateImage(cx, cy, _bstr_t("GRAY"), 1);
	byte *pRowMaskSrc,*pRowMaskDst;
	if (bGS)
	{
		color *pRowSrc,*pRowDst;
		for (int y = 0; y < cy; y++)
		{
			sptrImgGS->GetRowMask(y, &pRowMaskDst);
			sptrImgGS->GetRow(y, 0, &pRowDst);
			pSrc->GetRowMask(y, &pRowMaskSrc);
			pSrc->GetRow(y, 0, &pRowSrc);
			memcpy(pRowMaskDst, pRowMaskSrc, cx);
			if (bNegative)
			{
				for (int x = 0; x < cx; x++)
					pRowDst[x] = 0xFFFF-pRowSrc[x];
			}
			else
				memcpy(pRowDst, pRowSrc, cx*sizeof(color));
		}
	}
	else
	{
		color *pRowSrc0,*pRowSrc1,*pRowSrc2,*pRowDst;
		for (int y = 0; y < cy; y++)
		{
			sptrImgGS->GetRowMask(y, &pRowMaskDst);
			sptrImgGS->GetRow(y, 0, &pRowDst);
			pSrc->GetRowMask(y, &pRowMaskSrc);
			pSrc->GetRow(y, 0, &pRowSrc0);
			pSrc->GetRow(y, 1, &pRowSrc1);
			pSrc->GetRow(y, 2, &pRowSrc2);
			memcpy(pRowMaskDst, pRowMaskSrc, cx);
			if (bNegative)
			{
				for (int x = 0; x < cx; x++)
					pRowDst[x] = 0xFFFF-(color)Bright(pRowSrc0[x],pRowSrc1[x],pRowSrc2[x]);
			}
			else
			{
				for (int x = 0; x < cx; x++)
					pRowDst[x] = (color)Bright(pRowSrc0[x],pRowSrc1[x],pRowSrc2[x]);
			}
		}
	}
	return sptrImgGS;
}

typedef color (*PROCESSPOINT3x3)(color c00, color c01, color c02, color c10, color c11, color c12,
	color c20, color c21, color c22);

void _ProcessRow3x3(color *pRowSrc0, color *pRowSrc1, color *pRowSrc2, color *pRowDst, int cx, PROCESSPOINT3x3 pfProcess)
{
	pRowDst[0] = (*pfProcess)(pRowSrc0[0],pRowSrc0[0],pRowSrc0[1],
		pRowSrc1[0],pRowSrc1[0],pRowSrc1[1],pRowSrc2[0],pRowSrc2[0],pRowSrc2[1]);
	for (int x = 1; x < cx-1; x++)
		pRowDst[x] = (*pfProcess)(pRowSrc0[x-1],pRowSrc0[x],pRowSrc0[x+1],
			pRowSrc1[x-1],pRowSrc1[x],pRowSrc1[x+1],pRowSrc2[x-1],pRowSrc2[x],pRowSrc2[x+1]);
	pRowDst[cx-1] = (*pfProcess)(pRowSrc0[cx-2],pRowSrc0[cx-1],pRowSrc0[cx-1],
		pRowSrc1[cx-2],pRowSrc1[cx-1],pRowSrc1[cx-1],pRowSrc2[cx-2],pRowSrc2[cx-1],pRowSrc2[cx-1]);
}

void _Process3x3(IImage3 *&pRes, IImage3 *&pIntr, int cx, int cy, PROCESSPOINT3x3 pfProcess)
{
	TimeTrace("_Process3x3");
	IImage3 *pDst = pIntr;
	IImage3 *pSrc = pRes;
	color *pRowSrc0,*pRowSrc1,*pRowSrc2,*pRowDst;
	pSrc->GetRow(0, 0, &pRowSrc0);
	pSrc->GetRow(1, 0, &pRowSrc1);
	pDst->GetRow(0, 0, &pRowDst);
	_ProcessRow3x3(pRowSrc0,pRowSrc0,pRowSrc1,pRowDst,cx,pfProcess);
	for (int y = 1; y < cy-1; y++)
	{
		pSrc->GetRow(y-1, 0, &pRowSrc0);
		pSrc->GetRow(y, 0, &pRowSrc1);
		pSrc->GetRow(y+1, 0, &pRowSrc2);
		pDst->GetRow(y, 0, &pRowDst);
		_ProcessRow3x3(pRowSrc0,pRowSrc1,pRowSrc2,pRowDst,cx,pfProcess);
	}
	pSrc->GetRow(cy-2, 0, &pRowSrc0);
	pSrc->GetRow(cy-1, 0, &pRowSrc1);
	pDst->GetRow(cy-1, 0, &pRowDst);
	_ProcessRow3x3(pRowSrc0,pRowSrc1,pRowSrc1,pRowDst,cx,pfProcess);
	pRes = pDst;
	pIntr = pSrc;
}

color _Erode3x3(color c00, color c01, color c02, color c10, color c11, color c12,
	color c20, color c21, color c22)
{
	return min(min(min(min(c00,c01),min(c02,c10)),min(min(c11,c12),min(c20,c21))),c22);
}

color _Dilate3x3(color c00, color c01, color c02, color c10, color c11, color c12,
	color c20, color c21, color c22)
{
	return max(max(max(max(c00,c01),max(c02,c10)),max(max(c11,c12),max(c20,c21))),c22);
}

color _Average3x3(color c00, color c01, color c02, color c10, color c11, color c12,
	color c20, color c21, color c22)
{
	DWORD dwSum = DWORD(c00>>8)+DWORD(c01>>8)+DWORD(c02>>8)+DWORD(c10>>8)+DWORD(c11>>8)+
		DWORD(c12>>8)+DWORD(c20>>8)+DWORD(c21>>8)+DWORD(c22>>8);
	return color(dwSum/9)<<8;
}

void _Threshold(IImage3 *&pRes, IImage3 *&pIntr, int cx, int cy, color nThreshold)
{
	TimeTrace("_Threshold");
	IImage3 *pDst = pIntr;
	IImage3 *pSrc = pRes;
	color *pRowSrc0,*pRowSrc1,*pRowSrc2,*pRowDst;
	pDst->GetRow(0, 0, &pRowDst);
	memset(pRowDst,0,cx*sizeof(color));
	pDst->GetRow(1, 0, &pRowDst);
	memset(pRowDst,0,cx*sizeof(color));
	for (int y = 2; y < cy-2; y++)
	{
		pSrc->GetRow(y-2, 0, &pRowSrc0);
		pSrc->GetRow(y, 0, &pRowSrc1);
		pSrc->GetRow(y+2, 0, &pRowSrc2);
		pDst->GetRow(y, 0, &pRowDst);
		for (int x = 2; x < cx-2; x++)
		{
			color b0,b1,b2,b3,b4;
			b3 = pRowSrc0[x];
			b0 = pRowSrc1[x];
			b1 = pRowSrc1[x-2];
			b2 = pRowSrc1[x+2];
			b4 = pRowSrc2[x];
			pRowDst[x] = (b1-b0>nThreshold && b2-b0>nThreshold) ||
				(b3-b0>nThreshold && b4-b0>nThreshold) ? 0xFFFF : 0;
		}
		pRowDst[0] = 0; pRowDst[1] = 0; pRowDst[cx-2] = 0; pRowDst[cx-1] = 0;
	}
	pDst->GetRow(cy-2, 0, &pRowDst);
	memset(pRowDst,0,cx*sizeof(color));
	pDst->GetRow(cy-1, 0, &pRowDst);
	memset(pRowDst,0,cx*sizeof(color));
	pRes = pDst;
	pIntr = pSrc;
}

typedef void (*PROCESSPOINT3x3Nei)(color *pDst, color *pNei);

void _Process3x3Nei(IImage3 *pSrc, int cx, int cy, PROCESSPOINT3x3Nei pfProcess)
{
	color *pRowSrc0,*pRowSrc1,*pRowSrc2;
//	for (int y = cy-2; y >= 1; y--)
	for (int y = 1; y < cy-1; y++)
	{
		pSrc->GetRow(y-1, 0, &pRowSrc0);
		pSrc->GetRow(y, 0, &pRowSrc1);
		pSrc->GetRow(y+1, 0, &pRowSrc2);
		for (int x = 1; x < cx-1; x++)
		{
			color a[9];
			a[0] = pRowSrc0[x-1];
			a[1] = pRowSrc0[x];
			a[2] = pRowSrc0[x+1];
			a[3] = pRowSrc1[x+1];
			a[4] = pRowSrc2[x+1];
			a[5] = pRowSrc2[x];
			a[6] = pRowSrc2[x-1];
			a[7] = pRowSrc1[x-1];
			a[8] = pRowSrc0[x-1];
			pfProcess(&pRowSrc1[x], a);
		}
	}
}

typedef void (*PROCESSPOINT3x3PNei)(color *pDst, color **pNei);

void _Process3x3PNei(IImage3 *pSrc, int cx, int cy, PROCESSPOINT3x3PNei pfProcess)
{
	color *pRowSrc0,*pRowSrc1,*pRowSrc2;
	for (int y = cy-2; y >= 1; y--)
	{
		pSrc->GetRow(y-1, 0, &pRowSrc0);
		pSrc->GetRow(y, 0, &pRowSrc1);
		pSrc->GetRow(y+1, 0, &pRowSrc2);
		for (int x = 0; x < cx-1; x++)
		{
			if (pRowSrc1[x] > 0)
			{
				color* a[9];
				a[0] = &pRowSrc0[x-1];
				a[1] = &pRowSrc0[x];
				a[2] = &pRowSrc0[x+1];
				a[3] = &pRowSrc1[x+1];
				a[4] = &pRowSrc2[x+1];
				a[5] = &pRowSrc2[x];
				a[6] = &pRowSrc2[x-1];
				a[7] = &pRowSrc1[x-1];
				a[8] = &pRowSrc0[x-1];
				pfProcess(&pRowSrc1[x], a);
			}
		}
	}
}

void _SkeletonPoint(color *pDst, color *pNei)
{
	if (*pDst == 0xFFFF)
	{
		color a1 = pNei[7];
		int k1 = 0, k2 = 0, tz = 0;
		for (int i = 0; i < 8; i++)
		{
			color a2 = pNei[i];
			if (a2==0xFFFF)
				k1++;
			if (a2==0)
				tz++;
			if (a1 != 0xFFFF && a2 == 0xFFFF)
				k2++;
			a1=a2;
		}
		if (k2==1 && k1>2 && tz>2)
			*pDst = 0x8888;
	}
}

void _SkeletonPoint1(color *pDst, color *pNei)
{
	if (*pDst == 0xFFFF)
	{
		color a1 = pNei[7];
		int k1 = 0, k2 = 0;
		for (int i = 0; i < 8; i++)
		{
			color a2 = pNei[i];
			if (a2==0xFFFF)
				k1++;
			if (a1 != 0xFFFF && a2 == 0xFFFF)
				k2++;
			a1=a2;
		}
		if (k2==1 && k1>2)
			*pDst = 0;
		else if (k2==2)
		{
			if (pNei[1]==pNei[3]&&pNei[1]==0xFFFF||pNei[3]==pNei[5]&&pNei[3]==0xFFFF||
				pNei[5]==pNei[7]&&pNei[5]==0xFFFF||pNei[7]==pNei[1]&&pNei[7]==0xFFFF)
				*pDst = 0;
		}
	}
}

void _SkeletonPoint2(color *pDst, color **ppNei)
{
	if (*pDst > 0)
	{
		color a1 = *(ppNei[7]);
		int k1 = 0, k2 = 0;
		for (int i = 0; i < 8; i++)
		{
			color a2 = *(ppNei[i]);
			if (a2>0)
				k1++;
			if (a1 == 0 && a2 > 0)
				k2++;
			a1=a2;
		}
		if (k2>2)
		{
			*pDst = 0x100;
			for (int i = 0; i < 8; i++)
			{
				if (*(ppNei[i])==0xFFFF)
					*(ppNei[i]) = 0xFE00;
			}
		}
		else if (k2==1)
			if(k1<3)
				*pDst = 0xFD00;
	}
}

void _Skeleton(IImage3 *pSrc, int cx, int cy)
{
	TimeTrace("_Skeleton");
	for (int i = 0; i < 4; i++)
	{
		_Process3x3Nei(pSrc,cx,cy,_SkeletonPoint);
		bool b = false;
		for (int y = 0; y < cy; y++)
		{
			for (int x = 0; x < cx; x++)
			{
				color *pRowSrc;
				pSrc->GetRow(y, 0, &pRowSrc);
				if (pRowSrc[x] == 0x8888)
				{
					pRowSrc[x] = 0;
					b = true;
				}
			}
		}
		if (!b) break;
	}
	_Process3x3Nei(pSrc,cx,cy,_SkeletonPoint1);
	_Process3x3PNei(pSrc,cx,cy,_SkeletonPoint2);
}

int sign(int a)
{
	if (a<0)
		return -1;
	else
		return 1;
}


class CLine : public _CArray<POINTS,100,20>
{
public:
	int status;
	double dSize,dLen,dAngle;
	int x1,x2,y1,y2;
	double dRad1,dRad2,dAngC1,dAngC2;
	bool bOrig; // Found on image or made by connection
	_CArray<CLine*,10,10> Children;
	CLine()
	{
		status = 0;
		dSize = 0.;
		x1 = x2 = y1 = y2 = -1;
		bOrig = true;
	}
	CLine(CLine &f)
	{
		Copy(f);
	}
	CLine(CLine *p1, CLine *p2, int nMode)
	{
		bOrig = false;
		status = 3;
		dSize = p1->dSize+p2->dSize;
		if (nMode == 11)
		{
			x1 = p1->x2;
			y1 = p1->y2;
			x2 = p2->x2;
			y2 = p2->y2;
		}
		else if (nMode == 12)
		{
			x1 = p1->x2;
			y1 = p1->y2;
			x2 = p2->x1;
			y2 = p2->y1;
		}
		else if (nMode == 21)
		{
			x1 = p1->x1;
			y1 = p1->y1;
			x2 = p2->x2;
			y2 = p2->y2;
		}
		else //if (nMode == 22)
		{
			x1 = p1->x1;
			y1 = p1->y1;
			x2 = p2->x1;
			y2 = p2->y1;
		}
		CalcAngle();
		int n1 = p1->bOrig?1:p1->Children.GetSize();
		int n2 = p2->bOrig?1:p2->Children.GetSize();
		Children.SetSize(n1+n2);
		int i = 0;
		if (p1->bOrig)
			Children.SetAt(i++, p1);
		else
			for (; i < n1; i++)
				Children.SetAt(i, p1->Children.GetAt(i));
		if (p2->bOrig)
			Children.SetAt(i, p2);
		else
			for (; i < n1+n2; i++)
				Children.SetAt(i, p2->Children.GetAt(i-n1));
	}
	void CalcAngle()
	{
		dLen = sqrt((double)((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)));
		if (dLen > 0.)
			dAngle = acos(((double)(x2-x1)*sign(y2-y1))/dLen);
		else
			status = 0;
	}
	void AddPoint(int x, int y)
	{
		POINTS pt;
		pt.x = x;
		pt.y = y;
		Add(pt);
	}
	void Copy(CLine &f)
	{
		bOrig = f.bOrig;
		status = f.status;
		dSize = f.dSize;
		dLen = f.dLen;
		dAngle = f.dAngle;
		x1 = f.x1;
		y1 = f.y1;
		x2 = f.x2;
		y2 = f.y2;
		dRad1 = f.dRad1;
		dRad2 = f.dRad2;
		dAngC1 = f.dAngC1;
		dAngC2 = f.dAngC2;
		CopyData(f);
		Children.CopyData(f.Children);
	}
};

class CLinesArray : public _CArray<CLine*,1000,100>
{
	bool m_bDelete;
public:
	CLinesArray(bool bDelete = false)
	{
		m_bDelete = bDelete;
	}
	~CLinesArray()
	{
		if (m_parr && m_bDelete)
		{
			for (int i = 0; i < m_nSize; i++)
				delete m_parr[i];
		}
	}
};

class CPointDirs
{
public:
	color *a[9];
	POINT d[9];
};

void _MakeDirs(CPointDirs &dir, IImage3 *pSrc, int x, int y)
{
	color *pRowSrc0,*pRowSrc1,*pRowSrc2;
	pSrc->GetRow(y-1, 0, &pRowSrc0);
	pSrc->GetRow(y, 0, &pRowSrc1);
	pSrc->GetRow(y+1, 0, &pRowSrc2);
	dir.a[0] = &pRowSrc0[x];
	dir.a[1] = &pRowSrc1[x+1];
	dir.a[2] = &pRowSrc2[x];
	dir.a[3] = &pRowSrc1[x-1];
	dir.a[4] = &pRowSrc0[x+1];
	dir.a[5] = &pRowSrc2[x+1];
	dir.a[6] = &pRowSrc2[x-1];
	dir.a[7] = &pRowSrc0[x-1];
	dir.a[8] = &pRowSrc0[x];
	dir.d[0] = _point(0,-1);
	dir.d[1] = _point(1,0);
	dir.d[2] = _point(0,1);
	dir.d[3] = _point(-1,0);
	dir.d[4] = _point(1,-1);
	dir.d[5] = _point(1,1);
	dir.d[6] = _point(-1,1);
	dir.d[7] = _point(-1,-1);
	dir.d[8] = _point(0,-1);
}

void _SearchLines(CLinesArray &arrLines, IImage3 *pSrc, int cx, int cy, CFilter *pFltr,
	int nBase)
{
	TimeTrace("_SearchLines");
	for (int y = 0; y < cy; y++)
	{
		for (int x = 0; x < cx; x++)
		{
			color *pRowSrc;
			pSrc->GetRow(y, 0, &pRowSrc);
			if (pRowSrc[x] == 0xFE00 || pRowSrc[x] == 0xFD00)
			{
				CLine *pLine = new CLine;
				pLine->status = pRowSrc[x]==0xFE00?2:1;
				pRowSrc[x] -= 0x1000;
				int x0 = x;
				int y0 = y;
				CPointDirs dirs;
				int i;
				do
				{
					_MakeDirs(dirs,pSrc,x0,y0);
					for (i = 0; i < 8 ; i++)
					{
						if (*(dirs.a[i]) == 0xFFFF)
						{
							*dirs.a[i] -= 0x1000;
							pLine->AddPoint(x0, y0);
							x0 += dirs.d[i].x;
							y0 += dirs.d[i].y;
							if (i > 3)
								pLine->dSize += sqrt(2.);
							else
								pLine->dSize += 1.;
							break;
						}
					}
				}
				while (i<8);
				_MakeDirs(dirs,pSrc,x0,y0);
				for (i = 0; i < 8 ; i++)
				{
					if (*dirs.a[i] > 0xFC00)
					{
						*dirs.a[i] -= 0x1000;
						pLine->AddPoint(x0, y0);
						x0 += dirs.d[i].x;
						y0 += dirs.d[i].y;
						if (i > 3)
							pLine->dSize += sqrt(2.);
						else
							pLine->dSize += 1.;
						break;
					}
				}
				pLine->FreeExtra();
				if (*dirs.a[i] == 0xED00)
					pLine->status = 1;
				pLine->x1 = x;
				pLine->y1 = y;
				pLine->x2 = x0;
				pLine->y2 = y0;
				if (sqrt((double)((x-x0)*(x-x0)+(y-y0)*(y-y0)))<(pLine->dSize)/s_dCurveCoef)
//				if (sqrt((x-x0)*(x-x0)+(y-y0)*(y-y0))<(pLine->dSize)/1.3)
					pLine->status = 0; // curve
				if (pLine->status != 2 && pLine->dSize < 5.)
					pLine->status = 0; // short
				pLine->CalcAngle();
				arrLines.Add(pLine);
			}
		}
		pFltr->Notify(nBase+y*100/cy);
	}
}

void _MapLine(CLine *p, IImage3 *pSrc, color clrMap, bool bMap0);
void _FilterShort(CLinesArray &arrLines, IImage3 *pSrc)
{
	TimeTrace("_FilterShort");
	for (int i = 0; i < arrLines.GetSize(); i++)
	{
		CLine *p = arrLines.GetAt(i);
		if (p->status == 0)
		{
			_MapLine(p, pSrc, 0, true);
			arrLines.SetAt(i, 0);
			delete p;
		}
	}
}

void _FindLinesWithAngle(CLinesArray &arrDst, CLinesArray &arrSrc, double ang, double dThr, int xc, int yc)
{
	for (int i = 0; i < arrSrc.GetSize(); i++)
	{
		CLine *p1 = arrSrc.GetAt(i);
		if (p1 == 0) continue;
		if (p1->status != 0 && ((p1->dAngle-ang<dThr && p1->dAngle-ang>-dThr) ||
			(p1->dAngle-PI-ang<dThr && p1->dAngle-PI-ang>-dThr) ||
			(p1->dAngle+PI-ang<dThr && p1->dAngle+PI-ang>-dThr)))
		{
			p1->dRad1 = sqrt((double)((xc-p1->x1)*(xc-p1->x1)+(yc-p1->y1)*(yc-p1->y1)));
			p1->dRad2 = sqrt((double)((xc-p1->x2)*(xc-p1->x2)+(yc-p1->y2)*(yc-p1->y2)));
			p1->dAngC1 = asin((double)(p1->y1-yc)/p1->dRad1);
			p1->dAngC2 = asin((double)(p1->y2-yc)/p1->dRad2);
			if(p1->x1-xc<0) p1->dAngC1 = PI-p1->dAngC1;
			if(p1->x2-xc<0) p1->dAngC2 = PI-p1->dAngC2;
			arrDst.Add(p1);
		}
	}
}

void _FindLinesOnStrip(CLinesArray &arrDst, CLinesArray &arrSrc, double ang, int iPos, int iStepXY)
{
	for (int i = 0; i < arrSrc.GetSize(); i++)
	{
		CLine *p1 = arrSrc.GetAt(i);
		if (abs((int)(p1->dRad1*sin(p1->dAngC1-ang))-iPos) < iStepXY &&
			abs((int)(p1->dRad2*sin(p1->dAngC2-ang))-iPos) < iStepXY)
		{
			arrDst.Add(p1);
		}
	}
}

struct CJoinVar
{
	int m_i, m_j;
	CJoinVar *m_pPrev;
	CJoinVar *m_pNext;
	CJoinVar()
	{
		m_pPrev = m_pNext = NULL;
		m_i = m_j = -1;
	}
};

template<class JoinVar> class CJoinVars : _CArray<JoinVar,100,10>
{
	JoinVar *m_pOptimal;
	void Exclude(JoinVar &Var)
	{
		if (Var.m_pPrev)
			Var.m_pPrev->m_pNext = Var.m_pNext;
		else
			m_pOptimal = (JoinVar*)Var.m_pNext;
		if (Var.m_pNext)
			Var.m_pNext->m_pPrev = Var.m_pPrev;
		Var.m_pPrev = Var.m_pNext = NULL;
		Var.m_i = Var.m_j = -1;
	}
	void Add(int n, JoinVar &var, bool bInsertInList)
	{
		GetPtrAt(n) = var;
		if (bInsertInList)
		{
			if (m_pOptimal)
			{
				JoinVar *p1 = NULL;
				for(JoinVar *p = m_pOptimal; p != NULL; p = (JoinVar*)p->m_pNext)
				{
					if (Better(GetPtrAt(n),*p))
					{
						GetPtrAt(n).m_pNext = p;
						GetPtrAt(n).m_pPrev = p->m_pPrev;
						if (p->m_pPrev)
							p->m_pPrev->m_pNext = &GetPtrAt(n);
						p->m_pPrev = &GetPtrAt(n);
						if (p == m_pOptimal)
							m_pOptimal = &GetPtrAt(n);
						break;
					}
					p1 = p;
				}
				if (p == NULL)
				{
					p1->m_pNext = &GetPtrAt(n);
					GetPtrAt(n).m_pPrev = p1;
				}
			}
			else
				m_pOptimal = &GetPtrAt(n);
		}
	}
protected:
	virtual bool Better(JoinVar &Var1, JoinVar &Var2) = 0;
public:
	CJoinVars(int nSize)
	{
		SetSize(nSize);
		m_pOptimal = 0;
		for (int i = 0; i < nSize; i++)
		{
			GetPtrAt(i).m_i = GetPtrAt(i).m_j = -1;
		}
	}
	JoinVar *GetOptimal() {return m_pOptimal;}
	void Free(JoinVar &Var)
	{
		if (Var.m_i == -1) return;
		int i = min(Var.m_i,Var.m_j);
		int j = max(Var.m_i,Var.m_j);
		Exclude(GetPtrAt(i));
		GetPtrAt(i).m_i = GetPtrAt(i).m_j = -1;
		GetPtrAt(j).m_i = GetPtrAt(j).m_j = -1;
	}
	void TryVariant(JoinVar &Var)
	{
		if (Better(Var, GetPtrAt(Var.m_i)) && Better(Var, GetPtrAt(Var.m_j)))
		{
			Free(GetPtrAt(Var.m_i));
			Free(GetPtrAt(Var.m_j));
			Add(Var.m_i,Var,true);
			Add(Var.m_j,Var,false);
		}
	}
};

struct CJoinVarShort : public CJoinVar
{
	double m_dAngle;
	int m_nMode;
	double m_dRate;
	CJoinVarShort(int i, int j, double dAngle, int nMode, double dRate)
	{
		m_i = i;
		m_j = j;
		m_dAngle = dAngle;
		m_nMode = nMode;
		m_dRate = dRate;
	}
};

class CJoinVarsShort : public CJoinVars<CJoinVarShort>
{
protected:
	virtual bool Better(CJoinVarShort &Var1, CJoinVarShort &Var2)
	{
		if (Var2.m_i == -1) return true;
//		return Var1.m_dAngle<Var2.m_dAngle;
		return Var1.m_dRate<Var2.m_dRate;
	}
public:
	CJoinVarsShort(int nSizeIJ) : CJoinVars<CJoinVarShort>(nSizeIJ)
	{
	}
};

void TryAddToJoinVarsShort(CJoinVarsShort &Variants, CLine *p1, CLine *p2, int i, int j, double dMaxConnect)
{
	double dAngleIJ = min(min(max(p1->dAngle,p2->dAngle)-min(p1->dAngle,p2->dAngle),
		max(p1->dAngle-PI,p2->dAngle)-min(p1->dAngle-PI,p2->dAngle)),
		max(p1->dAngle+PI,p2->dAngle)-min(p1->dAngle+PI,p2->dAngle));
	if (dAngleIJ < dMaxConnect)
	{
		int nMode1 = -1;
		int nDist;
		if (abs(p1->x1-p2->x1) < 5 && abs(p1->y1-p2->y1) < 5)
		{
			nMode1 = 11;
			nDist = max(abs(p1->x1-p2->x1),abs(p1->y1-p2->y1));
		}
		else if (abs(p1->x1-p2->x2) < 5 && abs(p1->y1-p2->y2) < 5)
		{
			nMode1 = 12;
			nDist = max(abs(p1->x1-p2->x2),abs(p1->y1-p2->y2));
		}
		else if (abs(p1->x2-p2->x1) < 5 && abs(p1->y2-p2->y1) < 5)
		{
			nMode1 = 21;
			nDist = max(abs(p1->x2-p2->x1),abs(p1->y2-p2->y1));
		}
		else if (abs(p1->x2-p2->x2) < 5 && abs(p1->y2-p2->y2) < 5)
		{
			nMode1 = 22;
			nDist = max(abs(p1->x2-p2->x2),abs(p1->y2-p2->y2));
		}
		if (nMode1 != -1)
		{
			CJoinVarShort var(i,j,dAngleIJ,nMode1,dAngleIJ+dMaxConnect*nDist/5);
			Variants.TryVariant(var);
		}
	}
}

void MakeJoinVarsShort(CJoinVarsShort &Variants, CLinesArray &arrSrc, double dMaxConnect)
{
	int nSize = arrSrc.GetSize();
	for(int i = 0; i < arrSrc.GetSize(); i++)
	{
		CLine *p1 = arrSrc.GetAt(i);
		if (p1 == 0 || p1->status == 0) continue;
		for(int j = i+1; j < arrSrc.GetSize(); j++)
		{
			CLine *p2 = arrSrc.GetAt(j);
			if (p2 == 0 || p2->status == 0) continue;
			TryAddToJoinVarsShort(Variants, p1, p2, i, j, dMaxConnect);
		}
	}
}

void AddObjectToJoinVarsShort(CJoinVarsShort &Variants, CLinesArray &arrSrc, int n, double dMaxConnect)
{
	CLine *p1,*p2 = arrSrc.GetAt(n);
	for(int i = 0; i < n; i++)
	{
		CLine *p1 = arrSrc.GetAt(i);
		if (p1 == 0 || p1->status == 0) continue;
		TryAddToJoinVarsShort(Variants, p1, p2, i, n, dMaxConnect);
	}
	p1 = p2;
	for(i = n+1; i < arrSrc.GetSize(); i++)
	{
		CLine *p2 = arrSrc.GetAt(i);
		if (p2 == 0 || p2->status == 0) continue;
		TryAddToJoinVarsShort(Variants, p1, p2, n, i, dMaxConnect);
	}
}

void _DoConnectAngShort(CLinesArray &arrSrc)
{
	double dMaxConnect = s_dMaxAngleShort;
	CJoinVarsShort Variants(arrSrc.GetSize());
	MakeJoinVarsShort(Variants, arrSrc, dMaxConnect);
	while(Variants.GetOptimal())
	{
		CJoinVarShort *pVar = Variants.GetOptimal();
		int i = pVar->m_i, j = pVar->m_j;
		CLine *p1 = arrSrc.GetAt(i);
		CLine *p2 = arrSrc.GetAt(j);
		CLine *p3 = new CLine(p1, p2, pVar->m_nMode);
		arrSrc.SetAt(i, p3);
		arrSrc.SetAt(j, 0);
		if (!p1->bOrig)
			delete p1;
		if (!p2->bOrig)
			delete p2;
		Variants.Free(*pVar);
		AddObjectToJoinVarsShort(Variants, arrSrc, i, dMaxConnect);
	}
}

#if 0
void _DoConnectAngShort(CLinesArray &arrSrc)
{
//	double dMaxConnect = 0.04;
	double dMaxConnect = s_dMaxAngleShort;
	bool bConnect;
	do 
	{
		bConnect = false;
		double dMin = dMaxConnect;
		int iConnect = -1, jConnect = -1, nMode;
		int nSize = arrSrc.GetSize();
		for(int i = 0; i < arrSrc.GetSize(); i++)
		{
			CLine *p1 = arrSrc.GetAt(i);
			if (p1 == 0 || p1->status == 0) continue;
			for(int j = i+1; j < arrSrc.GetSize(); j++)
			{
				CLine *p2 = arrSrc.GetAt(j);
				if (p2 == 0 || p2->status == 0) continue;
				double dAngleIJ = min(min(max(p1->dAngle,p2->dAngle)-min(p1->dAngle,p2->dAngle),
					max(p1->dAngle-PI,p2->dAngle)-min(p1->dAngle-PI,p2->dAngle)),
					max(p1->dAngle+PI,p2->dAngle)-min(p1->dAngle+PI,p2->dAngle));
				if (dAngleIJ < dMin)
				{
					int nMode1 = -1;
					if (abs(p1->x1-p2->x1) < 5 && abs(p1->y1-p2->y1) < 5)
						nMode1 = 11;
					else if (abs(p1->x1-p2->x2) < 5 && abs(p1->y1-p2->y2) < 5)
						nMode1 = 12;
					else if (abs(p1->x2-p2->x1) < 5 && abs(p1->y2-p2->y1) < 5)
						nMode1 = 21;
					else if (abs(p1->x2-p2->x2) < 5 && abs(p1->y2-p2->y2) < 5)
						nMode1 = 22;
					if (nMode1 != -1)
					{
						dMin = dAngleIJ;
						iConnect = i;
						jConnect = j;
						nMode = nMode1;
					}
				}
			}
		}
		if (jConnect >= 0)
		{
			CLine *p1 = arrSrc.GetAt(iConnect);
			CLine *p2 = arrSrc.GetAt(jConnect);
			CLine *p3 = new CLine(p1, p2, nMode);
			arrSrc.SetAt(iConnect, p3);
			arrSrc.SetAt(jConnect, 0);
			if (!p1->bOrig)
				delete p1;
			if (!p2->bOrig)
				delete p2;
			bConnect = true;
		}
	} while (bConnect);
}
#endif

double DeltaAng(double a1, double a2)
{
	double dMin = min(a1,a2);
	double dMax = max(a1,a2);
	if (dMax-dMin<PI/2)
		return dMax-dMin;
	else
		return PI-(dMax-dMin);
}

double DeltaAng3(double a1, double a2, double a3)
{
	double ang1 = DeltaAng(a1,a2);
	double ang2 = DeltaAng(a2,a3);
	double ang3 = DeltaAng(a1,a3);
	return max(ang1,max(ang2,ang3));
}

double TriangleHeight(int x1, int y1, int x2, int y2, int x3, int y3)
{
	double A = sqrt((double)((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)));
	double B = sqrt((double)((x1-x3)*(x1-x3)+(y1-y3)*(y1-y3)));
	double C = sqrt((double)((x3-x2)*(x3-x2)+(y3-y2)*(y3-y2)));
	double h1,dAngCos;
	if (A>=1 && B>=1)
	{
		dAngCos = (A*A+B*B-C*C)/(2*A*B);
		if (dAngCos > 0.9999999)
			dAngCos = 0.9999999;
		h1 = B*sqrt(1.-dAngCos*dAngCos);
	}
	else
		h1 = 0;
	return h1;
}

double DeltaRad3(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
{
	double h1 = TriangleHeight(x1,y1,x2,y2,x3,y3);
	double h2 = TriangleHeight(x1,y1,x2,y2,x4,y4);
	return max(h1,h2);
}

bool _TryConnectAngFar(CLine *p1, CLine *p2, int &nMode1, double &dMin, double dMaxAngle, double dMaxAngle3,
	double dMaxRad3)
{
	if (DeltaAng(p1->dAngle, p2->dAngle) < dMaxAngle)
	{
		double r11 = sqrt((double)((p1->x1-p2->x1)*(p1->x1-p2->x1)+(p1->y1-p2->y1)*(p1->y1-p2->y1)));
		double r12 = sqrt((double)((p1->x1-p2->x2)*(p1->x1-p2->x2)+(p1->y1-p2->y2)*(p1->y1-p2->y2)));
		double r21 = sqrt((double)((p1->x2-p2->x1)*(p1->x2-p2->x1)+(p1->y2-p2->y1)*(p1->y2-p2->y1)));
		double r22 = sqrt((double)((p1->x2-p2->x2)*(p1->x2-p2->x2)+(p1->y2-p2->y2)*(p1->y2-p2->y2)));
		double dRadMin = min(min(r11,r12),min(r21,r22));
		int nMode = dRadMin==r11?11:dRadMin==r12?12:dRadMin==r21?21:22;
		double dRadMax = nMode==11?r22:nMode==12?r21:nMode==21?r12:r11;
		int x11 = nMode==11||nMode==12?p1->x1:p1->x2;
		int y11 = nMode==11||nMode==12?p1->y1:p1->y2;
		int x12 = nMode==11||nMode==12?p1->x2:p1->x1;
		int y12 = nMode==11||nMode==12?p1->y2:p1->y1;
		int x21 = nMode==11||nMode==21?p2->x1:p2->x2;
		int y21 = nMode==11||nMode==21?p2->y1:p2->y2;
		int x22 = nMode==11||nMode==21?p2->x2:p2->x1;
		int y22 = nMode==11||nMode==21?p2->y2:p2->y1;
		if (dRadMin < dMin)
		{
			double dAng = acos(((double)(x12-x22)*sign(y12-y22))/dRadMax);
			if (DeltaRad3(x12,y12,x22,y22,x11,y11,x21,y21) < dMaxRad3 &&
				DeltaAng3(dAng,p1->dAngle,p2->dAngle) < dMaxAngle3)
			{
				double dRads1 = sqrt((double)((p1->x1-p1->x2)*(p1->x1-p1->x2)+(p1->y1-p1->y2)*(p1->y1-p1->y2)));
				double dRads2 = sqrt((double)((p2->x1-p2->x2)*(p2->x1-p2->x2)+(p2->y1-p2->y2)*(p2->y1-p2->y2)));
				if (dRadMin/dRads1 < 5 && dRadMin/dRads2 < 5 && dRadMax >= max(dRads1,dRads2))
				{
					dMin = dRadMin;
					nMode1 = nMode;
					return true;
				}
			}
		}
	}
	return false;
}

void _DoConnectAngFar(CLinesArray &arrSrc)
{
	double dMaxAngle = s_dMaxAngleFar;
//	double dMaxAngle3 = 0.08;
	double dMaxAngle3 = s_dMaxAngle3Far;
	double dMaxRad3 = 4.;
	bool bConnect;
	do 
	{
		bConnect = false;
		for(int i = 0; i < arrSrc.GetSize(); i++)
		{
			CLine *p1 = arrSrc.GetAt(i);
			if (p1 == 0 || p1->status == 0) continue;
			if (p1->x1 == 57 && p1->y1 == 4 || p1->x2 == 57 && p1->y2 == 4)
				p1->x1 = p1->x1;
			double dMin = 1000000.;
			int jConnect = -1, nMode;
			for(int j = i+1; j < arrSrc.GetSize(); j++)
			{
				CLine *p2 = arrSrc.GetAt(j);
				if (p2 == 0 || p2->status == 0) continue;
				if (_TryConnectAngFar(p1, p2, nMode, dMin, dMaxAngle, dMaxAngle3, dMaxRad3))
					jConnect = j;
			}
			if (jConnect >= 0)
			{
				CLine *p2 = arrSrc.GetAt(jConnect);
				CLine *p3 = new CLine(p1, p2, nMode);
				arrSrc.SetAt(i, p3);
				arrSrc.SetAt(jConnect, 0);
				if (!p1->bOrig)
					delete p1;
				if (!p2->bOrig)
					delete p2;
				bConnect = true;
				bool bConnect1;
				do
				{
					bConnect1 = false;
					int i1Connect = -1, nMode1;
					double dMin = 1000000.;
					for (int i1 = 0; i1 < arrSrc.GetSize(); i1++)
					{
						if (i1 == i) continue;
						CLine *p4 = arrSrc.GetAt(i1);
						if (p4 == 0 || p4->status == 0) continue;
						if (_TryConnectAngFar(p3, p4, nMode1, dMin, dMaxAngle, dMaxAngle3, dMaxRad3))
							i1Connect = i1;
					}
					if (i1Connect > -1)
					{
						CLine *p4 = arrSrc.GetAt(i1Connect);
						CLine *p5 = new CLine(p3, p4, nMode1);
						arrSrc.SetAt(i, p5);
						arrSrc.SetAt(i1Connect, 0);
						if (!p3->bOrig)
							delete p3;
						if (!p4->bOrig)
							delete p4;
						bConnect1 = true;
						p3 = p5;
					}
				}
				while(bConnect1);
			}
		}
	} while (bConnect);
}

#if defined(__TIME_TRACE)
	CTracePoint1 tpFindLines("FindLines");
	CTracePoint1 tpFindLinesOnStrip("tpFindLinesOnStrip");
//	CTracePoint1 tpDoConnect("DoConnect",&tpFindLinesOnStrip);
	CTracePoint1 tpConnectAngShort("_DoConnectAngShort",&tpFindLinesOnStrip);
	CTracePoint1 tpConnectAngFar("_DoConnectAngFar", &tpConnectAngShort);
	CTracePoint1 tpAdd("Add",&tpConnectAngFar);
//	CTracePoint1 tpConnectAngShort("_DoConnectAngShort");
//	CTracePoint1 tpConnectAngFar("_DoConnectAngFar", &tpConnectAngShort);
#endif

void _DoConnect(CLinesArray &arrSrc)
{
/*	StartTracePoint(tpConnectAngShort);
	_DoConnectAngShort(arrSrc);
	StartTracePoint(tpConnectAngFar);
	_DoConnectAngFar(arrSrc);
	StopTracePoint(tpConnectAngFar);*/
}

void CDelLineBase::ConnectLines(CLinesArray &arrLines, IImage3 *pSrc, int cx, int cy,
	int nStart, int nEnd)
{
	TimeTrace("ConnectLines");
//	double dThresh = 0.2;
	CLinesArray arr;
	arr.CopyData(arrLines);
	int nNotify = nStart;
	double dDeltaAng = max(s_dMaxAngleShort,max(s_dMaxAngleFar,s_dMaxAngle3Far));
	double dThresh = dDeltaAng;
//	int nNotifyAdd = (nEnd-nStart)/int(PI/dDeltaAng);
	int nTotalObjects = 2*arr.GetSize();
	int nProcessObjects = 0;
	for (double ang = 0.; ang < PI; ang += dDeltaAng)
	{
		CLinesArray arr1;
		StartTracePoint(tpFindLines);
		_FindLinesWithAngle(arr1, arr, ang, dThresh, cx/2, cy/2);
		StopTracePoint(tpFindLines);
		int nHalfMaxCxCy = max(cx/2,cy/2);
		int nStepXY = (int)(nHalfMaxCxCy*0.15);
		int nProcessObjects1 = nProcessObjects;
		for (int iPos = -nHalfMaxCxCy+nStepXY; iPos < nHalfMaxCxCy-nStepXY; iPos += nStepXY)
		{
			CLinesArray arr2;
			StartTracePoint(tpFindLinesOnStrip);
			_FindLinesOnStrip(arr2, arr1, ang, iPos, nStepXY);
			StartTracePoint(tpConnectAngShort);
			_DoConnectAngShort(arr2);
			StartTracePoint(tpConnectAngFar);
			_DoConnectAngFar(arr2);
			StartTracePoint(tpAdd);
			for (int i = 0; i < arr2.GetSize(); i++)
			{
				CLine *p = arr2.GetAt(i);
				if (p && !p->bOrig)
					arrLines.Add(p);
			}
			StopTracePoint(tpAdd);
			nProcessObjects1 += arr2.GetSize()/2;
			Notify(nStart+(nEnd-nStart)*nProcessObjects1/nTotalObjects);
		}
		nProcessObjects += arr1.GetSize();
	}
	PrintTracePoint(tpFindLines);
	PrintTracePoint(tpAdd);
}

void _FilterShort2(CLinesArray &arrLines, IImage3 *pSrc, int nSize)
{
	TimeTrace("_FilterShort2");
	for (int i = 0; i < arrLines.GetSize(); i++)
	{
		CLine *p = arrLines.GetAt(i);
		if (p == 0) continue;
		if (p->dLen < nSize)
			p->status = 0;
	}
}

void _FilterHorizontal(CLinesArray &arrLines)
{
	TimeTrace("_FilterHorizontal");
	double dAngleLimit=::GetValueInt( ::GetAppUnknown(), "\\VTDelLine", "SetAngleLimit", 20. );
	for (int i = 0; i < arrLines.GetSize(); i++)
	{
		CLine *p = arrLines.GetAt(i);
		if (p == 0) continue;
		if ((p->dAngle < PI*dAngleLimit/180. && p->dAngle > -PI*dAngleLimit/180.) ||
			(p->dAngle > PI*(180.-dAngleLimit)/180. && p->dAngle < -PI*(180.-dAngleLimit)/180.))
			p->status = 0;
	}
}

void _MapLine(CLine *p, IImage3 *pSrc, color clrMap, bool bMap0)
{
	color *pRowSrc;
	pSrc->GetRow(p->y1, 0, &pRowSrc);
	CPointDirs dirs;
	int i;
	int nSize1 = 1, nSize2 = ((int)p->dSize)+1;
	if (pRowSrc[p->x1] > 0xE000)
	{
		pRowSrc[p->x1] = clrMap;
		int x0 = p->x1;
		int y0 = p->y1;
		do
		{
			_MakeDirs(dirs,pSrc,x0,y0);
			for (i = 0; i < 8 ; i++)
			{
				if (*(dirs.a[i]) >= 0xEFFF)
				{
					x0 += dirs.d[i].x;
					y0 += dirs.d[i].y;
					if (bMap0)
					{
						nSize1++;
						if (nSize1 < nSize2)
							*dirs.a[i] = clrMap;
						else
							i = 8;
					}
					else
						*dirs.a[i] = clrMap;
					break;
				}
			}
		}
		while (i<8);
		if (!bMap0 || nSize1 < nSize2)
		{
			_MakeDirs(dirs,pSrc,x0,y0);
			for (i = 0; i < 8 ; i++)
			{
				if (*dirs.a[i] > 0xE000)
				{
					*dirs.a[i] = clrMap;
					x0 += dirs.d[i].x;
					y0 += dirs.d[i].y;
					break;
				}
			}
		}
	}
	if (bMap0)
	{
		_MakeDirs(dirs,pSrc,p->x1,p->y1);
		for (i = 0; i < 8 ; i++)
			if (*dirs.a[i] == 0x100)
				*dirs.a[i] = 0;
		_MakeDirs(dirs,pSrc,p->x2,p->y2);
		for (i = 0; i < 8 ; i++)
			if (*dirs.a[i] == 0x100)
				*dirs.a[i] = 0;
	}
}

void _MapLines(CLinesArray &arrLines, IImage3 *pSrc, color clrMap)
{
	TimeTrace("_MapLines");
	for (int i = 0; i < arrLines.GetSize(); i++)
	{
		CLine *p = arrLines.GetAt(i);
		if (p == 0 || p->status == 0) continue;
		if (p->bOrig)
			_MapLine(p, pSrc, clrMap, false);
		else
			for (int j = 0; j < p->Children.GetSize(); j++)
			{
				CLine *p1 = p->Children.GetAt(j);
				_MapLine(p1, pSrc, clrMap, false);
			}
	}
}

void _MarkJunctions(IImage3 *pSrc, int cx, int cy)
{
	for (int y = 0; y < cy; y++)
	{
		color *pRowSrc;
		pSrc->GetRow(y, 0, &pRowSrc);
		for (int x = 0; x < cx; x++)
		{
			if (pRowSrc[x] == 1)
				pRowSrc[x] = 0xC000;
		}
	}
}

void _MarkOtherPoints(IImage3 *pSrc, int cx, int cy)
{
	TimeTrace("_MarkOtherPoints");
	for (int y = 0; y < cy; y++)
	{
		color *pRowSrc;
		pSrc->GetRow(y, 0, &pRowSrc);
		for (int x = 0; x < cx; x++)
		{
			if (pRowSrc[x] > 0x4000)
				pRowSrc[x] = 0x8000;
		}
	}
}

void _MarkPoints(IImage3 *pSrc, int cx, int cy, color clrPoint, color clrMark)
{
	TimeTrace("_MarkPoints");
	for (int y = 0; y < cy; y++)
	{
		color *pRowSrc;
		pSrc->GetRow(y, 0, &pRowSrc);
		for (int x = 0; x < cx; x++)
		{
			if (pRowSrc[x] == clrPoint)
				pRowSrc[x] = clrMark;
		}
	}
}

void _MarkPoints1(IImage3 *pSrc, int cx, int cy, color clrPoint1, color clrPoint2, color clrMark)
{
	TimeTrace("_MarkPoints");
	for (int y = 0; y < cy; y++)
	{
		color *pRowSrc;
		pSrc->GetRow(y, 0, &pRowSrc);
		for (int x = 0; x < cx; x++)
		{
			if (pRowSrc[x] >= clrPoint1 && pRowSrc[x] <= clrPoint2)
				pRowSrc[x] = clrMark;
		}
	}
}

/*void _ClearPoints(IImage3 *pSrc, int cx, int cy, color clrPoint, color clrMark)
{
	TimeTrace("_MarkPoints");
	for (int y = 0; y < cy; y++)
	{
		color *pRowSrc;
		pSrc->GetRow(y, 0, &pRowSrc);
		for (int x = 0; x < cx; x++)
		{
			if (pRowSrc[x] != clrPoint)
				pRowSrc[x] = clrMark;
		}
	}
}*/

void _DilatePoints(IImage3 *pSrc, int cx, int cy, color clrPoint, color clrMark, int nStep = 1,
	int nMin = 0, int nMax = 0x4000)
{
	TimeTrace("_DilatePoints");
	for (int y = nStep; y < cy-nStep; y++)
	{
		for (int x = nStep; x < cx-nStep; x++)
		{
			color *pRowSrc;
			pSrc->GetRow(y, 0, &pRowSrc);
			if (pRowSrc[x] == clrPoint)
			{
				for (int y1 = y-nStep; y1 <= y+nStep; y1++)
				{
					color *pRowSrc1;
					pSrc->GetRow(y1, 0, &pRowSrc1);
					for (int x1 = x-nStep; x1 <= x+nStep; x1++)
					{
						if (pRowSrc1[x1] >= nMin && pRowSrc1[x1] < nMax)
							pRowSrc1[x1] = clrMark;
					}
				}
			}
		}
	}
}

void _DilatePoints1(IImage3 *pSrc, int cx, int cy, color clrPoint, color clrMark, color clrExpand)
{
	TimeTrace("_DilatePoints1");
	color *pRowSrc0,*pRowSrc1,*pRowSrc2;
	pSrc->GetRow(0, 0, &pRowSrc0);
	pSrc->GetRow(1, 0, &pRowSrc1);
	pSrc->GetRow(2, 0, &pRowSrc2);
	for (int y = 1; y < cy-1; y++)
	{
		for (int x = 1; x < cx-1; x++)
		{
			if (pRowSrc1[x] == clrPoint)
			{
				for (int x1 = x-1; x1 <= x+1; x1++)
				{
					if (pRowSrc0[x1] == clrExpand) pRowSrc0[x1] = clrMark;
					if (pRowSrc1[x1] == clrExpand) pRowSrc1[x1] = clrMark;
					if (pRowSrc2[x1] == clrExpand) pRowSrc2[x1] = clrMark;
				}
			}
		}
		pRowSrc0 = pRowSrc1;
		pRowSrc1 = pRowSrc2;
		pSrc->GetRow(y+1, 0, &pRowSrc2);
	}
}

void _DilatePoints2(IImage3 *pSrc, int cx, int cy, color clrPoint, color clrMark, color clrExpand)
{
	TimeTrace("_DilatePoints2");
	color *pRowSrc0,*pRowSrc1,*pRowSrc2;
	pSrc->GetRow(0, 0, &pRowSrc0);
	pSrc->GetRow(1, 0, &pRowSrc1);
	pSrc->GetRow(2, 0, &pRowSrc2);
	for (int y = 1; y < cy-1; y++)
	{
		for (int x = 1; x < cx-1; x++)
		{
			if (pRowSrc1[x] == clrPoint)
			{
				for (int x1 = x-1; x1 <= x+1; x1++)
				{
					if (pRowSrc0[x1] <= clrExpand) pRowSrc0[x1] = clrMark;
					if (pRowSrc1[x1] <= clrExpand) pRowSrc1[x1] = clrMark;
					if (pRowSrc2[x1] <= clrExpand) pRowSrc2[x1] = clrMark;
				}
			}
		}
		pRowSrc0 = pRowSrc1;
		pRowSrc1 = pRowSrc2;
		pSrc->GetRow(y+1, 0, &pRowSrc2);
	}
}


void _MakeMaximums(int *pMaxs, IImage3 *pSrc, IImage3 *pBin, int x, int y, int cx, int cy,
	int nScope, int nPane)
{
	int clrMax = -1;
	color *pRow;
	pSrc->GetRow(y, nPane, &pRow);
	for (int i = 1; i < nScope && x+i < cx; i++)
	{
		color clr = pRow[x+i];
		if (clr > clrMax) clrMax = clr;
	}
	pMaxs[0] = clrMax;
	clrMax = -1;
	for (i = 1; i < nScope && x-i >= 0; i++)
	{
		color clr = pRow[x-i];
		if (clr > clrMax) clrMax = clr;
	}
	pMaxs[1] = clrMax;
	clrMax = -1;
	int clrMax1 = -1, clrMax2 = -1;
	for (i = 1; i < nScope && y+i < cy; i++)
	{
		pSrc->GetRow(y+i, nPane, &pRow);
		color clr = pRow[x];
		if (clr > clrMax) clrMax = clr;
		clr = x+i < cx ? pRow[x+i] : -1;
		if (clr > clrMax1) clrMax1 = clr;
		clr = x-i >= 0 ? pRow[x-i] : -1;
		if (clr > clrMax2) clrMax2 = clr;
	}
	pMaxs[2] = clrMax;
	pMaxs[4] = clrMax1;
	pMaxs[6] = clrMax2;
	clrMax = -1;
	clrMax1 = -1;
	clrMax2 = -1;
	for (i = 1; i < nScope && y-i >= 0; i++)
	{
		pSrc->GetRow(y-i, nPane, &pRow);
		color clr = pRow[x];
		if (clr > clrMax) clrMax = clr;
		clr = x+i < cx ? pRow[x+i] : -1;
		if (clr > clrMax1) clrMax1 = clr;
		clr = x-i >= 0 ? pRow[x-i] : -1;
		if (clr > clrMax2) clrMax2 = clr;
	}
	pMaxs[3] = clrMax;
	pMaxs[5] = clrMax2;
	pMaxs[7] = clrMax1;
	for (int i = 0; i < 4; i++)
	{
		if (pMaxs[2*i] == -1 && pMaxs[2*i+1] != -1)
			pMaxs[2*i] = pMaxs[2*i+1];
		else if (pMaxs[2*i] != -1 && pMaxs[2*i+1] == -1)
			pMaxs[2*i+1] = pMaxs[2*i];
	}
}

color _MakeMinMaxMax(int *pMaxs, color clrPrev)
{
	int iPos = 0;
	int clrMax = -1;
	for (int i = 0; i < 4; i++)
	{
		color clr = min(pMaxs[2*i],pMaxs[2*i+1]);
		if (clr > clrMax)
		{
			iPos = i;
			clrMax = clr;
		}
	}
	return clrMax>=0?(color)clrMax:clrPrev;
}

void _MakeMinimums(int *pMins, IImage3 *pSrc, IImage3 *pBin, int x, int y, int cx, int cy,
	int nScope, int nPane)
{
	int clrMin = 0x10000;
	color *pRow;
	pSrc->GetRow(y, nPane, &pRow);
	for (int i = 1; i < nScope && x+i < cx; i++)
	{
		color clr = pRow[x+i];
		if (clr < clrMin) clrMin = clr;
	}
	pMins[0] = clrMin;
	clrMin = 0x10000;
	for (i = 1; i < nScope && x-i >= 0; i++)
	{
		color clr = pRow[x-i];
		if (clr < clrMin) clrMin = clr;
	}
	pMins[1] = clrMin;
	clrMin = 0x10000;
	int clrMin1 = 0x10000, clrMin2 = 0x10000;
	for (i = 1; i < nScope && y+i < cy; i++)
	{
		pSrc->GetRow(y+i, nPane, &pRow);
		color clr = pRow[x];
		if (clr < clrMin) clrMin = clr;
		clr = x+i < cx ? pRow[x+i] : -1;
		if (clr < clrMin1) clrMin1 = clr;
		clr = x-i >= 0 ? pRow[x-i] : -1;
		if (clr < clrMin2) clrMin2 = clr;
	}
	pMins[2] = clrMin;
	pMins[4] = clrMin1;
	pMins[6] = clrMin2;
	clrMin = 0x10000;
	clrMin1 = 0x10000;
	clrMin2 = 0x10000;
	for (i = 1; i < nScope && y-i >= 0; i++)
	{
		pSrc->GetRow(y-i, nPane, &pRow);
		color clr = pRow[x];
		if (clr < clrMin) clrMin = clr;
		clr = x+i < cx ? pRow[x+i] : -1;
		if (clr < clrMin1) clrMin1 = clr;
		clr = x-i >= 0 ? pRow[x-i] : -1;
		if (clr < clrMin2) clrMin2 = clr;
	}
	pMins[3] = clrMin;
	pMins[5] = clrMin2;
	pMins[7] = clrMin1;
	for (int i = 0; i < 4; i++)
	{
		if (pMins[2*i] == -1 && pMins[2*i+1] != -1)
			pMins[2*i] = pMins[2*i+1];
		else if (pMins[2*i] != -1 && pMins[2*i+1] == -1)
			pMins[2*i+1] = pMins[2*i];
	}
}

color _MakeMaxMinMin(int *pMins, color clrPrev)
{
	int iPos = 0;
	int clrMin = 0x10000;
	for (int i = 0; i < 4; i++)
	{
		color clr = max(pMins[2*i],pMins[2*i+1]);
		if (clr < clrMin)
		{
			iPos = i;
			clrMin = clr;
		}
	}
	return clrMin<0x10000?(color)clrMin:clrPrev;
}


bool CDelLineBase::InvokeFilter()
{
	IImage3Ptr pSrc = GetDataArgument("Source");
	int nDarkLines = GetArgLong("DarkLines");
	int nSize = GetArgLong("Size");
	int nThreshold = GetArgLong("Threshold");
	int nLineWidth = GetArgLong("LineWidth");
	s_dMaxAngleShort = s_dMaxAngleFar = s_dMaxAngle3Far = GetArgDouble("Angle");
	s_dCurveCoef = GetArgDouble("CurveCoef");
	int nDebug = GetArgLong("Debug");
	int bGS = _IsGrayScale(pSrc);
	if (bGS == -1) return false;
	int cx,cy;
	pSrc->GetSize(&cx, &cy);
	StartNotification(500);
	IImage3Ptr sptrGS1 = _MakeGrayscaleImage(pSrc,cx,cy,bGS,nDarkLines==0);
	Notify(15);
	IImage3Ptr sptrGS2 = _MakeGrayscaleImage(sptrGS1,cx,cy,bGS,false);
	IImage3 *pGSIntr = sptrGS1, *pGSRes = sptrGS2;
	Notify(30);
	_Process3x3(pGSRes,pGSIntr,cx,cy,_Average3x3);
	Notify(45);
	_Process3x3(pGSRes,pGSIntr,cx,cy,_Erode3x3);
	_Process3x3(pGSRes,pGSIntr,cx,cy,_Dilate3x3);
	Notify(60);
	_Process3x3(pGSRes,pGSIntr,cx,cy,_Average3x3);
	Notify(75);
	if (nDebug <= 9) _Threshold(pGSRes,pGSIntr,cx,cy,nThreshold);
	Notify(90);
	if (nDebug <= 8) _Skeleton(pGSRes,cx,cy);
	Notify(100);
	CLinesArray arrLines(true);
	if (nDebug <= 7) _SearchLines(arrLines, pGSRes, cx, cy, this, 100);
	if (nDebug <= 7) _FilterShort(arrLines, pGSRes);
	Notify(200);
	if (nDebug <= 6) ConnectLines(arrLines, pGSRes, cx, cy, 200, 450);
	Notify(450);
	if (nDebug <= 6) _FilterShort2(arrLines, pGSRes, nSize);
	if (nDebug <= 6 && GetValueInt(GetAppUnknown(), "\\Metall\\DelLines", "NotRemoveHorizontalLines", 0)) _FilterHorizontal(arrLines);
	// 0x4000 - lines to delete. 0x8000 - other lines. 0x4001 - intermediate points during
	// dilatation, 0xC000 - junctions
	if (nDebug <= 5) _MapLines(arrLines, pGSRes, 0x4000);
	if (nDebug <= 4) _MarkOtherPoints(pGSRes, cx, cy);
	MakeResult(pSrc, pGSRes, cx, cy, bGS?true:false, nDarkLines, nLineWidth, nDebug);
	FinishNotification();
	SetModified(true);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//CDelLine
////////////////////////////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg	CDelLineInfo::s_pargs[] = 
{
	{"DarkLines",szArgumentTypeInt, "1", true, false },
	{"Size",szArgumentTypeInt, "20", true, false },
	{"Threshold",szArgumentTypeInt, "1", true, false },
	{"LineWidth",szArgumentTypeInt, "3", true, false },
	{"Angle",szArgumentTypeDouble,"0.15", true, false },
	{"CurveCoef",szArgumentTypeDouble, "1.3", true, false },
	{"Source",szArgumentTypeImage, 0, true, true },
	{"Result",szArgumentTypeImage, 0, false, true },
	{"Debug",szArgumentTypeInt, "0", true, false },
	{0, 0, 0, false, false },
};


CDelLine::CDelLine()
{
}

void CDelLine::MakeResult(IImage3 *pSrc, IImage3 *pGSRes, int cx, int cy, bool bGS,
	int nDarkLines, int nLineWidth, int nDebug)
{
	if (nDebug <= 3) _DilatePoints(pGSRes, cx, cy, 0x8000, 0x8001);
	if (nDebug <= 2) _DilatePoints(pGSRes, cx, cy, 0x4000, 0x4001, nLineWidth);
	if (nDebug <= 2) _MarkPoints(pGSRes, cx, cy, 0x4001, 0x4000);
	IImage3Ptr pDst = GetDataResult();
	TimeTrace("_MakeResult");
	pDst->CreateImage(cx, cy, _bstr_t(bGS?"GRAY":"RGB"), bGS?1:3);
	for (int y = 0; y < cy; y++)
	{
		color *pRowSrc0,*pRowSrc1,*pRowSrc2;
		color *pRowDst0,*pRowDst1,*pRowDst2;
		color *pBinRow;
		pGSRes->GetRow(y, 0, &pBinRow);
		pDst->GetRow(y, 0, &pRowDst0);
		pSrc->GetRow(y, 0, &pRowSrc0);
		if (!bGS)
		{
			pDst->GetRow(y, 1, &pRowDst1);
			pSrc->GetRow(y, 1, &pRowSrc1);
			pDst->GetRow(y, 2, &pRowDst2);
			pSrc->GetRow(y, 2, &pRowSrc2);
		}
		for (int x = 0; x < cx; x++)
		{
			if (nDebug)
				pRowDst0[x] = pBinRow[x];
			else
				if (pBinRow[x] == 0x4000)
				{
					if (nDarkLines)
					{
						int aMaxs[8];
						_MakeMaximums(aMaxs, pSrc, pGSRes, x, y, cx, cy, 10, 0);
						pRowDst0[x] = _MakeMinMaxMax(aMaxs, pRowDst0[x]);
						if (!bGS)
						{
							_MakeMaximums(aMaxs, pSrc, pGSRes, x, y, cx, cy, 10, 1);
							pRowDst1[x] = _MakeMinMaxMax(aMaxs, pRowDst1[x]);
							_MakeMaximums(aMaxs, pSrc, pGSRes, x, y, cx, cy, 10, 2);
							pRowDst2[x] = _MakeMinMaxMax(aMaxs, pRowDst2[x]);
						}
					}
					else
					{
						int aMins[8];
						_MakeMinimums(aMins, pSrc, pGSRes, x, y, cx, cy, 10, 0);
						pRowDst0[x] = _MakeMaxMinMin(aMins, pRowDst0[x]);
						if (!bGS)
						{
							_MakeMinimums(aMins, pSrc, pGSRes, x, y, cx, cy, 10, 1);
							pRowDst1[x] = _MakeMaxMinMin(aMins, pRowDst1[x]);
							_MakeMinimums(aMins, pSrc, pGSRes, x, y, cx, cy, 10, 2);
							pRowDst2[x] = _MakeMaxMinMin(aMins, pRowDst2[x]);
						}
					}
				}
				else
				{
					pRowDst0[x] = pRowSrc0[x];
					if (!bGS)
					{
						pRowDst1[x] = pRowSrc1[x];
						pRowDst2[x] = pRowSrc2[x];
					}
				}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//CDelLineBin
////////////////////////////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg	CDelLineBinInfo::s_pargs[] = 
{
	{"DarkLines",szArgumentTypeInt, "1", true, false },
	{"Size",szArgumentTypeInt, "20", true, false },
	{"Threshold",szArgumentTypeInt, "1", true, false },
	{"LineWidth",szArgumentTypeInt, "3", true, false },
	{"Angle",szArgumentTypeDouble, "0.15", true, false },
	{"CurveCoef",szArgumentTypeDouble, "1.3", true, false },
	{"Source",szArgumentTypeImage, 0, true, true },
	{"Binary",szArgumentTypeBinaryImage, 0, true, true },
	{"Result",szArgumentTypeBinaryImage, 0, false, true },
	{"Debug",szArgumentTypeInt, "0", true, false },
	{0, 0, 0, false, false },
};


CDelLineBin::CDelLineBin()
{
}

void CDelLineBin::MakeResult(IImage3 *pSrc, IImage3 *pMaskImg, int cx, int cy, bool bGS,
	int nDarkLines, int nLineWidth, int nDebug)
{
	_MarkPoints(pMaskImg, cx, cy, 0x8000, 0);
	for (int i = 0; i < nLineWidth; i++)
	{
		_DilatePoints2(pMaskImg, cx, cy, 0x4000, 0x4001, 0x3FFF);
		_MarkPoints(pMaskImg, cx, cy, 0x4001, 0x4000);
	}
//	_DilatePoints(pMaskImg, cx, cy, 0x4000, 0x4001, nLineWidth);
//	_MarkPoints(pMaskImg, cx, cy, 0x4001, 0x4000);
	IBinaryImagePtr pSrcBin = GetDataArgument("Binary");
	IBinaryImagePtr pDstBin = GetDataResult();
	TimeTrace("MakeResult");
	// Mark points of objects, which are not included in delete lines, as 0xFFFF.
//	_ClearPoints(pMaskImg, cx, cy, 0x4000, 0);
	for (int y = 0; y < cy; y++)
	{
		byte *pRowSrcBin;
		color *pMaskImgRow;
		pMaskImg->GetRow(y, 0, &pMaskImgRow);
		pSrcBin->GetRow(&pRowSrcBin, y, TRUE);
		for (int x = 0; x < cx; x++)
			if (pMaskImgRow[x] != 0x4000)
				pMaskImgRow[x] =  pRowSrcBin[x] > 127 ? 0xFFFF : 0;
	}
	// Dilate object to eliminate objects points, included in deleted lines
	color clr = 0xFFFF;
	int nSteps = min(2*nLineWidth+1, 256);
	for (y = 0; y < nSteps; y++)
	{
		_DilatePoints1(pMaskImg, cx, cy, clr, clr-1, 0x4000);
		clr--;
	}
	_MarkPoints1(pMaskImg, cx, cy, 0xFF00, 0xFFFF, 0xFFFF);
/*	for (y = 0; y < 2*nLineWidth+1; y++)
	{
		_DilatePoints(pMaskImg, cx, cy, 0xFFFF, 0xFFFE, 1, 0x4000, 0x4001);
		_MarkPoints(pMaskImg, cx, cy, 0xFFFE, 0xFFFF);
	}*/
	pDstBin->CreateNew(cx, cy);
	for (y = 0; y < cy; y++)
	{
		byte *pRowSrcBin,*pRowDstBin;
		color *pMaskImgRow;
		pMaskImg->GetRow(y, 0, &pMaskImgRow);
		pDstBin->GetRow(&pRowDstBin, y, TRUE);
		pSrcBin->GetRow(&pRowSrcBin, y, TRUE);
		for (int x = 0; x < cx; x++)
		{
			if (pMaskImgRow[x] == 0x4000)
				pRowDstBin[x] = 0;
			else
				pRowDstBin[x] = pRowSrcBin[x];
		}
	}
}
