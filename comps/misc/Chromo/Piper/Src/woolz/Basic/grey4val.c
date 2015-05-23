/*
 * grey4val.c	Jim Piper	November 1983
 *
 * Modifications
 *
 *  18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	05 May 1987		BDP		protection against null or empty objs
 *	12 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>


#define rvdmn ((struct rectvaltb *)vdmn)

/*
 * Extract 4 grey values from object
 * at locations (k,l), (k+1,l), (k,l+1), (k+1,l+1).
 * This should save some time compared with 4 calls to greyval().
 * Object protection added to this module to protect against erroneous
 * objects and interval and value domains, local variables are now initialised
 * after the object has been checked rather than at point of declaration
 * bdp 5/5/87
 */
grey4val(obj,l,k,g4)
struct object *obj;
int l,k;
register GREY *g4;
{
	register ki, i, n, j;
	int nv;
	register struct intervaldomain *idmn ;
	register struct interval *intl;
	register struct valuetable *vdmn ;
	register struct intervalline *itvl;
	register struct valueline *vlln;
	struct valintline *val;
	GREY *g;

	/* if a null object is detected, or if either of the interval or value
	domains are null, then four zeros are returned, otherwise if an empty
	object or out of range parameter is specified , then the current checking
	will ensure module is protected and return upto four background values.
	In the checks below the object check will also check that for a type 1
	object an interval domain exists.	bdp 5/5/87  */
	
	for (i=0; i<4; i++ )
		g4[i] = 0;
	
	if (woolz_check_obj(obj, "grey4val") != 0)
		return(1);
	if (obj->vdom == NULL)
		return(1);
	
	idmn = obj->idom;	/* these moved from decl. to after consistency checks */
	vdmn = obj->vdom;
	
	for (i=0; i<4; i++ )
		g4[i]=vdmn->bckgrnd;

	/* the four points are generated, two on first pass through tthe following
	loop, on line 'l', then next two on line l+1 on second pass */
	
	for (i=0; i<=1; i++) {
	    if (l >= idmn->line1 && l <= idmn->lastln) {
		switch (idmn->type) {
		case 1:			/* interval domain */
			itvl = &idmn->intvlines[l - idmn->line1];
			n = itvl->nintvs;
			intl = itvl->intvs;
			ki = k - idmn->kol1;
			while (n-- > 0) {
				if (ki+1 < intl->ileft)
					break;		/* from while statement */
				if (ki <= intl->iright) {
					switch (vdmn->type){
					case 1:
						vlln = vdmn->vtblines[l - vdmn->line1];
						g = vlln->values + k - vlln->vkol1;
						break;
					case 11:
						g = rvdmn->values + k - rvdmn->kol1 + rvdmn->width*(l- rvdmn->line1);
						break;
					case 21:
						val = ((struct ivtable *)vdmn)->vil +
								l - ((struct ivtable *)vdmn)->line1;
						nv = val->nintvs;
						vlln = val->vtbint;
						while (nv-- > 0) {
							if (k >= vlln->vkol1-1 && k <= vlln->vlastkl) {
								g = vlln->values + k - vlln->vkol1;
								break; /* from while loop */
							}
							vlln++;
						}
						break;
					}
					/* points cannot lie in two intervals */
					if (ki >= intl->ileft)
						g4[0] = *g;
					if (ki+1 <= intl->iright)
						g4[1] = *(g+1);
				}
				intl++;
			}
			break;
		default:
			break;
		case 2:			/* rectangular domain */
			if (k+1 < idmn->kol1 || k > idmn->lastkl)
				break;
			switch(vdmn->type) {
				case 1:
					vlln = vdmn->vtblines[l - vdmn->line1];
					g = vlln->values + k - vlln->vkol1;
					break;
				case 11:
					g = rvdmn->values + k - rvdmn->kol1 + rvdmn->width*(l- rvdmn->line1);
					break;
				case 21:
					val = ((struct ivtable *)vdmn)->vil +
							l - ((struct ivtable *)vdmn)->line1;
					nv = val->nintvs;
					vlln = val->vtbint;
					while (nv-- > 0) {
						if (k >= vlln->vkol1-1 && k <= vlln->vlastkl) {
							g = vlln->values + k - vlln->vkol1;
							break; /* from while loop */
						}
						vlln++;
					}
					break;
			}
			if (k >= idmn->kol1)
				g4[0] = *g;
			if (k+1 <= idmn->lastkl)
				g4[1] = *(g+1);
			break;
		}
		l++;
		g4 += 2;
	    }
	}
}
