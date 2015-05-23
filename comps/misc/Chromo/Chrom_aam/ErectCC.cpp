#include "stdafx.h"
#include "ErectCC.h"
#include "misc_utils.h"
#include <math.h>
#include <limits.h>
#include "measure5.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "ImagePane.h"
#include "alloc.h"
#include "aam_utils.h"
#include "ContourApi.h"
#include "chrom_aam_utils.h"
#include "aphine.h"

#include "docview5.h"

#define MaxColor color(-1)

IErectCC::IErectCC()
{
	m_img=0;
	m_faxis.free();
}

IErectCC::~IErectCC()
{
}

IUnknown* IErectCC::DoGetInterface( const IID &iid )
{
	if( iid == IID_IChromoConvert)return (IChromoConvert*)this;
	return ComObjectBase::DoGetInterface(iid);
}

static bool _callback_dummy(int, long)
{
	return(true);
}

typedef _ptr_t2<_dpoint> dpoint_ptr;

static void MakeSmoothAxis(_point *axis, int lAxisSize,
						   dpoint_ptr *smooth_axis, dpoint_ptr *vect_axis, int *new_len)
// На входе - ось axis и ее длина lAxisSize.
// На выходе - сглаженная ось smooth_axis, причем длиннее, чем исходная,
// орт ее направления в каждой точке vect_axis
// и количество точек new_len
{
	const int zoom2=16;
	const int lAdd=5; //на столько точек (физических) удлиннять ось

	//скопируем ее во вспомогательный массив, увеличим число точек
	int len2=(lAxisSize-1)*zoom2+1;
	smart_alloc(daxis0, _dpoint, len2);
	{for(int i=0; i<lAxisSize; i++) daxis0[i*zoom2]=axis[i];}
	{for(int i=0; i<lAxisSize-1; i++)
	{
		for(int j=1; j<zoom2; j++)
		{
			int ii=i*zoom2;
			double a=double(j)/zoom2;
			daxis0[ii+j].x = daxis0[ii].x*(1-a) + daxis0[ii+zoom2].x*a;
			daxis0[ii+j].y = daxis0[ii].y*(1-a) + daxis0[ii+zoom2].y*a;
		}
	}}

	//посчитать вектора
	smart_alloc(daxisvect0, _dpoint, len2);
	{ //посчитаем вектора направления
		for(int i=0; i<len2; i++) 
		{
			int i0=max(i-3*zoom2,0);
			int i1=min(i+3*zoom2,len2-1);
			double dx=daxis0[i1].x-daxis0[i0].x;
			double dy=daxis0[i1].y-daxis0[i0].y;
			double d=_hypot(dx,dy); d=max(d,1e-5);
			daxisvect0[i].x=dx/d; daxisvect0[i].y=dy/d;
		}
	}


	//прикинем длину объекта
	double dlen=0;
	for(int i=1; i<len2-1; i++)
	{
		dlen+=
			(daxis0[i].x-daxis0[i-1].x)*(daxisvect0[i].x+daxisvect0[i-1].x)/2+
			(daxis0[i].y-daxis0[i-1].y)*(daxisvect0[i].y+daxisvect0[i-1].y)/2;
		//прибавляем к длине проекцию на ось
	}

	long len=int(ceil(dlen));
	smart_alloc(daxis, _dpoint, len);
	smart_alloc(daxisvect, _dpoint,	len);

	dlen=0;
	int j=0;
	for(i=1; i<len2; i++)
	{
		while(dlen>=j && j<len)
		{
			daxis[j]=daxis0[i];
			daxisvect[j]=daxisvect0[i];
			j++;
		}
		dlen+=
			(daxis0[i].x-daxis0[i-1].x)*(daxisvect0[i].x+daxisvect0[i-1].x)/2+
			(daxis0[i].y-daxis0[i-1].y)*(daxisvect0[i].y+daxisvect0[i-1].y)/2;
	}
	while(dlen>=j && j<len) //вроде не надо, но на всякий случай
	{
		daxis[j]=daxis0[len2-1];
		daxisvect[j]=daxisvect0[len2-1];
		j++;
	}

	smart_alloc(shift, double, len); //сдвиг точки оси
	//сглаживание оси.
	for(i=0; i<len; i++)
	{ //найдем, насколько точка оси сдвинута по перпендикуляру к оси относительно средней линии по ближайшим точкам
		double s=0;
		int n=0;
		for(j=-2; j<=2; j++)
		{
			int i1=i-j*5;
			if(i1>=0 && i1<len)
			{
				s+=	(daxis[i].x-daxis[i1].x)*(daxisvect[i].y)-
					(daxis[i].y-daxis[i1].y)*(daxisvect[i].x);
				n++;
			}
		}
		shift[i]=s/n;
	}
	for(i=0; i<len; i++)
	{ //сдвинем точки перпендикулярно оси
		daxis[i].x -= shift[i]*daxisvect[i].y;
		daxis[i].y += shift[i]*daxisvect[i].x;
	}

	//передать результаты
	smooth_axis->attach(smart_daxis.detach());
	vect_axis->attach(smart_daxisvect.detach());
	*new_len=len;
}

//(3) #define DISTWEIGHT(m)   ((m)<128 ? 4:1)
#define DISTWEIGHT(m) 1


#define DISTTEST(ind,ind1,r)            \
	if(dist_map[ind]>dist_map[ind1]+r)	\
	{									\
		dist_map[ind]=dist_map[ind1]+r;	\
		num_map[ind]=num_map[ind1];		\
	}

static void CreateMaps(_dpoint *axis, int len,
					   int *num_map, int *dist_map,
					   byte **mask, int cx, int cy)
{
	//создать карты номеров точек и дистанции от оси
	//вход: axis - собственно ось длины len,
	//выход: num_map и dist_map - массивы cy*cx (по строкам),
	//в которые заносится номер ближайшей точки оси и расстояние до нее
	int x,y,i,ind;

	//инициализация массивов - дистанция максимальна
	for(y=0; y<cy; y++)
	{
		for(x=0; x<cx; x++)
		{
			ind=y*cx+x;
			dist_map[ind]=65535; //далеко :-)
			num_map[ind]=0; //проинициализируем на всякий пожарный
		}
	}

	//заполнение точек на кривой
	for(i=0; i<len; i++)
	{
		x=int(axis[i].x+0.5);
		y=int(axis[i].y+0.5);
		int x1=max(1,min(x,cx-2)); //точка внутри, не доходя до границы
		int y1=max(1,min(y,cy-2)); //точка внутри, не доходя до границы
		ind=y1*cx+x1;
		int dist=abs(x1-x)+abs(y1-y); //если точку подвинули - нарастим длину
		num_map[ind]=i;
		dist_map[ind]=min(dist_map[ind],dist); //если там более близкая точка - не трогать
	}

	for(i=0; i<2; i++) //для метода (3) - только один проход!
	{
		//проход слева направо и сверху вниз
		for(y=1; y<cy; y++)
		{
			for(x=1; x<cx; x++)
			{
				ind=y*cx+x;
				int weight = DISTWEIGHT(mask[y][x]); //по пустым точкам дистанция больше
				DISTTEST(ind,ind-1,weight);
				DISTTEST(ind,ind-cx,weight);
			}
		}

		//проход справа налево и снизу вверх
		for(y=cy-2; y>=0; y--)
		{
			for(x=cx-2; x>=0; x--)
			{
				ind=y*cx+x;
				int weight = DISTWEIGHT(mask[y][x]); //по пустым точкам дистанция больше
				DISTTEST(ind,ind+1,weight);
				DISTTEST(ind,ind+cx,weight);
			}
		}
	}

	//можно добавить еще проходы для более точного вычисления расстояний -
	//лучше перед этими двумя.
	//Скажем, на 3 точки по одной оси и на 4 по другой, r=5;

}

static void CalcWidth(_dpoint *daxis, _dpoint *vect,
					  int *width, int len,
					  int *num_map, int *dist_map, 
					  byte **mask, int cx, int cy,
					  int direction)
{
	//посчитать ширину хромосомы - вправо и влево от оси
	//вход: axis и axisvect - собственно ось длины len
	//и орт направления оси для поиска в каждой точке
	//num_map и dist_map - привязка точек к оси (cx*cy)
	//mask - маска объекта (cx*cy)
	//direction - +1=влево, -1=вправо

	for(int i=0; i<len; i++)
	{
		width[i]=0; //инициализация
		double dx = vect[i].y*direction;
		double dy = -vect[i].x*direction;
		int dist=0;
		for(int j=0; j<100; j++)
		{
			double xd=daxis[i].x+dx*j;
			double yd=daxis[i].y+dy*j;
			int xx=int(xd+0.5);
			int yy=int(yd+0.5);
			byte mm=0;
			if(xx>=0 && xx<cx && yy>=0 && yy<cy) mm=mask[yy][xx];
			else break; // вышли из прямоугольника - хватит работать
			//внутри прямоугольника - смотрим на маску
			//если вышли за него - бросаем работу
				
			//(3) dist += DISTWEIGHT(mm); //по пустым точкам дистанция больше
			if(mm)
			{
				//(3)if(dist<=dist_map[yy*cx+xx]+1) width[i]=j;
				int i1=num_map[yy*cx+xx]; //номер "ближайшей" точки оси
				double rr=_hypot(xx-daxis[i1].x, yy-daxis[i1].y); //можно что-нибудь int и пошустрее сделать
				//double rr=(xx-daxis[i1].x)*vect[i].y-(yy-daxis[i1].y)*vect[i].x;
				//rr=fabs(rr);
				if(j<=rr+3) width[i]=j;
				//&& j<dist_map[yy*cx+xx]*1.4 //точку засчитываем, только если расстояние от оси соответствует
				//&& abs(num_map[yy*cx+xx]-i)<j+2 //точку засчитываем, только если она принадлежит достаточно близкому участку оси
			}
		}
	}
}

HRESULT IErectCC::Invalidate()
{
	m_img=0;
	return S_OK;
}

static inline int iround(double x)
{
	return int(floor(x+0.5));
}

HRESULT IErectCC::Recalc(IChromosome *pChromo)
{
	if(pChromo==0) return S_FALSE;
	
	IUnknown *punkO=0;
	INamedDataObject2Ptr(pChromo)->GetParent(&punkO);
	IMeasureObjectPtr ptrO(punkO);
	if(punkO!=0) punkO->Release();
	if(ptrO==0) return S_FALSE;

	IUnknown *punk=0;
	ptrO->GetImage(&punk);
	if(punk!=0) punk->Release();
	IImage3Ptr image(punk);
	if(image==0) return S_FALSE;

	if(image==0) return(E_FAIL);


	IChromosome *ptrC=pChromo;

	ROTATEPARAMS rot;
	ptrC->GetRotateParams(&rot);

	//длина оси
	long lAxisSize=0;
	BOOL bIsManual;
	ptrC->GetAxisSize(AT_Original|AT_ShortAxis, &lAxisSize, &bIsManual);
	if(lAxisSize<4) return 0; //с короткими осями - не выпрямляем

	//получим повернутую ось - для проверки направления
//	smart_alloc(axis_rot, _point, lAxisSize);
//	ptrC->GetAxis(AT_Original|AT_Rotated|AT_ShortAxis, axis_rot);
	bool bAxisBottomToTop = false;//axis_rot[lAxisSize-1].y<axis_rot[0].y;

	BOOL bRotated180 = false;//axis_rot[lAxisSize-1].y<axis_rot[0].y;
	ptrC->IsMirrorV(&bRotated180);
	bAxisBottomToTop = (bRotated180!=FALSE);

	//получим саму ось
	smart_alloc(axis, _point, lAxisSize);
	ptrC->GetAxis(AT_Original|AT_ShortAxis, axis);
	//если кверх ногами - перевернем ось
	if(bAxisBottomToTop)
	{
		for(int i=0; i<lAxisSize/2; i++) 
		{
			_point p=axis[i];
			axis[i]=axis[lAxisSize-1-i];
			axis[lAxisSize-1-i]=p;
		}
	}

	dpoint_ptr daxis;
	dpoint_ptr daxisvect;
	int len;
	MakeSmoothAxis(axis, lAxisSize, &daxis, &daxisvect, &len);

	_point offset;
	image->GetOffset(&offset);
	int cx,cy;
	image->GetSize(&cx,&cy);

	_bstr_t	bstrCC;
	int nPanes;
	{
		IUnknown *punkCC = 0;
		image->GetColorConvertor( &punkCC );
		IColorConvertor2Ptr	ptrCC( punkCC );
		if(punkCC!=0) punkCC->Release();
		if(ptrCC==0) return E_FAIL;

		nPanes = ::GetImagePaneCount( image );

		BSTR	bstr;
		ptrCC->GetCnvName( &bstr );
		bstrCC = bstr;
		::SysFreeString( bstr ); 
	}

	smart_alloc(srcRows, color*, cy*nPanes);
	{for(int pane=0; pane<nPanes; pane++)
	{
		for(int y=0; y<cy; y++)
		{
			image->GetRow(y, pane, srcRows+pane*cy+y);
		}
	}}

	smart_alloc(srcMask, byte*, cy);
	{for(int y=0; y<cy; y++)
	{
		image->GetRowMask(y, srcMask+y);
	}}

	//построим карту для определения по координатам точки индекса на оси
	smart_alloc(num_map, int, cx*cy);
	smart_alloc(dist_map, int, cx*cy); //выделим место под карты
	CreateMaps(daxis, len, num_map, dist_map, srcMask, cx,cy); //обсчитаем карты
	  
	//посчитаем ширину нового объекта
	int width2=100; //половина ширины;

	//установим центральную линию
	int lNewAxisSize;

	{	//установим короткую ось
		//длина оси
		long lShortAxisSize=0;
		BOOL bIsManual;
		ptrC->GetAxisSize(AT_Original|AT_ShortAxis, &lShortAxisSize, &bIsManual);

		//получим саму ось
		smart_alloc(faxis, FPOINT, max(lShortAxisSize,2));
		ptrC->GetAxis(AT_Original|AT_ShortAxis|AT_AxisDouble, (POINT *)faxis);
		if(bAxisBottomToTop)
		{
			for(int i=0; i<lShortAxisSize/2; i++) 
			{
				_fpoint p=faxis[i];
				faxis[i]=faxis[lShortAxisSize-1-i];
				faxis[lShortAxisSize-1-i]=p;
			}

		}
		if(lShortAxisSize<2) faxis[1]=faxis[0];

		lNewAxisSize=max(lShortAxisSize,2);
		m_faxis.alloc(lNewAxisSize);

		for(int j=0; j<lShortAxisSize; j++)
		{
			//double t=(j*2-lShortAxisSize+1.0)/(lShortAxisSize-1.0); //-1..1
			int i_best=0; 
			double r_best=1e8;
			for(int i=0; i<len-1; i++)
			{	//найдем ближайшую точку сглаженной оси
				double dx=faxis[j].x-daxis[i].x;
				double dy=faxis[j].y-daxis[i].y;
				double r=dx*daxisvect[i].x+dy*daxisvect[i].y; //проекция на ось
				double r2=_hypot(dx,dy); //расстояние до оси
				
				const double len_mul=1.2; //!!!!
				double t=(i*2-len+1.0)/len*len_mul;
				double adjust0=0; //!!!!
				double adjust1=1; //!!!!
				t=t*adjust1+adjust0;
				double t1=(j*2-lShortAxisSize+1.0)/max(lShortAxisSize,1);
				double r3=fabs(t-t1); //разница в относительной позиции точек на оси

				r=fabs(r)+r2/4+r3*8;
				if(r<r_best)
				{
					i_best=i;
					r_best=r;
				}
			}
			//int i=j*(len-1)/(lShortAxisSize-1);
			i=i_best;
			double xd=axis[j].x-daxis[i].x;
			double yd=axis[j].y-daxis[i].y;
			double xd1=xd*daxisvect[i].y-yd*daxisvect[i].x;
			double yd1=yd*daxisvect[i].y+xd*daxisvect[i].x;
			//xd1=0; yd1=0;

			m_faxis[j]=_dpoint(width2+xd1,i+yd1); //поправить - чтоб поворот был
		}
	}

	// Установим центромеру
	_point ptCen;
	_point ptDraw[2];
	ptrC->GetCentromereCoord(AT_Original, &ptCen, ptDraw, &bIsManual);
	{
		int i_best=0;
		double r_best=1e20;
		for(int i=0; i<len; i++)
		{
			double r=_hypot(ptCen.x-daxis[i].x,ptCen.y-daxis[i].y);
			if(r<r_best)
			{
				i_best=i;
				r_best=r;
			}
		}
		m_ptCen=_point(width2,i_best);
		double r0=_hypot(ptDraw[0].x-daxis[i_best].x, ptDraw[0].y-daxis[i_best].y);
		m_ptDraw[0]=_point(width2+int(r0+0.5),i_best);
		double r1=_hypot(ptDraw[1].x-daxis[i_best].x, ptDraw[1].y-daxis[i_best].y);
		m_ptDraw[1]=_point(width2-int(r1+0.5),i_best);

		//лучше так:
		for(int j=0; j<2; j++)
		{
			int i=i_best;
            double xd=ptDraw[j].x-daxis[i].x;
			double yd=ptDraw[j].y-daxis[i].y;
			double xd1=xd*daxisvect[i].y-yd*daxisvect[i].x;
			double yd1=yd*daxisvect[i].y+xd*daxisvect[i].x;
			//xd1=0; yd1=0;

			m_ptDraw[j]=_point(width2+iround(xd1),i/*+int(yd1)*/);
		}
	}

	IUnknown *punk2 = ::CreateTypedObject(_bstr_t(szTypeImage));
	IImage3Ptr imgOut(punk2);
	punk2->Release();

	imgOut->CreateImage(width2*2+1,len,bstrCC, ::GetImagePaneCount( image ) );
	imgOut->InitRowMasks();

	offset.x+=cx/2; offset.y+=cy/2;
	offset.x-=width2; offset.y-=len/2;
	imgOut->SetOffset(offset,false);

	int x_min=0, x_max=0;
	int y_min=len-1, y_max=0;

	{//fill image
		smart_alloc(pc, color*, nPanes);
		smart_alloc(left_width, int, len);
		smart_alloc(right_width, int, len);

		CalcWidth(daxis, daxisvect, left_width, len,
			num_map, dist_map, srcMask, cx,cy, -1);
		CalcWidth(daxis, daxisvect, right_width, len,
			num_map, dist_map, srcMask, cx,cy, 1);

		for(int y=0; y<len; y++)
		{
			byte *m;
			imgOut->GetRowMask(y,&m);
			m+=width2;
			for(int pane=0; pane<nPanes; pane++)
			{
				imgOut->GetRow(y, pane, pc+pane);
				pc[pane]+=width2;
			}

			for(int x=0; x<=width2; x++)
			{
				double xd=daxis[y].x+daxisvect[y].y*x;
				double yd=daxis[y].y-daxisvect[y].x*x;
				{
					int xx=int(xd);	xx=max(min(xx,cx-2),0);
					int yy=int(yd);	yy=max(min(yy,cy-2),0);
					double ax=xd-xx, ay=yd-yy;
					if(ax<0) ax=0;
					if(ax>1) ax=1;
					if(ay<0) ay=0;
					if(ay>1) ay=1;
					double mm=
						(srcMask[yy][xx]*(1-ax)+srcMask[yy][xx+1]*ax)*(1-ay)+
						(srcMask[yy+1][xx]*(1-ax)+srcMask[yy+1][xx+1]*ax)*ay;
					if(xd<0 || xd>cx || yd<0 || yd>cy) mm=0; //за краями затыкать
					if(x>right_width[y]) mm=0; //за краями затыкать
					if(mm<128)
					{
						m[x]=0;
					}
					else
					{
						m[x] = 255;
						if(x<x_min) x_min=x;
						if(x>x_max) x_max=x;
						if(y>y_max) y_max=y;
						if(y<y_min) y_min=y;
					}
					for(int pane=0; pane<nPanes; pane++)
					{
						int c00 = srcRows[pane*cy+yy][xx];
						int c01 = srcRows[pane*cy+yy][xx+1];
						int c10 = srcRows[pane*cy+yy+1][xx];
						int c11 = srcRows[pane*cy+yy+1][xx+1];
						if(pane==3)
						{
							if(c00==0) c00=128*256; //для ratio pane (CGH) - дефолтное значение вместо 0
							if(c01==0) c01=128*256; //для ratio pane (CGH) - дефолтное значение вместо 0
							if(c10==0) c10=128*256; //для ratio pane (CGH) - дефолтное значение вместо 0
							if(c11==0) c11=128*256; //для ratio pane (CGH) - дефолтное значение вместо 0
						}
						double cc=
							(c00*(1-ax)+c01*ax)*(1-ay)+
							(c10*(1-ax)+c11*ax)*ay;

						cc=max(min(cc,65535),0);
						pc[pane][x]=int(cc);
					}
				}
			}

			//в другую сторону
			for(x=0; x>=-width2; x--)
			{
				double xd=daxis[y].x+daxisvect[y].y*x;
				double yd=daxis[y].y-daxisvect[y].x*x;
				{
					int xx=int(xd);	xx=max(min(xx,cx-2),0);
					int yy=int(yd);	yy=max(min(yy,cy-2),0);
					double ax=xd-xx, ay=yd-yy;
					if(ax<0) ax=0;
					if(ax>1) ax=1;
					if(ay<0) ay=0;
					if(ay>1) ay=1;
					double mm=
						(srcMask[yy][xx]*(1-ax)+srcMask[yy][xx+1]*ax)*(1-ay)+
						(srcMask[yy+1][xx]*(1-ax)+srcMask[yy+1][xx+1]*ax)*ay;
					if(xd<0 || xd>cx || yd<0 || yd>cy) mm=0; //за краями затыкать
					if(-x>left_width[y]) mm=0; //за краями затыкать
					if(mm<128)
					{
						m[x]=0;
					}
					else
					{
						m[x] = 255;
						if(x<x_min) x_min=x;
						if(x>x_max) x_max=x;
						if(y>y_max) y_max=y;
						if(y<y_min) y_min=y;
					}
					for(int pane=0; pane<nPanes; pane++)
					{
						int c00 = srcRows[pane*cy+yy][xx];
						int c01 = srcRows[pane*cy+yy][xx+1];
						int c10 = srcRows[pane*cy+yy+1][xx];
						int c11 = srcRows[pane*cy+yy+1][xx+1];
						if(pane==3)
						{
							if(c00==0) c00=128*256; //для ratio pane (CGH) - дефолтное значение вместо 0
							if(c01==0) c01=128*256; //для ratio pane (CGH) - дефолтное значение вместо 0
							if(c10==0) c10=128*256; //для ratio pane (CGH) - дефолтное значение вместо 0
							if(c11==0) c11=128*256; //для ratio pane (CGH) - дефолтное значение вместо 0
						}
						double cc=
							(c00*(1-ax)+c01*ax)*(1-ay)+
							(c10*(1-ax)+c11*ax)*ay;
						if(ax<0.0 || ax>1.0 || ay<0.0 || ay>1.0) //debug
							cc=(rand()%256)*256;
						pc[pane][x]=int(cc);
					}
				}
			}
		}
	}
	imgOut->InitContours();

	

	IUnknown *punk3 = ::CreateTypedObject(_bstr_t(szTypeImage));
	m_img=IImage3Ptr(punk3);
	punk3->Release();

	if(y_max<=y_min) //на всякий пожарный
	{
		if(y_max+y_min<len) y_max=y_min+1;
		else y_min=y_max-1;
	}

	if(x_min<=-width2) x_min=-width2+1;	//оставляем хоть по 1 пикселу с краев
	if(x_max>=width2) x_max=width2-1;	//- для работы сглаживания
	m_img->CreateImage(x_max-x_min+1,y_max-y_min+1,bstrCC, ::GetImagePaneCount( image ) );
	m_img->InitRowMasks();

	int x_ofs=width2-(-x_min);
	int y_ofs=y_min;
	BOOL bMirrorV=0;

	int w=x_max-x_min+1;
	int h=y_max-y_min+1;

	CAphineParams params;

	{	//скопируем изображение из imgOut в m_img
		//ptrC->IsMirrorV(&bMirrorV);

		//bMirrorV=!bMirrorV;
		//bMirrorV=0; //пока отменим.

		/*{ //debug
			FILE *f=fopen("d:\\vt5\\ErectCC.log","a");
			fprintf(f,"rot: dAngle=%6.3f bManual=%i bMirrorV=%i bMirrorH=%i;   bMirrorV=%i\n",
				rot.dAngle, rot.bManual, rot.bMirrorV, rot.bMirrorH, bMirrorV);
			fclose(f);
		}*/

		for(int yy=0; yy<h; yy++)
		{
			int y=yy+y_ofs;
			int y1 = bMirrorV ? h-1-yy : yy;
			{	//copy mask
				//добавляю сглаживание - медианой по 9 точкам
				byte *mSrc, *mDst;
				imgOut->GetRowMask(y, &mSrc);
				byte *mSrcM=mSrc, *mSrcP=mSrc;
				if(y>0) imgOut->GetRowMask(y-1, &mSrcM);
				if(y<len-1) imgOut->GetRowMask(y+1, &mSrcP);
				mSrc+=x_ofs; mSrcM+=x_ofs; mSrcP+=x_ofs;
				m_img->GetRowMask(y1, &mDst);
				for(int x=0; x<w; x++)
				{
					int n=0;
					if(mSrcM[-1]>128) n++; if(mSrcM[0]>128) n++; if(mSrcM[1]>128) n++;
					if(mSrc[-1]>128) n++; if(mSrc[0]>128) n++; if(mSrc[1]>128) n++;
					if(mSrcP[-1]>128) n++; if(mSrcP[0]>128) n++; if(mSrcP[1]>128) n++;
					*mDst= (n>=3) ? 255 : 0;
					mDst++; mSrc++;	mSrcM++; mSrcP++;
				}
			}
			for(int pane=0; pane<nPanes; pane++)
			{ //copy panes
				color *cSrc,*cDst;
				imgOut->GetRow( y, pane, &cSrc );
				cSrc+=x_ofs;
				m_img->GetRow( y1, pane, &cDst );
				memcpy( cDst, cSrc, w*sizeof(color) );
			}
		}

		//подвинем ось и центромеру
		for(int i=0; i<lNewAxisSize; i++)
		{
			m_faxis[i].x-=x_ofs;
			m_faxis[i].y-=y_ofs;
			if(bMirrorV) m_faxis[i].y=h-1-m_faxis[i].y;
		}
		m_ptCen.x-=x_ofs; m_ptCen.y-=y_ofs;
		m_ptDraw[0].x-=x_ofs; m_ptDraw[0].y-=y_ofs;
		m_ptDraw[1].x-=x_ofs; m_ptDraw[1].y-=y_ofs;
		if(bMirrorV)
		{
			m_ptCen.y=h-1-m_ptCen.y;
			m_ptDraw[0].y=h-1-m_ptDraw[0].y;
			m_ptDraw[1].y=h-1-m_ptDraw[1].y;
		}
	}

	if(fabs(rot.dAngleVisible)>0.001 || rot.bMirrorH)
	{
		double	fZoomX = 1;
		double	fAngle = rot.dAngleVisible;
		double co=::cos(fAngle), si=::sin(fAngle);

		int cx,cy;
		m_img->GetSize(&cx,&cy);
		_point offset;
		m_img->GetOffset(&offset);

		_point ptOldCenter(offset.x+cx/2,offset.y+cy/2);

		CMatrix2x2 m(co, -si, si, co);
		if(rot.bMirrorH) m=m*CMatrix2x2(-1,0, 0,1);

		params=
			CAphineParams(
				m,
				CVector2D(ptOldCenter.x,ptOldCenter.y),
				CVector2D(ptOldCenter.x,ptOldCenter.y)/*new*/);

		_size new_size;
		_point new_offset;
		atCalcNewSizeAndOffsetMasked(IImage2Ptr(m_img),  params,
			new_size, new_offset );

		params.center_new = params.center_new-CVector2D(new_offset.x,new_offset.y);
		new_offset=_point(0,0);

		IUnknown *punk4 = ::CreateTypedObject(_bstr_t(szTypeImage));
		IImage3Ptr img2(punk4);
		punk4->Release();

		img2->CreateImage(new_size.cx,new_size.cy,bstrCC, ::GetImagePaneCount( image ) );
		img2->InitRowMasks();
		img2->SetOffset(new_offset,false);

		if(0)
		{
		atExecuteFast( IImage2Ptr(m_img), IImage2Ptr(img2), params, 
			_callback_dummy, 0);
		} else
		{
		atExecute( IImage2Ptr(m_img), IImage2Ptr(img2), params, 
			_callback_dummy, 0);
		}

		m_img=img2;

		//повернем ось и центромеру
		for(int i=0; i<lNewAxisSize; i++)
		{
			CVector2D vec(m_faxis[i].x,m_faxis[i].y);
			vec=params*vec;
			m_faxis[i].x=vec.x;
			m_faxis[i].y=vec.y;
		}
		{
			CVector2D vec(m_ptCen.x,m_ptCen.y);
			vec=params*vec;
			m_ptCen.x=int(vec.x+0.5);
			m_ptCen.y=int(vec.y+0.5);
		}
		for(i=0; i<2; i++)
		{
			CVector2D vec(m_ptDraw[i].x,m_ptDraw[i].y);
			vec=params*vec;
			m_ptDraw[i].x=int(vec.x+0.5);
			m_ptDraw[i].y=int(vec.y+0.5);
		}
	}

	if(1){	//установим длинную ось
		//длина оси
		long lLongAxisSize=0;
		BOOL bIsManual;
		ptrC->GetAxisSize(AT_Original|AT_LongAxis, &lLongAxisSize, &bIsManual);

		//получим саму ось
		smart_alloc(faxis, FPOINT, max(lLongAxisSize,2));
		faxis[0]=_fpoint(0,0);
		ptrC->GetAxis(AT_Original|AT_LongAxis|AT_AxisDouble, (POINT*)faxis);

		if(bAxisBottomToTop)
		{
			for(int i=0; i<lLongAxisSize/2; i++) 
			{
				_fpoint p=faxis[i];
				faxis[i]=faxis[lLongAxisSize-1-i];
				faxis[lLongAxisSize-1-i]=p;
			}

		}
		if(lLongAxisSize<2) faxis[1]=faxis[0];

		lNewAxisSize=max(lLongAxisSize,2);
		m_faxis_long.alloc(lNewAxisSize);

		for(int j=0; j<lLongAxisSize; j++)
		{
			//double t=(j*2-lLongAxisSize+1.0)/(lLongAxisSize-1.0); //-1..1
			int i_best=0; 
			double r_best=1e8;
			for(int i=0; i<len-1; i++)
			{	//найдем ближайшую точку сглаженной оси
				double dx=faxis[j].x-daxis[i].x;
				double dy=faxis[j].y-daxis[i].y;
				double r=dx*daxisvect[i].x+dy*daxisvect[i].y; //проекция на ось
				double r2=_hypot(dx,dy); //расстояние до оси
				
				const double len_mul=1.2; //!!!!
				double t=(i*2-len+1.0)/len*len_mul;
				double adjust0=0; //!!!!
				double adjust1=1; //!!!!
				t=t*adjust1+adjust0;
				double t1=(j*2-lLongAxisSize+1.0)/max(lLongAxisSize,1);
				double r3=fabs(t-t1); //разница в относительной позиции точек на оси

				r=fabs(r)+r2/4+r3*8;
				if(r<r_best)
				{
					i_best=i;
					r_best=r;
				}
			}
			//int i=j*(len-1)/(lLongAxisSize-1);
			i=i_best;
			double xd=faxis[j].x-daxis[i].x;
			double yd=faxis[j].y-daxis[i].y;
			double xd1=xd*daxisvect[i].y-yd*daxisvect[i].x;
			double yd1=yd*daxisvect[i].y+xd*daxisvect[i].x;
			//xd1=0; yd1=0;

			m_faxis_long[j]=_dpoint(width2+xd1,i+yd1); //поправить - чтоб поворот был
		}

		//коррекция от безобразий (дрожание в пределах пиксела)
		if(1)
		{
			for(int di=2; di>0; di--)
			{ // прогоним медиану (убирание пиков) по шаблонам разной ширины
				// медианный фильтр по 3 точкам
				smart_alloc(x2,double,lNewAxisSize);
				for(int i=0; i<lNewAxisSize; i++) x2[i]=m_faxis_long[i].x;
				for(int i=di; i<lNewAxisSize-di; i++)
				{
					double x=x2[i];
					double xmax=max(x2[i-di],x2[i+di]);
					double xmin=min(x2[i-di],x2[i+di]);
					x=min(x,xmax);
					x=max(x,xmin);
					m_faxis_long[i].x=x;
				}
			}
		}

		//подвинем ось
		for(int i=0; i<lNewAxisSize; i++)
		{
			m_faxis_long[i].x-=x_ofs;
			m_faxis_long[i].y-=y_ofs;
			if(bMirrorV) m_faxis_long[i].y=h-1-m_faxis_long[i].y;
		}

		//повернем ось и центромеру
		for(int i=0; i<lNewAxisSize; i++)
		{
			CVector2D vec(m_faxis_long[i].x,m_faxis_long[i].y);
			vec=params*vec;
			m_faxis_long[i].x=vec.x;
			m_faxis_long[i].y=vec.y;
		}

	}

	if(0){	//установим длинную ось		
		int lShortAxisSize=m_faxis.size();
		long lLongAxisSize=0;
		BOOL bIsManual;
		ptrC->GetAxisSize(AT_Original|AT_LongAxis, &lLongAxisSize, &bIsManual);

		lLongAxisSize=max(lLongAxisSize,2);
		m_faxis_long.alloc(lLongAxisSize);

		for(int i=0; i<lLongAxisSize; i++)
		{
			double rj=double(i)*(lShortAxisSize-1)/(lLongAxisSize-1);
			int j=int(rj);
			if(j>lShortAxisSize-2) j=lShortAxisSize-2;
			if(j<0) j=0;
			double r=rj-j;
			m_faxis_long[i].x = m_faxis[j].x*(1-r)+m_faxis[j+1].x*r;
			m_faxis_long[i].y = m_faxis[j].y*(1-r)+m_faxis[j+1].y*r;
		}
	}

	return S_OK;
}

HRESULT IErectCC::CCGetAxisSize(IChromosome *pChromo, int nFlags /*ChromoAxisType*/, long *plSize, BOOL *pbIsManual)
{
	if(pChromo==0) return S_FALSE;
	if((nFlags&AT_Rotated) == 0)
	{
		return pChromo->GetAxisSize(AT_Original|nFlags, plSize, pbIsManual);
	}

	if(m_img==0) Recalc(pChromo);
	if(plSize)
	{
		if(nFlags & AT_LongAxis)
		{
			*plSize=m_faxis_long.size();
		}
		else
		{
			*plSize=m_faxis.size();
		}
	}
	if(pbIsManual) *pbIsManual=false;

	return S_OK;
}

HRESULT IErectCC::CCGetAxis(IChromosome *pChromo, int nFlags /*ChromoAxisType*/, POINT *pAxisPoints)
{
	if(pChromo==0) return S_FALSE;
	if((nFlags&AT_Rotated) == 0)
	{
		return pChromo->GetAxis(AT_Original|nFlags, pAxisPoints);
	}
	if(m_img==0) Recalc(pChromo);
	if(pAxisPoints!=0)
	{
		if(nFlags & AT_LongAxis)
		{
			int nSize=m_faxis_long.size();
			for(int i=0; i<nSize; i++)
			{
				if(nFlags & AT_AxisDouble)
					((FPOINT *)pAxisPoints)[i] = _fpoint(m_faxis_long[i].x,m_faxis_long[i].y);
				else
					pAxisPoints[i] = _point(int(m_faxis_long[i].x+0.5),int(m_faxis_long[i].y+0.5));
			}
		}
		else
		{
			int nSize=m_faxis.size();
			for(int i=0; i<nSize; i++)
			{
				if(nFlags & AT_AxisDouble)
					((FPOINT *)pAxisPoints)[i] = _fpoint(m_faxis[i].x,m_faxis[i].y);
				else
					pAxisPoints[i] = _point(int(m_faxis[i].x+0.5),int(m_faxis[i].y+0.5));
			}
		}
	}

	return S_OK;
}

HRESULT IErectCC::CCGetCentromereCoord(IChromosome *pChromo, int nFlags/*ChromoAxisType*/, POINT *pptCen, POINT *pptDraw, BOOL *pbIsManual) // pptDraw can be NULL
{
	if(pChromo==0) return S_FALSE;
	if((nFlags&AT_Rotated) == 0)
	{
		return pChromo->GetCentromereCoord(AT_Original|nFlags, pptCen, pptDraw, pbIsManual);
	}
	if(m_img==0) Recalc(pChromo);
	if(pptCen!=0)
	{
		*pptCen=m_ptCen;
	}
	if(pptDraw!=0)
	{
		pptDraw[0]=m_ptDraw[0];
		pptDraw[1]=m_ptDraw[1];
	}
	return S_OK;
}

HRESULT IErectCC::CCGetImage(IChromosome *pChromo, IUnknown **ppunk)
{
	if(m_img==0) Recalc(pChromo);
	if(m_img!=0) m_img->AddRef();
	*ppunk=m_img;
	return S_OK;
}
