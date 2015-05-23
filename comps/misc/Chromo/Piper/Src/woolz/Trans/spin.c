/*
 *
 *	S P I N . C --
 *
 *           Image Recognition Systems
 *           720 Birchwood Boulevard
 *           Birchwood Science Park
 *           Warrington
 *           Cheshire
 *           WA3 7PX
 *
 *	Written: J. Piper
 *           Medical Research Council
 *           Clinical and Population Cytogenetics Unit
 *           Edinburgh.
 *
 *	Copyright (c) and intellectual property rights Image Recognition Systems
 *
 *  Date:    November 1983
 *
 *	Description: Rotate object by specified number of degrees. Alternatively, 
 *			 perform a more general rotational linear transformation specified 
 *			 by rotation matrix.
 *
 * 			 If type 1 object, do this by rotating into a sufficiently
 * 			 large object with 1 interval per line.
 * 			 If original object non-negative everywhere, then threshold
 * 			 rotated object to remove blank bays, inclusions, and
 * 			 structures less than 2 pixels high or wide.
 *
 * 			 The rotation is performed about the object mid-point, and
 * 			 in scaled integer arithmetic so that it will run reasonably
 * 			 fast on machines without hardware floating point (in such cases
 * 			 it may be necessary to re-code much of this with "long"
 * 			 integer declarations).
 * 			 In particular, it will produce funny results if the grey values
 * 			 lie outside the range [1, 32767]
 *
 * Modifications
 *
 *	 7 Feb 1991		CAS		Mizar define -> OSK
 *	22 Nov 1988		dcb		Added param. to woolz_exit to say where called from
 *	18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	11 May 1987		BDP		protection against null or empty objs
 *	 2 Mar 1987		GJP		Woolz_exit
 *	13 Oct 1986		GJP		Little escapaded into newvaluttables
 *	 9 Oct 1986		CAS		Changed a freedom, Free to a freeobj
 *	24 Sep 1986		GJP		Big Free not little free
 *	13 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#ifndef C68000
#include <math.h>
#endif C68000
#include <wstruct.h>

static int sin1 = 0;
static int cos1 = 0;
static int sin2 = 0;
static int cos2 = 0;
static int nll = 0;
static int nl1 = 0;
static int nkl = 0;
static int nk1 = 0;
static int turnline = 0;
static int turnkol = 0;
static int fractline = 0;
static int fractkol = 0;
static int lineorigin = 0;
static int kolorigin = 0;


static double xscale,yscale,s,c;

/*
 * Protection added. the functions spin, spinsqueeze, squeeze are the public
 * functions, all of which perform no direct operations upon an object, but
 * pas it to rotrans for processing. for this reason objec tchecking is in
 * that module. for null objects null is returned, for empty a copy of the 
 * original obejct is returned.  bdp  11/5/87
 */
 
struct object * spin (obj, radians)
struct object *obj;
double radians;
{
	struct object *rottrans();

#ifdef WTRACE
	fprintf(stderr,"spin by %d radians\n",radians);
#endif WTRACE
	xscale = 1.0;
	yscale = 1.0;
	return(rottrans(obj,radians));
}


struct object *spinsqueeze (obj,radians,xs,ys)
struct object *obj;
double *radians,*xs,*ys;
{
	struct object *rottrans();
#ifdef WTRACE
	fprintf(stderr,"spinsqueeze %d radians, xscale %d, yscale %d\n",
*radians,*xs,*ys);
#endif WTRACE
	xscale = *xs;
	yscale = *ys;
	return(rottrans(obj,*radians));
}


struct object *squeeze (obj,xs,ys)
struct object *obj;
double xs,ys;
{
	struct object *rottrans();
#ifdef WTRACE
	fprintf(stderr,"squeeze, xscale %d, yscale %d\n",xs,ys);
#endif WTRACE

	xscale = xs;
	yscale = ys;
	return(rottrans(obj,0.0));
}


struct object *
rottrans (obj,radians)
struct object *obj;
double radians;
{
	register l,i,j;
	int mfractline,mfractkol;
	register GREY *g;
	GREY g4[4];
	struct object *makemain(), *nobj, *tobj, *threshold();
	struct intervaldomain *idom, *jdom, *makedomain();
	struct iwspace iwsp;
	struct gwspace gwsp;
	register struct interval *jtvl, *itvl;
	struct valuetable *newvaluetb();
	struct polygondomain *pdom,*npoly,*spinpoly();
	struct boundlist *blist,*nblist,*spinblist();

	/* first check the supplied object */
	
	if ( woolz_check_obj(obj, "rottrans") != 0)
		return(NULL);
	if ( wzemptyidom(obj->idom) > 0)
		return(makemain(1, newidomain(obj->idom), NULL, NULL, NULL ));

	s = 256.0 * sin(radians);
	c = 256.0 * cos(radians);
	sin1 = (int) (xscale*s);
	cos1 = (int) (yscale*c);
	sin2 = (int) (xscale*c);
	cos2 = - (int) (yscale*s);

	switch (obj->type) {
	case 1:
		if (obj->vdom == NULL)
		{
#ifdef WTRACE
			fprintf(stderr,"spin, value domain does not exist\n");
#endif WTRACE
			return(makemain(1, newidomain(obj->idom), NULL, NULL, NULL));
		}
		idom = obj->idom;
		/*
		 * find line limits of rotated object
		 */
		lineorigin = (idom->line1 + idom->lastln)/2;
		kolorigin = (idom->kol1 + idom->lastkl)/2;
		turn (lineorigin,kolorigin);
		nll = turnline;
		nl1 = nll;
		nkl = turnkol;
		nk1 = nkl;
		initrasterscan(obj,&iwsp,0);
		while (nextintervalendpoint(&iwsp) == 0)
			limits(iwsp.linpos,iwsp.colpos);
		nl1--;
		nll++;
		nk1--;
		nkl++;
		/*
		 * make a domain structure
		 */
		jdom = makedomain(1, nl1, nll, nk1, nkl);
		itvl = (struct interval *) Malloc ((nll-nl1+3)*sizeof(struct interval));
		jtvl = itvl;
		jdom->freeptr = (char *) itvl;
		for (l=nl1; l<=nll; l++) {
			jtvl->ileft = nkl;
			jtvl->iright = nk1;
			makeinterval(l, jdom, 1, jtvl);
			jtvl++;
		}
		/*
		 * find column bounds for each line in rotated object.
		 * when considering effect of any given line in input
		 * picture, extend to adjacent lines to ensure enough
		 * space at edges.
		 */
		initrasterscan(obj,&iwsp,0);
		while (nextinterval(&iwsp) == 0) {
			turn(iwsp.linpos, iwsp.colpos);
			for (j=0; j<iwsp.colrmn; j++) {
				jtvl = itvl + turnline - nl1;
				for (i=0; i<2; i++) {
					if (jtvl->ileft > turnkol)
						jtvl->ileft = turnkol;
					if (jtvl->iright < turnkol+1)
						jtvl->iright = turnkol+1;
					jtvl++;
				}
				stepkl();
			}
		}
		/*
		 * correct interval list by finding minimum column of
		 * entire object, and subtract this from all endpoints.
		 * Also clear those lines with empty interval.
		 */
		jtvl = itvl;
		nk1 = jtvl->ileft;
		nkl = jtvl->iright;
		for (l=nl1; l<=nll; l++) {
			if (jtvl->ileft < nk1)
				nk1 = jtvl->ileft;
			if (jtvl->iright > nkl)
				nkl = jtvl->iright;
			jtvl++;
		}
		jtvl = itvl;
		for (l=nl1; l<=nll; l++) {
			if (jtvl->ileft > jtvl->iright) {
				jtvl->ileft = jtvl->iright = 0;
				jdom->intvlines[l-nl1].nintvs = 0;
			} else {
				jtvl->ileft -= nk1;
				jtvl->iright -= nk1;
			}
			jtvl++;
		}
		jdom->kol1 = nk1;
		jdom->lastkl = nkl;
		/*
		 * make a values table
		 */
		nobj = makemain(1,jdom,NULL,NULL,obj);
		nobj->vdom = newvaluetb(nobj,1,obj->vdom->bckgrnd);
		nobj->vdom->linkcount++;
			
		/*
		 * scan new object, filling in grey table
		 * BEWARE - coordinates are scaled up by 256,
		 * interpolated grey values by 65536.  This requires
		 * 32-bit integers to work for most objects.
		 *
		 * Invert the rotation.  Take care with the scaling.
		 */
		sin1 = - (int) (s/yscale);
		cos1 = (int) (c/yscale);
		sin2 = (int) (c/xscale);
		cos2 = (int) (s/xscale);

		initgreyscan(nobj,&iwsp,&gwsp);
		while (nextgreyinterval(&iwsp) == 0) {
			g = gwsp.grintptr;
			turn(iwsp.linpos,iwsp.colpos);
			for (i=0; i<iwsp.colrmn; i++) {
#ifdef OSK
				grey4val(obj,turnline,turnkol,g4);
				*g++ = interp(g4,fractline,fractkol);
#else OSK
				mfractline = 256 - fractline;
				mfractkol = 256 - fractkol;
				grey4val(obj,turnline,turnkol,g4);
				*g++ = (g4[0] * mfractline*mfractkol
				   + g4[1] * mfractline*fractkol
				   + g4[2] * fractline*mfractkol
				   + g4[3] * fractline*fractkol
				   + 32767) / 65536;
#endif OSK
				stepkl();
			}
		}
		/*
		 * threshold to remove unset zero background
		 */
		tobj = threshold (nobj, 1);
		freeobj(nobj);
		return(tobj);
	case 10:
		pdom = (struct polygondomain *) obj->idom;
		lineorigin = pdom->vtx->vtY;
		kolorigin = pdom->vtx->vtX;
		npoly = spinpoly(pdom);
		nobj = makemain(10,npoly,NULL,NULL,obj);
		return(nobj);
	case 11:
		blist = (struct boundlist *) obj->idom;
		lineorigin = blist->poly->vtx->vtY;
		kolorigin = blist->poly->vtx->vtX;
		nblist = spinblist(blist,NULL);
		nobj = makemain(11,nblist,NULL,NULL,obj);
		return(nobj);
	default:
		fprintf(stderr,"can't rotate type %d object\n",obj->type);
		woolz_exit(61, "rottrans");
	}
}


static
limits (l, k)
{
	turn(l,k);
	if (nl1 > turnline)
		nl1 = turnline;
	if (nll < turnline)
		nll = turnline;
	if (nk1 > turnkol)
		nk1 = turnkol;
	if (nkl < turnkol)
		nkl = turnkol;
}


static
turn (l, k)
register int k, l;
{
	register l256,k256;
	l -= lineorigin;
	k -= kolorigin;
	l256 = l*cos1 + k*sin1;
	k256 = l*cos2 + k*sin2;
	turnline = l256 / 256;
	turnkol = k256 / 256;
	fractline = l256 - 256*turnline;
	if (fractline < 0) {
		turnline--;
		fractline += 256;
	}
	fractkol = k256 - 256*turnkol;
	if (fractkol < 0) {
		turnkol--;
		fractkol += 256;
	}
	turnline += lineorigin;
	turnkol += kolorigin;
}


static
stepkl()
{
	fractline += sin1;
	while (fractline >= 256) {
		fractline -= 256;
		turnline++;
	}
	while (fractline < 0) {
		fractline += 256;
		turnline--;
	}
	fractkol += sin2;
	while (fractkol >= 256) {
		fractkol -= 256;
		turnkol++;
	}
	while (fractkol < 0) {
		fractkol += 256;
		turnkol--;
	}
}


static
struct polygondomain *spinpoly(pdom)
struct polygondomain *pdom;
{
	struct polygondomain *npoly,*makepolydmn();
	register struct ivertex *vertex,*nvertex;
	register i;
	vertex = pdom->vtx;
	npoly = makepolydmn(pdom->type,vertex,pdom->nvertices,pdom->maxvertices,1);
	nvertex = npoly->vtx;
	for (i=0; i<pdom->nvertices; i++) {
		turn (vertex->vtY,vertex->vtX);
		nvertex->vtY = turnline;
		nvertex->vtX = turnkol;
		vertex++;
		nvertex++;
	}
	return(npoly);
}


static
struct boundlist *spinblist(blist,up)
struct boundlist *blist,*up;
{
	struct boundlist *nblist;
	struct polygondomain *spinpoly();
	if (blist == NULL)
		return(NULL);
	else {
		nblist = (struct boundlist *) Malloc(sizeof(struct boundlist));
		nblist->type = blist->type;
		nblist->up = up;
		nblist->next = spinblist(blist->next,up);
		nblist->down = spinblist(blist->down,nblist);
		nblist->wrap = blist->wrap;
		nblist->poly = spinpoly(blist->poly);
		return(nblist);
	}
}

