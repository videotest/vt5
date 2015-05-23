/*
 *
 *	E R O S I O N 4 . C --
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
 *  Date:   July 1983.
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
 * erosion by 4-way shifting followed by intersection
 * Protection aganst null and empty objects has been added in the same way as
 * for erosion.c, namely detection of supplied null, null is returned, detection
 * of supplied empty object, empty object is generated and returned. Not e also
 * possible null return from a single point object due to working of intersectn.
 * This is inconsistent and should be an empty object. bdp 7/5/87 
 */
struct object *
erosion4 (obj)
struct object *obj;
{
	register i,j,k;
	struct object *objs[5], *erodobj;
	struct object *intersectn(), *shift();

	if (woolz_check_obj(obj, "erosion4") != 0)
		return(NULL);
	if (wzemptyidom(obj->idom) > 0)
		return(makemain(1, newidomain(obj->idom), NULL, NULL, NULL) );
		
	k = 0;
	for (i = -1; i <= 1; i += 2) {
		objs[k++] = shift(obj,0,i);
		objs[k++] = shift(obj,i,0);
	}
	objs[4] = obj;
	erodobj = intersectn(objs,5,0);
	erodobj->assoc = obj;
	for (k=0; k<4; k++) {
		freeobj(objs[k]);
	}
	return(erodobj);
}
