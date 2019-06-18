#include "stdafx.h"
#include "resizebin.h"
#include "resource.h"
#include "binimageint.h"
#include "binary.h"
#include "alloc.h"

IMPLEMENT_DYNCREATE(CResizeBin, CCmdTargetEx);

// {D35302C7-D1C3-4483-AA36-16B8B1DD4409}
GUARD_IMPLEMENT_OLECREATE(CResizeBin, "FiltersMain.ResizeBin", 
0xd35302c7, 0xd1c3, 0x4483, 0xaa, 0x36, 0x16, 0xb8, 0xb1, 0xdd, 0x44, 0x9);

// {321CF685-0B2E-4d58-91B1-8B1350D6E66E}
static const GUID guidResizeBin = 
{ 0x321cf685, 0xb2e, 0x4d58, { 0x91, 0xb1, 0x8b, 0x13, 0x50, 0xd6, 0xe6, 0x6e } };

ACTION_INFO_FULL(CResizeBin, IDS_ACTION_RESIZE_BIN, IDS_MENU_FILTERS, IDS_TB_MAIN, guidResizeBin);

ACTION_TARGET(CResizeBin, szTargetAnydoc);

ACTION_ARG_LIST(CResizeBin)
	ARG_INT(_T("cx"), 640 )
	ARG_INT(_T("cy"), 480 )
	ARG_BINIMAGE( _T("BinImage") )
	RES_BINIMAGE( _T("BinResult") )
	//RES_IMAGE( _T("GrayResult") )
END_ACTION_ARG_LIST();

////////////////////////////////////////////////////////////////////////////////////////////////////
//CResizeBin
////////////////////////////////////////////////////////////////////////////////////////////////////
CResizeBin::CResizeBin()
{
}

static inline int round(double x)
{
	return int(floor(x+0.5));
}

bool CResizeBin::InvokeFilter()
{
	IBinaryImagePtr ptrBinSrc = GetDataArgument();
	IBinaryImagePtr	ptrBinDst = GetDataResult("BinResult");
	//IImage4Ptr	ptrGrayDst = GetDataResult("GrayResult");
	if ((ptrBinSrc == NULL) || (ptrBinDst == NULL)) return false;

	int cx1 = GetArgumentInt( "cx" );
	int cy1 = GetArgumentInt( "cy" );
	if(cx1<=0 || cy1<=0) return false;

	int cx=0, cy=0;
	ptrBinSrc->GetSizes(&cx,&cy);

	StartNotification(200);

	smart_alloc(src,byte*,cy);
	for(int y=0; y<cy; y++)
		ptrBinSrc->GetRow(src+y,y,FALSE);

 	if( ptrBinDst->CreateNew(cx1,cy1) != S_OK)
		return false;

 	//if( ptrGrayDst->CreateImage(max(cx,cx1),max(cy,cy1),_bstr_t("GRAY"),-1) != S_OK)
	//	return false;

	{ // resize без сглаживания
		smart_alloc(xx,int,cx1)
		for(int x=0; x<cx1; x++)
			xx[x] = x*cx/cx1;

		for(int y=0; y<cy1; y++)
		{
			int y0 = y*cy/cy1;
			byte *psrc=0, *pdst=0;
			//color *pgray=0;
			ptrBinSrc->GetRow(&psrc,y0,FALSE);
			ptrBinDst->GetRow(&pdst,y,FALSE);
			//ptrGrayDst->GetRow(y0, 0, &pgray);
			for(int x=0; x<cx1; x++)
			{
				int x0 = xx[x];
				pdst[x] = psrc[x0];
				//pgray[x0] = psrc[x0] ? 65535 : 0;
			}
		}
	}

	//if(0)
	//{ // resize со сглаживанием по маске в форме окружности
	//	double rr = 2;
	//	int nn = 16;

	//	smart_alloc(xx,int,cx1)
	//	for(int x=0; x<cx1; x++)
	//		xx[x] = x*cx/cx1;

	//	smart_alloc(xx2,int,cx1*nn)
	//	for(int x=0; x<cx1; x++)
	//	{
	//		for(int i=0; i<nn; i++)
	//		{
	//			xx2[x*nn+i] = int( (x+0.5)*cx/cx1 + rr*cos(i*2*PI/nn) );
	//			if(xx2[x*nn+i]<0) xx2[x*nn+i]=0;
	//			if(xx2[x*nn+i]>=cx) xx2[x*nn+i]=cx-1;
	//		}
	//	}

	//	smart_alloc(yy2,int,cy1*nn)
	//	for(int y=0; y<cy1; y++)
	//	{
	//		for(int i=0; i<nn; i++)
	//		{
	//			yy2[y*nn+i] = int( (y+0.5)*cy/cy1 + rr*sin(i*2*PI/nn) );
	//			if(yy2[y*nn+i]<0) yy2[y*nn+i]=0;
	//			if(yy2[y*nn+i]>=cy) yy2[y*nn+i]=cy-1;
	//		}
	//	}


	//	for(int y=0; y<cy1; y++)
	//	{
	//		int y0 = y*cy/cy1;
	//		byte *psrc=0, *pdst=0;
	//		color *pgray=0;
	//		ptrBinSrc->GetRow(&psrc,y0,FALSE);
	//		ptrBinDst->GetRow(&pdst,y,FALSE);
	//		ptrGrayDst->GetRow(y, 0, &pgray);
	//		for(int x=0; x<cx1; x++)
	//		{
	//			int x0 = xx[x];
	//			//pgray[x0] = psrc[x0] ? 65535 : 0;
	//			byte c = psrc[x0];
	//			int s1=0, s2=0;
	//			for(int i=0; i<nn/2; i++)
	//			{
	//				byte c1 = src[ yy2[y*nn+i] ] [ xx2[x*nn+i] ];
	//				byte c2 = src[ yy2[y*nn+nn/2+i] ] [ xx2[x*nn+nn/2+i] ];
	//				if( c1==c ) s1++;
	//				if( c2==c ) s1++;
	//				if( c1==c && c2==c ) s2++;
	//			}

	//			if( /*s2<nn/16 &&*/ s1 < nn/2)
	//				// если точка лежит не на линии своего цвета и рядом мало таких же - инвертируем
	//				c = c ? 0 : 255;

	//			pdst[x] = c;
	//			pgray[x] = ( psrc[x0] ? s1 : (nn-s1) ) * 65535 / nn;
	//		}
	//	}
	//}


	//if(0)
	//{ // поиск и обработка контуров
	//	CRect rcBound(0,0, cx,cy);
	//	CWalkFillInfo wfi(rcBound);
	//	for(int y=0; y<cy; y++)
	//	{
	//		byte *psrc=0, *pdst=0;
	//		ptrBinSrc->GetRow(&psrc,y,FALSE);
	//		pdst = wfi.GetRowMask(y);
	//		for(int x=0; x<cx; x++)
	//			pdst[x] = psrc[x]!=0 ? 255 : 0;
	//	}

	//	CPoint pt(0,0);
	//	wfi.InitContours(false, CPoint( -1, -1 ), &pt, NORECT, 0, 0, true);
	//	int nCounter = wfi.GetContoursCount();
	//	for(int i = 0; i < nCounter; i++)
	//	{
	//		Contour* pContour = wfi.GetContour(i);
	//		int n = pContour->nContourSize;
	//		int dn = 3;

	//		smart_alloc(x_shift, double, n);
	//		smart_alloc(y_shift, double, n);

	//		for(int j = 0; j < n; j++)
	//		{
	//			ContourPoint pt = pContour->ppoints[j];

	//			int j1, j2;
	//			double dx, dy, h;
	//			for(dn=3; dn>=1; dn--)
	//			{
	//				j1 = (j + dn) % n;
	//				j2 = (j + n - dn) % n;

	//				dx = pContour->ppoints[j2].x - pContour->ppoints[j1].x;
	//				dy = pContour->ppoints[j2].y - pContour->ppoints[j1].y;
	//				double h = hypot(dx,dy);

	//				ContourPoint pt1 = pContour->ppoints[j1];
	//				ContourPoint pt2 = pContour->ppoints[j2];

	//				int s = (pt.x-pt1.x)*(pt.y-pt2.y) - (pt.y-pt1.y)*(pt.x-pt2.x);
	//				// площадь треугольника pt-pt1-pt2
	//				if(abs(s)<h) break; // если высота треугольника <2 - остановимся
	//			}

	//			if(h>1e-5)
	//			{
	//				dx /= h;
	//				dy /= h;
	//			}

	//			double s=0, sx=0, sy=0;
	//			for(int jj = -dn; jj<=dn; jj++)
	//			{
	//				int k = (j+n+jj)%n;
	//				//s += ( pt.x - pContour->ppoints[k].x ) * dy -
	//				//	 ( pt.y - pContour->ppoints[k].y ) * dx;
	//				sx += pContour->ppoints[k].x;
	//				sy += pContour->ppoints[k].y;
	//			}
	//			//s /= dn*2+1;
	//			sx /= dn*2+1;
	//			sy /= dn*2+1;
	//			for( int yy=-1; yy<=1; yy++)
	//			{
	//				for( int xx=-1; xx<=1; xx++)
	//				{
	//					int x = pt.x+xx; 
	//					int y = pt.y+yy; 
	//					s = ( x - sx ) * dy -
	//						( y - sy ) * dx +
	//						( fabs(dx) + fabs(dy) ) / 2;
	//					if( x>=0 && x<cx && y>=0 && y<cy)
	//					{
	//						color *pgray=0;
	//						ptrGrayDst->GetRow(y, 0, &pgray);
	//						int c = s*16384 + 32768;
	//						if(c<1) c=1;
	//						if(c<65535)
	//						{
	//							int c0 = pgray[ x ];
	//							if(c0!=65535) c = max(c,c0);
	//							//if(c0!=0) c = min(c,c0);
	//							pgray[ x ] = c;
	//						}
	//					}
	//				}
	//			}

	//			//color *pgray=0;
	//			//ptrGrayDst->GetRow(pt.y, 0, &pgray);
	//			//pgray[ pt.x ] = s*32768 + 32768;

	//			/*
	//			x_shift[j] = - s * dy;
	//			y_shift[j] = s * dx;

	//			int x1 = round(double(pt.x)*cx1/cx), y1 = round(double(pt.y)*cy1/cy);
	//			byte *pdst=0;
	//			ptrBinDst->GetRow(&pdst,y1,FALSE);
	//			pdst[x1]=j;

	//			int ww = cx1/cx/2+1, hh = cy1/cy/2+1;
	//			for(int yy=-hh; yy<=hh; yy++)
	//			{
	//				for(int xx=-ww; xx<=ww; xx++)
	//				{
	//					double score = xx*dy*cx/cx1 - yy*dx*cy/cy1;
	//					if(score<0)
	//					{
	//						int x2=x1+xx; int y2=y1+yy;
	//						if(x2>=0 && x2<cx1 && y2>=0 && y2<cy1)
	//						{
	//							ptrBinDst->GetRow(&pdst,y2,FALSE);
	//							pdst[x2]=2;
	//						}
	//					}
	//				}
	//			}

	//			// test dx/dy
	//			if(0)
	//			{
	//				x1 += x_shift[j]*cx1/cx, y1 += y_shift[j]*cy1/cy;
	//				if(x1>=0 && x1<cx1 && y1>=0 && y1<cy1)
	//				{
	//					ptrBinDst->GetRow(&pdst,y1,FALSE);
	//					pdst[x1]=j;
	//				}
	//			}
	//			*/
	//		}
	//	}
	//}

	Notify(0);
	
	FinishNotification();
	
	return true;
}