/*
 * polytoobj.c
 * -----------
 * Ji Liang Mar 5, 1987
 *
 * change an arbitrary type 1 polygon curve to a type 1 object
 *
 * MODIFICATIONS
 * -------------
 *	01-10-87	SCG			Return NULL object if polygon is empty ie no vertices
 *	25-09-87	jimp@IRS	Countpoly returns at least one.
 *	06-08-87	jimp	Bug-fix (free-ing space)
 *	31-07-87	jimp	Bug-fix.
 *	13-03-87	jimp	Added comments, compiled in OS9 system, changed
 *				malloc to Malloc, etc..
 *	16-03-87	jimp	Took out original sort code, added "qsort" call.
 *				Substantial re-write to cope with possibility of
 *				looping input polygon, such as is common from
 *				manual chromosome split procedures.
 */

#include <stdio.h>
#include <wstruct.h>

struct line_int {
	COORD l;
	COORD k1;
	COORD k2;
};

#define MAX(a,b)	a>b?a:b
#define MIN(a,b)	a<b?a:b
#define abs(a)		a>=0?a:-(a)

struct object *polytoobj(plgd)
struct polygondomain *plgd;
{
	struct object *obj, *makemain();
	struct intervaldomain *idmn, *makedomain();
	register struct interval *jp;
	struct interval *jpw;
	register struct ivertex *itv;
	register struct line_int *lp;
	struct line_int *lpb;
	register int i, n, tmp, incr;
	int line, l1, ll, k1, kl;
	int nints, xdiff, line_int_order();

	/*
	 *	rubber banding can create polygon with zero vertices
	 *  look for this situation and protect
	 */
	if((n = countpoly(plgd)) < 1) {
		return(NULL);
	}
	lpb = lp = (struct line_int *) Malloc(sizeof(struct line_int) * n);
	/*
	 * An interval-representation polygon of the input polygon is
	 * is constructed in lp, with a pair of vertices for each interval.
	 */
	itv = plgd->vtx;
	l1 = ll = line = lp->l = itv->vtY;
	k1 = kl = lp->k1 = lp->k2  = itv->vtX;
	lp++;
	itv++;
	n = plgd->nvertices - 1;
	for( ; n > 0; n--,itv++){
		/*
		 * establish line and column bounds
		 */
		l1 = MIN(itv->vtY, l1);
		ll = MAX(itv->vtY, ll);
		k1 = MIN(itv->vtX, k1);
		kl = MAX(itv->vtX, kl);
		tmp = abs(itv->vtY - line);	
		if (tmp > 1){
			if(itv->vtY > line)
				incr = 1;
			else
				incr = -1;
			xdiff = itv->vtX - (itv-1)->vtX;
			for(i = 1; i < tmp; i++,lp++){
				line += incr;
				lp->l = line;
				lp->k1 = (lp-1)->k2;
				lp->k2 = i*xdiff/tmp + (itv-1)->vtX;
			}
		}
		if (tmp != 0){
			lp->l = itv->vtY;
			lp->k1 = (lp - 1)->k2;
			lp->k2 = itv->vtX;
			lp++;
		}
		else {
			(lp - 1)->k2 = itv->vtX;
		}
		line = itv->vtY;
	}
	n = lp-lpb; 
	/*
	 * order end-points of individual intervals correctly
	 * (they may well be wrong-way-round).
	 */
	lp = lpb;
	for (i=0; i<n; i++,lp++)
		if (lp->k1 > lp->k2) {
			tmp = lp->k1;
			lp->k1 = lp->k2;
			lp->k2 = tmp;
		}
	/*
	 * Sort intervals into order.  Use "qsort", it's much quicker !
	 */
	qsort(lpb,n,sizeof(struct line_int),line_int_order);
	/*
	 * change line_interval representation to true type 1 object
	 */
	jp = jpw = (struct interval *) Malloc(sizeof(struct interval) * n);
	idmn = makedomain(1, l1, ll, k1, kl);
	idmn->freeptr = (char *) jpw;
	obj = makemain(1, idmn, 0, 0, 0);
	line = l1;
	nints = 0;
	lp = lpb;
	for ( ; n > 0; n--,jp++,lp++) {
		jp->ileft = lp->k1 - k1;
		jp->iright = lp->k2;
		/*
		 * check here for possibility of overlapping intervals.
		 * beware of case where second (or subsequent) interval
		 * has right end point less than current right end.
		 */
		while (n>1 && (lp+1)->l==lp->l && jp->iright>=(lp+1)->k1-1) {
			n--;
			lp++;
			if (lp->k2 > jp->iright)
				jp->iright = lp->k2;
		}
		jp->iright -= k1;
		if (lp->l == line)
			nints++;
		else {
			makeinterval(line,idmn,nints,jpw);
			line++;	/* guaranteed OK by construction of lp[] */
			nints = 1;
			jpw=jp;
		}
	}
	makeinterval(line,idmn,nints,jpw);
	Free(lpb);
	return(obj);
}

/*
 * find the number of intervals likely from the input polygon as
 * the total Y excursion plus the number of points
 */
countpoly(plgd)
struct polygondomain *plgd;
{
	register struct ivertex *ivt = plgd->vtx;
	register int n = plgd->nvertices;
	register int yd, s;

	s = n;
	for(; n > 1; n--){
		yd = ivt->vtY;
		ivt++;
		yd -= ivt->vtY;
		s += abs(yd);
	}
	return(s);
}


/*
 * Order line_int's.  Must take account of possibility
 * of overlapping intervals with same left end point
 */
line_int_order(lp1,lp2)
register struct line_int *lp1,*lp2;
{
	register int xdiff, ydiff;
	if ((ydiff = lp1->l - lp2->l) != 0)
		return(ydiff);
	else if ((xdiff = lp1->k1 - lp2->k1) != 0)
		return(xdiff);
	else
		return(lp1->k2 - lp2->k2);
}
