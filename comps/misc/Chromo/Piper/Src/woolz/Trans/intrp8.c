/*
 * intrp8.c		Jim Piper		4 September 1984
 *
 * Convert an integer values polygon to corresponding floating
 * point polygon with vertex values divided by 8.
 *
 * Modifications
 *
 *	08 May 1987		BDP		protection against null or empty objs
 *	13 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>

/*
 * Only protection necessary here checks polygon exists and returns NULL
 * if not. bdp 8/5/87
 */
struct polygondomain *intrp8(p1)
struct polygondomain *p1;
{
	struct polygondomain *p2 , *makepolydmn();
	register struct ivertex *iv;
	register struct fvertex *fv;
	register i;

	if (p1 == NULL)
		return(NULL);
 	p2 = makepolydmn(2,NULL,0,p1->nvertices,1);
	p2->nvertices = p1->nvertices;
	iv = p1->vtx;
	fv = (struct fvertex *) p2->vtx;
	for (i=0; i<p1->nvertices; i++) {
		fv->vtX = (double) iv->vtX / 8.0;
		fv->vtY = (double) iv->vtY / 8.0;
		fv++;
		iv++;
	}
	return(p2);
}
