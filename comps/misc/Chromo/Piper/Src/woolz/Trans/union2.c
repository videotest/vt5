/*
 *
 *	U N I O N 2 . C --
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
 *  Date:    August 1983.
 *
 *	Purpose:	Make union of 2 domain, value table objects.   
 *
 *	Description:
 *
 * Modifications
 *
 *	13 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>


struct object *union2 (obj1, obj2)
struct object *obj1, *obj2;
{
	struct object *objs[2], *unionn();
	objs[0] = obj1;
	objs[1] = obj2;
	return (unionn(objs,2,0));
}
