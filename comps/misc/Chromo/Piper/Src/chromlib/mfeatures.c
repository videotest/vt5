
/*
 * mfeatures.c		7/3/84		Jim Piper
 *
 * compute features of chromosome objects
 *
 * Who		when		Significant modification
 * ---		----		------------------------
 *	18 Nov 1987	SCG	Do not allow higher feature measurement
 *				on one pixel wide objects.  This would
 *				produce a zero profile and subsequent zero
 *				division in wdd. (This is now corrected).
 *  JP		29/10/85	obj->plist no longer set to be NULL.
 *  JP		14/05/86	"fipconvhull" used for FIP digitised objects
 *  JP		20/05/86	split into smaller modules
 *	JP		07/07/86	test for small objects moved to AFTER rotation
 *	11 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>
#include <chromanal.h>
#define PROFSMOOTH 1

/*
 * select which profiles should be used, one for band parameters,
 * one for centromere location.  We already know that profile "0" (density)
 * is better than profile "-1" (mean density)  or profile "-2" (max
 * density) for wdd features.
 */
static int whichprof[2] = {0,2};


struct chromosome *chromfeatures(obj,basic)
struct chromosome *obj;
{
	struct chromosome *robj, *vertical();
	struct object *mpol, *chromaxis2(), *prof[2];
	struct object *cvh, *convhull();
	struct histogramdomain *h;

	if (basic != 0) {
		basicfeatures(obj);
		robj = vertical(obj);
		/*
		 * Small objects can cause trouble e.g. in axis finding.
		 * Don't do any further processing of anything that is
		 * too small here.  Other feature values will be zero,
		 * but since this cannot be a chromosome this does not matter.
		 */
		if ((robj->idom->lastln - robj->idom->line1) < 8 ||
				(robj->idom->lastkl - robj->idom->kol1) < 8)
			return(robj);
	} else {
		cvh = convhull(obj);
		obj->plist->hullarea = hullarea(cvh, &obj->plist->hullperim);
		freeobj(cvh);
		robj = obj;
	}
	mpol = chromaxis2(robj);
	/*
	 * find density and moment profiles, and shorten mid-points polygon
	 * and profile if tips lie outside object.  Then chromosome length
	 * is resulting profile length.
	 */
	multiprofiles(robj,mpol,prof,whichprof,2);
	shorten(mpol,prof[0],prof[1]);
	robj->plist->length = ((struct polygondomain *)(mpol->idom))->nvertices;
	/* set tip values to zero */
	h = (struct histogramdomain *)prof[1]->idom;
	h->hv[0] = h->hv[h->npoints-1] = 0;
	/*
	 * Smoothing possibly improves the power of wdd features,
	 * so perhaps we should smooth more strongly here.
	 */
	histosmooth(prof[0], PROFSMOOTH);
	histosmooth(prof[1], PROFSMOOTH+2);
	/*
	 * find centromere, measure features
	 */
	centromerefeatures(robj,mpol,prof[1]);
	/*
	 * profile density distribution features
	 */
	wddfeatures(robj,prof[0],prof[1]);
	freeobj(mpol);
	freeobj(prof[0]);
	freeobj(prof[1]);
	return (robj);
}


basicfeatures (obj)
struct chromosome *obj;
{
	struct chromplist *plist;
	plist = obj->plist;
	plist->area = area(obj);
	plist->mass = mass(obj);
}
