#include "stdafx.h"
#include "MorphDivEx.h"
#include "resource.h"
#include "binary.h"
#include "TimeTest.h"
#include "alloc.h"
#include <vector>
#include <functional>
#include <queue>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define NEW_DIVIDE 1

BYTE ErroStep1(WORD *pwork, WORD *pwork1, int cx, int cy, int *pd, WORD NumI,
	WORD MaxCol, DWORD *q, DWORD *q1, DWORD Count1, DWORD *Count2)
{
	BYTE flag=0;
	WORD *pworky;
	int	iRowMemSize = (cx+3)/4*4;
	int i=0,j=0,n=0;
	*Count2=0;
	int M_i=0;
	if ( NumI%5==1 || NumI%5==3)
		M_i=9;
	else 
		M_i=5;
	for (j=0;j<(int)Count1;j++)
	{
		pworky = pwork + q[j];
		for (i=1;i<M_i;i++)
		{
			n=pd[i];
		    if (*(pworky+n)==MaxCol)
			{
				*(pworky+n)=NumI;
				q1[*Count2]=pworky+n-pwork;
				(*Count2)++;
				flag=1;
			}
		}
	}
	return flag;
}

BYTE ErroX(int iOffset, WORD *pwork, WORD *pwork1, int cx, int cy, int *pd, WORD NumI,
	WORD MaxCol, DWORD *q, DWORD *q1)
{ // Чистка (~open by reconstruction), размер маски зависит от удаленности от границы
CTimeTest tt1(true, "erosion2:ero in MorphDivideExBin");
tt1.m_bEnableFileOutput = true;
	int porog1=0,porog2=0;
	int koff=0;
	if (iOffset==2)
	{
		porog1 = 5;
		porog2 = 15;
		koff = 15;
	}
	else
	{
		porog1 = 0;
		porog2 = 8;
		koff = 8;
	}
	WORD *p_Wy;
	WORD *p_W1y;
	WORD *p_Wd;
	WORD p1,p2,p3,p4;
	BYTE flag=0;
	WORD *pworky;
	WORD *pwork1y;
	int Mask=0,Maskx1=0,Maskx2=0,Masky1=0,Masky2=0;
	int	iRowMemSize = (cx+3)/4*4;
	int x=0,y=0,i=0,m_min=0,m_max=0,x1=0,y1=0;
	int M_i=0,Count=0;
	M_i=5;
	for (y=1;y<cy-1;y++)
	{
		pworky = pwork+y*iRowMemSize+1;
		pwork1y = pwork1+y*iRowMemSize+1;
		for (x=1;x<cx-1;x++)
		{
			*pwork1y=*pworky;
			if (*pworky!=0)
			{
				if (*pworky<porog1)
				{ // 0..porog1-1 - максимум из минимумов по тройкам соседей
					p1=*(pworky+pd[1]);
					p2=*(pworky+pd[2]);
					p3=*(pworky+pd[3]);
					p4=*(pworky+pd[4]);
 					m_min = max(max(min(p1,min(p2,p3)),min(p2,min(p3,p4))),
							max(min(p3,min(p4,p1)),min(p4,min(p1,p2))));
					if (m_min>0 && m_min<*pworky)
						*pwork1y=m_min;
				}
				else
				{ // porog1..porog2-1 - min по 4 соседям (но не 0)
					if (*pworky<porog2)
					{
						m_min=MaxCol;
						flag=1;
						for (i=1;i<M_i;i++)
						{
							if (*(pworky+pd[i])<m_min)
							{
								m_min=*(pworky+pd[i]);
							}
						}
						if (m_min!=0)
							*pwork1y=m_min;
					}
					else
					{ // porog2 и выше - min по квадрату размером 1+(*pworky-porog2)/koff
						Mask=1+(*pworky-porog2)/koff;
						Maskx1=Mask;
						if (x-Maskx1<0) Maskx1=x;
						Maskx2=Mask;
						if (x+Maskx2>=cx) Maskx2=cx-x-1;
						Masky1=Mask;
						if (y-Masky1<0) Masky1=y;
						Masky2=Mask;
						if (y+Masky2>=cy) Masky2=cy-y-1;
						m_min=MaxCol;
						for (y1=y-Masky1;y1<=y+Masky2;y1++)
						{
							p_Wd=pwork+y1*iRowMemSize+x-Maskx1;
							for (x1=x-Maskx1;x1<=x+Maskx2;x1++)
							{
								if (*p_Wd<m_min && *p_Wd>0)
									m_min=*p_Wd;
								p_Wd++;
							}
						}
						*pwork1y=m_min;
					}
				}
			}
			pworky++;
			pwork1y++;
		}
	}
tt1.Stop();
CTimeTest tt2(true, "erosion2:dil in MorphDivideExBin");
tt2.m_bEnableFileOutput = true;
	smart_alloc(pwork2, WORD, cy*iRowMemSize+1);
	ZeroMemory(pwork2 ,sizeof(WORD)*(cy*iRowMemSize+1));
	WORD *p_W=pwork1;
	WORD *p_W1=pwork2;
	Count=0;
	do
	{ // реконструкция (примерно как в Open By Reconstruction)
		flag=0;
		Count++;
		for (y=1;y<cy-1;y++)
		{
			pworky = pwork+y*iRowMemSize+1;
			p_Wy = p_W+y*iRowMemSize+1;
			p_W1y = p_W1+y*iRowMemSize+1;
			for (x=1;x<cx-1;x++)
			{
				*p_W1y=*p_Wy;
				if (*p_Wy<*pworky)
				{
					m_max=*p_Wy;
					for (i=1;i<9;i++)
					{
						if (*(p_Wy+pd[i])>m_max)
						{
							m_max=*(p_Wy+pd[i]);
							flag=1;
						}
					}
					if (m_max>*pworky)
						*p_W1y=*pworky;
					else
						*p_W1y=m_max;
				}
				pworky++;
				p_Wy++;
				p_W1y++;
			}
		}
		p_Wd=p_W;
		p_W=p_W1;
		p_W1=p_Wd; 
	}while(flag);
	if (p_W!=pwork1)
		CopyMemory(pwork1,pwork2,cy*iRowMemSize*sizeof(WORD));
	//delete pwork2; 
tt2.Stop();
	return flag;
}

BYTE ErroXLight(WORD *pwork, WORD *pwork1, int cx, int cy, int *pd, WORD NumI,
	WORD MaxCol, DWORD *q, DWORD *q1)
{
	int porog1=5;
	int porog2=30;
	WORD *p_Wy;
	WORD *p_W1y;
	WORD *p_Wd;
	WORD p1,p2,p3,p4;
	BYTE flag=0;
	WORD *pworky;
	WORD *pwork1y;
	int	iRowMemSize = (cx+3)/4*4;
	int x=0,y=0,i=0,m_min=0,m_max=0;
	int M_i=0,Count=0;
	M_i=5;
	for (y=1;y<cy-1;y++)
	{
		pworky = pwork+y*iRowMemSize+1;
		pwork1y = pwork1+y*iRowMemSize+1;
		for (x=1;x<cx-1;x++)
		{
			*pwork1y=*pworky;
			if (*pworky!=0)
			{
				m_min=MaxCol;
				flag=1;
				p1=*(pworky+pd[1]);
				p2=*(pworky+pd[2]);
				p3=*(pworky+pd[3]);
				p4=*(pworky+pd[4]);
				if (*pworky<porog1)
					m_min = max(max(min(p1,p2),min(p2,p3)),max(min(p3,p4),min(p4,p1)));
				if (*pworky>=porog1 && *pworky<porog2)
					m_min = max(max(min(p1,min(p2,p3)),min(p2,min(p3,p4))),
						max(min(p3,min(p4,p1)),min(p4,min(p1,p2))));
				if (*pworky>=porog2)
					m_min = min(min(p1,p2),min(p3,p4));
				if (m_min>0 && m_min<*pworky)
					*pwork1y=m_min;
			}
			pworky++;
			pwork1y++;
		}
	}
	smart_alloc(pwork2, WORD, cy*iRowMemSize+1);
	ZeroMemory(pwork2 ,sizeof(WORD)*(cy*iRowMemSize+1));
	WORD *p_W=pwork1;
	WORD *p_W1=pwork2;
	Count=0;
	do
	{
		flag=0;
		Count++;
		for (y=1;y<cy-1;y++)
		{
			pworky = pwork+y*iRowMemSize+1;
			p_Wy = p_W+y*iRowMemSize+1;
			p_W1y = p_W1+y*iRowMemSize+1;
			for (x=1;x<cx-1;x++)
			{
				*p_W1y=*p_Wy;
				if (*p_Wy<*pworky)
				{
					m_max=*p_Wy;
					for (i=1;i<9;i++)
					{
						if (*(p_Wy+pd[i])>m_max)
						{
							m_max=*(p_Wy+pd[i]);
							flag=1;
						}
					}
					if (m_max>*pworky)
						*p_W1y=*pworky;
					else
						*p_W1y=m_max;
				}
				pworky++;
				p_Wy++;
				p_W1y++;
			}
	}
	p_Wd=p_W;
	p_W=p_W1;
	p_W1=p_Wd; 
	}while(flag);
	if (p_W!=pwork1)
		CopyMemory(pwork1,pwork2,cy*iRowMemSize*sizeof(WORD));
	//delete pwork2; 
	return flag;
}

BYTE DillX(WORD *pwork,WORD *pwork1,int cx,int cy,int *pd,WORD MaxCol,BYTE *pres)
{
	BYTE *presy;
	BYTE flag=0;
	WORD *pworky;
	WORD *pwork1y;
	int	iRowMemSize = (cx+3)/4*4;
	int x=0,y=0,i=0;
	WORD a1=0,a=0,m_max=0;
	for (y=1;y<cy-1;y++)
	{
		pworky = pwork+y*iRowMemSize+1;
		pwork1y = pwork1+y*iRowMemSize+1;
		presy = pres+y*iRowMemSize+1;
		for (x=1;x<cx-1;x++)
		{
			*pwork1y=*pworky;
			a=*pworky;
			if (a!=0)
			{
				m_max=a;
				for (i=1;i<5;i++)
				{
					a1=*(pworky+pd[i]);
					if (a1>m_max) 
					{
						m_max=a1;
					}
				}
				*pwork1y=m_max;
			}
			pworky++;
			pwork1y++;
			presy++;
		}
	}
	return flag;
}

//static FILE *fdeb;
BYTE Dill1(WORD *pwork, WORD *pwork1, int cx, int cy, int *pd, int *pd1,
	DWORD *q, DWORD *q1, DWORD Count, DWORD *Count1,
	BYTE *pre, WORD *pimg, int NumI)
{
	BYTE flag=0;
	BYTE flag1=0;
	BYTE flag2=0;
	int i=0,j=0,n=0;
	WORD a=0;
	WORD *pworky;
	BYTE *prey;
	WORD *pimgy;
	int	iRowMemSize = (cx+3)/4*4;
	*Count1=0;
	WORD NCol=0;
	if (NumI>2)
		NCol=(WORD)(NumI-2);
//fprintf(fdeb, "   (");

	for (j=0;j<(int)Count;j++)
	{
		pworky=pwork+q[j];
		pimgy=pimg+q[j]; 
		prey=pre+q[j];
		a=*pworky;
		if (a>0)
		{
			flag1=0;
			flag2=0;
			for(i=1;i<9;i++)
			{
				n=pd[i];
				if(*(pworky+n)==0 && *(prey+n)!=0)
				{ // точка внутри объекта, но еще не залита
					if(*(pimgy+n)>NCol)
					{ // точка достаточно высокого уровня
						*(pworky+n)=a;
						q1[*Count1]=pworky+n-pwork;
						(*Count1)++;
//fprintf(fdeb, "*(%d,%d,%d,%d) ", (pworky+n-pwork)%1304, (pworky+n-pwork)/1304, j, i);
						flag1++;
						flag=1;
					}
					flag2++;
				}			
			}
			if(flag1==0 && flag2>0)
			{
				q1[*Count1]=pworky-pwork;
				(*Count1)++;
//fprintf(fdeb, "(%d,%d,%d,%d) ", (pworky-pwork)%1304, (pworky-pwork)/1304, j, i);
			}
		}
	}
//fprintf(fdeb, ")\n");
	return flag;
}

BYTE Dill2(WORD *pwork, WORD *pwork1, int cx, int cy, int *pd, int *pd1,
	DWORD *q, DWORD *q1, BYTE *f, BYTE *f1, DWORD Count, DWORD *Count1, BYTE *pre,
	WORD *pimg, int NumI)
{
	BYTE flag=0;
	BYTE flag1=0;
	BYTE flag2=0;
	int i=0,j=0,n=0;
	WORD a=0;
	WORD *pworky;
	BYTE *prey;
	WORD *pimgy;
	int	iRowMemSize = (cx+3)/4*4;
	*Count1=0;
	WORD NCol=0;
	if (NumI>2)
		NCol=(WORD)(NumI-2);
//fprintf(fdeb, "   (");
	for (j=0;j<(int)Count;j++)
	{
		if (f[j])
		{
			q1[*Count1]=q[j];
			f1[*Count1]=1;
			(*Count1)++;
			continue;
		}
		pworky=pwork+q[j];
		pimgy=pimg+q[j]; 
		prey=pre+q[j];
		a=*pworky;
		if (a>0)
		{
			flag1=0;
			flag2=0;
			for(i=1;i<9;i++)
			{
				n=pd[i];
				if(*(pworky+n)==0 && *(prey+n)!=0)
				{
					if(*(pimgy+n)>NCol)
					{
						*(pworky+n)=a;
						q1[*Count1]=pworky+n-pwork;
						f1[*Count1]=0;
						(*Count1)++;
//fprintf(fdeb, "*(%d,%d) ", (pworky+n-pwork)%1304, (pworky+n-pwork)/1304);
						flag1++;
						flag=1;
					}
					flag2++;
				}			
			}
			if(flag1==0 && flag2>0)
			{
				q1[*Count1]=pworky-pwork;
				f1[*Count1]=1;
				(*Count1)++;
//fprintf(fdeb, "(%d,%d) ", (pworky-pwork)%1304, (pworky-pwork)/1304);
			}
		}
	}
//fprintf(fdeb, ")\n");
	return flag;
}

BYTE Del_L(BYTE *pres, int cx, int cy, int *pd, int *pd1)
{
	int x=0,y=0,i=0,a1=0,a2=0,k=0,n=0;
	BYTE *presy;
	BYTE *presy1;
	int	iRowMemSize = (cx+3)/4*4;
	for (y=1;y<cy-1;y++)
	{
		presy = pres+y*iRowMemSize+1;
		for (x=1;x<cx-1;x++)
		{
			if(*presy==1)
			{
				k=0;
				a1=*(presy+pd1[1]);
				for(i=2;i<10;i++)
				{					
					a2=*(presy+pd1[i]);
					if (a1==1 && a2>1) k++;
					a1=a2;
					if (a2==0)
					{
						k=0;
						i=10;
					}
				}
				if (k==1)
				{
					presy1=presy;
					*presy1=255;
					do
					{
						n=0;
						for(i=1;i<9;i++)
							if(*(presy1+pd[i])==1)
							{ 
								presy1+=pd[i];
								i=9;
								n=1;
							}
						k=0;
						a1=*(presy1+pd1[1]);
						for(i=2;i<10;i++)
						{					
							a2=*(presy1+pd1[i]);
							if (a1==1 && a2>1) k++;
							a1=a2;
							if (a2==0)
							{
								k=0;
								i=10;
							}
						}
						if (k<2)
							*presy1=255;
						else
							n=0;
					}while(n);
				}
			}
			presy++;
		}		
	}
	return 0;
}

#define CALC_NEIGHBORS(pos,nNeighbors) \
{ \
	nNeighbors=0; \
	for(int j=0; j<8; j++) \
	{ \
		int Tst1 = pos + d2[j]; \
		if(pre[Tst1]==0) nNeighbors++; \
	} \
}

// для перебора будем хранить проверенные варианты в мапе,
// а непроверенные - в heap
class CLine
{
public:
	DWORD pos1, pos2;
	//CLine() {};
	CLine(int pos1new, int pos2new) : pos1(pos1new), pos2(pos2new) {};
};

struct LineLessPos : binary_function<CLine, CLine, bool> {
    bool operator()(const CLine& line1, const CLine& line2) const
	{
		return( line1.pos1<line2.pos1 ||
			line1.pos1==line2.pos1 && line1.pos2<line2.pos2 );
	}
};

// WARNING! Может использоваться только с одним iRowMemSize одновременно!
class CLine2 : public CLine
{
public:
	int x1,y1, x2,y2;
	double len;

	//CLine2() : len(-1) {};
	CLine2(int pos1new, int pos2new) : CLine(pos1new,pos2new), len(-1),
		y1(pos1new/iRowMemSize), x1(pos1new%iRowMemSize),
		y2(pos2new/iRowMemSize), x2(pos2new%iRowMemSize)
	{ UpdateLen(); };
	double UpdateLen() // проапдейтить и вернуть длину
	{ return len = _hypot(x2-x1, y2-y1); }

	static int iRowMemSize;
};

int CLine2::iRowMemSize;

struct LineGreaterLen : binary_function<CLine2, CLine2, bool> {
    bool operator()(const CLine2& line1, const CLine2& line2) const
	{
		return line1.len > line2.len;
	}
};

void DrowLine(DWORD Pos1, DWORD Pos2, BYTE* pre, int cx, int cy)
{
	int	iRowMemSize = (cx+3)/4*4;

	int d[4] = {1, iRowMemSize, -1, -iRowMemSize};
	int d2[8] =
	{1, 1+iRowMemSize, iRowMemSize, -1+iRowMemSize,
	-1, -1-iRowMemSize, -iRowMemSize, 1-iRowMemSize};

	CLine2::iRowMemSize = iRowMemSize;
	CLine2 best_line(Pos1,Pos2);

	const int nHash=1024;
	byte checks[nHash]; // хэш вместо полноценного сета для проверки
	{ for(int i=0; i<nHash; i++) checks[i] = 0; }
	priority_queue<CLine2, vector<CLine2>, LineGreaterLen> q;

	int nNeighbors1=0, nNeighbors2=0;
	CALC_NEIGHBORS(Pos1, nNeighbors1);
	CALC_NEIGHBORS(Pos2, nNeighbors2);
	
	if(nNeighbors1 && nNeighbors2)
		q.push(best_line); // если линия нормальная(от края до края) - займемся оптимизацией

	while(!q.empty())
	{
		//if(nPushCount>=203403) break; // !!! debug
		CLine2 line = q.top();
		q.pop();

		if( line.len > best_line.len + 2.0 ) break; // если в очереди только мусор - выходим
		if( line.len < best_line.len ) best_line = line; // если нашли лучшую - запомним

		//TRACE( "testing line (%i,%i)-(%i,%i) len=%1.3f\n",
		//	line.x1, line.y1,
		//	line.x2, line.y2,
		//	line.len );

		for(int i=0; i<4; i++)
		{
			int Tst = line.pos1 + d[i];
			if(pre[Tst]!=0)
			{ // точка в объекте, проверим, крайняя ли
				int nNeighbors=0;
				CALC_NEIGHBORS(Tst,nNeighbors);
				if(nNeighbors==0) continue; // не крайняя точка
				//CLine2 t( line.pos2, Tst );
				//int hash = (t.x1 & 15) + ((t.y1 & 15)<<4) + ((t.x2 & 15)<<8) + ((t.y2 & 15)<<12);
				int hash = line.pos2 + Tst*13731;
				hash ^= hash >> 16;
				hash ^= hash >> 8;
				hash &= nHash-1;
				if(!checks[hash])
				{ // если удалось воткнуть в set - значит, точку еще не проверяли
					checks[hash] = true;
					q.push( CLine2( line.pos2, Tst ) );
					//CLine2 line2( line.pos2, Tst );
					//TRACE( "pushing line (%i,%i)-(%i,%i) len=%1.3f\n",
					//	line2.x1, line2.y1,
					//	line2.x2, line2.y2,
					//	line2.len );
				}
			}
		}
	}

	int y1=best_line.y1;
	int x1=best_line.x1;
	int y2=best_line.y2;
	int x2=best_line.x2;

	int rad = int(ceil(best_line.len));
	rad = max(rad,1);

	if(rad==1)
	{ // для коротких линий - проверить, существует ли линия вообще
		int n=0;
		bool b1 = pre[best_line.pos1 + d2[7]]!=0;
		for(int i=0; i<8; i++)
		{
			bool b2 = pre[best_line.pos1 + d2[i]]!=0;
			if(b1 & !b2) n++;
			b1 = b2;
		}
		if(n<=1) return; // линия не разрезает объект
	}

	double kx = (double)(x2-x1)/rad;
	double ky = (double)(y2-y1)/rad;
	int i;
	for (i=0;i<=rad;i++)
	{
		int y=y1+(int)floor(ky*i+0.5);
		int x=x1+(int)floor(kx*i+0.5);
		pre[y*iRowMemSize+x]=1;
	}
}

void DrowDivLine(WORD *pwork, BYTE *pre, int cx, int cy, int *pd, int *pd1)
{
	int x=0,y=0,i=0,a1=0,a2=0,k=0,n=0,tz=0;
	DWORD Pos1=0,Pos2=0;
	WORD *pworky;
	WORD *pworky1;
	BYTE *prey;
	int	iRowMemSize = (cx+3)/4*4;
//  Скелетизация - для начала утоньшим так, чтобы была просто непрерывная (по 4 соседям) линия
	for (y=1;y<cy-1;y++)
	{
		pworky = pwork+y*iRowMemSize+1;
		for (x=1;x<cx-1;x++)
		{
			if(*pworky==1)
			{
				k=0;
				n=0;
				a1=*(pworky+pd1[1]);
				for(i=2;i<10;i++)
				{					
					a2=*(pworky+pd1[i]);
					if (a1!=255)
					{
						n++;
						if (a2==255)
							k++;
					}
					a1=a2;
				}
				if (k==1 && n>2)
				{
					*pworky=255;
				}
			}
			pworky++;
		}
	}

//  Скелетизация шаг 2 - утоньшим до непрерывной по 8 соседям (уберем точки в уголках)
	for (y=1;y<cy-1;y++)
	{
		pworky = pwork+y*iRowMemSize+1;
		for (x=1;x<cx-1;x++)
		{
			if(*pworky==1)
			{
				k=0;
				n=0;
				int a = *(pworky+pd[1]) == 1 ? 1 : 0;
				a += *(pworky+pd[2]) == 1 ? 2 : 0;
				a += *(pworky+pd[3]) == 1 ? 4 : 0;
				a += *(pworky+pd[4]) == 1 ? 8 : 0;
				if( a==3 || a==6 || a==12 || a==9 )
					*pworky=255;
			}
			pworky++;
		}
	}

//	return;
// Drow Line
	for (y=1;y<cy-1;y++)
	{
		prey = pre+y*iRowMemSize+1;
		pworky = pwork+y*iRowMemSize+1;
		for (x=1;x<cx-1;x++)
		{
			if(*pworky==1)
			{
				k=0;
				n=0;
				a1=*(pworky+pd1[1]);
				for(i=2;i<10;i++)
				{					
					a2=*(pworky+pd1[i]);
					if (a1==1 && a2!=1) k++;
					if (a2==0)
						n=1;
					a1=a2;
				}
				if (k==1)// && n==1)
				{					
					pworky1=pworky;
					*pworky1=255;
					Pos1=pworky1-pwork;
					do
					{	
						k=0;
						for(i=1;i<9;i++)
						{					
							if (*(pworky1+pd[i])==1)
							{
								pworky1+=pd[i];
								i=10;
								k=1;
							}							 
						}
						if (k==1)
						{
            				n=0;
							tz=0;
							a1=*(pworky1+pd1[1]);
							for(i=2;i<10;i++)
							{					
								a2=*(pworky1+pd1[i]);
								if (a1==1 && a2!=1) n++;
								if (a2==0) tz=1;
								a1=a2;
							}
							if (n<2 && tz==0)
								*pworky1=255;
							else
							{
								Pos2=pworky1-pwork;
								DrowLine(Pos1,Pos2,pre,cx,cy);
								Pos1=pworky1-pwork;
								*pworky1=255;
							}
						}
					}while(k);
					Pos2=pworky1-pwork;
					DrowLine(Pos1,Pos2,pre,cx,cy);
				}
				else
					if (k==0 )//&& n==1)
					{
						*pworky=255;
						*prey=1;
					}
			}
			pworky++;
			prey++;
		}
	}
	for (y=1;y<cy-1;y++)
	{
		prey = pre+y*iRowMemSize+1;
		pworky = pwork+y*iRowMemSize+1;
		for (x=1;x<cx-1;x++)
		{
			if(*pworky==1)
			{
				*prey=1;
			}
			pworky++;
			prey++;
		}
	}
}

void CActionMorphDivideEx::DoDivide(byte *pur/*source binary*/, byte *pre/*result*/,
	int cx, int cy, int iOffset, int nModifiedAlg, BOOL bFast)
{
	WORD  CountObject=0;
	//WORD *Val1=0;
	//DWORD *p_Count1=0;
	//DWORD *p_Count2=0;
	//WORD *p_ObjColor=0;
	//WORD *pwork=0;
	//WORD *pwork1=0;
	//int *pd=0;
	//int *pd1=0;
	//WORD *SizeObjCount=0;
	//WORD *SizeObjCount_h=0;
    //DWORD **p_p_ObjCount=0;
	int Hx_max = cx;
	int Hy_max = cy;
    int	iRowMemSize = (cx+3)/4*4;
//	ZeroMemory(pre,cy*iRowMemSize);
	int x=0,y=0,i=0,j=0,n=0;
	smart_alloc(pwork, WORD, cy*iRowMemSize+1);
	smart_alloc(pwork1, WORD, cy*iRowMemSize+1);
	ZeroMemory(pwork,(cy*iRowMemSize+1)*sizeof(WORD));
	ZeroMemory(pwork1,(cy*iRowMemSize+1)*sizeof(WORD));
	WORD *pworky;
    WORD *p_W;
	WORD *p_W1;
	WORD *p_Wd;
	byte *pury,*prey;

	int pd[10];
    pd[1]=-iRowMemSize; 
    pd[2]=1; 
    pd[3]= iRowMemSize;
    pd[4]=-1;
    pd[5]=-iRowMemSize+1;
    pd[6]= iRowMemSize+1;
    pd[7]= iRowMemSize-1;
    pd[8]=-iRowMemSize-1;

    int pd1[10];
    pd1[1]=-iRowMemSize;
    pd1[2]=-iRowMemSize+1;
    pd1[3]= 1;
    pd1[4]= iRowMemSize+1;
    pd1[5]= iRowMemSize;
    pd1[6]= iRowMemSize-1;
    pd1[7]=-1;
    pd1[8]=-iRowMemSize-1;
    pd1[9]=-iRowMemSize;
	StartNotification(8*cy+1700);
	// Make binary in pre and pwork
    for (y=1;y<cy-1;y++)
	{
		pury = pur+y*iRowMemSize+1;
		prey = pre+y*iRowMemSize+1;
		pworky = pwork+y*iRowMemSize+1;
		for (x=1;x<cx-1;x++)
		{
			if (*pury==255)
			{
				*pworky=65535;
				*prey=255;
			}
			pworky++;
			prey++;
			pury++;
		}
	}

	n=0;
	p_W=pwork;
	p_W1=pwork1;
	int NumI=1;
	WORD MaxCol=65535;

	smart_alloc(p_Count1, DWORD, cy*iRowMemSize/2);
	smart_alloc(p_Count2, DWORD, cy*iRowMemSize/2);
    DWORD Count1=0;
    DWORD Count2=0;
    DWORD Count3=0;
	DWORD Count_h=0;
	DWORD *q = p_Count1;
	DWORD *q1 = p_Count2;
	DWORD *qd=0;
	// 1. Make object contours in q
	for (y=1;y<cy-1;y++)
	{
	    pworky = p_W+y*iRowMemSize+1;
		for (x=1;x<cx-1;x++)
		{	
			if(*pworky==MaxCol)
				for(i=1;i<5;i++)
					if(*(pworky+pd[i])==0)
					{
						q[Count1++]=pworky-p_W;
						i=10;
					}
			pworky++;
		}
		Notify(y);
	}
	// Mark these contours
    for (j=0;j<(int)Count1;j++)
	{
		pworky = p_W + q[j];
		if (*pworky==MaxCol)
			*pworky=1;								
	}

	// 2. Preliminary erosion: in objects set each point's value as number of the operation
	CTimeTest tt1(true, "erosion1 in MorphDivideExBin");
	tt1.m_bEnableFileOutput = true;
    NumI=2;
	do
	{
       n=ErroStep1(p_W,p_W1,cx,cy,pd,NumI,MaxCol,q,q1,Count1,&Count2);
	   // Эрозия: из каждых 5 шагов второй и четвертый по 8 точкам, остальные по 4
	   // возвращает, было ли хоть что

	   NumI++;
	   qd=q;
	   q=q1;
	   q1=qd;
	   Count_h=Count1;
	   Count1=Count2;
	   Count2=Count_h;
	   if (NumI>500) n=0;
		Notify(cy+NumI);
	}while(n);

	// если было больше 500 шагов эрозии - остальное забиваем фоном???
    for (y=0;y<cy;y++)
	{
		pworky = pwork+y*iRowMemSize;
		for (x=0;x<cx;x++)
		{
			if(*pworky==MaxCol)
			   *pworky=0;
			pworky++;
		}
	}
	// Теперь p_W - расстояния от границы
//char szBuff[256];
//sprintf(szBuff, "NumI = %d\n", NumI);
//OutputDebugString(szBuff);

	tt1.Stop();
	CTimeTest tt2(true, "erosion2 in MorphDivideExBin");
	tt2.m_bEnableFileOutput = true;
	// 3. Erosion on step 2
	// (Open by reconstruction c размером маски пропорциональным расстоянию от границы)
    if(iOffset>1)
	{
       n=ErroX(iOffset,p_W,p_W1,cx,cy,pd,NumI,MaxCol,q,q1);
	   p_Wd=p_W;
	   p_W=p_W1;
       p_W1=p_Wd; 
	}
	else
	{
       n=ErroXLight(p_W,p_W1,cx,cy,pd,NumI,MaxCol,q,q1);
	   p_Wd=p_W;
	   p_W=p_W1;
       p_W1=p_Wd; 
	}
	// Теперь p_W - очищенные расстояния от границы (после OBR)
	Notify(cy+600);
	tt2.Stop();
	CTimeTest tt3(true, "find local maximums");
	tt3.m_bEnableFileOutput = true;

	// 4. Find local maximums (into Val1)
	// ищем точки >= всех 8 соседей, метим их MaxCol
	smart_alloc(Val1, WORD, cy*iRowMemSize+1);
	ZeroMemory(Val1,(cy*iRowMemSize+1)*sizeof(WORD));
    WORD *Val1y;
	int a,k;
	for (y=1;y<cy-1;y++)
	{
		pworky = p_W+y*iRowMemSize+1;
		Val1y = Val1+y*iRowMemSize+1;
		for (x=1;x<cx-1;x++)
		{			
			a=*pworky;
			if (a!=0)
			{
					k=0;
				    for (i=1;i<9;i++)
					    if (*(pworky+pd1[i])>a) 
						{
                            k=1;
							i=10;
						}
					if (k==0)
						*Val1y=MaxCol;
			}

			pworky++;
			Val1y++;
		}
		Notify(cy+600+y);
	}
	// 5. Clear false maximims
	int iCount = 0;
	do
	{
		k=0;
	    for (y=1;y<cy-1;y++)
		{
			pworky = p_W+y*iRowMemSize+1;
			Val1y = Val1+y*iRowMemSize+1;
			for (x=1;x<cx-1;x++)
			{	
				if (*Val1y==MaxCol)
				{
					for(i=1;i<9;i++)
						if(*(pworky+pd[i])>=*pworky && *(Val1y+pd[i])==0)
						{ // если есть соседняя неотмеченная точка, которая >= нашей
							// (идея в том, что могли быть помечены ровные площадки шире 1 пиксела)
							i=10;
							*Val1y=0;
							k=1;
						}
				}
				Val1y++;
				pworky++;
			}
		}
		if (k==1)
		{
		    for (y=cy-2;y>0;y--)
			{
			    pworky = p_W+y*iRowMemSize+cx-2;
			    Val1y = Val1+y*iRowMemSize+cx-2;
			    for (x=cx-2;x>0;x--)
				{	
				    if (*Val1y==MaxCol)
					{
					    for(i=1;i<9;i++)
						    if(*(pworky+pd[i])>=*pworky && *(Val1y+pd[i])==0)
							{
							    i=10;
							    *Val1y=0;
							    k=1;
							}
					}
				    Val1y--;
				    pworky--;
				}
			}

		}
		iCount++;
		Notify(2*cy+600+max(iCount,500));
	}while(k);
	// 6. Clear local maximums on objects' borders
    for (y=1;y<cy-1;y++)
	{
		Val1y = Val1+y*iRowMemSize+1;
		pury = pur+y*iRowMemSize+1;
		for (x=1;x<cx-1;x++)
		{
			if(*Val1y==MaxCol)
				for(i=1;i<5;i++)
					if(*(pury+pd[i])==0)
					{
						*Val1y=0;
						i=10;
					}
			Val1y++;
			pury++;
		}
		Notify(2*cy+1100+y);
	}
	tt3.Stop();
	CTimeTest ttMO(true, "mark object in MorphDivideExBin");
	ttMO.m_bEnableFileOutput = true;
	// Now in Val1 REAL local maximums inside objects. Use it as objects.

	// 7. Count objects and mark objects' bodies by objects' numbers.
	ZeroMemory(p_Count1,(cy*iRowMemSize/5)*sizeof(DWORD));
	ZeroMemory(p_Count2,(cy*iRowMemSize/5)*sizeof(DWORD));
	smart_alloc(p_ObjColor, WORD, 10000);
	Count1=0;
	Count2=0;
	CountObject=0;
	for (y=1;y<cy-1;y++)
	{
		Val1y = Val1+y*iRowMemSize+1;
		pworky = p_W+y*iRowMemSize+1;
		for (x=1;x<cx-1;x++)
		{	
			if(*Val1y==MaxCol)
			{
				p_ObjColor[CountObject]=*pworky;
				CountObject++;
				*Val1y=CountObject;
				Count1=1;
				p_Count1[0]=Val1y-Val1;
				do
				{ 
					Count2=0;
					for (i=0;i<(int)Count1;i++)
						for (j=1;j<9;j++)
							if(*(Val1+p_Count1[i]+pd[j])==MaxCol)
							{
								p_Count2[Count2] = p_Count1[i]+pd[j];
								*(Val1+p_Count2[Count2])=CountObject;
								Count2++;
							}
					Count1=Count2;
					CopyMemory(p_Count1,p_Count2,Count2*sizeof(DWORD));
				}while(Count2);					  
			}
			Val1y++;
			pworky++;
		}
		Notify(3*cy+1100+y);
	}
	// в Val1 нанесены "объекты" разного размера, каждый - связная область локального максимума

	smart_alloc(SizeObjCount, WORD, CountObject);
	smart_alloc(SizeObjCount_h, WORD, CountObject);
	typedef _ptr_t2<DWORD> ptrDWORD;
	smart_alloc(p_p_ObjCount, ptrDWORD, CountObject);
//	DWORD *p_ObjCount;
	for (i=0;i<CountObject;i++)
	{
		//p_p_ObjCount[i]=0;
		SizeObjCount[i]=0;
		SizeObjCount_h[i]=0;
	}
	for (y=1;y<cy-1;y++) // Count border points in each objects
	{
		Val1y = Val1+y*iRowMemSize+1;
		for (x=1;x<cx-1;x++)
		{	
			if (*Val1y!=0)
				for (i=1;i<5;i++)
					if(*(Val1y+pd[i])==0)
					{
						SizeObjCount[(*Val1y)-1]++;
						i=10;
					}
			Val1y++;
		}
	}
	for (i=0;i<CountObject;i++)
		p_p_ObjCount[i].alloc(SizeObjCount[i]);
		//p_p_ObjCount[i]=new DWORD[SizeObjCount[i]];
	// 8. Find border points.
	for (y=1;y<cy-1;y++)
	{
		Val1y = Val1+y*iRowMemSize+1;
		for (x=1;x<cx-1;x++)
		{	
			if (*Val1y!=0)
				for (i=1;i<5;i++)
					if(*(Val1y+pd[i])==0)
					{
						p_p_ObjCount[(*Val1y)-1][SizeObjCount_h[(*Val1y)-1]++] = Val1y - Val1;
						i=10;
					}
			Val1y++;
		}
		Notify(4*cy+1100+y);
	}
	// сложили координаты граничных точек "объектов" в p_p_ObjCount

	if (p_W!=pwork1) // Results of erosion in pwork1
		CopyMemory(pwork1,pwork,cy*iRowMemSize*sizeof(WORD));
	q = p_Count1;
	q1 = p_Count2;
	qd=0;
	int NumI1=1;
	ttMO.Stop();
	CTimeTest ttLM(true, "local maximums in MorphDivideExBin");
	ttLM.m_bEnableFileOutput = true;
	// Local maximums in pwork	
	CopyMemory(pwork,Val1,cy*iRowMemSize*sizeof(WORD));
	p_W=pwork;
	p_W1=pwork;
	int NColor=0;
	for (i=0;i<CountObject;i++) // Number of erosion for each maximum in p_ObjColor
		if (NColor<p_ObjColor[i])
			NColor=p_ObjColor[i];
    Count_h=0;
    Count1=0;
	Count2=0;
	Count3=0;
	ttLM.Stop();
	CTimeTest ttDil(true, "dilate object in MorphDivideExBin");
	ttDil.m_bEnableFileOutput = true;
	bool bCurClrAdded = false;
	smart_alloc(f1, BYTE, cx*cy/2);
	smart_alloc(f2, BYTE, cx*cy/2);
	BYTE *tf;
	memset(f1, 0, cx*cy/2);
	memset(f2, 0, cx*cy/2);
//fdeb = fopen("c:\\morph.txt", "wt");
	// 9. Dilate maximums to the borders of original objects
	if(0){ // !!! debug
		IUnknownPtr ptrI( ::CreateTypedObject(_bstr_t(szTypeImage)), false );
		IImage3Ptr      sptrI(ptrI);
		sptrI->CreateImage( cx-2,cy-2, _bstr_t("Gray"), -1 );
		color *p;
		for(int y=0; y<cy-2; y++)
		{
			sptrI->GetRow(y, 0, &p);
			for(int x=0; x<cx-2; x++)
				p[x] = pwork1[iRowMemSize*(y+1)+x+1]*256*4;
		}
		// debug - вставка в документ вспомогательной картинки
		IDocumentPtr doc(m_punkTarget);
		m_changes.SetToDocData(doc, sptrI);
	}
	do
	{
		if (!bCurClrAdded)
		{
			if (NColor>0)
				for (i=0;i<CountObject;i++)
					if (p_ObjColor[i]==NColor)
						for (j=0;j<SizeObjCount[i];j++)
						{
							q[Count1]=p_p_ObjCount[i][j];
							f1[Count1]=0;
							Count1++;
						}
			bCurClrAdded = true;
		}

		if (!bFast)
		{
			k=Dill1(p_W,p_W1,cx,cy,pd,pd1,q,q1,Count1,&Count2,pre,pwork1,NColor);
			NumI1++;
//fprintf(fdeb, "NumI1 = %d, k = %d, NColor = %d, Count1 = %d, Count2 = %d\n", NumI1, k,
//	NColor, Count1, Count2);
			qd = q;
			if (k==0)
			{
				NColor--;
				bCurClrAdded = false;
			}
			if (Count2>0)
				k=1;
			q = q1;
			q1 = qd;
			Count_h=Count1;
			Count1=Count2;
			Count2=Count_h;
		}
		else
		{
			k=Dill2(p_W,p_W1,cx,cy,pd,pd1,q,q1,f1,f2,Count1,&Count2,pre,pwork1,NColor);
			NumI1++;
//fprintf(fdeb, "NumI1 = %d, k = %d, NColor = %d, Count1 = %d, Count2 = %d, Count3 = %d\n",
//	NumI1, k, NColor, Count1, Count2, Count3);
			qd = q;
			if (k==0)
			{
				NColor--;
				bCurClrAdded = false;
			}
			if (Count2>0)
				k=1;
			q = q1;
			q1 = qd;
			tf = f1;
			f1 = f2;
			f2 = tf;
			Count_h=Count1;
			Count1=Count2;
			Count2=Count_h;
			if (!bCurClrAdded)
				memset(f1, 0, Count1);
		}
		if (nModifiedAlg==0 && NumI1>20*NumI)
			k=0;
		Notify(5*cy+1100+max(NumI1,500));
	}while(k);
	//delete f1;
	//delete f2;
//fclose(fdeb);
	ttDil.Stop();
	CTimeTest ttDivLin(true, "draw dividing lines in MorphDivideExBin");
	ttDivLin.m_bEnableFileOutput = true;
	// 9. Mark points on the contacts of two objects as background
	if (nModifiedAlg == FALSE)
	{
		for(y=1;y<cy-1;y++)
		{
    		pworky = pwork+y*iRowMemSize+1;
			for(x=1;x<cx-1;x++)
			{
				for(i=1;i<5;i++)
					if(*pworky!=*(pworky+pd[i]) && (*pworky)*(*(pworky+pd[i]))>0)
						*pworky=0;
				pworky++;
			}
			Notify(5*cy+1600+y);
		}
		// Make image: objects' point as 255, background as 0, dividing lines as 1
		for(y=0;y<cy;y++)
		{
    		pworky = pwork+y*iRowMemSize;
			prey = pre+y*iRowMemSize;
			for(x=0;x<cx;x++)
			{
				if (*pworky>0)
					*pworky=255;
				else
					if(*prey==0)
						*pworky=0;
					else
						*pworky=1;
				pworky++;
				prey++;
			}
		}
	}
	else
	{
		CopyMemory(pwork1,pwork,cy*iRowMemSize*sizeof(WORD));
		for(y=1;y<cy-1;y++)
		{
    		pworky = pwork+y*iRowMemSize+1;
    		WORD *pwork1y = pwork1+y*iRowMemSize+1;
			prey = pre+y*iRowMemSize+1;
			for(x=1;x<cx-1;x++)
			{
				if (*prey==0)
					*pworky=0;
				else
				{
					bool bContact = false;
					for(i=1;i<5;i++)
						if(*pwork1y!=*(pwork1y+pd[i]) && (*pwork1y)*(*(pwork1y+pd[i]))>0)
						{
							bContact = true;
							break;
						}
					if (bContact)
						*pworky=1;
					else
						*pworky = 255;
				}
				pworky++;
				pwork1y++;
				prey++;
			}
			Notify(5*cy+1600+y);
		}
	}
	// Now dividing lines' countours are found, draw it on image
	CopyMemory(pre,pur,cy*iRowMemSize);
	DrowDivLine(pwork,pre,cx,cy,pd,pd1);
	if(0){ // !!! debug
		IUnknownPtr ptrI( ::CreateTypedObject(_bstr_t(szTypeImage)), false );
		IImage3Ptr      sptrI(ptrI);
		sptrI->CreateImage( cx-2,cy-2, _bstr_t("Gray"), -1 );
		color *p;
		for(int y=0; y<cy-2; y++)
		{
			sptrI->GetRow(y, 0, &p);
			for(int x=0; x<cx-2; x++)
				p[x] = (pwork[iRowMemSize*(y+1)+x+1] & 3)*43600;
		}
		// debug - вставка в документ вспомогательной картинки
		IDocumentPtr doc(m_punkTarget);
		m_changes.SetToDocData(doc, sptrI);
	}
	Notify(6*cy+1700);
	ttDivLin.Stop();
	CTimeTest ttClear(true, "clear in MorphDivideExBin");
	ttClear.m_bEnableFileOutput = true;
#if !defined(NEW_DIVIDE)
	// Clear
	int p1=0,p2=0,p3=0,p4=0;
    for (y=0;y<cy;y++)
	{
		prey = pre+y*iRowMemSize;
		for (x=0;x<cx;x++)
		{
		   if (*prey==255)
		   {
			   p1=*(prey-iRowMemSize);
			   p2=*(prey+1);
			   p3=*(prey+iRowMemSize);
			   p4=*(prey-1);
			   if (p1<2 && p2<2 && p1+p2>0)
				   *prey=2;
			   if (p2<2 && p3<2 && p2+p3>0)
				   *prey=2;
			   if (p3<2 && p4<2 && p3+p4>0)
				   *prey=2;
			   if (p4<2 && p1<2 && p4+p1>0)
				   *prey=2;
		   }
		   prey++;
		}
		Notify(6*cy+1700+y);
	}
#endif
	for (y=0;y<cy;y++)
	{
		prey = pre+y*iRowMemSize;
		for (x=0;x<cx;x++)
		{
		   if (*prey<3)
			   *prey=0;
		   prey++;
		}
	} 
	for (y=0;y<cy-1;y++)
	{
		prey = pre+y*iRowMemSize;
		for (x=0;x<cx-1;x++)
		{
		   if (*prey==255)
			   if (*(prey+iRowMemSize+1)==255)
				   if (*(prey+1)==0)
					   if (*(prey+iRowMemSize)==0)
						   *prey=0;

		   if (*prey==0)
			   if (*(prey+iRowMemSize+1)==0)
				   if (*(prey+1)==255)
					   if (*(prey+iRowMemSize)==255)
			               *(prey+1)=0;

		   prey++;
		}
		Notify(7*cy+1700+y);
	}
    FinishNotification();
	//delete Val1;
	//Val1=0;
	//delete p_Count1;
	//p_Count1=0;
	//delete p_Count2;
	//p_Count2=0;
	//delete p_ObjColor;
	//p_ObjColor=0;
	//delete pwork;
	//pwork=0;
	//delete pwork1;
	//pwork1=0;
	//delete pd;
	//pd=0;
	//delete pd1;
	//pd1=0;
	//delete SizeObjCount;
	//SizeObjCount=0;
	//delete SizeObjCount_h;
	//SizeObjCount_h=0;
	//for (i=0;i<CountObject;i++)
	//	if(p_p_ObjCount[i]!=0)
	//	{
	//		delete p_p_ObjCount[i];
	//		p_p_ObjCount[i]=0;
	//	}
	//delete p_p_ObjCount;
	//p_p_ObjCount=0;
	ttClear.Stop();
}



IMPLEMENT_DYNCREATE(CActionMorphDivideEx, CCmdTargetEx);

// {B6E63343-3AD3-4c01-B13C-0494B342A746}
GUARD_IMPLEMENT_OLECREATE(CActionMorphDivideEx, "FiltersMain.MorphDivideEx",
0xb6e63343, 0x3ad3, 0x4c01, 0xb1, 0x3c, 0x4, 0x94, 0xb3, 0x42, 0xa7, 0x46);

// {BE795E55-FA69-4a55-9C81-DC80DC691045}
static const GUID guidMorphDivideEx = 
{ 0xbe795e55, 0xfa69, 0x4a55, { 0x9c, 0x81, 0xdc, 0x80, 0xdc, 0x69, 0x10, 0x45 } };

ACTION_INFO_FULL(CActionMorphDivideEx, IDS_ACTION_MORPH_DIVIDE_EX, IDS_MENU_FILTERS, IDS_TB_MAIN, guidMorphDivideEx);

ACTION_TARGET(CActionMorphDivideEx, szTargetAnydoc);

ACTION_ARG_LIST(CActionMorphDivideEx)
	ARG_INT(_T("Method"), 3)
	ARG_INT(_T("Algorythm"), 1)
	ARG_BINIMAGE(_T("BinImage"))
	RES_BINIMAGE(_T("BinResult"))
END_ACTION_ARG_LIST();

//////////////////////////////////////////////////////////////////////
//CActionMorphDivideEx implementation
CActionMorphDivideEx::CActionMorphDivideEx()
{
}

CActionMorphDivideEx::~CActionMorphDivideEx()
{
}

bool CActionMorphDivideEx::InvokeFilter()
{
	int	 nMethod = GetArgumentInt("Method");
	int nAlg = GetArgumentInt("Algorythm");
	nMethod = min(max(nMethod,1),3);
	BOOL bFast = GetValueInt(GetAppUnknown(), "MorphDivideExBin", "Fast");
	CBinImageWrp imgSrc(GetDataArgument());
	CBinImageWrp imgDst(GetDataResult());
	long cx = imgSrc.GetWidth(), cy = imgSrc.GetHeight();
	if( cy == 0 )
		return false;
	CPoint	ptOffset;
	imgSrc->GetOffset(&ptOffset);
    int	iRowMemSize = (cx+2+3)/4*4;
	CArray<byte,byte&> pur,pre;
	pur.SetSize(iRowMemSize*(cy+2));
	pre.SetSize(iRowMemSize*(cy+2));
	memset(pur.GetData(), 0, iRowMemSize*(cy+2));
	memset(pre.GetData(), 0, iRowMemSize*(cy+2));
	for (int i = 0; i < cy; i++)
	{
		byte *pRow;
		imgSrc->GetRow(&pRow, i, TRUE);
		//memcpy(pur.GetData()+(i+1)*iRowMemSize+1, pRow, cx);
		byte *p = pur.GetData()+(i+1)*iRowMemSize+1;
		for (int j = 0; j < cx; j++)
			p[j] = pRow[j] ? 255 : 0;
	}
	DoDivide(pur.GetData(),pre.GetData(),cx+2,cy+2,nMethod,nAlg,bFast);
	pur.RemoveAll();
	if (!imgDst.CreateNew(cx, cy))
		return false;
	imgDst->SetOffset(ptOffset, true);
	for (int i = 0; i < cy; i++)
	{
		byte *pRow;
		imgDst->GetRow(&pRow, i, TRUE);
		memcpy(pRow, pre.GetData()+(i+1)*iRowMemSize+1, cx);
	}
	return true;
}

void CActionUltimateErosion::DoDivide(byte *pur/*source binary*/, byte *pre/*result*/,
	int cx, int cy, int iOffset, int nModifiedAlg, BOOL bFast)
{
	WORD  CountObject=0;
	int Hx_max = cx;
	int Hy_max = cy;
    int	iRowMemSize = (cx+3)/4*4;
	int x,y,i,j,n;
	smart_alloc(pwork, WORD, cy*iRowMemSize+1);
	smart_alloc(pwork1, WORD, cy*iRowMemSize+1);
	ZeroMemory(pwork,(cy*iRowMemSize+1)*sizeof(WORD));
	ZeroMemory(pwork1,(cy*iRowMemSize+1)*sizeof(WORD));
	WORD *pworky;
    WORD *p_W;
	WORD *p_W1;
	WORD *p_Wd;
	byte *pury,*prey;

	int pd[10];
    pd[1]=-iRowMemSize; 
    pd[2]=1; 
    pd[3]= iRowMemSize;
    pd[4]=-1;
    pd[5]=-iRowMemSize+1;
    pd[6]= iRowMemSize+1;
    pd[7]= iRowMemSize-1;
    pd[8]=-iRowMemSize-1;

    int pd1[10];
    pd1[1]=-iRowMemSize;
    pd1[2]=-iRowMemSize+1;
    pd1[3]= 1;
    pd1[4]= iRowMemSize+1;
    pd1[5]= iRowMemSize;
    pd1[6]= iRowMemSize-1;
    pd1[7]=-1;
    pd1[8]=-iRowMemSize-1;
    pd1[9]=-iRowMemSize;
	StartNotification(8*cy+1700);
	// Make binary in pre and pwork
    for (y=1;y<cy-1;y++)
	{
		pury = pur+y*iRowMemSize+1;
		prey = pre+y*iRowMemSize+1;
		pworky = pwork+y*iRowMemSize+1;
		for (x=1;x<cx-1;x++)
		{
			if (*pury==255)
			{
				*pworky=65535;
				*prey=255;
			}
			pworky++;
			prey++;
			pury++;
		}
	}

	n=0;
	p_W=pwork;
	p_W1=pwork1;
	int NumI=1;
	WORD MaxCol=65535;

	smart_alloc(p_Count1, DWORD, cy*iRowMemSize/2);
	smart_alloc(p_Count2, DWORD, cy*iRowMemSize/2);
    DWORD Count1=0;
    DWORD Count2=0;
    DWORD Count3=0;
	DWORD Count_h=0;
	DWORD *q = p_Count1;
	DWORD *q1 = p_Count2;
	DWORD *qd=0;
	// 1. Make object contours in q
	for (y=1;y<cy-1;y++)
	{
	    pworky = p_W+y*iRowMemSize+1;
		for (x=1;x<cx-1;x++)
		{	
			if(*pworky==MaxCol)
				for(i=1;i<5;i++)
					if(*(pworky+pd[i])==0)
					{
						q[Count1++]=pworky-p_W;
						i=10;
					}
			pworky++;
		}
		Notify(y);
	}
	// Mark these contours
    for (j=0;j<(int)Count1;j++)
	{
		pworky = p_W + q[j];
		if (*pworky==MaxCol)
			*pworky=1;								
	}

	// 2. Preliminary erosion: in objects set each point's value as number of the operation
	CTimeTest tt1(true, "erosion1 in MorphDivideExBin");
	tt1.m_bEnableFileOutput = true;
    NumI=2;
	do
	{
       n=ErroStep1(p_W,p_W1,cx,cy,pd,NumI,MaxCol,q,q1,Count1,&Count2);
	   NumI++;
	   qd=q;
	   q=q1;
	   q1=qd;
	   Count_h=Count1;
	   Count1=Count2;
	   Count2=Count_h;
	   if (NumI>500) n=0;
		Notify(cy+NumI);
	}while(n);
	// ???
    for (y=0;y<cy;y++)
	{
		pworky = pwork+y*iRowMemSize;
		for (x=0;x<cx;x++)
		{
			if(*pworky==MaxCol)
			   *pworky=0;
			pworky++;
		}
	}
//char szBuff[256];
//sprintf(szBuff, "NumI = %d\n", NumI);
//OutputDebugString(szBuff);

	tt1.Stop();
	CTimeTest tt2(true, "erosion2 in MorphDivideExBin");
	tt2.m_bEnableFileOutput = true;
	// 3. Erosion on step 2
    if(iOffset>1) 
	{
       n=ErroX(iOffset,p_W,p_W1,cx,cy,pd,NumI,MaxCol,q,q1);
	   p_Wd=p_W;
	   p_W=p_W1;
       p_W1=p_Wd; 
	}
	else
	{
       n=ErroXLight(p_W,p_W1,cx,cy,pd,NumI,MaxCol,q,q1);
	   p_Wd=p_W;
	   p_W=p_W1;
       p_W1=p_Wd; 
	}
	Notify(cy+600);
	tt2.Stop();
	CTimeTest tt3(true, "find local maximums");
	tt3.m_bEnableFileOutput = true;

	// 4. Find local maximums (into Val1)
	smart_alloc(Val1, WORD, cy*iRowMemSize+1);
	ZeroMemory(Val1,(cy*iRowMemSize+1)*sizeof(WORD));
    WORD *Val1y;
	int a,k;
	for (y=1;y<cy-1;y++)
	{
		pworky = p_W+y*iRowMemSize+1;
		Val1y = Val1+y*iRowMemSize+1;
		for (x=1;x<cx-1;x++)
		{			
			a=*pworky;
			if (a!=0)
			{
					k=0;
				    for (i=1;i<9;i++)
					    if (*(pworky+pd1[i])>a) 
						{
                            k=1;
							i=10;
						}
					if (k==0)
						*Val1y=MaxCol;
			}

			pworky++;
			Val1y++;
		}
		Notify(cy+600+y);
	}
	// 5. Clear false maximims
	int iCount = 0;
	do
	{
		k=0;
	    for (y=1;y<cy-1;y++)
		{
			pworky = p_W+y*iRowMemSize+1;
			Val1y = Val1+y*iRowMemSize+1;
			for (x=1;x<cx-1;x++)
			{	
				if (*Val1y==MaxCol)
				{
					for(i=1;i<9;i++)
						if(*(pworky+pd[i])>=*pworky && *(Val1y+pd[i])==0)
						{
							i=10;
							*Val1y=0;
							k=1;
						}
				}
				Val1y++;
				pworky++;
			}
		}
		if (k==1)
		{
		    for (y=cy-2;y>0;y--)
			{
			    pworky = p_W+y*iRowMemSize+cx-2;
			    Val1y = Val1+y*iRowMemSize+cx-2;
			    for (x=cx-2;x>0;x--)
				{	
				    if (*Val1y==MaxCol)
					{
					    for(i=1;i<9;i++)
						    if(*(pworky+pd[i])>=*pworky && *(Val1y+pd[i])==0)
							{
							    i=10;
							    *Val1y=0;
							    k=1;
							}
					}
				    Val1y--;
				    pworky--;
				}
			}

		}
		iCount++;
		Notify(2*cy+600+max(iCount,500));
	}while(k);
	// 6. Clear local maximums on objects' borders
    for (y=1;y<cy-1;y++)
	{
		Val1y = Val1+y*iRowMemSize+1;
		pury = pur+y*iRowMemSize+1;
		for (x=1;x<cx-1;x++)
		{
			if(*Val1y==MaxCol)
				for(i=1;i<5;i++)
					if(*(pury+pd[i])==0)
					{
						*Val1y=0;
						i=10;
					}
			Val1y++;
			pury++;
		}
		Notify(2*cy+1100+y);
	}
	tt3.Stop();
	CTimeTest ttMO(true, "mark object in MorphDivideExBin");
	ttMO.m_bEnableFileOutput = true;
	// Now in Val1 REAL local maximums inside objects. Use it as objects.

	//AAM - отдаем результат Ultimate Erosion
	//AAM - заодно добьемся слияния одиночных точек на картинке
	for (y=1;y<cy-1;y++)
	{
		prey = pre+y*iRowMemSize+1;
		pury = pur+y*iRowMemSize+1;
		Val1y = Val1+y*iRowMemSize+1;
		WORD *Val1y_m = Val1+(y-1)*iRowMemSize+1;
		WORD *Val1y_p = Val1+(y+1)*iRowMemSize+1;
		for (x=1;x<cx-1;x++)
		{
			if(*Val1y) *prey = 255;
			else
			{
				if( (*Val1y_m || *Val1y_p) && (Val1y[-1] || Val1y[1]) ) *prey = *pury;
				else *prey = 0;
			}
			prey++;
			pury++;
			Val1y++;
			Val1y_p++;
			Val1y_m++;
		}
	} 
    FinishNotification();
}



IMPLEMENT_DYNCREATE(CActionUltimateErosion, CCmdTargetEx);

GUARD_IMPLEMENT_OLECREATE(CActionUltimateErosion, "FiltersMain.UltimateErosion",
0x8be30b85, 0xe471, 0x4947, 0xa6, 0x1b, 0xe0, 0x7d, 0x93, 0x1b, 0xf5, 0x53);

static const GUID guidUltimateErosion = 
{ 0xe8fc1cbe, 0xfc07, 0x4335, { 0x8e, 0x35, 0xdf, 0xdc, 0x56, 0xd1, 0x66, 0x47 } };

ACTION_INFO_FULL(CActionUltimateErosion, IDS_ACTION_ULTIMATEEROSION, IDS_MENU_FILTERS, IDS_TB_MAIN, guidUltimateErosion);

ACTION_TARGET(CActionUltimateErosion, szTargetAnydoc);

ACTION_ARG_LIST(CActionUltimateErosion)
	ARG_INT(_T("Method"), 3)
	ARG_INT(_T("Algorythm"), 1)
	ARG_BINIMAGE(_T("BinImage"))
	RES_BINIMAGE(_T("BinResult"))
END_ACTION_ARG_LIST();

//////////////////////////////////////////////////////////////////////
//CActionUltimateErosion implementation
CActionUltimateErosion::CActionUltimateErosion()
{
}

CActionUltimateErosion::~CActionUltimateErosion()
{
}

bool CActionUltimateErosion::InvokeFilter()
{
	int	 nMethod = GetArgumentInt("Method");
	int nAlg = GetArgumentInt("Algorythm");
	nMethod = min(max(nMethod,1),3);
	BOOL bFast = GetValueInt(GetAppUnknown(), "MorphDivideExBin", "Fast");
	CBinImageWrp imgSrc(GetDataArgument());
	CBinImageWrp imgDst(GetDataResult());
	long cx = imgSrc.GetWidth(), cy = imgSrc.GetHeight();
	if( cy == 0 )
		return false;
	CPoint	ptOffset;
	imgSrc->GetOffset(&ptOffset);
    int	iRowMemSize = (cx+2+3)/4*4;
	CArray<byte,byte&> pur,pre;
	pur.SetSize(iRowMemSize*(cy+2));
	pre.SetSize(iRowMemSize*(cy+2));
	memset(pur.GetData(), 0, iRowMemSize*(cy+2));
	memset(pre.GetData(), 0, iRowMemSize*(cy+2));
	for (int i = 0; i < cy; i++)
	{
		byte *pRow;
		imgSrc->GetRow(&pRow, i, TRUE);
		//memcpy(pur.GetData()+(i+1)*iRowMemSize+1, pRow, cx);
		byte *p = pur.GetData()+(i+1)*iRowMemSize+1;
		for (int j = 0; j < cx; j++)
			p[j] = pRow[j] ? 255 : 0;
	}
	DoDivide(pur.GetData(),pre.GetData(),cx+2,cy+2,nMethod,nAlg,bFast);
	pur.RemoveAll();
	if (!imgDst.CreateNew(cx, cy))
		return false;
	imgDst->SetOffset(ptOffset, true);
	for (int i = 0; i < cy; i++)
	{
		byte *pRow;
		imgDst->GetRow(&pRow, i, TRUE);
		memcpy(pRow, pre.GetData()+(i+1)*iRowMemSize+1, cx);
	}
	return true;
}

