#include "StdAfx.h"
#include "Bayer2RGB.h"

template<class ColorRepr> void _Colorize1Row(ColorRepr *psr, ColorRepr *psb,
	ColorRepr *pd, ColorRepr *pCnvR, ColorRepr *pCnvG, ColorRepr *pCnvB,
	int cx, ColorRepr Mask, int nSatNum)
{
	ColorRepr *pds = pd;
	for (int x = 1; x < cx-2; x +=2)
	{
		pd[3] = pCnvB[Mask&psb[1]]; // 0,0 blue
		pd[4] = pCnvG[Mask&(unsigned(2*psr[1]+psb[0]+psb[2])/4)]; // 0,0 green
		pd[5] = pCnvR[Mask&(unsigned(psr[0]+psr[2])/2)]; // 0,0 red
		pd[6] = pCnvB[Mask&(unsigned(psb[1]+psb[3])/2)]; //0,1 blue
		pd[7] = pCnvG[Mask&(unsigned(2*psb[2]+psr[1]+psr[3])/4)]; // 0,1 green
		pd[8] = pCnvR[Mask&(psr[2])]; // 0,1 red
		psr+=2;
		psb+=2;
		pd +=6;
	}
	memcpy(pds, pds+6, 3*sizeof(ColorRepr));
	memcpy(pds+3, pds+6, 3*sizeof(ColorRepr));
	memcpy(pds+3*(cx-1), pds+3*(cx-3), 3*sizeof(ColorRepr));
	memcpy(pds+3*(cx-2), pds+3*(cx-3), 3*sizeof(ColorRepr));
}

static int nSat;

template<class ColorRepr> void _SaturatePoint(ColorRepr *pd, ColorRepr Mask)
{
	int b = pd[0], g = pd[1], r = pd[2];
	int br = (b+(g<<1)+r)>>2;
	b = ((b-br)<<nSat)+br;
	pd[0] = min(max(b,0),Mask);
	g = ((g-br)<<nSat)+br;
	pd[1] = min(max(g,0),Mask);
	r = ((r-br)<<nSat)+br;
	pd[2] = min(max(r,0),Mask);
}

template<class ColorRepr> void _SaturatePointAdd(ColorRepr *pd, ColorRepr Mask)
{
	int b = pd[0], g = pd[1], r = pd[2];
	int br = (b+(g<<1)+r)>>2;
	int nSat1 = nSat-1;
	b = ((b-br)<<nSat)+((b-br)<<nSat1)+br;
	pd[0] = min(max(b,0),Mask);
	g = ((g-br)<<nSat)+((g-br)<<nSat1)+br;
	pd[1] = min(max(g,0),Mask);
	r = ((r-br)<<nSat)+((r-br)<<nSat1)+br;
	pd[2] = min(max(r,0),Mask);
}

template<class ColorRepr> void _SaturatePoint05(ColorRepr *pd, ColorRepr Mask)
{
	int b = pd[0], g = pd[1], r = pd[2];
	int br = (b+(g<<1)+r)>>2;
	b = ((b-br)>>1)+(b-br)+br;
	pd[0] = min(max(b,0),Mask);
	g = ((g-br)>>1)+(g-br)+br;
	pd[1] = min(max(g,0),Mask);
	r = ((r-br)>>1)+(r-br)+br;
	pd[2] = min(max(r,0),Mask);
}

template<class ColorRepr> void _SaturatePoint10(ColorRepr *pd, ColorRepr Mask)
{
	int b = pd[0], g = pd[1], r = pd[2];
	int br = (b+(g<<1)+r)>>2;
	b = ((b-br)<<1)+br;
	pd[0] = min(max(b,0),Mask);
	g = ((g-br)<<1)+br;
	pd[1] = min(max(g,0),Mask);
	r = ((r-br)<<1)+br;
	pd[2] = min(max(r,0),Mask);
}

template<class ColorRepr> void _Colorize2Rows(ColorRepr *ps0, ColorRepr *ps1, ColorRepr *ps2,
	ColorRepr *ps3, ColorRepr *pd1, ColorRepr *pd2, ColorRepr *pCnvR, ColorRepr *pCnvG,
	ColorRepr *pCnvB, int cx, ColorRepr Mask, int nSaturation, FARPROC cp)
{
	typedef void (*pColorizePoint)(ColorRepr *pd, ColorRepr Mask);
	for (int x = 1; x < cx-2; x +=2)
	{
		pd1[3] = pCnvB[Mask&(unsigned(ps0[0]+ps0[2]+ps2[0]+ps2[2])/4)]; //0,1 blue
		pd1[4] = pCnvG[Mask&(unsigned(ps0[1]+ps1[0]+ps1[2]+ps2[1])/4)]; // 0,1 green
		pd1[5] = pCnvR[Mask&(ps1[1])]; // 0,1 red
		if (cp)	((pColorizePoint&)cp)(&pd1[3],Mask);
		pd1[6] = pCnvB[(unsigned(ps0[2]+ps2[2])/2)]; // 0,0 blue
		pd1[7] = pCnvG[(unsigned(4*ps1[2]+ps0[1]+ps0[3]+ps2[1]+ps2[3])/8)]; // 0,0 green
		pd1[8] = pCnvR[(unsigned(ps1[1]+ps1[3])/2)]; // 0,0 red
		if (cp)	((pColorizePoint&)cp)(&pd1[6],Mask);
		pd2[3] = pCnvB[(unsigned(ps2[0]+ps2[2])/2)]; // 1,1 blue
		pd2[4] = pCnvG[(unsigned(4*ps2[1]+ps1[0]+ps1[2]+ps3[0]+ps3[2])/8)]; // 1,1 green
		pd2[5] = pCnvR[(unsigned(ps1[1]+ps3[1])/2)]; // 1,1 red
		if (cp)	((pColorizePoint&)cp)(&pd2[3],Mask);
		pd2[6] = pCnvB[(ps2[2])]; // 1,0 blue
		pd2[7] = pCnvG[(unsigned(ps1[2]+ps2[1]+ps2[3]+ps3[2])/4)]; // 1,0 green
		pd2[8] = pCnvR[(unsigned(ps1[1]+ps1[3]+ps3[1]+ps3[3])/4)]; // 1,0 red
		if (cp)	((pColorizePoint&)cp)(&pd2[6],Mask);
		ps0+=2;
		ps1+=2;
		ps2+=2;
		ps3+=2;
		pd1+=6;
		pd2+=6;
	}
}

template<class ColorRepr> void _DoColorize(ColorRepr *pSrc, ColorRepr *pDst, int cx, int cy,
	int nSaturation, ColorRepr *pCnvR, ColorRepr *pCnvG, ColorRepr *pCnvB, DWORD dwRowSrc,
	DWORD dwRowDst, ColorRepr Mask)
{
	void (*pSatFunc)(ColorRepr *pd, ColorRepr Mask);
	if (nSaturation == 0)
		pSatFunc = NULL;
	else if (nSaturation==1)
		pSatFunc = _SaturatePoint05;
	else if (nSaturation==2)
		pSatFunc = _SaturatePoint10;
	else if (nSaturation&1)
		pSatFunc = _SaturatePointAdd;
	else
		pSatFunc = _SaturatePoint;
	nSaturation /= 2;
	nSat = nSaturation;
	LPBYTE ps0s = (LPBYTE)pSrc;
	LPBYTE ps1s = ps0s+dwRowSrc;
	LPBYTE ps2s = ps1s+dwRowSrc;
	LPBYTE ps3s = ps2s+dwRowSrc;
	LPBYTE pd1s = ((LPBYTE)pDst)+dwRowDst;
	LPBYTE pd2s = pd1s+dwRowDst;
//	_Colorize1Row((ColorRepr*)ps1s, (ColorRepr*)ps0s, (ColorRepr*)pDst, pCnvR, pCnvB, pCnvB,
//		cx, Mask, nSaturation);
	for (int y = 1; y < cy-2; y+=2)
	{
		_Colorize2Rows(((ColorRepr *)ps0s), ((ColorRepr *)ps1s), ((ColorRepr *)ps2s),
			((ColorRepr *)ps3s), (ColorRepr *)pd1s, (ColorRepr *)pd2s, pCnvR, pCnvG, pCnvB,
			cx, Mask, nSaturation, (FARPROC)pSatFunc);
		// Make first and last column
		memcpy(pd1s, pd1s+12*sizeof(ColorRepr), 3*sizeof(ColorRepr));
		memcpy(pd1s+3*sizeof(ColorRepr), pd1s+12*sizeof(ColorRepr), 3*sizeof(ColorRepr));
		memcpy(pd1s+6*sizeof(ColorRepr), pd1s+12*sizeof(ColorRepr), 3*sizeof(ColorRepr));
		memcpy(pd1s+9*sizeof(ColorRepr), pd1s+12*sizeof(ColorRepr), 3*sizeof(ColorRepr));
		memcpy(pd2s, pd2s+12*sizeof(ColorRepr), 3*sizeof(ColorRepr));
		memcpy(pd2s+3*sizeof(ColorRepr), pd2s+12*sizeof(ColorRepr), 3*sizeof(ColorRepr));
		memcpy(pd2s+6*sizeof(ColorRepr), pd2s+12*sizeof(ColorRepr), 3*sizeof(ColorRepr));
		memcpy(pd2s+9*sizeof(ColorRepr), pd2s+12*sizeof(ColorRepr), 3*sizeof(ColorRepr));
		memcpy(pd1s+3*(cx-1)*sizeof(ColorRepr), pd1s+3*(cx-3)*sizeof(ColorRepr), 3*sizeof(ColorRepr));
		memcpy(pd1s+3*(cx-2)*sizeof(ColorRepr), pd1s+3*(cx-3)*sizeof(ColorRepr), 3*sizeof(ColorRepr));
		memcpy(pd2s+3*(cx-1)*sizeof(ColorRepr), pd2s+3*(cx-3)*sizeof(ColorRepr), 3*sizeof(ColorRepr));
		memcpy(pd2s+3*(cx-2)*sizeof(ColorRepr), pd2s+3*(cx-3)*sizeof(ColorRepr), 3*sizeof(ColorRepr));
		// Next step
		ps0s += 2*dwRowSrc;
		ps1s += 2*dwRowSrc;
		ps2s += 2*dwRowSrc;
		ps3s += 2*dwRowSrc;
		pd1s += 2*dwRowDst;
		pd2s += 2*dwRowDst;
	}
	memcpy(pDst, ((LPBYTE)pDst)+dwRowDst, dwRowDst);
	memcpy(pd1s, pd1s-dwRowDst, dwRowDst);
//	_Colorize1Row((ColorRepr*)ps1s, (ColorRepr*)ps0s, (ColorRepr*)pd1s, pCnvR, pCnvB, pCnvB,
//		cx, Mask, nSaturation);
}

template void _DoColorize<WORD>(WORD *pSrc, WORD *pDst, int cx, int cy,
	int nSaturation, WORD *pCnvR, WORD *pCnvG, WORD *pCnvB, DWORD dwRowSrc,
	DWORD dwRowDst, WORD Mask);
template void _DoColorize<BYTE>(BYTE *pSrc, BYTE *pDst, int cx, int cy,
	int nSaturation, BYTE *pCnvR, BYTE *pCnvG, BYTE *pCnvB, DWORD dwRowSrc,
	DWORD dwRowDst, BYTE Mask);

