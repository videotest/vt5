/*
 * intersect2.c	Jim Piper	August 1983
 * make intersections of 2 domain, value table objects.
 *
 * Modifications
 *
 *	13 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>
struct object *intersect2 (obj1, obj2)
struct object *obj1, *obj2;
{
	struct object *objs[2], *intersectn();
	objs[0] = obj1;
	objs[1] = obj2;
	return (intersectn(objs,2,0));
}
