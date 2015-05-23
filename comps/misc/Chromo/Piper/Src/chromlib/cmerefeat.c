
/*
 * cmerefeat.c		Jim Piper		20-05-86
 *		Modifications
 *
 *	24 Sep 1986		GJP		change free to Free
 */

#include <stdio.h>
#include <wstruct.h>
#include <chromanal.h>

centromerefeatures(obj,mpol,prof)
struct object *mpol, *prof;
struct chromosome *obj;
{
	struct ipoint *point, *ncentromere();
	struct chromplist *plist;
	struct ivector v;
	/*
	* find centromere from second moment profile
	*/
	point = ncentromere(mpol,prof);
	plist = obj->plist;
	if (point != NULL) {
		plist->cx = point->k;
		plist->cy = point->l;
		plist->cangle = 0;	/* since chromosome vertical */
		plist->cindexa = cindexa(obj,point->l);
		plist->cindexm = cindexm(obj,point->l);
		plist->cindexl = cindexl(mpol,point->l);
		plist->mdra = plist->cindexa > 50 ?
			100*(100-plist->cindexm)/(101-plist->cindexa):
			100*plist->cindexm/(1+plist->cindexa);
		Free(point);
	} else {
		plist->cindexa = -1;	/* failed flag */
	}
}
