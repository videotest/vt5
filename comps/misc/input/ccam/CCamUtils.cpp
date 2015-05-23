#include "StdAfx.h"
#include "CCamUtils.h"
#include "ccapi.h"
#include "Defs.h"
#include <math.h>
#include "StdProfile.h"

int BitnessByDatamode(int datamode)
{
	if (datamode==CC_DATA_16BIT_11_DOWNTO_0)
		return 12;
	else if (datamode==CC_DATA_16BIT_9_DOWNTO_0||datamode==CC_DATA_16BIT_11_DOWNTO_2)
		return 10;
	else
		return 8;
}

template<class ColorRepr> void MakeCnvBuffer(ColorRepr *pCnvBuff, ColorRepr MaxColor)
{
	for (int i = 0; i <= MaxColor; i++)
		pCnvBuff[i] = i;
}

struct COLORPOINT
{
	int r,g,b,br;
};

void _ReadCnvBuffer(CColorCurve &Curve, LPCTSTR lpstrSec)
{
	CArray<COLORPOINT,COLORPOINT&> arr;
	BOOL bEnable = CStdProfileManager::m_pMgr->GetProfileInt(lpstrSec, _T("Enable"), FALSE, true, true);
	if (!bEnable) return;
//	int nPoints = CStdProfileManager::m_pMgr->GetProfileInt(lpstrSec, _T("Points"), 0, true, true);
	for (int nPoint = 0; ; nPoint++)
	{
		TCHAR szEntry[200];
		_itot(nPoint, szEntry, 10); 
		CString s = CStdProfileManager::m_pMgr->GetProfileString(lpstrSec, szEntry, NULL, false, true);
		if (s.IsEmpty()) break;
		COLORPOINT cp;
		cp.r = cp.g = cp.b = cp.br = -1;
		if (_stscanf(s, _T("(%d,%d,%d,%d)"), &cp.r, &cp.g, &cp.b, &cp.br) != 4) continue;
		if (cp.r == -1 || cp.g == -1 || cp.b == -1 || cp.br == -1) continue;
		int n = arr.GetSize();
		if (n > 0 && (cp.r <= arr[n-1].r || cp.g <= arr[n-1].g || cp.b <= arr[n-1].b))
			continue;
		arr.Add(cp);
	}
	Curve.nPoints = arr.GetSize();
	if (Curve.nPoints > 0)
	{
		Curve.Red.alloc(Curve.nPoints);
		Curve.Green.alloc(Curve.nPoints);
		Curve.Blue.alloc(Curve.nPoints);
		Curve.Bright.alloc(Curve.nPoints);
		for (int i = 0; i < arr.GetSize(); i++)
		{
			Curve.Red[i] = arr[i].r;
			Curve.Green[i] = arr[i].g;
			Curve.Blue[i] = arr[i].b;
			Curve.Bright[i] = arr[i].br;
		}
	}
}

static void _CvtPoints(_ptr_t2<int> &Buff, int nPoints, int *pPoints, int nMaxColor)
{
	Buff.alloc(nPoints);
	for (int i = 0; i < nPoints; i++)
		Buff[i] = pPoints[i]*nMaxColor/255;
}

void _MakeCurve(CColorCurve &Curve8, CColorCurve &Curve, int nMaxColor)
{
	Curve.nPoints = Curve8.nPoints;
	_CvtPoints(Curve.Red, Curve.nPoints, Curve8.Red, nMaxColor);
	_CvtPoints(Curve.Green, Curve.nPoints, Curve8.Green, nMaxColor);
	_CvtPoints(Curve.Blue, Curve.nPoints, Curve8.Blue, nMaxColor);
	_CvtPoints(Curve.Bright, Curve.nPoints, Curve8.Bright, nMaxColor);
}

void ReadCnvBuffers(CColorCurve &Curve8, CColorCurve &Curve10, CColorCurve &Curve12)
{
	_ReadCnvBuffer(Curve8, _T("ColorCurve"));
	_MakeCurve(Curve8, Curve10, 0x3FF);
	_MakeCurve(Curve8, Curve12, 0xFFF);
}


template<class ColorRepr> void InitCnvBuffer(int nPoints, int *pClr, int *pBri,
	ColorRepr *pCnvBuff, ColorRepr MaxColor)
{
	if (nPoints == 0)
	{
		for (int i = 0; i <= MaxColor; i++)
			pCnvBuff[i] = i;
		return;
	}
	memset(pCnvBuff, 0, (MaxColor+1)*sizeof(ColorRepr));
	int x;
	for (int i = 0; i < nPoints; i++)
		pCnvBuff[pClr[i]] = (ColorRepr)pBri[i];
	if (pClr[0] == 0)
		pCnvBuff[0] = pBri[0];
	else
	{
		for (x = 0; x < pClr[0]; x++)
			pCnvBuff[x] = pBri[0]*x/pClr[0];
	}
	if (pClr[nPoints-1] == MaxColor)
		pCnvBuff[MaxColor] = pBri[nPoints-1];
	else
	{
		for (x = pClr[nPoints-1]; x <= MaxColor; x++)
			pCnvBuff[x] = pBri[nPoints-1]+(MaxColor-pBri[nPoints-1])*(x-pClr[nPoints-1])/(MaxColor-pClr[nPoints-1]);
	}
	for (i = 0; i < nPoints-1; i++)
	{
		if (pClr[i+1]==pClr[i])
			pClr[i+1] = pBri[i+1];
		else
		{
			for (x = pClr[i]+1; x < pClr[i+1]; x++)
				pCnvBuff[x] = (ColorRepr)(pBri[i]+(pBri[i+1]-pBri[i])*(x-pClr[i])/(pClr[i+1]-pClr[i]));
		}
	}
}


template<class ColorRepr> void ApplyCnvBufferBCG(ColorRepr *pCnvBuff, int nBrightness, int nContrast, double dGamma, ColorRepr MaxColor)
{
	double a = 1.+nContrast/255.;//pow(MaxColor,nContrast/100.);
	int b = nBrightness*MaxColor/255;
	dGamma = dGamma==0.?1.:(dGamma!=1.)?::tan((dGamma+1.)/4.*PI):1000.;
	double	g = max(0.001, dGamma);
	double CnvMul = MaxColor/pow(MaxColor, 1/g);
	for (int i = 0; i <= MaxColor; i++)
	{
		double d = a*double(pCnvBuff[i])+b;
		if (d < 0) d = 0;
		if (d > MaxColor) d = MaxColor;
		d = CnvMul*pow(d,1/g);
		pCnvBuff[i] = (ColorRepr)(d);
	}
}

template void MakeCnvBuffer<BYTE>(int nPoints, int *pClr, int *pBri, _ptr_t2<byte> &Buff,
	int dBrightness, int dContrast, double dGamma, BYTE MaxColor);
template void MakeCnvBuffer<WORD>(int nPoints, int *pClr, int *pBri, _ptr_t2<byte> &Buff,
	int dBrightness, int dContrast, double dGamma, WORD MaxColor);

template<class ColorRepr> void MakeCnvBuffer(int nPoints, int *pClr, int *pBri, _ptr_t2<byte> &Buff,
	int nBrightness, int nContrast, double dGamma, ColorRepr MaxColor)
{
	Buff.alloc((MaxColor+1)*sizeof(ColorRepr));
	InitCnvBuffer(nPoints, pClr, pBri, (ColorRepr*)(byte*)Buff, MaxColor);
	ApplyCnvBufferBCG((ColorRepr*)(byte*)Buff, nBrightness, nContrast, dGamma, MaxColor);
}


template<class ColorRepr> void CalcMeanValues(ColorRepr *pData, DWORD dwRow, int cx, int cy,
	ColorRepr &nRed, ColorRepr &nGreen,	ColorRepr &nBlue, int nMaxValue)
{
	double r = 0., g = 0., b = 0.;
	for (int y = 0; y < cy; y++)
	{
		ColorRepr *pRow = (ColorRepr*)pData;
		for (int x = 0; x < cx; x++)
		{
			r += double(pRow[2]);
			g += double(pRow[1]);
			b += double(pRow[0]);
			pRow += 3;
		}
		pData += dwRow;
	}
	nRed = min(max(int(r/(cx*cy)),0),nMaxValue);
	nGreen = min(max(int(g/(cx*cy)),0),nMaxValue);
	nBlue = min(max(int(b/(cx*cy)),0),nMaxValue);
}

template<class ColorRepr> void DoCalcWhiteBalanceBri(ColorRepr *pData, DWORD dwRow, int cx, int cy,
	int &nBrightR, int &nBrightG, int &nBrightB, ColorRepr nMaxColor)
{
	ColorRepr nRed,nGreen,nBlue;
	CalcMeanValues<ColorRepr>(pData, dwRow, cx, cy, nRed, nGreen, nBlue, nMaxColor);
	int br = (int(nRed)*60+int(nGreen)*118+int(nBlue)*22)/200;
	nBrightR += (br-nRed)*255/nMaxColor;
	nBrightG += (br-nGreen)*255/nMaxColor;
	nBrightB += (br-nBlue)*255/nMaxColor;
}

template<class ColorRepr> void DoCalcWhiteBalanceContr(ColorRepr *pData, DWORD dwRow, int cx, int cy,
	int &nContrastR, int &nContrastG, int &nContrastB, ColorRepr nMaxColor)
{
	ColorRepr nRed,nGreen,nBlue;
	CalcMeanValues<ColorRepr>(pData, dwRow, cx, cy, nRed, nGreen, nBlue, nMaxColor);
	int br = (int(nRed)*60+int(nGreen)*118+int(nBlue)*22)/200;
	nContrastR = int(255*double(br-nRed)/double(nRed));
	nContrastG = int(255*double(br-nGreen)/double(nGreen));
	nContrastB = int(255*double(br-nBlue)/double(nBlue));
}

template void DoCalcWhiteBalanceBri<BYTE>(BYTE *pData, DWORD dwRow, int cx, int cy,
	int &nBrightR, int &nBrightG, int &nBrightB, BYTE nMaxColor);
template void DoCalcWhiteBalanceBri<WORD>(WORD *pData, DWORD dwRow, int cx, int cy,
	int &nBrightR, int &nBrightG, int &nBrightB, WORD nMaxColor);
template void DoCalcWhiteBalanceContr<BYTE>(BYTE *pData, DWORD dwRow, int cx, int cy,
	int &nBrightR, int &nBrightG, int &nBrightB, BYTE nMaxColor);
template void DoCalcWhiteBalanceContr<WORD>(WORD *pData, DWORD dwRow, int cx, int cy,
	int &nBrightR, int &nBrightG, int &nBrightB, WORD nMaxColor);

template<class ColorRepr> void MirrorImageGS(ColorRepr *pData, DWORD dwRowCR, int cx, int cy,
	BOOL bVertical, BOOL bHorizontal)
{
	if (bVertical)
	{
		_ptr_t2<ColorRepr> buf(dwRowCR);
		for (int y = 0; y < (cy-1)/2; y++)
		{
			ColorRepr *p1 = pData+y*dwRowCR;
			ColorRepr *p2 = pData+(cy-1-y)*dwRowCR;
			memcpy((ColorRepr*)buf, p1, dwRowCR*sizeof(ColorRepr));
			memcpy(p1, p2, dwRowCR*sizeof(ColorRepr));
			memcpy(p2, (ColorRepr *)buf, dwRowCR*sizeof(ColorRepr));
		}
	}
	if (bHorizontal)
	{
		for (int y = 0; y < cy; y++)
		{
			ColorRepr *pRow = pData+y*dwRowCR;
			for (int x = 0; x < (cx-1)/2; x++)
			{
				ColorRepr c = pRow[x];
				pRow[x] = pRow[cx-1-x];
				pRow[cx-1-x] = c;
			}
		}
	}
}

template void MirrorImageGS<BYTE>(BYTE *pData, DWORD dwRowCR, int cx, int cy,
	BOOL bVertical, BOOL bHorizontal);
template void MirrorImageGS<WORD>(WORD *pData, DWORD dwRowCR, int cx, int cy,
	BOOL bVertical, BOOL bHorizontal);


