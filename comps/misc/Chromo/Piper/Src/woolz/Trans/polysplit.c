/*
 * polysplit.c		Jim Piper	November 30 1983
 * polygon domain operations
 *
 * Modifications
 *
 *	18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	13 Aug 1987 BDP		Integrated protection against null or empty objs
 *	16 Jul 1987	jimp	Bug fix (dobj->vdom = NULL)
 *	13 mar 1987	jimp	Major re-write to use Ji Liang's polytoobj()
 *	 6 Jan 1987	CAS		An extra bit of debug
 *	15 Oct 1986	CAS		Fix free bit
 *	 9 Oct 1986	CAS		Added a local debug facility - while
 *					looking for not deallocd space
 *	24 Sep 1986	GJP		Big Free not little free
 *	13 Sep 1986	CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>
#define SIZETHRESH	3

static FILE *debug_channel = NULL;

/*
 *	P O L Y S P L I T _ D E B U G
 *

 * Protection added will return an object list of zero if supplied object
 * is either NULL or empty. If the polydomain is empty then zero is
 * also returned 
 */
Polysplit_debug(chan)
FILE	*chan;
{
	debug_channel = chan;
}

/*
 *	P O L Y S P L I T
 *
 * use a polygon to split a type 1 object
 * suppress objects whose length or width is less than SIZETHRESH
 */
polysplit (obj, poly, objlist, numobj)
struct object *obj, *poly, **objlist;
register int *numobj;
{
	struct object *pobj, *polytoobj(), *dobj, *diffdom();
	register int i;

	*numobj = 0;
	if (woolz_check_obj(obj, "polysplit.1") != 0)
		return(1);
	if (wzemptyidom(obj->idom) > 0)
		return(1);
	if (woolz_check_obj(poly, "polysplit.2") != 0)
		return(1);
	/*
	 * convert polygon to object
	 */
	pobj = polytoobj(poly->idom);
	/*
	 * subtract from input object
	 */
	dobj = diffdom(obj,pobj);
	/*
	 * segment, ignoring small objects
	 */
	topset(1);		/* set for square connectivity */
	label(dobj,numobj,objlist,10,SIZETHRESH);
	topset(0);		/* reset for 8-connectivity */
	if (debug_channel)
		fprintf(debug_channel,"Polysplit finished label\n");
	for (i=0; i<*numobj; i++) {
		objlist[i]->vdom = obj->vdom;
		if (obj->vdom) obj->vdom->linkcount++;
		objlist[i]->assoc = obj;
	}
	freeobj(pobj);
	dobj->vdom = NULL;	/* attached by "diffdom" */
	freeobj(dobj);
	return(0);
}
