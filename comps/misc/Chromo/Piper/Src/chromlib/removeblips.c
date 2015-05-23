/*
 * removeblips.c		Jim Piper		20/1/84
 *
 * Clean a polygon by a simple hysteresis filter.
 * Polygon assumed to be approximately vertical, and
 * to represent a mid-points polygon of a chromosome.
 *
 * Modifications
 *
 *	11 Sep 1986		CAS		Includes
 */
#include <stdio.h>
#include <wstruct.h>


removeblips(p)
struct object *p;
{
	register int i,n;
	register struct ivertex *vtx;
	register struct polygondomain *pdom;

	pdom = (struct polygondomain *)p->idom;
	vtx = pdom->vtx;
	n = pdom->nvertices-2;
	/* alter position of atypical, isolated points */
	for (i=0; i<n; i++) {
		/* look at x coordinates
		/* if x coordinates of next but adjoining points the same, force
		/* x coordinate of middle point to be same
		/* (cheaper than further checks) */
		if (vtx[i].vtX == vtx[i+2].vtX)
			vtx[i+1].vtX = vtx[i].vtX;
	}
}
