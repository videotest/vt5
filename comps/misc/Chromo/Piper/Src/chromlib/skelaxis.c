/*
 * skelaxis.c		Jim Piper		16-05-96
 *
 * compute chromosome medial axis from skeleton
 *
 * Modifications
 *
 *	11 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>

struct object *skelaxis(obj)
struct object *obj;
{
	register struct object *mpol, *skobj;
	struct object *skeleton();
	struct object *apolyfromskel();
	struct valuetable *newvaluetb();
	struct polygondomain *pdom;
	int x1,y1,numbranches;

	/*
	 * skeletonisation
	 */
	skobj = skeleton(obj,6);
	skobj->vdom = newvaluetb(skobj,1,0);
	setval(skobj,1);
	numbranches=afindtipandbranch(skobj,&x1,&y1);
	if (numbranches == 0) {
		mpol = apolyfromskel(skobj,x1,y1);
		pdom = (struct polygondomain *)mpol->idom;
		if (pdom->nvertices <= 3) {
			freeobj(mpol);
			mpol = NULL;
		} 
	} else
		mpol = NULL;
	freeobj(skobj);
	return(mpol);
}
