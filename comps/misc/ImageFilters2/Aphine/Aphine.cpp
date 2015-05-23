// FilterAphine.cpp: implementation of the CFilterAphine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "math.h"
#include "limits.h"

#include "aphine.h"
#include "misc_utils.h"

#include "action_filter.h"

static void dbg_assert( bool bExpression, const char *pszErrorDescription )
{
	if( !bExpression )
	{
		
#ifdef _DEBUG
		DebugBreak();
#endif
		
	}
}

// обновить max и min координат x,y
// чтобы помещалась проекция точки r=(x,y);
inline void UpdateMaxMin(
						 CAphineParams &params, 
						 CVector2D &r,
						 CVector2D &r_min,
						 CVector2D &r_max)
{
	CVector2D corner;
	corner=params*r;
	if (corner.x>r_max.x) r_max.x=corner.x;
	if (corner.y>r_max.y) r_max.y=corner.y;
	if (corner.x<r_min.x) r_min.x=corner.x;
	if (corner.y<r_min.y) r_min.y=corner.y;
}

inline void GetMaxMin(
					  CAphineParams &params,
					  CVector2D r0, //координаты верхнего левого угла исходника
					  CVector2D size, //размеры прямоугольника исходника
					  CVector2D &r_min,
					  CVector2D &r_max)
{
	r_min=CVector2D( 1e20, 1e20);
	r_max=CVector2D(-1e20,-1e20);
	CVector2D r=r0;
	UpdateMaxMin(params, r, r_min, r_max);
	r.x+=size.x;
	UpdateMaxMin(params, r, r_min, r_max);
	r.y+=size.y;
	UpdateMaxMin(params, r, r_min, r_max);
	r.x=r0.x;
	UpdateMaxMin(params, r, r_min, r_max);
}

//fixed point base
#define H 4096
inline int fix(double x)
{
	return(int(floor(x*H+0.5)));
}
inline double flo(int x)
{
	return(double(x)/H);
}



void atCalcNewSizeAndOffset(IImage2Ptr &image, CAphineParams& params,
							_size &new_size, _point &new_offset )
{
	_size size;
	_point offset;
	image->GetSize( (int*)&size.cx, (int*)&size.cy );
	image->GetOffset( &offset );
	
	CVector2D r_min,r_max;
	GetMaxMin(params,
		CVector2D(offset.x,offset.y),
		CVector2D(size.cx,size.cy),
		r_min, r_max);
	
	new_offset.x = int(floor(r_min.x));
	new_offset.y = int(floor(r_min.y));
	new_size.cx  = int(ceil(r_max.x))-new_offset.x;
	new_size.cy  = int(ceil(r_max.y))-new_offset.y;
}

void atCalcNewSizeAndOffsetMasked(IImage2Ptr &image, CAphineParams& params,
								  _size &new_size, _point &new_offset )
{
	_size size;
	_point offset;
	image->GetSize( (int*)&size.cx, (int*)&size.cy );
	image->GetOffset( &offset );
	
	CVector2D r0=params*CVector2D(offset.x+.5,offset.y+.5);
	CVector2D d_u=params.a*CVector2D(1,0);
	CVector2D d_v=params.a*CVector2D(0,1);
	
	int x0=fix(r0.x);
	int y0=fix(r0.y);
	
	int dxdu=fix(d_u.x);
	int dxdv=fix(d_v.x);
	int dydu=fix(d_u.y);
	int dydv=fix(d_v.y);
	
	int x_max=INT_MIN;
	int x_min=INT_MAX;
	int y_max=INT_MIN;
	int y_min=INT_MAX;
	
	CVector2D r1=r0;
	int x1=x0, y1=y0;
	for( long vv=0;vv<size.cy;vv++)
	{
		CVector2D r=r1;
		int x=x1, y=y1;
		byte *pmask = 0;
		image->GetRowMask(vv, &pmask);
		for( long uu = 0; uu < size.cx; uu++, pmask++)
		{
			if( *pmask==255 )
			{
				if (x>x_max) x_max=x;
				if (y>y_max) y_max=y;
				if (x<x_min) x_min=x;
				if (y<y_min) y_min=y;
			}
			r=r+d_u;
			x+=dxdu; y+=dydu;
		}
		r1=r1+d_v;
		x1+=dxdv; y1+=dydv;
	}
	
	CVector2D r_min,r_max;
	GetMaxMin(params,
		CVector2D(0,0),
		CVector2D(1,1),
		r_min, r_max); // посчитать размеры клеточки 1x1
	
	double dx=(r_max.x-r_min.x)/2;
	double dy=(r_max.y-r_min.y)/2;
	
	new_offset.x = int(floor(flo(x_min)-dx));
	new_offset.y = int(floor(flo(y_min)-dy));
	new_size.cx  = int(floor(flo(x_max)+dx))-new_offset.x;
	new_size.cy  = int(floor(flo(y_max)+dy))-new_offset.y;
}

void atMakeMask( IImage2Ptr &image, IImage2Ptr &imageNew, CAphineParams &params)
{
	int a11=(int)params.a.a11*H;
	int a12=(int)params.a.a12*H;
	int a21=(int)params.a.a21*H;
	int a22=(int)params.a.a22*H;
	
	_point offset_new, offset;
	imageNew->GetOffset( &offset_new );
	image->GetOffset( &offset );
	_size size, size_new;
	image->GetSize( (int*)&size.cx, (int*)&size.cy );
	imageNew->GetSize( (int*)&size_new.cx, (int*)&size_new.cy );
	
	double x0=params.center_new.x-offset_new.x+0.5;
	double y0=params.center_new.y-offset_new.y+0.5;
	double u0=(params.center.x-offset.x)*H-H/2;
	double v0=(params.center.y-offset.y)*H-H/2;
	
	int u1=int( (-x0)*a11+(-y0)*a12+u0 );
	int v1=int( (-x0)*a21+(-y0)*a22+v0 );
	for( long yy=0;yy<size_new.cy;yy++)
	{
		int u=u1;
		int v=v1;
		byte *pDst;
		imageNew->GetRowMask( yy, &pDst );
		for( long xx = 0; xx < size_new.cx; xx++, pDst++)
		{
			if (u<0 || v<0 || u>=size.cx*H || v>=size.cy*H)
			{
				*pDst = 0;
			}
			else
			{
				int iu=int(u/H);
				int iv=int(v/H);
				
				byte	*ptemp = 0;
				image->GetRowMask( iv, &ptemp );
				*pDst = ptemp[iu];
			}
			u+=a11; v+=a21;
		}
		u1+=a12; v1+=a22;
	}
}

static int GetPaneCount(IImage2Ptr image1)
{
	return ::GetImagePaneCount( image1 );
}

static bool IsPaneEnable( int nPane, int nPaneCount )
{
	return (nPane<nPaneCount);
}
// старая ф-я - только для теста
bool atExecute1( IImage2Ptr &image, IImage2Ptr &imageNew,
				CAphineParams& params,
				bool (*pCallBack)(int, long), long lParam )
{
	int nPanes=GetPaneCount(image);

	params.a=~params.a; //затычка - т.к. изменился код
	int percent=0;
	if(!pCallBack(percent, lParam)) return(false);
	
	atMakeMask(image, imageNew, params);
	
	byte *pmask;
	
	int a11=(int)params.a.a11*H;
	int a12=(int)params.a.a12*H;
	int a21=(int)params.a.a21*H;
	int a22=(int)params.a.a22*H;
	
	int nx=int(ceil(fabs(params.a.a11)+fabs(params.a.a12)));
	int ny=int(ceil(fabs(params.a.a21)+fabs(params.a.a22)));
	
	_point offset_new, offset;
	imageNew->GetOffset( &offset_new );
	image->GetOffset( &offset );
	_size size, size_new;
	image->GetSize( (int*)&size.cx, (int*)&size.cy );
	imageNew->GetSize( (int*)&size_new.cx, (int*)&size_new.cy );
	
	double x0=params.center_new.x-offset_new.x-0.5/nx;
	double y0=params.center_new.y-offset_new.y-0.5/ny;
	double u0=(params.center.x-offset.x)*H-H/2;
	double v0=(params.center.y-offset.y)*H-H/2;
	
	int b11=a11/nx; int b21=a21/nx;
	int b12=a12/ny; int b22=a22/ny;
	
	int perc1=0;
	int nperc1=0;
	
	
	int 	npanes = ::GetImagePaneCount( image );
	
	{for( int nPane=0;nPane<npanes;nPane++ )
		if(IsPaneEnable(nPane,nPanes)) nperc1+=size_new.cy;}	
	
	for( int nPane=0;nPane<npanes;nPane++ )
		if(IsPaneEnable(nPane,nPanes))
		{
			int u1=int( (-x0)*a11+(-y0)*a12+u0 );
			int v1=int( (-x0)*a21+(-y0)*a22+v0 );
			for( long yy=0;yy<size_new.cy;yy++)
			{
				color* pDst = 0;
				imageNew->GetRow( yy, nPane, &pDst );
				int u2=u1;
				int v2=v1;
				for( long xx = 0; xx < size_new.cx; xx++, pDst++)
				{
					int u3=u2;
					int v3=v2;
					int ww=0;
					for(long iy=0; iy<ny; iy++)
					{
						int u=u3;
						int v=v3;
						for(long ix=0; ix<nx; ix++)
						{
							int iu=int(u/H);
							int au=u-iu*H;
							if(u<0) {iu=0; au=0;}
							if(u>=(size.cx-1)*H) {iu=size.cx-2; au=H;}
							
							int iv=int(v/H);
							int av=v-iv*H;
							if(v<0) {iv=0; av=0;}
							if(v>=(size.cy-1)*H) {iv=size.cy-2; av=H;}
							
							pmask = 0;
							image->GetRowMask( iv, &pmask );
							pmask+=iu;
							if( *pmask==255 )
							{
								color* pSrc = 0;
								image->GetRow( iv, nPane, &pSrc );
								pSrc+=iu;
								
								int w0=*(pSrc++)*(H-au)/H;
								w0+=*pSrc*au/H;
								
								image->GetRow( iv+1, nPane, &pSrc );
								pSrc+=iu;
								
								int w1=*(pSrc++)*(H-au)/H;
								w1+=*pSrc*au/H;
								int w=(w0*(H-av)+w1*av)/H;
								ww+=w;
							}
							u+=b11; v+=b21;
						}
						u3+=b12; v3+=b22;
					}
					ww/=(nx*ny);
					*pDst = int(ww);
					u2+=a11; v2+=a21;
				}
				u1+=a12; v1+=a22;
				perc1++;
				int perc2=perc1*100/nperc1;
				while(percent<=perc2)
				{
					if(!pCallBack(percent, lParam)) return(false);
					percent++;
				}
			}
		}
		dbg_assert(percent!=100);
		return true;
}

bool atExecuteFast( IImage2Ptr &image, IImage2Ptr &imageNew,
				   CAphineParams& params,
				   bool (*pCallBack)(int, long), long lParam )
{
	bool result=true;
	int percent=0;
	if(!pCallBack(percent, lParam)) return(false);

	int nPanes=GetPaneCount(image);
	
	bool bIsPaneEnable0 = IsPaneEnable( 0, nPanes );
	bool bIsPaneEnable1 = IsPaneEnable( 1, nPanes );
	bool bIsPaneEnable2 = IsPaneEnable( 2, nPanes );
	bool bIsPaneEnable3 = IsPaneEnable( 3, nPanes );
	
	_point offset_new, offset;
	imageNew->GetOffset( &offset_new );
	image->GetOffset( &offset );
	_size size, size_new;
	image->GetSize( (int*)&size.cx, (int*)&size.cy );
	imageNew->GetSize( (int*)&size_new.cx, (int*)&size_new.cy );
	
	CAphineParams par1=~params;
	CVector2D r0=par1*CVector2D(offset_new.x+.5,offset_new.y+.5);
	CVector2D d_x=par1.a*CVector2D(1,0);
	CVector2D d_y=par1.a*CVector2D(0,1);
	
	int u0=fix(r0.x-offset.x);
	int v0=fix(r0.y-offset.y);
	
	int dudx=fix(d_x.x);
	int dvdx=fix(d_x.y);
	int dudy=fix(d_y.x);
	int dvdy=fix(d_y.y);
	
	int perc1=0;
	int nperc1=size_new.cy;
	
	color** rows0=new color*[size.cy];
	color** rows1=new color*[size.cy];
	color** rows2=new color*[size.cy];
	byte**	rowsm=new byte*[size.cy];
	for(int y=0; y<size.cy; y++)
	{
		image->GetRow(y,0, &rows0[y]);
		image->GetRow(y,1, &rows1[y]);
		image->GetRow(y,2, &rows2[y]);
		image->GetRowMask(y, &rowsm[y]);
	}
	
	//CVector2D r1=r0;
	int u1=u0;
	int v1=v0;
	for( long yy=0;yy<size_new.cy;yy++)
	{
		color* pDst0, *pDst1, *pDst2;
		imageNew->GetRow( yy, 0, &pDst0 );
		imageNew->GetRow( yy, 1, &pDst1 );
		imageNew->GetRow( yy, 2, &pDst2 );
		byte*  pDstMask = 0;
		imageNew->GetRowMask( yy, &pDstMask );
		
		int u=u1;
		int v=v1;
		for( long xx = 0; xx < size_new.cx; xx++, pDst0++,pDst1++,pDst2++,pDstMask++)
		{
			if (u>=0 && u<size.cx*H && v>=0 && v<size.cy*H)
			{
				int iu=u/H;
				int iv=v/H;
				
				byte *pmask =  rowsm[iv]+iu;
				
				if( (*pDstMask=*pmask)==255 )
				{
					if(bIsPaneEnable0)
						*pDst0 =  rows0[iv][iu];
					if(bIsPaneEnable1)
						*pDst1 = rows1[iv][iu];
					if(bIsPaneEnable2)
						*pDst2 = rows2[iv][iu];
				}
			}
			else
			{
				*pDstMask=0;
			}
			u+=dudx; v+=dvdx;
		}
		
		u1+=dudy; v1+=dvdy;
		perc1++;
		int perc2=perc1*100/nperc1;
		while(percent<=perc2)
		{
			if(!pCallBack(percent, lParam))
			{
				result=false;
				goto m1; //безобразие, конечно... лучше бы
				//try поставить или через флажки
			}
			percent++;
		}
	}
	dbg_assert(percent!=100);
	
m1:
	delete [] rows0;
	delete [] rows1;
	delete [] rows2;
	delete [] rowsm;
	
	return result;
}

bool atExecute( IImage2Ptr &image, IImage2Ptr &imageNew,
			   CAphineParams& params,
			   bool (*pCallBack)(int, long), long lParam )
{
	int nPanes=GetPaneCount(image);
	bool bIsPaneEnable0 = IsPaneEnable( 0, nPanes );
	bool bIsPaneEnable1 = IsPaneEnable( 1, nPanes );
	bool bIsPaneEnable2 = IsPaneEnable( 2, nPanes );
	bool bIsPaneEnable3 = IsPaneEnable( 3, nPanes );
	bool result=true;
	int percent=0;
	if(!pCallBack(percent, lParam)) return(false);
	
	//atMakeMask(image, imageNew, params);
	
	byte *pmask;
	
	_point offset_new, offset;
	imageNew->GetOffset( &offset_new );
	image->GetOffset( &offset );
	_size size, size_new;
	image->GetSize( (int*)&size.cx, (int*)&size.cy );
	imageNew->GetSize( (int*)&size_new.cx, (int*)&size_new.cy );
	
	CAphineParams par1=~params;
	
	CVector2D r_min,r_max;
	GetMaxMin(par1,
		CVector2D(0,0),
		CVector2D(1,1),
		r_min, r_max); // посчитать размеры клеточки 1x1
	int nx=int(ceil(r_max.x-r_min.x));
	int ny=int(ceil(r_max.y-r_min.y));
	
	CVector2D r0=par1*CVector2D(offset_new.x+.5/nx,offset_new.y+.5/ny);
	CVector2D d_x=par1.a*CVector2D(1,0);
	CVector2D d_y=par1.a*CVector2D(0,1);
	
	int u0=fix(r0.x-offset.x-0.5);
	int v0=fix(r0.y-offset.y-0.5);
	
	int dudx=fix(d_x.x);
	int dvdx=fix(d_x.y);
	int dudy=fix(d_y.x);
	int dvdy=fix(d_y.y);
	
	int dudx1=fix(d_x.x/nx);
	int dvdx1=fix(d_x.y/nx);
	int dudy1=fix(d_y.x/ny);
	int dvdy1=fix(d_y.y/ny);
	
	int perc1=0;
	int nperc1=size_new.cy;
	
	color** rows0=new color*[size.cy];
	color** rows1=new color*[size.cy];
	color** rows2=new color*[size.cy];
	color** rows3=new color*[size.cy];
	byte**	rowsm=new byte*[size.cy];
	for(int y=0; y<size.cy; y++)
	{
		image->GetRow(y,0, &rows0[y]);
		image->GetRow(y,1, &rows1[y]);
		image->GetRow(y,2, &rows2[y]);
		image->GetRow(y,3, &rows3[y]);
		image->GetRowMask(y, &rowsm[y]);
	}
	
	//CVector2D r1=r0;
	int u1=u0;
	int v1=v0;
	for( long yy=0;yy<size_new.cy;yy++)
	{
		color* pDst0, *pDst1, *pDst2, *pDst3;
		imageNew->GetRow( yy, 0, &pDst0 );
		imageNew->GetRow( yy, 1, &pDst1 );
		imageNew->GetRow( yy, 2, &pDst2 );
		imageNew->GetRow( yy, 3, &pDst3 );
		byte*  pDstMask = 0;
		imageNew->GetRowMask( yy, &pDstMask );
		
		int u2=u1;
		int v2=v1;
		for( long xx = 0; xx < size_new.cx; xx++, pDst0++,pDst1++,pDst2++,pDst3++,pDstMask++)
		{
			int w0,w1,w2,w3,wm;
			w0=w1=w2=w3=wm=0;
			int u3=u2;
			int v3=v2;
			for( long iy = 0; iy < ny; iy++)
			{
				int u=u3;
				int v=v3;
				for( long ix = 0; ix < nx; ix++)
				{
					if (u>=0 && u<(size.cx-1)*H && v>=0 && v<(size.cy-1)*H)
					{
						int iu=u/H;
						int iv=v/H;
						int au=u & (H-1);
						int av=v & (H-1);
						int a00=(H-au)*(H-av)/(H*nx*ny);
						int a01=au*(H-av)/(H*nx*ny);
						int a10=(H-au)*av/(H*nx*ny);
						int a11=au*av/(H*nx*ny);
						
						pmask = rowsm[iv]+iu;
						
						{
							color* 
								pSrc = rows0[iv]+iu;
							w0+=(*pSrc++)*a00; w0+=(*pSrc)*a01;
							pSrc = rows0[iv+1]+iu;
							w0+=(*pSrc++)*a10; w0+=(*pSrc)*a11;
						}
						{
							color* 
								pSrc = rows1[iv]+iu;
							w1+=(*pSrc++)*a00; w1+=(*pSrc)*a01;
							pSrc = rows1[iv+1]+iu;
							w1+=(*pSrc++)*a10; w1+=(*pSrc)*a11;
						}
						{
							color* 
								pSrc = rows2[iv]+iu;
							w2+=(*pSrc++)*a00; w2+=(*pSrc)*a01;
							pSrc = rows2[iv+1]+iu;
							w2+=(*pSrc++)*a10; w2+=(*pSrc)*a11;
						}
						if(bIsPaneEnable3)
						{
							color* 
								pSrc = rows3[iv]+iu;
							w3+=(*pSrc++)*a00; w3+=(*pSrc)*a01;
							pSrc = rows3[iv+1]+iu;
							w3+=(*pSrc++)*a10; w3+=(*pSrc)*a11;
						}
						{
							byte* 
								pSrc = rowsm[iv]+iu;
							wm+=(*pSrc++)*a00; wm+=(*pSrc)*a01;
							pSrc = rowsm[iv+1]+iu;
							wm+=(*pSrc++)*a10; wm+=(*pSrc)*a11;
						}
					}
					else
					{ // тот же код, но с защитой от выхода за пределы
						int iu=u/H;
						int iv=v/H;
						int au=u & (H-1);
						int av=v & (H-1);
						if(u<0) {iu=0; au=0;}
						if(u>=(size.cx-1)*H) {iu=size.cx-2; au=H;}
						if(v<0) {iv=0; av=0;}
						if(v>=(size.cy-1)*H) {iv=size.cy-2; av=H;}
						int a00=(H-au)*(H-av)/(H*nx*ny);
						int a01=au*(H-av)/(H*nx*ny);
						int a10=(H-au)*av/(H*nx*ny);
						int a11=au*av/(H*nx*ny);
						
						image->GetRowMask( iv, &pmask );
						pmask+=iu;
						
						{
							color* pSrc;
							image->GetRow( iv, 0, &pSrc );pSrc+=iu;
							w0+=(*pSrc++)*a00; w0+=(*pSrc)*a01;
							image->GetRow( iv+1, 0, &pSrc );pSrc+=iu;
							w0+=(*pSrc++)*a10; w0+=(*pSrc)*a11;
						}
						{
							color* pSrc;
							image->GetRow( iv, 1, &pSrc );pSrc+=iu;
							w1+=(*pSrc++)*a00; w1+=(*pSrc)*a01;
							image->GetRow( iv+1, 1, &pSrc );pSrc+=iu;
							w1+=(*pSrc++)*a10; w1+=(*pSrc)*a11;
						}
						{
							color* pSrc;
							image->GetRow( iv, 2, &pSrc );pSrc+=iu;
							w2+=(*pSrc++)*a00; w2+=(*pSrc)*a01;
							image->GetRow( iv+1, 2, &pSrc );pSrc+=iu;
							w2+=(*pSrc++)*a10; w2+=(*pSrc)*a11;
						}
						if(bIsPaneEnable3)
						{
							color* pSrc;
							image->GetRow( iv, 3, &pSrc );pSrc+=iu;
							w3+=(*pSrc++)*a00; w3+=(*pSrc)*a01;
							image->GetRow( iv+1, 3, &pSrc );pSrc+=iu;
							w3+=(*pSrc++)*a10; w3+=(*pSrc)*a11;
						}
						if(u+H/2>=0 && u+H/2<size.cx*H &&
							v+H/2>=0 && v+H/2<size.cy*H) 
						{
							byte* pSrc;
							image->GetRowMask( iv, &pSrc );pSrc+=iu;
							wm+=(*pSrc++)*a00; wm+=(*pSrc)*a01;
							image->GetRowMask( iv+1, &pSrc );pSrc+=iu;
							wm+=(*pSrc++)*a10; wm+=(*pSrc)*a11;
						}
					}
					u+=dudx1; v+=dvdx1;
				}
				u3+=dudy1; v3+=dvdy1;
			}
			if(wm>=255*H/2)
			{
				*pDstMask=255 ;
				if(bIsPaneEnable0)
					*pDst0 = w0/H;
				if(bIsPaneEnable1)
					*pDst1 = w1/H;
				if(bIsPaneEnable2)
					*pDst2 = w2/H;
				if(bIsPaneEnable3)
					*pDst3 = w3/H;
			}
			else
			{
				*pDstMask=0;
			}
			u2+=dudx; v2+=dvdx;
		}
		
		u1+=dudy; v1+=dvdy;
		perc1++;
		int perc2=perc1*100/nperc1;
		while(percent<=perc2)
		{
			if(!pCallBack(percent, lParam))
			{
				result=false;
				goto m1; //безобразие, конечно... лучше бы
				//try поставить или через флажки
			}
			percent++;
		}
	}
	dbg_assert(percent!=100);
	
m1:
	delete [] rows0;
	delete [] rows1;
	delete [] rows2;
	delete [] rowsm;
	
	return result;
}
