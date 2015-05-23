#if !defined(__PUtils_H__)
#define __PUtils_H__

#include "ChrUtils.h"
#include "math.h"

int hypot8(int i, int j);

inline int isqrt(int x)
{
	return (int)sqrt(double(x));
}
inline int isqrt8(int x)
{
	return (int)sqrt(double(64*x));
}


class _CGrayVal
{
public:
	CImageWrp pImgWrp;
	CIntervalImage *pIntvl;
	POINT ptOffsImg;
	bool bAvailable;
	int  nClrMode; // 0 - gray, 1 - rgb, 2 - cgh
	bool bBlackOnWhite;
	color clrDefault;
	_CGrayVal(CImageWrp &pImgWrp, CIntervalImage *pIntvl);
	void Gray4Val(int l, int k, color *g4);
	void Pane4Val(int l, int k, int c, color *g4, byte *mask4);
};

color interp(color *g4, int fractline, int fractkol);
color interpmask(color *g4, byte *mask4, byte *resmask, int fractline, int fractkol);


#endif

