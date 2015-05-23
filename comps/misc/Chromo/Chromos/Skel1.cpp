#include "StdAfx.h"
#include "ChrUtils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static unsigned long lut[48] =
{
	000075400340, 000000200000, 000000200000, 000000000000, 
	000074600362, 000074600363, 000000000000, 000000000000, 
	036475372364, 000000000000, 000175200765, 000000000000, 
	036074170360, 036074170360, 000074000360, 000074000360, 
	000000200000, 000000200001, 000000200001, 000000200001, 
	000074600363, 000074600363, 000074600363, 000074600363, 
	000175200765, 000175200765, 000175200765, 000175200765, 
	000303601417, 000303601417, 000303601417, 000303601417, 
	000000000000, 000000200001, 000000200001, 000000000000, 
	000074600363, 000074600363, 000000000000, 000000000000, 
	000175200765, 000000000000, 000175200765, 000000000000, 
	000000000000, 000000000000, 000000000000, 000000000000
};

struct CSkelLine
{
	INTERVAL *intp;
	int intcount;
	int ijntpos;
	int ijleft;
	int ijright;
	int prevright;
};

// TO DO : realize check for array bounds.

int intcount(CIntervalImage *pDel)
{
	int n = 0;
	for (int i = 0; i < pDel->nrows; i++)
	{
		if (pDel->prows)
		{
			n += pDel->prows[i].nints;
		}
	}
	return n;
}

void strip(CIntervalImage *pSkel, CIntervalImage *pDel, int *pDelArea, int nSmooth)
{
	struct CSkelLine dm1lint, lm1lint, lp1lint;
	int nSmoothBits = nSmooth>=0?16:0;
	*pDelArea = 0; //*delarea = 0;
	int skl1 = pSkel->ptOffset.y; //skl1 = skeldom->line1;
	int skll = skl1+pSkel->szInt.cy-1; //skll = skeldom->lastln;
	int pdl1 = skl1;
	pDel->Free();
	int l = skll-skl1+1;
	pDel->nrows = l;
	pDel->prows = new INTERVALROW[l];
	memset(pDel->prows, 0, l*sizeof(INTERVALROW));
	int nNewInts = 0;
	CArray<INTERVAL,INTERVAL&> arrIntvls;
	arrIntvls.SetSize(8*intcount(pSkel));
	INTERVAL *jntp = arrIntvls.GetData();
	for (int nRow = 0; nRow < pSkel->nrows; nRow++)
	{
		if (nRow > 0)
		{
			INTERVALROW *pPrevRow = &pSkel->prows[nRow-1];
			lm1lint.intcount = pPrevRow->nints;
			if (lm1lint.intcount > 0)
			{
				lm1lint.intp = pPrevRow->pints;
				lm1lint.ijleft = lm1lint.intp->left;
				lm1lint.ijright = lm1lint.prevright = lm1lint.intp->right;
			}
		}
		else
			lm1lint.intcount = 0;
		if (nRow+pSkel->ptOffset.y > pdl1)
		{
			INTERVALROW *pPrevRow = &pDel->prows[nRow-1];
			dm1lint.intcount = pPrevRow->nints;
			if (dm1lint.intcount > 0)
			{
				dm1lint.intp = pPrevRow->pints;
				dm1lint.ijleft = dm1lint.intp->left;
				dm1lint.prevright = dm1lint.ijright = dm1lint.intp->right;
			}
			else
				dm1lint.intcount = 0;
		}
		if (nRow < pSkel->nrows-1)
		{
			INTERVALROW *pPrevRow = &pSkel->prows[nRow+1];
			lp1lint.intcount = pPrevRow->nints;
			if (lp1lint.intcount > 0)
			{
				lp1lint.intp = pPrevRow->pints;
				lp1lint.ijleft = lp1lint.intp->left;
				lp1lint.prevright = lp1lint.ijright = lp1lint.intp->right;
			}
		}
		else
			lp1lint.intcount = 0;
		INTERVAL *intp = jntp;
		nNewInts = 0;
		for (int iInt = 0; iInt < pSkel->prows[nRow].nints; iInt++)
		{
			int lastrem = 0;
			int delflipflop = 0;
			int k1 = pSkel->prows[nRow].pints[iInt].left;
			int kl = pSkel->prows[nRow].pints[iInt].right;
			for (int k = k1; k <= kl; k++)
			{
				int w = 16;
				if (lm1lint.intcount <= 0 || k < lm1lint.ijleft)
					w = 0;
				else
				{
					while (k > lm1lint.ijright)
					{
						if (--lm1lint.intcount <= 0)
						{
							lm1lint.intcount = -1;
							w = 0; 
							break;
						}
						else
						{
							lm1lint.prevright = lm1lint.ijright;
							lm1lint.ijleft = (++lm1lint.intp)->left; 
							lm1lint.ijright = lm1lint.intp->right; 
							if (k < lm1lint.ijleft)
							{ 
								w = 0;
								break;
							} 
						}
					}
				}
				if (k > k1)
					w += 8;
				if (k < kl)
					w += 4;
				int b = 16;
				if (lp1lint.intcount <= 0 || k < lp1lint.ijleft)
					b = 0;
				else
				{
					while (k > lp1lint.ijright)
					{
						if (--lp1lint.intcount <= 0)
						{
							lp1lint.intcount = -1;
							b = 0;
							break;
						}
						else
						{ 
							lp1lint.prevright = lp1lint.ijright; 
							lp1lint.ijleft = (++lp1lint.intp)->left; 
							lp1lint.ijright = lp1lint.intp->right; 
							if (k < lp1lint.ijleft)
							{ 
								b = 0; 
								break;
							} 
						} 
					}
				}
				if (b > 0)
				{	/* (actually b==16) */
					/* not a 4-connected boundary point ? */
					if (w == 28)
					{		/* RETAIN POINT */
						lastrem = 0;	/* for next point */
						if (delflipflop == 0)
						{
							nNewInts++;
							if (nNewInts > arrIntvls.GetSize())
								arrIntvls.SetSize(nNewInts+10);
							delflipflop = 1;
							intp->left = k;
						}
						/* skip over further interior points */
						k = min((kl-1),lm1lint.ijright);
						k = min(k,lp1lint.ijright);
						intp->right = k;
						continue;	/* WITH LOOP */
					}
					b = 4;
				}
				/*
				 * If neighbour 2 present, see if it still present
				 * in thinned object "delobj"
				 */
				if (w >= 16)
				{
					int rtp = 16;
					if (dm1lint.intcount <= 0 || k < dm1lint.ijleft)
						rtp = 0;
					else
					{
						while (k > dm1lint.ijright)
						{
							if (--dm1lint.intcount <= 0)
							{ 
								dm1lint.intcount = -1;
								rtp = 0; 
								break; 
							} 
							else
							{ 
								dm1lint.prevright = dm1lint.ijright; 
								dm1lint.ijleft = (++dm1lint.intp)->left; 
								dm1lint.ijright = dm1lint.intp->right; 
								if (k < dm1lint.ijleft)
								{ 
									rtp = 0; 
									break; 
								} 
							} 
						} 
					}
					w += (16-rtp);
				}
				/* here "k" refers to the middle position in the run of three */
				/* also, interwal stepping has already been performed by POINTS1 */
				/* first check if any interwals in line */ 
				if (lm1lint.intcount != 0)
				{ 
					/* past end of last interwal in line ? */ 
					if (lm1lint.intcount < 0)
					{ 
						if (k-1 == lm1lint.ijright) 
							w += 2; 
						/* otherwise not yet past last interwal, check cases */ 
					}
					else
					{ 
						if (k > lm1lint.ijleft || k-1 == lm1lint.prevright) 
							w += 2; 
						if (k < lm1lint.ijright && k+1 >= lm1lint.ijleft) 
							w++; 
					} 
				} 
				w = lut[w];
				if (w != 0)
				{
					/* here "k" refers to the middle position in the run of three */
					/* also, interbal stepping has already been performed by POINTS1 */
					/* first check if any interbals in line */ 
					if (lp1lint.intcount != 0)
					{ 
						/* past end of last interbal in line ? */ 
						if (lp1lint.intcount < 0)
						{ 
							if (k-1 == lp1lint.ijright) 
								b += 2; 
							/* otherbise not yet past last interbal, check cases */ 
						}
						else
						{ 
							if (k > lp1lint.ijleft || k-1 == lp1lint.prevright) 
								b += 2; 
							if (k < lp1lint.ijright && k+1 >= lp1lint.ijleft) 
								b++; 
						} 
					} 
					b += (nSmoothBits + lastrem);
					w &= (1 << b);
				}

				if (w != 0)
				{ /* REMOVE POINT */
					(*pDelArea)++;
					lastrem = 8;	/* for next point */
					if (delflipflop == 1)
					{
						delflipflop = 0;
						intp++;
					}
				}
				else
				{			/* RETAIN POINT */
					lastrem = 0;	/* for next point */
					if (delflipflop == 0)
					{
						nNewInts++;
						delflipflop = 1;
						intp->left = k;
					}
					intp->right = k;
				}
			}
			/*
			 * clear up an open interval end
			 */
			if (delflipflop == 1)
				intp++;
		}
		pDel->prows[nRow].nints = nNewInts;
		if (nNewInts > 0)
		{
			pDel->prows[nRow].pints = new INTERVAL[nNewInts];
			memcpy(pDel->prows[nRow].pints, jntp, nNewInts*sizeof(INTERVAL));
		}
		jntp = intp;
	}
}

CIntervalImage *skeleton(CIntervalImage *pSrc, int nSmoothPasses)
{
	int nDelArea;
	/* first check input object */
	/*
	 * copy input object, and make plenty of space for intermediate
	 * intervals in the deleted-this-pass object
	 */
	CIntervalImage *pDel = new CIntervalImage;
	pDel->ptOffset = pSrc->ptOffset;
	pDel->szInt = pSrc->szInt;
	CIntervalImage *pAlt = new CIntervalImage;
	pAlt->ptOffset = pSrc->ptOffset;
	pAlt->szInt = pSrc->szInt;
	/*
	 * first pass - output thinned object in delobj
	 */
	strip(pSrc,pDel,&nDelArea,nSmoothPasses--);
	/*
	 * subsequent passes - use output of previous pass as input,
	 * swap between the two temporary objects.
	 */
	while (nDelArea > 0)
	{
		pSrc = pDel;
		pDel = pAlt;
		pAlt = pSrc;
		strip(pSrc,pDel,&nDelArea,nSmoothPasses--);
	}
	/*
	 * now need to standardise the interval domain, copy for
	 * output object, and free workspace
	 */
	delete pAlt;
	return pDel;
}

int find_tips_and_branches(CIntervalImage *pSrc, CImageWrp &pImgWrp, int *px1, int *py1)
{
	int nHeight = pImgWrp.GetHeight();
	int nWidth = pImgWrp.GetWidth();
	int nTipMarked = 0;
	for (int nRow = 0; nRow < pSrc->nrows; nRow++)
	{
		int y0 = nRow+pSrc->ptOffset.y-pImgWrp.GetOffset().y;
		for (int nInt = 0; nInt < pSrc->prows[nRow].nints; nInt++)
		{
			for (int k = pSrc->prows[nRow].pints[nInt].left; k <= pSrc->prows[nRow].pints[nInt].right; k++)
			{
				int nCount = 0;
				int x0 = pSrc->ptOffset.x+k-pImgWrp.GetOffset().x;
				for (int y = y0-1; y <= y0+1; y++)
				{
					if (y < 0 || y >= nHeight)
						continue;
					for (int x = x0-1; x <= x0+1; x++)
					{
						if (x < 0 || x >= nWidth)
							continue;
						color *pMask = pImgWrp.GetRow(y,0);
						if (pMask[x])
							nCount++;
					}
				}
				if (nCount > 3 || nCount == 1)
					return 1;
				if (nCount == 2) // tip
				{
					if (nTipMarked == 0)
					{
						*px1 = k+pSrc->ptOffset.x;
						*py1 = nRow+pSrc->ptOffset.y;
					}
					else if (nTipMarked > 1) // > 2 tips
						return 1;
					nTipMarked++;
				}
			}
		}
	}
	return nTipMarked==0?1:0;
}

static POINT arrNeis[] =
{
	{-1,-1},
	{-1,0},
	{-1,1},
	{0,1},
	{1,1},
	{1,0},
	{1,-1},
	{0,-1},
};

void make_binary_image(CIntervalImage *pSrc, CImageWrp &wrp)
{
	wrp.CreateNew(pSrc->szInt.cx, pSrc->szInt.cy, "GRAY");
	for (int y = 0; y < pSrc->szInt.cy; y++)
	{
		color *pMask = wrp.GetRow(y,0);
		memset(pMask, 0, pSrc->szInt.cx*sizeof(color));
		INTERVALROW IntRow = pSrc->prows[y];
		for (int i = 0; i < IntRow.nints; i++)
		{
			INTERVAL Int = IntRow.pints[i];
			for (int x = Int.left; x <= Int.right; x++)
				pMask[x] = 0xFFFF;
		}
	}
	wrp.SetOffset(pSrc->ptOffset);
};

CChromoAxis *axis_from_int_img(CIntervalImage *pSrc, CImageWrp &wrp, int x1, int y1)
{
	// Create the array of points by image
	CArray<CPoint,CPoint&> arrPoints;
	CPoint pt(x1-pSrc->ptOffset.x,y1-pSrc->ptOffset.y);
	arrPoints.Add(CPoint(x1,y1));
	bool bContinue = true;
	while (bContinue)
	{
		bContinue = false;
		for (int i = 0; i < sizeof(arrNeis)/sizeof(arrNeis[0]); i++)
		{
			CPoint ptNew(pt.x+arrNeis[i].x,pt.y+arrNeis[i].y);
			if (ptNew.x < 0 || ptNew.x >= pSrc->szInt.cx || ptNew.y < 0 || ptNew.y >= pSrc->szInt.cy)
				continue;
			color *pRow = wrp.GetRow(ptNew.y, 0);
			if (pRow[ptNew.x])
			{
				wrp.GetRow(pt.y,0)[pt.x] = 0;
				pt = ptNew;
				arrPoints.Add(CPoint(pt.x+pSrc->ptOffset.x,pt.y+pSrc->ptOffset.y));
//				pRow[ptNew.x] = 0;
				bContinue = true;
				break;
			}
		}
	}
	// Create the axis from selected points
	int nDiv = 3;
	int nPoints = 3+(arrPoints.GetSize()-1)/nDiv;
	CArray<CPoint,CPoint&> arrPtsSel;
	arrPtsSel.SetSize(nPoints);
	CPoint *ppt = arrPtsSel.GetData()+1;
	int nSec = (arrPoints.GetSize()-1)%nDiv;
	int nPoints2 = arrPoints.GetSize()/2;
	for (int i = 0; i < arrPoints.GetSize(); i++)
	{
		if (i <= nPoints2 && i%nDiv == 0 || i > nPoints2 && i%nDiv == nSec)
		{
			int i3 = i/nDiv;
			arrPtsSel[i3+1] = arrPoints[i];
		}
	}
	int nExtend = 9;
	int nDist = nExtend/nDiv;
	if (nPoints < nDist+3)
		nDist = nPoints-3;
	if (nDist > 0)
	{
		int nMul = (2*nExtend)/(nDiv*nDist);
		arrPtsSel[0].x = arrPtsSel[1].x+nMul*(arrPtsSel[1].x-arrPtsSel[nDist+1].x);
		arrPtsSel[0].y = arrPtsSel[1].y+nMul*(arrPtsSel[1].y-arrPtsSel[nDist+1].y);
		arrPtsSel[nPoints-1].x = arrPtsSel[nPoints-2].x+nMul*(arrPtsSel[nPoints-2].x-arrPtsSel[nPoints-2-nDist].x);
		arrPtsSel[nPoints-1].y = arrPtsSel[nPoints-2].y+nMul*(arrPtsSel[nPoints-2].y-arrPtsSel[nPoints-2-nDist].y);
	}
	// Init axis object
	CChromoAxis *pAxis = new CChromoAxis;
	pAxis->Init(IntegerAxis, nPoints, arrPtsSel.GetData());
	return pAxis;
}

CChromoAxis *SkelAxis(CIntervalImage *pSrc, CImageWrp &pImgWrp)
{
	CIntervalImage *pSkel = skeleton(pSrc, 6);
	int x1,y1;
	CImageWrp pImgTst;
	make_binary_image(pSkel, pImgTst);
	int nBranches = find_tips_and_branches(pSkel, pImgTst, &x1, &y1);
	if (nBranches == 0)
	{
		CChromoAxis *pAxis = axis_from_int_img(pSkel,pImgTst,x1,y1);
		delete pSkel;
		if (pAxis->m_nSize > 3)
			return pAxis;
		else
			delete pAxis;
	}
	else
		delete pSkel;
	return NULL;
}


