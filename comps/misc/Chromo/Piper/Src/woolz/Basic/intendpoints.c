/*
 *   intendpoints.c
 *
 * Modifications
 *
 *  18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	05 May 1987		BDP		protection against null or empty objs
 *	12 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>

/*
 * return the next interval endpoint in iwsp.colpos
 * if first interval endpoint in interval then iwsp.colrmn is non-zero,
 * otherwise zero
 */
nextintervalendpoint(iwsp)
register struct iwspace *iwsp;
{
	if (iwsp->colrmn == 0)
		return(nextinterval(iwsp));
	else {
		iwsp->colrmn = 0;
		iwsp->colpos = iwsp->rgtpos;
		return(0);
	}
}


/*
 * return pointer to the first interval in a specified line
 * Protection against Null objects added here, will return NULL if an error
 * is found. It should be noted that protection elsewhere should prevent this
 * from ever happening, but if it does the effect needs to be handled by the 
 * calling routine because NULL intervals are not supported in even the 
 * revised woolz . bdp 5/5/87 */

struct interval *
firstintptr(obj,line)
struct object *obj;
{
	register struct intervaldomain *jdp ;
	
	if (woolz_check_obj(obj, "firstintptr") == 0) {
		jdp = obj->idom;	/* only initialise after the checks */
		switch (jdp->type) {
		case 1:			/* conventional domain */
			return(jdp->intvlines[line-jdp->line1].intvs);
		case 2:			/* rectangular object - return
					   pointer to column bounds */
			return((struct interval *) &(jdp->kol1));
		default:
			break;
		}
	}

	return(NULL);		/* fairly nasty error return this, NULL intervals
						are not generally supported by the system */
						
}


/*
 * return pointer to the last interval in a specified line
 * protection here is as above with same constraints. bdp 5/5/87  */

struct interval *
lastintptr(obj,line)
struct object *obj;
{
	register struct intervaldomain *jdp;
	struct intervalline *ivln;
	
	if (woolz_check_obj(obj, "lastintptr") == 0) {
		jdp = obj->idom;	/* only initialise after object checked */
		switch (jdp->type) {
		case 1:			/* conventional domain */
			ivln = jdp->intvlines + line - jdp->line1;
			return(ivln->intvs + ivln->nintvs -1);
		case 2:			/* rectangular object - return
					   pointer to column bounds */
			return((struct interval *) &(jdp->kol1));
		default:
			break;
		}
	}
	return(NULL);
}
