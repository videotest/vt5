/*
 * fipconvhull.c		Jim Piper @ AUC			14-05-86
 *
 * Construct convex hull of FIP-digitised object such that the
 * hull is corrected for the non-square aspect-ratio of FIP digitisation.
 *
 * Derived from ../libsrc/convhull.c
 *
 * Modifications
 *
 *	29 Sep 1986		JIMP(CAS)	Bug in cvhfipsqueeze corrected. Could cause
 *								div by zero in slave about 1 cell in 40
 */

#include <stdio.h>
#include <wstruct.h>


struct object *fipconvhull (obj)
struct object *obj;
{
	struct object *cvh, *cvhpoly();
	cvh = cvhpoly(obj);
	cvhfipsqueeze(cvh->idom);
	chlpars(cvh,obj);
	return(cvh);
}


/*
 * Scale-transform polygon coordinates, remove resulting duplicate
 * points (for convex hull transformation from FIP digitisations).
 */
cvhfipsqueeze(pdom)
struct polygondomain *pdom;
{
	register int i,j,n;
	register struct ivertex *vtx,*wtx;
	
	n = pdom->nvertices;
	switch (pdom->type) {
		case 1:
			/*
			 * squeeze the Y coordinates
			 */
			vtx = pdom->vtx;
			for (i=0; i<n; i++,vtx++)
				vtx->vtY = (3*vtx->vtY+2)/4;
			/*
			 * check for and delete duplicated vertices
			 */
			vtx = pdom->vtx;
			wtx = vtx+1;
			for (i=1; i<n; i++) {
				if (vtx->vtX == wtx->vtX && vtx->vtY == wtx->vtY) {
					for (j=i; j<n; j++,vtx++,wtx++) {
						vtx->vtX = wtx->vtX;
						vtx->vtY = wtx->vtY;
					}
					wtx = pdom->vtx + i;
					vtx = wtx - 1;
					i--;			/** jimp(cas) 29-09-86 **/
					n--;
					pdom->nvertices--;
				} else {
					vtx++;
					wtx++;
				}
			}
			break;
		default:
			fprintf(stderr,"CVHFIPSQUEEZE: Illegal poly type\n");
			break;
	}
}
