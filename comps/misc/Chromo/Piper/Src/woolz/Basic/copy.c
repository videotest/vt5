/*
 *      C O P Y . C  --  Basic object duplication stuff
 *
 *
 *  Written: Clive A Stubbings
 *           Image Recognition Systems
 *           720 Birchwood Boulevard
 *           Birchwood Science Park
 *           Warrington
 *           Cheshire
 *           WA3 7PX
 *
 *	Copyright (c) and intellectual property rights Image Recognition Systems (1988)
 *
 *  Date:    15th August 1988
 *
 *  Modifications
 *
 */
#include <stdio.h>
#include <wstruct.h>

/*
 *	C O P Y V A L U E T B  --  Make a duplicate valuetable of the type
 *								requested for the given object
 */
struct valuetable *copyvaluetb(obj,type,bckgrnd)
struct object *obj;
int		type,bckgrnd;
{
	struct object *nobj, *makemain();
	struct valuetable *vdom, *newvaluetb();
	struct iwspace oldiwsp, newiwsp;
	struct gwspace oldgwsp, newgwsp;
	GREY	*newg, *oldg;
	int		i;
	
	vdom = newvaluetb(obj,type,bckgrnd);
	nobj = makemain(1,obj->idom,vdom,NULL,NULL);
	initgreyscan(obj,&oldiwsp,&oldgwsp);
	initgreyscan(nobj,&newiwsp,&newgwsp);
	while ((nextgreyinterval(&oldiwsp) == 0) && (nextgreyinterval(&newiwsp) == 0)) {
		oldg = oldgwsp.grintptr;
		newg = newgwsp.grintptr;
		for (i=0; i<oldiwsp.colrmn; i++) {
			*newg++ = *oldg++;
		}
	}
	nobj->vdom = NULL;
	freeobj(nobj);
	return(vdom);
}


