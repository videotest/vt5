/*
 * itrpoly.c		Jim Piper	November 30 1983
 * polygon domain operations
 *
 * Modifications
 *
 *	08 May 1987		BDP		protection against null or empty objs
 *	13 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>

/*
 * only protection necessary here is to check pdom exists and if not return
 * a NULL domain. It will cope with empty domains ok.  bdp 8/5/87 
 */
struct polygondomain *inttorealpoly(p1)
struct polygondomain *p1;
{
	struct polygondomain *p2 , *makepolydmn();
	register struct ivertex *iv;
	register struct fvertex *fv;
	register i;

	if ( p1 == NULL )
		return(NULL);
	p2 = makepolydmn(2,NULL,0,p1->nvertices,1);
	p2->nvertices = p1->nvertices;
	iv = p1->vtx;
	fv = (struct fvertex *) p2->vtx;
	for (i=0; i<p1->nvertices; i++) {
		fv->vtX = (float) iv->vtX;
		fv->vtY = (float) iv->vtY;
		fv++;
		iv++;
	}
	return(p2);
}
