#include "stdafx.h"
#include "misc_utils.h"

void ContourSetPoint( Contour *pcntr, int nNumberPoint, int x, int y)
{
	try
	{
		pcntr->ppoints[nNumberPoint].x = x;
		pcntr->ppoints[nNumberPoint].y = y;
	}
	catch(...)
	{
		dbg_assert(FALSE);
	}
}

void ContourAddPoint( Contour *pcntr, int x, int y, bool bAddSamePoints = true)
{
	if(pcntr->nContourSize > 0)
	{
		if(pcntr->ppoints[pcntr->nContourSize-1].x == x &&
		   pcntr->ppoints[pcntr->nContourSize-1].y == y && !bAddSamePoints)
		   return;
	}
	const int iContourAligment=1000;
	if( !pcntr->nAllocatedSize )
	{
		dbg_assert( !pcntr->ppoints );

		pcntr->ppoints = new ContourPoint[iContourAligment];
		pcntr->nAllocatedSize = iContourAligment;
		
	}

	if( pcntr->nContourSize == pcntr->nAllocatedSize )
	{
		ContourPoint	*p = pcntr->ppoints;
		pcntr->ppoints = new ContourPoint[pcntr->nAllocatedSize+iContourAligment];
		memcpy( pcntr->ppoints, p, pcntr->nAllocatedSize*sizeof( ContourPoint ) );
		pcntr->nAllocatedSize+=iContourAligment;
		delete p;
	}

	pcntr->ppoints[pcntr->nContourSize].x = x;
	pcntr->ppoints[pcntr->nContourSize].y = y;
	pcntr->nContourSize++;
}
