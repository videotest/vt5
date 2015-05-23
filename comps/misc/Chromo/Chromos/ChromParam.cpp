#include "stdafx.h"
#include "Cromos.h"
#include "ChromParam.h"
#include "PUtils.h"
#include "ChromoSubs.h"
#include "Settings.h"
 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CChromParam::CChromParam()
{
	bOk = false;
	pCntr = NULL;
	pIntvl = NULL;
	m_ptOffs = m_ptCenter = CPoint(0,0);
	bRotated = false;
	dAngle = dLen = 0.;
	nPtsDist = nPoints = nCmerePos = 0;
	ptCentromere = INVPOINT;
	bPolarity = false;
	dArea = dDensity = dLenght = dPerim = cvdd = nssd = 0.;
	wdd1 = wdd2 = wdd2p = wdd3 = wdd4 = wdd5 = wdd6 = 0.;
	gwdd1 = gwdd2 = gwdd2p = gwdd3 = gwdd4 = gwdd5 = gwdd6 = 0.;
	mwdd1 = mwdd2 = mwdd2p = mwdd3 = mwdd4 = mwdd5 = mwdd6 = 0.;
	dLenghtCI = dAreaCI = dDensCI = mdra = dNBIndex = 0.;
	nBands = 0;

}

CChromParam::~CChromParam()
{
	delete pIntvl;
};

void CChromParam::CalcParams1()
{
	// Calculate area and density
	int cx = pImgWrp.GetWidth();
	int cy = pImgWrp.GetHeight();
	CString s = pImgWrp.GetColorConvertor();
	dArea = 0;
	dDensity = 0;
	if (s.CompareNoCase(_T("Gray"))==0||s.CompareNoCase(_T("CGH"))==0||s.CompareNoCase(_T("FISH"))==0)
	{
		int nPane = s.CompareNoCase(_T("Gray"))==0?0:g_CGHPane;
		for (int y = 0; y < cy; y++)
		{
			color *pData = pImgWrp.GetRow(y,nPane);
			BYTE  *pMask = pImgWrp.GetRowMask(y);
			for (int x = 0; x <cx; x++)
			{
				if (pMask[x])
				{
					dArea++;
					dDensity += double(pData[x]);
				}
			}
		}
	}
	else if (s.CompareNoCase(_T("RGB"))==0)
	{
		for (int y = 0; y < cy; y++)
		{
			color *red = pImgWrp.GetRow(y,0);
			color *green = pImgWrp.GetRow(y,1);
			color *blue = pImgWrp.GetRow(y,2);
			BYTE  *pMask = pImgWrp.GetRowMask(y);
			for (int x = 0; x <cx; x++)
			{
				if (pMask[x])
				{
					dArea++;
					color bright = Bright(blue[x], green[x], red[x]);
					dDensity += double(bright);
				}
			}
		}
	}
	else
	{
		for (int y = 0; y < cy; y++)
		{
			color *pData = pImgWrp.GetRow(y,0);
			BYTE  *pMask = pImgWrp.GetRowMask(y);
			for (int x = 0; x <cx; x++)
			{
				if (pMask[x])
				{
					dArea++;
					dDensity += double(pData[x]);
				}
			}
		}
	}
	dDensity /= 256.;
//	dDensity /= dArea;
	// Calculate perimeter
	if (g_ShortAxisAlg == 1)
	{
		CChromoPerimeter *pPerim = new CChromoPerimeter;
		pPerim->MakeByInterval(pIntvl);
		dPerim = pPerim->CalcPerim();
		delete pPerim;
	}
	else
	{
		CPoint ptOffs(pImgWrp.GetOffset());
		int yMin = pCntr->ppoints[0].y, yMax = pCntr->ppoints[0].y;
		for (int i = 1; i < pCntr->nContourSize; i++)
		{
			ContourPoint pt = pCntr->ppoints[i];
			if (pt.y < yMin) yMin = pt.y;
			if (pt.y > yMax) yMax = pt.y;
		}
		int yDiap = yMax-yMin+1;
		int *pnLt = new int[yDiap];
		memset(pnLt, 0xFF, yDiap*sizeof(int));
		int *pnRt = new int[yDiap];
		memset(pnRt, 0xFF, yDiap*sizeof(int));
		for (i = 0; i < pCntr->nContourSize; i++)
		{
			ContourPoint pt = pCntr->ppoints[i];
			int y = pt.y-yMin;
			if (pnLt[y] == -1 || pt.x < pnLt[y])
				pnLt[y] = pt.x;
			if (pnRt[y] == -1 || pt.x > pnRt[y])
				pnRt[y] = pt.x;
		}
		CPoint *ppt = new CPoint[2*yDiap+1];
		ppt[0].x = pnLt[0];
		ppt[1].x = pnRt[0];
		ppt[0].y = ppt[1].y = yMin;
		int n = 2;
		for (i = 1; i < yDiap; i++)
		{
			if (pnRt[i] == -1) continue;
			ppt[n].y = yMin+i;
			ppt[n].x = pnRt[i];
			while (n >= 3 && (ppt[n-1].y-ppt[n-2].y)*(ppt[n].x-ppt[n-2].x)>=(ppt[n-1].x-ppt[n-2].x)*(ppt[n].y-ppt[n-2].y))
				ppt[n-1] = ppt[n--];
			n++;
		}
		for (i = yDiap-1; i >= 0; i--)
		{
			if (pnLt[i] == -1) continue;
			ASSERT(n < 2*yDiap+1);
			ppt[n].y = yMin+i;
			ppt[n].x = pnLt[i];
			while (n >= 3 && (ppt[n-1].y-ppt[n-2].y)*(ppt[n].x-ppt[n-2].x)>=(ppt[n-1].x-ppt[n-2].x)*(ppt[n].y-ppt[n-2].y))
				ppt[n-1] = ppt[n--];
			n++;
		}
		dPerim = 0;
		for (i = 1; i < n ; i++)
			dPerim += (int)(8*sqrt(double((ppt[i].x-ppt[i-1].x)*(ppt[i].x-ppt[i-1].x)+(ppt[i].y-ppt[i-1].y)*(ppt[i].y-ppt[i-1].y)))+.5);
		delete ppt;
		delete pnLt;
		delete pnRt;
	}
}

void CChromParam::CalcParams2()
{
	// Calculate lenght
	dLenght = m_LongAxis.m_nSize;
	cvdd = _cvdd(m_Prof0.m_pnProf, m_Prof0.m_nSize);
	nssd = _nssd(m_Prof0.m_pnProf, m_Prof0.m_nSize);
}

void CChromParam::CalcParams3()
{
}

void CChromParam::CalcParams4()
{
	// Calc centromere features
	int cindexa,cindexm,cindexl;
	int yCen = ptCentromere.y-pIntvl->ptOffset.y;
	cindexa = _cindexa(pIntvl,yCen);
	cindexm = _cindexm(pImgWrp,pIntvl,yCen);
	cindexl = _cindexl(m_LongAxis.m_iptAxis,m_LongAxis.m_nSize,ptCentromere.y);
	double dCoef = g_FloatArith==1?100.:1.;
	dAreaCI = cindexa/dCoef;
	dDensCI = cindexm/dCoef;
	dLenghtCI = cindexl/dCoef;
	mdra = double(cindexa > 50 ? 100*(100-cindexm)/(101-cindexa) : 100*cindexm/(1+cindexa))/dCoef;
	// Calc WDD features
	int npointsg;
	int *profg = gradprof(m_Prof0.m_pnProf, m_Prof0.m_nSize, &npointsg);
	short wdd[6],mwdd[6],gwdd[6];
	_wdd(m_Prof0.m_pnProf, m_Prof0.m_nSize, wdd);
	_wdd(m_Prof2.m_pnProf, m_Prof2.m_nSize, mwdd);
	_wdd(profg, npointsg, gwdd);
	delete profg;
	if (g_FloatArith==1&&dAreaCI>0.5||g_FloatArith!=1&&dAreaCI>50)
	{
		for (int i=0; i<=4; i += 2)
		{
			wdd[i] = - wdd[i];
			mwdd[i] = - mwdd[i];
			gwdd[i] = - gwdd[i];
		}
	}
	dCoef = g_FloatArith==1?256.:1.;
	wdd1 = wdd[0]/dCoef;
	wdd2 = wdd[1]/dCoef;
	wdd3 = wdd[2]/dCoef;
	wdd4 = wdd[3]/dCoef;
	wdd5 = wdd[4]/dCoef;
	wdd6 = wdd[5]/dCoef;
	mwdd1 = mwdd[0]/dCoef;
	mwdd2 = mwdd[1]/dCoef;
	mwdd3 = mwdd[2]/dCoef;
	mwdd4 = mwdd[3]/dCoef;
	mwdd5 = mwdd[4]/dCoef;
	mwdd6 = mwdd[5]/dCoef;
	gwdd1 = gwdd[0]/dCoef;
	gwdd2 = gwdd[1]/dCoef;
	gwdd3 = gwdd[2]/dCoef;
	gwdd4 = gwdd[3]/dCoef;
	gwdd5 = gwdd[4]/dCoef;
	gwdd6 = gwdd[5]/dCoef;
}

void CChromParam::GetParamVector(int *pn)
{
	pn[0] = (int)dArea;
	pn[1] = (int)dLenght;
	pn[2] = (int)(dDensity/dArea);
	// Centromeric index by area, ratio of mass c.i. to area c.i.
	pn[3] = (int)dAreaCI;
	pn[4] = (int)dDensCI;
	pn[26] = (int)dLenghtCI;
	pn[27] = (int)dPerim;
	// Modify c.i. appropriately if object upside-down
	if (pn[3] > 50)
	{
		pn[3] = 100 - pn[3];
		pn[4] = 100 - pn[4];
		pn[26] = 100 - pn[26];
	}
	// Convert v[4] to ratio - watch for division by zero 
	if (pn[3] < 1)
	{
		pn[3] = 1;
		pn[4] = 1;
	}
	pn[4] = 100 * pn[4] / pn[3];
	// cvdd, nsdd, ddm1, mdra
	pn[5] = cvdd;
	pn[6] = nssd;
	pn[7] = mdra;
	// wdd of density profile
	// wdd of 2nd moment of density profile
	// wdd of gradient of density profile
	pn[8] = wdd1;
	pn[9] = wdd2;
	pn[10] = wdd3;
	pn[11] = wdd4;
	pn[12] = wdd5;
	pn[13] = wdd6;
	pn[14] = mwdd1;
	pn[15] = mwdd2;
	pn[16] = mwdd3;
	pn[17] = mwdd4;
	pn[18] = mwdd5;
	pn[19] = mwdd6;
	pn[20] = gwdd1;
	pn[21] = gwdd2;
	pn[22] = gwdd3;
	pn[23] = gwdd4;
	pn[24] = gwdd5;
	pn[25] = gwdd6;
}


void dprintf(char * szFormat, ...);

bool CChromParam::Init(IMeasureObject *punkMO, IChromosome *pChromos, IUnknown *punkImageObj)
{
	// Get image and check its validity
	if (punkImageObj == NULL)
	{
		IUnknownPtr punkImage;
		punkMO->GetImage(&punkImage);
		if (punkImage == 0)
		{
			dprintf("No image\n");
			return false;
		}
		pImgWrp = punkImage;
	}
	else
		pImgWrp = CImageWrp(punkImageObj);
	if (pImgWrp.IsEmpty() || pImgWrp.GetHeight() < 6 || pImgWrp.GetRow(0, 0) == NULL)
		return false; //Image empty or too small and can couse the problems, or color convrtor not found
	pImgWrpOrig = pImgWrp;
	// Get contours
	if (g_ShortAxisAlg == 0)
	{
		pCntr = max_contour(pImgWrp);
		int nCountSize = pCntr?pCntr->nContourSize:0;
		if (nCountSize < CHROMOSOM_PERIMETR_BOUND)
		{
			if (pCntr)
				dprintf("Empty perimeter (%d)\n", nCountSize);
			else
				dprintf("No contour at all\n");
			return false;
		}
	}
	else
		pCntr = NULL;
	// Calculate intervaled image
	pIntvl = new CIntervalImage;
	pIntvl->MakeByImage(pImgWrp);
	m_ptOffs = pImgWrp.GetOffset();
	// Save chromosome object for calculation
	m_sptrChromos = pChromos;
	return true;
}



