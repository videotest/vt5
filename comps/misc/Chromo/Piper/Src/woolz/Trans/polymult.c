/*
 * polymult.c		Jim Piper @ I.R.S.		22-08-86
 *
 * multiply vertex values of polygon by constant
 *
 * modifications:
 *
 *	08 May 1987		BDP		protection against null or empty objs
 */
#include <stdio.h>
#include <wstruct.h>
/*
 * protection against null domain added. no action occurs in case of error.
 * bdp 8/5/87
 */
 
polymult(poly,m)
register struct polygondomain *poly;
register int m;
{
	register int i,stat;
	register struct ivertex *iv;
	register struct fvertex *fv;

	stat = 0;
	if (poly == NULL)
		stat = 1;
	else
	{
		switch (poly->type) {
		case 1:
		iv = poly->vtx;
		for (i=0; i<poly->nvertices; i++) {
			iv->vtX *= m;
			iv->vtY *= m;
			iv++;
		}
		break;

		case 2:
		fv = (struct fvertex *) poly->vtx;
		for (i=2; i<poly->nvertices; i++) {
			fv->vtX *= (float)m;
			fv->vtY *= (float)m;
			fv++;
		}
		break;
		default:
			stat = 1;
			break;
		}
	}
	return(stat);
}
