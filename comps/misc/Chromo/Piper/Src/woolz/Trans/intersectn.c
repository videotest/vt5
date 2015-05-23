/*
 *
 *	I N T E R S E C T N . C --
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
 * Modifications
 *
 *	18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	14 May 1987		BDP		added check for non overlapping objs to prevent negative malloc
 *	08 May 1987		BDP		protection against null or empty objs
 *	08 May 1987		BDP		15/1/87 mod changed to return empty not NULL
 *	15 jan 1987		CAS		Cope with case of no intersection
 *	15 Oct 1986		CAS		initialisation of linkcount
 *	24 Sep 1986		GJP		Big Free not little free
 *	13 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>

#define TRUE 1
#define FALSE 0
/*
 * Intersection of set of objects.
 * Do domains only if "uvt" zero,
 * in "uvt" non-zero make an average grey table.
 * Protection added. As this is an intersectn, if any of the supplied objects
 * is empty then an empty object is generated and returned. NULL object is
 * regarded as an error and will cause just a NULL to be returned as elsewhere
 * in woolz  . Additionally this module has been changed to return an empty
 * object rather than a null object when no parts of supplied objects intersect
 * bdp 8/5/87  
 */
struct object *
intersectn (objs, n, uvt)
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
	register int i;
	int nullobject,emptyobject;		/* only used for protection checks */
	int j,k,inttot,l,change,lwas,nints,anyintersect;
	COORD line1,lastln;
	register COORD kol1,lastkl;
	GREY *greyptr, gv;

	/*
	 * preliminary stuff
	 * this section expanded to contain extra woolz null and empty checking
	 * the test loop will preferentially test for any of the objects being
	 * null or the count being being zero and return null. it will secondarily
	 * test for empty. This is done in one loop using flags but could be 
	 * done in two loops with embedded returns equally messily ! bdp 8/5/87
	 */
	nullobject = emptyobject = FALSE;
	for (i=0; i<n ; i++)
	{
		if (woolz_check_obj(objs[i], "intersectn") != 0)
			nullobject = TRUE;
		else
			if (objs[i]->type != 1)	/* test retained from earlier checks. 8/5/87 */
				nullobject = TRUE;
			else
				if (wzemptyidom(objs[i]->idom) > 0)
					emptyobject = TRUE;
	}
	
	/* the ordering of the following tests is important */
	
	if ( (n < 1) || nullobject )
	{
#ifdef WTRACE
		fprintf(stderr,"intersctn exiting with n = %d  nullobject = %d\n",n,nullobject);
#endif WTRACE
		return(NULL);
	}
	if ( emptyobject )
	{
#ifdef WTRACE
		fprintf(stderr,"intersectn exiting , empty object\n");
#endif WTRACE
		return(makemain(1, makedomain(1,1,0,1,0), NULL, NULL, NULL ));
	}
	if (n == 1)
		return(objs[0]);

	anyintersect = 0;
	/*
	 * find the line and column bounds of the intersection.
	 */
	line1 = objs[0]->idom->line1;
	lastln = objs[0]->idom->lastln;
	kol1 = objs[0]->idom->kol1;
	lastkl = objs[0]->idom->lastkl;
	for (i=1; i<n; i++) {
		idom = objs[i]->idom;
		if (line1 < idom->line1)
			line1 = idom->line1;
		if (lastln > idom->lastln)
			lastln = idom->lastln;
		if (kol1 < idom->kol1)
			kol1 = idom->kol1;
		if (lastkl > idom->lastkl)
			lastkl = idom->lastkl;
	}
	/* the following is a bug fix for when non overlapping objects cause a 
	negative malloc. The use of embedded returns is not too structured but
	expedient for error handling.  bdp 14/5/87 */
	
	if ((line1 > lastln) || (kol1 > lastkl))
		return(makemain(1, makedomain(1,1,0,1,0), NULL, NULL, NULL ));
		
	/*
	 * count the individual intervals so that sufficient space
	 * for the intersection may be allocated.
	 */
	inttot=0;
	for (i=0; i<n; i++)
		inttot += intcount(objs[i]->idom);

	/*
	 * set up domain, value table structures, and object.
	 */
#ifdef WTRACE
	fprintf(stderr,"interesctn, making domain with l1 = %d, ll = %d, lkol = %d\n",line1,lastln,lastkl,kol1);
#endif WTRACE
	
	idom = makedomain(1,line1,lastln,0,lastkl-kol1);
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
	tiwsp = biwsp + n;

	/*
	 * Construct the intersection object's table of intervals.
	 * Initialise scanning on each object/workspace combination.
	 * Scan synchronously, setting up the intersection of
	 * overlapping intervals.  Needs a clear head !!
	 */
	for (i=0; i<n; i++) {
		initrasterscan(objs[i],iwsp,0);
		nextinterval(iwsp++);
	}

	l = lwas;
	for (;;) {
	/*
	 * find next line of intersection
	 */
		do {
			change = 0;
			for (iwsp=biwsp; iwsp<tiwsp; iwsp++)
				if (iwsp->linpos > l)
					l = iwsp->linpos;
			for (iwsp=biwsp; iwsp<tiwsp; iwsp++) {
				while (iwsp->linpos < l) {
					if (nextinterval(iwsp) != 0)
						goto firstfinished;
					if (iwsp->linpos > l)
						change = 1;
				}
			}
		} while (change != 0);
	/*
	 * find next interval of intersection
	 */
		kol1 = biwsp->lftpos;
		lastkl = biwsp->rgtpos;
		for (;;) {
			do {
				change = 0;
				for (iwsp=biwsp; iwsp<tiwsp; iwsp++)
					if (iwsp->lftpos > lastkl) {
						kol1 = iwsp->lftpos;
						lastkl = iwsp->rgtpos;
					} else if (iwsp->lftpos > kol1)
						kol1 = iwsp->lftpos;
				for (iwsp=biwsp; iwsp<tiwsp; iwsp++) {
					while (iwsp->rgtpos < kol1) {
						if (nextinterval(iwsp) != 0)
							goto firstfinished;
						if (iwsp->linpos != l) {
							l = iwsp->linpos;
							goto jumpline;
						}
						if (iwsp->lftpos > kol1)
							change = 1;
					}
				}
			} while (change != 0);
			for (iwsp=biwsp; iwsp<tiwsp; iwsp++)
				if (iwsp->rgtpos <= lastkl) {
					lastkl = iwsp->rgtpos;
				}
				
			if (lastkl >= kol1) {
				itvl->ileft = kol1 - idom->kol1;
				itvl->iright = lastkl - idom->kol1;
				if (l == lwas) {
					nints++;
				} else {
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
				anyintersect = 1;
			}
			kol1 = lastkl+1;
		}

jumpline:
	;
	}

firstfinished:
/*
 *	If no intersecting intervals - clean up memory Malloc'd
 *	and return an empty object. Thsi should be handled more safely than a
 * NULL object particularly at the application level. bdp 8/5/87.
 */
	if (!anyintersect) {
#ifdef WTRACE
		fprintf(stderr,"no intersectn found, generating empty object\n");
#endif WTRACE
		freeobj(obj);
		Free(biwsp);
		return(makemain(1, makedomain(1,1,0,1,0), NULL, NULL, NULL ));
	}
	makeinterval(lwas,idom,nints,jtvl);
	for (j = lwas+1; j<=lastln; j++) {
				/*
				 * we would prefer makeinterval(j,idom,0,NULL) here but
				 * it has unfortunate side-effects in "ibound" and "label"
				 */
		makeinterval(j,idom,0,itvl);
	}

	/*
	 * standardise the interval list (remove leading and trailing
	 * empty lines, set kol1 so that at least one interval commences
	 * at zero, set lastkl correctly)
	 */
	standardidom(idom);

	if (uvt != 0) {
		gwsp = (struct gwspace *) Malloc (n * sizeof (struct gwspace));
		/*
		 * construct an empty greytable
		 */
		obj->vdom = newvaluetb(obj,objs[0]->vdom->type,objs[0]->vdom->bckgrnd);
		obj->vdom->linkcount++;	
		/*
		 * fill the grey table with mean of grey values.
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
				gv = 0;
				for (iwsp=biwsp,i=0; iwsp<tiwsp; iwsp++,i++) {
					while (iwsp->linrmn >= 0 && (iwsp->linpos < l || (iwsp->linpos == l && iwsp->rgtpos < k)))
						nextgreyinterval(iwsp);
					gv += *(gwsp[i].grintptr + k - iwsp->lftpos);
				}
				*greyptr++ = gv / n;
			}
		}
		Free(gwsp);
	}
	Free(biwsp);
	return(obj);
}
