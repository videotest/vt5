
/*
 * chromaxis.c		Jim Piper		20-05-86
 *
 *		Modifications
 *
 *	 7 Feb 1991		CAS		remove Mizar defs -> osk
 *	24 Sep 1986		GJP		change free to Free
 */

#include <stdio.h>
#include <math.h>
#include <wstruct.h>
#include <chromanal.h>

/*
 * Find chromosome axis of vertical chromosome.
 * First find "poor man's skeleton".  If this is both long and bent,
 * find axis from skeleton if skeleton is unbranched.
 */
struct object *chromosomeaxis(obj)
struct object *obj;
{
	register struct object *mpol, *skobj;
	struct object *midptspoly(), *skeleton();
	struct object *polyfromskel();
	struct valuetable *newvaluetb();
	struct intervaldomain *intrp8(),*uspaxis8();
	struct polygondomain *pdom;
	int numbranches, i, xr, yr;

	/*
	 * find mid points polygon, smooth
	 */
	mpol = midptspoly(obj);
	removeblips(mpol);
	modifytips(mpol);
	xr = xrange(mpol);
	yr = ((struct polygondomain *)(mpol->idom))->nvertices;
	if (yr > 22 && xr > 1) {
		/*
		 * skeletonisation
		 */
		skobj = skeleton(obj,6);
		skobj->vdom = newvaluetb(skobj,1,0);
		setval(skobj,1);
		numbranches=findtipandbranch(skobj);
		if (numbranches == 0) {
			freeobj(mpol);
			mpol = polyfromskel(skobj);
		}
		freeobj(skobj);
	}
/* PREVIOUS CODE REPLACED
	mpol = (struct object *) realsmoothunitpoly(mpol,AXISSMOOTH);
*/
	pdom = (struct polygondomain *)mpol->idom;
#ifdef OSK
	axissmooth(pdom,2);
#else OSK
	axissmooth(pdom,8);
#endif OSK
	mpol->idom = uspaxis8(pdom);
	Free(pdom);
/*
	pdom = (struct polygondomain *)mpol->idom;
	mpol->idom = intrp8(pdom);
	Free(pdom);
*/
	return(mpol);
}
