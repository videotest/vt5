// DivFunc.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "Math.h"
#include "DivFunc.h"
#include "Image.h"
#include "malloc.h"
#include "MorphInfo.h"

BOOL ChangePercent(int ip)
{
	return TRUE;
}

void Reset()
{
}

/*****************************
	Erosion scan N 1
******************************/
BOOL Eqs1(WORD * hptr,int bpl,int x,WORD bFill)
{
 char i,j;
 WORD * hptrt=hptr;
 // 1 1 1
 // 1 T 1
 // 1 1 1
  for (j=0;j<=2;j++)
  {
   for (i=0;i<=2;i++)
   {
    if ((i==1)&&(j==1)) continue;
    if ((hptr[x+i]==0)||(hptr[x+i]==bFill-1))
	{
	hptrt[bpl+x+1]=bFill;
	return (TRUE);
	}
   }
   hptr+=bpl;
   }
  return (FALSE);
}

/*****************************
	Erosion scan N 2
******************************/
BOOL Eqs2 (WORD * hptr,int bpl,int x,WORD bFill)
{
//   1
// 1 T 1
//   1
    if ((hptr[x+1]==0)||(hptr[x+1]==bFill-1)||
	(hptr[bpl+x]==0)||(hptr[bpl+x]==bFill-1)||
	(hptr[bpl+x+2]==0)||(hptr[bpl+x+2]==bFill-1)||
	(hptr[(bpl<<1)+x+1]==0)||(hptr[(bpl<<1)+x+1]==bFill-1))
	{
	hptr[bpl+x+1]=bFill;
	return (TRUE);
	}
	else
  return (FALSE);
}

/**************************
	Erosion MAIN
***************************/
BOOL Erosion (WORD * hptr,int X,int Y,WORD bFill)
{
 BOOL a=FALSE,b=FALSE;
 int x,y,bpl8=((X+3)&~3);
 BOOL  (* func)(WORD *,int,int,WORD);

 if (bFill%2==1) func=Eqs2;
	    else func=Eqs1;
  for (y=0;y<Y-2;y++)
  {
   for (x=0;x<X-2;x++)
   {
    if (hptr[bpl8+x+1]!=1) continue;
    a=func(hptr,bpl8,x,bFill);
    if (!b&&a) b=TRUE;
   }
   hptr+=bpl8;
  }
  return b;
}

/*****************************
	Find Minimum
*****************************/
void FindMin (WORD * hptr,WORD * _hptr,int bpl,int x,WORD bTB)
{
 char i,j;

  for (j=0;j<=2;j++)
  {
   for (i=0;i<=2;i++)
   if (hptr[x+i]>bTB) return;
  hptr+=bpl;
 }
_hptr[bpl+x+1]=bTB;
}

/**************************************
		Find erodants
**************************************/
void Erodants(WORD *hptr,WORD *_hptr,int X,int Y)
{
 int x,y,bpl8=((X+3)&~3);
 WORD bTB;

  for (y=0;y<Y-2;y++)
  {
   for (x=0;x<X-2;x++)
   {
    if ((bTB=hptr[bpl8+x+1])==0) continue;
    FindMin(hptr,_hptr,bpl8,x,bTB);
   }
   hptr+=bpl8;
  _hptr+=bpl8;
  }
}

/*********************************
	Test image for clean
**********************************/
void TestImage(WORD *Er_ptr,WORD *Ring_ptr,int bpl8,int x,WORD bTB)
{
 char i,j;

for (j=0;j<=2;j++)
 for (i=0;i<=2;i++)
 {
  if ((i==1)&&(j==1)) continue;
  if (Er_ptr[j*bpl8+x+i]==255)
  {
  Er_ptr[bpl8+x+1]=255;
  return;
  }
 }
 Ring_ptr-=bpl8<<1;
for (j=-2;j<=4;j++)
{
 for (i=-2;i<=4;i++)
 {
  if (Ring_ptr[x+i]>bTB)
  {
  Er_ptr[bpl8+x+1]=255;
  return;
  }
 }
 Ring_ptr+=bpl8;
 }
}

/************************************
	Clean image forward
************************************/
void CleanImageForward(WORD *hptr1,WORD *hptr2,int X,int Y)
{
int x,y,bpl8=((X+3)&~3);
WORD bTB;

hptr1+=bpl8<<1;
hptr2+=bpl8<<1;
for (y=2;y<Y-4;y++)
{
 for (x=2;x<X-4;x++)
 {
 if ((bTB=hptr1[bpl8+x+1])==0) continue;
 TestImage(hptr1,hptr2,bpl8,x,bTB);
 }
hptr1+=bpl8;
hptr2+=bpl8;
}
}

/************************************
	Clean image back
************************************/
void CleanImageBack(WORD *hptr1,WORD *hptr2,int X,int Y)
{
int x,y,bpl8=((X+3)&~3);
WORD bTB;

hptr1+=(DWORD)(Y-5)*bpl8;
hptr2+=(DWORD)(Y-5)*bpl8;
for (y=Y-5;y>=2;y--)
{
 for (x=X-5;x>=2;x--)
 {
 if ((bTB=hptr1[bpl8+x+1])==0) continue;
 TestImage(hptr1,hptr2,bpl8,x,bTB);
 }
hptr1-=bpl8;
hptr2-=bpl8;
}
}

/***********************************
	Dilation Main
***********************************/
BOOL Dilation (WORD * hptr8,WORD * xhptr8,int X,int Y,WORD bFill)
{
int x,y,bpl8=((X+3)&~3),bpl8_2=((X+3)&~3)<<1;
BOOL a=FALSE;
  for (y=0;y<Y-2;y++)
  {
   for (x=0;x<X-2;x++)
  {
   if (xhptr8[bpl8+x+1]==0) continue;
   if (hptr8[bpl8+x+1]!=0) continue;
   if (hptr8[x]>=bFill)
   {
   // 1 1 1
   //	T
   // 0 0 0
   if ((hptr8[x+1]>=bFill)&&
       (hptr8[x+2]>=bFill)&&
       (!hptr8[bpl8_2+x])&&(!hptr8[bpl8_2+x+1])&&(!hptr8[bpl8_2+x+2]))
       goto Set;
   // 1   0
   // 1 T 0
   // 1   0
   if ((hptr8[bpl8+x]>=bFill)&&
       (hptr8[bpl8_2+x]>=bFill)&&
       (!hptr8[x+2])&&(!hptr8[bpl8+x+2])&&(!hptr8[bpl8_2+x+2]))
       goto Set;
   // 1 1
   // 1 T 0
   //	0 0
   if ((hptr8[x+1]>=bFill)&&
       (hptr8[bpl8+x]>=bFill)&&
       (!hptr8[bpl8+x+2])&&(!hptr8[bpl8_2+x+1])&&(!hptr8[bpl8_2+x+2]))
       goto Set;
       continue;
   }

   if (hptr8[x+2]>=bFill)
   {
  // 0	 1
  // 0 T 1
  // 0	 1
   if ((hptr8[bpl8+x+2]>=bFill)&&
       (hptr8[bpl8_2+x+2]>=bFill)&&
       (!hptr8[x])&&(!hptr8[bpl8+x])&&(!hptr8[bpl8_2+x]))
       goto Set;
   //	1 1
   // 0 T 1
   // 0 0
   if ((hptr8[x+1]>=bFill)&&
       (hptr8[bpl8+x+2]>=bFill)&&
       (!hptr8[bpl8+x])&&(!hptr8[bpl8_2+x])&&(!hptr8[bpl8_2+x+1]))
       goto Set;
       continue;
  }
  if (hptr8[bpl8_2+x]>=bFill)
   {
   // 0 0 0
   //	T
   // 1 1 1
   if ((hptr8[bpl8_2+x+1]>=bFill)&&
       (hptr8[bpl8_2+x+2]>=bFill)&&
       (!hptr8[x])&&(!hptr8[x+1])&&(!hptr8[x+2]))
       goto Set;
   //	0 0
   // 1 T 0
   // 1 1
   if ((hptr8[bpl8+x]>=bFill)&&
       (hptr8[bpl8_2+x+1]>=bFill)&&
       (!hptr8[x+1])&&(!hptr8[x+2])&&(!hptr8[bpl8+x+2]))
       goto Set;
       continue;
  }
   // 0 0
   // 0 T 1
   //	1 1
  if (hptr8[bpl8_2+x+2]>=bFill)
  {
   if ((hptr8[bpl8_2+x+1]>=bFill)&&
       (hptr8[bpl8+x+2]>=bFill)&&
       (!hptr8[x])&&(!hptr8[x+1])&&(!hptr8[bpl8+x]))
       goto Set;
   }
continue;
Set:
	a=TRUE;
	hptr8[bpl8+x+1]=bFill-1;
  }
   hptr8+=bpl8;
   xhptr8+=bpl8;
  }
  return(a);
}

/*****************************
	Ordinary dilation
*****************************/
void DilationX(WORD *hptr,WORD *_hptr,WORD * pSrcImage,int X,int Y)
{
char i,j;
int x,y,bpl=((X+3)&~3);
WORD BT;

  for (y=0;y<Y-2;y++)
 {
  for (x=0;x<X-2;x++)
  {
   if (hptr[bpl+x+1]!=0) continue;
   if (pSrcImage[bpl+x+1]==0) continue;
   for (j=0;j<=2;j++)
	for (i=0;i<=2;i++)
		if ((BT=hptr[(DWORD)j*bpl+x+i])!=0)
		{
//		_hptr[bpl+x+1]=BT*3-BT/3;
		_hptr[bpl+x+1]=CalcDilateNum(BT);
		j=2;break;
		}
  }
 hptr+=bpl;
 _hptr+=bpl;
 pSrcImage+=bpl;
 }
}

void Clean( CMorphInfo &in )
{
	register int i,j;
	int c,a,g,v;   

	if(!in.m_pBuf1)return;
	
	for(i = in.Frame_x1; i < in.Frame_x2; i++ )
	{
		if(in.m_pimage[(long)in.Frame_y1*in.iCol4+i] && !in.m_pimage[(long)(in.Frame_y1+1)*in.iCol4+i] )
			in.m_pimage[(long)in.Frame_y1*in.iCol4+i] = 0;
		if(in.m_pimage[(long)(in.Frame_y2-1)*in.iCol4+i] && !in.m_pimage[(long)(in.Frame_y2-2)*in.iCol4+i] )
			in.m_pimage[(long)(in.Frame_y2-1)*in.iCol4+i] = 0;
	}
	if(!ChangePercent(25))
	{
		Reset();
		return;
	}
	for(i = in.Frame_y1; i < in.Frame_y2; i++ )
	{
		if(in.m_pimage[(long)i*in.iCol4+in.Frame_x1] && !in.m_pimage[(long)i*in.iCol4+in.Frame_x1+1] )
			in.m_pimage[(long)i*in.iCol4+in.Frame_x1] = 0;
		if( in.m_pimage[(long)i*in.iCol4+in.Frame_x2-1] && !in.m_pimage[(long)i*in.iCol4+in.Frame_x2-2] )
			in.m_pimage[(long)i*in.iCol4+in.Frame_x2-1] = 0;
	}
	if(!ChangePercent(50))
	{
		Reset();
		return;
	}                                                             
	memcpy(in.m_pBuf1, &in.m_pimage[(long)in.Frame_y1*in.iCol4], in.m_iCol);
	memcpy(in.m_pBuf2, &in.m_pimage[(long)(in.Frame_y1+1)*in.iCol4], in.m_iCol);
	memcpy(in.m_pBuf3, &in.m_pimage[(long)(in.Frame_y1+2)*in.iCol4], in.m_iCol);

	for(i = in.Frame_y1 + 1; i < in.Frame_y2-2; i++)
	{
		do
		{
			c=0;
			for(j = in.Frame_x1 + 1; j < in.Frame_x2; j++)
			{
				a = in.m_pBuf2[j];
				v = (int)in.m_pBuf1[j]   + (int)in.m_pBuf3[j];
				g = (int)in.m_pBuf2[j-1] + (int)in.m_pBuf2[j+1];

				if(( v + g ) != 510 )
					if (( v + g ) > 510 )
						in.m_pBuf2[j]=0xff;
					else
						in.m_pBuf2[j]=0;
				else
					if (!v||!g)
						in.m_pBuf2[j]=0;

				if ((unsigned char)a != in.m_pBuf2[j] )
				{
					c=1;
					in.m_pimage[(long)i*in.iCol4+j] = in.m_pBuf2[j];
				}
			}
		} while (c!=0);                                   	

		memcpy(in.m_pBuf1, in.m_pBuf2, in.m_iCol);
		memcpy(in.m_pBuf2, in.m_pBuf3, in.m_iCol);
		memcpy(in.m_pBuf3, &in.m_pimage[(long)(i+2)*in.iCol4], in.m_iCol);
	}
	if(!ChangePercent(75))
	{
		Reset();
		return;
	}
	memcpy(in.m_pBuf1, &in.m_pimage[(long)in.Frame_y2*in.iCol4], in.m_iCol);
	memcpy(in.m_pBuf2, &in.m_pimage[(long)(in.Frame_y2-1)*in.iCol4], in.m_iCol);
	memcpy(in.m_pBuf3, &in.m_pimage[(long)(in.Frame_y2-2)*in.iCol4], in.m_iCol);

	for(i = in.Frame_y2 - 1; i > in.Frame_y1+1; i--)
	{
		do
		{
			c=0;
			for(j = in.Frame_x1 + 1; j < in.Frame_x2; j++)
			{
				a = in.m_pBuf2[j];
				v = (int)in.m_pBuf1[j]   + (int)in.m_pBuf3[j];
				g = (int)in.m_pBuf2[j-1] + (int)in.m_pBuf2[j+1];

				if(( v + g ) != 510 )
					if (( v + g ) > 510 )
						in.m_pBuf2[j]=0xff;
					else
						in.m_pBuf2[j]=0;
				else
					if (!v||!g)
						in.m_pBuf2[j]=0;

				if (a != in.m_pBuf2[j] )
				{
					c=1;
					in.m_pimage[(long)i*in.iCol4+j] = in.m_pBuf2[j];
				}
			}
		}while (c!=0);

		memcpy(in.m_pBuf1, in.m_pBuf2, in.m_iCol);
		memcpy(in.m_pBuf2, in.m_pBuf3, in.m_iCol);
		memcpy(in.m_pBuf3, &in.m_pimage[(long)(i-2)*in.iCol4], in.m_iCol);
	}
}

void Ers4( CMorphInfo &in )
{
	int     i, j;
    
	memcpy(in.m_pBuf1, &in.m_pimage[0], in.m_iCol);
	memcpy(in.m_pBuf2, &in.m_pimage[0], in.m_iCol);
	memcpy(in.m_pBuf3, &in.m_pimage[in.iCol4], in.m_iCol);

	for(j = 0; j< in.m_iRow;j++)
	{
		i = 0;
		if(!(in.m_pBuf1[i]
		   &&in.m_pBuf2[i]
		   &&in.m_pBuf3[i]
		   &&in.m_pBuf2[i+1]))
				in.m_pimage[(long)i+(long)j*in.iCol4] = 0;
		for(i = 1; i< in.m_iCol-1;i++)
		{
			if(!(in.m_pBuf2[i-1]
			   &&in.m_pBuf1[i]
			   &&in.m_pBuf2[i]
			   &&in.m_pBuf3[i]
			   &&in.m_pBuf2[i+1]))
					in.m_pimage[(long)i+(long)j*in.iCol4] = 0;
               
		}   
		i = in.m_iCol-1;
		if(!(in.m_pBuf2[i-1]
		   &&in.m_pBuf1[i]
		   &&in.m_pBuf2[i]
		   &&in.m_pBuf3[i]))
				in.m_pimage[(long)i+(long)j*in.iCol4] = 0;
		if(j<in.m_iRow-2)
		{
			memcpy(in.m_pBuf1, in.m_pBuf2, in.m_iCol);
			memcpy(in.m_pBuf2, in.m_pBuf3, in.m_iCol);
			memcpy(in.m_pBuf3, &in.m_pimage[(long)(j+2)*in.iCol4], in.m_iCol);
		}
		if(j == in.m_iRow-2)
		{
			memcpy(in.m_pBuf1, in.m_pBuf2, in.m_iCol);
			memcpy(in.m_pBuf2, in.m_pBuf3, in.m_iCol);
		}
	}
}

void Ers8( CMorphInfo &in )
{
	int     i, j;
                                    
	
	
	memcpy(in.m_pBuf1, &in.m_pimage[0], in.m_iCol);
	memcpy(in.m_pBuf2, &in.m_pimage[0], in.m_iCol);
	memcpy(in.m_pBuf3, &in.m_pimage[in.iCol4], in.m_iCol);
	
	
	for(j = 0; j< in.m_iRow;j++)
	{
		i = 0;
		if(!(in.m_pBuf1[0]
			   &&in.m_pBuf2[0]
			   &&in.m_pBuf3[0]
			   &&in.m_pBuf1[1]
			   &&in.m_pBuf2[1]                  
			   &&in.m_pBuf3[1]))
					in.m_pimage[(long)i+(long)j*in.iCol4] = 0;
		for(i = 1; i< in.m_iCol-1;i++)
		{
			if(!(in.m_pBuf1[i-1]
			   &&in.m_pBuf2[i-1]
			   &&in.m_pBuf3[i-1]
			   &&in.m_pBuf1[i]
			   &&in.m_pBuf2[i]
			   &&in.m_pBuf3[i]
			   &&in.m_pBuf1[i+1]
			   &&in.m_pBuf2[i+1]
			   &&in.m_pBuf3[i+1]))
					in.m_pimage[(long)i+(long)j*in.iCol4] = 0;
               
		}
		i = in.m_iCol-1;
		if(!(in.m_pBuf1[i-1]
		   &&in.m_pBuf2[i-1]
		   &&in.m_pBuf3[i-1]
		   &&in.m_pBuf1[i]
		   &&in.m_pBuf2[i]
		   &&in.m_pBuf3[i]))
				in.m_pimage[(long)i+(long)j*in.iCol4] = 0;
		if(j<in.m_iRow-2)
		{
			memcpy(in.m_pBuf1, in.m_pBuf2, in.m_iCol);
			memcpy(in.m_pBuf2, in.m_pBuf3, in.m_iCol);
			memcpy(in.m_pBuf3, &in.m_pimage[(long)(j+2)*in.iCol4], in.m_iCol);
		}
		if(j==in.m_iRow-2)
		{
			memcpy(in.m_pBuf1, in.m_pBuf2, in.m_iCol);
			memcpy(in.m_pBuf2, in.m_pBuf3, in.m_iCol);
		}
	}
}

BOOL Deaglom( CMorphInfo &in )
{                  
	int	i, j;
	BOOL bFound = FALSE;
	
//     1  x  0
//     1  0  0
//     1  x  0
	memcpy(in.m_pBuf1, &in.m_pimage[0], in.m_iCol);
	memcpy(in.m_pBuf2, &in.m_pimage[in.iCol4], in.m_iCol);
	memcpy(in.m_pBuf3, &in.m_pimage[2*in.iCol4], in.m_iCol);

	for(i=1; i < in.m_iRow-1; i++)
	{
		for(j=1; j < in.m_iCol-1; j++)
		{
		    if(((!in.m_pBuf2[j] && in.m_pBuf1[j-1]==0xff && in.m_pBuf1[j]==0xff && in.m_pBuf1[j+1]==0xff &&
			     !in.m_pBuf3[j-1] && !in.m_pBuf3[j] && !in.m_pBuf3[j+1])))
			{                                                                                            
				in.m_pimage[(long)i*in.iCol4+j]=0xfe;
				bFound = TRUE;
			}
		}
		if(i != in.m_iRow-2)
		{
			memcpy( in.m_pBuf1, in.m_pBuf2, in.iCol4 );
			memcpy( in.m_pBuf2, in.m_pBuf3, in.iCol4 );
			memcpy( in.m_pBuf3, &in.m_pimage[(long)(i+2)*in.iCol4], in.m_iCol);
		}
	}
	
//     0  0  0
//     x  0  x
//     1  1  1
	memcpy(in.m_pBuf1, &in.m_pimage[0], in.m_iCol);
	memcpy(in.m_pBuf2, &in.m_pimage[in.iCol4], in.m_iCol);
	memcpy(in.m_pBuf3, &in.m_pimage[2*in.iCol4], in.m_iCol);

	for(i=1; i < in.m_iRow-1; i++)
	{
		for(j=1; j < in.m_iCol-1; j++)
		{
		    if(((!in.m_pBuf2[j] && in.m_pBuf1[j+1]==0xff && in.m_pBuf2[j+1]==0xff && in.m_pBuf3[j+1]==0xff &&
			     !in.m_pBuf1[j-1] && !in.m_pBuf2[j-1] && !in.m_pBuf3[j-1])))
			{
				in.m_pimage[(long)i*in.iCol4+j]=0xfe;
				bFound = TRUE;
			}
		}
		if(i != in.m_iRow-2)
		{
			memcpy( in.m_pBuf1, in.m_pBuf2, in.iCol4 );
			memcpy( in.m_pBuf2, in.m_pBuf3, in.iCol4 );
			memcpy( in.m_pBuf3, &in.m_pimage[(long)(i+2)*in.iCol4], in.m_iCol);
		}
	}
	
//     0  x  1
//     0  0  1
//     0  x  1

	memcpy( in.m_pBuf1, &in.m_pimage[0], in.m_iCol);
	memcpy( in.m_pBuf2, &in.m_pimage[in.iCol4], in.m_iCol);
	memcpy( in.m_pBuf3, &in.m_pimage[2*in.iCol4], in.m_iCol);
	for(i=1; i < in.m_iRow-1; i++)
	{
		for(j=1; j < in.m_iCol-1; j++)
		{
		    if(((!in.m_pBuf2[j] && !in.m_pBuf1[j-1] && !in.m_pBuf1[j] && !in.m_pBuf1[j+1] &&
			     in.m_pBuf3[j-1]==0xff && in.m_pBuf3[j]==0xff && in.m_pBuf3[j+1]==0xff)))
			{
				in.m_pimage[(long)i*in.iCol4+j]=0xfe;
				bFound = TRUE;
			}
		}
		if(i<in.m_iRow-2)
		{
			memcpy( in.m_pBuf1, in.m_pBuf2, in.iCol4 );
			memcpy( in.m_pBuf2, in.m_pBuf3, in.iCol4 );
			memcpy( in.m_pBuf3, &in.m_pimage[(long)(i+2)*in.iCol4], in.m_iCol);
		}
	}
	
	
//     1  1  1
//     x  0  x
//     0  0  0	
	memcpy( in.m_pBuf1, &in.m_pimage[0], in.m_iCol);
	memcpy( in.m_pBuf2, &in.m_pimage[in.iCol4],  in.m_iCol);
	memcpy( in.m_pBuf3, &in.m_pimage[2*in.iCol4],  in.m_iCol);
	
	for(i=1; i < in.m_iRow-1; i++)
	{
		for(j=1; j < in.m_iCol-1; j++)
		{
		    if(((!in.m_pBuf2[j] && in.m_pBuf1[j-1]==0xff && in.m_pBuf2[j-1]==0xff && in.m_pBuf3[j-1]==0xff &&
			     !in.m_pBuf1[j+1] && !in.m_pBuf2[j+1] && !in.m_pBuf3[j+1])))
			{
				in.m_pimage[(long)i*in.iCol4+j]=0xfe;
				bFound = TRUE;
			}
		}
		if(i<in.m_iRow-2)
		{
			memcpy( in.m_pBuf1, in.m_pBuf2, in.iCol4 );
			memcpy( in.m_pBuf2, in.m_pBuf3, in.iCol4 );
			memcpy( in.m_pBuf3, &in.m_pimage[(long)(i+2)*in.iCol4], in.m_iCol);
		}
	}

//     x  0  0
//     1  0  0
//     1  1  x
	
	memcpy(in.m_pBuf1, &in.m_pimage[0],  in.m_iCol);
	memcpy(in.m_pBuf2, &in.m_pimage[in.iCol4],  in.m_iCol);
	memcpy(in.m_pBuf3, &in.m_pimage[2*in.iCol4],  in.m_iCol);
	
	for(i=1; i < in.m_iRow-1; i++)
	{
		for(j=1; j < in.m_iCol-1; j++)
		{
		    if(((!in.m_pBuf2[j] && in.m_pBuf1[j]==0xff && in.m_pBuf1[j+1]==0xff && in.m_pBuf2[j+1]==0xff &&
			     !in.m_pBuf2[j-1] && !in.m_pBuf3[j] && !in.m_pBuf3[j-1])))
			{
				in.m_pimage[(long)i*in.iCol4+j]=0xfe;
				bFound = TRUE;
			}
		}
		if(i<in.m_iRow-2)
		{
			memcpy( in.m_pBuf1, in.m_pBuf2, in.iCol4 );
			memcpy( in.m_pBuf2, in.m_pBuf3, in.iCol4 );
			memcpy( in.m_pBuf3, &in.m_pimage[(long)(i+2)*in.iCol4], in.m_iCol);
		}
	}
	
//     0  0  x
//     0  0  1
//     x  1  1
	
	memcpy( in.m_pBuf1, &in.m_pimage[0],  in.m_iCol);
	memcpy( in.m_pBuf2, &in.m_pimage[in.iCol4],  in.m_iCol);
	memcpy( in.m_pBuf3, &in.m_pimage[2*in.iCol4],  in.m_iCol);
	
	for(i=1; i < in.m_iRow-1; i++)
	{
		for(j=1; j < in.m_iCol-1; j++)
		{
		    if(((!in.m_pBuf2[j] && in.m_pBuf2[j+1]==0xff && in.m_pBuf3[j]==0xff && in.m_pBuf3[j+1]==0xff &&
			     !in.m_pBuf1[j] && !in.m_pBuf2[j-1] && !in.m_pBuf1[j-1])))
			{
				in.m_pimage[(long)i*in.iCol4+j]=0xfe;
				bFound = TRUE;
			}
		}
		if(i<in.m_iRow-2)
		{
			memcpy( in.m_pBuf1, in.m_pBuf2, in.iCol4 );
			memcpy( in.m_pBuf2, in.m_pBuf3, in.iCol4 );
			memcpy( in.m_pBuf3, &in.m_pimage[(long)(i+2)*in.iCol4], in.m_iCol);
		}
	}
		
//     x  1  1
//     0  0  1
//     0  0  x
	
	memcpy(in.m_pBuf1, &in.m_pimage[0], in.m_iCol);
	memcpy(in.m_pBuf2, &in.m_pimage[in.iCol4], in.m_iCol);
	memcpy(in.m_pBuf3, &in.m_pimage[2*in.iCol4], in.m_iCol);
	
	for(i=1; i < in.m_iRow-1; i++)
	{
		for(j=1; j < in.m_iCol-1; j++)
		{
		    if(((!in.m_pBuf2[j] && in.m_pBuf2[j-1]==0xff && in.m_pBuf3[j-1]==0xff && in.m_pBuf3[j]==0xff &&
			     !in.m_pBuf1[j] && !in.m_pBuf2[j+1] && !in.m_pBuf1[j+1])))
			{
				in.m_pimage[(long)i*in.iCol4+j]=0xfe;
				bFound = TRUE;
			}
		}
		if(i<in.m_iRow-2)
		{
			memcpy( in.m_pBuf1, in.m_pBuf2, in.iCol4 );
			memcpy( in.m_pBuf2, in.m_pBuf3, in.iCol4 );
			memcpy( in.m_pBuf3, &in.m_pimage[(long)(i+2)*in.iCol4], in.m_iCol);
		}
	}
		
//     1  1  x
//     1  0  0
//     x  0  0
	
	memcpy(in.m_pBuf1, &in.m_pimage[0], in.m_iCol);
	memcpy(in.m_pBuf2, &in.m_pimage[in.iCol4], in.m_iCol);
	memcpy(in.m_pBuf3, &in.m_pimage[2*in.iCol4], in.m_iCol);
	
	for(i=1; i < in.m_iRow-1; i++)
	{
		for(j=1; j < in.m_iCol-1; j++)
		{
		    if(((!in.m_pBuf2[j] && in.m_pBuf1[j-1]==0xff && in.m_pBuf1[j]==0xff && in.m_pBuf2[j-1]==0xff &&
			     !in.m_pBuf2[j+1] && !in.m_pBuf3[j] && !in.m_pBuf3[j+1])))
			{
				in.m_pimage[(long)i*in.iCol4+j]=0xfe;
				bFound = TRUE;
			}
		}
		if(i<in.m_iRow-2)
		{
			memcpy( in.m_pBuf1, in.m_pBuf2, in.iCol4 );
			memcpy( in.m_pBuf2, in.m_pBuf3, in.iCol4 );
			memcpy( in.m_pBuf3, &in.m_pimage[(long)(i+2)*in.iCol4], in.m_iCol);
		}
	}
	for(long l = 0; l<(long)in.iCol4*in.m_iRow; l++)
		if(in.m_pimage[l])
			in.m_pimage[l] = 0xff;

	return bFound;
}



BOOL Deaglom1( CMorphInfo &in )
{                  
	int	i, j;
	BOOL bFound = FALSE;

//     1  x  0
//     1  0  0
//     1  x  0
	memcpy(in.m_pBuf1, &in.m_pimage[0], in.m_iCol);
	memcpy(in.m_pBuf2, &in.m_pimage[in.iCol4], in.m_iCol);
	memcpy(in.m_pBuf3, &in.m_pimage[2*in.iCol4], in.m_iCol);

	for(i=1; i < in.m_iRow-1; i++)
	{
		for(j=1; j < in.m_iCol-1; j++)
		{
		    if(((!in.m_pBuf2[j] && in.m_pBuf1[j-1]==0xff && in.m_pBuf1[j]==0xff && in.m_pBuf1[j+1]==0xff &&
			     !in.m_pBuf3[j-1] && !in.m_pBuf3[j] && !in.m_pBuf3[j+1])))
			{                                                                                            
				in.m_pimage[(long)i*in.iCol4+j]=0xfe;
				bFound = TRUE;
			}
		}
		if(i != in.m_iRow-2)
		{
			memcpy( in.m_pBuf1, in.m_pBuf2, in.iCol4 );
			memcpy( in.m_pBuf2, in.m_pBuf3, in.iCol4 );
			memcpy( in.m_pBuf3, &in.m_pimage[(long)(i+2)*in.iCol4], in.m_iCol);
		}
	}
	
//     0  0  0
//     x  0  x
//     1  1  1
	memcpy(in.m_pBuf1, &in.m_pimage[0], in.m_iCol);
	memcpy(in.m_pBuf2, &in.m_pimage[in.iCol4], in.m_iCol);
	memcpy(in.m_pBuf3, &in.m_pimage[2*in.iCol4], in.m_iCol);

	for(i=1; i < in.m_iRow-1; i++)
	{
		for(j=1; j < in.m_iCol-1; j++)
		{
		    if(((!in.m_pBuf2[j] && in.m_pBuf1[j+1]==0xff && in.m_pBuf2[j+1]==0xff && in.m_pBuf3[j+1]==0xff &&
			     !in.m_pBuf1[j-1] && !in.m_pBuf2[j-1] && !in.m_pBuf3[j-1])))
			{
				in.m_pimage[(long)i*in.iCol4+j]=0xfe;
				bFound = TRUE;
			}
		}
		if(i != in.m_iRow-2)
		{
			memcpy( in.m_pBuf1, in.m_pBuf2, in.iCol4 );
			memcpy( in.m_pBuf2, in.m_pBuf3, in.iCol4 );
			memcpy( in.m_pBuf3, &in.m_pimage[(long)(i+2)*in.iCol4], in.m_iCol);
		}
	}
	
//     0  x  1
//     0  0  1
//     0  x  1

	memcpy( in.m_pBuf1, &in.m_pimage[0], in.m_iCol);
	memcpy( in.m_pBuf2, &in.m_pimage[in.iCol4], in.m_iCol);
	memcpy( in.m_pBuf3, &in.m_pimage[2*in.iCol4], in.m_iCol);
	for(i=1; i < in.m_iRow-1; i++)
	{
		for(j=1; j < in.m_iCol-1; j++)
		{
		    if(((!in.m_pBuf2[j] && !in.m_pBuf1[j-1] && !in.m_pBuf1[j] && !in.m_pBuf1[j+1] &&
			     in.m_pBuf3[j-1]==0xff && in.m_pBuf3[j]==0xff && in.m_pBuf3[j+1]==0xff)))
			{
				in.m_pimage[(long)i*in.iCol4+j]=0xfe;
				bFound = TRUE;
			}
		}
		if(i<in.m_iRow-2)
		{
			memcpy( in.m_pBuf1, in.m_pBuf2, in.iCol4 );
			memcpy( in.m_pBuf2, in.m_pBuf3, in.iCol4 );
			memcpy( in.m_pBuf3, &in.m_pimage[(long)(i+2)*in.iCol4], in.m_iCol);
		}
	}
	
	
//     1  1  1
//     x  0  x
//     0  0  0	
	memcpy( in.m_pBuf1, &in.m_pimage[0], in.m_iCol);
	memcpy( in.m_pBuf2, &in.m_pimage[in.iCol4],  in.m_iCol);
	memcpy( in.m_pBuf3, &in.m_pimage[2*in.iCol4],  in.m_iCol);
	
	for(i=1; i < in.m_iRow-1; i++)
	{
		for(j=1; j < in.m_iCol-1; j++)
		{
		    if(((!in.m_pBuf2[j] && in.m_pBuf1[j-1]==0xff && in.m_pBuf2[j-1]==0xff && in.m_pBuf3[j-1]==0xff &&
			     !in.m_pBuf1[j+1] && !in.m_pBuf2[j+1] && !in.m_pBuf3[j+1])))
			{
				in.m_pimage[(long)i*in.iCol4+j]=0xfe;
				bFound = TRUE;
			}
		}
		if(i<in.m_iRow-2)
		{
			memcpy( in.m_pBuf1, in.m_pBuf2, in.iCol4 );
			memcpy( in.m_pBuf2, in.m_pBuf3, in.iCol4 );
			memcpy( in.m_pBuf3, &in.m_pimage[(long)(i+2)*in.iCol4], in.m_iCol);
		}
	}

//     x  0  0
//     1  0  0
//     1  1  x
	
	memcpy(in.m_pBuf1, &in.m_pimage[0],  in.m_iCol);
	memcpy(in.m_pBuf2, &in.m_pimage[in.iCol4],  in.m_iCol);
	memcpy(in.m_pBuf3, &in.m_pimage[2*in.iCol4],  in.m_iCol);
	
	for(i=1; i < in.m_iRow-1; i++)
	{
		for(j=1; j < in.m_iCol-1; j++)
		{
		    if(((!in.m_pBuf2[j] && in.m_pBuf1[j]==0xff && in.m_pBuf1[j+1]==0xff && in.m_pBuf2[j+1]==0xff &&
			     !in.m_pBuf2[j-1] && !in.m_pBuf3[j] && !in.m_pBuf3[j-1])))
			{
				in.m_pimage[(long)i*in.iCol4+j]=0xfe;
				bFound = TRUE;
			}
		}
		if(i<in.m_iRow-2)
		{
			memcpy( in.m_pBuf1, in.m_pBuf2, in.iCol4 );
			memcpy( in.m_pBuf2, in.m_pBuf3, in.iCol4 );
			memcpy( in.m_pBuf3, &in.m_pimage[(long)(i+2)*in.iCol4], in.m_iCol);
		}
	}
	
//     0  0  x
//     0  0  1
//     x  1  1
	
	memcpy( in.m_pBuf1, &in.m_pimage[0],  in.m_iCol);
	memcpy( in.m_pBuf2, &in.m_pimage[in.iCol4],  in.m_iCol);
	memcpy( in.m_pBuf3, &in.m_pimage[2*in.iCol4],  in.m_iCol);
	
	for(i=1; i < in.m_iRow-1; i++)
	{
		for(j=1; j < in.m_iCol-1; j++)
		{
		    if(((!in.m_pBuf2[j] && in.m_pBuf2[j+1]==0xff && in.m_pBuf3[j]==0xff && in.m_pBuf3[j+1]==0xff &&
			     !in.m_pBuf1[j] && !in.m_pBuf2[j-1] && !in.m_pBuf1[j-1])))
			{
				in.m_pimage[(long)i*in.iCol4+j]=0xfe;
				bFound = TRUE;
			}
		}
		if(i<in.m_iRow-2)
		{
			memcpy( in.m_pBuf1, in.m_pBuf2, in.iCol4 );
			memcpy( in.m_pBuf2, in.m_pBuf3, in.iCol4 );
			memcpy( in.m_pBuf3, &in.m_pimage[(long)(i+2)*in.iCol4], in.m_iCol);
		}
	}
		
//     x  1  1
//     0  0  1
//     0  0  x
	
	memcpy(in.m_pBuf1, &in.m_pimage[0], in.m_iCol);
	memcpy(in.m_pBuf2, &in.m_pimage[in.iCol4], in.m_iCol);
	memcpy(in.m_pBuf3, &in.m_pimage[2*in.iCol4], in.m_iCol);
	
	for(i=1; i < in.m_iRow-1; i++)
	{
		for(j=1; j < in.m_iCol-1; j++)
		{
		    if(((!in.m_pBuf2[j] && in.m_pBuf2[j-1]==0xff && in.m_pBuf3[j-1]==0xff && in.m_pBuf3[j]==0xff &&
			     !in.m_pBuf1[j] && !in.m_pBuf2[j+1] && !in.m_pBuf1[j+1])))
			{
				in.m_pimage[(long)i*in.iCol4+j]=0xfe;
				bFound = TRUE;
			}
		}
		if(i<in.m_iRow-2)
		{
			memcpy( in.m_pBuf1, in.m_pBuf2, in.iCol4 );
			memcpy( in.m_pBuf2, in.m_pBuf3, in.iCol4 );
			memcpy( in.m_pBuf3, &in.m_pimage[(long)(i+2)*in.iCol4], in.m_iCol);
		}
	}
		
//     1  1  x
//     1  0  0
//     x  0  0
	
	memcpy(in.m_pBuf1, &in.m_pimage[0], in.m_iCol);
	memcpy(in.m_pBuf2, &in.m_pimage[in.iCol4], in.m_iCol);
	memcpy(in.m_pBuf3, &in.m_pimage[2*in.iCol4], in.m_iCol);
	
	for(i=1; i < in.m_iRow-1; i++)
	{
		for(j=1; j < in.m_iCol-1; j++)
		{
		    if(((!in.m_pBuf2[j] && in.m_pBuf1[j-1]==0xff && in.m_pBuf1[j]==0xff && in.m_pBuf2[j-1]==0xff &&
			     !in.m_pBuf2[j+1] && !in.m_pBuf3[j] && !in.m_pBuf3[j+1])))
			{
				in.m_pimage[(long)i*in.iCol4+j]=0xfe;
				bFound = TRUE;
			}
		}
		if(i<in.m_iRow-2)
		{
			memcpy( in.m_pBuf1, in.m_pBuf2, in.iCol4 );
			memcpy( in.m_pBuf2, in.m_pBuf3, in.iCol4 );
			memcpy( in.m_pBuf3, &in.m_pimage[(long)(i+2)*in.iCol4], in.m_iCol);
		}
	}
	

//     1  1  x
//     1  0  x
//     1  1  x
	memcpy(in.m_pBuf1, &in.m_pimage[0], in.m_iCol);
	memcpy(in.m_pBuf2, &in.m_pimage[in.iCol4], in.m_iCol);
	memcpy(in.m_pBuf3, &in.m_pimage[2*in.iCol4], in.m_iCol);
	for(i=1; i < in.m_iRow-1; i++)
	{
		for(j=1; j < in.m_iCol-1; j++)
		{
		    if(((!in.m_pBuf2[j] && in.m_pBuf1[j-1]==0xff && in.m_pBuf1[j]==0xff && in.m_pBuf2[j-1]==0xff &&
			     in.m_pBuf3[j-1]==0xff && in.m_pBuf3[j]==0xff)))
			{                                                                                            
				in.m_pimage[(long)i*in.iCol4+j]=0xfe;
				bFound = TRUE;
			}
		}
		if(i != in.m_iRow-2)
		{
			memcpy( in.m_pBuf1, in.m_pBuf2, in.iCol4 );
			memcpy( in.m_pBuf2, in.m_pBuf3, in.iCol4 );
			memcpy( in.m_pBuf3, &in.m_pimage[(long)(i+2)*in.iCol4], in.m_iCol);
		}
	}

//     x  1  1
//     x  0  1
//     x  1  1
	memcpy(in.m_pBuf1, &in.m_pimage[0], in.m_iCol);
	memcpy(in.m_pBuf2, &in.m_pimage[in.iCol4], in.m_iCol);
	memcpy(in.m_pBuf3, &in.m_pimage[2*in.iCol4], in.m_iCol);
	for(i=1; i < in.m_iRow-1; i++)
	{
		for(j=1; j < in.m_iCol-1; j++)
		{
		    if(((!in.m_pBuf2[j] && in.m_pBuf1[j+1]==0xff && in.m_pBuf1[j]==0xff && in.m_pBuf2[j+1]==0xff &&
			     in.m_pBuf3[j+1]==0xff && in.m_pBuf3[j]==0xff)))
			{                                                                                            
				in.m_pimage[(long)i*in.iCol4+j]=0xfe;
				bFound = TRUE;
			}
		}
		if(i != in.m_iRow-2)
		{
			memcpy( in.m_pBuf1, in.m_pBuf2, in.iCol4 );
			memcpy( in.m_pBuf2, in.m_pBuf3, in.iCol4 );
			memcpy( in.m_pBuf3, &in.m_pimage[(long)(i+2)*in.iCol4], in.m_iCol);
		}
	}

//     1  1  1
//     1  0  1
//     x  x  x
	memcpy(in.m_pBuf1, &in.m_pimage[0], in.m_iCol);
	memcpy(in.m_pBuf2, &in.m_pimage[in.iCol4], in.m_iCol);
	memcpy(in.m_pBuf3, &in.m_pimage[2*in.iCol4], in.m_iCol);
	for(i=1; i < in.m_iRow-1; i++)
	{
		for(j=1; j < in.m_iCol-1; j++)
		{
		    if(((!in.m_pBuf2[j] && in.m_pBuf1[j-1]==0xff && in.m_pBuf1[j]==0xff && in.m_pBuf1[j+1]==0xff &&
			     in.m_pBuf2[j-1]==0xff && in.m_pBuf2[j+1]==0xff)))
			{                                                                                            
				in.m_pimage[(long)i*in.iCol4+j]=0xfe;
				bFound = TRUE;
			}
		}
		if(i != in.m_iRow-2)
		{
			memcpy( in.m_pBuf1, in.m_pBuf2, in.iCol4 );
			memcpy( in.m_pBuf2, in.m_pBuf3, in.iCol4 );
			memcpy( in.m_pBuf3, &in.m_pimage[(long)(i+2)*in.iCol4], in.m_iCol);
		}
	}

//     x  x  x
//     1  0  1
//     1  1  1
	memcpy(in.m_pBuf1, &in.m_pimage[0], in.m_iCol);
	memcpy(in.m_pBuf2, &in.m_pimage[in.iCol4], in.m_iCol);
	memcpy(in.m_pBuf3, &in.m_pimage[2*in.iCol4], in.m_iCol);
	for(i=1; i < in.m_iRow-1; i++)
	{
		for(j=1; j < in.m_iCol-1; j++)
		{
		    if(((!in.m_pBuf2[j] && in.m_pBuf3[j-1]==0xff && in.m_pBuf3[j]==0xff && in.m_pBuf3[j+1]==0xff &&
			     in.m_pBuf2[j-1]==0xff && in.m_pBuf2[j+1]==0xff)))
			{                                                                                            
				in.m_pimage[(long)i*in.iCol4+j]=0xfe;
				bFound = TRUE;
			}
		}
		if(i != in.m_iRow-2)
		{
			memcpy( in.m_pBuf1, in.m_pBuf2, in.iCol4 );
			memcpy( in.m_pBuf2, in.m_pBuf3, in.iCol4 );
			memcpy( in.m_pBuf3, &in.m_pimage[(long)(i+2)*in.iCol4], in.m_iCol);
		}
	}
	for(long l = 0; l<(long)in.iCol4*in.m_iRow; l++)
		if(in.m_pimage[l])
			in.m_pimage[l] = 0xff;
	return bFound;
}

BOOL Deaglom2( CMorphInfo &in )
{                  
	int	i;
	BOOL bFound = FALSE;

	register LPBYTE pBuf1,pBuf2,pBuf3;

	pBuf1 = &in.m_pimage[0];
	pBuf2 = &in.m_pimage[in.iCol4];
	pBuf3 = &in.m_pimage[2*in.iCol4];

	for(i=1; i < in.m_iRow-1; i++)
	{
//		for(j=1; j < in.m_iCol-1; j++)
		register int iCol = in.m_iCol-1;
//		register LPBYTE pDest = &in.m_pimage[(long)i*in.iCol4];
		for(register int j=1; j < iCol; j++)
		{
			if (!pBuf2[j])
			{
#define			b00  pBuf1[j-1]
#define			b01  pBuf1[j]
#define			b02  pBuf1[j+1]
#define			b10  pBuf2[j-1]
#define			b11  pBuf2[j]
#define			b12  pBuf2[j+1]
#define			b20  pBuf3[j-1]
#define			b21  pBuf3[j]
#define			b22  pBuf3[j+1]
				//     1  x  0
				//     1  0  0
				//     1  x  0
				//
				//     1  1  x
				//     1  0  x
				//     1  1  x
				if (b00==0xff && b10==0xff && b20==0xff && (!b02 && !b12 && !b22 || b01==0xff && b21==0xff))
					pBuf2[j]=0xfe;
				//     0  x  1
				//     0  0  1
				//     0  x  1
				//
				//     x  1  1
				//     x  0  1
				//     x  1  1
				else if (b02==0xff && b12==0xff && b22==0xff && (!b00 && !b10 && !b20 || b01==0xff && b21==0xff))
					pBuf2[j]=0xfe;
				//     0  0  0
				//     x  0  x
				//     1  1  1
				//
				//     x  x  x
				//     1  0  1
				//     1  1  1
				else if (b20==0xff && b21==0xff && b22==0xff &&	(!b00 && !b01 && !b02 || b10==0xff && b12==0xff))
					pBuf2[j]=0xfe;
				//     1  1  1
				//     x  0  x
				//     0  0  0
				//
				//     1  1  1
				//     1  0  1
				//     x  x  x
				else if (b00==0xff && b01==0xff && b02==0xff && (!b20 && !b21 && !b22 || b10==0xff && b12==0xff))
					pBuf2[j]=0xfe;
				//     x  1  1
				//     0  0  1
				//     0  0  x
				else if(b01==0xff && b02==0xff && b12==0xff && !b10 && !b20 && !b21)
					pBuf2[j]=0xfe;
				//     0  0  x
				//     0  0  1
				//     x  1  1
				else if(b12==0xff && b21==0xff && b22==0xff && !b00 && !b10 && !b01)
					pBuf2[j]=0xfe;
				//     x  0  0
				//     1  0  0
				//     1  1  x
				else if(b10==0xff && b20==0xff && b21==0xff && !b01 && !b12 && !b02)
					pBuf2[j]=0xfe;
				//     1  1  x
				//     1  0  0
				//     x  0  0
				else if(b00==0xff && b01==0xff && b10==0xff && !b12 && !b21 && !b22)
					pBuf2[j]=0xfe;
#undef b00
#undef b01
#undef b02
#undef b10
#undef b11
#undef b12
#undef b20
#undef b21
#undef b22
			}
		}
		
		if(i != in.m_iRow-2)
		{
			pBuf1 = pBuf2;
			pBuf2 = pBuf3;
			pBuf3 = &in.m_pimage[(long)(i+2)*in.iCol4];
		}
	}

	for(long l = 0; l<(long)in.iCol4*in.m_iRow; l++)
		if(in.m_pimage[l]==0xfe)
		{
			in.m_pimage[l] = 0xff;
			bFound = TRUE;
		}
	return bFound;
}


void Erose( CMorphInfo &in )
{                          
	if( in.m_iFactor )Ers8( in );
	else Ers4( in );
	in.m_iFactor^=1;
}