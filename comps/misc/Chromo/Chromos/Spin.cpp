#include "StdAfx.h"
#include "Cromos.h"
#include "PUtils.h"
#include "ChrUtils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static int sin1 = 0;
static int cos1 = 0;
static int sin2 = 0;
static int cos2 = 0;
static int nll = 0;
static int nl1 = 0;
static int nkl = 0;
static int nk1 = 0;
static int turnline = 0;
static int turnkol = 0;
static int fractline = 0;
static int fractkol = 0;
static int lineorigin = 0;
static int kolorigin = 0;


static double xscale = 1.;
static double yscale = 1.;
static double s,c;

#define SMALL int
#define COORD1 int

/* standard workspace structure for interval objects */

static void turn(int l, int k)
{
	int l256,k256;
	l -= lineorigin;
	k -= kolorigin;
	l256 = l*cos1 + k*sin1;
	k256 = l*cos2 + k*sin2;
	turnline = l256 / 256;
	turnkol = k256 / 256;
	fractline = l256 - 256*turnline;
	if (fractline < 0)
	{
		turnline--;
		fractline += 256;
	}
	fractkol = k256 - 256*turnkol;
	if (fractkol < 0)
	{
		turnkol--;
		fractkol += 256;
	}
	turnline += lineorigin;
	turnkol += kolorigin;
}


static void limits(int l,  int k)
{
	turn(l,k);
	if (nl1 > turnline)
		nl1 = turnline;
	if (nll < turnline)
		nll = turnline;
	if (nk1 > turnkol)
		nk1 = turnkol;
	if (nkl < turnkol)
		nkl = turnkol;
}

static void stepkl()
{
	fractline += sin1;
	while (fractline >= 256)
	{
		fractline -= 256;
		turnline++;
	}
	while (fractline < 0)
	{
		fractline += 256;
		turnline--;
	}
	fractkol += sin2;
	while (fractkol >= 256)
	{
		fractkol -= 256;
		turnkol++;
	}
	while (fractkol < 0)
	{
		fractkol += 256;
		turnkol--;
	}
}

void rottrans(CImageWrp &pImgWrp, CIntervalImage *pIntImg, double radians, CImageWrp &pImgWrp1, CIntervalImage *&pIntImg1,
	POINT *pptObjCenter, bool bBlackOnWhite, bool bMakeGray)
{
	// Three coordinate systems used in this function:
	// 1. Document c.s (DCS).
	// 2. Rotated DCS (RDCS).
	// 3. Shifted RDCS (SRDCS).
	int l,i,j;
	color g4[4];
	byte mask4[4];
	s = 256.0 * sin(radians);
	c = 256.0 * cos(radians);
	sin1 = (int) (xscale*s);
	cos1 = (int) (yscale*c);
	sin2 = (int) (xscale*c);
	cos2 = - (int) (yscale*s);
	// find line limits of rotated object
	CPoint ptOffs = pIntImg->ptOffset;
	int nHeight = pIntImg->szInt.cy;
	int nWidth  = pIntImg->szInt.cx;
	if (nHeight == 0 || nWidth == 0)
		return;
	pptObjCenter->y = lineorigin = ptOffs.y+(nHeight-1)/2; // DCS
	pptObjCenter->x = kolorigin = ptOffs.x+(nWidth-1)/2;
	turn(lineorigin,kolorigin);
	nll = turnline; // RDCS
	nl1 = nll;
	nkl = turnkol;
	nk1 = nkl;
	for (i = 0; i < pIntImg->nrows; i++)
	{
		if (pIntImg->prows[i].nints > 0)
		{
			limits(i+ptOffs.y, pIntImg->prows[i].pints[0].left+ptOffs.x);
			limits(i+ptOffs.y, pIntImg->prows[i].pints[pIntImg->prows[i].nints-1].right+ptOffs.x);
		}
	}
	nl1--; // RDCS
	nll++;
	nk1--;
	nkl++;
	pIntImg1 = new CIntervalImage;
	pIntImg1->nrows = nll-nl1+1;
	pIntImg1->prows = new INTERVALROW[pIntImg1->nrows];
	memset(pIntImg1->prows, 0, (pIntImg1->nrows)*sizeof(INTERVALROW));
	for (l = nl1; l <= nll; l++)
	{
		pIntImg1->prows[l-nl1].nints = 1;
		pIntImg1->prows[l-nl1].pints = new INTERVAL;
		pIntImg1->prows[l-nl1].pints[0].left  = nkl;
		pIntImg1->prows[l-nl1].pints[0].right = nk1;
	}
	//	find column bounds for each line in rotated object.
	//	when considering effect of any given line in input
	//	picture, extend to adjacent lines to ensure enough
	//	space at edges.
	for (l = 0; l < pIntImg->nrows; l++)
	{
		for (int k = 0; k < pIntImg->prows[l].nints; k++)
		{
			turn(l+ptOffs.y, pIntImg->prows[l].pints[k].left+ptOffs.x);
			int n = pIntImg->prows[l].pints[k].right-pIntImg->prows[l].pints[k].left+1;
			for (j = 0; j < n; j++)
			{
				int ll = turnline - nl1;
				for (i = 0; i < 2; i++)
				{
					if (turnkol < pIntImg1->prows[ll+i].pints[0].left)
						pIntImg1->prows[ll+i].pints[0].left = turnkol; // in RDCS
					if (turnkol+1 > pIntImg1->prows[ll+i].pints[0].right)
						pIntImg1->prows[ll+i].pints[0].right = turnkol+1;
				}
				stepkl();
			}
		}
	}
	// correct interval list by finding minimum column of
	// entire object, and subtract this from all endpoints.
	// Also clear those lines with empty interval.
	nk1 = pIntImg1->prows[0].pints[0].left;
	nkl = pIntImg1->prows[0].pints[0].right;
	for (l = nl1; l <= nll; l++)
	{
		if (pIntImg1->prows[l-nl1].pints[0].left < nk1)
			nk1 = pIntImg1->prows[l-nl1].pints[0].left;
		if (pIntImg1->prows[l-nl1].pints[0].right > nkl)
			nkl = pIntImg1->prows[l-nl1].pints[0].right;
	}
	for (i = 0; i < pIntImg1->nrows; i++)
	{
		if (pIntImg1->prows[i].nints == 0)
			continue;
		if (pIntImg1->prows[i].pints[0].left > pIntImg1->prows[i].pints[0].right)
		{
			delete pIntImg1->prows[i].pints;
			pIntImg1->prows[i].pints = NULL;
			pIntImg1->prows[i].nints = 0;
		}
		else
		{
			pIntImg1->prows[i].pints[0].left -= nk1;
			pIntImg1->prows[i].pints[0].right -= nk1;
		}
	}
	pIntImg1->ptOffset.x = nk1; // Now image offset in RDCS, coords in intervals in SRDCS
	pIntImg1->ptOffset.y = nl1;
	// scan new object, filling in grey table
	// BEWARE - coordinates are scaled up by 256,
	// interpolated grey values by 65536.  This requires
	// 32-bit integers to work for most objects.
	//
	// Invert the rotation.  Take care with the scaling.
	sin1 = - (int) (s/yscale);
	cos1 = (int) (c/yscale);
	sin2 = (int) (c/xscale);
	cos2 = (int) (s/xscale);
	CString sCC = bMakeGray?_T("Gray"):pImgWrp.GetColorConvertor();
	if (sCC.IsEmpty()) return;
	int nPanesNum = bMakeGray?-1:pImgWrp.GetColorsCount();
	pImgWrp1.CreateNew(nkl-nk1+1,nll-nl1+1,sCC,nPanesNum);
	pImgWrp1.SetOffset(pIntImg1->ptOffset);
	pImgWrp1.InitRowMasks();
	for (int y = 0; y < nll-nl1+1; y++)
	{
		color *pDst = pImgWrp1.GetRow(y, 0);
		memset(pDst, 0, (nkl-nk1+1)*sizeof(color));
		byte *pDstMask = pImgWrp1.GetRowMask(y);
		memset(pDstMask, 0, (nkl-nk1+1));
	}
	_CGrayVal gval(pImgWrp,pIntImg);
	color nBackgnd = bBlackOnWhite?0xFF:0;
	gval.clrDefault = bBlackOnWhite?0xFFFF:0;
	if (bMakeGray)
	{
		for (i = 0; i < nll-nl1+1; i++)
		{
			color *pDst = pImgWrp1.GetRow(i, 0);
			byte *pDstMask = pImgWrp1.GetRowMask(i);
			for (int k = 0; k < pIntImg1->prows[i].nints; k++)
			{
				turn(i+nl1, pIntImg1->prows[i].pints[k].left+nk1); // RDCS
				int n = pIntImg1->prows[i].pints[k].right-pIntImg1->prows[i].pints[k].left+1;
				int x0 = pIntImg1->prows[i].pints[k].left;
				for (j = 0; j < n; j++)
				{
					gval.Gray4Val(turnline,turnkol,g4); // SRDCS
					color g = interp(g4,fractline,fractkol);
					pDst[x0+j] = g<<8;
//					if (pDst[x0+j] > 0)
					if (g != nBackgnd)
						pDstMask[x0+j] = 0xFF;
					stepkl();
				}
			}
		}
	}
	else // not gray
	{
		int nColors = pImgWrp1.GetColorsCount();
		for (i = 0; i < nll-nl1+1; i++)
		{
			byte *pDstMask = pImgWrp1.GetRowMask(i);
			for (int c = 0; c < nColors; c++)
			{
				color *pDst = pImgWrp1.GetRow(i, c);
				for (int k = 0; k < pIntImg1->prows[i].nints; k++)
				{
					turn(i+nl1, pIntImg1->prows[i].pints[k].left+nk1); // RDCS
					int n = pIntImg1->prows[i].pints[k].right-pIntImg1->prows[i].pints[k].left+1;
					int x0 = pIntImg1->prows[i].pints[k].left;
					for (j = 0; j < n; j++)
					{
						gval.Pane4Val(turnline,turnkol,c,g4,mask4); // SRDCS
						pDst[x0+j] = interpmask(g4,mask4,&pDstMask[x0+j],fractline,fractkol)<<8;
						stepkl();
					}
				}
			}
		}
	}
	pIntImg1->MakeByImage(pImgWrp1);
}




