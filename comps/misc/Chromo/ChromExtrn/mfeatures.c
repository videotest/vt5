
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
#define COORD _COORD
#include <wstruct.h>
#include <chromanal.h>
#undef COORD
#undef BLOB
#include <windows.h>
#include "DebugObj.h"

#define PROFSMOOTH 1

#pragma warning(disable:4013)

/*
 * select which profiles should be used, one for band parameters,
 * one for centromere location.  We already know that profile "0" (density)
 * is better than profile "-1" (mean density)  or profile "-2" (max
 * density) for wdd features.
 */
static int whichprof[2] = {0,2};


#if defined(_DEBUG)
struct chromosome *_chromfeatures(obj,basic,pmpol,skel,debug)
struct debugobj *debug;
#else
struct chromosome *_chromfeatures(obj,basic,pmpol,skel)
#endif
struct chromosome *obj;
struct object **pmpol;
int *skel;
{
	struct chromosome *robj, *vertical();
	struct object *mpol, *_chromaxis2(), *prof[2];
	struct object *cvh, *convhull();
	struct histogramdomain *h;

	if (basic != 0) {
		_basicfeatures(obj);
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
#if defined(_DEBUG)
	mpol = _chromaxis2(robj,skel,debug);
#else
	mpol = _chromaxis2(robj,skel);
#endif
	/*
	 * find density and moment profiles, and shorten mid-points polygon
	 * and profile if tips lie outside object.  Then chromosome length
	 * is resulting profile length.
	 */
	multiprofiles(robj,mpol,prof,whichprof,2);
#if defined(_DEBUG)
{
struct histogramdomain *hdom;
hdom = (struct histogramdomain *)prof[0]->idom;
debug->prof0 = malloc(sizeof(struct histogramdomain)+hdom->npoints*sizeof(int));
*debug->prof0 = *hdom;
debug->prof0->hv = (int *)(debug->prof0+1);
memcpy(debug->prof0->hv,hdom->hv,hdom->npoints*sizeof(int));
hdom = (struct histogramdomain *)prof[1]->idom;
debug->prof1 = malloc(sizeof(struct histogramdomain)+hdom->npoints*sizeof(int));
*debug->prof1 = *hdom;
debug->prof1->hv = (int *)(debug->prof1+1);
memcpy(debug->prof1->hv,hdom->hv,hdom->npoints*sizeof(int));
}
#endif
/*{
int pt;
struct histogramdomain *hdom;
hdom = (struct histogramdomain *)prof[0]->idom;
for (pt = 0; pt < hdom->npoints; pt++)
{
	char szBuff[250];
	sprintf(szBuff, "%d : %d\n", pt, (int)hdom->hv[pt]);
	OutputDebugString(szBuff);
}
OutputDebugString("\n");
}*/
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
#if defined(_DEBUG)
{
struct histogramdomain *hdom;
hdom = (struct histogramdomain *)prof[0]->idom;
debug->sprof0 = malloc(sizeof(struct histogramdomain)+hdom->npoints*sizeof(int));
*debug->sprof0 = *hdom;
debug->sprof0->hv = (int *)(debug->sprof0+1);
memcpy(debug->sprof0->hv,hdom->hv,hdom->npoints*sizeof(int));
hdom = (struct histogramdomain *)prof[1]->idom;
debug->sprof1 = malloc(sizeof(struct histogramdomain)+hdom->npoints*sizeof(int));
*debug->sprof1 = *hdom;
debug->sprof1->hv = (int *)(debug->sprof1+1);
memcpy(debug->sprof1->hv,hdom->hv,hdom->npoints*sizeof(int));
}
#endif
	/*
	 * find centromere, measure features
	 */
	centromerefeatures(robj,mpol,prof[1]);
	/*
	 * profile density distribution features
	 */
	wddfeatures(robj,prof[0],prof[1]);
//	freeobj(mpol);
	*pmpol = mpol;
	freeobj(prof[0]);
	freeobj(prof[1]);
	return (robj);
}


_basicfeatures (obj)
struct chromosome *obj;
{
	struct chromplist *plist;
	plist = obj->plist;
	plist->area = area(obj);
	plist->mass = mass(obj);
	return 0;
}
