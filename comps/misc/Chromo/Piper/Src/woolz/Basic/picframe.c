/*
 * picframe.c	Jim Piper	10 August 1983
 *
 * Draw an object in DIGS.
 *
 * Modifications
 *
 *	 7 Feb 1991		CAS		voids
 *	02 Dec 1988		SCG		rewrite picpoly to be more stack efficient
 *  18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	20 Aug 1987	jimp@IRS	arrowhead() call for arrow cursor.
 *					CHECK that this mod matches the MRC
 *					version of 04-03-87.
 *	06 May 1987		BDP		protection against null or empty objs
 *	12 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>

#define ABS(i)	(i>=0? i: -i)

#ifdef PICDEBUG
extern FILE *pic_chan;
#endif PCIDEBUG

/*
 * Protection added to this module assumes that picframe function is the
 * only public function in this module.  The only checking necessary is
 * for the object existing. If the object is null immediate return is
 * effected. All routines should copy with an empty object, either by
 * their own for loops , or by protection added to  the interval scanning
 * routines.  bdp 6/5/87 
 */
picframe (obj, f)
struct object *obj;
struct pframe *f;
{
	struct intervaldomain *idom;
	struct ipoint *pt;
	
#ifdef PICDEBUG

	int st;
	
	/* the obscure conditional is afudge to keep the cursor out hopefully */
	
	st = 1;
	if (obj->type == 40)
	{
		pt = (struct ipoint *)obj;
		if (pt->style == 6)
			st = 0;			/* must be the cursor so ignore it */
	}
	if (st)
	{
		fprintf(pic_chan,"type %d  ox=%d, oy=%d, ix=%d, iy=%d, scale=%d, dx=%d, dy=%d\n",
				obj->type,f->ox,f->oy,f->ix,f->iy,f->scale,f->dx,f->dy);
		switch (obj->type)
		{
		case 1:
			idom = obj->idom;
			fprintf(pic_chan,"    lines %d to %d, cols %d to %d\n",
				idom->line1,idom->lastln,idom->kol1,idom->lastkl);
			break;
		case 40:
			fprintf(pic_chan,"    style %d,line %d,  col %d\n",pt->style,pt->l,pt->k);
			break;
		default:
			break;
		}
		fflush(pic_chan);
	}
#endif PICDEBUG
	
	if (woolz_check_obj(obj, "picframe") != 0)
		return(1);		/* return a failed status to anyone interested */
		
	/* switch on object type */
	switch(obj->type) {

	case 0 :		/* interval/grey-table object */
	case 1 :
		picintobj(obj,f);
		break;

	case 10 :	/* polygon */
	case 12 :	/* convexhull */
		picpoly(obj->idom,f);
		break;

	case 11 :	/* boundary list */
		picbound(obj->idom,f);
		break;

	case 13:	/* histogram */
		pichisto(obj->idom,f);
		break;

	case 20 :	/* rectangle - vertices specified */
		picrect(obj->idom,f);
		break;

	case 30 :	/* vectors */
	case 31 :
		picvec(obj,f);
		break;

	case 40 :	/* point (cursor) */
	case 41 :
		picpoint(obj,f);
		break;

	case 70 :	/* text object */
		pictext(obj,f);
		break;

	default:
		break;
	}
	return(0);	/* operation completed ok */
}

static
picintobj(obj,f)
struct object *obj;
register struct pframe *f;
{
	struct iwspace iwsp;
	struct gwspace gwsp;
	register GREY *g;
	register x,xo,yo,xs,ys;
	int xs3,ys3,intlength;
	xs = f->ix*f->scale;
	ys = f->iy*f->scale;
	xo = f->dx - xs*f->ox;
	yo = f->dy - ys*f->oy;
	if (obj->vdom == NULL) {
		/* scanning routines will protect against empty objects */
		initrasterscan(obj,&iwsp,0);
		while (nextinterval(&iwsp) == 0) {
			intlength = xs*iwsp.colrmn;
			moveto(xo + xs*iwsp.lftpos, yo + ys*iwsp.linpos);
			lineby(0,ys);
			lineby(intlength,0);
			lineby(0,-ys);
			lineby(-intlength,0);
		}
	} else {
		initgreyscan(obj,&iwsp,&gwsp);
		while (nextgreyinterval(&iwsp) == 0) {
			moveto(xo + xs*iwsp.lftpos, yo + ys*iwsp.linpos);
			pixline(iwsp.colrmn,ABS(xs),ABS(ys),gwsp.grintptr,
					(f->ix>0? 1: -1));
		}
	}
}


/*
 * display a polygon domain
 */
picpoly(pdom,f)
struct polygondomain *pdom;
struct pframe *f;
{
	register struct fvertex *fverts;
	register struct ivertex *iverts;
	register i,xo,yo,xs,ys;
	int xdum,ydum;
	xs = f->ix*f->scale;
	ys = f->iy*f->scale;
	xo = f->dx - xs*f->ox + ABS(xs)/2;	/* to line up centrally with pixels */
	yo = f->dy - ys*f->oy + ABS(ys)/2;
	switch (pdom->type) {
	case 1 :
		iverts = pdom->vtx;
		moveto(xo+xs*iverts->vtX, yo+ys*iverts->vtY);
		for (i=1; i<pdom->nvertices; i++) {
			iverts++;
			lineto(xo+xs*iverts->vtX, yo+ys*iverts->vtY);
		}
		break;
	case 2 :
/* compiler fault in OS-9 - generates illegal code here */
		fverts = (struct fvertex *) pdom->vtx;
		xdum = xs*fverts->vtX;
		ydum = ys*fverts->vtY;
		moveto(xo+xdum, yo+ydum);
		for (i=1; i<pdom->nvertices; i++) {
			fverts++;
			xdum = xs*fverts->vtX;
			ydum = ys*fverts->vtY;
			lineto(xo+xdum, yo+ydum);
		}
		break;
	default:
		break;
	}
}



/*
 * display boundary list of polygons recursively
 */
picbound(bound,f)
struct boundlist *bound;
struct pframe *f;
{
	struct boundlist *bp;

	/*
	 * This code has been replaced with code below as the latter is more
	 * stack efficient
	 *	if (bound != NULL) {
	 *		picpoly(bound->poly,f);
	 *		picbound(bound->next,f);
	 *		picbound(bound->down,f);
	 *	}
	 */	
		
	if((bp = bound) != NULL) {
		do {
			picpoly(bp->poly, f);
			picbound(bp->down, f);
		} while((bp = bp->next) != NULL);
	}
}


/*
 * display a vector
 */
picvec(vec,f)
struct ivector *vec;
struct pframe *f;
{
	register struct fvector *fvec;
	register i,xo,yo,xs,ys;
	xs = f->ix*f->scale;
	ys = f->iy*f->scale;
	xo = f->dx - xs*f->ox + ABS(xs)/2;	/* to line up centrally with pixels */
	yo = f->dy - ys*f->oy + ABS(ys)/2;
	switch (vec->type) {
	case 30 :
		moveto(xo+xs*vec->k1, yo+ys*vec->l1);
		lineto(xo+xs*vec->k2, yo+ys*vec->l2);
		break;
	case 31 :
/**/
		fvec = (struct fvector *) vec;
		moveto(xo+(int)(xs*fvec->k1), yo+(int)(ys*fvec->l1));
		lineto(xo+(int)(xs*fvec->k2), yo+(int)(ys*fvec->l2));
/**/
		break;
	default:
		break;
	}
}


/*
 * display a point (in future, in a variety of styles; just a cross for now)
 */
picpoint(point,f)
struct ipoint *point;
struct pframe *f;
{
	register struct fpoint *fpoint;
	register i,xo,yo,xs,ys;
	xs = f->ix*f->scale;
	ys = f->iy*f->scale;
	xo = f->dx - xs*f->ox + ABS(xs)/2;	/* to line up centrally with pixels */
	yo = f->dy - ys*f->oy + ABS(ys)/2;
	switch (point->type) {
	case 40 :
		moveto(xo+xs*point->k, yo+ys*point->l);
		piccursor(f->scale,point->style);
		break;
	case 41 :
/**/
		fpoint = (struct fpoint *) point;
		moveto(xo+(int)(xs*fpoint->k), xo+(int)(xs*fpoint->l));
		piccursor(f->scale,fpoint->style);
/**/
		break;
	default:
		break;
	}
}

piccursor(scale,style)
{
	switch (style) {
	default :
	case 0:				/* constant sized cross */
		scale = 16;
	case 1:				/* scaled cross */
		moveby(-scale/2,-scale/2);
		lineby(scale,scale);
		moveby(0,-scale);
		lineby(-scale,scale);
		return(0);
	case 2:				/* constant sized blob */
		moveby(-8,-8);
		lineby(16,0);
		moveby(0,8);
		lineby(-16,0);
		moveby(0,8);
		lineby(16,0);
		return(0);
	case 4:				/* constant sized hollow square */
		scale = 32;
		lwidth(2);
	case 5:				/* scaled hollow square */
		moveby(-scale,-scale);
		scale *= 2;
		lineby(scale,0);
		lineby(0,scale);
		lineby(-scale,0);
		lineby(0,-scale);
		if (style == 4)
			lwidth(1);
		return(0);
	case 6:				/* tilted arrow head */
		arrowhead(32);		/* in DDGS */
		return(0);
	}
}


picrect(rdom,f)
struct irect *rdom;
struct pframe *f;
{
	register i, j;
	COORD *k, *l;
	float *fk, *fl;
	struct frect *fdom;
	register xo,yo,xs,ys;
	int xdum, ydum;
	xs = f->ix*f->scale;
	ys = f->iy*f->scale;
	xo = f->dx - xs*f->ox + ABS(xs)/2;	/* to line up centrally with pixels */
	yo = f->dy - ys*f->oy + ABS(ys)/2;
	switch(rdom->type) {
	case 1 :
		k = rdom->irk;
		l = rdom->irl;
		moveto(xo+xs*k[0], yo+ys*l[0]);
		for (i=1; i<=4; i++) {
			j = i==4? 0: i;
			lineto(xo+xs*k[j], yo+ys*l[j]);
		}
		break;
	case 2 :
		fdom = (struct frect *) rdom;
		fk = fdom->frk;
		fl = fdom->frl;
		xdum = xs*fk[0];
		ydum = ys*fl[0];
		moveto(xo+xdum, yo+ydum);
		for (i=1; i<=4; i++) {
			j = i==4? 0: i;
			xdum = xs*fk[j];
			ydum = ys*fl[j];
			lineto(xo+xdum, yo+ydum);
		}
		break;
	}
}



pichisto(h,f)
struct histogramdomain *h;
struct pframe *f;
{
	int *iv;
	float *fv;
	register i, k, l;
	register xs,ys;
	xs = f->ix*f->scale;
	ys = f->iy*f->scale;
	k = f->dx - xs*(f->ox - h->k) + xs/2;
	l = f->dy - ys*(f->oy - h->l) + ys/2;
	moveto(k, l);
	switch (h->type) {
	default:
	case 1:
		iv = h->hv;
		switch (h->r) {
		default:
		case 0:
			for (i=0; i<h->npoints; i++) {
				lineto(k+xs* (*iv), l);
				l += ys;
				iv++;
			}
			lineto(k, l-ys);
			return(0);
		case 1:
			for (i=0; i<h->npoints; i++) {
				lineto(k, l+ys* (*iv));
				k += xs;
				iv++;
			}
			lineto(k-xs, l);
			return(0);
		}
	case 2:
/**/
		fv = (float *) h->hv;
		switch (h->r) {
		default:
		case 0:
			for (i=0; i<h->npoints; i++) {
				lineto(k+ (int)(xs* *fv), l);
				l += ys;
				fv++;
			}
			lineto(k, l-ys);
			return(0);
		case 1:
			for (i=0; i<h->npoints; i++) {
				lineto(k, l+ (int)(ys* *fv));
				k += xs;
				fv++;
			}
			lineto(k-xs, l);
			return(0);
		}
/**/
		break;
	}
}


/*
 * text object
 * a quick and dirty fix here for text with new-line characters
 */
pictext(tobj,f)
struct text *tobj;
struct pframe *f;
{
	int xo,yo,xs,ys;
	register char *c;
	char *d;
	xs = f->ix*f->scale;
	ys = f->iy*f->scale;
	xo = f->dx + xs*(tobj->tdom->k - f->ox);
	yo = f->dy + ys*(tobj->tdom->l - f->oy);
	c = d = tobj->text;
	while (*c != '\0') {
		if (*c == '\n') {
			*c = '\0';
			dispstring(d,xo,yo);
			yo -= 100;
			*c = '\n';
			d = c+1;
		}
		c++;
	}
	if (c != d)
		dispstring(d,xo,yo);
}
