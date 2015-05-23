/*
 * polysqueeze.c
 *
 * Scale-transform polygon coordinates.
 *
 * Modifications:
 *
 *	08 May 1987		BDP		protection against null or empty objs
 */
#include <stdio.h>
#include <wstruct.h>

/*
 * protection just checks for domain existing, and returns without doing
 * anything if theres a problem. will also do this if vertices if zero as
 * well. bdp 8/5/87
 */
polysqueeze(pdom,xs,ys)
struct polygondomain *pdom;
double xs,ys;
{
	register int i,n,stat;
	register struct ivertex *vtx;
	register struct fvertex *ftx;
	
	stat = 0;
	if (pdom == NULL)
		stat = 1;
	else
	{
		n = pdom->nvertices;
		if (n == 0)
			stat = 1;
		else
			switch (pdom->type) {
			case 1:
				vtx = pdom->vtx;
				for (i=0; i<n; i++,vtx++) {
					vtx->vtX = vtx->vtX*xs;
					vtx->vtY = vtx->vtY*ys;
				}
				break;
			case 2:
				ftx = (struct fvertex *) pdom->vtx;
				for (i=0; i<n; i++,ftx++) {
					ftx->vtX *= xs;
					ftx->vtY *= ys;
				}
				break;
			default:
				stat = 1;
		}
	}
	return(stat);
}
