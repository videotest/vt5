#if !defined(__CnvImage_H__)
#define __CnvImage_H__


typedef void (*CNVFUNC)(LPBYTE lpSrc, LPBYTE lpDst, unsigned DX, unsigned nBPP);

extern CNVFUNC g_CnvFuncs[4][2];


#endif