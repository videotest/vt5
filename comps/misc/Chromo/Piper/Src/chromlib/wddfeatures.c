/*
 * wddfeatures.c		Jim Piper		20-05-86
 *
 * Modifications
 *
 *	11 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>
#include <chromanal.h>

wddfeatures(obj,prof0,prof2)
struct chromosome *obj;
struct object *prof0, *prof2;
{
	int i;
	struct chromplist *plist = obj->plist;
	struct object *profg, *gradprof();
	profg = gradprof(prof0);
	wdd(prof0,plist->wdd);
	wdd(prof2,plist->mwdd);
	wdd(profg,plist->gwdd);
	freeobj(profg);
	plist->cvdd = cvdd(prof0);
	plist->nssd = nssd(prof0);
	/*
	 * get wdd features the right way round w.r.t. area centromeric index
	 */
	if (plist->cindexa > 50) {
		for (i=0; i<=4; i += 2) {
			plist->wdd[i] = - plist->wdd[i];
			plist->mwdd[i] = - plist->mwdd[i];
			plist->gwdd[i] = - plist->gwdd[i];
		}
	}
}
