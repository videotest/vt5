/*
 * iuspace.c
 *
 * Convert an integer polygon to an integer polygon in which
 * the vertices are 8-connected neighbours, such that the path
 * chosen minimises the perpendicular distance from any vertex
 * in the new polygon and the arc which it is derived from.
 *
 * Modifications
 *
 *	12 Nov 1987		BDP		ipolylength didn't cope with empty polys properly
 *	08 May 1987		BDP		protection against null or empty objs
 *	13 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>

/*
 * expression for triangle area
 */
#define AREA(x1,x2,x3,y1,y2,y3) (y1+y2)*(x2-x1)+(y2+y3)*(x3-x2)+(y3+y1)*(x1-x3)
#define max(a,b) (a>=b? a: b)

/*
 * protection added just needs to check pdom exists. if not NULL is returned.
 * routine already copes with an empty domain.  bdp 8/5/87
 */
struct polygondomain *iuspace(pdom)
struct polygondomain *pdom;
{
	int i,n,x,y,x1,x2,y1,y2,xinc1,xinc2,yinc1,yinc2,xdiff,ydiff;
	int xpot1,ypot1,xpot2,ypot2,area1,area2;
	struct polygondomain *ndom, *makepolydmn();
	register struct ivertex *vtx, *wtx;

	if (pdom == NULL)
		return(NULL);
	ndom = makepolydmn(1,NULL,0,ipolylength(pdom),1);
	ndom->nvertices = ndom->maxvertices;
	n = pdom->nvertices;
	vtx = pdom->vtx;
	wtx = ndom->vtx;
	x = x1 = wtx->vtX = vtx->vtX;
	y = y1 = wtx->vtY = vtx->vtY;
	for (i=1; i<n; i++) {
		vtx++;
		x2 = vtx->vtX;
		y2 = vtx->vtY;
		xdiff = x2-x1;
		ydiff = y2-y1;
		/*
		 * compute possible increment directions
		 * in each octant, these are either parallel
		 * to an axis or parallel to a diagonal
		 */
		xinc1 = yinc1 = 0;
		if (ydiff >= 0) {
			yinc2 = 1;
			if (xdiff >= 0) {
				xinc2 = 1;
				if (ydiff >= xdiff)
					yinc1 = 1;
				else
					xinc1 = 1;
			} else {
				xinc2 = -1;
				if (ydiff+xdiff >= 0)
					yinc1 = 1;
				else
					xinc1 = -1;
			}
		} else {
			yinc2 = -1;
			if (xdiff >= 0) {
				xinc2 = 1;
				if (ydiff+xdiff <= 0)
					yinc1 = -1;
				else
					xinc1 = 1;
			} else {
				xinc2 = -1;
				if (ydiff <= xdiff)
					yinc1 = -1;
				else
					xinc1 = -1;
			}
		}
		/*
		 * using the potential movements computed above,
		 * construct the path from x1,y1 to x2,y2
		 * such that at each step the next point chosen is
		 * that perpendicularly closer to line (x1,y1),(x2,y2)
		 */
		while (x != x2 || y != y2) {
			xpot1 = x + xinc1;
			ypot1 = y + yinc1;
			xpot2 = x + xinc2;
			ypot2 = y + yinc2;
			area1 = AREA(x1,xpot1,x2,y1,ypot1,y2);
			area2 = AREA(x1,xpot2,x2,y1,ypot2,y2);
			if (area1 < 0)
				area1 = -area1;
			if (area2 < 0)
				area2 = -area2;
			if (area1 < area2) {
				x = xpot1;
				y = ypot1;
			} else {
				x = xpot2;
				y = ypot2;
			}
			wtx++;
			wtx->vtX = x;
			wtx->vtY = y;
		}
		x1 = x2;
		y1 = y2;
	}
	return(ndom);
}


/*
 * return the number of vertices required to convert a polygon
 * to an "iunitspace()"-ed polygon.
 * 		Added a check on vertex count, cos min value of one gets returned by
 * this routine converting an empty polygon into a non empty one ! bdp 12/11/87
 */
ipolylength(pdom)
struct polygondomain *pdom;
{
	register int i,n,x,y;
	int l;
	register struct ivertex *vtx, *wtx;

	n = pdom->nvertices;
	if ( n < 2 )
		return(n);		/* zero for empty or one for single, bdp 12/11/87 */
	vtx = pdom->vtx;
	wtx = vtx+1;
	l = 1;
	for (i=1; i<n; i++) {
		x = wtx->vtX - vtx->vtX;
		y = wtx->vtY - vtx->vtY;
		if (x < 0)
			x = -x;
		if (y < 0)
			y = -y;
		l += max(x,y);
		vtx++;
		wtx++;
	}
	return(l);
}
