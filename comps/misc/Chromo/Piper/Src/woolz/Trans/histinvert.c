/*
 * histinvert.c	Jim Piper
 *
 * Modifications
 *
 *	22 Nov 1988		dcb		Added param. to woolz_exit to say where called from
 *	18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	07 May 1987		BDP		protection against null or empty objs
 *	2  Mar 1987		GJP		Woolz_exit
 *	13 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>


/*
 * invert a histogram end-to-end
 * Protection added. A null object supplied will cause a null to be returned.
 * The routine is protected against an empty domain. bdp 7/5/87 
 */
histinvert(h)
struct object *h;
{
	register int *hb,*ht,t;
	struct histogramdomain *hdom;

	if (woolz_check_obj(h, "histinvert") != 0)
		return(1);
	if (h->type != 13) {
		fprintf(stderr,"histinvert: wrong type\n");
		woolz_exit(91, "histinvert");
	}
	hdom = (struct histogramdomain *)h->idom;
	hb = hdom->hv;
	ht = hb + hdom->npoints - 1;
	while (hb < ht) {
		t = *ht;
		*ht = *hb;
		*hb = t;
		hb++;
		ht--;
	}
	return(0);
}
