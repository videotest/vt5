#include "stdafx.h"
#include "canny.h"
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
_ainfo_base::arg	CCannyThresholdInfo::s_pargs[] = 
{
	{"Threshold",	szArgumentTypeInt, "32", true, false },
	{"Kind",	szArgumentTypeInt, "0", true, false }, // 0 - просто canny, 1 - только контура темных выпуклых, -1 - только контура светлых выпуклых
	{"Img",		szArgumentTypeImage, 0, true, true },
	{"Canny",	szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};


static double inline sqr(double x)
{ return x*x; }

static inline int iround(double x)
{
	return int(floor(x+0.5));
}

bool CCannyThreshold::InvokeFilter()
{
	IImage3Ptr	ptrSrc( GetDataArgument() );
	IBinaryImagePtr	ptrBin( GetDataResult() );

	if( ptrSrc == NULL || ptrBin==NULL )
		return false;

	int cx, cy;
	ptrSrc->GetSize(&cx,&cy);
	ptrBin->CreateNew(cx,cy);
	POINT point;
	ptrSrc->GetOffset(&point);
	ptrBin->SetOffset(point,true);

    m_nThreshold = GetArgLong("Threshold")*256;
	m_nKind = GetArgLong("Kind");
    m_nWorkPane = GetValueInt( GetAppUnknown( ), "\\ThresholdSettings", "WorkPane", 1 ); 

	try
	{
		StartNotification( cy, 1 );

		smart_alloc(srcRows, color*, cy);
		smart_alloc(binRows, byte*, cy);

		for(int y=0; y<cy; y++)
		{
			ptrSrc->GetRow(y, m_nWorkPane, &srcRows[y]);
			ptrBin->GetRow(&binRows[y],y,FALSE);
		}

		//IUnknownPtr ptrI( ::CreateTypedObject(_bstr_t(szTypeImage)), false );
		//IImage3Ptr      sptrI(ptrI);
		//sptrI->CreateImage( cx,cy, _bstr_t("Gray"), -1 );

		//smart_alloc(grayRows, color *, cy);
		//for(int y=0; y<cy; y++)
		//{
		//	sptrI->GetRow(y, 0, grayRows+y);
		//	for(int x=0; x<cx; x++)
		//		grayRows[y][x] = 32768;
		//}

		////if(bShowDebugImage)
		//if(1) { // debug - вставка в документ вспомогательной картинки
		//	if (m_ptrTarget==0) return S_FALSE;
		//		IDocumentPtr doc(m_ptrTarget);
		//	if(doc==0) return S_FALSE;
		//	SetToDocData(doc, sptrI);
		//}

		for(int y=2; y<cy-2; y++)
		{
			for(int x=2; x<cx-2; x++)
			{
				binRows[y][x] = 0;
				int grad_x = int(srcRows[y][x+1]) - srcRows[y][x-1];
				int grad_y = int(srcRows[y+1][x]) - srcRows[y-1][x];
				double grad = _hypot(grad_x, grad_y);
				if(grad>m_nThreshold)
				{
					int dx, dy;
					if(abs(grad_x)>abs(grad_y))
					{
						dx=1; dy=0;
					}
					else
					{
						dx=0; dy=1;
					}
					int grad1_x = int(srcRows[y+dy][x+dx+1]) - srcRows[y+dy][x+dx-1];
					int grad1_y = int(srcRows[y+dy+1][x+dx]) - srcRows[y+dy-1][x+dx];
					double grad1 = _hypot(grad1_x, grad1_y);
					int grad2_x = int(srcRows[y-dy][x-dx+1]) - srcRows[y-dy][x-dx-1];
					int grad2_y = int(srcRows[y-dy+1][x-dx]) - srcRows[y-dy-1][x-dx];
					double grad2 = _hypot(grad2_x, grad2_y);
					if (grad>=grad1 && grad>=grad2)
					{
						binRows[y][x] = 255;
						if(m_nKind)
						{ // проверить тип границы - ограничивает она светлое пятно или темное
							int h=8;
							h = min(cx-1-x, min(x, h)); // h - не далее, чем до края
							h = min(cy-1-y, min(y, h)); // h - не далее, чем до края
							double step_x = grad_x/grad;
							double step_y = grad_y/grad;
							int cnt = 0;
							int c = srcRows[y][x];
							int th0 = c - m_nThreshold/2;
							int th1 = c + m_nThreshold/2;
							for(int k=1; k<=h; k++)
							{
								int dx = iround(step_x*k);
								int dy = iround(step_y*k);
								int c1 = srcRows[y-dx][x+dy];
								if(c1>th1) cnt++;
								else if(c1<th0) cnt--;
								c1 = srcRows[y+dx][x-dy];
								if(c1>th1) cnt++;
								else if(c1<th0) cnt--;
								if(abs(cnt)>2) break;
							}
							if( m_nKind<0 && cnt<=2 ) binRows[y][x] = 0;
							if( m_nKind>0 && cnt>=-2 ) binRows[y][x] = 0;
						}

						//// для помеченных точек - нарисовать черный круг на gray
						//if(binRows[y][x]==255)
						//{
						//	double step_x = grad_x/grad;
						//	double step_y = grad_y/grad;
						//	int r=12;
						//	int x0 = x - iround(r*step_x);
						//	int y0 = y - iround(r*step_y);
						//	for(int yy=-r; yy<=r; yy++)
						//	{
						//		int y1 = y0+yy;
						//		if(y1<0 || y1>=cy) continue;
						//		for(int xx=-r; xx<=r; xx++)
						//		{
						//			int x1 = x0+xx;
						//			if(x1<0 || x1>=cx) continue;
						//			if(xx*xx+yy*yy>r*r) continue;
						//			if(xx*step_x+yy*step_y<r/2) continue;
						//			grayRows[y1][x1] = max(0, grayRows[y1][x1] - grad/64);
						//		}
						//	}
						//	x0 = x + iround(r*step_x);
						//	y0 = y + iround(r*step_y);
						//	for(int yy=-r; yy<=r; yy++)
						//	{
						//		int y1 = y0+yy;
						//		if(y1<0 || y1>=cy) continue;
						//		for(int xx=-r; xx<=r; xx++)
						//		{
						//			int x1 = x0+xx;
						//			if(x1<0 || x1>=cx) continue;
						//			if(xx*xx+yy*yy>r*r) continue;
						//			//grayRows[y1][x1] = min(65535, grayRows[y1][x1] + 128);
						//		}
						//	}
						//}
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

	return true;
}

/////////////////////////////////////////////////////////////////////////////

