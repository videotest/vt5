#include "StdAfx.h"
#include "ChrUtils.h"
#include "PUtils.h"
#include "Settings.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define MAXTABLU	16

static unsigned char hlutab[(MAXTABLU+1) * (MAXTABLU+2) / 2] = {
  0,
  8, 11,
 16, 18, 23,
 24, 25, 29, 34,
 32, 33, 36, 40, 45,
 40, 41, 43, 47, 51, 57,
 48, 49, 51, 54, 58, 62, 68,
 56, 57, 58, 61, 64, 69, 74, 79,
 64, 64, 66, 68, 72, 75, 80, 85, 90,
 72, 72, 74, 76, 79, 82, 87, 91, 96,102,
 80, 80, 82, 84, 86, 89, 93, 98,102,108,113,
 88, 88, 89, 91, 94, 97,100,104,109,114,119,124,
 96, 96, 97, 99,101,104,107,111,115,120,125,130,136,
104,104,105,107,109,111,115,118,122,126,131,136,142,147,
112,112,113,115,116,119,122,125,129,133,138,142,148,153,158,
120,120,121,122,124,126,129,132,136,140,144,149,154,159,164,170,
128,128,129,130,132,134,137,140,143,147,151,155,160,165,170,175,181,
};

int hypot8(int i, int j)
{
	register int t;

	/* make arguments positive */
	if (i < 0)
		i = -i;
	if (j < 0)
		j = -j;
	/* get in right order */
	if (i > j) {
		t = i;
		i = j;
		j = t;
	}
	/* check whether can use table look-up */
	if (j > MAXTABLU)
		return(sqrt(double(64*i*i + 64*j*j)));
	else
		return((int)hlutab[((j*(j+1)) >> 1) + i]);
}

_CGrayVal::_CGrayVal(CImageWrp &pImgWrp, CIntervalImage *pIntvl)
{
	this->pImgWrp = pImgWrp;
	this->pIntvl = pIntvl;
	ptOffsImg = pImgWrp.GetOffset();
	CString s = pImgWrp.GetColorConvertor();
//	bGray = s.CompareNoCase(_T("RGB"))!=0;
	nClrMode = s.CompareNoCase(_T("RGB"))==0?1:s.CompareNoCase(_T("CGH"))==0||
		s.CompareNoCase(_T("FISH"))==0?2:0;
	bAvailable = true;
	bBlackOnWhite = FALSE;
	clrDefault = 0;
}

/*
 * Extract 4 grey values from object
 * at locations (k,l), (k+1,l), (k,l+1), (k+1,l+1).
 */
void _CGrayVal::Gray4Val(int y, int x, color *g4)
{
	g4[0] = g4[1] = g4[2] = g4[3] = clrDefault;
	int nHeight = pImgWrp.GetHeight();
	int nWidth = pImgWrp.GetWidth();
	if (nClrMode==0||nClrMode==2) // gray or CGH
	{
		int nPane = nClrMode==0?0:g_CGHPane; 
		color *pg = g4;
		for (int i = 0; i <= 1; i++)
		{
			if (y >= pIntvl->ptOffset.y && y < pIntvl->ptOffset.y+pIntvl->nrows)
			{
				INTERVALROW *pIntRow = &pIntvl->prows[y-pIntvl->ptOffset.y];
				int nInts = pIntRow->nints;
				int nCol = x-pIntvl->ptOffset.x;
				INTERVAL *pInt = pIntRow->pints;
				while (nInts-- > 0)
				{
					if (nCol+1 < pInt->left)
						break;
					if (nCol <= pInt->right)
					{
						color *pData0 = pImgWrp.GetRow(y-ptOffsImg.y,nPane);
						if (nCol >= pInt->left)
							pg[0] = pData0[x-ptOffsImg.x];
						if (nCol+1 <= pInt->right)
							pg[1] = pData0[x-ptOffsImg.x+1];
					}
					pInt++;
				}
				y++;
				pg += 2;
			}
		}
	}
	else // RGB
	{
		color *pg = g4;
		for (int i = 0; i <= 1; i++)
		{
			if (y >= pIntvl->ptOffset.y && y < pIntvl->ptOffset.y+pIntvl->nrows)
			{
				INTERVALROW *pIntRow = &pIntvl->prows[y-pIntvl->ptOffset.y];
				int nInts = pIntRow->nints;
				int nCol = x-pIntvl->ptOffset.x;
				INTERVAL *pInt = pIntRow->pints;
				while (nInts-- > 0)
				{
					if (nCol+1 < pInt->left)
						break;
					if (nCol <= pInt->right)
					{
//						color *pData0 = pImgWrp.GetRow(y-ptOffsImg.y,0);
						color *red = pImgWrp.GetRow(y-ptOffsImg.y,0);
						color *green = pImgWrp.GetRow(y-ptOffsImg.y,1);
						color *blue = pImgWrp.GetRow(y-ptOffsImg.y,2);
						if (nCol >= pInt->left)
							pg[0] = Bright(red[x-ptOffsImg.x],green[x-ptOffsImg.x],blue[x-ptOffsImg.x]);
						if (nCol+1 <= pInt->right)
							pg[1] = Bright(red[x-ptOffsImg.x+1],green[x-ptOffsImg.x+1],blue[x-ptOffsImg.x+1]);
//							pg[1] = pData0[x-ptOffsImg.x+1];
					}
					pInt++;
				}
				y++;
				pg += 2;
			}
		}
	}
	for (int i = 0; i < 4; i++)
	{
		g4[i] >>= 8;
	}
	if (bBlackOnWhite)
	{
		for (int i = 0; i < 4; i++)
//			g4[i] = 0xFFFF-g4[i];
			g4[i] = 0xFF-g4[i];
	}
}


void _CGrayVal::Pane4Val(int y, int x, int c, color *g4, byte *mask4)
{
	g4[0] = g4[1] = g4[2] = g4[3] = clrDefault;
	mask4[0] = mask4[1] = mask4[2] = mask4[3] = 0;
	int nHeight = pImgWrp.GetHeight();
	int nWidth = pImgWrp.GetWidth();
	{
		color *pg = g4;
		byte  *pm = mask4;
		for (int i = 0; i <= 1; i++)
		{
			if (y >= pIntvl->ptOffset.y && y < pIntvl->ptOffset.y+pIntvl->nrows)
			{
				INTERVALROW *pIntRow = &pIntvl->prows[y-pIntvl->ptOffset.y];
				int nInts = pIntRow->nints;
				int nCol = x-pIntvl->ptOffset.x;
				INTERVAL *pInt = pIntRow->pints;
				while (nInts-- > 0)
				{
					if (nCol+1 < pInt->left)
						break;
					if (nCol <= pInt->right)
					{
						color *pData0 = pImgWrp.GetRow(y-ptOffsImg.y,c);
						if (nCol >= pInt->left)
						{
							pg[0] = pData0[x-ptOffsImg.x];
							pm[0] = 0xFF;
						}
						if (nCol+1 <= pInt->right)
						{
							pg[1] = pData0[x-ptOffsImg.x+1];
							pm[1] = 0xFF;
						}
					}
					pInt++;
				}
				y++;
				pg += 2;
				pm += 2;
			}
		}
	}
	for (int i = 0; i < 4; i++)
		g4[i] >>= 8;
	if (bBlackOnWhite)
	{
		for (int i = 0; i < 4; i++)
			g4[i] = 0xFF-g4[i];
	}
}


color interp(color *g4, int fractline, int fractkol)
{
	int mfractline, mfractkol;
	color g;
	mfractline = 256 - fractline;
	mfractkol = 256 - fractkol;
	g = (g4[0] * mfractline*mfractkol + g4[1] * mfractline*fractkol +
		g4[2] * fractline*mfractkol + g4[3] * fractline*fractkol + 32767) / 65536;
	return(g);
}

color interpmask(color *g4, byte *m4, byte *pm, int fractline, int fractkol)
{
	double d1 = fractline/256.;
	double d2 = fractkol/256.;
	double d1_ = 1.-d1;
	double d2_ = 1.-d2;
	int m40=m4[0]/255, m41=m4[1]/255, m42=m4[2]/255, m43=m4[3]/255;
	double dClr = g4[0]*d1_*d2_*m40+g4[1]*d1_*d2*m41+g4[2]*d1*d2_*m42+g4[3]*d1*d2*m43;
	double dCoef = d1_*d2_*m40+d1_*d2*m41+d1*d2_*m42+d1*d2*m43;
	ASSERT(dCoef >= 0. && dCoef <= 1.);
	if (dCoef == 0.)
	{
		*pm = 0;
		return 0;
	}
	double dClr1 = dClr/dCoef;
	ASSERT(dClr1 >= 0. && dClr1 <= 255.);
	*pm = 255;
	return dClr1;
}

