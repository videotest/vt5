/*
 * mwrangle.c	Jim Piper	January 1984
 *
 * Construct orientation of minimum width rectangle from convex hull
 * of type 1 object.
 *
 * Modifications
 *
 *	18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	09 Dec 1987		BDP		Fix a divide by zero
 *	08 May 1987		BDP		protection against null or empty objs
 *	13 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <math.h>
#include <wstruct.h>

/*
 * extract angle (as scaled sin, cos) of minimum width rectangle
 * from convex hull
 * (it is relatively obvious that minimum width rectangle long
 * side must be parallel to a chord of convex hull, and all
 * sides must have at least one vertex of convex hull lying
 * within them).
 * Protection added just checks the input object and returns null if
 * any probelm found . bdp  8/5/87
 */
mwrangle(cvh,chr,nn1,nn2,s,c)
struct object *cvh;
struct chord **chr;
int *nn1, *nn2, *s, *c;
{
	register struct polygondomain *pdom;
	register struct chord *ch;
	register struct cvhdom *cdom;
	register int i, w, minwidth;
	int n1, n2;
	int gap();
	double os9bodge;

 	if (woolz_check_obj(cvh, "mwrangle") != 0)
 		return(NULL);
 	pdom = (struct polygondomain *)cvh->idom;
	cdom = (struct cvhdom *)cvh->vdom;
	ch = cdom->ch;
	minwidth = gap(ch,pdom,nn1,nn2);
	*chr = ch++;
	for (i=1; i<cdom->nchords; i++) {
		if ((w=gap(ch,pdom,&n1,&n2)) < minwidth) {
			minwidth = w;
			*chr = ch;
			*nn1 = n1;
			*nn2 = n2;
		}
		ch++;
	}
	ch = *chr;
	*c = ch->acon;
	*s = ch->bcon;
}



/*
 * find width of polygon perpendicular to a chord.
 * also return the vertex numbers in polygon which are maximally distant.
 */
gap (ch, pdom, nummin, nummax)
struct chord *ch;
register struct polygondomain *pdom;
register int *nummin;
int *nummax;
{
	register struct ivertex *vtx;
	register int i, c, minc, maxc;
	int acon, bcon;
	vtx = pdom->vtx;
	acon = ch->acon;
	bcon = ch->bcon;
	for (i=0; i<pdom->nvertices; i++) {
		c = acon*vtx->vtX - bcon*vtx->vtY;
		if (i == 0) {
			minc = maxc = c;
			*nummin = *nummax = i;
		} else if (c > maxc) {
			maxc = c;
			*nummax = i;
		} else if (c < minc) {
			minc = c;
			*nummin = i;
		}
		vtx++;
	}
	/* return 32*gap (rounded to integer) */
	return( ch->cl > 0 ? ((maxc - minc) << 8)/ch->cl : 1 );
}
