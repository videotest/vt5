/*
 * label.c
 *
 * C O P Y R I G H T  (c)  Medical Research Council 1986
 *
 * label - segment an object into connected components
 * This algorithm originated in SDS Fortran around 1970.
 * It uses a single pass through the input object and is thus
 * probably extendable to the case of labelling interval sets
 * produced by continuous scanning systems.
 *
 * Major re-write of the earlier C version to make use of non-Fortran-like
 * facilities provided by C -	Jim Piper	27-11-86.
 * Significant improvements: Allocated space can be expanded if it
 * proves inadequate.  Chained data is now properly structured.
 *
 *
 * Modifications
 *
 *	31 Jan 1991		CAS			Bug in monotone join fixed (generated non-monotone list)
 *	15 Feb 1989		CAS/SEAJ	Return status if insufficent space in objlist.
 *								Remove error message (should be handled by caller)
 *	18 Nov 1988		dcb			woolz_check_obj() instead of wzcheckobj()
 *	03 Dec 1987		BDP			Protect against single pixel objects which crash it
 *	08 May 1987		BDP			protection against null or empty objs
 *	 2 Mar 1987		GJP			Woolz_exit
 *	 6 Feb 1987		JIMP/CAS	Bug fixes when buckling more store - CAS fix was
 *								overkill, some variable renaming improves legibility.
 *								A few more comments as well.
 *	23 Feb 1987		JIMP		Bug fix to allocation list size.
 *
 *
 * The routines which manipulate linked lists all take as argument the
 * free-list base address.  This should allow simple future extension to
 * using more than one list, e.g. lists with different sized links
 * for pointers and for intervals.
 * The only really devious bits of coding are in procedures "merge"
 * and "monotonejoin" where in each case,
 * as in the Fortran original, the two objects being merged may be
 * reversed depending on which has the lower first line number
 * or interval end point respectively.
 */
#include <stdio.h>
#include <wstruct.h>

/*
 * The allocation list contains the interval end-points of the intervals of
 * the current and previous lines.  Its length therefore needs to be twice the
 * maximum number of intervals in a line of the input object (see mintcount()).
 */
struct allocbuf {
	struct link *a_link;
	struct interval a_int;
};

/*
 * A chain-link is used to hold either data (line number, interval
 * end points) or a pointer to another chain.  Do not confuse this
 * latter use with the pointer which points to the next link in
 * the same chain.
 */
struct link {
	struct link *l_link;
	union {
		int line;
		struct link *u_link;
		struct interval intv;
	} 
	l_u;
};

extern SMALL jdqt;

/*
 *	L A B E L  --  Extract objects from a larger object
 *
 * If given a null object will just return a count of zero in mm and zero
 * as its return value. If insufficient space is available in the objlist
 * it will return the number segmented in mm but zero as return value for
 * the calling procedure to sort out.
 *
 *	obj		- orig object
 *	mm		- returned number of objects generated
 *	objlist	- where to write derived objects
 *	nobj	- Space avail to write objects in objlist
 *	ignlns	- ignore objects less than size
 */
label (obj,mm,objlist,nobj,ignlns)
struct object *obj;
struct object **objlist;
int *mm;
{ 
	struct intervaldomain *jdp, *makedomain();
	struct valuetable *jvp;
	struct object *makemain();
	struct iwspace iwsp;
	register struct interval *itvl;
	struct interval *jtvl;
	register struct allocbuf *crntal, *altemp;
	struct allocbuf *al, *crntst, *crlast;
	struct allocbuf *precal, *precst, *prlast;
	register int jjj, nints, mkl;
	int maxinline,nob,line,ended,lend,chainlistsize;
	struct link *freechain, *alprec, *alloc, *link1, *link2;
	struct link *newchain1(), *newchain2(), *addlink1(), *addlink2();
	struct link *chainalloc();
	int lftcrn, lftprc, ofl, oll, rtcrn, rtprec, jrtcrn, mxkl, jl, jr;
#ifdef DEBUG
	int		lastleft;
#endif DEBUG

	/*----- COMMENTS FROM FORTRAN VERSION - HELPFUL, BUT BEWARE -----*/
	/* al - allocation list and limits of intervals */
	/* alloc-pointer to object to which current interval allocated or */
	/* the limits of the interval.ie alloc=al(crntal,1) or al(crntal,2) */
	/* alprec-similar to alloc alprec=al(precal,1) or al(precal,2) */
	/* allocate the intervals in current line to objects */
	/* crlast-pointer to last interval in current line in allocation list */
	/* crntal-pointer to current interval in current line in allocn list */
	/* crntst-starting point in allocation list for current line */
	/* ignlns-an object is not ignored if it has more than this number of lines */
	/* and columns (j.p. 14/2/81) */
	/* jdqt and jjdqt-chose the diagonal or square topology */
	/* lftcrn-left end of current object interval in current line */
	/* lftprc-left end of current object interval in preceding line */
	/* line-section line currently being considered */
	/* ll-last line of section */
	/* mm - number of objects found */
	/* nob-pointer to next free location in obj */
	/* nobj-dimension of array obj */
	/* ofl-first line of object */
	/* oll-last  line of object */
	/* precal-pointer to current interval in preceding line in alloc list */
	/* precst-starting point in allocation list for preceding line */
	/* prlast-pointer to last interval in preceding line in allocn list */
	/* rtcrn-right end of current object interval in current line */
	/* rtprec-right end of current object interval in preceding line */

	/* if any problems with supplied object return 0 */
	*mm = 0;
	if (woolz_check_obj(obj, "label") != 0 )
		return(0);
	if ( (wzemptyidom(obj->idom) > 0) || (obj->idom->type != 1) )
		return(0);

	/*
	 * Allocate and initialise the working spaces.
	 *
	 * The chain store size is not easily predictable.
	 * Here we use ((twice the number of lines) plus (one third the
	 * number of intervals)), very much smaller than the
	 * previous allocation, but apparently adequate to
	 * segment both metaphase spreads and single connected objects.
	 * In any case, this version of label can if necessary allocate
	 * more space when the original chain store fills up.
	 */

	chainlistsize = mintcount(obj->idom,&maxinline) / 3;
#ifdef DEBUG
	fprintf(stderr,"label: max int/line %d\n",maxinline);
#endif DEBUG		 
	chainlistsize += (obj->idom->lastln - obj->idom->line1 + 1)*2;
	freechain = chainalloc(0,chainlistsize);
	buckle(freechain,freechain,chainlistsize);
	/*
	 *	The allocation list holds 2 lines of data, current + prev. We read a
	 *	line of intervals into here to process. At the end of each line we
	 *	logically swap the halves so the old current is the new prev and we
	 *	read the next line in. Size is determined as twice (maximum number
	 *	of intervals in a line of input object, plus one).
 	 */
	al = (struct allocbuf *) Malloc(2 * (maxinline+1) * sizeof(struct allocbuf));
	jvp = obj->vdom;

	/* initialise interval scanning */
	initrasterscan (obj, &iwsp, 0);
	line = iwsp.linpos;
	ended = 0 ;
	lend = 0 ;
	nextinterval(&iwsp);
	crntst = al + 1 + maxinline;
	nob = 0 ;
	prlast = al ;
	precst = al + 1 ;

	/* commence new line */
	while (!ended) {
		crntal = crntst-1 ;

		line++;
		if (lend) { 
			ended = 1 ;
			lend = 0 ;
		} else {
			/* read the intervals into the allocation buffer */
#ifdef DEBUG
			lastleft = -9999;
#endif DEBUG
			while (iwsp.linpos <= line) {
				crntal++;
				crntal->a_int.ileft = iwsp.lftpos;
				crntal->a_int.iright = iwsp.rgtpos;
#ifdef DEBUG
				if (iwsp.lftpos < lastleft)
					fprintf(stderr,"label: input sequence error %d %d\n",lastleft,iwsp.lftpos);
				lastleft = iwsp.lftpos;
#endif DEBUG
				if (nextinterval(&iwsp) != 0) {
					lend = 1 ;
					break ;
				}
			}
		}
/*
 *	Work through current line checking linkage to previous
 */
		crlast = crntal ;
		crntal = crntst ;
		precal = precst ;
		alloc = NULL ;

		/* test whether last interval in current line dealt with */
		while (crntal <= crlast) {
			lftcrn = crntal->a_int.ileft ;
			jrtcrn = crntal->a_int.iright ;
			rtcrn = jrtcrn+2 ;
			alprec = precal->a_link;
			lftprc = precal->a_int.ileft ;
			rtprec = precal->a_int.iright +2 ;

			if (precal > prlast || rtcrn <= lftprc+jdqt) {  /* Past end of prev line ? */
				/* is interval in current line already allocated */
				if (!alloc) { 
					/* start a new object and allocate this interval to it */
					/* interval list */
					link1 = newchain2(freechain,lftcrn,jrtcrn);
					/* line-of-intervals list */
					link2 = newchain1(freechain,link1);
					/* "object" -- first line, last line, line-of-intervals pointer */
					crntal->a_link  = newchain1(freechain,line) ;
					crntal->a_link = addlink1 (freechain, crntal->a_link, line);
					crntal->a_link = addlink1 (freechain, crntal->a_link, link2);
				}
				/* move on to next interval in current line */
				crntal++ ;
				alloc = NULL;
			}
			else {									/* Potential overlap */
				if (rtprec > lftcrn+jdqt) { 
					/* case of overlapping intervals: */
					/* is intvl in current line already allocated ? */
					if (!alloc) { 
						/* allocate this interval and add to object list */
						alloc = alprec ;
						link1 = alloc->l_link ;
						crntal->a_link = alloc ;
						/* test whether this line has already been started for this object */
						if (link1->l_u.line != line) {		/* First interval in this object on this line */
							/* update last line */
							link1->l_u.line = line;
							/* add a link to the line list for this line */
							link1 = newchain2(freechain,lftcrn,jrtcrn);
							alloc->l_u.u_link = addlink1 (freechain, alloc->l_u.u_link, link1);
						}
						else {								/* Subsequent interval for this object on this line */
							/* add interval to interval list for last line */
							link1 = alloc->l_u.u_link;
							link1->l_u.u_link = addlink2 (freechain, link1->l_u.u_link, lftcrn, jrtcrn);
						}
					}
					else {									/* This interval spans more than 1 in prev line */
						/* merge lists and reallocate intervals */
						/* test whether both already allocated to same object */
						if (alprec != alloc) { 
							merge (&alprec, &alloc, freechain);
							/* reallocate intervals in preceding line */
							for (altemp = precst; altemp <= prlast; altemp++)	{
								if (altemp->a_link == alloc) { 
									altemp->a_link = alprec ;	
								}	
							}
							/* Fix cur + prev line buffers while we process rest of the line (and next) */
							/* reallocate intervals in current line */
							for (altemp = crntst; altemp <= crntal; altemp++)	{
								if (altemp->a_link == alloc) { 
									altemp->a_link = alprec ;	
								}	
							}
							alloc = alprec ;
						}
					}
					if (rtcrn < rtprec) { 
						/* move to next interval in this line */
						crntal++;
						alloc = NULL ;
						continue;	/* the outer while loop */
					}
				}
				/* move on to next interval in preceding line */
				precal++ ;
			}
		}

		/* all intervals in current line dealt with: */
		/* find and construct any finished objects */
		if (precst <= prlast) { 
			for (precal = precst; precal <= prlast; precal++) {
				alprec = precal->a_link ;
				/* has the object to which this interval was allocated been dealt with */
				if (alprec) { 
					/* remove any later intervals allocated to the same object */
					for (altemp = precal; altemp <= prlast; altemp++) {
						if (altemp->a_link == alprec) { 
							altemp->a_link  = NULL ;	
						}	
					}
					/* test if this object has intervals in the current line */
					/* and if so skip to end of outer loop */
					if (crntst <= crlast) { 
						for (altemp = crntst; altemp <= crlast; altemp++)	{
							if (altemp->a_link == alprec)
								goto loopend;
						}
					}
					/* construct object - first find line and column bounds */
					link1 = alprec->l_link;
					oll = link1->l_u.line;		/* Lastline */
					link1 = link1->l_link;
					ofl = link1->l_u.line;		/* firstline */
					link1 = alprec->l_u.u_link;
					nints = 0;
				/*
				 * Size object - get col1, lastcol, nints
				 */
					for (jjj=ofl; jjj<=oll; jjj++) {
						link1 = link1->l_link;
						link2 = link1->l_u.u_link;
						do {
							link2 = link2->l_link;
							jl = link2->l_u.intv.ileft;
							jr = link2->l_u.intv.iright;
							if (nints == 0 || jl < mkl)
								mkl = jl ;	
							if (nints == 0 || jr > mxkl)
								mxkl = jr ;	
							nints++;
							/* test for end of line */
						} 
						while (link2 != link1->l_u.u_link);
					}
					/* test whether object large enough, if not ignore it */
					/* test for height or width less than threshold + deallocate chain space if not */
					if (oll-ofl < ignlns || mxkl-mkl < ignlns) {
						for (jjj = ofl; jjj <= oll; jjj++)	{
							link1 = link1->l_link;
							link2 = link1->l_u.u_link;
							/* recover chain space */
							join (freechain, link2) ;
						}
					}
					else {
						if (nob >= nobj) {		/* test for object array overflow */
							Free(al);			/* Free everything + return what we have already found if no more space */
							chainfree();
							*mm = nob;
							return(0);
						}
						link1 = alprec->l_u.u_link;
						/* set up domain and object, and update counts */
						jdp = makedomain(1,ofl,oll,mkl,mxkl);
						*objlist = makemain(1,jdp,jvp,NULL,obj);
						objlist++;
						nob++;
						/* get the size correct here !!! */
						itvl = (struct interval *) Malloc (nints * sizeof(struct interval));
						jdp->freeptr = (char *) itvl;
#ifdef OSK
						jdp->linkcount = 1;
#endif OSK
						/* write intervals and interval pointers lists */
						for (jjj=ofl; jjj<=oll; jjj++) {
							jtvl = itvl;
							nints = 0;
							link1 = link1->l_link;
							link2 = link1->l_u.u_link;
#ifdef DEBUG
							lastleft = -9999;
#endif DEBUG
							do {
								link2 = link2->l_link;
								itvl->ileft = link2->l_u.intv.ileft - mkl;
								itvl->iright = link2->l_u.intv.iright - mkl;
#ifdef DEBUG
								if (itvl->ileft < lastleft)
									fprintf(stderr,"label int sequence %d %d\n",lastleft,itvl->ileft);
								lastleft = itvl->ileft;
#endif DEBUG
								itvl++;
								nints++;
								/* test for end of line */
							} while (link2 != link1->l_u.u_link);
							makeinterval(jjj,jdp,nints,jtvl);
							join (freechain, link2) ;
						}
					}
					/* return line list etc to free-list store */
					join (freechain, alprec->l_u.u_link);
					join (freechain, alprec) ;
				}
loopend:			 
				;
			}
		}

		/* update pointers - swap the two halves of the allocation list */
		/* before getting the intervals in the next line */
		altemp = crntst ;
		crntst = precst ;
		precst = altemp ;
		prlast = crlast ;
	}
	*mm = nob ;
	Free(al);
	chainfree();
	return(nob);
} 


/*
 * make a new chain: extract a link from the free-list pointed to by
 * "chainbase", link it to itself, add appropriate entries.
 * Newchain1 will handle single scalar and pointer entries so long
 * as the C compiler has pointers and integers the same length (this
 * is a language requirement).
 */
static
struct link *newchain1(chainbase,entry)
struct link *chainbase;
{
	register struct link *newlink;
	struct link *getlink();
	newlink = getlink(chainbase);
	newlink->l_link = newlink;
	newlink->l_u.line = entry;
	return(newlink);
}

/* Newchain2 is for intervals */
static
struct link *newchain2(chainbase,entry1,entry2)
struct link *chainbase;
{
	register struct link *newlink;
	struct link *getlink();
	newlink = getlink(chainbase);
	newlink->l_link = newlink;
	newlink->l_u.intv.ileft = entry1;
	newlink->l_u.intv.iright = entry2;
	return(newlink);
}

/*
 * Add to a chain: extract a link from the free-list pointed to by
 * "chainbase", link it to chain, add appropriate entries.
 * Addchain1 will handle single scalar and pointer entries so long
 * as the C compiler has pointers and integers the same length (this
 * is a language requirement).
 */
static
struct link *addlink1(chainbase,chain,entry)
register struct link *chainbase, *chain;
{
	register struct link *newlink;
	struct link *getlink();
	newlink = getlink(chainbase);
	newlink->l_link = chain->l_link;
	chain->l_link = newlink;
	newlink->l_u.line = entry;
	return(newlink);
}

/* Addchain2 handles intervals */
static
struct link *addlink2(chainbase,chain,entry1,entry2)
register struct link *chainbase, *chain;
{
	register struct link *newlink;
	struct link *getlink();
	newlink = getlink(chainbase);
	newlink->l_link = chain->l_link;
	chain->l_link = newlink;
	newlink->l_u.intv.ileft = entry1;
	newlink->l_u.intv.iright = entry2;
	return(newlink);
}

/*
 * allocate chain list and store information in static structure
 *
 * The size of the chain link is not easily predictable in advance.
 * We therefore permit extension of the chain list by malloc() calls,
 * and require a structure to store a sensible chain list size
 * increment.  Allocated chunks of chain list are themselves chained
 * together (using the first chain location)
 * in order to permit space-freeing; the original base of the chain
 * and most recent chunk base are also stored in this structure.
 */
struct allocthings {
	int allocsize;
	struct link *orig_base;
	struct link *chunk_base;
};
static struct allocthings allocthings;

static
struct link *chainalloc(flag,n)
{
	struct link *chain;
	/*
	 * memory allocation size determined by first allocation
	 */
	if (flag)
		n = allocthings.allocsize;
	else
		allocthings.allocsize = n;
	/*
	 * allocate memory, chain to existing allocated memory
	 */
	chain = (struct link *) Malloc((n+1) * sizeof(struct link));
	chain->l_link = NULL;
	if (flag) {
		allocthings.chunk_base->l_link = chain;
		allocthings.chunk_base = chain;
	}
	else {
		allocthings.orig_base = allocthings.chunk_base = chain;
	}
	/*
	 * retain first link for chaining allocated memory chunks
	 */
	return(chain+1);
}

static
chainfree()
{
	freechain(allocthings.orig_base);
}

static
freechain(l)
struct link *l;
{
	if (l != NULL) {
		freechain(l->l_link);
		Free(l);
	}
}

/* get an unused link from the free chain store */
static
struct link *getlink(chainbase)
register struct link *chainbase;
{
	register struct link *newlink;
	struct link *chainalloc();
	newlink = chainbase->l_link;
	if (newlink ==chainbase) {
		newlink = chainalloc(1,0);
		buckle(chainbase,newlink,allocthings.allocsize);
	}
	/*
	 * set chainbase link pointer to the NEXT free link,
	 * and return the link that has been found
	 */
	chainbase->l_link = newlink->l_link;
	return(newlink);
}

/*
 * Link up a new section (perhaps the first) of the chain store.
 * The base of the chain is inserted at the bottom of the new section,
 * then bubbled up as the chain is linked so that at the end the last
 * link points back to the base of the chain.
 */
buckle (chainbase,newlinkbase,length)
struct link *chainbase;
register struct link *newlinkbase;
register int length;
{
	register struct link *lin, *linwas;
	newlinkbase->l_link = chainbase;
	lin = newlinkbase;
	for (; length > 1; length--) {
		linwas = lin++;
		lin->l_link = linwas->l_link;
		linwas->l_link = lin;
	}
}


/* merge chain list2 into chain (e.g. free-list) list1 */
static
join (list1, list2)
register struct link *list1, *list2;	
{ 
	register struct link *last1;
	if ( list1 != NULL && list2 != NULL) { 
		last1 = list1->l_link;
		list1->l_link = list2->l_link ;
		list2->l_link = last1 ;
	}
} 


/* merge two objects whose reference lists begin at k1,k2 into */
/* one object beginning at k1 */
static
merge (ak1, ak2, freechain)
struct link **ak1, **ak2, *freechain; 
{ 
	register struct link *intvln1, *intvln2, *k1;
	struct link *nk1, *nnk1, *iplp1;
	struct link *k2, *nk2, *nnk2, *iplp2;
	register int more, i, ll1, ll2;
	int fl1, fl2;
	do {
		k1 = *ak1 ;
		nk1 = k1->l_link;
		nnk1 = nk1->l_link;
		/* first line of k1 */
		fl1 = nnk1->l_u.line;

		k2 = *ak2 ;
		nk2 = k2->l_link;
		nnk2 = nk2->l_link;
		/* first line of k2 */
		fl2 = nnk2->l_u.line;

	/*
	 * if k1's first line is greater than k2's first line then
	 * the objects are interchanged so that k2 absorbs k1 -
	 * this is why k1 and k2 are passed by address.
	 */
		more = fl2-fl1 ;
		if (more < 0) {
			*ak1 = k2;			/* Swap them + go round `do' loop again - yuk */
			*ak2 = k1;
		}
		else {

			/* last line of k1 */
			ll1 = nk1->l_u.line;
			/* interval pointer list pointer for first object */
			iplp1 = k1->l_u.u_link;
			/* last line of k2 */
			ll2 = nk2->l_u.line;
			/* interval pointer list pointer for second object */
			iplp2 = k2->l_u.u_link;

			/*
						 * add extra location to interval pointers
						 * list of shorter object - of course, "last lines"
						 * of objects being merged will differ by exactly one.
						 */
			if (ll1 > ll2) { 
				iplp2 = addlink1 (freechain, iplp2, 0);
				k2->l_u.u_link = iplp2 ;
				nk2->l_u.line = ll1 ;
			} 
			else if (ll1 < ll2) {
				iplp1 = addlink1 (freechain, iplp1, 0);
				k1->l_u.u_link = iplp1 ;
				nk1->l_u.line = ll2 ;
			}
			intvln1 = iplp1->l_link;
			intvln2 = iplp2->l_link;

			for (i=0; i<more; i++)
				intvln1 = intvln1->l_link;

			/* chain lists for corresponding lines are 'monotone-joined' until list */
			/* of pointers to interval lists for object with fewer lines is exhausted */
			do {
				monotonejoin (&(intvln1->l_u.u_link), intvln2->l_u.u_link);
				intvln1 = intvln1->l_link;
				intvln2 = intvln2->l_link;
			} 
			while (intvln1 != iplp1->l_link);
			/* return space to free list */
			join (freechain, k2) ;
			join (freechain, iplp2) ;
			return (0) ;
		}
	} 
	while (more < 0);
} 


/*
 * merge two monotone chainlists (of intervals) into one monotone chain.
 * The address of the combined list is returned via amlist1.
 * Magnitude of link determined by left end point of link interval
 */
static
monotonejoin (amlist1, mlist2)
struct link **amlist1, *mlist2 ;	
{ 
	register int mlist;
	struct link *mlist1;
	register struct link *l1, *nl2, *l;

	mlist1 = *amlist1;
/*
 * if first chain is empty the join consists of the second only
 */
	if (mlist1 == NULL) {
		*amlist1 = mlist2;
		return(0);
	}
/*
 * if second chain is empty the join consists of the first only
 */
	if (mlist2 == NULL)
		return(0);
#ifdef DEBUG
	fprintf(stderr,"monotone join:\n");
	showlinks(mlist1,1,"1");
	showlinks(mlist2,1,"2");
#endif DEBUG
/*
 *	if mlist1 consists of single link swap it to be second list
 */
	if (mlist1 == mlist1->l_link) {
		l = mlist1;
		*amlist1 = mlist1 = mlist2 ;
		mlist2 = l;
	}
	l1 = mlist1;
	mlist = 1;
/*
 *	Merge the lists monotonically. When all links of mlist2 have been added
 *	to mlist1, exit with amlist1 as address of combined list
 */
	while (mlist) {
		nl2 = mlist2->l_link;
		if (nl2 == nl2->l_link)
			mlist = 0 ;	
		if (nl2->l_u.intv.ileft < mlist1->l_u.intv.ileft) {	/* Link in middle ? */
			for (;;) {
				l = l1->l_link;
				if (nl2->l_u.intv.ileft < l->l_u.intv.ileft)
					break;
				l1 = l;
			}
		} else							/* Else link at end of list */
			l1 = mlist1;
		mlist2->l_link = nl2->l_link;	/* detach first link from mlist2 */
		nl2->l_link = l1->l_link;		/* insert it in correct place */
		l1->l_link = nl2;
		if (nl2->l_u.intv.ileft >= mlist1->l_u.intv.ileft) {
			*amlist1 = mlist1 = nl2 ;
			l1 = mlist1 ;
		} else
			l1 = l1->l_link;
	}
#ifdef DEBUG
	showlinks(mlist1,0,"E");
#endif DEBUG
	return(0);
} 

/*
 * revised version of library intcount().
 * (i) only takes case of type 1 interval domain,
 * (ii) returns maximum-intervals-in-a-line
 */
static
mintcount(idom,maxinline)
struct intervaldomain *idom;
int *maxinline;
{
	register ll,intcount,max;
	register struct intervalline *intl;
	intl = idom->intvlines;
	max = intcount = 0;
	ll = idom->lastln - idom->line1;
	for ( ; ll >= 0; ll--) {
		if (intl->nintvs > max)
			max = intl->nintvs;
		intcount += (intl++)->nintvs;
	}
	*maxinline = max;
	return(intcount);
}

#ifdef DEBUG
/*
 *	S H O W L I N K S  --  Show links in a chain list
 *
 *	all: if false only show list if it is not monotone
 */
showlinks(mlist,all,tag)
struct link *mlist;
int		all;
char	*tag;
{
	struct link *l;
	int		er;

	l = mlist->l_link;
	if (all) {
		fprintf(stderr,"\t\t%s:",tag);
		while (l != mlist) {
			fprintf(stderr," %d",l->l_u.intv.ileft);
			l = l->l_link;
		}
		fprintf(stderr," %d\n",l->l_u.intv.ileft);
	} else {
		er = 0;
		while (l != mlist) {
			if (l->l_u.intv.ileft > l->l_link->l_u.intv.ileft)
				er = 1;
			l = l->l_link;
		}
		if (er) {
			l = mlist->l_link;
			fprintf(stderr,"\t\t%s:",tag);
			while (l != mlist) {
				fprintf(stderr," %d",l->l_u.intv.ileft);
				l = l->l_link;
			}
			fprintf(stderr," %d\n",l->l_u.intv.ileft);
		}
	}
}
#endif DEBUG
