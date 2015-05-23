/*
 * h256norm.c	Jim Piper
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
 * Protection against NULL and empty objects added. Null object supplied will
 * cause immediate return. An error indicator is now returned which calling
 * routine may check if desired. For an empty domain immediate return is also
 * effected though generally the routine would cope with this. bdp 7/5/87
 */
histo256normalise(obj)
struct object *obj;
{
	register struct histogramdomain *hdom;
	register int i, maxh, minh, *h;

	if (woolz_check_obj(obj, "histo256normalise") != 0)
		return(1);
	
	hdom = (struct histogramdomain *) obj->idom;
	if (hdom->npoints <= 0)
		return(1);
		
	h = hdom->hv;
	for (i=0; i<hdom->npoints; i++) {
		if (i == 0)
			minh = maxh = *h;
		else if (*h > maxh)
			maxh = *h;
		else if (*h < minh)
			minh = *h;
		h++;
	}
	maxh -= (minh - 1);
	h = hdom->hv;
	for (i=0; i<hdom->npoints; i++) {
		*h = (*h - minh) * 255 / maxh;
		h++;
	}
	return(0);
}
