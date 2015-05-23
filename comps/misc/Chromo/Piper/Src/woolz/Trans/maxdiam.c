/*
 * maxdiam.c
 *
 * maximum diameter of an object.
 * For now, use "city block" metric on type 1 objects only
 *
 * Modifications
 *
 *	18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	08 May 1987		BDP		protection against null or empty objs
 *	13 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>
/*
 * protection against NULL and empty objects added. Will return zero
 * if any probelms are found. Another change has been added to check
 * for empty, currently with an empty object a line count of zero will
 * be compared with an undefined column count, and the larger value 
 * returned. The routine will return 0 if line count is zero.
 */
maxdiam(obj)
struct object *obj;
{
	register struct intervaldomain *idom;
	register int k,l,diam;

	diam = 0;
	if (woolz_check_obj(obj, "maxdiam") == 0)
	{
		switch (obj->type) {
		case 1:
			idom = obj->idom;
			k = idom->lastkl - idom->kol1 + 1;
			l = idom->lastln - idom->line1 + 1;
			if (l > 0)
				if (l > k)
				 	diam = l;
				 else
				 	diam = k;
			break;
		default:
			fprintf(stderr,"MAXDIAM no support for type %d\n",(int) obj->type);
		}
	}
	return(diam);
}
