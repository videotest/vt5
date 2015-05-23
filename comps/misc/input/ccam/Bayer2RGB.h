#if !defined(__BayerToRGB_H__)
#define __BayerToRGB_H__



template<class ColorRepr> void _DoColorize(ColorRepr *pSrc, ColorRepr *pDst, int cx, int cy,
	int nSaturation, ColorRepr *pCnvR, ColorRepr *pCnvG, ColorRepr *pCnvB, DWORD dwRowSrc,
	DWORD dwRowDst, ColorRepr Mask);





#endif
