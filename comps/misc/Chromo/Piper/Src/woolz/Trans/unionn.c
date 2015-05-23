/*
 *
 *	U N I O N N . C --
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
 *  Date:   August 1983.
 *
 *	Purpose:	   
 *
 *	Description:
 *
 * Modifications
 *
 *	18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	02 Nov 1987		BDP			Fixed a workspace initialisation error for empty objs
 *	02 Sep 1987		jimp@IRS	Vdom->linkcount set.
 *	11 May 1987		BDP			protection against null or empty objs
 *	15 Oct 1986		CAS			linkcount
 *	24 Sep 1986		GJP			Big Free not little free
 *	13 Sep 1986		CAS			Includes
 */

#include <stdio.h>
#include <wstruct.h>

/*
 * union of set of objects.
 * Do domains only unless "uvt" non-zero,
 * in which case make an average grey table.
 * Protection added to cope with null and empty objects. Protection also has
 * been added to cope with the negative line numbers possible from dilation etc.
 * As this routine is a logical OR operation an attempt is made to carry on with
 * usable objects where possible and to leave out unusable ones.  Accordingly
 * the array of input objects is firstly scanned and has NULL objects removed
 * and the count decremented accoringly. The smae count check as at present is
 * then applied at this point, and if no objects now exist, NULL is returned.
 * empty objects will be handled but make no contribution to new object. If 
 * all objects are empty, this will be detected and an empty object generated
 * and returned. for negative line handling a change has been intorduced into the 
 * scanning routines to give protection here. the resultant object will clip
 * the negative lines, and is thus guaranteed to return non negative values.
 * bdp 11/5/87
 */
struct object *
unionn (objs, n, uvt)
struct object **objs;
{
	struct object *obj, *makemain();
	struct intervaldomain *makedomain();
	register struct intervaldomain *idom;
	struct intervalline *ivln;
	struct interval *itvl, *jtvl;
	struct valuetable *newvaluetb();
	register struct iwspace *iwsp;
	struct iwspace *biwsp,*tiwsp,niwsp;
	struct gwspace *gwsp,ngwsp;
	register int i,j;
	int nullcount,k,inttot,numactive,l,change,lwas,nints,noverlap;
	COORD line1,lastln;
	register COORD kol1,lastkl;
	GREY *greyptr, gv;

	/*
	 * preliminary stuff
	 */
	nullcount = 0;			/* count of unusable objects */
	for (i=0; i<n ; i++ )
	{
		if (woolz_check_obj(objs[i], "unionn") != 0)
		{
			obj = objs[i];
			for ( j=i; j<n ; j++ )
				objs[j] = objs[j+1];
			objs[n-1] = obj;
			nullcount++;
		}
	}
	
	n -= nullcount;
	if (n < 1)
		return(NULL);		/* if no usable objects then return a null */
	if (n == 1)
		return(objs[0]);

	/*
	 * check all objects have the same type
	 */
	for (i=0; i<n; i++)
		if (objs[i]->type != 1)
			return(NULL);

#ifdef WTRACE
	fprintf(stderr,"unionn called with %d usable objects\n",n);
#endif WTRACE

	/*
	 * find the line and column bounds of the union.
	 */
	line1 = objs[0]->idom->line1;
	lastln = objs[0]->idom->lastln;
	kol1 = objs[0]->idom->kol1;
	lastkl = objs[0]->idom->lastkl;
	for (i=1; i<n; i++) {
		idom = objs[i]->idom;
		if (line1 > idom->line1)
			line1 = idom->line1;
		if (lastln < idom->lastln)
			lastln = idom->lastln;
		if (kol1 > idom->kol1)
			kol1 = idom->kol1;
		if (lastkl < idom->lastkl)
			lastkl = idom->lastkl;
	}
	/*
	 * count the individual intervals so that sufficient space
	 * for the union may be allocated.
	 */
	inttot=0;
	for (i=0; i<n; i++)
		inttot += intcount(objs[i]->idom);

	/* if the intcount is zero, then all objects are empty so just create 
	and return an empty object */
	
	if (inttot == 0)
		return(makemain(1, newidomain(objs[0]->idom), NULL, NULL, NULL));
	
	/*
	 * set up domain, value table structures, and object.
	 */

	idom = makedomain(1,line1,lastln,kol1,lastkl);
	itvl = (struct interval *) Malloc (inttot * sizeof(struct interval));
	idom->freeptr = (char *) itvl;
	idom->linkcount = 0;
	lwas = line1;
	jtvl = itvl;
	nints = 0;
	obj = makemain(1,idom,NULL,NULL,NULL);

	/*
	 * allocate space for workspaces
	 */
	biwsp = iwsp = (struct iwspace *) Malloc (n * sizeof (struct iwspace));
	tiwsp = iwsp + n;
	numactive = n;

	/*
	 * Construct the union object's table of intervals.
	 * Initialise scanning on each object/workspace combination.
	 * Scan synchronously, setting up the union of adjacent and
	 * overlapping intervals.  Needs a clear head !!
	 */
	for (i=0; i<n; i++) {
		initrasterscan(objs[i],iwsp,0);
		if (nextinterval(iwsp++) != 0)
			numactive--;			/* numactive-- not n-- !! bdp 2/11/87 */
	}

#ifdef WTRACE
	fprintf(stderr,"unionn, %d active workspaces initialised\n",numactive);
	for (iwsp=biwsp; iwsp<tiwsp; iwsp++)
		fprintf(stderr,"unionn, %d linesrmn ",iwsp->linrmn);
	fprintf(stderr,"\n");
#endif WTRACE
	/*
	 * find next line and left hand end of next interval of union
	 */
	while (numactive > 0) {
		/*
		 * find first remaining active object
		 */
		for (iwsp = biwsp; iwsp->linrmn < 0; iwsp++)
			;
		/*
		 * find minimum line number of remaining active intervals
		 */
		l = iwsp->linpos;
		kol1 = iwsp->lftpos;
		lastkl = iwsp->rgtpos;
		for (iwsp++; iwsp<tiwsp; iwsp++)
			if (iwsp->linrmn >= 0 && iwsp->linpos < l) {
				l = iwsp->linpos;
				kol1 = iwsp->lftpos;
				lastkl = iwsp->rgtpos;
			}
		/*
		 * find left-most interval in this line
		 */
		for (iwsp=biwsp; iwsp<tiwsp; iwsp++)
			if (iwsp->linrmn >= 0 && iwsp->linpos == l && iwsp->lftpos < kol1) {
				kol1 = iwsp->lftpos;
				lastkl = iwsp->rgtpos;
			}
		/*
		 * construct maximal interval with current left end-point
		 */
		do {
			change = 0;
			for (iwsp=biwsp; iwsp<tiwsp; iwsp++) {
				while (iwsp->linrmn >= 0 && iwsp->linpos == l && iwsp->lftpos <= lastkl+1) {
					if (iwsp->rgtpos > lastkl) {
						lastkl = iwsp->rgtpos;
						change = 1;
					}
					if (nextinterval(iwsp) != 0) {
						numactive--;
					}
				}
			}
		} while (change == 1);
		itvl->ileft = kol1 - idom->kol1;
		itvl->iright = lastkl - idom->kol1;
		if (l == lwas)
			nints++;
		else {
			makeinterval(lwas,idom,nints,jtvl);
			for (j = lwas+1; j<l; j++) {
				/*
				 * we would prefer makeinterval(j,idom,0,NULL) here but
				 * it has unfortunate side-effects in "ibound" and "label"
				 */
				makeinterval(j,idom,0,itvl);
			}
			lwas = l;
			nints = 1;
			jtvl = itvl;
		}
		itvl++;
	}
	makeinterval(lwas,idom,nints,jtvl);
	for (j = lwas+1; j<=lastln; j++) {
		/*
		 * we would prefer makeinterval(j,idom,0,NULL) here but
		 * it has unfortunate side-effects in "ibound" and "label"
		 */
		makeinterval(j,idom,0,itvl);
	}

	if (uvt != 0) {
		gwsp = (struct gwspace *) Malloc (n * sizeof (struct gwspace));
		/*
		 * construct an empty greytable
		 */
		obj->vdom = newvaluetb(obj,objs[0]->vdom->type,objs[0]->vdom->bckgrnd);
		obj->vdom->linkcount = 1;
	
		/*
		 * fill the grey table.  Where more than one input objects
		 * overlap, take mean of grey values.
		 */
		initgreyscan(obj,&niwsp,&ngwsp);
		iwsp = biwsp;
		for (i=0; i<n; i++) {
			initgreyscan(objs[i],iwsp,&gwsp[i]);
			nextgreyinterval(iwsp++);
		}
		while (nextgreyinterval(&niwsp) == 0) {
			l = niwsp.linpos;
			greyptr = ngwsp.grintptr;
			for (k = niwsp.lftpos; k <= niwsp.rgtpos; k++) {
				noverlap = 0;
				gv = 0;
				for (iwsp=biwsp,i=0; iwsp<tiwsp; iwsp++,i++) {
					while (iwsp->linrmn >= 0 && (iwsp->linpos < l || (iwsp->linpos == l && iwsp->rgtpos < k)))
						nextgreyinterval(iwsp);
					if (iwsp->linrmn >= 0 && iwsp->linpos == l && iwsp->lftpos <= k) {
						noverlap++;
						gv += *(gwsp[i].grintptr + k - iwsp->lftpos);
					}
				}
				*greyptr++ = gv / noverlap;
			}
		}
		Free(gwsp);
	}
#ifdef WTRACE
	fprintf(stderr,"unionn exiting\n");
#endif WTRACE

	Free(biwsp);
	return(obj);
}
