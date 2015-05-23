/*
 *
 *	D I L A T I O N . C --
 *
 *           Image Recognition Systems
 *           720 Birchwood Boulevard
 *           Birchwood Science Park
 *           Warrington
 *           Cheshire
 *           WA3 7PX
 *
 *	Written: J. Piper
 *           Medical Research Council
 *           Clinical and Population Cytogenetics Unit
 *           Edinburgh.
 *
 *	Copyright (c) and intellectual property rights Image Recognition Systems
 *
 *  Date:    	   January 1985.
 *
 *	Purpose:	   8-neighbour dilation, second version 
 *				   Expand intervals to laft and right by one (n) points
 *				   then take union with one-up and one-down shifted version.
 *
 *	Description:
 *
 * Modifications
 *
 *	11 Jan 1991		CAS			Allow negative line numbers...
 *	18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	06 May 1987		BDP			protection against null or empty objs
 *	12 Dec 1986		CAS/GJP		Don't return vdom - since it is meaningless
 *	15 Oct 1986		CAS			Fix space, linkcount etc
 *	 9 Oct 1986		CAS			Increment link count when we copy vdom
 *								from one object to another
 *								Changed a freedomain, Free to freeobj
 *	24 Sep 1986		GJP			Big Free not little free
 *	13 Sep 1986		CAS			Includes
 */

#include <stdio.h>
#include <wstruct.h>

#define WOOLZ_NEG_LINES			/* Define to allow generation of objects with neg lines */

struct object *
dilation (obj)
struct object *obj;
{
	register i;
	struct object *objs[3], *dilobj;
	struct object *unionn(), *shift();
	struct intervaldomain *idomsav,*newidomain(),*idom;
	struct iwspace iwsp;
	int stat;
	
	if (( woolz_check_obj(obj, "dilation")) != 0) 
		return(NULL);
	if (wzemptyidom(obj->idom) > 0)
		return(makemain(1,newidomain(obj->idom),NULL,NULL,NULL));
#ifdef WTRACE
	fprintf(stderr,"dilation called\n");
#endif WTRACE

	/*
	 * copy the interval domain
	 */
	idomsav = obj->idom;
	idom = newidomain(idomsav);
	idom->linkcount++;
	obj->idom = idom;

	/*
	 * scan object, expand intervals
	 * the following section of code has been duplicated as detailed above to 
	 * protect object from getting negative columns.  bdp 6/5/87
	 */
	initrasterscan(obj,&iwsp,0);
	if ( obj->idom->kol1 > 0 )
	{		/* can never be a negative in this case so dont bother checking */
		while (nextinterval(&iwsp) == 0) {
			iwsp.intpos->ileft--;
			iwsp.intpos->iright++;
		}
	}
	else
	{		/* kol1 must be zero, so just check ileft > 0 */
		while (nextinterval(&iwsp) == 0) {
			if (iwsp.intpos->ileft > 0 )
				iwsp.intpos->ileft--;
			iwsp.intpos->iright++;
		}
	}
	/*
	 * shift and take union
	 * This will also deal with nasties on account of
	 * overlapping intervals ?
	 * the second shift may also produce a negative line1 which unionn needs to
	 * be able to cope with. cannot alter this before the union or the dilation
	 * in the reest of the object wont happen properly, so will delete it upon 
	 * completion of the union.  bdp 6/5/87
	 */
	objs[0] = shift(obj,1,0);
	objs[1] = shift(obj,-1,0);
	objs[2] = obj;
	dilobj = unionn(objs,3,0);

#ifndef WOOLZ_NEG_LINES
	if (dilobj->idom->line1 < 0)
		dilobj->idom->line1 = 0;
#endif
	/*
	 * tidy up and return
	 */
/*	if (dilobj->vdom)
 *		freevaluetable(dilobj->vdom);
 *	dilobj->vdom = obj->vdom;
 *	if (obj->vdom)
 *		obj->vdom->linkcount++;
 */
	dilobj->assoc = obj;
	for (i=0; i<2; i++) {
		freeobj(objs[i]);
	}
	obj->idom = idomsav;
	freedomain(idom);
	standardidom(dilobj->idom);		/* sort any negative interval positions out */
	return(dilobj);
}

