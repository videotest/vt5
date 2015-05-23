/*
 *
 *	E X T R A C T O V . C --
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
 *  Date:    November 1983.
 *
 *	Purpose: Polygon domain operations.
 *
 *	Description:
 *
 * Modifications
 *
 *	05 Nov 1987		BDP		Dont return a vdom with an empty object
 *	13 Aug 1987		jimp/BDP	Integrated changes to use polytoobj.
 *	07 May 1987		BDP		protection against null or empty objs
 *	19 Jan 1987		CAS			Return NULL if fail after threshold
 *	17 Jan 1987		CAS			Add debug handling
 *	15 Jan 1987		CAS			Handle NULL return from intersect2
 *	15 Oct 1986		CAS			Fixes around space and free
 *	 9 Oct 1986		CAS			Increment vdom linkcount when we copy the
 *								pointer into another object
 *	24 Sep 1986		GJP			Big Free not little free
 *	13 Sep 1986		CAS			Includes
 */


#include <stdio.h>
#include <wstruct.h>


static FILE *debug_channel = NULL;

Extractov_debug(chan)
FILE *chan;
{
	debug_channel = chan;
}

/*
 * extract object which is (1) part of obj,
 * (2) within w pixels of poly
 * Protection added to this routine checks both the input objects. If either  si
 * in error, then null is returned. If either of the supplied objects is empty
 * then an empty object is generated and returned. If w is zero or less an 
 * empty object is returned, and finally if theres no intersection then an
 * empty object will also be returned. It should be noted that this last 
 * change is only possible as a result of changes to function threshold, and
 * allows removal of CAS fix 19/1/87 .   bdp 7/5/87 
 */
struct object * extractov(obj, poly, w)
struct object *obj, *poly;
{
	struct object *pobj, *polytoobj();
	struct object *dobj, *nobj, *dilation(), *intersect2();
	struct polygondomain *pdom;
	register i, n;

	if (debug_channel) {
		fprintf(debug_channel,"Extractov()\n");
		fflush(debug_channel);
	}

	if ( (wzcheckobj(obj) > 0) || (wzcheckobj(poly) > 0) )
		return(NULL);
	pdom = (struct polygondomain *) poly->idom;
	if ( (wzemptypdom(pdom) > 0) || ( w <= 0) )
		return(makemain(1, makedomain(1,1,0,1,0), NULL, NULL, NULL) );
	if (wzemptyidom(obj->idom) > 0)
		return(makemain(1, newidomain(obj->idom) , NULL , NULL ,NULL) );
		
#ifdef WTRACE
	fprintf(stderr,"extractov within %d pixels\n",w);
#endif WTRACE

	/*
	 * Get the polygon as an object which intersects
	 * the original object.
	 */
	pobj = polytoobj(poly->idom);
	dobj = intersect2(obj,pobj);
	freeobj(pobj);
#ifdef WTRACE
	if (dobj == NULL)
		fprintf(stderr,"Extract, intersect2 returned NULL\n");
#endif WTRACE
/*
 *	If the polygon + the main object don't intersect then intersect2
 *	will return an object with an idom where
 *		line1 == lastln and kol1 == lastkl
 *	This seems to have strange effects on dilate/shift etc so lets kludge
 *	this for now and nip this particular problem in the bud and return here
 *	in this case.
 *	The root of the problem is in woolz NULL object handling - This needs
 *	considerable further attention...
 *											CAS
 *
 *	freeobj(nobj);
 *	if ((dobj->idom->line1 == dobj->idom->lastln) && 
 *			(dobj->idom->kol1 == dobj->idom->lastkl)) {
 *		freeobj(dobj);
 *		if (debug_channel) {
 *			fprintf(debug_channel,"extractov - polygon outside object\n");
 *		}
 *		return(NULL);
 *	}
 *	The above CAS fix has now been commented out and could be removed, the
 *  effect described for threshold no longer occurs, now if nothing above
 *	threshold found an empty object where lastln=line1-1 is generated. this
 *  concept is handled throughout woolz.  bdp 7/5/87
 */
/*  The MRC rewrite using polytoobj now means intersect2 is being used instead
 *	of threshold, the same comments app;y however. If there is no intersection
 *  then an empty object will be returned, and this is handled throughout
 *  the woolz library.   bdp 13/8/87
 */
 	if (dobj->vdom)
		freevaluetable(dobj->vdom);
	dobj->vdom = NULL;

	if (debug_channel) {
		fprintf(debug_channel,"Extractov - pre-dilate 0x%x\n",dobj);
		fflush(debug_channel);
	}
	
	/*
	 * dilate w times
	 */
	for (i=0; i<w; i++) {
		nobj = dobj;
		dobj = dilation(nobj);
		freeobj(nobj);
		if (debug_channel) {
			fprintf(debug_channel,"Extractov - dilate 0x%x  %d of %d\n",dobj,i,w);
			fflush(debug_channel);
		}
	}
	if (debug_channel) {
		fprintf(debug_channel,"Extractov - pre-intersect\n");
		fflush(debug_channel);
	}
	/*
	 * intersect with original object
	 * give it the original grey table
	 */
	nobj = intersect2(obj, dobj);
	freeobj(dobj);
	if (nobj) {
		if (nobj->vdom)
			freevaluetable(nobj->vdom);
		if ( wzemptyidom(nobj->idom) == 0)
		{
			nobj->vdom = obj->vdom;		/* only do this if no wz error found */
			if (obj->vdom)
				obj->vdom->linkcount++;
			nobj->assoc = obj;
		}
		else
			nobj->vdom = NULL;
	}
	if (debug_channel) {
		fprintf(debug_channel,"Extractov - done obj 0x%x\n",nobj);
		fflush(debug_channel);
	}
	return(nobj);
}
