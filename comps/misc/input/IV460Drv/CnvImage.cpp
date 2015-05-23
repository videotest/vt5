#include "StdAfx.h"
#include "CnvImage.h"
#include "iv4.h"


void _BtCopy(LPBYTE lpDst, LPBYTE lpSrc, unsigned DX, unsigned nBPP)
{
	memcpy(lpDst, lpSrc, DX*nBPP);
}

void _8_To_24(LPBYTE lpDst, LPBYTE lpSrc, unsigned DX, unsigned nBPP)
{
	for (int i = 0; i < DX; i++,lpDst+=3,lpSrc++)
		lpDst[0] = lpDst[1] = lpDst[2] = lpSrc[0];
}

void _24_To_8(LPBYTE lpDst, LPBYTE lpSrc, unsigned DX, unsigned nBPP)
{
	for (int i = 0; i < DX; i++,lpDst++,lpSrc+=3)
		lpDst[0] = Bright(lpSrc[0],lpSrc[1],lpSrc[2]);
}

void _32_To_8(LPBYTE lpDst, LPBYTE lpSrc, unsigned DX, unsigned nBPP)
{
	for (int i = 0; i < DX; i++,lpDst++,lpSrc+=4)
		lpDst[0] = Bright(lpSrc[1],lpSrc[2],lpSrc[3]);
}

void _32_To_24(LPBYTE lpDst, LPBYTE lpSrc, unsigned DX, unsigned nBPP)
{
	for (int i = 0; i < DX; i++,lpDst+=3,lpSrc+=4)
	{
		lpDst[0] = lpSrc[0];
		lpDst[1] = lpSrc[1];
		lpDst[2] = lpSrc[2];
	}
}

CNVFUNC g_CnvFuncs[4][2] =
{
	{_BtCopy,_8_To_24},
	{NULL, NULL},
	{_24_To_8,_BtCopy},
	{_32_To_8,_32_To_24}
};


