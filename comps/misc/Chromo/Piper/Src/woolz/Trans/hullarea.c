/*
 * hullarea.c	Jim Piper	January 1984
 *
 * Compute area and perimeter of convex hull of type 1 object.
 *
 * Modifications
 *
 *	18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	07 May 1987		BDP		protection against null or empty objs
 *	13 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <math.h>
#include <wstruct.h>



/*
 * the area of an n-gon (x(i),y(i)), i=1,n can be expressed as the sum
 * (x(i)-x(i-1))*(y(i)+y(i-1))/2, i=1,n,  where x(0) is x(n), y(0) is y(n).
 *
 * we need to compensate as usual for the fact that woolz objects are
 * collections of points rather than plane polygons, and
 * a reasonable correction for the convex hull is to add half the perimeter
 *
 * Protection against null object added. An area of zero will be returned
 * immediately if an error found. An error of zero will also be returned if
 * vdom in the object doesnt exist.  bdp 7/5/87
 */
hullarea (cvh, perim)
struct object *cvh;
register int *perim;
{
	struct polygondomain *pdom;
	register struct ivertex *vtx;
	struct ivertex *wtx;
	struct cvhdom *cdom;
	register struct chord *ch;
	register int hull, i, mdlin2;

	if (woolz_check_obj(cvh, "hullarea") != 0)
		return(0);	/* return zero size, not status indicator */
	*perim = hull = 0;
	pdom = (struct polygondomain *) cvh->idom;
	cdom = (struct cvhdom *) cvh->vdom;
	if (cdom != NULL)
	{
		mdlin2 = 2*cdom->mdlin;
		vtx = pdom->vtx;
		wtx = vtx+1;
		ch = cdom->ch;
		for (i=1; i<pdom->nvertices; i++) {
			hull += (vtx->vtY + wtx->vtY - mdlin2) * ch->bcon;
			*perim += ch->cl;
			vtx++;
			wtx++;
			ch++;
		}
		if ((hull/=2) < 0)
			hull= -hull;
		hull += *perim/16; /* perim is *8 since ch->cl is *8 */
	}
	return (hull);
}
