#include "stdafx.h"
#include "SewLi.h"
#include "FindPos.h"
#include "atltypes.h"
#include "atlstr.h"
#include "SewUtils.h"
#include <math.h>
#include <io.h>
#include <fcntl.h>
#include <\vt5\awin\trace.h>

#if defined(_DEBUG)
#define _TEST_TIME
#define _TEST_PICTURES
#endif

static int s_nFoundDiff = -1;

int GetOptimalDiff() {return s_nFoundDiff;}


static int GetDifference(IImage3 *pimgBase, IImage3 *pimgSew, POINT ptOffset)
{
	int cxBase,cyBase,cxSew,cySew;
	pimgBase->GetSize(&cxBase, &cyBase);
	pimgSew->GetSize(&cxSew, &cySew);
	_rect rcBase(0,0,cxBase,cyBase); // sew rect in base coordinates
	if (ptOffset.x > 0)
		rcBase.left = ptOffset.x;
	if (ptOffset.x+cxSew < cxBase)
		rcBase.right = ptOffset.x+cxSew;
	if (ptOffset.y > 0)
		rcBase.top = ptOffset.y;
	if (ptOffset.y+cySew < cyBase)
		rcBase.bottom = ptOffset.y+cySew;
	_point ptSew(rcBase.left-ptOffset.x,rcBase.top-ptOffset.y);
	int nPanes = GetImagePaneCount(pimgBase);
	long lSumDiff = 0, lCount = 0;
	int n = 3;
	for (int yBase = rcBase.top; yBase < rcBase.bottom; yBase += n)
	{
		int ySew = yBase-ptOffset.y;
		color *pRowBase,*pRowSew;
		pimgBase->GetRow(yBase, 0, &pRowBase);
		pimgSew->GetRow(ySew, 0, &pRowSew);
		for (int xBase = rcBase.left; xBase < rcBase.right; xBase += n)
		{
			long l = pRowBase[xBase]-pRowSew[xBase-ptOffset.x];
			if (l < 0) l = -l;
			lSumDiff += l>>8;
			lCount++;
		}
	}
	lSumDiff /= lCount;
	return lSumDiff;
}

static POINT s_aDirs1[] =
{
	{-1,0},
	{0,1},
	{1,0},
	{0,-1},
};

const int s_nDirs1 = 4;

static POINT s_aDirs2[] =
{
	{-2,0},
	{0,2},
	{2,0},
	{0,-2},
	{-1,1},
	{1,1},
	{-1,-1},
	{1,-1},
};

const int s_nDirs2 = 8;

static bool ScanDirections(_rect rcBound, LPWORD pDiffs, IImage3 *pimgBase, IImage3 *pimgSew,
	int &nFoundOpt, POINT ptOffs, POINT &ptOffs1, DWORD dwRow, POINT *aDirs, int nDirs)
{
	bool bFound = false;
	for (int i = 0; i < nDirs; i++)
	{
		POINT ptTry;
		ptTry.x = ptOffs.x+aDirs[i].x;
		if (ptTry.x < rcBound.left || ptTry.x >= rcBound.right)
			continue;
		ptTry.y = ptOffs.y+aDirs[i].y;
		if (ptTry.y < rcBound.top || ptTry.y >= rcBound.bottom)
			continue;
		int nPos = (ptTry.y-rcBound.top)*dwRow+(ptTry.x-rcBound.left);
		WORD n = pDiffs[nPos];
		if (n == 0xFFFF)
		{
			n = GetDifference(pimgBase, pimgSew, ptTry);
			pDiffs[nPos] = n;
		}
		if (n < nFoundOpt)
		{
			bFound = true;
			nFoundOpt = n;
			ptOffs1 = ptTry;
		}
	}
	return bFound;
}

bool GradientDecent(IImage3 *pimgBase, IImage3 *pimgSew, POINT ptOffsStart, POINT *pptRes, int nMinOvr, int *pnOptDiff)
{
	int cxBase,cyBase,cxSew,cySew;
	pimgBase->GetSize(&cxBase, &cyBase);
	pimgSew->GetSize(&cxSew, &cySew);
	_rect rcBound(-cxSew+cxSew*nMinOvr/100,-cySew+cySew*nMinOvr/100,
		cxBase-cxBase*nMinOvr/100,cyBase-cyBase*nMinOvr/100);
	_ptr_t2<WORD> pDiffs(rcBound.width()*rcBound.height());
	memset(pDiffs.ptr,0xFF,rcBound.width()*rcBound.height()*sizeof(WORD));
	DWORD dwRow = rcBound.width();
	int nOptDiff = GetDifference(pimgBase, pimgSew, ptOffsStart);
	int nInitDiff = nOptDiff;
	POINT ptOffs = ptOffsStart;
	while (1)
	{
		int nFoundOpt = nOptDiff;
		POINT ptOffs1;
		bool bFound = ScanDirections(rcBound, pDiffs, pimgBase, pimgSew, nFoundOpt, ptOffs,
			ptOffs1, dwRow, s_aDirs1, s_nDirs1);
		if (!bFound)
		{
			bFound = ScanDirections(rcBound, pDiffs, pimgBase, pimgSew, nFoundOpt, ptOffs,
				ptOffs1, dwRow, s_aDirs2, s_nDirs2);
			if (!bFound)
				break;
		}
		ptOffs = ptOffs1;
		nOptDiff = nFoundOpt;
	}
	*pnOptDiff = nOptDiff;
	*pptRes = ptOffs;
	return true;
}

bool CorrectPos(ISewImageList *pFragList, ISewFragment *pFragTest, POINT ptOrig, POINT *pptRes)
{
#if defined(_TEST_TIME)	
	DWORD dwStart = GetTickCount();
#endif
	SIZE szTest;
	pFragTest->GetSize(&szTest);
	CRect rcOvr(ptOrig,szTest);
	int nMaxPercent = 0;
	long lposMax = 0;
	long lpos;
	ISewFragmentPtr sptrFrag;
	pFragList->GetFirstFragmentPos(&lpos);
	while (lpos)
	{
		long lposPrev = lpos;
		pFragList->GetNextFragment(&lpos, &sptrFrag);
		if ((ISewFragment *)sptrFrag == pFragTest)
			continue;
		POINT pt;
		sptrFrag->GetOffset(&pt);
		SIZE sz;
		sptrFrag->GetSize(&sz);
		if (pt.x < rcOvr.right && pt.x+sz.cx >= rcOvr.left &&
			pt.y < rcOvr.bottom && pt.y+sz.cy >= rcOvr.top)
		{
			int x1 = max(rcOvr.left,pt.x);
			int x2 = min(rcOvr.right,pt.x+sz.cx);
			int y1 = max(rcOvr.top,pt.y);
			int y2 = min(rcOvr.bottom,pt.y+sz.cy);
			int d = (x2-x1)*(y2-y1)*100/(rcOvr.Width()*rcOvr.Height());
			if (d > nMaxPercent)
			{
				nMaxPercent = d;
				lposMax = lposPrev;
			}
		}
	}
	int nMinOvrPerc = GetValueInt(GetAppUnknown(), "\\Sew Large Image\\Correction", "MinOverlapPercent", 10);
	if (lposMax == 0 || nMaxPercent < nMinOvrPerc)
		return false;
	pFragList->GetNextFragment(&lposMax, &sptrFrag);
	POINT ptOffsN;
	sptrFrag->GetOffset(&ptOffsN);
	IUnknownPtr punkImgTest,punkImg;
	int nRangeCoef;
	pFragList->GetRangeCoef(&nRangeCoef);
	int iZoom = 1;
	for (int i = 0; i < g_nFragmentImages; i++)
	{
		sptrFrag->GetImage(&iZoom, &punkImg);
		pFragTest->GetImage(&iZoom, &punkImgTest);
		if (punkImg != 0 && punkImgTest != 0)
			break;
		iZoom *= nRangeCoef;
	}
	CLoadFragment LoadFrag,LoadTestFrag;
	if (punkImg == 0 && punkImgTest == 0)
	{
		iZoom = GetValueInt(GetAppUnknown(), "\\Sew Large Image\\Correction", "UsedZoom", 2);
		LoadFrag.Load(sptrFrag, iZoom);
		LoadTestFrag.Load(pFragTest, iZoom);
		sptrFrag->GetImage(&iZoom, &punkImg);
		pFragTest->GetImage(&iZoom, &punkImgTest);
	}
	IImage3Ptr img(punkImg);
	IImage3Ptr imgTest(punkImgTest);
	if (img == 0 || imgTest == 0)
		return false;
	CPoint ptImgOffs((ptOrig.x-ptOffsN.x)/iZoom,(ptOrig.y-ptOffsN.y)/iZoom);
	CPoint ptImgOffsR;
	int nOptDiff;
	if ( GradientDecent(img,imgTest,ptImgOffs,&ptImgOffsR,nMinOvrPerc,&nOptDiff))
	{
		if (ptImgOffs.x != ptImgOffsR.x)
			pptRes->x = ptOffsN.x+ptImgOffsR.x*iZoom;
		if (ptImgOffs.y != ptImgOffsR.y)
			pptRes->y = ptOffsN.y+ptImgOffsR.y*iZoom;
		s_nFoundDiff = abs(nOptDiff);
#if defined(_TEST_TIME)
		int nDist = (int)sqrt(double((pptRes->x-ptOrig.x)*(pptRes->x-ptOrig.x)+(pptRes->y-ptOrig.y)*(pptRes->y-ptOrig.y)));
		trace("Corect pos, (%d,%d)->(%d,%d), diff %d, dist %d, time %d ms\n", ptOrig.x, ptOrig.y,
			pptRes->x, pptRes->y, nOptDiff, nDist, GetTickCount()-dwStart);
#endif
		return true;
	}
	return false;
}

class CReducedImage
{
public:
	int m_cx,m_cy;
	BYTE *m_pData;
	CReducedImage()
	{
		m_cx = m_cy = 0;
		m_pData = NULL;
	}
	~CReducedImage()
	{
		delete m_pData;
	}
	void Build(IImage3 *pimg, int nZoom)
	{
		int cx,cy;
		pimg->GetSize(&cx, &cy);
		m_cx = cx/nZoom;
		m_cy = cy/nZoom;
		_ptr_t2<DWORD> arrSums(m_cx*m_cy);
		arrSums.zero();
		_ptr_t2<DWORD> arrCnts(m_cx*m_cy);
		arrCnts.zero();
		int nPanes = GetImagePaneCount(pimg);
		for (int y = 0; y < cy; y++)
		{
			for (int c = 0; c < nPanes; c++)
			{
				color *pRow;
				pimg->GetRow(y, c, &pRow);
				for (int x = 0; x < cx; x++)
				{
					byte by = ColorAsByte(pRow[x]);
					if (y/nZoom*m_cx+x/nZoom > m_cx*m_cy)
						break;
					arrSums[y/nZoom*m_cx+x/nZoom] += by;
					arrCnts[y/nZoom*m_cx+x/nZoom]++;
				}
			}
		}
		m_pData = new BYTE[m_cx*m_cy];
		for (int i = 0; i < m_cx*m_cy; i++)
		{
			if (arrCnts[i] > 0)
			{
				DWORD dw = arrSums[i]/arrCnts[i];
				m_pData[i] = dw>255?255:(byte)dw;
			}
			else
				m_pData[i] = 0;
		}
		Contrast();
	}

	void Contrast()
	{
		byte	*p;
		long	hist[256], i;
		ZeroMemory(&hist, sizeof( hist ));
		for( i = 0, p = m_pData; i < m_cx*m_cy; i++, p++ )
			hist[*p]++;
		int	nMin = 0;
		int	nMax = 255;
		int	nMinPercent = m_cx*m_cy/100;
		int	nMaxPercent = m_cx*m_cy/100;
		while( nMinPercent >= 0 )
		{
			nMinPercent-=hist[nMin];
			nMin++;
		}
		while( nMaxPercent >= 0 )
		{
			nMaxPercent-=hist[nMax];
			nMax--;
		}
		int	a = max(nMax-nMin, 1);
		int	b = nMin;
		for( i = 0, p = m_pData; i < m_cx*m_cy; i++, p++ )
			*p = max( 0, min( 255, (*p-b)*256/a ) );
	}


#if defined(_TEST_PICTURES)
	void WriteDebug(const char *pszPathName)
	{
		DWORD dwRow = (m_cx+3)/4*4;
		BITMAPFILEHEADER bfh = {0};
		bfh.bfType = ((int('M')<<8)|int('B'));
		bfh.bfSize = sizeof(bfh)+sizeof(BITMAPINFOHEADER)+1024+dwRow*m_cy;
		bfh.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+1024;
		BITMAPINFOHEADER bi = {0};
		bi.biSize = sizeof(bi);
		bi.biHeight = m_cy;
		bi.biWidth = m_cx;
		bi.biBitCount = 8;
		bi.biPlanes = 1;
		bi.biClrUsed = bi.biClrImportant = 256;
		RGBQUAD rgb[256];
		for (int i = 0; i < 256; i++)
		{
			rgb[i].rgbRed = rgb[i].rgbGreen = rgb[i].rgbBlue = i;
			rgb[i].rgbReserved = 0;
		}
		int h = open(pszPathName, _O_BINARY|_O_CREAT|_O_RDWR);
		if (h != -1)
		{
			write(h, &bfh, sizeof(bfh));
			write(h, &bi, sizeof(bi));
			write(h, &rgb, sizeof(rgb));
			_ptr_t2<BYTE> row(dwRow);
			for (int i = 0; i < m_cy; i++)
			{
				memset((BYTE *)row, 0, dwRow);
				int y = m_cy-i-1;
				BYTE *p = m_pData+y*m_cx;
				memcpy(row, p, m_cx);
				write(h, p, dwRow);
			}
			close(h);
		}
	}
#endif
};

static int GetDifference(CReducedImage &riBase, CReducedImage &riSew, POINT ptOffset)
{
	_rect rcBase(0,0,riBase.m_cx,riBase.m_cy); // sew rect in base coordinates
	if (ptOffset.x > 0)
		rcBase.left = ptOffset.x;
	if (ptOffset.x+riSew.m_cx < rcBase.right)
		rcBase.right = ptOffset.x+riSew.m_cx;
	if (ptOffset.y > 0)
		rcBase.top = ptOffset.y;
	if (ptOffset.y+riSew.m_cy < rcBase.bottom)
		rcBase.bottom = ptOffset.y+riSew.m_cy;
	if (rcBase.left == 0 && rcBase.right == riBase.m_cx)
		rcBase.right--;
	_point ptSew(rcBase.left-ptOffset.x,rcBase.top-ptOffset.y);
	long lSumDiff = 0;
	long lCount = 0;
	long lOffs = rcBase.right == riBase.m_cx ? -1 : 1;
	long lOffs1 = rcBase.right == ptOffset.x+riSew.m_cx ? -1 : 1;
	for (int yBase = rcBase.top; yBase < rcBase.bottom; yBase += 3)
	{
		int ySew = yBase-ptOffset.y;
		byte *pRowBase = riBase.m_pData+yBase*riBase.m_cx;
		byte *pRowSew = riSew.m_pData+ySew*riSew.m_cx;
		for (int xBase = rcBase.left; xBase < rcBase.right; xBase+= 3)
		{
			long l = (long)pRowBase[xBase]-(long)pRowSew[xBase-ptOffset.x];
			if (l < 0) l = -l;
//			if (pRowBase[xBase] == 0 || pRowSew[xBase-ptOffset.x+lOffs1] == 0)
//				continue;
			long l1 = (long)pRowBase[xBase+lOffs]-(long)pRowBase[xBase];
			if (l1 < 0) l1 = -l1;
			long l2 = (long)pRowSew[xBase-ptOffset.x+lOffs1]-(long)pRowSew[xBase-ptOffset.x];
			if (l2 < 0) l2 = -l2;
			if ((l1 <= 10 || l2 <= 10) && l <= 10)
				continue;
			lSumDiff += l;
			lCount++;
		}
	}
	int nMinPoints = max(rcBase.width()*rcBase.height()/9/10, 10);
	if (lCount <= nMinPoints)
		return INT_MAX;
	lSumDiff /= lCount;
	return lSumDiff;
}

static int GetHistoDiff(int *pnHisto, int nTotal)
{
	int nTestPts = max(1,nTotal/10);
	long lSumMax = 0;
	long lBriMax = 0;
	for (int i = 255; i >= 0; i--)
	{
		lSumMax += pnHisto[i];
		lBriMax += i*pnHisto[i];
		if (lSumMax >= nTestPts)
			break;
	}
	lBriMax /= lSumMax;
	long lSumMin = 0;
	long lBriMin = 0;
	for (int i = 0; i < 256; i++)
	{
		lSumMin += pnHisto[i];
		lBriMin += i*pnHisto[i];
		if (lSumMin >= nTestPts)
			break;
	}
	lBriMin /= lSumMin;
	return (lBriMax-lBriMin);
};

static int GetRelDifference(CReducedImage &riBase, CReducedImage &riSew, POINT ptOffset)
{
	_rect rcBase(0,0,riBase.m_cx,riBase.m_cy); // sew rect in base coordinates
	if (ptOffset.x > 0)
		rcBase.left = ptOffset.x;
	if (ptOffset.x+riSew.m_cx < rcBase.right)
		rcBase.right = ptOffset.x+riSew.m_cx;
	if (ptOffset.y > 0)
		rcBase.top = ptOffset.y;
	if (ptOffset.y+riSew.m_cy < rcBase.bottom)
		rcBase.bottom = ptOffset.y+riSew.m_cy;
	_point ptSew(rcBase.left-ptOffset.x,rcBase.top-ptOffset.y);
	int HistoBase[256],HistoSew[256];
	memset(HistoBase,0,sizeof(HistoBase));
	memset(HistoSew,0,sizeof(HistoSew));
	long lSumDiff = 0;
	long lCount = 0;
	for (int yBase = rcBase.top; yBase < rcBase.bottom; yBase += 3)
	{
		int ySew = yBase-ptOffset.y;
		byte *pRowBase = riBase.m_pData+yBase*riBase.m_cx;
		byte *pRowSew = riSew.m_pData+ySew*riSew.m_cx;
		for (int xBase = rcBase.left; xBase < rcBase.right; xBase+= 3)
		{
			long l = (long)pRowBase[xBase]-(long)pRowSew[xBase-ptOffset.x];
			if (l < 0) l = -l;
			HistoBase[pRowBase[xBase]]++;
			HistoSew[pRowSew[xBase-ptOffset.x]]++;
			lSumDiff += l;
			lCount++;
		}
	}
	lSumDiff /= lCount;
	return lSumDiff;
	long lBaseDiff = GetHistoDiff(HistoBase, lCount);
	long lSewDiff = GetHistoDiff(HistoSew, lCount);
	if (lBaseDiff == 0 || lSewDiff == 0)
		return INT_MAX;
	return (lSumDiff*lSumDiff*255)/lBaseDiff/lSewDiff;
}


bool FindOptimalPosition(ISewImageList *pFragList, ISewFragment *pFragPrev,
	IImage3 *pimgTest, POINT *pptPos)
{
#if defined(_TEST_TIME)	
	DWORD dwStart = GetTickCount();
#endif
	// 1. Prepare two images to find position.
	BOOL bGlobalSearch = GetValueInt(GetAppUnknown(), "\\Sew Large Image\\FindOptimalPosition", "GlobalSearch", FALSE);
	int nMinOvrPerc = GetValueInt(GetAppUnknown(), "\\Sew Large Image\\FindOptimalPosition", "MinOverlapPercent", 10);
	int nMaxOvrPerc = GetValueInt(GetAppUnknown(), "\\Sew Large Image\\FindOptimalPosition", "MaxOverlapPercent", 40);
	int nSearchPerc = GetValueInt(GetAppUnknown(), "\\Sew Large Image\\FindOptimalPosition", "SearchPercent", 30);
	int nResolution = 8;
	IUnknownPtr punkImgPrev;
	int nRangeCoef;
	pFragList->GetRangeCoef(&nRangeCoef);
	POINT ptPosPrev;
	pFragPrev->GetOffset(&ptPosPrev);
	SIZE szPrev;
	pFragPrev->GetSize(&szPrev);
	int iZoom = 1;
	for (int i = 0; i < g_nFragmentImages; i++)
	{
		pFragPrev->GetImage(&iZoom, &punkImgPrev);
		if (punkImgPrev != 0)
			break;
		iZoom *= nRangeCoef;
	}
	CLoadFragment LoadPrevFrag;
	if (punkImgPrev == 0)
	{
		iZoom = GetValueInt(GetAppUnknown(), "\\Sew Large Image\\FindOptimalPosition", "UsedZoom", 1);
		LoadPrevFrag.Load(pFragPrev, iZoom);
		pFragPrev->GetImage(&iZoom, &punkImgPrev);
	}
	// 2. Make reduced images.
	IImage3Ptr imgPrev(punkImgPrev);
	int nZRes = nResolution/iZoom;
	int cxSumPrev = szPrev.cx/nResolution;
	int cySumPrev = szPrev.cy/nResolution;
	CReducedImage riPrev,riTest;
	riPrev.Build(imgPrev, nZRes);
	riTest.Build(pimgTest, nResolution);
#if defined(_TEST_PICTURES)
	riPrev.WriteDebug("c:\\riPrev.bmp");
	riTest.WriteDebug("c:\\ritest.bmp");
#endif
	int nMinOvrX = riPrev.m_cx*nMinOvrPerc/100;
	int nMinOvrY = riPrev.m_cy*nMinOvrPerc/100;
	// 3. Search optiomal position on reduced images.
	int nDiff = INT_MAX;
	CPoint ptOpt(0,0);
	int nMinX,nMaxX,nMinY,nMaxY;
	if (bGlobalSearch)
	{
		nMinX = -riTest.m_cx+nMinOvrX;
		nMaxX = riPrev.m_cx-nMinOvrX;
		nMinY = -riTest.m_cy+nMinOvrY;
		nMaxY = riPrev.m_cy-nMinOvrY;
	}
	else
	{
		int nPrevX = pptPos->x/nResolution;
		int nPrevY = pptPos->y/nResolution;
		int nSearchX = riPrev.m_cx*nSearchPerc/100;
		int nSearchY = riPrev.m_cy*nSearchPerc/100;
		int nLimX1 = -riTest.m_cx+nMinOvrX;
		int nLimX2 = riPrev.m_cx-nMinOvrX;
		int nLimY1 = -riTest.m_cy+nMinOvrY;
		int nLimY2 = riPrev.m_cy-nMinOvrY;
		nMinX = min(max(nLimX1,nPrevX-nSearchX),nLimX2);
		nMaxX = min(max(nLimX1,nPrevX+nSearchX),nLimX2);
		nMinY = min(max(nLimY1,nPrevY-nSearchY),nLimY2);
		nMaxY = min(max(nLimY1,nPrevY+nSearchY),nLimY2);
	}
	for (int x = nMinX; x < nMaxX; x++)
	{
		for (int y = nMinY; y < nMaxY; y++)
		{
			int nFDiff = GetRelDifference(riPrev, riTest, CPoint(x,y));
			if (nFDiff < nDiff)
			{
				nDiff = nFDiff;
				ptOpt = CPoint(x,y);
			}
		}
	}
	if (nDiff == INT_MAX)
		return false;
	// Find exact optimal position using full images
#if defined(_TEST_TIME)
	DWORD dwEndFind = GetTickCount();
#endif
	BOOL bUseGD = GetValueInt(GetAppUnknown(), "\\Sew Large Image\\Correction", "UseGradientDescent", FALSE);
	int nMinOvrPerc1 = GetValueInt(GetAppUnknown(), "\\Sew Large Image\\Correction", "MinOverlapPercent", 10);
	int nDiffGD;
	CPoint ptResGD;
	if (bUseGD)
	{
		if (iZoom == 1)
		{
			CPoint ptStartGD(ptOpt.x*nResolution, ptOpt.y*nResolution);
			GradientDecent(imgPrev, pimgTest, ptStartGD, &ptResGD, nMinOvrPerc1, &nDiffGD);
		}
		else
		{
			IUnknownPtr punkZImage = BuildZoomedImage(pimgTest, iZoom);
			IImage3Ptr img(punkZImage);
			if (img != 0)
			{
				CPoint ptStartGD(ptOpt.x*nZRes, ptOpt.y*nZRes);
				GradientDecent(imgPrev, img, ptStartGD, &ptResGD, nMinOvrPerc1, &nDiffGD);
				ptResGD.x *= iZoom;
				ptResGD.y *= iZoom;
			}
		}
	}
	else
	{
		IImage3Ptr imgTest1;
		if (iZoom == 1)
			imgTest1 = pimgTest;
		else
			imgTest1 = BuildZoomedImage(pimgTest, iZoom);
		int nPrevImgCX,nPrevImgCY;
		imgPrev->GetSize(&nPrevImgCX,&nPrevImgCY);
		int nTestImgCX,nTestImgCY;
		imgTest1->GetSize(&nTestImgCX,&nTestImgCY);
		int nMinOvrX = nPrevImgCX*nMinOvrPerc/100;
		int nMinOvrY = nPrevImgCY*nMinOvrPerc/100;
		CPoint ptStartGD(ptOpt.x*nZRes, ptOpt.y*nZRes);
		int x1 = min(max(ptStartGD.x-nZRes/2,-nTestImgCX+nMinOvrX),nPrevImgCX-nMinOvrX);
		int x2 = min(max(ptStartGD.x+nZRes/2,-nTestImgCX+nMinOvrX),nPrevImgCX-nMinOvrX);
		int y1 = min(max(ptStartGD.y-nZRes/2,-nTestImgCY+nMinOvrY),nPrevImgCY-nMinOvrY);
		int y2 = min(max(ptStartGD.y+nZRes/2,-nTestImgCY+nMinOvrY),nPrevImgCY-nMinOvrY);
		int nOptDiff = INT_MAX;
		CPoint ptOpt = ptStartGD;
		for (int x = x1; x < x2; x++)
		{
			for (int y = y1; y < y2; y++)
			{
				int nCDiff = GetDifference(imgPrev, imgTest1, CPoint(x,y));
				if (nCDiff < nOptDiff)
				{
					nOptDiff = nCDiff;
					ptOpt = CPoint(x,y);
				}
			}
		}
		ptResGD.x = ptOpt.x*iZoom;
		ptResGD.y = ptOpt.y*iZoom;
		nDiffGD = nOptDiff;
	}

	if (nDiff < INT_MAX)
	{
		pptPos->x = ptPosPrev.x+ptResGD.x;
		pptPos->y = ptPosPrev.y+ptResGD.y;
#if defined(_TEST_TIME)
		DWORD dwNow = GetTickCount();
		message<mc_DebugText>("FindOptimalPosition ->(%d,%d)->(%d,%d), diff %d->%d, time %d=%d+%d ms\n",
			ptPosPrev.x+ptOpt.x*nResolution, ptPosPrev.y+ptOpt.y*nResolution,
			pptPos->x, pptPos->y, nDiff, nDiffGD, dwNow-dwStart, dwEndFind-dwStart, dwNow-dwEndFind);
#endif
		return true;
	}
	return false;
}


CSewImage::CSewImage()
{
	m_x = m_y = m_cx = m_cy = 0;
	m_pbytes = 0;
}

CSewImage::~CSewImage()
{
	if( m_pbytes  )delete []m_pbytes;
	m_pbytes  = 0;
}

void CSewImage::SetImage(IImage3 *pimg, BOOL bContrast)
{
	::dbg_assert( m_pbytes == 0 );

	IUnknownPtr punkCC;
	pimg->GetColorConvertor(&punkCC);
	IColorConvertorPtr pcnv(punkCC);
	m_x = 0;
	m_y = 0;
	pimg->GetSize(&m_cx, &m_cy);
	m_pbytes = new byte[m_cx*m_cy];
	if( !m_pbytes )return;

	int	colors = ::GetImagePaneCount( pimg );
//	pcnv->GetColorPanesCount( &colors );

	color	**ppcolors = new color*[colors];

	int	i, c;

	byte	*pbyte = m_pbytes;

	for( i = 0; i < m_cy; i++ )
	{
		for( c = 0; c < colors; c++ )
		{
			pimg->GetRow( i, c, ppcolors+c );
		}
		pcnv->ConvertImageToDIB( ppcolors, pbyte, m_cx, false );
		pbyte+=m_cx;
	}

	delete []ppcolors;


	if (bContrast)
	Contrast();
//	Median();
//	Kirsch();
}

void CSewImage::SetImage(LPBITMAPINFOHEADER lpbi, SIZE sz, BOOL bContrast)
{
	m_x = m_y = 0;
	m_cx = sz.cx;
	m_cy = sz.cy;
	m_pbytes = new byte[m_cx*m_cy];
	byte *pbyteDst = m_pbytes;
	int nColors = lpbi->biClrUsed?lpbi->biClrUsed:lpbi->biBitCount==8?256:0;
	DWORD dwRowSrc = (lpbi->biBitCount/8*lpbi->biWidth+3)/4*4;
	byte *pDataSrc = (byte *)(((RGBQUAD *)(lpbi+1))+nColors);
	for(int y = 0; y < m_cy; y++ )
	{
		byte *pbyteSrc = pDataSrc+dwRowSrc*((m_cy-y-1)*lpbi->biHeight/sz.cy);
		for (int x = 0; x < m_cx; x++)
		{
			int xSrc = x*lpbi->biWidth/sz.cx;
			if (lpbi->biBitCount == 8)
				pbyteDst[x] = pbyteSrc[xSrc];
			else if (lpbi->biBitCount == 24)
				pbyteDst[x] = Bright(pbyteSrc[3*xSrc],pbyteSrc[3*xSrc+1],pbyteSrc[3*xSrc+2]);
		}
		pbyteDst+=m_cx;
	}

	if (bContrast)
	Contrast();
//	Median();
//	Kirsch();
}

void CSewImage::Median()
{
}

void CSewImage::Kirsch()
{
}

void CSewImage::Contrast()
{
	byte	*p;

	long	hist[256], i;
	ZeroMemory( &hist, sizeof( hist ) );

	for( i = 0, p = m_pbytes; i < m_cx*m_cy; i++, p++ )
		hist[*p]++;

	int	nMin = 0;
	int	nMax = 255;


	int	nMinPercent = m_cx*m_cy/100;
	int	nMaxPercent = m_cx*m_cy/100;

	while( nMinPercent >= 0 )
	{
		nMinPercent-=hist[nMin];
		nMin++;
	}

	while( nMaxPercent >= 0 )
	{
		nMaxPercent-=hist[nMax];
		nMax--;
	}

	int	a = max(nMax-nMin, 1 );
	int	b = nMin;

	for( i = 0, p = m_pbytes; i < m_cx*m_cy; i++, p++ )
		*p = max( 0, min( 255, (*p-b)*256/a ) );
}

#if defined(_TEST_PICTURES)
int g_nDebugWrite = 0;

void WriteDebug(const char *pszPathName, CSewImage &img)
{
	DWORD dwRow = (img.m_cx+3)/4*4;
	BITMAPFILEHEADER bfh = {0};
	bfh.bfType = ((int('M')<<8)|int('B'));
	bfh.bfSize = sizeof(bfh)+sizeof(BITMAPINFOHEADER)+1024+dwRow*img.m_cy;
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+1024;
	BITMAPINFOHEADER bi = {0};
	bi.biSize = sizeof(bi);
	bi.biHeight = img.m_cy;
	bi.biWidth = img.m_cx;
	bi.biBitCount = 8;
	bi.biPlanes = 1;
	bi.biClrUsed = bi.biClrImportant = 256;
	RGBQUAD rgb[256];
	for (int i = 0; i < 256; i++)
	{
		rgb[i].rgbRed = rgb[i].rgbGreen = rgb[i].rgbBlue = i;
		rgb[i].rgbReserved = 0;
	}
	int h = open(pszPathName, _O_BINARY|_O_CREAT|_O_RDWR);
	if (h != -1)
	{
		write(h, &bfh, sizeof(bfh));
		write(h, &bi, sizeof(bi));
		write(h, &rgb, sizeof(rgb));
		_ptr_t2<BYTE> row(dwRow);
		for (int i = 0; i < img.m_cy; i++)
		{
			memset((BYTE *)row, 0, dwRow);
			int y = img.m_cy-i-1;
			BYTE *p = img.m_pbytes+y*img.m_cx;
			memcpy(row, p, img.m_cx);
			write(h, p, dwRow);
		}
		close(h);
	}
}

void WriteDebug(CSewImage &imgBase, CSewImage &imgSew)
{
	if (g_nDebugWrite == 0) return;
	static int nPic = 0;
	char szBuff[256];
	sprintf(szBuff, "c:\\temp\\2\\%dbase.bmp", nPic);
	WriteDebug(szBuff, imgBase);
	sprintf(szBuff, "c:\\temp\\2\\%dsew.bmp", nPic);
	WriteDebug(szBuff, imgSew);
	nPic++;
	g_nDebugWrite--;
}
#endif


bool DoFindOptimalPos(CSewImage &siBase, CSewImage &siSew, int nMinPosX,
	int nMaxPosX, int nMinPosY, int nMaxPosY, long lStep, int nMinOvrPerc, 
	int	nCheckStep, POINT *pptRes, double *pdMinDiff)
{
	int	xOpt = 0;
	int	yOpt = 0;
	int	xTest, yTest, xMin, xMax, yMin, yMax;
	xMin = nMinPosX;
	xMax = nMaxPosX;
	yMin = nMinPosY;
	yMax = nMaxPosY;

	double	fMinRelDiff = 0;
	bool	bFirstTime = true;
//	int		cyMin = min( p1->cy, p2->cy );
//	int	nCheckStep = 4;

	DWORD dwStart = GetTickCount();
	while( nCheckStep != 0 )
	{
		for( yTest = yMin; yTest <= yMax; yTest+=nCheckStep )
		for( xTest = xMin; xTest <= xMax; xTest+=nCheckStep )
		{
			int	xs, xe, ys, ye; // x and y in base coordinates
			xs = max(xTest,0);
			xe = min(siBase.m_cx, xTest+siSew.m_cx);
			ys = max(0, yTest);
			ye = min(siBase.m_cy, yTest+siSew.m_cy);

			if ((xe-xs)*(ye-ys)*100/(siBase.m_cx*siBase.m_cy) < nMinOvrPerc)
				continue;

			int	x, y;
			int	nDiff = 0, t;
			int nCount = 0;
			byte	*pc1, *pc2;
//			int	xs1 = max( -xTest, 0 );
			

			for( y = ys; y < ye; y+=lStep )
			{
				pc1 = siBase.GetRow( xs, y );
				pc2 = siSew.GetRow( xs-xTest, y-yTest );

				for( x = xs; x < xe; x+=lStep, pc1+=lStep, pc2+=lStep )
				{
					t = (int)*pc2-(int)*pc1;
					if( t < 0 )t = -t;

					nDiff+=t;
					nCount++;
				}
			}

			if (GetTickCount()-dwStart > 10000)
				return false;

			double	fRelDiff = (double)nDiff/((double)nCount);

			if( bFirstTime || fRelDiff < fMinRelDiff )
			{
				fMinRelDiff = fRelDiff;
				xOpt = xTest;
				yOpt = yTest;
				bFirstTime = false;
			}
		}

		xMin = max(xOpt-nCheckStep+1,xMin);
		yMin = max(yOpt-nCheckStep+1,yMin);
		xMax = min( xOpt+nCheckStep-1, xMax );
		yMax = min( yOpt+nCheckStep-1, yMax );

		if( nCheckStep == 4 )
			nCheckStep = 1;
		else
			nCheckStep = 0;
	}

	pptRes->x = xOpt;
	pptRes->y = yOpt;
	*pdMinDiff = fMinRelDiff;
	return true;
}

class _CFindOptimalPos
{
public:
	DWORD dwStart,dwTime;
	BOOL bGlobalSearch;
	BOOL bContrast;
	int nMinOvrPerc,nSearchPerc/*,nCorrPerc*/,lStep,iZoom;
	int nCorrPoints;
	int	nCheckStep;
	CLoadFragment LoadPrevFrag;
	CLoadFragment LoadCurFrag;
	IImage3Ptr imgPrev;
	IUnknownPtr punkImgPrev;
	POINT ptPosPrev;
	SIZE szPrev;
	IImage3Ptr imgTest;
	_CFindOptimalPos()
	{
		dwStart = GetTickCount();
		dwTime = 0;
		nCheckStep = 4;
	}
	void ReadSettings()
	{
		bGlobalSearch = GetValueInt(GetAppUnknown(), "\\Sew Large Image\\FindOptimalPosition", "GlobalSearch", FALSE);
		nMinOvrPerc = GetValueInt(GetAppUnknown(), "\\Sew Large Image\\FindOptimalPosition", "MinOverlapPercent", 10);
		nSearchPerc = GetValueInt(GetAppUnknown(), "\\Sew Large Image\\FindOptimalPosition", "SearchPercent", 30);
//		nCorrPerc = GetValueInt(GetAppUnknown(), "\\Sew Large Image\\FindOptimalPosition", "CorrectPercent", 10);
		nCorrPoints = GetValueInt(GetAppUnknown(), "\\Sew Large Image\\FindOptimalPosition", "CorrectPoints", 10);
		lStep = ::GetValueInt( GetAppUnknown(), "\\Sew Large Image\\FindOptimalPosition", "ScanQuality", 5 );
		bContrast = ::GetValueInt( GetAppUnknown(), "\\Sew Large Image\\FindOptimalPosition", "Contrast", TRUE );
	}
	void PreparePrevFragment(ISewImageList *pFragList, ISewFragment *pFragPrev)
	{
		int nRangeCoef;
		pFragList->GetRangeCoef(&nRangeCoef);
		pFragPrev->GetOffset(&ptPosPrev);
		pFragPrev->GetSize(&szPrev);
		iZoom = 1;
		for (int i = 0; i < g_nFragmentImages; i++)
		{
			pFragPrev->GetImage(&iZoom, &punkImgPrev);
			if (punkImgPrev != 0)
				break;
			iZoom *= nRangeCoef;
		}
		if (punkImgPrev == 0)
		{
			iZoom = GetValueInt(GetAppUnknown(), "\\Sew Large Image\\FindOptimalPosition", "UsedZoom", 1);
			LoadPrevFrag.Load(pFragPrev, iZoom);
			pFragPrev->GetImage(&iZoom, &punkImgPrev);
		}
		imgPrev = punkImgPrev;
	}
	void PreparePrevFragmentZoom(ISewImageList *pFragList, ISewFragment *pFragPrev, double dZoomReq)
	{
		int nRangeCoef;
		pFragList->GetRangeCoef(&nRangeCoef);
		pFragPrev->GetOffset(&ptPosPrev);
		pFragPrev->GetSize(&szPrev);
		int iZoom1 = 1;
		IUnknownPtr punkImgPrev1;
		int nZoomFound=0,nZoomLoad=0;
		for (int i = 0; i < g_nFragmentImages; i++)
		{
			IUnknownPtr punk;
			pFragPrev->GetImage(&iZoom1, &punk);
			if (iZoom1 >= dZoomReq && nZoomLoad == 0)
				nZoomLoad = iZoom1;
			if (punk != 0 && punkImgPrev1 == 0)
			{
				punkImgPrev1 = punk;
				nZoomFound = iZoom1;
			}
			iZoom1 *= nRangeCoef;
		}
		if (punkImgPrev1 == 0)
		{
			iZoom = nZoomLoad != 0 ? nZoomLoad : iZoom1;
			LoadPrevFrag.Load(pFragPrev, iZoom);
			pFragPrev->GetImage(&iZoom, &punkImgPrev);
		}
		else
		{
			if (nZoomFound > nZoomLoad)
			{
				punkImgPrev = punkImgPrev1;
				iZoom = nZoomFound;
			}
			else
			{
				punkImgPrev = BuildZoomedImage(punkImgPrev1, nZoomLoad/nZoomFound);
				iZoom = nZoomLoad;
			}
		}
		imgPrev = punkImgPrev;
	}
	void PrepareCurFragment(ISewImageList *pFragList, ISewFragment *pFragCur)
	{
		int iZoom1 = iZoom;
		IUnknownPtr punkImgCur;
		pFragCur->GetImage(&iZoom1, &punkImgCur);
		if (punkImgCur == 0)
		{
			LoadCurFrag.Load(pFragCur, iZoom1);
			pFragCur->GetImage(&iZoom1, &punkImgCur);
		}
		imgTest = punkImgCur;
	}

	CSewImage siBase,siSew;
	int nMinX, nMaxX, nMinY, nMaxY, nMinOvrX, nMinOvrY;
	void InitScopeGlobal()
	{
		nMinOvrX = siBase.m_cx*nMinOvrPerc/100;
		nMinOvrY = siBase.m_cy*nMinOvrPerc/100;
		nMinX = -siSew.m_cx+nMinOvrX;
		nMaxX = siBase.m_cx-nMinOvrX;
		nMinY = -siSew.m_cy+nMinOvrY;
		nMaxY = siBase.m_cy-nMinOvrY;
	}
	void InitScope(POINT *pptPos)
	{
		InitScopeGlobal();
		if (!bGlobalSearch)
		{
			int nPrevX = (pptPos->x-ptPosPrev.x)/iZoom;
			int nPrevY = (pptPos->y-ptPosPrev.y)/iZoom;
			int nSearchX = siBase.m_cx*nSearchPerc/100;
			int nSearchY = siBase.m_cy*nSearchPerc/100;
			int nMinX1 = min(max(nMinX,nPrevX-nSearchX),nMaxX);
			int nMaxX1 = min(max(nMinX,nPrevX+nSearchX),nMaxX);
			int nMinY1 = min(max(nMinY,nPrevY-nSearchY),nMaxY);
			int nMaxY1 = min(max(nMinY,nPrevY+nSearchY),nMaxY);
			nMinX = nMinX1;
			nMaxX = nMaxX1;
			nMinY = nMinY1;
			nMaxY = nMaxY1;
		}
	}
	void InitScopeDynamic(POINT *pptPos)
	{
		InitScopeGlobal();
		int nPrevX = (pptPos->x-ptPosPrev.x)/iZoom;
		int nPrevY = (pptPos->y-ptPosPrev.y)/iZoom;
		int nSearchX = siBase.m_cx*nSearchPerc/100;
		int nSearchY = siBase.m_cy*nSearchPerc/100;
		int nMinX1 = min(max(nMinX,nPrevX-nSearchX),nMaxX);
		int nMaxX1 = min(max(nMinX,nPrevX+nSearchX),nMaxX);
		int nMinY1 = min(max(nMinY,nPrevY-nSearchY),nMaxY);
		int nMaxY1 = min(max(nMinY,nPrevY+nSearchY),nMaxY);
		if (nMaxX1 < 0)
			nMaxX1 = 0;
		else if (nMinX1 > 0)
			nMinX1 = 0;
		if (nMaxY1 < 0)
			nMaxY1 = 0;
		else if (nMinY1 > 0)
			nMinY1 = 0;
		nMinX = nMinX1;
		nMaxX = nMaxX1;
		nMinY = nMinY1;
		nMaxY = nMaxY1;
	}
	void InitScopeCorrect(POINT ptOrig, double dZoom)
	{
		InitScopeGlobal();
		int nOrigX = (ptOrig.x-ptPosPrev.x)/iZoom;
		int nOrigY = (ptOrig.y-ptPosPrev.y)/iZoom;
		int nCorrPoints1 = ((int)(nCorrPoints/dZoom))/iZoom;
		int nMinX1 = min(max(nMinX,nOrigX-nCorrPoints1),nMaxX);
		int nMaxX1 = min(max(nMinX,nOrigX+nCorrPoints1),nMaxX);
		int nMinY1 = min(max(nMinY,nOrigY-nCorrPoints1),nMaxY);
		int nMaxY1 = min(max(nMinY,nOrigY+nCorrPoints1),nMaxY);
		nMinX = nMinX1;
		nMaxX = nMaxX1;
		nMinY = nMinY1;
		nMaxY = nMaxY1;
		nCheckStep = 1;
	}
	void InitScopeByPoint(SIZE szCur, POINT *pptPos)
	{
		CPoint ptOrg;
		ptOrg.x = pptPos->x-(szCur.cx-szCur.cx*(pptPos->x-ptPosPrev.x)/szPrev.cx);
		ptOrg.y = pptPos->y-(szCur.cy-szCur.cy*(pptPos->y-ptPosPrev.y)/szPrev.cy);
		bGlobalSearch = false;
		InitScopeDynamic(&ptOrg);
	}
	void InitScopeByPoint(ISewFragment *pFragCur, POINT *pptPos)
	{
		SIZE szCur;
		pFragCur->GetSize(&szCur);
		CPoint ptOrg;
		ptOrg.x = pptPos->x-(szCur.cx-szCur.cx*(pptPos->x-ptPosPrev.x)/szPrev.cx);
		ptOrg.y = pptPos->y-(szCur.cy-szCur.cy*(pptPos->y-ptPosPrev.y)/szPrev.cy);
		bGlobalSearch = false;
		InitScopeDynamic(&ptOrg);
	}
	bool ProcessSearch(POINT *pptPos)
	{
		CPoint ptRes;
		double dMinDiff;
		bool bFound = false;
		if (nMinX < nMaxX && nMinY < nMaxY)
			bFound = DoFindOptimalPos(siBase,siSew,nMinX,nMaxX,nMinY,nMaxY,lStep,
				nMinOvrPerc,nCheckStep,&ptRes,&dMinDiff);
		// Save position
		if (bFound)
		{
			s_nFoundDiff = abs((int)dMinDiff);
			pptPos->x = ptPosPrev.x+ptRes.x*iZoom;
			pptPos->y = ptPosPrev.y+ptRes.y*iZoom;
			DWORD dwNow = GetTickCount();
			dwTime = dwNow-dwStart;
#if defined(_TEST_TIME)
			message<mc_DebugText>("FindOptimalPosition ->(%d,%d), diff %g, time %d ms\n",
				pptPos->x, pptPos->y, dMinDiff, dwTime);
#endif
			return true;
		}
		return false;
	}
};


bool FindOptimalPosition1(ISewImageList *pFragList, ISewFragment *pFragPrev,
	IImage3 *pimgTest, POINT *pptPos)
{
	_CFindOptimalPos _fop;
	_fop.ReadSettings();
	_fop.PreparePrevFragment(pFragList,pFragPrev);
	IImage3Ptr imgSewZ;
	if (_fop.iZoom == 1)
		imgSewZ = pimgTest;
	else
		imgSewZ = BuildZoomedImage(pimgTest, _fop.iZoom);
	if (_fop.imgPrev == 0 || _fop.imgTest == 0)
		return false;
	_fop.siBase.SetImage(_fop.imgPrev,_fop.bContrast);
	_fop.siSew.SetImage(imgSewZ,_fop.bContrast);
	_fop.InitScope(pptPos);
	return _fop.ProcessSearch(pptPos);
}

bool FindOptimalPositionByPoint(ISewImageList *pFragList, ISewFragment *pFragPrev,
	ISewFragment *pFragTest, POINT *pptPos)
{
	_CFindOptimalPos _fop;
	_fop.ReadSettings();
	_fop.PreparePrevFragment(pFragList,pFragPrev);
	_fop.PrepareCurFragment(pFragList,pFragTest);
	if (_fop.imgPrev == 0 || _fop.imgTest == 0)
		return false;
	_fop.siBase.SetImage(_fop.imgPrev,_fop.bContrast);
	_fop.siSew.SetImage(_fop.imgTest,_fop.bContrast);
	_fop.InitScopeByPoint(pFragTest,pptPos);
	return _fop.ProcessSearch(pptPos);
}

bool FindOptimalPositionCorrect(ISewImageList *pFragList, ISewFragment *pFragPrev,
	IImage3 *pimgTest, double dZoom, POINT *pptPos)
{
	_CFindOptimalPos _fop;
	_fop.ReadSettings();
	_fop.PreparePrevFragment(pFragList,pFragPrev);
	IImage3Ptr imgSewZ;
	if (_fop.iZoom == 1)
		imgSewZ = pimgTest;
	else
		imgSewZ = BuildZoomedImage(pimgTest, _fop.iZoom);
	_fop.siBase.SetImage(_fop.imgPrev,_fop.bContrast);
	_fop.siSew.SetImage(imgSewZ,_fop.bContrast);
	_fop.InitScopeCorrect(*pptPos,dZoom);
	return _fop.ProcessSearch(pptPos);
}

static int s_nCXLive = 640;
static bool PrepareFOPLive(ISewImageList *pFragList, ISewFragment *pFragPrev,
	LPBITMAPINFOHEADER lpbiLV, SIZE szLV, POINT *pptPos, _CFindOptimalPos &_fop)
{
	_fop.ReadSettings();
	CSize szLVReq(lpbiLV->biWidth,lpbiLV->biHeight);
	if (szLVReq.cx > s_nCXLive)
	{
		szLVReq.cx = s_nCXLive;
		szLVReq.cy = lpbiLV->biHeight*s_nCXLive/lpbiLV->biWidth;
	}
	_fop.PreparePrevFragmentZoom(pFragList,pFragPrev,max(double(szLV.cx)/double(szLVReq.cx),
		double(szLV.cy)/double(szLVReq.cy)));
	if (_fop.imgPrev == 0)
		return false;
	_fop.siBase.SetImage(_fop.imgPrev,_fop.bContrast);
	CSize szLVZ(szLV.cx/_fop.iZoom,szLV.cy/_fop.iZoom);
	_fop.siSew.SetImage(lpbiLV,szLVZ,_fop.bContrast);
#if defined(_TEST_PICTURES)
	WriteDebug(_fop.siBase, _fop.siSew);
#endif
	return true;
}

static bool FOPProcessSearch(_CFindOptimalPos &_fop, POINT *pptPos)
{
	bool b = _fop.ProcessSearch(pptPos);
	static DWORD s_dwReduceTime = 0;
	if (_fop.dwTime > 1000)
	{
		s_nCXLive /= 2;
		s_dwReduceTime = GetTickCount();
	}
	else if (_fop.dwTime < 20 && GetTickCount() - s_dwReduceTime >= 10000)
		s_nCXLive *= 2;
	return b;
}

bool FindOptimalPositionLive(ISewImageList *pFragList, ISewFragment *pFragPrev,
	LPBITMAPINFOHEADER lpbiLV, SIZE szLV, POINT *pptPos)
{
	_CFindOptimalPos _fop;
	if (!PrepareFOPLive(pFragList,pFragPrev,lpbiLV,szLV,pptPos,_fop))
		return false;
	_fop.InitScopeDynamic(pptPos);
	return FOPProcessSearch(_fop,pptPos);
}

bool FindOptimalPositionCorrectLive(ISewImageList *pFragList, ISewFragment *pFragPrev,
	LPBITMAPINFOHEADER lpbiLV, SIZE szLV, double dZoom, POINT *pptPos)
{
	_CFindOptimalPos _fop;
	if (!PrepareFOPLive(pFragList,pFragPrev,lpbiLV,szLV,pptPos,_fop))
		return false;
	_fop.InitScopeCorrect(*pptPos,dZoom);
	return _fop.ProcessSearch(pptPos);
}

int CalcDifferenceLive(ISewImageList *pFragList, ISewFragment *pFragPrev,
	LPBITMAPINFOHEADER lpbiLV, SIZE szLV, POINT ptPos)
{
	_CFindOptimalPos _fop;
	if (!PrepareFOPLive(pFragList,pFragPrev,lpbiLV,szLV,&ptPos,_fop))
		return false;
	_fop.nMinX = (ptPos.x-_fop.ptPosPrev.x)/_fop.iZoom;
	_fop.nMinY = (ptPos.y-_fop.ptPosPrev.y)/_fop.iZoom;
	_fop.nMaxX = _fop.nMinX+1;
	_fop.nMaxY = _fop.nMinY+1;
	if (!_fop.ProcessSearch(&ptPos))
		return -1;
	return s_nFoundDiff;
}
