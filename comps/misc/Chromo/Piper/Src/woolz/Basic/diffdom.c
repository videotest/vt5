/*
 *      D I F F D O M . C  --  Construct a new object whose interval domain is 
 *							   the set-difference of the domains of two given 
 *							   objects, an empty object may be produced.
 *							   Protection is given against empty or null objects.
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
 *  Date:    23 January 1985.
 *
 *  Modifications :
 *  18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *		12 Oct 1987		BDP		Moved initialisation of idom1 out of declarations
 *	 	 2 Oct 1987		BDP		Corrected the empty object handling, now simplified
 *		28 Jul 1987		jimp	Bug-fix in overlapping intervals tests.
 *		15 May 1987		BDP		Added check for when both objs the same, caused 
 *								zero malloc before
 *		 1 May 1987		BDP		protection against null or empty objs
 *		15 Oct 1986		CAS		Linkcount
 *		12 Sep 1986		CAS		Includes
 */
#include <stdio.h>
#include <wstruct.h>

struct object *
diffdom(obj1,obj2)
struct object *obj1,*obj2;
{
	struct iwspace iwsp2, iwsp1;
	struct interval *intp;
	register struct interval *jntp;
	struct intervaldomain *diffdom, *makedomain();
	struct intervaldomain *idom1;
	register int k1, dfinished, intervalcount;
	struct object *diff, *makemain();

	if ( woolz_check_obj(obj1, "diffdom.1") != 0 )
		return(NULL);		/* object 1 NULL return NULL */

/*
 *	 object 1 empty return an empty object 
 */
	if (wzemptyidom(obj1->idom) > 0) {
		fprintf(stderr, " object 1 empty object, return\n");
		return( makemain(1,newidomain(obj1->idom), NULL, NULL, NULL ));
	}
/*
 *	 object 2 NULL or empty then nothing to subtract so just copy 
 */
	if ((woolz_check_obj(obj2, "diffdom.2") != 0 ) || (wzemptyidom(obj2->idom) > 0) ) {
		fprintf(stderr, " object 2 empty object, return\n");
		return( makemain(1, newidomain(obj1->idom), obj1->vdom, NULL, NULL));
	}
			
	/* both objects ok, process them as normal... */
	/*	
	 * make a new interval table that is big enough
	 */
	idom1 = obj1->idom;
	k1 = idom1->kol1;
	intp = (struct interval *) Calloc(intcount(idom1)+intcount(obj2->idom), sizeof(struct interval));
	jntp = intp;
	diffdom = makedomain(1,idom1->line1,idom1->lastln,k1,idom1->lastkl);
	diffdom->freeptr = (char *) intp;
	diffdom->linkcount = 0;
	diff = makemain(1,diffdom,obj1->vdom,NULL,NULL);

	/*
	 * initialise interval scanning
	 */
	initrasterscan(obj1,&iwsp1,0);
	initrasterscan(obj2,&iwsp2,0);
	dfinished = 0;
	intervalcount = 0;
	/*
	 * scan object constructing intervals after subtraction of obj2
	 */
	nextinterval(&iwsp2);
	while (nextinterval(&iwsp1) == 0) {
		/*
		 * case 1 - interval in obj1 succedes interval in obj2 -
		 *	    get next interval of obj2
		 */
		while (dfinished == 0 &&
		    (iwsp1.linpos > iwsp2.linpos ||
		    (iwsp1.linpos == iwsp2.linpos && iwsp1.lftpos > iwsp2.rgtpos))){
			dfinished = nextinterval(&iwsp2);
		}
		/*
		 * case 2 - interval in obj1 precedes interval in obj2
		 *	    or obj2 finished - just copy interval
		 */
		if (dfinished != 0 || (iwsp1.linpos < iwsp2.linpos ||
		    (iwsp1.linpos == iwsp2.linpos && iwsp1.rgtpos < iwsp2.lftpos))){
			jntp->ileft = iwsp1.lftpos - k1;
			jntp->iright = iwsp1.rgtpos - k1;
			intervalcount++;
			jntp++;
		}
		/*
		 * case 3 - intervals overlap.
		 * there may be more than one obj2 interval.
		 */
		else {
			/*
			 * test left end of obj1 interval < left
			 * end of obj2 interval, when complete
			 * interval can be constructed immediately
			 */
			if (iwsp2.lftpos > iwsp1.lftpos) {
				jntp->ileft = iwsp1.lftpos - k1;
				jntp->iright = iwsp2.lftpos -1 - k1;
				intervalcount++;
				jntp++;
			}
			do {
				/*
				 * if right end of obj2 interval <
				 * right end of obj1 interval can
				 * plant left end of new interval
				 */
				if (iwsp2.rgtpos < iwsp1.rgtpos) {
					jntp->ileft = iwsp2.rgtpos +1 - k1;
					dfinished = nextinterval(&iwsp2);
					/*
					 * check if next obj2 interval
					 * still overlaps this obj1 interval
					 */
					if (dfinished == 0 && iwsp1.linpos  == iwsp2.linpos &&
						 iwsp1.rgtpos >= iwsp2.lftpos)
						jntp->iright = iwsp2.lftpos -1 -k1;
					else
						jntp->iright = iwsp1.rgtpos - k1;
					intervalcount++;
					jntp++;
				}
			} while  (dfinished == 0 && iwsp1.linpos == iwsp2.linpos && iwsp1.rgtpos > iwsp2.rgtpos);
		}
		/*
		 * is this obj1 interval the last in the line ?
		 * if so, load up the new intervals into the
		 * diff domain.
		 */
		if (iwsp1.intrmn == 0) {
			makeinterval(iwsp1.linpos,diffdom,intervalcount,intp);
			intervalcount = 0;
			intp = jntp;
		}
	}	/* end while */
	standardidom(diff->idom);	/* will remove leading blank or trailing lines */
	
	/* if the diffdomain is empty then no point having a value domain so
	get rid of it.  bdp 15/5/87  */
	
	if (wzemptyidom(diff->idom) > 0)
		if (diff->vdom != NULL)
		{
			diff->vdom->linkcount--;	/* cant be the last owner here */
			diff->vdom = NULL;
		}
	return(diff);
}
