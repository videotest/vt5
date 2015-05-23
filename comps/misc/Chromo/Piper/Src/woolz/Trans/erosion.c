/*
 *
 *	E R O S I O N . C --
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
 *	07 May 1987		BDP		protection against null or empty objs
 *	 9 Oct 1986		CAS		Changed a freedom, Free to a freeobj
 *	24 Sep 1986		GJP		Big Free not little free
 *	13 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>


/*
 * erosion by 8-way shifting followed by intersection
 * Protection added against NULL or empty objects. Null objects received
 * will have null returned, empty ones will have empty objects returned
 * There will be one inconsistent effect which should be noted, until
 * intersection is changed, erosion of a single point object will result in
 * a null object being generated rather than the expected empty object.
 * Another point to be mindfulof , is the negative shifting, generation of
 * negative kols is quite possible but should be sorted out by a standardidom
 * done in the intersection, but effect of negative line1 is uncertain . bdp 7/5/87
 */
struct object *
erosion (obj)
struct object *obj;
{
	register i,j,k;
	struct object *objs[9], *erodobj;
	struct object *intersectn(), *shift();

	if (woolz_check_obj(obj, "erosion") != 0)
		return(NULL);
	if (wzemptyidom(obj->idom) > 0)
		return(makemain(1,  newidomain(obj->idom),NULL,NULL,NULL) );

	k = 0;
	for (i = -1; i <= 1; i += 2) {
		for (j = -1; j <= 1; j++)
			objs[k++] = shift(obj,i,j);
		objs[k++] = shift(obj,0,i);
	}
	objs[8] = obj;
	erodobj = intersectn(objs,9,0);
	erodobj->assoc = obj;
	for (k=0; k<8; k++) {
		freeobj(objs[k]);
	}
	return(erodobj);
}
