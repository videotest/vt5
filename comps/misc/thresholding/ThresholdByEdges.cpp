#include "stdafx.h"
#include "ThresholdByEdges.h"
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
_ainfo_base::arg	CThresholdByEdgesInfo::s_pargs[] = 
{
	{"Smooth",	szArgumentTypeInt, "7", true, false },
	{"Img",		szArgumentTypeImage, 0, true, true },
	{"Edges",	szArgumentTypeBinaryImage, 0, true, true },
	{"Binary",	szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};


static double inline sqr(double x)
{ return x*x; }

bool CThresholdByEdges::InvokeFilter()
{
	IImage3Ptr	ptrImg( GetDataArgument("Img") );
	IBinaryImagePtr	ptrEdges( GetDataArgument("Edges") );
	IBinaryImagePtr	ptrBin( GetDataResult() );

	if( ptrImg == NULL || ptrEdges == NULL || ptrBin==NULL )
		return false;

	int nSmooth = GetArgLong("Smooth");

	int cx, cy;
	ptrImg->GetSize(&cx,&cy);

	int cx1, cy1;
	ptrEdges->GetSizes(&cx1,&cy1);
	if(cx1!=cx || cy1!=cy)
		return false;

	ptrBin->CreateNew(cx,cy);
	POINT point;
	ptrImg->GetOffset(&point);
	ptrBin->SetOffset(point,true);

	m_nWorkPane = GetValueInt( GetAppUnknown( ), "\\ThresholdSettings", "WorkPane", 1 ); 

	try
	{
		StartNotification( cy, 1 );

		smart_alloc(srcRows, color*, cy);
		smart_alloc(edgeRows, byte*, cy);
		smart_alloc(binRows, byte*, cy);

		smart_alloc(thrRows, color*, cy);

		smart_alloc(distBuf, color, cx*cy);
		smart_alloc(distRows, color*, cy);

		IUnknownPtr ptrI( ::CreateTypedObject(_bstr_t(szTypeImage)), false );
		IImage3Ptr      sptrThr(ptrI);
		sptrThr->CreateImage( cx,cy, _bstr_t("Gray"), -1 );

		//if(bShowDebugImage)
		if(1) { // debug - вставка в документ вспомогательной картинки
			if (m_ptrTarget==0) return S_FALSE;
				IDocumentPtr doc(m_ptrTarget);
			if(doc==0) return S_FALSE;
			SetToDocData(doc, sptrThr);
		}

		for(int y=0; y<cy; y++)
		{
			ptrImg->GetRow(y, m_nWorkPane, &srcRows[y]);
			ptrEdges->GetRow(&edgeRows[y],y,FALSE);
			ptrBin->GetRow(&binRows[y],y,FALSE);
			sptrThr->GetRow(y, 0, &thrRows[y]);
			distRows[y] = distBuf + y*cx;
		}

		for(int y=0; y<cy; y++)
		{
			for(int x=0; x<cx; x++)
			{
				binRows[y][x] = 255;
				if(edgeRows[y][x])
				{
					int s=0, s1=0;
					for(int yy=-nSmooth; yy<=nSmooth; yy++)
					{
						int y1=y+yy;
						if(y1<0 || y1>=cy) continue;
						for(int xx=-nSmooth; xx<=nSmooth; xx++)
						{
							int x1=x+xx;
							if(x1<0 || x1>=cx) continue;
							if(!edgeRows[y1][x1]) continue;
							if(xx*xx+yy*yy>nSmooth*nSmooth) continue;
							s++;
							s1 += srcRows[y1][x1];
						}
					}
					if(s>=nSmooth)
					{
						thrRows[y][x] = s1/s;
						binRows[y][x] = 0;
						distRows[y][x] = 0;
					}
				}
			}
		}

		for(int y=1; y<cy; y++)
		{
			for(int x=1; x<cx; x++)
			{
				if(distRows[y][x]>distRows[y-1][x-1]+3)
				{
					distRows[y][x] = distRows[y-1][x-1]+3;
					thrRows[y][x] = thrRows[y-1][x-1];
				}
				if(distRows[y][x]>distRows[y-1][x]+2)
				{
					distRows[y][x] = distRows[y-1][x]+2;
					thrRows[y][x] = thrRows[y-1][x];
				}
				if(distRows[y][x]>distRows[y-1][x+1]+3)
				{
					distRows[y][x] = distRows[y-1][x+1]+3;
					thrRows[y][x] = thrRows[y-1][x+1];
				}
				if(distRows[y][x]>distRows[y][x-1]+2)
				{
					distRows[y][x] = distRows[y][x-1]+2;
					thrRows[y][x] = thrRows[y][x-1];
				}
			}
		}

		for(int y=cy-2; y>=0; y--)
		{
			for(int x=cx-2; x>=0; x--)
			{
				if(distRows[y][x]>distRows[y+1][x-1]+3)
				{
					distRows[y][x] = distRows[y+1][x-1]+3;
					thrRows[y][x] = thrRows[y+1][x-1];
				}
				if(distRows[y][x]>distRows[y+1][x]+2)
				{
					distRows[y][x] = distRows[y+1][x]+2;
					thrRows[y][x] = thrRows[y+1][x];
				}
				if(distRows[y][x]>distRows[y+1][x+1]+3)
				{
					distRows[y][x] = distRows[y+1][x+1]+3;
					thrRows[y][x] = thrRows[y+1][x+1];
				}
				if(distRows[y][x]>distRows[y][x+1]+2)
				{
					distRows[y][x] = distRows[y][x+1]+2;
					thrRows[y][x] = thrRows[y][x+1];
				}
			}
		}

		for(int y=0; y<cy; y++)
		{
			for(int x=0; x<cx; x++)
			{
				if(srcRows[y][x]>=thrRows[y][x])
					binRows[y][x] = 255;
				else
					binRows[y][x] = 0;
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

