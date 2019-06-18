#include "stdafx.h"
#include "SegmColorPhases.h"
#include "misc_utils.h"
#include <math.h>

#include "nameconsts.h"
#include "ImagePane.h"
#include "binimageint.h"
#include "core5.h"
#include "alloc.h"
#include "stdio.h"

#define MaxColor color(-1)

/////////////////////////////////////////////////////////////////////////////
_ainfo_base::arg	CSegmColorPhasesInfo::s_pargs[] = 
{
	{"Img",		szArgumentTypeImage, 0, true, true },
	{"SegmColorPhases",szArgumentTypeBinaryImage, 0, false, true },
	{"TestPointX",		szArgumentTypeInt, "-1", true, false },
	{"TestPointY",		szArgumentTypeInt, "-1", true, false },
	{0, 0, 0, false, false },
};
// если задать TestPointX/TestPointY - то вместо реального выполнения акции
// просто вычислим номер фазы для одной точки (для BT 4397)

static inline double sqr(double x)
{ return x*x; }

static bool MakeBinary(IImage3 *pimg, IBinaryImage *pbinimg, POINT &ptOffset)
{
	INamedDataObject2Ptr sptrNDO2Image(pimg);
	IUnknownPtr punkParent;
	sptrNDO2Image->GetParent(&punkParent);
	IImage3Ptr imageParent(punkParent);
	if (imageParent == 0)
	{
		int cx,cy;
		pimg->GetSize(&cx,&cy);
		if( cy == 0 || cx == 0 )
			return false;
		pbinimg->CreateNew(cx, cy);
		ptOffset.x = ptOffset.y = 0;
	}
	else
	{
		int cx,cy;
		imageParent->GetSize(&cx,&cy);
		if( cy == 0 || cx == 0 )
			return false;
		pbinimg->CreateNew(cx, cy);
		pimg->GetOffset(&ptOffset);
	}
	return true;
}

static bool GetColor(int x, int y, color* pColors, int nPanes, IUnknown* punkImage)
{ // получить цвет с изображения в заданной точке (с учетом Image.offset)
	// результат в pColors, на выходе true - если дотянулись до точки
	IImage3Ptr ptrImg(punkImage);
	if(ptrImg==0) return false;

	int cx=0, cy=0;
	ptrImg->GetSize(&cx,&cy);
	POINT ptOffset;
	ptrImg->GetOffset(&ptOffset);

	x -= ptOffset.x;
	y -= ptOffset.y;
	if(x<0 || x>=cx || y<0 || y>=cy) return false;

	byte *pmask=0;
	ptrImg->GetRowMask( y ,&pmask );
	if( (pmask[x] & 128) == 0 ) return false;

	color *p;
	for( int nPane=0; nPane<nPanes; nPane++ )
	{
		ptrImg->GetRow(y, nPane, &p);
		if(!p) return false;
		pColors[nPane] = p[x];
	}
	return true;
}


bool CSegmColorPhases::InvokeFilter()
{
	IImage3Ptr	ptrSrcImage( GetDataArgument() );
	IBinaryImagePtr	ptrBin = GetDataResult();

	int nTestPointX = GetArgLong("TestPointX");
	int nTestPointY = GetArgLong("TestPointY");
			
	if( ptrSrcImage == NULL )
		return false;

	ptrSrcImage->GetSize((int*)&size.cx,(int*)&size.cy);
	int cx=size.cx, cy=size.cy;
	int nPanes = ::GetImagePaneCount( ptrSrcImage );

	double fDistMethod = ::GetValueDouble( GetAppUnknown(), "\\SegmColorPhases", "DistMethod", 0.0 );

	int nClusters = GetValueInt( GetAppUnknown( ), "\\SegmColorPhases", "Count", 2 );
	if(nClusters<1) return false;

	// Данные по кластерам
	smart_alloc(centers, double, nClusters*nPanes); // средние цвета по кластеру
	smart_alloc(normals, double, nClusters*nPanes); // средние цвета по кластеру, нормированные так, что их модуль = 1
	smart_alloc(weights, double, nClusters); // веса кластеров
	smart_alloc(phases, int, nClusters); // номера фаз кластеров

	// прочитаем инфу о кластерах
	char sz[256];
	for(int cluster=0; cluster<nClusters; cluster++ )
	{
		sprintf(sz, "Color%u", cluster);
		_bstr_t bstrColor = GetValueString( GetAppUnknown( ), "\\SegmColorPhases", sz, "" );
		char *p = bstrColor;
		for(int nPane=0; nPane<nPanes; nPane++)
		{
			while(*p!=0 && !isdigit_ru(*p) && *p!='-') p++;
			double n=0;
			int sign=1;
			if(*p=='-') { sign=-1; p++; }
			while(isdigit_ru(*p))
			{
				n = n*10 + (*p-'0');
				p++;
			}
			n*=sign;
			centers[cluster*nPanes+nPane] = n*256;
		}
		sprintf(sz, "Weight%u", cluster);
		weights[cluster] = GetValueDouble( GetAppUnknown( ), "\\SegmColorPhases", sz, 1.0 );
		weights[cluster] *= weights[cluster];
		weights[cluster] = max(1e-10, min(1e10, weights[cluster] ));
		sprintf(sz, "Phase%u", cluster);
		phases[cluster] = GetValueInt( GetAppUnknown( ), "\\SegmColorPhases", sz, 255 );
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

	if(nTestPointX>=0 && nTestPointY>=0)
	{ // задана точка - нам интересен цвет только в ней
		smart_alloc(pColors, color, nPanes);

		// попробуем получить цвет в точке
		bool bOk = 
			GetColor(nTestPointX, nTestPointY, pColors, nPanes, ptrSrcImage);
		if(!bOk)
		{ // если не получилось - потянем с парента
			INamedDataObject2Ptr sptrNDO2Image(ptrSrcImage);
			IUnknownPtr ptrParent;
			if(sptrNDO2Image!=0) 
				sptrNDO2Image->GetParent(&ptrParent);
			bOk = 
				GetColor(nTestPointX, nTestPointY, pColors, nPanes, ptrParent);
		}

		if(!bOk)
		{ // Если точку не выцепили - то вернуть фигню
			::SetValue(::GetAppUnknown(), "\\SegmColorPhases\\Result", "Cluster", long(-1));
			::SetValue(::GetAppUnknown(), "\\SegmColorPhases\\Result", "Phase", long(-1));
			return false;
		}

		int best=0;
		double r_best=1e20;
		for(int cluster=0; cluster<nClusters; cluster++)
		{

			double r=0;
			for( int nPane=0; nPane<nPanes; nPane++ )
			{
				double d = centers[cluster*nPanes+nPane];
				if(d>=0)
					r += sqr( pColors[nPane] - d );
				else
					r += 64.0*64.0*256.0*256.0; // если задать -1 - то значение данной компоненты цвета не используется (для выделения объектов на неизвестном фоне и наоборот)
			}

			if(fDistMethod!=0.0) // если 0.0 - сэкономим время
			{
				double r2=0, r3=0;
				for( int nPane=0; nPane<nPanes; nPane++ )
				{
					color c = pColors[nPane];
					r2 += c*normals[cluster*nPanes+nPane];
					r3 += sqr(c);
				}
				r2 = r3 - sqr(r2);
				r = r2*fDistMethod + r*(1-fDistMethod);
				// Если fDistMethod=1 - то считаем расстояние до прямой, проходящей через центр кластера;
				// если 0 - до центра кластера
			}

			r /= weights[cluster];
			if(r<r_best)
			{
				best = cluster;
				r_best = r;
			}
		}

		::SetValue(::GetAppUnknown(), "\\SegmColorPhases\\Result", "Cluster", long(best));
		::SetValue(::GetAppUnknown(), "\\SegmColorPhases\\Result", "Phase", long(phases[best]));
		return false;
	}

	if( ptrBin==NULL ) return false;

	/*ptrBin->CreateNew(size.cx,size.cy);
	POINT point;
	ptrSrcImage->GetOffset(&point);
	ptrBin->SetOffset(point,true);*/
	POINT ptOffs;
	MakeBinary(ptrSrcImage, ptrBin, ptOffs);

	try
	{
		StartNotification( size.cy );

		smart_alloc(ppsrc, color*, nPanes);

		for( int y=0; y<size.cy; y++ )
		{
			byte *pmask=0;
			ptrSrcImage->GetRowMask( y ,&pmask );
			byte* pDst;
			ptrBin->GetRow(&pDst,y+ptOffs.y,0);
			pDst += ptOffs.x;
			for( int nPane=0; nPane<nPanes; nPane++ )
				ptrSrcImage->GetRow(y, nPane, &ppsrc[nPane]);

			for( long x = 0; x < size.cx; x++, pmask++, pDst++)
			{
				*pDst = 0;
				if( (*pmask) & 128 )
				{
					int best=0;
					double r_best=1e20;
					for(int cluster=0; cluster<nClusters; cluster++)
					{

						double r=0;
						for( int nPane=0; nPane<nPanes; nPane++ )
						{
							double d = centers[cluster*nPanes+nPane];
							if(d>=0)
								r += sqr( ppsrc[nPane][x] - d );
							else
								r += 64.0*64.0*256.0*256.0; // если задать -1 - то значение данной компоненты цвета не используется (для выделения объектов на неизвестном фоне и наоборот)
						}

						if(fDistMethod!=0.0) // если 0.0 - сэкономим время
						{
							double r2=0, r3=0;
							for( int nPane=0; nPane<nPanes; nPane++ )
							{
								color c = ppsrc[nPane][x];
								r2 += c*normals[cluster*nPanes+nPane];
								r3 += sqr(c);
							}
							r2 = r3 - sqr(r2);
							r = r2*fDistMethod + r*(1-fDistMethod);
							// Если fDistMethod=1 - то считаем расстояние до прямой, проходящей через центр кластера;
							// если 0 - до центра кластера
						}

						r /= weights[cluster];
						if(r<r_best)
						{
							best = cluster;
							r_best = r;
						}
					}
					*pDst = phases[best];
				}
			}
			Notify( y );
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



bool CSegmColorPhases::CanDeleteArgument( CFilterArgument *pa )
{
	return false;
}
