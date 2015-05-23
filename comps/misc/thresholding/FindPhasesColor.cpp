#include "stdafx.h"
#include "FindPhasesColor.h"
#include "misc_utils.h"
#include <math.h>

#include "nameconsts.h"
#include "ImagePane.h"
#include "binimageint.h"
#include "core5.h"
#include "alloc.h"
#include "stdio.h"
#include "misc_math.h"
#include <vector>
using namespace std;

#define MaxColor color(-1)

/////////////////////////////////////////////////////////////////////////////
_ainfo_base::arg	CFindPhasesColorInfo::s_pargs[] = 
{
	{"nPhases",		szArgumentTypeInt, "3", true, false },
	{"CreateResult",	szArgumentTypeInt, "1", true, false },
	{"Img",		szArgumentTypeImage, 0, true, true },
	{"FindPhasesColor",szArgumentTypeImage, 0, false, true },
	{0, 0, 0, false, false },
};

/////////////////////////////////////////////////////////////////////////////
_ainfo_base::arg	CFastFindPhasesColorInfo::s_pargs[] = 
{
	{"nPhases",		szArgumentTypeInt, "3", true, false },
	{"CreateResult",	szArgumentTypeInt, "1", true, false },
	{"Img",		szArgumentTypeImage, 0, true, true },
	{"FindPhasesColor",szArgumentTypeImage, 0, false, true },
	{0, 0, 0, false, false },
};

bool CFindPhasesColor::InvokeFilter()
{
	IImage3Ptr	ptrSrcImage( GetDataArgument() );
	IImage3Ptr	ptrDstImage( GetDataResult() );
			
	if( ptrSrcImage == NULL || ptrDstImage==NULL )
		return false;

	ptrSrcImage->GetSize((int*)&size.cx,(int*)&size.cy);

	{	
		ICompatibleObjectPtr ptrCO( ptrDstImage );
		if( ptrCO == NULL ) return false;
		if( S_OK != ptrCO->CreateCompatibleObject( ptrSrcImage, NULL, 0 ) )
			return false;
	}

	int cx=size.cx, cy=size.cy;
	int nPanes = ::GetImagePaneCount( ptrSrcImage );
	if(cx<1 || cy<1 || nPanes<1) return false;

	int nClusters = 5; // количество кластеров для k-means
	int nPasses = 20; // число проходов для k-means
	int dx=1, dy=1; // для скорости - ставить побольше
	dx = dy = max(1,max(cx,cy)/100);
	nClusters = GetArgLong("nPhases");
	nPasses = 20+nClusters*2;

	int nPointsMax = (cx/dx)*(cy/dy);

	bool bCreateResult = GetArgLong("CreateResult")!=0;

	double fDistMethod = ::GetValueDouble( GetAppUnknown(), "\\SegmColorPhases", "DistMethod", 0.0 );

	try
	{
		StartNotification( nPasses, 1, 1 );

		smart_alloc(src, color*, cy*nPanes);
		smart_alloc(dst, color*, cy*nPanes);
		smart_alloc(msk, byte*, cy*nPanes);
		for( int y=0; y<cy; y++ )
		{
			ptrSrcImage->GetRowMask( y ,&msk[y] );
			for( int nPane=0; nPane<nPanes; nPane++ )
			{
				ptrSrcImage->GetRow(y, nPane, &src[nPane*cy+y]);
				ptrDstImage->GetRow(y, nPane, &dst[nPane*cy+y]);
			}
		}

		// перенесем точки в массив, заодно смасштабировав картинку
		smart_alloc( points, double, nPointsMax*nPanes );
		double *ppoint = points;
		int nPoints = 0;
		int nx=cx/dx, ny=cy/dy;
		for( int iy=0; iy<ny; iy++ )
		{
			int y0 = iy*cy/ny;
			int y1 = (iy+1)*cy/ny;
			for( int ix=0; ix<nx; ix++ )
			{
				int x0 = ix*cx/nx;
				int x1 = (ix+1)*cx/nx;
				for( int nPane=0; nPane<nPanes; nPane++ )
					ppoint[nPane] = 0;

				int n=0;
				for(int y=y0; y<y1; y++)
				{
					for(int x=x0; x<x1; x++)
					{
						if(msk[y][x]&128)
						{
							n ++ ;
							for( int nPane=0; nPane<nPanes; nPane++ )
							{
								ppoint[nPane] += src[nPane*cy+y][x];
							}
						}
					}
				}
				if(n)
				{
					for( int nPane=0; nPane<nPanes; nPane++ )
						ppoint[nPane] /= n;
                    nPoints++;
					ppoint += nPanes;
				}
			}
		}

		// Данные k-means
		smart_alloc(centers, double, nClusters*nPanes); // средние цвета по кластеру
		smart_alloc(normals, double, nClusters*nPanes); // средние цвета по кластеру, нормированные так, что их модуль = 1
		smart_alloc(sums, double, nClusters*nPanes); // суммы цвета по кластеру
		smart_alloc(counts, double, nClusters); // счетчики пикселей в кластерах (double - чтобы легко было переделать на fuzzy k-means)

		// Инициализируем центры для k-means
		for(int cluster=0; cluster<nClusters; cluster++)
		{
			counts[cluster] = 0;
			for( int nPane=0; nPane<nPanes; nPane++ )
				centers[cluster*nPanes+nPane] = 32768; /*rand()%65536;*/
		}
		for(int cluster=0; cluster<nClusters; cluster++)
		{
			double s=0;
			for( int nPane=0; nPane<nPanes; nPane++ )
				s += sqr(centers[cluster*nPanes+nPane]);
			s = sqrt( max(1.0,s) );
			for( int nPane=0; nPane<nPanes; nPane++ )
				normals[cluster*nPanes+nPane] = centers[cluster*nPanes+nPane] / s;
		}


		double deviation=0.0;

		smart_alloc(bad_center, double, nPanes); // для вычисления "плохой" точки как взвешенного из всех плохих (вес == r)

		// собственно процедура k-means
		for(int n=0; n<nPasses; n++) // пока просто 20 раз, потом разберемся
		{
			for(int cluster=0; cluster<nClusters; cluster++)
			{
				counts[cluster] = 0;
				for( int nPane=0; nPane<nPanes; nPane++ )
					sums[cluster*nPanes+nPane] = 0;
			}
			//int x_worst = 0, y_worst = 0;
			int n_worst=0;
			double r_worst=0;

			for(int nPane=0; nPane<nPanes; nPane++) bad_center[nPane] = 0;
			double bad_s=0; // для вычисления "плохой" точки как взвешенного из всех плохих (вес == r)

			double *ppoint = points;
			for( int nPoint=0; nPoint<nPoints; nPoint++, ppoint+=nPanes )
			{ // пройдем по всему изображению, считая суммы прилегающих к центрам кластеров точек
				// найдем лучший кластер для данной точки
				int best=0;
				double r_best=1e20;
				double r_second=1e20;
				for(int cluster=0; cluster<nClusters; cluster++)
				{
					double r=0;
					for( int nPane=0; nPane<nPanes; nPane++ )
					{
						r += sqr( 
							ppoint[nPane] -
							centers[cluster*nPanes+nPane] );
					}
					if(fDistMethod!=0.0) // если 0.0 - сэкономим время
					{
						double r2=0, r3=0;
						for( int nPane=0; nPane<nPanes; nPane++ )
						{
							r2 += ppoint[nPane]*normals[cluster*nPanes+nPane];
							r3 += sqr(ppoint[nPane]);
						}
						r2 = r3 - sqr(r2);
						r = r2*fDistMethod + r*(1-fDistMethod);
						// Если fDistMethod=1 - то считаем расстояние до прямой, проходящей через центр кластера;
						// если 0 - до центра кластера
					}
					if(r<r_best)
					{
						best = cluster;
						r_second=r_best;
						r_best = r;
					}
					else if(r<r_second)
					{
						r_second = r;
					}
				}
				// обновим суммы для лучшего кластера
				double weight = 1 - r_best/max(r_second,1);
				weight = max(weight,1e-2);

				for( int nPane=0; nPane<nPanes; nPane++ )
				{
					sums[best*nPanes+nPane] +=
						ppoint[nPane]*weight;
				}
				counts[best] += weight;
				if(r_best>r_worst)
				{ // по ходу найдем худшую точку - вдруг пригодится для инициализации нового кластера
					r_worst = r_best;
					n_worst = nPoint;
				}

				bad_s += r_best;
				for(int nPane=0; nPane<nPanes; nPane++)
					bad_center[nPane] += ppoint[nPane]*r_best;
			}
			// пересчитаем 

			int worst_cluster=0;
			for(int cluster=0; cluster<nClusters; cluster++)
			{
				if( counts[cluster] >= 10 )
				{
					for( int nPane=0; nPane<nPanes; nPane++ )
					{
						centers[cluster*nPanes+nPane] =
							sums[cluster*nPanes+nPane] / counts[cluster];
					}
				}
				if( counts[cluster] < counts[worst_cluster] )
					worst_cluster = cluster;
			}

			if( counts[worst_cluster] < 100 && n<nPasses/2 )
			{ // на первых проходах переинициализируем неудачные кластеры случайными числами
				for( int nPane=0; nPane<nPanes; nPane++ )
					centers[worst_cluster*nPanes+nPane] = 
					//src[nPane*cy+y_worst][x_worst];
					points[n_worst*nPanes+nPane];
					// rand()%65536;
					//bad_center[nPane]/max(bad_s,1);
			}

			for(int cluster=0; cluster<nClusters; cluster++)
			{ // посчитаем нормали кластеров
				double s=0;
				for( int nPane=0; nPane<nPanes; nPane++ )
					s += sqr(centers[cluster*nPanes+nPane]);
				s = sqrt( max(1.0,s) );
				for( int nPane=0; nPane<nPanes; nPane++ )
					normals[cluster*nPanes+nPane] = centers[cluster*nPanes+nPane] / s;
			}

			deviation = sqrt(bad_s/max((cx/dx)*(cy/dy),1));
			Notify(n);
		}

		{ // сортировка кластеров - предварительная, по яркости
			for(int c1=0; c1<nClusters; c1++)
			{
				int c_best = c1;
				double r_best = 1e20;
				for(int c2=c1; c2<nClusters; c2++)
				{
					double r=0;
					for( int nPane=0; nPane<nPanes; nPane++ )
						r += centers[c2*nPanes+nPane];
					if(r<r_best)
					{
						c_best = c2;
						r_best = r;
					}
				}

				// поменяем местами
				c2 = c_best;
				double tmp;
				for( int nPane=0; nPane<nPanes; nPane++ )
				{
					tmp = centers[c1*nPanes+nPane];
					centers[c1*nPanes+nPane] = centers[c2*nPanes+nPane];
					centers[c2*nPanes+nPane] = tmp;

					tmp = normals[c1*nPanes+nPane];
					normals[c1*nPanes+nPane] = normals[c2*nPanes+nPane];
					normals[c2*nPanes+nPane] = tmp;

					tmp = sums[c1*nPanes+nPane];
					sums[c1*nPanes+nPane] = sums[c2*nPanes+nPane];
					sums[c2*nPanes+nPane] = tmp;
				}
				tmp = counts[c1];
				counts[c1] = counts[c2];
				counts[c2] = tmp;

			}
		}

		int rand_seed = 0;
		if(nClusters>2)
		{ // сортировка кластеров - окончательная

			for(int n=0; n<10000; n++)
			{
				//int c1 = rand() % nClusters + 1;
				//int c2 = rand() % nClusters + 1;
				rand_seed = rand_seed*0x33168165 + 17231; // линейный конгруэнтный UCX c периодом 2^32
				int c1 = (rand_seed ^ (rand_seed>>16)) % nClusters;
				rand_seed = rand_seed*0x33168165 + 17231; // линейный конгруэнтный UCX c периодом 2^32
				int c2 = (rand_seed ^ (rand_seed>>16)) % nClusters;
				if(c2<=c1) continue;
				double r11=0, r22=0; // расстояния от кластеров c1 и с2 до их соседей
				double r12=0, r21=0; // новые (после разворота c1-c2 в противоположном направлении)
				for( int nPane=0; nPane<nPanes; nPane++ )
				{
					if(c1>0) r11 += sqr(centers[c1*nPanes+nPane] - centers[(c1-1)*nPanes+nPane]);
					if(c2<nClusters-1) r22 += sqr(centers[c2*nPanes+nPane] - centers[(c2+1)*nPanes+nPane]);
					if(c1>0) r21 += sqr(centers[c2*nPanes+nPane] - centers[(c1-1)*nPanes+nPane]);
					if(c2<nClusters-1) r12 += sqr(centers[c1*nPanes+nPane] - centers[(c2+1)*nPanes+nPane]);
				}
				r11 = sqrt(r11);
				r12 = sqrt(r12);
				r21 = sqrt(r21);
				r22 = sqrt(r22);

				if( r12+r21 < r11 + r22 )
				{
					while(c2>c1)
					{
						double tmp;
						for( int nPane=0; nPane<nPanes; nPane++ )
						{
							tmp = centers[c1*nPanes+nPane];
							centers[c1*nPanes+nPane] = centers[c2*nPanes+nPane];
							centers[c2*nPanes+nPane] = tmp;

							tmp = normals[c1*nPanes+nPane];
							normals[c1*nPanes+nPane] = normals[c2*nPanes+nPane];
							normals[c2*nPanes+nPane] = tmp;

							tmp = sums[c1*nPanes+nPane];
							sums[c1*nPanes+nPane] = sums[c2*nPanes+nPane];
							sums[c2*nPanes+nPane] = tmp;
						}
						tmp = counts[c1];
						counts[c1] = counts[c2];
						counts[c2] = tmp;

						c2--; c1++;
					}

				}
			}
		}

		// вывод оценки кластеризации (для определения оптимального числа кластеров)
		::SetValue( GetAppUnknown(), "\\FindPhasesColor\\Result", "Deviation", deviation/256 );
		double s=0;
		for(int i=0; i<nClusters; i++)
		{
			for(int j=i+1; j<nClusters; j++)
			{
				for( int nPane=0; nPane<nPanes; nPane++ )
					s += sqr( centers[i*nPanes+nPane] - centers[j*nPanes+nPane] );
			}
		}
		double deviation_cl = sqrt (s/max(nClusters*(nClusters-1),1));
		::SetValue( GetAppUnknown(), "\\FindPhasesColor\\Result", "DeviationRatio", deviation/max(deviation_cl,1) );

		// вывод результатов в shell.data
		SetValue( GetAppUnknown( ), "\\SegmColorPhases", "Count", long(nClusters) );
		char sz[256];
		char sz2[256]; // максимум 32 паны по 3 цифры - 32*4+2 символов, явно меньше 256
		for(int cluster=0; cluster<nClusters; cluster++ )
		{
			sprintf(sz, "Color%u", cluster);
			char *p = sz2;
			*(p++) = '(';
			for(int nPane=0; nPane<nPanes; nPane++)
			{
				int n = int( centers[cluster*nPanes+nPane]/256 );
				n=max(0,min(255,n));
				if(n>=100) *(p++) = (n/100) + '0';
				if(n>=10) *(p++) = (n/10)%10 + '0';
				*(p++) = n%10 + '0';
				if(nPane<nPanes-1) *(p++) = ',';
			}
			*(p++) = ')';
			*(p++) = 0;
			SetValue( GetAppUnknown( ), "\\SegmColorPhases", sz, sz2 );
			sprintf(sz, "Weight%u", cluster);
			SetValue( GetAppUnknown( ), "\\SegmColorPhases", sz, 1.0 );
			sprintf(sz, "Phase%u", cluster);
		}

		if(!bCreateResult) return false; // вот так вот. если нам не надо создавать Binary - просто выйдем, в shell.data все уже прописано

		// генерация результирующего изображения
		for( int y=0; y<cy; y++ )
		{
			for( int x=0; x<cx; x++ )
			{
				if(msk[y][x]&128)
				{
					// найдем лучший кластер для данной точки
					int best=0, second=0;
					double r_best=1e20, r_second=1e20;
					for(int cluster=0; cluster<nClusters; cluster++)
					{
						double r=0;
						for( int nPane=0; nPane<nPanes; nPane++ )
						{
							r += sqr( 
								src[nPane*cy+y][x] -
								centers[cluster*nPanes+nPane] );
						}
						if(fDistMethod!=0.0) // если 0.0 - сэкономим время
						{
							double r2=0, r3=0;
							for( int nPane=0; nPane<nPanes; nPane++ )
							{
								r2 += src[nPane*cy+y][x]*normals[cluster*nPanes+nPane];
								r3 += sqr(src[nPane*cy+y][x]);
							}
							r2 = r3 - sqr(r2);
							r = r2*fDistMethod + r*(1-fDistMethod);
							// Если fDistMethod=1 - то считаем расстояние до прямой, проходящей через центр кластера;
							// если 0 - до центра кластера
						}
						if(r<r_second)
						{
							second = cluster;
							r_second = r;
							if(r_second<r_best)
							{
								int ti = second; second=best; best=ti;
								double tr = r_second; r_second=r_best; r_best=tr;
							}
						}
					}
					for( int nPane=0; nPane<nPanes; nPane++ )
					{
						if(1/*r_best<r_second/4*/)
						{
							dst[nPane*cy+y][x] = color(
								centers[best*nPanes+nPane]
								/*65535*(1-r_best/max(1,r_second))*/
									);
						}
						else
							dst[nPane*cy+y][x] = 0;
					}
				}
			}
		}

		FinishNotification();
		return true;
	}
	catch( ... )
	{
		return false;
	}
}

/////////////////////////////////////////////////////////////////////////////

bool CFindPhasesColor::CanDeleteArgument( CFilterArgument *pa )
{
	return false;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

bool CFastFindPhasesColor::InvokeFilter()
{
	IImage3Ptr	ptrSrcImage( GetDataArgument() );
	IImage3Ptr	ptrDstImage( GetDataResult() );
			
	if( ptrSrcImage == NULL || ptrDstImage==NULL )
		return false;

	ptrSrcImage->GetSize((int*)&size.cx,(int*)&size.cy);

	{	
		ICompatibleObjectPtr ptrCO( ptrDstImage );
		if( ptrCO == NULL ) return false;
		if( S_OK != ptrCO->CreateCompatibleObject( ptrSrcImage, NULL, 0 ) )
			return false;
	}

	int cx=size.cx, cy=size.cy;
	int nPanes = ::GetImagePaneCount( ptrSrcImage );
	if(cx<1 || cy<1 || nPanes<1) return false;

	int nClusters = 5; // количество кластеров для k-means
	int nPasses = 20; // число проходов для k-means
	int dx=16, dy=16; // для скорости - ставить побольше
	int wx = int(ceil(double(cx)/dx));
	int wy = int(ceil(double(cy)/dy));

	nClusters = GetArgLong("nPhases");
	nPasses = 20+nClusters*2;

	bool bCreateResult = GetArgLong("CreateResult")!=0;

	double fDistMethod = ::GetValueDouble( GetAppUnknown(), "\\SegmColorPhases", "DistMethod", 0.0 );

	try
	{
		StartNotification( nPasses, 1, 1 );

		smart_alloc(src, color*, cy*nPanes);
		smart_alloc(dst, color*, cy*nPanes);
		smart_alloc(msk, byte*, cy*nPanes);

		for( int y=0; y<cy; y++ )
		{
			ptrSrcImage->GetRowMask( y ,&msk[y] );
			for( int nPane=0; nPane<nPanes; nPane++ )
			{
				ptrSrcImage->GetRow(y, nPane, &src[nPane*cy+y]);
				ptrDstImage->GetRow(y, nPane, &dst[nPane*cy+y]);
			}
		}

		StartNotification( nPasses, 1, 1 );

		vector<int> cluster_x;
		vector<int> cluster_y;
		int dc = 32*256;

		int nIndex = 0, nIndex2=0;
		int rnd1 = rand() %0xFFFFFF;
		smart_alloc(x_prev1, int, cx);
		smart_alloc(y_prev1, int, cx);
		smart_alloc(rnd_prev1, int, cx);
		smart_alloc(x_prev2, int, cx);
		smart_alloc(y_prev2, int, cx);
		smart_alloc(rnd_prev2, int, cx);
		{
			for( int x=0; x<cx; x++ )
			{
				x_prev2[x] = x_prev1[x] = 0;
				y_prev2[x] = y_prev1[x] = 0;
				rnd_prev2[x] = rnd_prev1[x] = 0;
			}
		}

		for( int y=1; y<cy; y++ )
		{
			int x_prev = 0;
			for( int x1=1; x1<cx; x1++ )
			{
				bool bad = false;
				for( int nPane=0; nPane<nPanes; nPane++ )
				{
					dst[nPane*cy+y][x1] = 0;
					if( abs( int(src[nPane*cy+y][x1]) - src[nPane*cy+y][x_prev] ) > dc )
					{
						nIndex++;
						bad = true;
						break;
					}
				}
				if(bad)
				{
					int xx = (x1+x_prev) >> 1;
					bool bad = false;

					int x0 = x_prev;
					for( int nPane=0; nPane<nPanes; nPane++ )
					{
						if( abs( int(src[nPane*cy+y][xx]) - src[nPane*cy+y_prev1[xx]][x_prev1[xx]] ) > dc )
						{
							bad = true;
							break;
						}
					}
					if(bad)
					{
						nIndex2++;
						rnd1 = rand() %0xFFFFFF;
						for(int x=x0; x<x1; x++)
						{
							x_prev2[x] = x1;
							y_prev2[x] = y;
							rnd_prev2[x] = rnd1;
							for( int nPane=0; nPane<nPanes; nPane++ )
								dst[nPane*cy+y][x] = rnd1 << (nPane+8);
						}
					}
					else
					{
						rnd1 = rnd_prev1[xx];
						for(int x=x0; x<x1; x++)
						{
							x_prev2[x] = x_prev1[xx];
							y_prev2[x] = y_prev1[xx];
							rnd_prev2[x] = rnd1;
							for( int nPane=0; nPane<nPanes; nPane++ )
								dst[nPane*cy+y][x] = rnd1 << (nPane+8);
						}
					}
					x_prev = x1;
				}
			}

			for( int x=1; x<cx; x++ )
			{
				x_prev1[x] = x_prev2[x];
				y_prev1[x] = y_prev2[x];
				rnd_prev1[x] = rnd_prev2[x];
			}
		}

		FinishNotification();
		return true;
	}
	catch( ... )
	{
		return false;
	}
}

/////////////////////////////////////////////////////////////////////////////



bool CFastFindPhasesColor::CanDeleteArgument( CFilterArgument *pa )
{
	return false;
}
