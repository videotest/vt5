/*
 * straight.c	Jim Piper	December 1983
 *
 * routines to straighten an object given a smooth medial polygon
 *
 *****************************************************************
 * OS9 version - uses scale by 8 integer polygon
 *****************************************************************
 *
 * given an object, and a smooth mid-points polygon, create a
 * straightened object by
 *     (1) starting at end, moving along mid-points with a
 *         distance increment of 1 per step, i.e. using
 *         non-integral coordinates.
 *     (2) setting up an interval perpendicular to mid-points
 *         direction at point in question, and filling in
 *         grey values by backward rotation/interpolation.
 *     (3) tidying up resulting mess into a standard object.
 *
 * If "demo" non-zero, display straightening as it occurs in
 * picture "frame".  If "demo" zero, "frame" not referenced.
 *
 * Modifications
 *
 *	16 Dec 1988		SEAJ	Return NULL if polygon has only one point
 *	20 Feb 1987		CAS		Remove 'pigs ear' code around threshold and free
 *							the old object properly
 *	11 Jan 1987		CAS		There is little point setting the freeptr
 *							if you leave the linkcount = 0....
 *	17 Dec 1986		jimp	makerect does not set up freeptr to point
 *							to grey-values (this is not unreasonable)
 *							so we do it explicitly here.
 *	11 Sep 1986		CAS		Includes
 */


#include <stdio.h>
#include <wstruct.h>



struct object *straighten(obj, poly, demo, frame)
struct object *obj, *poly;
struct pframe *frame;
{
	struct object *strobj, *tstrobj, *threshold(), *makerect(), *intobj;
	struct intervaldomain *idom;
	struct polygondomain *p;
	struct iwspace iwsp;
	struct gwspace gwsp;
	register GREY *g;
	int strwidth,halfwidth,sline,mlines,i;
	int x,y,sintheta,costheta;
	struct pframe strframe, slframe;

	p = (struct polygondomain *) poly->idom;
	if (poly->type != 10 || p->type != 1)
		return(NULL);
	if (p->nvertices <= 1)
		return(NULL);				/* only one point return NULL */
	mlines = p->nvertices;

	/*
	 * approximate mean width of object
	 * this is used as half-width of straightened object
	 */
	halfwidth = area(obj)/mlines;
	if (halfwidth > 10)
		halfwidth = 10;
	strwidth = 1 + 2*halfwidth;

	/*
	 * make a rectangular object with an empty grey table
	 */
	g = (GREY *) Calloc(mlines*strwidth,sizeof(GREY));
	strobj = makerect(1,mlines,1,strwidth,g,0,NULL,obj);
	strobj->vdom->freeptr = (char *)g;	/* makerect does not do this */
	strobj->vdom->linkcount = 1;

	/*
	 * make some structures for pretty displays
	 */
	if (demo) {
		intobj = makerect(1,1,1,strwidth,NULL,0,NULL,obj);
		strframe.dx = 3584;
		strframe.dy = 2048;
		strframe.scale = frame->scale;
		strframe.ox = (strobj->idom->kol1 + strobj->idom->lastkl)/2;
		strframe.oy = (strobj->idom->line1 + strobj->idom->lastln)/2;
		slframe.ix = strframe.ix = frame->ix;
		slframe.iy = strframe.iy = frame->iy;
		slframe.type = strframe.type = 60;
		slframe.dx = frame->dx;
		slframe.dy = frame->dy;
		slframe.scale = frame->scale / 8;
		slframe.ox = frame->ox * 8;
		slframe.oy = frame->oy * 8;
	}

	/*
	 * start transformation
	 */
	initgreyscan(strobj,&iwsp,&gwsp);
	for (sline=1; sline<=mlines; sline++) {
		/*
	 	* get next column, line (x,y) in unit-spaced steps
	 	* along smoothed mid-points polygon, and local orientation.
	 	* sline is line number in straightened object
	 	*/
		setnextpoint(p,&x,&y,&sintheta,&costheta,sline);
		/*
		 *  rotate and fill rotated interval centred at x,y.
		 */
		nextgreyinterval(&iwsp);
		g = gwsp.grintptr;
		profslice(obj,x,y,sintheta,costheta,halfwidth,g);

		/*
		 * display the interval being rotated on the screen
		 */
		if (demo) {
			disprotint(sline,g,intobj,&strframe);
			disprotslice(x,y,sintheta,costheta,&slframe);
		}
	}
	/*
	 * threshold to remove unset points
	 */
/*	idom = strobj->idom; */
	tstrobj = threshold(strobj,1);
/*	freedomain(idom); */
	if (demo)
		freeobj(intobj);
	freeobj(strobj);
	return(tstrobj);
}



/*
 * for demonstration purposes, display the rotated image slice
 */
static disprotint(sline,g,intobj,frame)
GREY *g;
struct object *intobj;
struct pframe *frame;
{
	/*
	 * intobj is a rectangular object i.e.has rectangular
	 * domain and grey-table
	 */
	register struct intervaldomain *idom = intobj->idom;
	register struct rectvaltb *vdom = (struct rectvaltb *)intobj->vdom;
	idom->line1 = idom->lastln = sline;
	vdom->line1 = vdom->lastln = sline;
	vdom->values = g;
	picframe(intobj,frame);
}


/*
 * for demonstration purposes, display a vector perpendicular
 * to the axis to indicate the slice being processed
 */
static disprotslice(x,y,sintheta,costheta,frame)
register int x,y,sintheta,costheta;
struct pframe *frame;
{
	struct ivector v;
	sintheta /= 8;
	costheta /= 8;
	v.type = 30;
	v.k1 = x - costheta;
	v.l1 = y - sintheta;
	v.k2 = x + costheta;
	v.l2 = y + sintheta;
	picframe(&v,frame);
}
