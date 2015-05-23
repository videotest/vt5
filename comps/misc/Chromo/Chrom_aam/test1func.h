#ifndef __Test1Func_h__
#define __Test1Func_h__

#include "misc_utils.h"

class CTest1Func 
{

public:
	CTest1Func();
	~CTest1Func();
 
	virtual void DoWork( BYTE *pur,int cx,int cy,BYTE *pre,
		                 _point *p1,_point *p2,int *CountP,int *Whide);

	virtual BYTE DilaX(WORD *pwork,WORD *pwork1,int cx,int cy,int *pd,WORD MaxCol,BYTE *pres);
	virtual BYTE Erro(WORD *pwork,WORD *pwork1,int cx,int cy,int *pd,WORD NumI,WORD MaxCol,
		              DWORD *q,DWORD *q1,DWORD Count1,DWORD *Count2);
	virtual BYTE ErroX(WORD *pwork,WORD *pwork1,int cx,int cy,int *pd,WORD NumI,WORD MaxCol,
		               DWORD *q,DWORD *q1);
	virtual BYTE ErroXLight(WORD *pwork,WORD *pwork1,int cx,int cy,int *pd,WORD NumI,WORD MaxCol,
		               DWORD *q,DWORD *q1);
	virtual BYTE Del_L(BYTE *pres,int cx,int cy,int *pd,int *pd1);
	virtual BYTE Dila(WORD *pwork,WORD *pwork1,int cx,int cy,int *pd,int *pd1,
					  DWORD *q,DWORD *q1,DWORD Count,DWORD *Count1,BYTE *pre,WORD *pimg,int NumI);  
	virtual void DrawDivLine(WORD *pwork,BYTE *pre,int cx,int cy,int *pd,int *pd1,
							 _point *p1,_point *p2,int *Count);
	virtual void DrawLine(DWORD Pos1,DWORD Pos2,BYTE* pre,int cx,int cy);

	int m_bWasFatalError;

protected:
	int	m_iOffset;
	int iConst;
	int mflag ;
	WORD *pwork;
	WORD *pwork1;
	int pd[10];
	int pd1[10];
	DWORD *p_Count1;
    DWORD *p_Count2;
	WORD *Val1;
    WORD *p_ObjColor;
	WORD *SizeObjCount;
	WORD *SizeObjCount_h;
	DWORD **p_p_ObjCount;
	WORD CountObject;
};



#endif //__Test1Func_h__