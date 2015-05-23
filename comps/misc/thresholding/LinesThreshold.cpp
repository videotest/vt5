#include "stdafx.h"
#include "linesthreshold.h"

#include "misc_utils.h"
#include <math.h>

#include "nameconsts.h"
#include "ImagePane.h"
#include "binimageint.h"
#include "core5.h"
#include "alloc.h"
#include "stdio.h"
#include "docview5.h"

/////////////////////////////////////////////////////////////////////////////
_ainfo_base::arg	CLinesThresholdInfo::s_pargs[] = 
{
	{"Threshold",	szArgumentTypeInt, "32", true, false },
	{"Mask",	szArgumentTypeInt, "7", true, false },
	{"Img",		szArgumentTypeImage, 0, true, true },
	//{"Bin",		szArgumentTypeBinaryImage, 0, true, true },
	{"Lines",	szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};


static double inline sqr(double x)
{ return x*x; }

static inline int iround(double x)
{
	return int(floor(x+0.5));
}

bool CLinesThreshold::InvokeFilter()
{
	IImage3Ptr	ptrSrc( GetDataArgument("Img") );
	//IBinaryImagePtr	ptrBin( GetDataArgument("Bin") );
	IBinaryImagePtr	ptrDstBin( GetDataResult() );

	if( ptrSrc == NULL || ptrDstBin==NULL )
		return false;

	int cx, cy;
	ptrSrc->GetSize(&cx,&cy);
	//int cx2, cy2;
	//ptrBin->GetSizes(&cx2,&cy2);
	//if(cx2!=cx || cy2!=cy)
	//	return false;

	ptrDstBin->CreateNew(cx,cy);
	POINT point;
	ptrSrc->GetOffset(&point);
	ptrDstBin->SetOffset(point,true);

	IUnknownPtr ptrGray( ::CreateTypedObject(_bstr_t(szTypeImage)), false );
	IImage3Ptr      sptrGray(ptrGray);
	sptrGray->CreateImage( cx,cy, _bstr_t("Gray"), -1 );

    m_nThreshold = GetArgLong("Threshold")*256;
	m_nMask = GetArgLong("Mask");
    m_nWorkPane = GetValueInt( GetAppUnknown( ), "\\ThresholdSettings", "WorkPane", 1 ); 

	try
	{
		StartNotification( cy, 1 );

		smart_alloc(srcRows, color*, cy);
		//smart_alloc(binRows, byte*, cy);
		smart_alloc(dstRows, byte*, cy);
		smart_alloc(grayRows, color*, cy);

		if(1) { // debug - вставка в документ вспомогательной картинки
			if (m_ptrTarget==0) return S_FALSE;
				IDocumentPtr doc(m_ptrTarget);
			if(doc==0) return S_FALSE;
			SetToDocData(doc, sptrGray);
		}


		for(int y=0; y<cy; y++)
		{
			ptrSrc->GetRow(y, m_nWorkPane, &srcRows[y]);
			sptrGray->GetRow(y, m_nWorkPane, &grayRows[y]);
			//ptrBin->GetRow(&binRows[y],y,FALSE);
			ptrDstBin->GetRow(&dstRows[y],y,FALSE);
		}

		const int nAngles = 4;
		double arr_dx[nAngles], arr_dy[nAngles];

		for(int i=0; i<nAngles; i++)
		{
			arr_dx[i] = cos(PI*i/nAngles);
			arr_dy[i] = sin(PI*i/nAngles);
		}

		for(int y=0; y<cy; y++)
		{
			for(int x=0; x<cx; x++)
			{
				dstRows[y][x] = 0;
				grayRows[y][x] = 0;
			}
		}

		for(int y=0; y<cy; y++)
		{
			Notify(y);
			for(int x=0; x<cx; x++)
			{
				for(int i=0; i<nAngles; i++)
				{
					// временно поставим тормозной алгоритм
					color c_max1 = srcRows[y][x];
					color c_max2 = srcRows[y][x];
					color c_min = srcRows[y][x];
					double dx = arr_dx[i];
					double dy = arr_dy[i];
					for(int i=-m_nMask; i<=m_nMask; i++)
					{
						int xx = x + iround(i*dx);
						int yy = y + iround(i*dy);
						if(yy<0 || yy>=cy || xx<0 || xx>=cx)
							continue;
						if(srcRows[yy][xx]<c_min)
							c_min = srcRows[yy][xx];
						if(i<0)
						{
							if(srcRows[yy][xx]>c_max1)
								c_max1 = srcRows[yy][xx];
						}
						else
						{
							if(srcRows[yy][xx]>c_max2)
								c_max2 = srcRows[yy][xx];
						}
					}
					if( c_min == srcRows[y][x] )
						grayRows[y][x] = max(grayRows[y][x], min(c_max1,c_max2) - c_min );
					if( c_min == srcRows[y][x] && (c_max1+c_max2)/2 - c_min >= m_nThreshold )
					{
						dstRows[y][x] += 1;
						//for(int i=-2; i<=2; i++)
						//{
						//	int xx = x + iround(dy*i);
						//	int yy = y + iround(-dx*i);
						//	if(yy<0 || yy>=cy || xx<0 || xx>=cx)
						//		continue;
						//	dstRows[yy][xx] ++;
						//}
					}
					// если точка min по участку вокруг себя - заселектить
				}
			}
		}

		for(int y=0; y<cy; y++)
		{
			for(int x=0; x<cx; x++)
			{
				dstRows[y][x] = dstRows[y][x] > 0 ? 255 : 0;
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

