/*
 * minwrect.c	Jim Piper	January 1984
 *
 * Construct minimum width rectangle from convex hull
 * of type 1 object.
 *
 * Modifications
 *
 *	18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	25 Nov 1987		BDP		protection against single point objects
 *	08 May 1987		BDP		protection against null or empty objs
 *	13 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <math.h>
#include <wstruct.h>

/*
 * extract minimum width rectangle from convex hull
 * (it is relatively obvious that minimum width rectangle long
 * side must be parallel to a chord of convex hull, and all
 * sides must have at least one vertex of convex hull lying
 * within them).
 * Protection just checks object for existence and returns Null object
 * if any probelm found.  bdp 8/5/87 
 */
struct object *
minwrect(cvh)
struct object *cvh;
{
	struct object *makemain(), *robj, *makemain();
	register struct polygondomain *pdom;
	struct polygondomain *makepolydmn();
	register struct frect *rdom;
	struct chord *ch;
	register struct cvhdom *cdom;
	struct chord chl[4], chperp;
	register int i;
	int gap();
	int s,c,np[4];
	

	if (woolz_check_obj(cvh, "minwrect") != 0)
		return(NULL);
	pdom = (struct polygondomain *)cvh->idom;
	cdom = (struct cvhdom *)cvh->vdom;
	if ( (pdom == NULL) || (cdom == NULL))
		return(NULL);
	if ( cdom->nchords < 1 )
		return(NULL);
	rdom = (struct frect *) Malloc(sizeof(struct frect));
	rdom->type = 2;		/* float */
	robj = makemain(20,rdom,NULL,NULL,cvh->assoc);
	
	mwrangle(cvh,&ch,np,np+2,&s,&c);
	rdom->rangle = atan(c!=0? (double)s/(double)c: 10000.0);

	/*
	 * construct two "pseudo-chords" parallel to min-width
	 * rectangle at opposite sides of hull.
	 */
	for (i=0; i<=2; i+=2) {
		chl[i].acon = ch->acon;
		chl[i].bcon = ch->bcon;
		chl[i].ccon = (pdom->vtx[np[i]].vtX - cdom->mdkol)*ch->acon -
				(pdom->vtx[np[i]].vtY - cdom->mdlin)*ch->bcon;
		chl[i].cl = ch->cl;
	}
	/*
	 * construct a "pseudo-chord" perpendicular to min-width
	 * rectangle, find opposites sides, construct two more "pseudo-chords"
	 */
	chperp.acon = -ch->bcon;
	chperp.bcon = ch->acon;
	chperp.cl = ch->cl;
	gap(&chperp,pdom,np+1,np+3);
	for (i=1; i<=3; i+=2) {
		chl[i].acon = chperp.acon;
		chl[i].bcon = chperp.bcon;
		chl[i].ccon = (pdom->vtx[np[i]].vtX - cdom->mdkol)*chperp.acon -
				(pdom->vtx[np[i]].vtY - cdom->mdlin)*chperp.bcon;
		chl[i].cl = chperp.cl;
	}
	/*
	 * compute the intersections of consecutive pairs of these
	 *  pseudo-chords, giving the rectangle vertices.
	 */
	for (i=0; i<=3; i++) {
		intersect(&chl[i],&chl[i==3? 0: i+1],rdom->frk+i,rdom->frl+i);
		rdom->frk[i] = rdom->frk[i] + cdom->mdkol;
		rdom->frl[i] = rdom->frl[i] + cdom->mdlin;
	}
	return(robj);
}


/*
 * compute intersection of two chords (which will be
 * returned in object-central coordinates)
 */
intersect (ch1, ch2, x, y)
register struct chord *ch1, *ch2;
register float *x, *y;
{
	double div;
	
	*y = *x = 0;
	div = (ch2->acon*ch1->bcon - ch2->bcon*ch1->acon);
	if (div > 0){
		*y = ((double)(ch1->acon*ch2->ccon - ch2->acon*ch1->ccon))/div;
		*x = ((double)(ch1->bcon*ch2->ccon - ch2->bcon*ch1->ccon))/div;
	}
}
