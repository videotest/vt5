#include "stdafx.h"
#include "Test1Func.h"
#include "math.h"

#define ADDALLOC 0

//#define dbg_assert(x) {if(!(x)) {int i=0; i=1/i;}};

CTest1Func::CTest1Func()
{
	m_bWasFatalError=0;

	m_iOffset = 0;
    mflag = 1;
	iConst = 0;

	CountObject=0;
	Val1=0;
	p_Count1=0;
	p_Count2=0;
	p_ObjColor=0;
	pwork=0;
	pwork1=0;
	SizeObjCount=0;
	SizeObjCount_h=0;
    p_p_ObjCount=0;

}
  

BYTE CTest1Func::Erro(WORD *pwork,WORD *pwork1,int cx,int cy,int *pd,WORD NumI,WORD MaxCol
					  ,DWORD *q,DWORD *q1,DWORD Count1,DWORD *Count2)
{

	BYTE flag=0;
	WORD *pworky;
	int	iRowMemSize = cx;
	int i,j,n;
	*Count2=0;

	int M_i;

	if (NumI-((int)((int)NumI/5))*5==1 ||  //NumI%5==1
		NumI-((int)((int)NumI/5))*5==3 )   //NumI%5==3
		M_i=9;
	else 
		M_i=5;
	//почему-то иногда по 4 соседним точкам, а иногда по 9 - пнуть семена

	for (j=0;j<(int)Count1;j++)	//по переданному контуру
	{
		pworky = pwork + q[j]; //очередная точка контура
		for (i=1;i<M_i;i++)
		{
			n=pd[i];
		    if (*(pworky+n)==MaxCol) //если точка, соседняя с контуром, залита
			{
				*(pworky+n)=NumI; //пометим ее
				q1[*Count2]=pworky+n-pwork; //и поместим во второй список
				(*Count2)++;
				flag=1;
			}
		}
	}
	//фактически получили контур внутри нашего исходного
	dbg_assert(int(*Count2)<=max(cy*cx/2,1000));


	return flag; //пометили хоть одну точку?
}


BYTE CTest1Func::ErroX(WORD *pwork,WORD *pwork1,int cx,int cy,int *pd,WORD NumI,WORD MaxCol
					    ,DWORD *q,DWORD *q1)
{	int porog1,porog2;
 	int koff; 

	if (m_iOffset==2)
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
	int Mask,Maskx1,Maskx2,Masky1,Masky2;
	int	iRowMemSize = cx;
	int x,y,i,m_min,m_max,x1,y1;

	int M_i,Count;
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
			{
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
			{
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
			{
//				if(*pworky>30)
//					Mask=Mask;
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

	WORD *pwork2 = new WORD[cy*iRowMemSize+1+ADDALLOC];
	ZeroMemory(pwork2 ,sizeof(WORD)*(cy*iRowMemSize+1+ADDALLOC));
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


//	if (Count>11) flag=0;

	}while(flag);

	if (p_W!=pwork1)
		CopyMemory(pwork1,pwork2,cy*iRowMemSize*sizeof(WORD));


    delete pwork2; 
	return flag;
}



BYTE CTest1Func::ErroXLight(WORD *pwork,WORD *pwork1,int cx,int cy,int *pd,WORD NumI,WORD MaxCol
					    ,DWORD *q,DWORD *q1)
{
	int porog1=5;
	int porog2=30;

	WORD *p_Wy;
	WORD *p_W1y;
	WORD p1,p2,p3,p4;

	BYTE flag=0;
	WORD *pworky;
	WORD *pwork1y;
	int	iRowMemSize = cx;
	int x,y,i,m_min,m_max;

	int M_i,Count;
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
				      m_min = max(max(min(p1,p2),min(p2,p3)),
					 	          max(min(p3,p4),min(p4,p1)));
				//для 4 крайних слоев - 3-е из 4 соседей
				if (*pworky>=porog1 && *pworky<porog2)
					m_min = max(max(min(p1,min(p2,p3)),min(p2,min(p3,p4))),
						        max(min(p3,min(p4,p1)),min(p4,min(p1,p2))));
				//от 5 до 29 слоя - 2-е из 4 соседей
				if (*pworky>=porog2)
					m_min = min(min(p1,p2),min(p3,p4));
				//для совсем внутренних областей - наименьшее из 4 соседей

				if (m_min>0 && m_min<*pworky)
				    *pwork1y=m_min;
				//если надо - уменьшим до выбранного соседа
			}
			pworky++;
			pwork1y++;
		}
	} //идиотизм какой-то... нафига? Спросить семена
	// теперь pwork1[] <= pwork[], взят из соседних точек

	WORD *pwork2 = new WORD[cy*iRowMemSize+1+ADDALLOC];
	ZeroMemory(pwork2 ,sizeof(WORD)*(cy*iRowMemSize+1+ADDALLOC));
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
					//p_work2[] - дилатация p_work1, но не больше p_work
				}
				pworky++;
				p_Wy++;
				p_W1y++;
			}
			
		} //смутно похоже на Close by Reconstruction - точнее, на реконструкцию
		
		{WORD *p_Wd=p_W; p_W=p_W1; p_W1=p_Wd;}		
		
		//	if (Count>11) flag=0;
		
	}while(flag);
	
	if (p_W!=pwork1)
		CopyMemory(pwork1,pwork2,cy*iRowMemSize*sizeof(WORD));
	//отдаем результат всегда в pwork1
	//кстати, похоже, теряем последний шаг реконструкции
	
    delete pwork2; 
	return flag; //false
}


BYTE CTest1Func::DilaX(WORD *pwork,WORD *pwork1,int cx,int cy,int *pd,WORD MaxCol,BYTE *pres)
{
    BYTE *presy;

	BYTE flag=0;
	WORD *pworky;
	WORD *pwork1y;
	int	iRowMemSize = cx;
	int x,y,i;
	WORD a1,a,m_max;

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


BYTE CTest1Func::Dila(WORD *pwork,WORD *pwork1,int cx,int cy,int *pd,int *pd1,
					  DWORD *q,DWORD *q1,DWORD Count,DWORD *Count1,BYTE *pre,WORD *pimg,int NumI)
{
	BYTE flag=0;
	BYTE flag1=0;
	BYTE flag2=0;
	int i,j,n; 
	WORD a;
	WORD *pworky;
	BYTE *prey;
	WORD *pimgy;
	int	iRowMemSize = cx;
	*Count1=0;
	WORD NCol=0;
	if (NumI>2)
		NCol=(WORD)(NumI-2);
	
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
				{
					if(*(pimgy+n)>NCol)
					{
						*(pworky+n)=a;
						q1[*Count1]=pworky+n-pwork;
						(*Count1)++;
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
			}
		}
	}
	
	return flag;
}


BYTE CTest1Func::Del_L(BYTE *pres,int cx,int cy,int *pd,int *pd1)
{
	int x,y,i,a1,a2,k,n;
	BYTE *presy;
	BYTE *presy1;
	int	iRowMemSize = cx;

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

void CTest1Func::DrawLine(DWORD Pos1,DWORD Pos2,BYTE* pre,int cx,int cy)
//провести линию, заполняя 1
{
	*(pre+Pos1)=1;
	*(pre+Pos2)=1;

	int	iRowMemSize = cx;
	int x,x1,x2,y,y1,y2,x_,y_;
	y1=Pos1/iRowMemSize;
	x1=Pos1-iRowMemSize*y1;
	y2=Pos2/iRowMemSize;
	x2=Pos2-iRowMemSize*y2;

	double rad = _hypot(x2-x1,y2-y1);

    if (rad>0)
	{
        double kx = (double)(x2-x1)/rad;
        double ky = (double)(y2-y1)/rad;
        int i;
		y_=y1;
		x_=x1;

        for (i=1;i<=(int)rad;i++)
		{
			y=y1+(int)(ky*i);
			x=x1+(int)(kx*i);
			*(pre+y*iRowMemSize+x)=1;
//			if(x!=x_ && y!=y_)
//				*(pre+y*iRowMemSize+x_)=1;
			x_=x;
			y_=y;
		}

	}

}

int MAXCOUNT=100;

void CTest1Func::DrawDivLine(WORD *pwork,BYTE *pre,int cx,int cy,int *pd,int *pd1,
							 _point *p1,_point *p2,int *Count)
{
	m_bWasFatalError=0;
	*Count=0;
	int x,y,i,a1,a2,k,n,tz;
	DWORD Pos1,Pos2;
	WORD *pworky;
    WORD *pworky1;
	BYTE *prey;
	int	iRowMemSize = cx;
//     Skel

	for (y=1;y<cy-1;y++)
	{
		pworky = pwork+y*iRowMemSize+1;
		for (x=1;x<cx-1;x++)
		{
			if(*pworky==1) //граница двух зерен
			{
				k=0;
				n=0;
                a1=*(pworky+pd1[1]);
				for(i=2;i<10;i++)
				{					
					a2=*(pworky+pd1[i]);
					if (a1==1)
					{
						n++;
						if (a2!=1) 
							k++;
					}
					a1=a2;
				} //k - число раздельных областей вблизи точки
				if (k==1 && n>2) //точка не разделяет области, но не является концевой
				{
					*pworky=255;
				}
			}
			pworky++;
		}
	} //линии были толщиной 2, теперь - толщиной 1



// Draw Line
	for (y=1;y<cy-1;y++)
	{
		prey = pre+y*iRowMemSize+1;
		pworky = pwork+y*iRowMemSize+1;
		for (x=1;x<cx-1;x++)
		{
			if(*pworky==1) //граница зерен
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
				} //k - число раздельных областей вокруг точки
				if (k==1)// && n==1) //конец линии
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
						 } //ползем по линии на шажок
						 if (k==1) //удалось сделать шаг?
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
							} //n - число разделенных точкой зерен, tz - рядом свободное место
							if (n<2 && tz==0)
								*pworky1=255; //если точка конечная - затираем
							else //дошли до пересечения границ или до пустоты
							{
								Pos2=pworky1-pwork;
            					DrawLine(Pos1,Pos2,pre,cx,cy); //спрямим полученный отрезок
								int xx1=Pos1%iRowMemSize, yy1=Pos1/iRowMemSize;
								int xx2=Pos2%iRowMemSize, yy2=Pos2/iRowMemSize;
								//if(max(abs(xx2-xx1),abs(yy2-yy1))>=2)
								if (Pos1!=Pos2)
								{
									if(*Count>=MAXCOUNT)
									{ //защита от ошибок
										m_bWasFatalError=1;
										return;
									}
									dbg_assert(*Count<MAXCOUNT);
									if(*Count<MAXCOUNT)
									{ //не дадим спискам переполниться
										p1[*Count].y=Pos1/cx;
										p1[*Count].x=Pos1-p1[*Count].y*cx;
										p2[*Count].y=Pos2/cx;
										p2[*Count].x=Pos2-p2[*Count].y*cx;
										(*Count)++;
									}
									
								} //запомним отрезок в списке
								Pos1=pworky1-pwork;
  
								*pworky1=255;
//								k=0;
							}
						 }

					}while(k); //пока не доползли до конца линии
					Pos2=pworky1-pwork;
					DrawLine(Pos1,Pos2,pre,cx,cy);
					if (Pos1!=Pos2)
					{
						if(*Count>=MAXCOUNT)
						{ //защита от ошибок
							m_bWasFatalError=1;
							return;
						}
						dbg_assert(*Count<MAXCOUNT);
						if(*Count<MAXCOUNT)
						{ //не дадим спискам переполниться
							p1[*Count].y=Pos1/cx;
							p1[*Count].x=Pos1-p1[*Count].y*cx;
							p2[*Count].y=Pos2/cx;
							p2[*Count].x=Pos2-p2[*Count].y*cx;
							(*Count)++;
						}
					} //внесли в список последний отрезок
				}
				else
					if (k==0 )//&& n==1) //внутренняя точка??? их не должно быть, затрем ее
					{
						*pworky=255;
						*prey=1;
					}

			}
			pworky++;
			prey++;
		}
	}

	for (y=1;y<cy-1;y++) // внесем в изображение неохваченные (одиночные?) точки
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

void CTest1Func::DoWork( BYTE *pur,int cx,int cy,BYTE *pre,
						_point *pP1,_point *pP2,int *CountP,int *width)
{
//////      Zero  Val
	CountObject=0;
	Val1=0;
	p_Count1=0;
	p_Count2=0;
	p_ObjColor=0;
	pwork=0;
	pwork1=0;
	SizeObjCount=0;
	SizeObjCount_h=0;
    p_p_ObjCount=0;
/////////////  end zero val 

    int	iRowMemSize = cx;

	byte	*pury;
	byte	*prey;

	ZeroMemory(pre,cy*iRowMemSize);

	int x,y,i,j,n;
	
	pwork = new WORD[cy*iRowMemSize+1+ADDALLOC];
	pwork1 = new WORD[cy*iRowMemSize+1+ADDALLOC];
	ZeroMemory(pwork,(cy*iRowMemSize+1+ADDALLOC)*sizeof(WORD));
	ZeroMemory(pwork1,(cy*iRowMemSize+1+ADDALLOC)*sizeof(WORD));
	WORD *pworky;
    WORD *p_W;
	WORD *p_W1;
	WORD *p_Wd;


    pd[1]=-iRowMemSize;
    pd[2]=1;
    pd[3]= iRowMemSize;
    pd[4]=-1;

    pd[5]=-iRowMemSize+1;
    pd[6]= iRowMemSize+1;
    pd[7]= iRowMemSize-1;
    pd[8]=-iRowMemSize-1;

    pd1[1]=-iRowMemSize;
    pd1[2]=-iRowMemSize+1;
    pd1[3]= 1;
    pd1[4]= iRowMemSize+1;
    pd1[5]= iRowMemSize;
    pd1[6]= iRowMemSize-1;
    pd1[7]=-1;
    pd1[8]=-iRowMemSize-1;
    pd1[9]=-iRowMemSize;

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
	} //внутреннюю область pwork и pre заполнили в соответствии с маской pur

	n=0;
	p_W=pwork;
	p_W1=pwork1;
	int NumI=1;
	WORD MaxCol=65535;

	p_Count1=new DWORD[max(cy*iRowMemSize/2,10000)+ADDALLOC];
	p_Count2=new DWORD[max(cy*iRowMemSize/2,10000)+ADDALLOC];
    DWORD Count1=0;
    DWORD Count2=0;
	DWORD *q = p_Count1;
	DWORD *q1 = p_Count2;

	//  Заполнение объектов с контурами

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
	}
	//все пустые точки, прилегающие к маске (наружный контур),
	//вносим в список q; (исключая края изображения)
	dbg_assert(int(Count1)<=max(cy*iRowMemSize/2,1000));

    for (j=0;j<(int)Count1;j++)
	{
		pworky = p_W + q[j];
		if (*pworky==MaxCol)
			*pworky=1;								
	}
	//контура забиваем 1


	{
		NumI=2;
		do
		{
			n=Erro(p_W,p_W1,cx,cy,pd,NumI,MaxCol,q,q1,Count1,&Count2);
			//получили контур внутри исходного, эрродировали изображение
			NumI++;

			{DWORD *qd=q; q=q1; q1=qd;}
			{DWORD Count_h=Count1; Count1=Count2; Count2=Count_h;}
			//поменяли контура местами - теперь q,Count1 - внутренний
			
			if (NumI>500) n=0;
			//	   if (NumI>m_iOffset) n=0; 
		}while(n);
	}
	//сейчас изображение залито:
	//1 - наружний контур, дальше - по расстоянию от границы.
	//снаружи - 0
	//можно сделать проще и быстрее - см. CAutoAdjustBin (ChromSegm)

	*width=NumI; //максимальная "толщина" объекта

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
	//то, что не залили от контура - забьем 0, чтоб не мешало

	m_iOffset=1;

	{
		if(m_iOffset>1) 
		{
			n=ErroX(p_W,p_W1,cx,cy,pd,NumI,MaxCol,q,q1);
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
	} //подчистили (?) картинку


////////////////////////////////////////////
	Val1 = new WORD[cy*iRowMemSize+1+ADDALLOC];
	ZeroMemory(Val1,(cy*iRowMemSize+1+ADDALLOC)*sizeof(WORD));
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
					if (*(pworky+pd1[i])>a) //упростить
					{
						k=1;
						i=10;
					}
					if (k==0)
						*Val1y=MaxCol;
			}
			//точки, максимально удаленные от краев,
			//(локальный максимум расстояния)
			//помечаем в Val1 как 65535
			
			pworky++;
			Val1y++;
		}
	}
	
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
						//если рядом есть точка с бОльшим расстоянием,
						//не помеченная как максимум - сбросим текущую.
				}
				Val1y++;
				pworky++;
			}
		}
		
		if (k==1)
		{
			for (y=cy-2;y>0;y--) //пробежим в обратную сторону
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
						//если рядом есть точка с бОльшим расстоянием,
						//не помеченная как максимум - сбросим текущую.
					}
					Val1y--;
					pworky--;
				}
			}
			
		}
	}while(k);
	//остались только истинные максимумы
	
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
	}
	//сбросили максимумы, вплотную прилегающие к границе
	
	ZeroMemory(p_Count1,(cy*iRowMemSize/5)*sizeof(DWORD));
	ZeroMemory(p_Count2,(cy*iRowMemSize/5)*sizeof(DWORD));
	p_ObjColor=new WORD[10000+ADDALLOC];
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
				p_ObjColor[CountObject]=*pworky; //толщина объекта
				CountObject++;
				*Val1y=CountObject; //номер объекта
				Count1=1;
				p_Count1[0]=Val1y-Val1;
				do // захватим все соседние точки, где Val1[]=65535
				{
					Count2=0;
					for (i=0;i<(int)Count1;i++) //по всем объектикам
						for (j=1;j<9;j++)
							if(*(Val1+p_Count1[i]+pd[j])==MaxCol)
							{
								p_Count2[Count2] = p_Count1[i]+pd[j];
								*(Val1+p_Count2[Count2])=CountObject;
								Count2++;
							} //расползлись на 1 точку, включив результат в список p_Count2
							Count1=Count2;
							CopyMemory(p_Count1,p_Count2,Count2*sizeof(DWORD));
				}while(Count2);
				
			}
			Val1y++;
			pworky++;
		}
	}
	//сейчас центры - одна или соседних несколько точек с одинаковым Val1[]
	
	SizeObjCount = new WORD[CountObject];
	SizeObjCount_h = new WORD[CountObject];
	p_p_ObjCount = new DWORD*[CountObject];
	for (i=0;i<CountObject;i++)
	{
		p_p_ObjCount[i]=0;
		SizeObjCount[i]=0;
		SizeObjCount_h[i]=0;
	}
	
	//   Расчет размера контуров объектов
	for (y=1;y<cy-1;y++)
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
	//SizeObjCount[i] - число пустых точек, прилегающих к зародышу i

	for (i=0;i<CountObject;i++)
		p_p_ObjCount[i]=new DWORD[SizeObjCount[i]];
	
	
	//   Расчет контуров объектов

	for (y=1;y<cy-1;y++)
	{
		Val1y = Val1+y*iRowMemSize+1;
		for (x=1;x<cx-1;x++)
		{
			if (*Val1y!=0)
			{
				for (i=1;i<5;i++)
				{
					if(*(Val1y+pd[i])==0)
					{
						p_p_ObjCount[(*Val1y)-1][SizeObjCount_h[(*Val1y)-1]++]=
							Val1y - Val1;
						i=10;
					}
				}
			}
			Val1y++;
		}
	} //внесли пустые точки, прилегающие к зародышам, в списки p_p_ObjCount
	
	//   Расширение
	if (p_W!=pwork1)
		CopyMemory(pwork1,pwork,cy*iRowMemSize*sizeof(WORD));
	q = p_Count1;
	q1 = p_Count2;
	int NumI1=1;

	CopyMemory(pwork,Val1,cy*iRowMemSize*sizeof(WORD));

	p_W=pwork;
	p_W1=pwork;
	int NColor=0;

	for (i=0;i<CountObject;i++)
	{
		if (NColor<p_ObjColor[i]) NColor=p_ObjColor[i];
	}
	//NColor - максимальная толщина объекта

	Count1=0;
	Count2=0;

	do
	{
		if (NColor>0)
		{
			for (i=0;i<CountObject;i++)
			{
				if (p_ObjColor[i]==NColor)
				{ //если объект максимальной толщины - внесем его зародыш в список
					for (j=0;j<SizeObjCount[i];j++) 
					{
						q[Count1++]=p_p_ObjCount[i][j];
					}
				}
			}
		}
		
		k=Dila(p_W,p_W1,cx,cy,pd,pd1,q,q1,Count1,&Count2,pre,pwork1,NColor);
		NumI1++;
		if (k==0)
			NColor--;
		if (Count2>0)
			k=1;
		
		{DWORD *qd=q; q=q1; q1=qd;}
		{DWORD Count_h=Count1; Count1=Count2; Count2=Count_h;}
		
		if (NumI1>20*NumI)
			k=0;
		//			if (NumI1>m_iOffset) k=0;
	}while(k);
	//короче говоря, ядра разрастаются, пока можно.


	//		delete CT;

	//  рисование линий
	for(y=1;y<cy-1;y++)
	{
		pworky = pwork+y*iRowMemSize+1;
		for(x=1;x<cx-1;x++)
		{
			for(i=1;i<5;i++)
				if(*pworky!=*(pworky+pd[i]) && (*pworky) && (*(pworky+pd[i]))>0)
					*pworky=0;
				pworky++;
		}
	} //удалили все точки на границах между зернами
	
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
	} //заполнили объект - теперь он из кусочков, границы зерен =1
	
	CopyMemory(pre,pur,cy*iRowMemSize);

	DrawDivLine(pwork,pre,cx,cy,pd,pd1,pP1,pP2,CountP);
	//спрямить все разделительные линии, внести отрезки в список
	if(m_bWasFatalError)
	{
		*CountP=0;
		return;
	}
	
	//  	Clear
	int p1,p2,p3,p4;
	for (y=1;y<cy-1;y++)
	{
		prey = pre+y*iRowMemSize+1;
		for (x=1;x<cx-1;x++)
		{
			if (*prey==255)	//зерно
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
			//если рядом две соседние точки 0 и 1 (граница ядер и пустота)
			//или 1 и 1 - подставили 2
			//(скругление уголков, что ли? и диагональные линии в сплошные)
			prey++;
		}
	} 
	
	for (y=0;y<cy;y++)
	{
		prey = pre+y*iRowMemSize;
		for (x=0;x<cx;x++)
		{
			if (*prey<3)
				*prey=0;
			prey++;
		}
	} //затрем все линии пустотой


	////////   Удаление малых контактов 
	for (y=0;y<cy-1;y++)
	{
		prey = pre+y*iRowMemSize;
		for (x=0;x<cx-1;x++)
		{
			//если два зерна касаются по диагонали - удалим 1 пиксел
			if (*prey==255)
			{
				if (*(prey+iRowMemSize+1)==255)
				{
					if (*(prey+1)==0)
					{
						if (*(prey+iRowMemSize)==0)
						{
							*prey=0;
						}
					}
				}
			}

			if (*prey==0)
			{
				if (*(prey+iRowMemSize+1)==0)
				{
					if (*(prey+1)==255)
					{
						if (*(prey+iRowMemSize)==255)
						{
							*(prey+1)=0;
						}
					}
				}
			}
			
			prey++;
		}
	} 
	
	
	
	//////////////////////////////////////////////////////////////////
	
	//   if (p_W!=pwork)
	//		CopyMemory(pwork,pwork1,cy*iRowMemSize*sizeof(WORD));
	//	if (p_W!=pwork)
	//        CopyMemory(pwork,Val1,cy*iRowMemSize*sizeof(WORD));
	
	/*    for (y=0;y<cy;y++)
	{
	prey = pre+y*iRowMemSize;
	pworky = pwork+y*iRowMemSize;
	for (x=0;x<cx;x++)
	{
	//			if(*pworky<255)
	//			   *prey=*pworky;
	//			else
	//				*prey=255;
	
	  if(*pworky>1)
	  *prey=255;
	  else
	  *prey=0;
	  
		
		  prey++;
		  pworky++;
		  }
		  }
	*/
	
	delete Val1; Val1=0;
	delete p_Count1; p_Count1=0;
	delete p_Count2; p_Count2=0;
	delete p_ObjColor; p_ObjColor=0;
	delete pwork; pwork=0;
	delete pwork1; pwork1=0;
	delete SizeObjCount; SizeObjCount=0;
	delete SizeObjCount_h; SizeObjCount_h=0;
	for (i=0;i<CountObject;i++)
	{
		if(p_p_ObjCount[i]!=0)
		{
			delete p_p_ObjCount[i];
			p_p_ObjCount[i]=0;
		}
	}
	delete p_p_ObjCount;
	p_p_ObjCount=0;
}

CTest1Func::~CTest1Func()
{
	if (Val1!=0) delete Val1;
	if (p_Count1!=0) delete p_Count1;
	if (p_Count2!=0) delete p_Count2;
	if (p_ObjColor!=0) delete p_ObjColor;
	if (pwork!=0) delete pwork;
	if (pwork1!=0) delete pwork1;
	if (SizeObjCount!=0) delete SizeObjCount;
	if (SizeObjCount_h!=0) delete SizeObjCount_h;
	if (p_p_ObjCount!=0) 
	{
		for (int i=0;i<CountObject;i++)
		{
			if(p_p_ObjCount[i]!=0) delete p_p_ObjCount[i];
		}
		delete p_p_ObjCount;
	}
}