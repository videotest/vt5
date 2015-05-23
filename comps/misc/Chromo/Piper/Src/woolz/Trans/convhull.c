/*
 * convhull.c	Jim Piper	January 1984
 *
 * Construct convex hull of type 1 object
 *
 * Modifications
 *
 *	18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	25 Nov 1987		BDP		Fixed bug with empty object handling !
 *	06 May 1987		BDP		protection against null or empty objs
 *	13 Oct 1986		CAS			Add init of linkcount to vdom after malloc
 *								Changed type of conv hull object from 10 to
 *								type 12..
 *	13 Sep 1986		CAS			Includes
 * 14-05-86 JP Rearranged to suit function "fipconvhull", and bug fixed.
 */

#include <stdio.h>
#include <math.h>
#include <wstruct.h>


struct object *convhull (obj)
struct object *obj;
{
	struct object *cvh, *cvhpoly();
	cvh = cvhpoly(obj);
	chlpars(cvh,obj);
	return(cvh);
}

/*
 * construct the minimal convex polygonal cover from interval domain
 * Protected against null objects. if found then a null will be returned.
 * the function is also protected against empty objects by the scanning
 * routines and will generate an empty polygon domain for an empty domain
 * bdp 6/5/87  
 */
struct object *
cvhpoly (obj)
struct object *obj;
{
	struct object *cvh, *makemain();
	struct polygondomain *cvhpdom, *makepolydmn();
	register struct ivertex *wtx, *w1tx, *w2tx;
	struct iwspace iwsp;
	register int nhalfway;

	if (woolz_check_obj(obj, "cvhpoly") != 0)
		return(NULL);
	
	cvhpdom = makepolydmn(1, NULL, 0, 3 + 2 * (obj->idom->lastln - obj->idom->line1), 1);
	cvh = makemain(12,cvhpdom,NULL,NULL,obj);
	wtx = cvhpdom->vtx;

	/*
	 * procede down right hand side of object, this will fail on first call
	 * to nextinterval for empty object. bdp 6/5/87
	 */
	initrasterscan(obj,&iwsp,0);
	while (nextinterval(&iwsp) == 0) {
		/*
		 * set up first chord
		 */
		if (iwsp.linpos == obj->idom->line1) {
			if (iwsp.nwlpos == 1) {
				wtx->vtX = iwsp.lftpos;
				wtx->vtY = iwsp.linpos;
				wtx++;
			}
			if (iwsp.intrmn == 0) {
				wtx->vtX = iwsp.rgtpos;
				wtx->vtY = iwsp.linpos;
				wtx++;
				cvhpdom->nvertices = 2;
				w1tx = wtx-1;
				w2tx = wtx-2;
			}
		} else {
		/*
		 * add extra chords, checking concavity condition
		 */
			if (iwsp.intrmn == 0) {
				wtx->vtX = iwsp.rgtpos;
				wtx->vtY = iwsp.linpos;
				cvhpdom->nvertices++;
				/*
				 * Concavity condition (may propagate backwards).
				 * Also deals satisfactorily with the case that first
				 * line consists of a single interval, itself a single point.
				 */
				while ((cvhpdom->nvertices >= 3) &&
		    			(wtx->vtY-w2tx->vtY)*(w1tx->vtX-w2tx->vtX) <=
		    			(w1tx->vtY-w2tx->vtY)*(wtx->vtX-w2tx->vtX)) {
					w1tx->vtX = wtx->vtX;
					w1tx->vtY = wtx->vtY;
					wtx--;
					w1tx--;
					w2tx--;
					cvhpdom->nvertices--;
				}
				wtx++;
				w1tx++;
				w2tx++;
			}
		}
	}
	/*
	 * now procede up left hand side of object
	 */
	initrasterscan(obj,&iwsp,3);
	nhalfway = cvhpdom->nvertices + 2;
	while (nextinterval(&iwsp) == 0) {
		if (iwsp.intrmn == 0) {
			wtx->vtX = iwsp.lftpos;
			wtx->vtY = iwsp.linpos;
			cvhpdom->nvertices++;
			/*
			 * Concavity condition (may propagate backwards).
			 * Also deals satisfactorily with the case that last
			 * line consists of a single interval, itself a single point.
			 */
			while ((cvhpdom->nvertices >= nhalfway) &&
		    		(wtx->vtY-w2tx->vtY)*(w1tx->vtX-w2tx->vtX) <=
		    		(w1tx->vtY-w2tx->vtY)*(wtx->vtX-w2tx->vtX)) {
				w1tx->vtX = wtx->vtX;
				w1tx->vtY = wtx->vtY;
				wtx--;
				w1tx--;
				w2tx--;
				cvhpdom->nvertices--;
			}
			wtx++;
			w1tx++;
			w2tx++;
		}
	}
	return(cvh);
}


/*
 * Fill in parameters of the convex hull into the values table.
 * Compute line equation parameters of chords plus 8*length
 * Protection added here by checking for null object and null domain in
 * supplied parameters. Should cope with empty objects also . bdp 6/5/87 
 */
chlpars(cvh, obj)
struct object *cvh, *obj;
{
	struct polygondomain *cvhpdom;
	struct cvhdom *cdom;
	register struct chord *chord;
	register struct ivertex *vtx, *wtx;
	register int i;

	/*
	 * this is a type 12 idom, so need to call woolz_check_pdom but not
	 * woolz_check_cvhdom as this has not been created yet
	 */
	if ( (woolz_check_pdom(cvh->idom, "chlpars.1") != 0) ||
		 (woolz_check_obj(obj, "chlpars.2") != 0 ) )
		return(1);		/* give an error indicator to interested parties */

	cvhpdom = (struct polygondomain *) cvh->idom;
	/*
	 * If no parameter structure, allocate space
	 */
	/* 25/11/87 bdp. need to protect the following malloc from empty object */
	
	if ( cvhpdom->nvertices == 0 )
		return(1);
	if (cvh->vdom == NULL) {
		cvh->vdom = (struct valuetable *) Malloc (sizeof(struct cvhdom) +
			(cvhpdom->nvertices-1) * sizeof(struct chord));
		((struct cvhdom *)cvh->vdom)->ch = (struct chord *)((char *)cvh->vdom + sizeof(struct cvhdom));
/*
 * We ought to make cvhdom compatable with valuetab really
	cdom->freeptr = NULL;
	cdom->linkcount = 1;
 */
 	}
	cdom = (struct cvhdom *) cvh->vdom;
	cdom->mdlin = (obj->idom->line1 + obj->idom->lastln) / 2;
	cdom->mdkol = (obj->idom->kol1 + obj->idom->lastkl) / 2;
	cdom->type = 1;
	cdom->nchords = cvhpdom->nvertices - 1;
	cdom->nsigchords = 0;
	chord = cdom->ch;
	vtx = cvhpdom->vtx;
	wtx = vtx + 1;
	for (i=0; i< cdom->nchords; i++) {
		chord->sig = 0;
		chord->acon = wtx->vtY - vtx->vtY;
		chord->bcon = wtx->vtX - vtx->vtX;
		chord->ccon = (wtx->vtX - cdom->mdkol)*chord->acon - (wtx->vtY - cdom->mdlin)*chord->bcon;
		chord->cl = hypot8(chord->acon, chord->bcon);
		chord++;
		vtx++;
		wtx++;
	}
	return(0);		/* successful completion */
}
