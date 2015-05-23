/*
 * octagon.c		Jim Piper		06-05-86
 *
 * Construct enclosing right octagon as a closed polygon.
 * Parameter tip should be set to zero unless the octagon
 * round the object minus its vertical extremeties is required.
 *
 * Modifications
 *
 *	18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	01 Dec 1987		BDP		rewritten to fix bug when using non zero tip parameter
 *	08 May 1987		BDP		protection against null or empty objs
 *	13 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>

/* 
 * protection against null objects added, will return null if probelm found.
 * protection against empty objects is also included to prevent a rather 
 * strange polygon emerging. As an empty polygon is not very sensible a NULL
 * is returned instead. As woolz has been protected now against null and 
 * empty polygons the distinction is perhaps trivial. bdp 8/5/87
 */
struct object *
octagon(obj,tip)
struct object *obj;
{
	struct object *oct, *makemain();
	struct polygondomain *odom, *makepolydmn();
	struct iwspace iwsp;
	register struct intervaldomain *idom;
	/*register*/ struct ivertex *vtx;
	register int l;
	int line1, lastln, kol1, lastkl;	
	int minxpy,maxxpy,maxxmy,maxymx;
	
	if (woolz_check_obj(obj, "octagon") != 0)
		return(NULL);
	idom = obj->idom;
	if (wzemptyidom(idom) > 0)
		return(NULL);			/* could return an empty polygon domain instead */

	kol1 = idom->kol1;
	lastkl = idom->lastkl;
	line1 = idom->line1;
	lastln = idom->lastln;
	
	odom = makepolydmn(1,NULL,0,9,1);
	odom->nvertices = 9;
	vtx = odom->vtx;
	oct = makemain(10,odom,NULL,NULL,NULL);
	/*
	 * find max(x+y), min(x+y), max(x-y), max(y-x)
	 */
	/* 01/11/87 bdp. The kol and line domain extents below need to reflect
	 * only that part of the object scanned for min and max values ,otherwise
	 * objects with broad tops or bases give wierd results. Need to scan the
	 * object within the tip line domains to find min and max column extents
	 */
	minxpy = lastln + lastkl;
	maxxpy = line1 + kol1;
	maxxmy = kol1 - lastln;
	maxymx = line1 - lastkl;
	line1 += tip;
	lastln -= tip;
	kol1 = idom->lastkl;	/* set to max ready for scan for min */
	lastkl = idom->kol1;	/* set to min ready for scan for max */
	
	initrasterscan(obj,&iwsp,0);
	while (nextinterval(&iwsp) == 0) {
		l = iwsp.linpos;
		if (l < line1 || l > lastln)
			continue;
		if (l + iwsp.lftpos < minxpy)
			minxpy = l + iwsp.lftpos;
		if (l + iwsp.rgtpos > maxxpy)
			maxxpy = l + iwsp.rgtpos;
		if (l - iwsp.lftpos > maxymx)
			maxymx = l - iwsp.lftpos;
		if (iwsp.rgtpos - l > maxxmy)
			maxxmy = iwsp.rgtpos - l;
		if (iwsp.lftpos < kol1)
			kol1 = iwsp.lftpos;
		if (iwsp.rgtpos > lastkl)
			lastkl = iwsp.rgtpos;
	}
#ifdef DEBUG
	fprintf(stderr,"octagon: minxpy %d, maxxpy %d, maxxmy %d, maxymx %d\n",
				minxpy, maxxpy, maxxmy, maxymx );
#endif DEBUG
	/*
	 * find and load intersection points
	 */
	/* use the modified kol and line extents below rather than the original
	 * ones. 1/11/87 bdp.
	 */
	 		
	vtx->vtX = kol1;
	vtx++->vtY = kol1 + maxymx;
	vtx->vtX = lastln - maxymx ;
	vtx++->vtY = lastln ;
	vtx->vtX = maxxpy - lastln ;
	vtx++->vtY = lastln ;
	vtx->vtX = lastkl;
	vtx++->vtY = maxxpy - lastkl;
	vtx->vtX = lastkl;
	vtx++->vtY = lastkl - maxxmy;
	vtx->vtX = line1 + maxxmy ;
	vtx++->vtY = line1;
	vtx->vtX = minxpy - line1 ;
	vtx++->vtY = line1;
	vtx->vtX = kol1;
	vtx++->vtY = minxpy - kol1;
	vtx->vtX = kol1;				/* repeat first point for display */
	vtx++->vtY = kol1 + maxymx;
	
	return(oct);
}


