/*
 * threshold.c		Jim Piper		4 February 1985
 *
 * Modifications
 *
 *	18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	27 May 1987		BDP		dont return vdom or plist with an empty object
 *	11 May 1987		BDP		protection against null or empty objs
 *	15 Oct 1986		CAS			Linkcount
 *	13 Sep 1986		CAS			Includes
 */

#include <stdio.h>
#include <wstruct.h>

extern SMALL highlow;

/*
 * Protection added at two levels to this routine. firstly the input object is
 * now checked for being null or empty, with corresponding null or empty
 * objec tbeing returned, and secondly this routine changed to return a proper
 * empty object when all points in supplied object  are below threshold. As 
 * it was before this routine was a major cause of probelms in these 
 * circumstances.  bdp 11/5/87
 */
struct object *
threshold (obj,thresh)
struct object *obj;
register int thresh;
{
	struct object *nobj, *makemain();
	struct intervaldomain *idom, *makedomain();
	register GREY *g;
	register colno,nints;
	register COORD over;
	COORD nl1,nll,nk1,nkl,l;
	struct iwspace iwsp;
	struct gwspace gwsp;
	register struct interval *itvl, *jtvl;

	/* firstly check the supplied obejct.  For empty objects no additional
	processing is necessary as protection in scanning routines will ensure
	that the while loop is not performed and nints is zero. this will result
	in the two circumstances of empty object generation being handled by the 
	same piece of code.  bdp 11/5/87  */
	
	if (woolz_check_obj(obj, "threshold") != 0 )
		return(NULL);

#ifdef WTRACE
	fprintf(stderr,"threshold called with value %d\n",thresh);
#endif WTRACE

	/*
	 * first pass - find line and column bounds of thresholded
	 * object and number of intervals.
	 */
	idom = obj->idom;
	nl1 = idom->lastln;
	nll = idom->line1;
	nk1 = idom->lastkl;
	nkl = idom->kol1;
	initgreyscan(obj,&iwsp,&gwsp);
	nints = 0;
	while (nextgreyinterval(&iwsp) == 0) {
		g = gwsp.grintptr;
		over = 0;
		for (colno = iwsp.lftpos; colno <= iwsp.rgtpos; colno++) {
			if ((highlow != 0 && *g >= thresh) ||
			    (highlow == 0 && *g < thresh)) {
				if (over == 0) {
					over = 1;
					if (iwsp.linpos < nl1)
						nl1 = iwsp.linpos;
					if (iwsp.linpos > nll)
						nll = iwsp.linpos;
					if (colno < nk1)
						nk1 = colno;
				}
			} else {
				if (over == 1) {
					if (colno > nkl)
						nkl = colno;
					over = 0;
					nints++;
				}
			}
			g++;
		}
		if (over == 1) {
			if (colno > nkl)
				nkl = colno;
			over = 0;
			nints++;
		}
	}
	nkl--;	/* since we have looked at points beyond interval ends */

	/* domain structure */
	if (nints > 0) {
		idom = makedomain(1,nl1,nll,nk1,nkl);
		itvl = (struct interval *) Malloc(nints * sizeof(struct interval));
		idom->freeptr = (char *) itvl;
		idom->linkcount = 0;

		/*
	 	* second pass - construct intervals
	 	*/
		initgreyscan(obj,&iwsp,&gwsp);
		nints = 0;
		jtvl = itvl;
		/* find thresholded endpoints */
		while (nextgreyinterval(&iwsp) == 0) {
			if (iwsp.linpos < nl1 || iwsp.linpos > nll)
				continue;
			g = gwsp.grintptr;
			over = 0;
			for (colno = iwsp.lftpos; colno <= iwsp.rgtpos; colno++) {
				if ((highlow != 0 && *g >= thresh) ||
			    	(highlow == 0 && *g < thresh)) {
					if (over == 0) {
						over = 1;
						itvl->ileft = colno - nk1;
					}
				} else {
					if (over == 1) {
						over = 0;
						itvl->iright = colno - nk1 - 1;
						nints++;
						itvl++;
					}
				}
				g++;
			}
			if (over == 1) {
				over = 0;
				itvl->iright = colno - nk1 - 1;
				nints++;
				itvl++;
			}
			/*
		 	* end of line ?
		 	*/
			if (iwsp.intrmn == 0) {
				makeinterval(iwsp.linpos,idom,nints,jtvl);
				jtvl = itvl;
				nints = 0;
			}
		}
		/* main object */
		nobj = makemain(1,idom,obj->vdom,obj->plist,obj);
	} else
		/* no thresholded points - make an empty domain. the change to the
		following statement is to make lastln les than first line for an empty
		object, and is very important.  bdp 11/5/87  */
		
		nobj = makemain(1,makedomain(1,1,0,1,0),NULL,NULL, NULL);

	return(nobj);
}
