/*
 * setnextpoint.c		Jim Piper		20/1/84
 *
 * extract the following from a mid-points polygon which is
 * (1) unitspaced, (2) in scale *8 integer coordinate system:
 *
 * (1) The scaled *8 coordinates of the sln'th point of the polygon
 * (2) The scaled *256 slope of the profile at that point, expressed
 *     as sine and cosine.
 *
 * Modifications
 *
 *	11 Sep 1986		CAS		Includes
 */
#include <stdio.h>
#include <wstruct.h>
#include <chromanal.h>

setnextpoint(p,xx,yy,sini,cosi,sln)
struct polygondomain *p;
int *xx,*yy,*sini,*cosi;
{
	register struct ivertex *fv;
	register int int1,int2;
	fv = p->vtx + sln - 1;
	*xx=fv->vtX;
	*yy=fv->vtY;
	int1=sln-HALF_SLOPE_NHD;
	int2=sln+HALF_SLOPE_NHD;
	if (int1 < 1) {
		int1=1;
		int2++;
	}
	if (int2 > p->nvertices) {
		int2=p->nvertices;
		if (int1 > 1)
			int1--;
	}
	axisslope(p,int1,int2,sini,cosi);
}
