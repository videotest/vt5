/*
 * probotype.c		Jim Piper		17-10-86
 *
 *
 * Use simple tests to decide probable type of object,
 * currently: NOISE, CHROMOSOME or COMPOSITE on basis of size
 *
 *	Modifications
 * 30 Oct 1989	dcb/GJP/CAS	Set plist->otype for CHROMOSOME as well as
 *							returning the value
 */
#include <stdio.h>
#include <wstruct.h>
#include <chromanal.h>

probotype(obj)
struct chromosome *obj;
{
	struct chromplist *plist = obj->plist;
	/*
	 * Otype already set with high confidence ??
	 * In which case, the type of guessing done
	 * here is NOT required !!
	 */
	if (plist->Cotype != 0 || plist->otconf == 100)
		return(0);
	/*
	 * Area known ?
	 */
	if (plist->area == 0)
		plist->area = area(obj);
	/*
	 * use area to guess type
	 */
	if (plist->area < 100) {
		plist->otype = NOISE;
		plist->otconf = 20;
		return(NOISE);
	}
	else if (plist->area > 2000) {
		plist->otype = COMPOSITE;
		plist->otconf = 20;
		return(COMPOSITE);
	}
	else {
		plist->otype = CHROMOSOME;
		plist->otconf = 20;
		return(CHROMOSOME);	/* the default */
	}
}
