/*
 * xrange.c		Jim Piper		20-05-86
 *
 * Modifications
 *
 *	11 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>


xrange(pol)
struct object *pol;
{
	register struct polygondomain *pdom;
	register struct ivertex *vtx;
	register int i, minx, maxx;
	pdom = (struct polygondomain *)pol->idom;
	vtx = pdom->vtx;
	minx = vtx->vtX;
	maxx = minx;
	for (i=1; i<pdom->nvertices; i++) {
		if ((++vtx)->vtX < minx)
			minx = vtx->vtX;
		else if (vtx->vtX > maxx)
			maxx = vtx->vtX;
	}
	return(maxx-minx);
}
