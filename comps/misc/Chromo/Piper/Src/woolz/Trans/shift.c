/*
 *
 *	S H I F T . C --
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
 *  Date:    July 1983.
 *
 *	Purpose:	   
 *
 *	Description:
 *
 *  Modifications:
 *
 *	18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	20 May 1987		BDP		fixed problem with shifted object idom and vdom being out of step
 *	11 May 1987		BDP		protection against null or empty objs
 *	13 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>


/*
 * Shift an object by lshift lines and kshift columns.
 * A new main and domain structure are provided, but the rest of the object
 * structure is NOT copied so beware when freeing space.
 *
 * Protection added, if supplied object doesnt exist then null is returned.
 * if the supplied object is empty, then the routine is alowed to continue
 * and wil generate a shifted empty object . This is consistent with woolz
 * empty objec thandling. bdp 11/5/87
 */
struct object *
shift (obj, lshift, kshift)
struct object *obj;
{
	struct intervaldomain *idom, *jdom;
	struct object *makemain();

	if (woolz_check_obj(obj, "shift") != 0 )
		return(NULL);

	idom = obj->idom;
#ifdef WTRACE
	fprintf(stderr,"shift called with lshift %d, colshift %d\n",lshift,kshift);
#endif WTRACE
	jdom = (struct intervaldomain *) Malloc(sizeof(struct intervaldomain));
	jdom->type = idom->type;
	jdom->line1 = idom->line1+lshift;
	jdom->lastln = idom->lastln+lshift;
	jdom->kol1 = idom->kol1+kshift;
	jdom->lastkl = idom->lastkl+kshift;
	jdom->intvlines = idom->intvlines;
	jdom->freeptr = NULL;
	jdom->linkcount = 0;

	/* make main originally just associated the obj->vdom with new object
	this created an illegal object when lshift != 0. It must either generate
	a new vdom which has also been shifted or just ignore it. As shift is
	at this time used by erosion and dilation only, which both ignore vdom
	a NULL vdom is used. this must be changed if grey table is ever 
	required here.  bdp 20/5/87  */
	
	return (makemain (1, jdom, NULL, NULL, obj));
}
