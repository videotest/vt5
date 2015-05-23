/*
 * xexcursion.c		26-05-86		Jim Piper
 *
 * Total x excursion of a polygon
 *
 * Modifications
 *
 *	11 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>

xexcursion(pol)
struct object *pol;
{
	register struct polygondomain *pdom;
	register struct ivertex *vtx;
	register int i, xexc, xd;
	pdom = (struct polygondomain *)pol->idom;
	vtx = pdom->vtx;
	xexc = 0;
	for (i=1; i<pdom->nvertices; i++) {
		xd = vtx->vtX;
		vtx++;
		xd -= vtx->vtX;
		if (xd < 0)
			xd = -xd;
		xexc += xd;
	}
	return(xexc);
}
