/*
 * ncentromere.c	Jim Piper	23/1/84
 *
 * Find centromere from mid-points polygon and associated profile.
 * Return as a point structure.  Return NULL if centromere not found.
 *
 * Mid-points polygon may either be float-valued, or integer-valued
 * scaled up by factor of 8.  Note however that most related routines
 * only	permit the scaled *8 format.
 *
 * Modifications
 *
 *	11 Sep 1986		CAS		Includes
 */
#include <stdio.h>
#include <wstruct.h>

struct ipoint *ncentromere(mpol, prof)
struct object *mpol, *prof;
{
	register struct polygondomain *pdom;
	register int pt;
	register struct ipoint *point;
	
	/*
	 * locate centromere in mirrored profile with
	 * quantum parameter set to be 10% of peak value.
	 */
	pt = cmerelocation(prof,10,1);
	if (pt < 0)
		return(NULL);
	else {
		point = (struct ipoint *)Malloc(sizeof(struct ipoint));
		point->type = 40;
		pdom = (struct polygondomain *) mpol->idom;
		switch (pdom->type) {
		default:
		case 1:
			point->k = (pdom->vtx[pt].vtX + 4) >> 3;
			point->l = (pdom->vtx[pt].vtY + 4) >> 3;
			break;
		case 2:
			point->k = (int) (((struct fvertex *)(pdom->vtx))[pt].vtX + 0.5);
			point->l = (int) (((struct fvertex *)(pdom->vtx))[pt].vtY + 0.5);
			break;
		}
		return(point);
	}
}
