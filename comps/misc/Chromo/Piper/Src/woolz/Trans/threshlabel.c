/*
 * threshlabel.c
 *
 * threshold and segment an input object
 *
 * Modifications
 *
 *	11 May 1987		BDP		protection against null or empty objs
 *	 9 Oct 1986		CAS		Changed freidom + Free to freeobj
 *	24 Sep 1986		GJP		Big Free not little free
 *	13 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>

/*
 * No specific protection necessary for this routine, protection provided by
 * threshold and label. only this comment added.  bdp 11/5/87
 */
threshlabel (obj, thresh, mm, objlist, nobj, ignlns)
struct object *obj;
struct object **objlist;
int *mm;
{
	struct object *tobj, *threshold();
	int stat;
	
	stat = 0;
	if ((tobj = threshold(obj,thresh)) == NULL)
		stat = 1;
	else
	{
		label(tobj,mm,objlist,nobj,ignlns);
		if (mm == 0)
			stat = 1;
		freeobj(tobj);
	}
	
	return(stat);
}
