/*
 * unitbound.c		Jim Piper		20 November 1984
 *
 * Make the outermost polygon of a boundary such that polygon
 * vertices lie on adjacent (8-connected) picture points, i.e. suppress
 * the compression into extended arcs.
 *
 * Use of a boundary with "wrap" = 1 will give a closed polygon.
 *
 * The result is a single polygon object.  If the object is disconnected then
 * only the FIRST component boundary is taken.  This routine takes as input
 * a boundary and so may not be as efficient as extracting the polygon
 * directly from the object; however, it leads to a small program.
 *
 * Modifications
 *
 *	22 Nov 1988		dcb		Added param. to woolz_exit to say where called from
 *	18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	11 May 1987		BDP		protection against null or empty objs
 *	2  Mar 1987		GJP		Woolz_exit
 *	13 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>
#define ABS(i)	(i<0? -i: i)

/* protections added just chec supplied object for null and returns null if any
 * problem with it. Bdp  11/5/87 
 */
struct object *
unitbound(bound)
struct object *bound;
{
	struct object *ubound, *makemain();
	struct polygondomain *bpol, *upol, *makepolydmn();
	register struct ivertex *ntx, *mtx;
	struct ivertex *vtx, *wtx;
	register int i,j,xinc,yinc;
	int length,bleng;

	if (woolz_check_obj(bound, "unitbound") != 0)
		return(NULL);
		
	if (bound->type != 11) {
		fprintf(stderr,"Not a boundary\n");
		woolz_exit(21, "unitbound");
	}
	bpol = ((struct boundlist *)(bound->idom))->poly;

	ntx = bpol->vtx;
	mtx = ntx+1;
	length = 1;
	bleng = bpol->nvertices - 1;

	/*
	 * find length of old polygon.
	 */
	for (i=0; i<bleng; i++) {
		xinc = mtx->vtX - ntx->vtX;
		if (xinc != 0)
			length += ABS(xinc);
		else {
			yinc = mtx->vtY - ntx->vtY;
			length += ABS(yinc);
		}
		ntx++;
		mtx++;
	}

	/*
	 * make a new polygon
	 */
	upol = makepolydmn(1,NULL,0,length,1);
	ubound = makemain(10,upol,NULL,NULL,NULL);
	upol->nvertices = length;
	ntx = upol->vtx;
	mtx = ntx + 1;
	wtx = bpol->vtx;
	vtx = wtx + 1;
	
	/*
	 * fill new polygon
	 */
	ntx->vtX = wtx->vtX;
	ntx->vtY = wtx->vtY;
	for (i=0; i<bleng; i++) {
		xinc = vtx->vtX - wtx->vtX;
		yinc = vtx->vtY - wtx->vtY;
		if (xinc != 0)
			length = ABS(xinc);
		else
			length = ABS(yinc);
		if (xinc > 0)
			xinc = 1;
		else if (xinc < 0)
			xinc = -1;
		if (yinc > 0)
			yinc = 1;
		else if (yinc < 0)
			yinc = -1;
		for (j=0; j<length; j++) {
			mtx->vtX = ntx->vtX + xinc;
			mtx->vtY = ntx->vtY + yinc;
			mtx++;
			ntx++;
		}
		vtx++;
		wtx++;
	}
	return(ubound);
}
