/*
 * modifytips.c		Jim Piper		20/1/84
 *
 * Modify tips of a mid-points polygon by deleting existing
 * tips and projecting more central part of polygon.
 *
 * This algorithm is not very successful.
 *
 * Modifications
 *
 *	11 Sep 1986		CAS		Includes
 */
#include <stdio.h>
#include <wstruct.h>


modifytips(p)
struct object *p;
{
	register int i,n,m,x;
	int m2,n1,n2;
	register struct ivertex *vtx;
	register struct polygondomain *pdom;

	pdom = (struct polygondomain *)p->idom;
	vtx = pdom->vtx;
	n = pdom->nvertices;
	m = n>>3;
	if (m<2)
		m = 2;
	m2 = m<<1;
	x = vtx[m].vtX << 1;
	for (i=0; i<m; i++)
		vtx[i].vtX = x - vtx[m2-i].vtX;
	n1 = n-1;
	n2 = n1-m2;
	x = vtx[n1-m].vtX << 1;
	for (i=0; i<m; i++)
		vtx[n1-i].vtX = x - vtx[n2+i].vtX;
}
