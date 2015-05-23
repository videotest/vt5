/*
 * histonorm.c	Jim Piper
 *
 * Modifications
 *
 *	18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	07 May 1987		BDP		protection against null or empty objs
 *	13 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>

/*
 * Protection against input object added. will return an optional status if
 * any problem ancountered. The routine is secure against problems caused by
 * empty domains.  bdp 7/5/87
 */
histonormalise(obj)
struct object *obj;
{
	register struct histogramdomain *hdom;
	register int i, maxh, *h;

	if (woolz_check_obj(obj, "histonormalise") != 0 )
		return(1);
	hdom = (struct histogramdomain *) obj->idom;
	h = hdom->hv;
	maxh = 0;
	for (i=0; i<hdom->npoints; i++) {
		if (*h > maxh)
			maxh = *h;
		h++;
	}
	h = hdom->hv;
	for (i=0; i<hdom->npoints; i++) {
		*h = *h * hdom->npoints / maxh;
		h++;
	}
	return(0);
}
