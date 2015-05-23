#ifndef __DivFunc_h__
#define __DivFunc_h__

#include "MorphInfo.h"

BOOL ChangePercent(int ip);
BOOL Erosion (WORD * hptr,int X,int Y,WORD bFill);
void Erodants(WORD *hptr, WORD *_hptr, int X, int Y);
void CleanImageForward(WORD *hptr1, WORD *hptr2, int X, int Y);
void CleanImageBack(WORD *hptr1, WORD *hptr2, int X, int Y);
void DilationX(WORD *hptr, WORD *_hptr, WORD * pSrcImage, int X, int Y);
BOOL Dilation (WORD * hptr8, WORD * xhptr8, int X, int Y, WORD bFill);
void Erose(CMorphInfo &in);
BOOL Deaglom(CMorphInfo &in);
void Clean(CMorphInfo &in);
inline WORD CalcDilateNum(WORD wErNum) {return wErNum*3-wErNum/3;}

void *AllocMem( DWORD dwSize );

#define AllocMem( dwSize ) __AllocMem( dwSize )

inline LPVOID __AllocMem( DWORD dwSize )
{
	LPVOID p = malloc(dwSize);
	if( p )
	{
		memset(p, 0, dwSize);
		return p;
	}
	else
	{
		return NULL;
	}
}



#endif //__DivFunc_h__