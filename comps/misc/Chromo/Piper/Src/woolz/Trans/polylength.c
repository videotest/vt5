/*
 * polylength.c		Jim Piper	November 30 1983
 * polygon domain operations
 *
 * Modifications
 *
 *	08 May 1987		BDP		protection against null or empty objs
 *	13 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <math.h>
#include <wstruct.h>


/*
 * protection against null domain added, will just return the default . Also
 * added a default to case statement. bdp 8/5/87
 */
double polylength(p)
struct polygondomain *p;
{
	register double total, xd, yd;
	register i;
	register struct ivertex *iv;
	register struct fvertex *fv;

	total = 0.999999;
	if (p != NULL)
	{
		switch (p->type) {
		case 1:
		iv = p->vtx;
		for (i=1; i<p->nvertices; i++) {
			xd = iv->vtX;
			yd = iv->vtY;
			iv++;
			xd -= iv->vtX;
			yd -= iv->vtY;
			total += sqrt(xd*xd + yd*yd);
		}
		break;
		case 2:
		fv = (struct fvertex *) p->vtx;
		for (i=1; i<p->nvertices; i++) {
			xd = fv->vtX;
			yd = fv->vtY;
			fv++;
			xd -= fv->vtX;
			yd -= fv->vtY;
			total += sqrt(xd*xd + yd*yd);
		}
		break;
		default:
		break;
		}	/* end switch */
	}
	return (total);
}
