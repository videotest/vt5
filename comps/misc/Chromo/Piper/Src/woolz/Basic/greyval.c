/*
 *   greyval.c
 *   random access to pointer to grey value in an interval domain object
 *
 * Modifications
 *
 *  18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	05 Nov 1987		BDP		Return pointer to zero on error, not zero
 *	05 May 1987		BDP		protection against null or empty objs
 *	12 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>

#define rvdmn ((struct rectvaltb *)vdmn)

GREY *
greyval(obj,l,k)
struct object *obj;
register int l,k;
{
	register ki, n;
	register struct valueline *vlln;
	register struct intervaldomain *idmn = obj->idom;
	register struct interval *intl;
	register struct valuetable *vdmn = obj->vdom;
	register struct intervalline *itvl;
	struct valintline *val;
	static GREY background;

	/* protection added to check that an object exists and contains both an
	interval domain , and a value domain before it is used. in the checks
	below, the object check will for a type one object also check for the 
	existence of an interval domain. If any of these checks fail,  then zero
	will be returned. if the in built range checks fail, then a background
	value is returned. Despite the temptation to rewrite this module to 
	remove all the gotos, only the initialisation of domain pointers has
	changed, this now happens after the checks not at declaration. bdp 5/5/87 */
	
	background = 0;
	
	if ((woolz_check_obj(obj, "greyval") != 0) || ( obj->vdom == NULL ))
		return(&background);
	
	idmn = obj->idom;
	vdmn = obj->vdom;
	
	if (l < idmn->line1 || l > idmn->lastln)
		goto outside;

	switch (idmn->type) {

	case 1:			/* interval domain */
		itvl = &idmn->intvlines[l - idmn->line1];
		n = itvl->nintvs;
		intl = itvl->intvs;
		ki = k - idmn->kol1;
		while (n-- > 0) {
			if (ki < intl->ileft)
				goto outside;		/* from while statement */
			if (ki <= intl->iright)
				goto inside;
			intl++;
		}
		goto outside;

	case 2:			/* rectangular domain */
		if (k < idmn->kol1 || k > idmn->lastkl)
			goto outside;
		else
			goto inside;

	default:
		goto outside;

	}

outside:
	background = vdmn->bckgrnd;
	return(&background);
inside:
	switch (vdmn->type){
		case 1:
			vlln = vdmn->vtblines[l - vdmn->line1];
			return(vlln->values + k - vlln->vkol1);
		case 11:
			return (rvdmn->values + k - rvdmn->kol1 + rvdmn->width*(l- rvdmn->line1));
		case 21:
			val = ((struct ivtable *)vdmn)->vil +
					l - ((struct ivtable *)vdmn)->line1;
			n = val->nintvs;
			vlln = val->vtbint;
			while (n-- > 0) {
				if (k >= vlln->vkol1 && k <= vlln->vlastkl)
					return(vlln->values + k - vlln->vkol1);
				vlln++;
			}
		default:
			break;
	}
}
