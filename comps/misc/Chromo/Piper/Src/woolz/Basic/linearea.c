/*
 * linearea.c	Jim Piper	July 1983
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
 * return the line-area of an object
 * defined as the sum of the line segments bounded by the left hand
 * end of the first interval in a line and the right hand end of the
 * last interval in that line.
 * Unlike its counterpart area.c, this routine requires protection against
 * NULL objects as it makes direct access to them rather than just via the 
 * interval scanning routines. A size of zero will be returned for any 
 * problem found . The routine will also cope with empty objects and return
 * a size of zero . bdp 5/5/87  */

linearea(obj)
struct object * obj;
{
	register int size,l,r;
	struct intervaldomain *idom;
	struct iwspace iwsp;

	size = 0;
	if ((obj != NULL) && (woolz_check_idom(obj->idom, "linearea") == 0)) {
		idom = obj->idom;
		switch (idom->type) {
		case 1:
			initrasterscan(obj,&iwsp,0);
			while (nextinterval(&iwsp) == 0) {
				l = iwsp.lftpos;
				while (iwsp.intrmn != 0)
					nextinterval(&iwsp);
				r = iwsp.rgtpos;
				size += (r -l +1);
			}
			break;
		case 2:
			size = (idom->lastln - idom->line1 + 1) * (idom->lastkl - idom->kol1 + 1);
		}
	}
	return(size);
}	
