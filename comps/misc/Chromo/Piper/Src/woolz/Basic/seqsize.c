/*
 * seqsize.c
 *
 * compute the number of bytes required to hold
 * object "obj" in byte-sequential form.
 *
 * Modifications
 *
 *  18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	05 Nov 1987		BDP		Fixed a bug when handling empty vdom.
 *	06 May 1987		BDP		protection against null or empty objs
 *	12 Dec 1986		CAS		Also do type 110 objects
 *	12 Sep 1986		CAS		Includes
 */
#include <stdio.h>
#include <wstruct.h>

/*
 *	S E Q S I Z E  --
 *
 * Protection added here to check supplied object exists, if not then
 * size of 0 is returned. this protects the switch statement.
 * routine should handle empty objects successfully. bdp 6/5/87.
 */
seqsize(obj)
struct object *obj;
{
	register int s, nl;

	s = 1;
	if (woolz_check_obj(obj, "seqsize") == 0) {
		switch(obj->type) {
			case 1:
				s += (1 + 4*sizeof(COORD));
				if (obj->idom->type == 1) {
					nl = obj->idom->lastln - obj->idom->line1 + 1;
					s += nl*sizeof(SMALL);
					s += intcount(obj->idom)*sizeof(struct interval);
				}
/*
 *	for a null vdom only a single zero will be written, so increment
 *	s once before test, then again after if there will be a grey and
 *	a packing byte written. bdp 5/11/87	
 */
				s ++;						/* grey table type */
				if (obj->vdom != NULL) {
					s++;					/* packing */
					s += sizeof(GREY);		/* background */
					s += area(obj);			/* assume byte packing */
				}
				s++;						/* plist flag */
				if (obj->plist != NULL)
					s += obj->plist->size;
				break;
		
			case 110:						/* File header */
				if (obj->plist)
					s += 1 + obj->plist->size;
			default:
				break;
		}
	}
	else
		s = 0;	/* error detected */
	return(s);
}

