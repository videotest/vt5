/*
 * axoutside.c		26-05-86		Jim Piper
 *
 * find if any polygon vertex except first or last lies to left of
 * left-most interval of object, or to right of right-most interval
 *
 * Modifications
 *
 *	23 Sep 1987	jimp		Remove the interesting message !
 *	11 Sep 1986	CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>

axoutside(mpol,obj)
struct object *mpol, *obj;
{
	struct polygondomain *pdom;
	struct ivertex *vtx;
	struct intervaldomain *idom;
	struct intervalline *itvln;
	int i, outside, n, nints, vx;

	outside = 0;
	idom = obj->idom;
	pdom = (struct polygondomain *)mpol->idom;
	vtx = pdom->vtx + 2;
	n = pdom->nvertices - 4;
	for (i=0; i<n; i++,vtx++) {
		itvln = idom->intvlines + vtx->vtY - idom->line1;
		if ((nints = itvln->nintvs) == 0)
			continue;	/* no intervals in this line (???) */
		vx = vtx->vtX - idom->kol1;
		if (vx < itvln->intvs->ileft || vx > itvln->intvs[nints-1].iright) {
			outside = 1;
			break;		/* from for loop */
		}
	}
	return(outside);
}
