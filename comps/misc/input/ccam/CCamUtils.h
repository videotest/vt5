#if !defined(__CCamUtils_H__)
#define __CCamUtils_H__

#include "misc_templ.h"

struct BITMAPINFO256
{
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD bmiColors[256];
};

struct CCCamImageData
{
	LPBITMAPINFOHEADER lpbi;
	byte *pData;
};

int BitnessByDatamode(int datamode);

#define _CCAM_MT_

#if defined(_CCAM_MT_)
#define __EnterCriticalSection(p) EnterCriticalSection(p)
#define __LeaveCriticalSection(p) LeaveCriticalSection(p)
#else
#define __EnterCriticalSection(p) 
#define __LeaveCriticalSection(p) 
#endif

template<class ColorRepr> void MakeCnvBuffer(int nPoints, int *pClr, int *pBri, _ptr_t2<byte> &Buff,
	int nBrightness, int nContrast, double dGamma, ColorRepr MaxColor);
template<class ColorRepr> void DoCalcWhiteBalanceBri(ColorRepr *pData, DWORD dwRow, int cx, int cy,
	int &nBrightR, int &nBrightG, int &nBrightB, ColorRepr nMaxColor);
template<class ColorRepr> void DoCalcWhiteBalanceContr(ColorRepr *pData, DWORD dwRow, int cx, int cy,
	int &nContrastR, int &nContrastG, int &nContrastB, ColorRepr nMaxColor);
template<class ColorRepr> void MirrorImageGS(ColorRepr *pData, DWORD dwRowCR, int cx, int cy,
	BOOL bVertical, BOOL bHorizontal);

struct CColorCurve
{
	int nPoints;
	_ptr_t2<int> Red;
	_ptr_t2<int> Green;
	_ptr_t2<int> Blue;
	_ptr_t2<int> Bright;
	CColorCurve()
	{
		nPoints = 0;
	};
};

void ReadCnvBuffers(CColorCurve &Curve8, CColorCurve &Curve10, CColorCurve &Curve12);


#endif
