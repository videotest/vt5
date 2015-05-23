#include "stdafx.h"
#include "resource.h"
#include "binary.h"
#include "\vt5\common2\misc_templ.h"
#include "..\common2\class_utils.h"
#include "contourapi.h"
#include "smoothcontourbin.h"
//#include <list>



IMPLEMENT_DYNCREATE(CSmoothContourBin, CCmdTargetEx);

// {A262E603-5CD3-44c0-BD88-8551587F9C19}
GUARD_IMPLEMENT_OLECREATE(CSmoothContourBin, "FiltersMain.SmoothContourBin", 
0xa262e603, 0x5cd3, 0x44c0, 0xbd, 0x88, 0x85, 0x51, 0x58, 0x7f, 0x9c, 0x19);

// {329EFDE3-2BCC-4501-BCEF-170AD4DC86B6}
static const GUID guidSmoothContourBin = 
{ 0x329efde3, 0x2bcc, 0x4501, { 0xbc, 0xef, 0x17, 0xa, 0xd4, 0xdc, 0x86, 0xb6 } };

ACTION_INFO_FULL(CSmoothContourBin, IDS_ACTION_SMOOTH_CONTOUR_BIN, IDS_MENU_FILTERS, IDS_TB_MAIN, guidSmoothContourBin);

ACTION_TARGET(CSmoothContourBin, szTargetAnydoc);


ACTION_ARG_LIST(CSmoothContourBin)
	ARG_INT(_T("Size"),7)
	ARG_BINIMAGE( _T("Image") )
	RES_BINIMAGE( _T("SmoothedContours") )
END_ACTION_ARG_LIST();

using namespace smooth;

CContour::CContour(CFilterBase* f, IBinaryImage* bin, int maskSize):
CWalkFillInfo(GetRect(bin))
{
	this->maskSize = maskSize;
	filter = f;

}
RECT CContour::GetRect(IBinaryImage* bin)
{
	bin->GetSizes(&cx,&cy);
	bi = bin;
	RECT r = {0,0,cx,cy};
	return r;
}

CContour::~CContour()
{

ClearContours();
	
}
void CContour::GetRow(byte** row, int y)
{
	*row = GetRowMask(y);
}
void CContour::Gauss(void)
{
	
	float* gauss = new float[(this->maskSize/2)*2+1];
	int mask = maskSize/2;
	float *f =  gauss + mask; 
	for(int i=0;i<=mask;i++)
	{
		f[-i] = f[i] = exp(-i*i/float(mask*mask)/2.f);
	}
	double s=0;
	for(int i=-mask;i<=mask;i++) s+= f[i];
	for(int i=-mask;i<=mask;i++) f[i]/=s;

	int n = GetContoursCount();

	for(int i=0;i<n;i++)
	{
		Contour	*c = GetContour(i);
		if(c->nContourSize<=maskSize) continue;
		Contour *c_new = ContourCopy(c);
		for(int j=0;j<c->nContourSize;j++)
		{
			float xx=0,yy =0;
			POINT p0;
			::ContourGetPoint( c, j, (int&)p0.x, (int&)p0.y);
			for(int m = -mask; m<= mask; m++)
			{
				if(!m) continue;
				POINT p1,p2;

				int d = j + m;
                if(d < 0) d = c->nContourSize+d;
				if(d >= c->nContourSize) d= d - c->nContourSize;

				::ContourGetPoint( c, d, (int&)p2.x, (int&)p2.y);

				xx+=f[m]*(p2.x-p0.x);
				yy+=f[m]*(p2.y-p0.y);
			}
			
			xx += xx>0 ? 0.5f: -0.5f;
			yy += yy>0 ? 0.5f: -0.5f;
			int dx = xx;
			int dy = yy;

			
			
		

		::ContourSetPoint( c_new, j, p0.x+dx, p0.y+dy);
		}
	//delete contour and assign another 
	ContourDelete(c);
	m_contours[i] = redraw(c_new);
	ContourDelete(c_new);
	}
	delete[] gauss;
}
void CContour::FillContour(byte **dst, int cx, int cy, ContourPoint *pPoints, int n, byte color)
{
	if(n<=3) return;
	int* axis = new int[cy];

	for(int i=0; i<cy; i++) 
					axis[i]=-1;

	int x=pPoints[n-1].x, 
		y=pPoints[n-1].y;

	int lastDir=pPoints[n-1].y>pPoints[n-2].y ? 1 : -1;

	for(i=0; i<n; i++)
	{
		if(y!=pPoints[i].y)
		{
			int dy= pPoints[i].y>y ? 1 : -1;

			if(y>=0 && y<cy)
			{
				if(axis[y]!=-1)
				{
					int xx1=min(x,axis[y]);
					int xx2=max(x,axis[y]);
					for(int fillx=xx1+1; fillx<=xx2; fillx++)
					{
						if(fillx>=0 && fillx<cx)
							dst[y+1][fillx+1] ^= color;
					}
					axis[y]=-1;
				}
				else axis[y]=x;
				if(dy!=lastDir /*&& lastDir!=0*/)
				{
					if(axis[y]==-1) axis[y]=x;
					else axis[y]=-1;
				}
			}
			lastDir=dy;
		}
		x=pPoints[i].x;
		y=pPoints[i].y;
	}
	delete[] axis;
}

Contour*  CContour::redraw(Contour* c)
{

	int n = c->nContourSize;
	Contour* c_new = ContourCreate();

	ContourPoint prev,curr;
	prev = c->ppoints[n-1];

	for(int i =0;i<n;i++)
	{
		curr = c->ppoints[i];
		int x =0,y =0;
		if(curr.x==prev.x && curr.y==prev.y) continue;
		int dx = curr.x - prev.x;
		int dy = curr.y - prev.y;
		while(dx || dy)
		{
			if(dx)
			{
				if(dx>0)
				{
					dx--;
					x++;
					ContourAddPoint(c_new, prev.x+x, prev.y + y, false);
				}
				else 
				{
					dx++;
					x--;
					ContourAddPoint(c_new, prev.x+x, prev.y + y, false);
				}

			}

			if(dy)
			{

				if(dy>0)
				{
					dy--;
					y++;
					ContourAddPoint(c_new, prev.x+x, prev.y + y, false);
				}
				else 
				{
					dy++;
					y--;
					ContourAddPoint(c_new, prev.x+x, prev.y + y, false);
				}

			}
		}

		prev = curr;
	}
	//now glade contour
//	{
//		list<ContourPoint> lpoints;
//		list<ContourPoint>::iterator curr, prev, next, temp;
//		n = c_new->nContourSize;
//		for(i =0;i<n;i++)
//		{
//			lpoints.push_back(c_new->ppoints[i]);
//		}
//		
//
//		prev = next = curr = lpoints.begin();
//		/*next++;*/
//		while(curr!=lpoints.end())
//			{
//				bool bCycle=false;
//				next=curr;
//				next++;
//				if(next==lpoints.end()) 
//					{
//					next=lpoints.begin();
//					bCycle=true;
//					}
//				for(int n =0;n<maskSize && lpoints.size()>maskSize ;n++)
//					{
//						if(*next==*curr)
//						{
//							/*int n = lpoints.size();*/
//							next++;
//							if(bCycle)
//							{
//								curr = lpoints.erase(curr,lpoints.end());
//								lpoints.erase(lpoints.begin(),next);
//							}
//							else curr = lpoints.erase(curr,next);
//							if(curr==lpoints.end()) curr = lpoints.begin();
//							/*n-=lpoints.size();*/
//			
//						}
//						next=curr;
//						next++;
//						if(next == lpoints.end())
//						{
//							next=lpoints.begin();
//							bCycle=true;
//						}
//
//					}
//				curr++;				
//			}
////
////
////		/*while(lpoints.size()>3)
////		{
////			prev = next = curr;
////			next++;
////			if(next==lpoints.end()) next=lpoints.begin();
////			if(prev==lpoints.begin()) prev = lpoints.end();
////			prev--;
////
////			if(*prev==*next)
////			{				
////				curr = lpoints.erase(curr);
////				if(curr==lpoints.end()) curr = lpoints.begin();
////				if(lpoints.size()<3) break;
////				curr = lpoints.erase(curr);
////				if(lpoints.size()<3) break;
////			}
////
////
////			if(curr==lpoints.end()) curr==lpoints.begin();
////			curr++;
////			if(curr==lpoints.end()) break;
////		}
////*/
////
////		
////
//		n=lpoints.size();
//		curr = lpoints.begin();
//		
//		
//
//		for(i=0;i<n;i++)
//		{
//			c_new->ppoints[i]=*curr;
//			curr++;
//		}
//		c_new->nContourSize = n;
////
////
////
//	}
	return c_new;
}




bool CContour::Build()
{
	if( cy == 0 )
		return false;

	byte* pbin; 
	for(long y = 0; y < cy; y++ )
	{
		bi->GetRow(&pbin, y, FALSE);
		byte* pmaskdest = GetRowMask(y);
		for(long x = 0; x < cx; x++, pmaskdest++)
		{
			if(pbin[x]!= 0)
				*pmaskdest = 255;
			else
				*pmaskdest = 0;
		}
		filter->Notify( y/2 );
	}

		CRect rectBounds = m_rect;
		CPoint pointStart = rectBounds.TopLeft();
		for (int j = pointStart.y; j < rectBounds.bottom; j++)
			for (int i = rectBounds.left; i < rectBounds.right; i++)
				if (GetPixel(i, j) == m_byteRawBody)
				{
					Contour	*p = ::ContourWalk(*this, i, j, 6);
					if( !p )continue;
					AddContour(p);
					::ContourMark(*this, p, m_byteContourMark);
				}

}

void CContour::Fill()
{

	ClearMasks();
	int n = GetContoursCount();
	Contour* c;
	m_byteContourMark = 4;
	for(int i = 0;i<n;i++)
	{
		c = (Contour*)m_contours[i];
		FillContour(m_pprows, cx, cy, c->ppoints,c->nContourSize,0xff);
		MarkContourAvoidConnect(c);
		/*::ContourMark(*this, c, 0xff);*/

	}
}

void CContour::MarkContourAvoidConnect(Contour* pcntr)
{
	byte mark =253;
	for( int nPoint = 0; nPoint < pcntr->nContourSize; nPoint++ )
	{
		int	x = pcntr->ppoints[nPoint].x;
		int	y = pcntr->ppoints[nPoint].y;
		if(
			GetPixel(x-1,y-1)== 254	||
			GetPixel(x,y-1)== 254	||
			GetPixel(x+1,y-1)== 254 ||
			GetPixel(x-1,y)== 254	||
			GetPixel(x+1,y)== 254	||
			GetPixel(x-1,y+1)== 254 ||
			GetPixel(x,y+1)== 254	||
			GetPixel(x+1,y+1)== 254 

			) 
		{
			/*if(GetPixel(x,y) != 254)*/ mark = 252;			
		}

		SetPixel( x, y, mark );
	}
		for( nPoint = 0; nPoint < pcntr->nContourSize; nPoint++ )
	{
		int	x = pcntr->ppoints[nPoint].x;
		int	y = pcntr->ppoints[nPoint].y;
		if(GetPixel(x,y)==253) SetPixel(x,y,254);
		else SetPixel(x,y,0);
	}

}

CSmoothContourBin::CSmoothContourBin(void)
{
}

CSmoothContourBin::~CSmoothContourBin(void)
{

}

bool CSmoothContourBin::InvokeFilter()
{
	int	nSize = GetArgumentInt( "Size" );
	IBinaryImagePtr bin = GetDataArgument();
	IBinaryImagePtr binOut = GetDataResult();
	int cx,cy;
	bin->GetSizes(&cx,&cy);
	binOut->CreateNew(cx,cy);
	CPoint	ptOffset;
	bin->GetOffset(&ptOffset);
	binOut->SetOffset(ptOffset, true);
	StartNotification(cy);



	CContour c(this, bin, nSize);
	c.Build();
	c.Gauss();
	c.Fill();

	byte* pbin;
	byte* pmaskdest;
	for(long y = 0; y < cy; y++ )
	{
		binOut->GetRow(&pbin, y, FALSE);
		c.GetRow(&pmaskdest,y);

		for(long x = 0; x < cx; x++, pmaskdest++)
		{
			
			if( *pmaskdest</*=*/1)
			{
				 pbin[x]= 0;
			}
			else
				pbin[x]= 255;
		}
		Notify( cy/2+y/2 );
	}

	FinishNotification();
	return true;
}
