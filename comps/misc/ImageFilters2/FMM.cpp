#include "stdafx.h"
#include "FMM.h"
#include "misc_utils.h"
#include <math.h>
#include "alloc.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "ImagePane.h"
#include "PointList.h"

#include "..\thresholding\FastMarch.h"

#define MaxColor color(-1)

_ainfo_base::arg        CDilateOnImageInfo::s_pargs[] = 
{       
	{"Quantity",    szArgumentTypeDouble, "100.0", true, false }, // Как далеко заходит "дилатация"
	{"Tolerance",   szArgumentTypeDouble, "20.0", true, false }, // насколько близкие к цвету объекта цвета считаем "хорошими"
	{"Image",    szArgumentTypeImage, 0, true, true },
	{"Seed",        szArgumentTypeBinaryImage, 0, true, true },
	{"DilateOnImage",szArgumentTypeBinaryImage, 0, false, true },	
	{0, 0, 0, false, false },
};

/////////////////////////////////////////////////////////////////////////////
bool CDilateOnImage::CanDeleteArgument( CFilterArgument *pa )
{
	return CheckInterface( pa->m_punkArg, IID_IBinaryImage );
}

bool CDilateOnImage::InvokeFilter()
{
	IImage4Ptr	ptrV( GetDataArgument("Image") );
	IBinaryImagePtr ptrBin( GetDataArgument("Seed") );
	IBinaryImagePtr ptrBinResult( GetDataResult() );
	
	if(ptrV==0 || ptrBin==0 || ptrBinResult==0 )
		return false;

	double fTolerance = GetArgDouble("Tolerance");
	fTolerance = max(0.001, fTolerance);
	double fQuantity = GetArgDouble("Quantity");

	int cx=0,cy=0;
	ptrV->GetSize(&cx,&cy);
	POINT ptOffset={0};
	ptrV->GetOffset(&ptOffset);

	int cx_Bin=0, cy_Bin=0;
	ptrBin->GetSizes(&cx_Bin,&cy_Bin);
	POINT ptOffset_Bin={0};
	ptrBin->GetOffset(&ptOffset_Bin);

	if(cx<3 || cy<3) return false; // не умеем c мелюзгой работать

	//ptrD1->CreateImage(cx, cy, _bstr_t("Gray"), -1 ); 
	//ptrD1->SetOffset( ptOffset, TRUE);
	if( S_OK != ptrBinResult->CreateNew(cx_Bin, cy_Bin) ) return false;
	ptrBinResult->SetOffset( ptOffset_Bin, TRUE);
	for(int y=0; y<cy_Bin; y++)
	{
		byte *p=0, *p1=0;
		ptrBin->GetRow(&p, y, false);
		ptrBinResult->GetRow(&p1, y, false);
		if(p!=0 && p1!=0) memcpy(p1, p, cx_Bin);
	}

	/////////////////
	//_fast_march_t<CSimplePointProcessor> marcher1;
	
	//_fast_march_t<CColorLikenessPointProcessor> marcher1;
	//marcher1.SetupImage(ptrV);

	_fast_march_t<CPhasesColorLikenessPointProcessor> marcher1;
	marcher1.SetupImages(ptrV, ptrBinResult);
	marcher1.Init(cx,cy);
	marcher1.m_fTolerance = (fTolerance*256)*(fTolerance*256);

	// пробежимся по всем точкам binary
	int x0 = max( ptOffset.x, ptOffset_Bin.x );
	int y0 = max( ptOffset.y, ptOffset_Bin.y );
	int x1 = min( ptOffset.x+cx, ptOffset_Bin.x+cx_Bin );
	int y1 = min( ptOffset.y+cy, ptOffset_Bin.y+cy_Bin );
	for( int y = y0+1; y < y1-1; y++ )
	{
		byte *p=0;
		ptrBin->GetRow(&p, y-ptOffset_Bin.y, false);
		if(p) for( int x = x0+1; x < x1-1; x++ )
		{
			if( p[x-ptOffset_Bin.x] )
			{
				marcher1.AddPoint( x-ptOffset.x, y-ptOffset.y );
				marcher1.OnAddPoint( x-ptOffset.x, y-ptOffset.y, 0, 1);
			}
		}
	}
	marcher1.Process();

	StartNotification( y1-y0 );
	for( int y = y0; y < y1; y++ )
	{
		byte *p=0;
		ptrBinResult->GetRow(&p, y-ptOffset_Bin.y, false);
		if(p) for( int x = x0+1; x < x1-1; x++ )
		{
			if( marcher1.m_pDist[(y-ptOffset.y)*cx+(x-ptOffset.x)] > fQuantity )
                p[x-ptOffset_Bin.x] = 0;
		}
		Notify(y);
	}
	Notify(0);
	FinishNotification();

	return true;
}

#if(0)
// кусок InvokeFilter для FastMarching c поддержкой внешнего контура
// (тестовый код, сейчас все делается на основе шаблона _fast_march_t)

{
	try
	{
		StartNotification( cy );

		smart_alloc(dist, double, cx*cy);
		smart_alloc(velo, double, cx*cy);
		//smart_alloc(vx, double, cx*cy);
		//smart_alloc(vy, double, cx*cy);
		//smart_alloc(rad, double, cx*cy);
		smart_alloc(s_ind, byte, cx*cy);

		int n_snake_max=64;
		int n_snake=n_snake_max;
		smart_alloc(x_snake, double, n_snake); // координаты узлов снейка
		smart_alloc(y_snake, double, n_snake); 
		smart_alloc(sx_snake, double, n_snake);	// суммы для обновления координат
		smart_alloc(sy_snake, double, n_snake);
		smart_alloc(s_snake, double, n_snake); // сумма весов для обновления координат; x = sx/s
		// s = sum( exp(u/width) ), width - ширина интересной нам полосы
		smart_alloc(tmp_snake, double, n_snake); // некая временная переменная

		smart_alloc(x_history, double, n_snake*20); // координаты узлов снейка - на память...
		smart_alloc(y_history, double, n_snake*20); 
		int n_history = 0;

		// проинициализируем snake
		for(int k=0; k<n_snake; k++)
		{
			x_snake[k] = (cx+1)/2 - 5*cos(k*2*PI/n_snake);
			y_snake[k] = (cy+1)/2 + 5*sin(k*2*PI/n_snake);
			//x_snake[k] = (cx+1)/2 + rand()%21-10;
			//y_snake[k] = (cy+1)/2 + rand()%21-10;
			s_snake[k] = 10;
			sx_snake[k] = x_snake[k]*s_snake[k];
			sy_snake[k] = y_snake[k]*s_snake[k];
			int x = int(x_snake[k]+0.5);
			int y = int(y_snake[k]+0.5);
		}

		{for( int yy=-20; yy<=20; yy++ )
		{
			int y = (cy+1)/2 + yy;
			for(int xx=-20; xx<=20; xx++)
			{
				if(xx*xx+yy*yy>20*20) continue;
				int x = (cx+1)/2 + xx;
				double a = atan2(yy,xx)/(PI*2)*n_snake;
				if(a<0) a+=n_snake;
				int n = int(a + 0.5);
				n %= n_snake;
			}
		}}

		bool bLineFlag = true;

		{for( int y=0; y<cy; y++ )
		{
			color* p;
			ptrV->GetRow(y, 0, &p);
			for(int x=0; x<cx; x++)
			{
                dist[y*cx+x] = 1e200;
				//velo[y*cx+x] = exp(p[x]/1024); //(p[x]/256.0+1);
				velo[y*cx+x] = p[x]/256.0;
			}
		}}

		double c_sum=0, c_avg=0;
		double c_count=0;

		double fTolerance = 4.0;

		CHeap band;
		int x = (cx+1)/2;
		int y = (cy+1)/2;
		dist[y*cx+x] = 0;
		band.Insert(x,y, dist[y*cx+x]);
		c_count++;
		c_sum += velo[y*cx+x];
		c_avg = c_sum/c_count;

		{
			double u_snake_step = 50; // шаг по u для snake
			double u_snake_update = u_snake_step; // последний момент обновления snake

			CTimeValue* p;
			while( p = band.GetFirst() )
			{
				x = p->x; y = p->y;

				if( x<2 || x>=cx-2 || y<2 || y>=cy-2 )
				{
					band.DeleteFirst();
					continue;
				}
				int dx=1, dy=0;
				do
				{
					int x0 = x+dx, y0 = y+dy;
					if( dist[y0*cx+x0]>1e199 )
					{
						double h = 0; // переменная для подстройки скорости движения; указывает, насколько мы опередили контур или отстали от него
						if(0){
							// вначале найдем ближайший отрезок контура
							int n0=0, n1=0;
							int dn = n_snake/4;
							int n0_best = 0;
							double r0_best = 1e200;
							double r1_best = 1e200;
							double L_best = 1e200;
							while(n0<n_snake)
							{
								n1 = n0+dn;
								if(n1>=n_snake) n1-=n_snake;
								double r0 = _hypot(x0-x_snake[n0],y0-y_snake[n0]);
								double r1 = _hypot(x0-x_snake[n1],y0-y_snake[n1]);
								double L = _hypot(x_snake[n1]-x_snake[n0],y_snake[n1]-y_snake[n0]);
								if(r0+r1<r0_best+r1_best)
								{
									n0_best = n0;
									r0_best = r0;
									r1_best = r1;
									L_best = L;
								}
								n0 += dn;
							}
							n0 = n0_best;
							n1 = n0+dn;
							double f = (n0*r0_best + n1*r1_best)/(r0_best+r1_best) +
								(r0_best-r1_best)*(n1-n0)/L_best;
							if(f>n_snake) f-=n_snake;
							if(f<0) f+=n_snake;
							int n = int(f+0.5);

							n0 = n-2; if(n0<0) n0 += n_snake;
							n1 = n+2; if(n1>=n_snake) n1 -= n_snake;

							double r0 = _hypot(x0-x_snake[n0],y0-y_snake[n0]);
							double r1 = _hypot(x0-x_snake[n1],y0-y_snake[n1]);
							double L = _hypot(x_snake[n1]-x_snake[n0],y_snake[n1]-y_snake[n0]);

							n1 = n0+4;
							f = (n0*r0 + n1*r1)/(r0+r1) + (r0-r1)*(n1-n0)/L;
							if(f<0) f+=n_snake;
							if(f>n_snake) f-=n_snake;
							n = int(f+0.5);

							int n_best_test=0;
							double r_best = 1e200;
							for(int k=0; k<n_snake; k++)
							{
								double r = _hypot(x0-x_snake[k],y0-y_snake[k]);
								if(r<r_best)
								{
									n_best_test = k;
									r_best = r;
								}
							}
							//n = n-n_best_test;
							//if(n>n_snake/2) n-= n_snake;
							//if(n<-n_snake/2) n+= n_snake;
							//s_ind[y0*cx+x0] = n + n_snake/2; // посмотрим на погрешность

							n = n_best_test;
							n0 = n-1; if(n0<0) n0 += n_snake;
							n1 = n+1; if(n1>=n_snake) n1 -= n_snake;
							L = _hypot(x_snake[n1]-x_snake[n0],y_snake[n1]-y_snake[n0]);
							h = (x_snake[n1]-x_snake[n0])*(y0-y_snake[n0]) -
								(y_snake[n1]-y_snake[n0])*(x0-x_snake[n0]);
							h /= L;
							//h = max(h, -(r0+r1)/2);
							//h = min(h, (r0+r1)/2);
							
							if(h>0) h = (r0+r1)/2;
							//else h = (r0+r1)/2;
							
							s_ind[y0*cx+x0] = int(h/2+64);

						}

						//double v = velo[y0*cx+x0];
						//double v = _hypot((velo[(y0+dx)*cx+x0-dy]-velo[(y0-dx)*cx+x0+dy])/2, velo[(y0+dy)*cx+x0+dx]-velo[y0*cx+x0] );
						//double v = exp( fabs(velo[y0*cx+x0]-c_avg)/fTolerance );
						double v = 1 + fabs(velo[y0*cx+x0]-c_avg)/fTolerance;
						if(h<-10) v/=2; // нефиг отставать
						if(h<-20) v/=2; // нефиг отставать
						if(h<-30) v/=2; // нефиг отставать
						if(h<-40) v/=2; // нефиг отставать
						if(h>10) v+=1; // не торопиться
						if(h>20) v+=2; // не торопиться
						if(h>30) v+=4; // не торопиться
						if(h>40) v+=8; // не торопиться

						//double v = 0.1+rand()%1000/1000.0;
						double u1 = dist[y*cx+x];
						double u2 = min(dist[(y0+dx)*cx+x0-dy],dist[(y0-dx)*cx+x0+dy]);
						if(u2>=u1+v) u2=u1+v;
						if(u1>=u2+v) u1=u2+v;
						double u21 = (u2-u1)/2;
						double d = sqrt( v*v/2 - u21*u21 );
						double u = (u1+u2)/2 + d;

						if(u<dist[y0*cx+x0])
						{
							dist[y0*cx+x0] = u;

							if(x0>2 && x0<cx-3 && y0>2 && y0<cy-3)
							{
								band.Insert(x0,y0,u);
								
								if(bLineFlag)
								{ // обновление snake
									if(u1>u_snake_update)
									{
										if(u1>16000)
											bLineFlag = false; // остановка обновления // debug

										// ----------------------------------
										int y0 = (cy+1)/2;
										int x0;
										for(x0=0; x0<cx; x0++)
										{
											if(dist[y0*cx+x0]<u_snake_update) break;
										}
										if(x0<cx && x0>0) // если нашли край
										{
											x0--; // встанем снаружи
											int n_max = 65536;
											for(int pass=0; pass<2; pass++)
											{
												// на 1-м проходе
												for(int k=0; k<n_snake; k++)
												{
													s_snake[k]=0;
													sx_snake[k]=0;
													sy_snake[k]=0;
												}

												int x=x0, y=y0; // запомним начальную точку
												int dx=0, dy=1; // направление края при обходе "по стенке"
												for(int i=0; i<4000; i++)
												{
													int n = i*n_snake/n_max;
													s_snake[n] ++;
													sx_snake[n] += x;
													sy_snake[n] += y;

													color *p=0;
													ptrD1->GetRow(y, 0, &p);
													if(0) p[x] = 65535;

													int tmp = dx; dx=dy; dy=-tmp;
													for(int j=0; j<4; j++)
													{
														if( dist[(y+dy)*cx+(x+dx)] >= u_snake_update )
															break;
														int tmp = dx; dx=-dy; dy=tmp;
													}
													x += dx; y+=dy;
													if(x==x0 && y==y0) break;
												}
												n_max = i+1;

												// на 1-м проходе
												for(int k=0; k<n_snake; k++)
												{
													if(s_snake[k]==0)
													{
														x_snake[k] = x_snake[k-1];
														y_snake[k] = y_snake[k-1];
													}
													else
													{
														x_snake[k] = sx_snake[k]/max(1,s_snake[k]);
														y_snake[k] = sy_snake[k]/max(1,s_snake[k]);
													}
												}
											}
										}
										// ----------------------------------
										if(n_history>19) n_history=19;
										for(int n=0; n<n_snake; n++)
										{
											x_history[n_history*n_snake+n] = x_snake[n];
											y_history[n_history*n_snake+n] = y_snake[n];
										}
										n_history++;

										u_snake_update += u_snake_step;
									}
								}
								c_count += 1/v;
								c_sum += velo[y*cx+x] / v;
								c_avg = c_sum/c_count;
							}
						}
					}
					
					int tmp=dx; dx=-dy; dy=tmp; // поворот на 90 градусов
				}
				while(dx!=1);
				band.DeleteFirst();
			}
		}

		double log2_256=log(2)/256;
		for( int y=0; y<cy; y++ )
		{
			color *p=0;
			ptrD1->GetRow(y, 0, &p);
			double d0 = log(max(1,dist[cx*3+3]));
			for(int x=0; x<cx; x++)
			{
				double d = dist[y*cx+x]/dist[cx*3+3]*65536;
				//double d = log(max(1,dist[y*cx+x]))*16*256;
				//d = d/d0*65535;
				//double d = log(max(1,dist[y*cx+x]))/log2_256;
				//d = snake_indices[y*cx+x]*65535/n_snake;
				//d = s_ind[y*cx+x]*65536/n_snake;
				// d = s_ind[y*cx+x]*256;
				d = marcher1.m_pDist[y*cx+x]/16;
				p[x] = int(max(0,min(65535,d)));
			}
			Notify(y);
		}
		FinishNotification();

		if(0)for(int i_history=0; i_history<n_history; i_history++)
		{ // отобразить точки контура
			if((n_history-i_history)%3 != 0) continue;
			double* px = x_history+i_history*n_snake;
			double* py = y_history+i_history*n_snake;
			//double* px = x_snake;
			//double* py = y_snake;
			for(int n=0; n<n_snake; n++)
			{
				double x0 = px[n]+0.5;
				double y0 = py[n]+0.5;
				double x1 = px[(n+1)%n_snake]+0.5;
				double y1 = py[(n+1)%n_snake]+0.5;
				int len = int( max( ceil(fabs(x1-x0)), ceil(fabs(y1-y0)) ) );
				len = max(len,1);
				if(1)for(int i=0; i<len; i++)
				{
					int x = int( (x0*(len-i)+x1*i) / len );
					int y = int( (y0*(len-i)+y1*i) / len );
					x = max(0, min(cx-1, x) );
					y = max(0, min(cy-1, y) );
					color *p=0;
					ptrD1->GetRow(y, 0, &p);
					p[x] = 65535;
				}

				int x = int(px[n]+0.5);
				int y = int(py[n]+0.5);
				x = max(1, min(cx-2, x) );
				y = max(1, min(cy-2, y) );

				color *p=0;
				ptrD1->GetRow(y, 0, &p);
				p[x] = 65535;
				//p[x+1] = p[x-1] = 65535;
				//ptrD1->GetRow(y-1, 0, &p);
				//p[x] = 65535;
				//ptrD1->GetRow(y+1, 0, &p);
				//p[x] = 65535;
			}
		}

		FinishNotification();
	}
	catch( ... )
	{
		return false;
	}
}
#endif