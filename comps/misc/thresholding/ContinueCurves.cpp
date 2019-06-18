#include "stdafx.h"
#include "continuecurves.h"
#include "misc_utils.h"
#include <math.h>

#include "nameconsts.h"
#include "ImagePane.h"
#include "binimageint.h"
#include "docview5.h"
#include "core5.h"
#include "alloc.h"
#include "stdio.h"
#include "\vt5\awin\misc_ptr.h"

/////////////////////////////////////////////////////////////////////////////
_ainfo_base::arg	CContinueCurvesInfo::s_pargs[] = 
{
	{"r_min", szArgumentTypeDouble, "10.0", true, false},
	{"r_max", szArgumentTypeDouble, "40.0", true, false},
	{"threshold", szArgumentTypeDouble, "0.5", true, false},
	{"Bin",	szArgumentTypeBinaryImage, 0, true, true },
	{"Curves",	szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};


static double inline sqr(double x)
{ return x*x; }

static inline int round(double x)
{
	return int(floor(x+0.5));
}

bool CContinueCurves::InvokeFilter()
{
	IBinaryImagePtr	ptrSrc( GetDataArgument() );
	IBinaryImagePtr	ptrDst( GetDataResult() );

	if( ptrSrc == NULL || ptrDst==NULL )
		return false;

	double r_min = GetArgDouble("r_min");
	double r_max = GetArgDouble("r_max");
	double r_detector = r_min;
	double threshold = GetArgDouble("threshold");

	int cx, cy;
	ptrSrc->GetSizes(&cx,&cy);
	ptrDst->CreateNew(cx,cy);
	POINT point;
	ptrSrc->GetOffset(&point);
	ptrDst->SetOffset(point,true);

	try
	{
		StartNotification( cy, 1 );

		smart_alloc(srcRows, byte*, cy);
		smart_alloc(dstRows, byte*, cy);
		src = srcRows;
		dst = dstRows;

		for(int y=0; y<cy; y++)
		{
			ptrSrc->GetRow(&srcRows[y],y,FALSE);
			ptrDst->GetRow(&dstRows[y],y,FALSE);
		}

		for(int y=0; y<cy; y++)
		{
			for(int x=0; x<cx; x++)
			{
				dstRows[y][x] = 0;
			}
		}

		IUnknownPtr ptrI( ::CreateTypedObject(_bstr_t(szTypeImage)), false );
		IImage3Ptr      sptrI(ptrI);
		sptrI->CreateImage( cx,cy, _bstr_t("Gray"), -1 );

		smart_alloc(grayRows, color *, cy);
		for(int y=0; y<cy; y++)
		{
			sptrI->GetRow(y, 0, grayRows+y);
			for(int x=0; x<cx; x++)
				grayRows[y][x] = 0;
		}

		//if(bShowDebugImage)
		if(1) { // debug - вставка в документ вспомогательной картинки
			if (m_ptrTarget==0) return S_FALSE;
				IDocumentPtr doc(m_ptrTarget);
			if(doc==0) return S_FALSE;
			SetToDocData(doc, sptrI);
		}

		int r=int(ceil(r_detector));
		int na = round(r_detector*8);
		smart_alloc(dx, double, na*2);
		smart_alloc(dy, double, na*2);
		smart_alloc(rr, double, na*2);
		for(int i=0; i<na*2; i++)
		{
			dx[i] = round(r_detector*cos(i*2*PI/na));
			dy[i] = round(r_detector*sin(i*2*PI/na));
			rr[i] = r_detector / max(0.0001, cos(PI*i/na)) / 2;
		}

		double th1 = acos( r_detector / (2*r_min) ) * na / PI;
		double th2 = acos( r_detector / (2*r_max) ) * na / PI;

		for(int y1=r; y1<cy-r; y1++)
		{
			for(int x1=r; x1<cx-r; x1++)
			{
				if(!srcRows[y1][x1]) continue; // работаем только внутри маски
				if(	srcRows[y1][x1-1] && srcRows[y1][x1+1] && 
					srcRows[y1-1][x1] && srcRows[y1+1][x1] )
					continue; // работаем только на краях маски
				int cnt=0, cnt_max=0;
				int i_best = 0;
				for(int step=1; step<2; step*=2)
				{
					cnt=0;
					for(int i=0; i<na*2; i++)
					{
						int y2 = y1 + dy[i]/step;
						int x2 = x1 + dx[i]/step;
						if(!srcRows[y2][x2]) // в отличие от FindCurves ищем по фону
							cnt++;
						else
							cnt=0;
						if(cnt>cnt_max)
						{
							cnt_max = cnt;
							i_best = i;
						};
					}
					cnt_max = min(na, cnt_max);

					// от области на фоне ( i_best-(cnt_max-1) .. i_best )
					// перейдем к области на объекте
					i_best -= cnt_max; // теперь указывает на последнюю точку маски
					cnt_max = na-cnt_max; // вывернули
					if(i_best<cnt_max) i_best += na;

					if(cnt_max>=th1 && cnt_max<=na/2) break;
				}
				//grayRows[y1][x1] = cnt_max*65535/na;
				if(cnt_max>=th1 && cnt_max<=th2)
				{
					dstRows[y1][x1] = 255;

					double a = (i_best - 0.5*(cnt_max-1))*PI*2/na;
					double r = rr[cnt_max];
					//r = (r + r_min) / 2;
					//r = 10;
					int x = x1 + r*cos(a);
					int y = y1 + r*sin(a);
					if(x>=0 && x<cx && y>=0 && y<cy)
					{
						//grayRows[y][x] = min(65535, grayRows[y][x]+r_min/r*16384);
					}

					int dirA = i_best-(cnt_max-1);
					int dirB = i_best;

					int yA = y1 + dy[dirA];
					int xA = x1 + dx[dirA];
					int yB = y1 + dy[dirB];
					int xB = x1 + dx[dirB];

					int yA2 = yB + (yA-y1)*3;
					int xA2 = xB + (xA-x1)*3;
					int yB2 = yA + (yB-y1)*3;
					int xB2 = xA + (xB-x1)*3;

					{
						double fx = x1, fy=y1;
						double fdx = xB-xA, fdy = yB-yA;
						double len1 = max(1, _hypot(fdx,fdy));
						fdx /= len1; fdy /= len1;
						double step = 1/rr[cnt_max], step2 = sqrt(1-step*step);
						for(int k=0; k<len1*0.5+rr[cnt_max]*0.5; k++)
						{
							int x = round(fx), y = round(fy);
							if(x>=0 && x<cx && y>=0 && y<cy)
							{
								grayRows[y][x] = min(65535, grayRows[y][x]+8192);
							}
							fx += fdx; fy += fdy;
							double tmp = fdx*step2 + fdy*step;
							fdy = fdy*step2 - fdx*step;
							fdx = tmp;
						}
					}
					{
						double fx = x1, fy=y1;
						double fdx = xB-xA, fdy = yB-yA;
						double len1 = max(1, _hypot(fdx,fdy));
						fdx /= len1; fdy /= len1;
						double step = 1/rr[cnt_max], step2 = sqrt(1-step*step);
						for(int k=0; k<len1*0.5+rr[cnt_max]*0.5; k++)
						{
							int x = round(fx), y = round(fy);
							if(x>=0 && x<cx && y>=0 && y<cy)
							{
								grayRows[y][x] = min(65535, grayRows[y][x]+8192);
							}
							fx -= fdx; fy -= fdy;
							double tmp = fdx*step2 - fdy*step;
							fdy = fdy*step2 + fdx*step;
							fdx = tmp;
						}
					}

					//if( 0 == x1%21 && 0 == y1%21 )
					{
						//if( xA2>=0 && xA2<cx && yA2>=0 && yA2<cy &&
						//	xB2>=0 && xB2<cx && yB2>=0 && yB2<cy )
						//{
						//	int len = int(ceil(_hypot(xA2-xB2,yA2-yB2)));
						//	len = max(1, len);
						//	for(int i=0; i<=len; i++)
						//	{
						//		int x = xA2 + round( double(xB2-xA2)*i/len );
						//		int y = yA2 + round( double(yB2-yA2)*i/len );
						//		grayRows[y][x] = min(65535, grayRows[y][x]+8192);
						//	}
						//}
						//if( xA>=0 && xA<cx && yA>=0 && yA<cy &&
						//	xB>=0 && xB<cx && yB>=0 && yB<cy )
						//{
						//	int len = int(ceil(_hypot(xA-xB,yA-yB)));
						//	len = max(1, len);
						//	for(int i=0; i<=len; i++)
						//	{
						//		int x = xA + round( double(xB-xA)*i/len );
						//		int y = yA + round( double(yB-yA)*i/len );
						//		grayRows[y][x] = min(65535, grayRows[y][x]+8192);
						//	}
						//}

						if(0)if(xA2>=0 && xA2<cx && yA2>=0 && yA2<cy &&
							xA>=0 && xA<cx && yA>=0 && yA<cy )
						{
							int len = int(ceil(_hypot(xA2-xA,yA2-yA)));
							len = max(1, len);
							for(int i=0; i<=len; i++)
							{
								int x = xA + round( double(xA2-xA)*i/len );
								int y = yA + round( double(yA2-yA)*i/len );
								grayRows[y][x] = min(65535, grayRows[y][x]+8192);
							}
							//grayRows[yA2][xA2] = min(65535, (grayRows[yA2][xA2]+65535)/2);
						}
						if(0)if(xB2>=0 && xB2<cx && yB2>=0 && yB2<cy &&
							xB>=0 && xB<cx && yB>=0 && yB<cy )
						{
							int len = int(ceil(_hypot(xB2-xB,yB2-yB)));
							len = max(1, len);
							for(int i=0; i<=len; i++)
							{
								int x = xB + round( double(xB2-xB)*i/len );
								int y = yB + round( double(yB2-yB)*i/len );
								grayRows[y][x] = min(65535, grayRows[y][x]+8192);
							}
							//grayRows[yB2][xB2] = min(65535, (grayRows[yB2][xB2]+65535)/2);
						}
					}

				}
				else
				{
					dstRows[y1][x1] = max(128, dstRows[y1][x1]);
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

	return true;
}

/////////////////////////////////////////////////////////////////////////////

