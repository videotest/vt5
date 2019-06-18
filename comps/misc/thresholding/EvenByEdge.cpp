#include "stdafx.h"
#include "evenbyedge.h"

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
_ainfo_base::arg	CEvenByEdgeInfo::s_pargs[] = 
{
	{"Img",		szArgumentTypeImage, 0, true, true },
	{"Bin",		szArgumentTypeBinaryImage, 0, true, true },
	{"EvenByEdge",	szArgumentTypeImage, 0, false, true },
	{0, 0, 0, false, false },
};


static double inline sqr(double x)
{ return x*x; }

static inline int round(double x)
{
	return int(floor(x+0.5));
}

bool CEvenByEdge::InvokeFilter()
{
	IImage3Ptr	ptrSrc( GetDataArgument("Img") );
	IBinaryImagePtr	ptrBin( GetDataArgument("Bin") );
	IImagePtr	ptrDst( GetDataResult() );

	if( ptrSrc == NULL || ptrBin==NULL || ptrDst==NULL )
		return false;

    m_nWorkPane = GetValueInt( GetAppUnknown( ), "\\ThresholdSettings", "WorkPane", 1 ); 

	int cx, cy;
	ptrSrc->GetSize(&cx,&cy);
	int cx2, cy2;
	ptrBin->GetSizes(&cx2,&cy2);
	if(cx2!=cx || cy2!=cy)
		return false;

	{	
		ICompatibleObjectPtr ptrCO( ptrDst );
		if( ptrCO == NULL )
			return false;
		if( S_OK != ptrCO->CreateCompatibleObject( ptrSrc, NULL, 0 ) )
			return false;
	}

	try
	{
		StartNotification( cy, 1 );

		smart_alloc(srcRows, color*, cy);
		smart_alloc(binRows, byte*, cy);
		smart_alloc(dstRows, color*, cy);

		for(int y=0; y<cy; y++)
		{
			ptrSrc->GetRow(y, m_nWorkPane, &srcRows[y]);
			ptrBin->GetRow(&binRows[y],y,FALSE);
			ptrDst->GetRow(y, m_nWorkPane, &dstRows[y]);
		}

		for(int y=0; y<cy; y++)
		{
			//Notify(y);
			for(int x=0; x<cx; x++)
			{
				dstRows[y][x] =
					binRows[y][x] ? 255 : 0;
			}
		}

		for(int y=1; y<cy-1; y++)
		{
			//Notify(y);
			for(int x=1; x<cx-1; x++)
			{
				int c = dstRows[y][x];
				c = min(c, dstRows[y-1][x]+1);
				c = min(c, dstRows[y][x-1]+1);
				dstRows[y][x] = c;
			}
		}
		for(int y=cy-2; y>0; y--)
		{
			//Notify(y);
			for(int x=cx-2; x>0; x--)
			{
				int c = dstRows[y][x];
				c = min(c, dstRows[y+1][x]+1);
				c = min(c, dstRows[y][x+1]+1);
				dstRows[y][x] = c;
			}
		}
		
		double s0[256], s1[256];
		for(int i=0; i<256; i++)
		{
			s0[i] = s1[i] = 0;
		}

		for(int y=0; y<cy; y++)
		{
			//Notify(y);
			for(int x=0; x<cx; x++)
			{
				int c = dstRows[y][x];
				s0[c] ++ ; 
				s1[c] += srcRows[y][x]; 
			}
		}

		for(int i=0; i<256; i++)
		{
			s1[i] /= max(1, s0[i]);
		}

		for(int y=0; y<cy; y++)
		{
			//Notify(y);
			for(int x=0; x<cx; x++)
			{
				int i = dstRows[y][x];
				int c = srcRows[y][x];
				dstRows[y][x] = s1[i];
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

