#include "stdafx.h"
#include "bytemorpho.h"
#include "alloc.h"

//static int TDilate=0;

void DilateColor(BYTE **sel, int cx, int cy, int mask, byte cFore, byte cForeNew, byte cBack)
//обработка маски - дилатация
{
	//TDilate-=GetTickCount();
	
	int m=mask, mm=m*m, m1=(m-1)/2, m2=m-m1;
	smart_alloc(count, int, (cx+1)*(cy+1));
	smart_alloc(countRows, int*, cy+1);
	for(int y=0; y<=cy; y++) countRows[y] = count+(cx+1)*y;
	
	for(int x=0; x<=cx; x++) count[x]=0; //1-я строка =0
	
	for(y=1; y<=cy; y++)
	{
		countRows[y][0]=0;
		for(int x=1; x<=cx; x++)
		{
			countRows[y][x]=
				countRows[y-1][x]+countRows[y][x-1]-
				countRows[y-1][x-1];
			if (sel[y-1][x-1]==cFore)
				countRows[y][x]++;
		}
	}
	
	smart_alloc(y0, int, cy);
	smart_alloc(y1, int, cy);
	for(y=0; y<cy; y++)
	{
		y0[y]=max(y-m1,0);
		y1[y]=min(y+m2,cy);
	}
	
	smart_alloc(x0, int, cx);
	smart_alloc(x1, int, cx);
	for(x=0; x<cx; x++)
	{
		x0[x]=max(x-m1,0);
		x1[x]=min(x+m2,cx);
	}
	
	for(y=0; y<cy; y++)
	{
		int *c0=countRows[y0[y]];
		int *c1=countRows[y1[y]];
		for(int x=0; x<cx; x++)
		{
			int xx0=x0[x], xx1=x1[x];
			if(sel[y][x]==cBack && c1[xx1]-c1[xx0]-c0[xx1]+c0[xx0]>2)
				sel[y][x]=cForeNew;
		}
	}
	//TDilate+=GetTickCount();
}

int DeleteSmallSegments(byte **buf, int cx, int cy, int del_size, int fill_color)
{
	smart_alloc(ind, byte, 256);
	for(int i=0; i<256; i++) ind[i]=i;
	
	smart_alloc(cnt_buf, byte, cx*cy);
	smart_cnt_buf.zero();
	
	smart_alloc(cnt, byte*, cy);
	for(int y=0; y<cy; y++) cnt[y]=cnt_buf+y*cx;
	
	int cur_ind;
	
	cnt[0][0]=cur_ind=0;
	for(int x=1; x<cx; x++)
	{
		if(buf[0][x]!=buf[0][x-1]) cur_ind++;
		cnt[0][x]=cur_ind;
	}
	
	for(y=1; y<cy; y++)
	{
		for(int x=0; x<cx; )
		{
			int i=-1;
			int xx=x;
			for(; x<cx && buf[y][x]==buf[y][xx]; x++)
				if(buf[y][x]==buf[y-1][x] && cnt[y-1][x]>i) i=cnt[y-1][x];
				if(i<0)
				{
					cur_ind++;
					if(cur_ind>255) cur_ind=255;
					i=cur_ind;
				}
				for(; xx<x; xx++) cnt[y][xx]=i;
		}
	}
	
	for(y=1; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			if(cnt[y-1][x]<cnt[y][x] && buf[y-1][x]==buf[y][x])
				ind[cnt[y-1][x]]=ind[cnt[y][x]];
		}
	}
	
	for(i=cur_ind; i>=0; i--)
		ind[i]=ind[ind[i]];
	
	smart_alloc(area, int, 256);
	smart_area.zero();
	
	smart_alloc(min_x, int, 256);
	smart_alloc(max_x, int, 256);
	smart_alloc(min_y, int, 256);
	smart_alloc(max_y, int, 256);
	for(i=cur_ind; i>=0; i--)
	{
		min_x[i]=cx; max_x[i]=0;
		min_y[i]=cy; max_y[i]=0;
	}
	

	//FILE *f=fopen("d:\\vt5\\out.log","w");
	for(y=0; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			byte c=ind[cnt[y][x]];
			//fputc(c+'A',f);
			area[c]++;
			if(x<min_x[c]) min_x[c]=x;
			if(x>max_x[c]) max_x[c]=x;
			if(y<min_y[c]) min_y[c]=y;
			if(y>max_y[c]) max_y[c]=y;
		}
		//fputs("\n",f);
	}
	//fclose(f);
	
	smart_alloc(del_it, BOOL, 256);
	for(i=cur_ind; i>=0; i--)
	{
		del_it[i]=
			max_x[i]-min_x[i]<=del_size && max_y[i]-min_y[i]<=del_size;
	}
	
	for(y=0; y<cy; y++)
	{
		for(int x=0; x<cx-1; x++)
		{
			byte c=ind[cnt[y][x]];
			if(del_it[c]) buf[y][x]=fill_color;
		}
	}
	
	int count=0;
	for(i=1; i<=cur_ind; i++)
	{
		if(area[i]>1) count++;
	}
	
	//CString s;
	//s.Format("Areas: %d (%d)", count,cur_ind);
	//char s[200];
	//sprintf(s,"Areas: %d (%d)", count,cur_ind);
	//MessageBox(0,s,"FindCells/DeleteSmallSegments",0);
	
	return(count);
}
