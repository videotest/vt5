/*
 * makeivtable.c		Jim Piper		24 May 1985
 *
 * make a type 21 (interval-structured)
 * grey table to cover the domain of an object.
 *
 * It is intended that in future this routine might also make
 * a type 22 grey-table (float grey values) hence the "type" parameter.
 *
 * Modifications
 *
 *	22 Nov 1988		dcb		Added param. to woolz_exit to say where called from
 *  18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	05 May 1987		BDP		protection against null or empty objs
 *	2  Mar 1987		GJP		Woolz_exit
 *	15 Oct 1986		CAS		Linkcount
 *	12 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>

/*
 * Protection against Null objects intorduced. Generally due to limited use
 * of this routine, may not be necessary but consistency stops future problems
 * arising. Routine will correctly handle empty objects and return an empty
 * interval table. for a problem with supplied object NULL will be returned.
 * this should be handled by the calling routine as null intervaltables are 
 * not supported even by the revised woolz. bdp 5/5/87.
 */
struct ivtable *
makeivtable(type,obj,bckgrnd)
struct object *obj;
{
	struct ivtable *v;
	register struct valintline *vil;
	register struct valueline *val;
	register GREY *g;
	register int a;
	struct iwspace iwsp;
	struct intervaldomain *idom;

	if (type != 21) {
		fprintf(stderr,"MAKEIVTABLE: illegal type %d\n",type);
		woolz_exit(11, "makeivtable");
	}
	if ( woolz_check_obj(obj, "makeivtable") != 0 )
		return(NULL);		/* must be checked for as NULL ivtables not 
							generally handled by the system */
	idom = obj->idom;
	/*
	 * allocate space for basic, per line and per interval structures
	 */
	v = (struct ivtable *) Calloc(sizeof(struct ivtable) +
		(idom->lastln - idom->line1 + 1) * sizeof(struct valintline) +
		intcount(idom) * sizeof(struct valueline), 1);
	vil = (struct valintline *) (v + 1);
	val = (struct valueline *) (vil + idom->lastln - idom->line1 + 1);

	/* protect the calloc against a zero request */
	if ( (a = area(obj) ) == 0)
		g = NULL;
	else
		g = (GREY *) Calloc(a,sizeof(GREY));
	/*
	 * fill in structure values
	 */
	v->freeptr = (char *) g;
	v->linkcount = 0;
	v->type = type;
	v->line1 = idom->line1;
	v->lastln = idom->lastln;
	v->bckgrnd = bckgrnd;
	v->vil = vil;
	v->original = obj;
	/*
	 * fill in the line and interval structures
	 */
	vil--;
	initrasterscan(obj,&iwsp,0);
	while (nextinterval(&iwsp) == 0) {
		if (iwsp.nwlpos != 0) {
			vil += iwsp.nwlpos;
			vil->vtbint = val;
		}
		vil->nintvs++;
		val->vkol1 = iwsp.lftpos;
		val->vlastkl = iwsp.rgtpos;
		val->values = g;
		val++;
		g += iwsp.colrmn;
	}
	return(v);
}
