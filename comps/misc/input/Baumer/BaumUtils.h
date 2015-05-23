#if !defined(__BaumUtils_H__)
#define __BaumUtils_H__

inline int Range(int n, int nMin, int nMax) {return n < nMin ? nMin : n > nMax ? nMax : n;}
namespace BaumUtils{
inline int Bright ( int b, int g, int r )	//функция для получения яркости
{
	return (r*60+g*118+b*22)/200;
};
}
using namespace BaumUtils;

inline bool DIBIsTrueColor1(LPBITMAPINFOHEADER lpbi)
{
	return lpbi->biBitCount == 24;
}

inline bool DIBIsTrueColor2(LPBITMAPINFOHEADER lpbi)
{
	return lpbi->biBitCount >= 30;
}

inline bool DIBIsGrayScale1(LPBITMAPINFOHEADER lpbi)
{
	return lpbi->biBitCount == 8;
}

inline bool DIBIsGrayScale2(LPBITMAPINFOHEADER lpbi)
{
	return lpbi->biBitCount == 8;
}

inline bool DIBIsTrueColor(LPBITMAPINFOHEADER lpbi)
{
	return lpbi->biBitCount >= 24;
}

inline bool DIBIsGrayScale(LPBITMAPINFOHEADER lpbi)
{
	return lpbi->biBitCount <= 24;
}

inline int DIBBps(LPBITMAPINFOHEADER lpbi)
{
	return lpbi->biBitCount == 8 || lpbi->biBitCount == 24 ? 1 : 2;
}

inline int DIBBpp(LPBITMAPINFOHEADER lpbi)
{
	return DIBIsTrueColor(lpbi)?DIBBps(lpbi)*3:DIBBps(lpbi);
}

inline DWORD DIBRowSize(LPBITMAPINFOHEADER lpbi)
{
	return (DIBBpp(lpbi)*lpbi->biWidth+3)/4*4;
}

inline LPVOID DIBData(LPBITMAPINFOHEADER lpbi)
{
	return (LPVOID)(((RGBQUAD *)(lpbi+1))+lpbi->biClrUsed);
}

inline int DIBShift(LPBITMAPINFOHEADER lpbi)
{
	int nShift = 8;
	switch(lpbi->biBitCount)
	{
	case 10: case 30: nShift = 6; break;
	case 12: case 36: nShift = 4; break;
	case 16: case 48: nShift = 0; break;
	}
	return nShift;
}


#endif