/*
 * standardidom.c		Jim Piper		14/11/84
 *
 * standardise the interval list (remove leading and trailing
 * empty lines, set kol1 so that at least one interval commences
 * at zero, set lastkl correctly)
 *
 * Modifications
 *
 *	06 May 1987		BDP		protection against null or empty objs
 *	12 Sep 1986		CAS		Includes
 */
#include <stdio.h>
#include <wstruct.h>

/* protection in this function needs to be considered from two points. first
the function has to be protected against a null domain being suplied, or 
an empty doamin. in either case it will return having performed no action.
secondly , being a routine capable of generating an empty routine, it is 
imperative that it does this in a way understood by other woolz routines. 
for this reason standardidom has been changed to ensure that if the object is
now empty, it will be returned with lastln=line1 - 1.  bdp 6/5/87
*/
standardidom(idom)
register struct intervaldomain *idom;
{
	register struct intervalline *itvl;
	register struct interval *intv;
	register i,j,k,r;

	if (idom == NULL)
		return(1);		/* indicate nothing done to anyone interested */
	if (idom->line1 > idom->lastln)		/* then its empty so return */
		return(1);

	/* find first non-empty line */
	while (idom->intvlines->nintvs == 0) {
		idom->line1++;
		idom->intvlines++;
		if (idom->line1 > idom->lastln)		/* changed from == to > 6/5/87 */
			return(1);
	}

	/* find last non-empty line */
	while (idom->intvlines[idom->lastln-idom->line1].nintvs == 0) {
		idom->lastln--;
		if (idom->line1 == idom->lastln)	/* must be at least one here */
			break;
	}

	/* find left-most end point of non-empty lines */
	itvl = idom->intvlines;
	k = itvl->intvs->ileft;	/* first line IS now non-empty */
	for (i=idom->line1; i<=idom->lastln; i++) {
		intv = itvl->intvs;
		for (j=0; j<itvl->nintvs; j++) {
			if (intv->ileft < k)
				k = intv->ileft;
			intv++;
		}
		itvl++;
	}

	/* update interval domain and interval end points, find right-most */
	idom->kol1 += k;
	r = 0;
	itvl = idom->intvlines;
	for (i=idom->line1; i<=idom->lastln; i++) {
		intv = itvl->intvs;
		for (j=0; j<itvl->nintvs; j++) {
			intv->ileft -= k;
			intv->iright -= k;
			if (intv->iright > r)
				r = intv->iright;
			intv++;
		}
		itvl++;
	}
	idom->lastkl = idom->kol1 + r;
	return(0);
}
