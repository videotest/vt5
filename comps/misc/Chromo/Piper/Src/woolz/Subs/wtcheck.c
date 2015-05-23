/*
 *      W T C H E C K . C  --  woolz object checking routines
 *
 *
 *  Written: B. Pinnington
 *           Image Recognition Systems
 *           720 Birchwood Boulevard
 *           Birchwood Science Park
 *           Warrington
 *           Cheshire
 *           WA3 7PX
 *
 *	Copyright (c) and intellectual property rights Image Recognition Systems (1988)
 *
 *  Date:    10th January 1988
 *
 *	Description
 *			Several routines are provided for checking a whole
 *			object, or just checking a domain. In all cases
 *			the result is returned as zero for correct, and
 *			non zero for any error.
 *			In this version, used for the validation testing
 *			for the woolz library an extensive object checking
 *			for all object and domain types has been included.
 *			This will be used as a program filter after each
 *			unit module test, or as the output tester in the
 *	 		automatic test loops. A summary of each object
 *			is sent to wz_db_fp, plus highlighted error messages.
 *
 *  Modifications
 *
 *	 1 Dec 1988		CAS		Move woolzexitfn back to application..
 *	30 Nov 1988		SCG/dcb	bug in idom->intvlines->intvs range checking fixed
 *	29 Nov 1988		SCG/dcb	allocsize mods in light of fact that Alloc_size does
 *							not return actual size requested by Malloc or Calloc
 *							Also completely reshuffled tests for type 21 vdom
 *	28 Nov 1988		SCG/dcb	Separate interval domain is not an error (see shift)
 *	28 Nov 1988		SCG/dcb	handle checking of idom type 2
 *	24 Nov 1988		SCG/dcb objects can have NULL (idom, vdom) freeptrs;
 *							modified (not properly) the tests to cope.  In the
 *							future these must be written to take into account 
 *							linkcount when looking at freeptrs.
 *	23 Nov 1988		SCG/dcb	woolz_check_exit used to report pointer on which error
 *							detected, then call woolz_exit
 *	22 Nov 1988		dcb		Added 'called_from' parameter to ALL check functions
 *							to indicate where they are called from if an error
 *							occurs. This is passed out via woolz_exit, which
 *							will cope with the extra parameter.
 *							NOTE: Where woolz_check_obj() calls sub functions,
 *							it passes the name of the function that called
 *							woolz_check_obj(), not woolz_check_obj. Similarly
 *							for the recursive function woolz_check_blist()
  *	18 Nov 1988		SCG/DCB	Introduced nested checking so that 
 *							these routines may be used in the application code
 *							woolz_check_level(), and woolz_check_debug()
 *							controls this. woolzexitfn() is defined here
 *	12 Aug 1988		CAS		Cleaned up - put std header on etc.
 *	18 Nov 1987		BDP		Replace memctrl access with Alloc_size calls
 *							and extended memory pointer checking
 *							removed wtemptyidom and wtemptypdom
 *	17 Nov 1987		BDP		Had to remove some checks on unused fields
 *							in chord structures.
 *	13 Nov 1987		BDP		More Yuk, Change the histogram space check
 *							to cope with hdom and points being in the
 *							same or different blocks. This strictly
 *							should be applied to all domain structures
 *							but better still to define a consistent
 *							standard for access to domain arrays!!
 *	10 Nov 1987		BDP		Yuk, Yuk, Yuk !! Had to change the boundlist
 *							chain consistency check because ibound and
 *							readobj fail to fill out the up ponters!!
 *							As interim fix for this check the memory
 *							block header for sensible values ! This is
 *							now assuming that all boundlists are from
 *							separate callocs ! Yuk, Yuk, Yuk !!
 *	10 Nov 1987		BDP		Put stack check into boundlist check.
 *	26 Oct 1987		BDP		Grey table gap checking checks idom for
 *							nitvs of zero, cos of drop off in std vdom
 *							where cannot know a line is empty!!
 *	15 Oct 1987		BDP		Extended to do a mega checking of objects
 *
 */
#include <stdio.h>
#include <wstruct.h>
#include <wtcheck.h>
#include <wtcklims.h>

# define IN 0
# define OUT 1
/*
 * statics
 */
static FILE *wz_db_fp;		/* set in woolz_debug */
static int check_level;		/* level of checking */

/* 
 * general space checking variables 
 */
static int	ecount, 	/* calculated count of elements of type being tested */
		acount,		/* count of allocated elements of type being tested */
		allocsize;	/* size of current memory block */
/*
 *
 *	Function:		woolz_check_obj
 *
 *	Purpose:		check consistency of supplied object
 *
 *	Description:	Checks the object for being null, or for its first
 *				domain pointer being null. Also checks that type is
 *				legal. Any failure is reported with unique status
 *				Function can be further context expanded via the
 *				the case table established. Logging of errors and
 *				handling of fatal errors may be added later
 *
 *	Entry:			object to be checked
 *
 *	Exit:			0 for correct,  non-zero for error status
 *
 */
int 
woolz_check_obj(obj, called_from)
struct object *obj;
char *called_from;
{
	register int objstatus = 0;
	
/*----*/if(check_level < 1) return(0);

	
	if (obj == NULL)
		objstatus = CKNULL;
	else {
		switch (obj->type) {
			case 1:		/* interval structured object */
			case 2:
				if ((objstatus = woolz_check_idom(obj->idom, called_from))==0) {
					if (obj->vdom != NULL)
						objstatus =  woolz_check_vdom(obj, called_from);
				}
				break;
			case 10:	/* polygon object */
				if (obj->idom == NULL)
					objstatus =  PDMNULL;
				else
					objstatus =  woolz_check_pdom(obj->idom, called_from);
				break;
			case 11:	/* Boundary list object */
				if ((obj->vdom != NULL) || (obj->plist != NULL))
					objstatus =  BDLEDPTR;
				else {
					objstatus =  woolz_check_blist(obj->idom, called_from);
				}
				break;
			case 12:
				if (obj->idom == NULL)
					objstatus =  PDMNULL;
				else {
					objstatus =  woolz_check_pdom(obj->idom, called_from);
					if ( objstatus == 0 ) {
						if (obj->vdom == NULL)
							objstatus = CVDNULL;
						else
							objstatus = woolz_check_cvhdom(obj->vdom, called_from);
					}
				}
				break;
			case 13:
				if (obj->vdom != NULL)
					objstatus = HDMEDNULL;
				else
					if (obj->idom == NULL)
						objstatus = HDMNULL;
					else
						objstatus = woolz_check_hdom(obj->idom, called_from);
				break;
			case 20:
				if (obj->idom == NULL)
					objstatus = RDMNULL;
				else
					objstatus = woolz_check_rdom(obj->idom, called_from);
				break;
			case 30:
			case 31:
			case 40:
			case 41:
				break;		/* no check implemented yet for these cases */
			case 70:
				objstatus = woolz_check_tdom(obj, called_from);
				break;
 			case 110:
				if ((obj->idom != NULL) || (obj->vdom != NULL))
					objstatus = HDREDPTR;
				break;
			default:
				objstatus = CKINVTYPE;
		}
	}
	if(objstatus)
		return(woolz_check_exit(objstatus, called_from, obj));
	else
		return(0);
}

size_t Alloc_size(void *p)
{
	return _msize(p);
}

/*
 *
 *	Function:		woolz_check_idom
 *
 *	Purpose:		perform totally rigorous validation of interval dom
 *
 *	Description:	Performs a series of increasing complex tests on the
 *					interval domain and its associated structures to
 *					check with a very high level of confidence that no
 *					aspect of it will cause a problem. Has very
 *					intimate knowledge of data structures and ranges
 *					therefore, so should be updated whenever the woolz
 *					data structures are changed
 *					tests:
 *					1. Check for NULL or empty domain
 *					2. Type
 *					3. Linkcount range
 *					4. Domain extent
 *					5. Free ptr == first interval line ptr ( not mand.)
 *					6. Check intervals referenced sequentially ( not
 *						mandatory
 *					7. Interval coordinates range
 *					8. Check allocated domain memory not exceeded
 *					9. Check interval memory not exceeded.
 *					10. Check memory header blocks
 *					Also gives warnings if top or bottom lines have
 *					been standardised.
 *
 *	Entry:			Interval domain pointer
 *
 *	Exit:			Exit status of first error detected or 0 if o.k.
 *
 */
int
woolz_check_idom(idom, called_from)
register struct intervaldomain *idom;
char *called_from;
{
	struct intervalline *il;
	register struct interval *iv;
	register int i, j, nints, st;
	int inout, lines;

/*----*/if(check_level < 1) return(0);

	/* first check that its not empty */
	if (idom == NULL)
		return(woolz_check_exit(IDMNULL, called_from, idom));
	else
		if (idom->line1 > idom->lastln)
			return(woolz_check_exit(CKEMPTYDMN, called_from, idom));

/*----*/if(check_level < 2) return(0);

	/* check type */
	if ((idom->type != 1) && (idom->type != 2) )
		return(woolz_check_exit(IDMETYPE, called_from, idom));

	switch(idom->type) {
	case 1:
		/* 
		 * check link count is sensible 
		 */
		if ( (idom->linkcount < 0) || (idom->linkcount > CKIDMNLCMAX))
			return(woolz_check_exit(IDMELC, called_from, idom));
		/* 
		 * check the coordintate block 
		 */
		if ( ckcoord(idom->line1,idom->lastln,idom->kol1,idom->lastkl) > 0)
			return(woolz_check_exit(IDMECOORD, called_from, idom));
			
		il = idom->intvlines;
		/*
		 * now check that freeptr == first intervalline interval record
		 * this test only included as no instance of this is known, it is not
		 * illegal so much as likely to be due to a cock up somewhere
		 * This check is not valid for certain objects dilation/skel etc.
		 *
		 * if ((idom->freeptr != NULL) &&
		 *		(il->intvs != (struct interval *)idom->freeptr))
		 *	return(woolz_check_exit(IDMEFPTR, called_from, idom));
		 */

		allocsize = Alloc_size(idom);
		if (allocsize < 0)
			return(woolz_check_exit(allocsize, called_from, idom));
		/*
		 * This checks if the interval domain space is separate from
		 * the intervals themselves. This is a valid condition for some
		 * objects, such as made by shift
		 */
		if(abs((char *) il - (char *) idom) <= allocsize)
			inout = IN;
		else
			inout = OUT;
			
		switch (inout) {
		case IN:
			/* 
			 * check that allocated domain memory is not exceeded 
			 */
			ecount = idom->intvlines - (struct intervalline *)(idom + 1);
			if (ecount > 0)
				if(wz_db_fp)
					fprintf(wz_db_fp, "CHECK WARNING: %d lines skipped at start\n",ecount);

			lines = idom->lastln - idom->line1 + 1;
			ecount += lines;
			acount = (allocsize - sizeof(struct intervaldomain)) /
						sizeof(struct intervalline);
			if (ecount > acount) {
				if(wz_db_fp)
					fprintf(wz_db_fp, "CHECK DEBUG ERROR: %d lines found in %d byte space\n",
						ecount, allocsize );
				return(woolz_check_exit(IDMESIZE, called_from, idom));
			}
			if (ecount < acount)
				if(wz_db_fp)
					fprintf(wz_db_fp,"CHECK WARNING: %d lines unused at end\n",ecount);
			/*
			 *	Fall through to OUT case, check freeptr, and set allocsize
			 */
		case OUT:
			allocsize = Alloc_size(idom->freeptr);
			if (allocsize < 0 ) {
				if(allocsize == ALCNULL) {
					if(wz_db_fp)
						fprintf(wz_db_fp, "CHECK WARNING: NULL idom freeptr\n");
				} else
					return(woolz_check_exit(allocsize, called_from, idom));
			}
			break;
		default:
			break;
		}
		
/*----*/if(check_level < 3) return(0);

		/* 
		 * now check that interval structures are referenced sequentially 
		 */
		nints = 0;				/* count the intervals as we go */
		for (i = 0, iv = il->intvs; i < lines; i++) {
			nints += il->nintvs;
			/*
			 * this test is not valid for thinned/skeletonised/dilated/rotated
			 * objects
			 *
			 * if (il->intvs != iv)
			 *		return(woolz_check_exit(IDMEIVPTR, called_from, idom));
			 * iv += il->nintvs;
			 */
			il++;				/* next interval line structure */
		}
	
		switch (inout) {
		case IN:
			if ((allocsize != ALCNULL) &&
				(sizeof(struct interval) * nints > allocsize)) {
				if(wz_db_fp)
					fprintf(wz_db_fp, "CHECK: Interval allocated space exceeded\n");
				return(woolz_check_exit(IDMEFSIZE, called_from, idom));
			}
		case OUT:
			/* 
			 * check that the interval coordinates are sensible 
			 * obtain interval space ptr as below, rather than using freeptr as the latter
			 * is legitimately NULL for some objects (mol for example)
			 */
			for (i=0, il=idom->intvlines; i<lines; i++, il++)
				for (j=0, iv=il->intvs; j<il->nintvs; j++, iv++)
					if (ckcoord(0,0,iv->ileft,iv->iright) > 0)
						return(woolz_check_exit(IDMEIVCOORD, called_from, idom));
			break;
		default:
			break;
		}
	case 2:
		/* 
		 * check link count is sensible 
		 */
		if ( (idom->linkcount < 0) || (idom->linkcount > CKIDMNLCMAX))
			return(woolz_check_exit(IDMELC, called_from, idom));
		/* 
		 * check the coordintate block 
		 */
		if ( ckcoord(idom->line1,idom->lastln,idom->kol1,idom->lastkl) > 0)
			return(woolz_check_exit(IDMECOORD, called_from, idom));
		allocsize = Alloc_size(idom);
		if (allocsize < 0)
			return(woolz_check_exit(allocsize, called_from, idom));
		break;
	default:
		break;
	}		
	return(0);			/* no problems */	
}

/*
 *
 *	Function:		woolz_check_pdom
 *
 *	Purpose:		Check a polygon domain data structure exhaustively
 *
 *	Description:	Performs a number of data integrity checks on the
 *					domain and returns the first encountered error.
 *					tests:
 *					1. Domain exists
 *					2. Type
 *					3. Empty
 *					4. Vertices count < max count
 *					5. vertices count not negative or > predefined limit
 *					6. Vertex coordinates ranges
 *					7. Allocated memory block not corrupted
 *					8. Allocated size not exceeded
 *
 *	Entry:			Polygon domain pointer
 *
 *	Exit:			Status of first detected error or 0 if o.k.
 *
 */
int 
woolz_check_pdom(pdom, called_from)
register struct polygondomain *pdom;
char *called_from;
{
	register struct ivertex *vx;
	register struct fvertex *fx;
	register int i;
	int vertexsize, inout;
		
/*----*/if(check_level < 1) return(0);

	if (pdom == NULL)
		return(woolz_check_exit(PDMNULL, called_from, pdom));

/*----*/if(check_level < 2) return(0);
	
	if ((pdom->type != 1) && (pdom->type != 2))
		return(woolz_check_exit(PDMETYPE, called_from, pdom));
	
	if (pdom->nvertices == 0)
		return(woolz_check_exit(CKEMPTYDMN, called_from, pdom));
	
	/* check the running count is less than the allocated count */
	if (pdom->nvertices > pdom->maxvertices)
		return(woolz_check_exit(PDMECNT, called_from, pdom));
	
	/* check that the counts are sensible anyway */
	if ( (pdom->nvertices < 0) || (pdom->nvertices > CKPDMNVXMAX) )
		return(woolz_check_exit(PDMEVCNT, called_from, pdom));

/*----*/if(check_level < 3) return(0);
		
	/* now check that all the vertex coodinates are sensible */
	switch(pdom->type)
	{
	case 1:
		vx = pdom->vtx;
		for ( i=0; i<pdom->nvertices; i++ ) {
			if ( ckcoord(vx->vtY, 0, vx->vtX, 0) )
				return(woolz_check_exit(PDMECOORD, called_from, pdom));
			vx++;
		}
		vertexsize = sizeof(struct ivertex);	/* for memory check later */
		ecount = pdom->vtx - (struct ivertex *)(pdom + 1); /* see if any skipped */
		break;
		
	case 2:
		fx = (struct fvertex *) pdom->vtx;
		for ( i=0; i<pdom->nvertices; i++ ) {
			if ( ckfcoord(vx->vtY, 0, vx->vtX, 0) )
				return(woolz_check_exit(PDMECOORD, called_from, pdom));
			fx++;
		}
		vertexsize = sizeof(struct fvertex);	/* for memory check later */
		ecount = ((struct fvertex *)pdom->vtx) - ((struct fvertex *)(pdom + 1)); /* see if any skipped */
		break;
	
	}	/* end switch */
	
	/* check that allocated domain memory is not exceeded */
	if (ecount > 0)
		if(wz_db_fp)
			fprintf(wz_db_fp, "CHECK WARNING: %d vertices skipped\n",ecount);

	allocsize = Alloc_size(pdom);
	if (allocsize < 0)
		return(woolz_check_exit(allocsize, called_from, pdom));
		
	if(abs((char *) pdom - (char *) pdom->vtx) <= allocsize)
		inout = IN;
	else
		inout = OUT;
	
	switch (inout) {
	case IN:
		ecount += pdom->nvertices;
		acount = (allocsize - sizeof(struct polygondomain)) / vertexsize;
		if (ecount > acount) {
			if(wz_db_fp)
				fprintf(wz_db_fp, "CHECK DEBUG ERROR: %d vertices found in %d byte space\n",
					ecount, allocsize );
			return(woolz_check_exit(PDMESIZE, called_from, pdom));
		}
		break;
	case OUT:
	default:
		break;
	}
	return(0);			/* everything looks o.k. */
}

/*
 *
 *	Function:		woolz_check_vdom
 *
 *	Purpose:		Validate a value table as far as possible
 *
 *	Description:	Performs extensive checking across all three types
 *					of values domain structure.
 *						Test the following:
 *					1. Linkcount
 *					2. Type
 *						For Standard Value domains
 *					s1. Line coordinates range
 *					s2. Vdom memory block
 *					s3. No overlap of vtblines and vtblines* arrays
 *					s4. Vtblines array does not exceed allocated space
 *					s5. Grey table memory block
 *					s6. Grey pointers all within grey table space
 *					s7. Intervalline column coordinates range
 *						For Rectangle Value Domains
 *					r1. Free pointer must = Grey pointer (see note)
 *					r2. Coordinates range
 *					r3. Memory block for grey table
 *					r4. Size of grey block not exceeded
 *					r5. Fully covers idom
 *						For Interval Structured value Domains
 *					i1. Interval line array starts after ivtable
 *					i2. Interval coordinates
 *					i3.	Grey pointers within grey table
 *					i4. Intervals sequential and properly addressed
 *					i5. Memory block for ivtable
 *					i6. size of vdom not exceeded
 *
 *	Entry:			N.B. an OBJECT pointer is supplied here
 *
 *	Exit:			Zero if o.k. or status of first detected error
 *
 */
int 
woolz_check_vdom(vobj, called_from)
struct object *vobj;
char *called_from;
{
	register struct valueline *vi;		/* value interval really */
	register struct valintline *vil;	/* interval line equivalent */
 	register struct valuetable *vdom;
	register struct ivtable *ivdom;
	struct rectvaltb *rvdom;
	struct intervaldomain *idom;
	struct intervalline *il;
	GREY *g;
	char *freetop, *valuetop; /* valuetop is top of value table */
	register int i, j, lines, nints, vdomsize;
	int inout, greysize;
		
/*----*/if(check_level < 1) return(0);

	vdom = vobj->vdom;
	idom = vobj->idom;

	if(idom == NULL)
		return(woolz_check_exit(IDMNULL, called_from, idom));
	
	if(vdom == NULL)
		return(woolz_check_exit(VDMNULL, called_from, vdom));
		
/*----*/if(check_level < 2) return(0);
	
	/* 1,2. First Check type and linkcount before structure casting */
	if ((vdom->linkcount < 0 ) || (vdom->linkcount > CKVDMNLCMAX))
		return(woolz_check_exit(VDMELC, called_from, vdom));
	
	switch(vdom->type){
	case 1:
		/* 
		 * s1. check line coordinates 
		 */
		if ( ckcoord(vdom->line1,vdom->lastln, 0 ,0) )
			return(woolz_check_exit(VDMSCOORD, called_from, vdom));
		
		lines = vdom->lastln - vdom->line1 + 1;
		vi = *vdom->vtblines;		/* set vi to point to first valueline */

		/* 
		 * s2. for the memory checks there should be two allocated blocks, one
		 * containing vdom, vl pointer array, and valuelines contiguously, and
		 * the other containing the grey values themselves 
		 */
		allocsize = Alloc_size(vdom);
		if (allocsize < 0)
			return(woolz_check_exit(allocsize, called_from, vdom));

		if(abs((char *) vdom->vtblines - (char *) vdom) <= allocsize)
			inout = IN;
		else
			inout = OUT;
					
		switch (inout) {
		case IN:
			/* 
			 * s3. The first element count calculation looks to see if any of the
			 * element type have been skipped, as there is a useless level of
			 * indirection built in here, this has to be done in two bits here. 
			 */
			ecount = vdom->vtblines - (struct valueline **)(vdom + 1);
			if (ecount > 0)
				if(wz_db_fp)
					fprintf(wz_db_fp, "CHECK WARNING: %d valueline ** elements skipped\n",ecount);
			
			/* 
			 * s4. Now just look at the valueline array and make sure that it doesnt
			 * run off the end of the allocated space 
			 */
			if (((char *)(vi + lines)) > ((char *) vdom ) + allocsize )
				return(woolz_check_exit(VDMSSIZE, called_from, vdom));
		
		case OUT:	
			/* 
			 * if a vdom was standardised then there would unused elements at the
			 * end of this array, it is impossible to tell the difference between 
			 * this and skipped elements at the start of the vlines array itself, so
			 * just check that theres no overlap which would be an error.  
			 */
			if ( (vdom->vtblines + lines) > ((struct valueline **)(vi)))
				return(woolz_check_exit(VDMSVSIZE, called_from, vdom));

			/* 
			 * s5. Next check that the grey memory block is alright 
			 * NULL vdom freeptr is not an error for some objects (mol for example)
			 */
			allocsize = Alloc_size(vdom->freeptr);
			if (allocsize < 0) {
				if(allocsize == ALCNULL) {
					if(wz_db_fp)
						fprintf(wz_db_fp, "CHECK WARNING: NULL vdom freeptr\n");
				} else
					return(woolz_check_exit(allocsize, called_from, vdom));
			}
			break;
		default:
			break;
		}

/*----*/if(check_level < 3) return(0);
		/*
		 * some objects can have NULL vdom->freeptr; these tests should not 
		 * depend on vdom->freeptr being defined, do this properly later.
 		 */
		if(vdom->freeptr == NULL) break;
		
		freetop = vdom->freeptr + allocsize;
		
		/* 
		 * s6. Now going to scan down the grey table with two pointers,
		 * one a grey pointer being incremented on the basis of grey count
		 * and the other updated from the interval records. Need to check that
		 * grey interval coordinates are sensible, and that grey pointers are 
		 * within allocated memory.  Use idom structure to spot unused lines
		 * cos std vdom has this vital information missing 
		 */
		il = (struct intervalline *) (idom + 1); /* like this cos of stdidom */
		for ( i=0; i<lines; i++) {
			if (il->nintvs > 0) {
				/* 
				 * s7 coordinate validation 
				 */
				if(ckcoord(0, 0, vi->vkol1, vi->vlastkl))
					return(woolz_check_exit(VDMSIVCOORD, called_from, vdom));
				if(((char *) vi->values < vdom->freeptr) ||
					((char *) vi->values > (freetop - vi->vlastkl + vi->vkol1 - 1)))
					return(woolz_check_exit(VDMSVPTR, called_from, vdom));
			}
			vi++;			/* address next value line */
			il++;			/* address next interval line */
		}
		break;

	case 11:
		rvdom = (struct rectvaltb *) vdom;
		/* 
		 * r1. N.B. Due to the facts that the rect value table has only one
		 * grey pointer not one per line, cannot have a rectangular grey table
		 * within another, consequence is ... freeptr must be equal to the
		 * grey pointer ! 
		 */
		if ((rvdom->freeptr != NULL) && 
			(rvdom->freeptr != (char *) rvdom->values))
			return(woolz_check_exit(VDMRFPTR, called_from, rvdom));
		/* 
		 * r3,r4. Check enough space exists and that memory block is ok 
		 */
		allocsize = Alloc_size(rvdom->freeptr);
		if (allocsize < 0) {
			if(allocsize == ALCNULL) {
				if(wz_db_fp)
					fprintf(wz_db_fp, "CHECK WARNING: NULL rvdom freeptr\n");
			} else
				return(woolz_check_exit(allocsize, called_from, rvdom));
		}
		
		if (( (rvdom->lastln - rvdom->line1 + 1) *  rvdom->width *
				sizeof(GREY) ) > allocsize )
			return(woolz_check_exit(VDMRFSIZE, called_from, rvdom));
			
 		/* 
 		 * r5. Finally check that this fully covers the idom 
 		 */
		if( (idom->line1  < rvdom->line1)  ||
			(idom->lastln > rvdom->lastln) ||
			(idom->kol1   < rvdom->kol1)   ||
			(idom->lastkl > rvdom->kol1 + rvdom->width) )
			return(woolz_check_exit(VDMRCIDM, called_from, rvdom));

/*----*/if(check_level < 3) return(0);

		/* 
		 * r2. coordinate check 
		 */
		if ( ckcoord( rvdom->line1, rvdom->lastln, rvdom->kol1,
					  rvdom->kol1 + rvdom->width) )
			return(woolz_check_exit(VDMRCOORD, called_from, rvdom));
		
		break;
	case 21:
		ivdom = (struct ivtable *) vdom;
		
		/* 
		 * i1. Check that the interval line array starts after the ivtable 
		 */
		vil = (struct valintline *) (ivdom + 1);
		if ( vil != ivdom->vil)
			return(woolz_check_exit(VDMIILPTR, called_from, ivdom));

		allocsize = Alloc_size(ivdom->freeptr);		
		if (allocsize < 0) {
			/*
			 * objects with NULL ivdom->freeptr can exist, so skip tests below if
			 * above is NULL; do this properly later
			 */
			if(allocsize == ALCNULL)
				break;
			else
				return(woolz_check_exit(allocsize, called_from, ivdom->freeptr));
		}

/*----*/if(check_level < 3) return(0);

		valuetop = vdom->freeptr + allocsize; /* top of grey value tabel space */
		/* 
		 * i2,i3. Now going to scan down the interval records, checking both 
		 * the interval coordinates are in range, and that the grey pointers
		 * fall within the grey table extent.
		 * Also works out the total number of intervals as a by-product
		 */
		nints = 0;
		for ( i=0; i<lines; i++) {
			vi = vil->vtbint;
			nints += vil->nintvs;
			for( j=0; j < vil->nintvs; j++) {
				if( ckcoord( 0, 0, vi->vkol1, vi->vlastkl) )
					return(woolz_check_exit(VDMIIVCOORD, called_from, ivdom));
				if( ( (char *)vi->values < vdom->freeptr ) ||
					( (char *)vi->values >
						(valuetop - vi->vlastkl + vi->vkol1 - 1) ) )
 					return(woolz_check_exit(VDMIGSIZE, called_from, ivdom));
				vi++;			/* address next grey interval */
			}
			vil++;				/* next grey interval line structure */
		}
		
		greysize = sizeof(struct valueline) * nints;
		/* 
		 * i6. Check allocated size not exceeded 
		 */
		if (greysize > allocsize) {
			if(wz_db_fp)
				fprintf(wz_db_fp, "CHECK: Invalid grey table size %d in memory size %d\n",
					vdomsize, allocsize );
			return(woolz_check_exit(VDMISIZE, called_from, vdom->freeptr));
		}
		break;

		vdomsize = sizeof(struct ivtable) +
					(sizeof(struct valintline) * lines) +
					(sizeof(struct valueline) * nints);

		/* 
		 * i5. Check vdom memory block header 
		 */
		allocsize = Alloc_size(ivdom);
		if (allocsize < 0)
			return(woolz_check_exit(allocsize, called_from, ivdom));

		/* 
		 * i6. Check allocated size not exceeded 
		 */
		if (vdomsize > allocsize) {
			if(wz_db_fp)
				fprintf(wz_db_fp, "CHECK: Invalid vdom size %d in memory size %d\n",
					vdomsize, allocsize );
			return(woolz_check_exit(VDMISIZE, called_from, vdom));
		}
		
		/* 
		 * i4. Now going to do much the same again but checking that intervals
		 * are sequential and properly addressed, this should be incorporated
		 * into the above check, but things get abit complex, so its been
		 * integrated with the size checking. 
		 */
		vil = (struct valintline *) (ivdom + 1);
		vi = vil->vtbint;	/* reset vi */
		for (i=0; i<nints; i++) {
			if (vi != vil->vtbint)
				return(woolz_check_exit(VDMIIVPTR, called_from, vdom));
			vi += vil->nintvs;
			vil++;
		}
	default:
		return(woolz_check_exit(VDMETYPE, called_from, vdom));
	}	/* end switch */

	return(0);		/* successfully tested */
}

/*
 *
 *	Function:		woolz_check_cvhdom
 *
 *	Purpose:		Exhaustive validation of convex hull domain
 *
 *	Description:	Performs following tests:
 *					1. Mid coordinates are in range
 *					2. No of sig chords <= chords
 *					3. No of chords > 0 and <= predefined maximum
 *					4. Acon Bcon fields in range
 *					5. Chord length in range
 *					( 6.) Chord bay row column in range
 *					( 7.) Barea sensible
 *					8. Memory block header
 *					9. Allocated size not exceeded
 *					Bracketed numbers indicate test commented out for
 *					now as fields are not filled out by software
 *
 *	Entry:			Convex hull domain pointer
 *
 *	Exit:			Error status of first detected error, or zero if ok
 *
 */
int 
woolz_check_cvhdom(cdom, called_from)
struct cvhdom *cdom;
char *called_from;
{
	struct chord *chrd;
	register int i, inout;

/*----*/if(check_level < 1) return(0);
	
	if(cdom == NULL)
		return(woolz_check_exit(CVDNULL, called_from, cdom));
		
/*----*/if(check_level < 2) return(0);

	/* 1. check mid coordinates a ok */
	if( ckcoord( cdom->mdlin, 0, cdom->mdkol, 0) )
		return(woolz_check_exit(CVDECOORD, called_from, cdom));
	
	/* 2. check sigcount is not greater than count */

	if( cdom->nsigchords > cdom->nchords)
		return(woolz_check_exit(CVDECNT, called_from, cdom));
	
	/* 3. check chord count sensible */

	if( (cdom->nchords > CKCDMNCHMAX) || (cdom->nchords < 0) )
		return(woolz_check_exit(CVDECCNT, called_from, cdom));
		
	/* 
	 * 8,9. check that allocated domain memory is not exceeded 
	 */
	ecount = cdom->ch  - ( (struct chord *)(cdom + 1)) ;
	if (ecount > 0)
		if(wz_db_fp)
			fprintf(wz_db_fp, "CHECK WARNING: %d chords skipped\n",ecount);
	allocsize = Alloc_size(cdom);
	if (allocsize < 0)
		return(woolz_check_exit(allocsize, called_from, cdom));
	if(abs((char *) cdom - (char *) cdom->ch) <= allocsize)
		inout = IN;
	else
		inout = OUT;
	 
	switch (inout) {
	case IN:
		ecount += cdom->nchords;
		acount = (allocsize - sizeof(struct cvhdom)) / sizeof(struct chord);
		if (ecount > acount) {
			if(wz_db_fp)
				fprintf(wz_db_fp, "CHECK DEBUG ERROR: %d chords found in %d byte space\n",
					ecount, allocsize );
			return(woolz_check_exit(CVDESIZE, called_from, cdom));
		}
	case OUT:
	default:
		break;
	}
/*----*/if(check_level < 3) return(0);

	for( i=0; i<cdom->nchords; i++) {
		chrd = cdom->ch;

		/* 4. check acon bcon coordinates  ok */
		if (ckcoord( chrd->acon, 0, chrd->bcon, 0 ))
			return(woolz_check_exit(CVDECCOORD, called_from, cdom));
			
		/* 5. check chord length ok */
		if ((chrd->cl < 0) || (chrd->cl > CKCHRDLENMAX))
			return(woolz_check_exit(CVDECLEN, called_from, cdom));
			
		/* 6,7. the checks below have been temporarily removed, at present they are
		undefined */
/*		if ( ckcoord( chrd->bl, 0, chrd->bk, 0) )
			return(woolz_check_exit(CVDECCOORD, called_from, cdom));
		if ( chrd->cl * chrd->bd < chrd->barea )
			return(woolz_check_exit(CVDEBAREA, called_from, cdom));
*/
	}

	return(0);		/* successfully completed all tests */
}

/*
 *
 *	Function:		woolz_check_rdom
 *
 *	Purpose:		Check a rectangle domain
 *
 *	Description:	Checks domain boundaries for int or float rects
 *					tests:
 *					1. Type
 *					2. Check coordinates
 *					3. Memory block header
 *
 *	Entry:			rectangle domain pointer
 *
 *	Exit:			Zero is successful, or non-zero error status
 *
 */

int 
woolz_check_rdom(rdom, called_from)
struct irect *rdom;
char *called_from;
{
	struct frect *frdom;

/*----*/if(check_level < 1) return(0);

	if(rdom == NULL)
		return(woolz_check_exit(RDMNULL, called_from, rdom));
		
/*----*/if(check_level < 2) return(0);

	/* 1. Check type */
	/* 2. Check rectangle coordinates range */
	switch(rdom->type) {
		case 1:
			if (ckcoord(rdom->irl[0], rdom->irl[1], rdom->irk[0], rdom->irk[1]))
				return(woolz_check_exit(RDMECOORD, called_from, rdom));
			if (ckcoord(rdom->irl[2], rdom->irl[3], rdom->irk[2], rdom->irk[3]))
				return(woolz_check_exit(RDMECOORD, called_from, rdom));
			break;
		case 2:
			frdom = (struct frect *) rdom;
			if (ckfcoord(frdom->frl[0], frdom->frl[1], frdom->frk[0], frdom->frk[1]))
				return(woolz_check_exit(RDMECOORD, called_from, frdom));
			if (ckfcoord(frdom->frl[2], frdom->frl[3], frdom->frk[2], frdom->frk[3]))
				return(woolz_check_exit(RDMECOORD, called_from, frdom));
			break;
		default:
			return(woolz_check_exit(RDMETYPE, called_from, rdom));
	}

	/* 3. check memory block header using alloc_size function */
	allocsize = Alloc_size(rdom);
	if (allocsize < 0)
		return(woolz_check_exit(allocsize, called_from, rdom));
	return(0);
}

/*
 *
 *	Function:		woolz_check_hdom
 *
 *	Purpose:		Exhaustively validate a histogram domain
 *
 *	Description:	Test the following:
 *					1. Type
 *					2. R, vertical or horizontal indicator
 *					3. Number of points range
 *					4. Row column of output, range
 *					5. Memory header block
 *					6. Allocated size is not exceeded
 *
 *	Entry:			Histogram domain pointer
 *
 *	Exit:			Error status of first error or zero if o.k.
 *
 */
int 
woolz_check_hdom(hdom, called_from)
struct histogramdomain *hdom;
char *called_from;
{
	register int hdomsize, inout;

/*----*/if(check_level < 1) return(0);

	if(hdom == NULL)
		return(woolz_check_exit(HDMNULL, called_from, hdom));
		
/*----*/if(check_level < 2) return(0);
	
	/* 
	 * 1,2. first check that the type field and 'r' field each contain expected
	 * values
	 */
	if((hdom->type != 1) && (hdom->type != 2))
		return(woolz_check_exit(HDMETYPE, called_from, hdom));

	if( (hdom->r != 1) && (hdom->r != 2) )
		return(woolz_check_exit(HDMER, called_from, hdom));
		
	/* 
	 * 3. check that number of points is within reason 
	 */
	if( (hdom->npoints < 0) || (hdom->npoints > CKHDMNPTMAX) )
		return(woolz_check_exit(HDMECNT, called_from, hdom));
		
	/* 
	 * 4. check output coordintates 
	 */
	if( ckcoord( hdom->l, 0, hdom->k, 0) )
		return(woolz_check_exit(HDMECOORD, called_from, hdom));
	/* 
	 * 5,6. finally check that for the size of this domain there aren't too many pointes 
	 */
	ecount = hdom->hv  - ( (int *)(hdom + 1) );
	
	if (ecount > 0)
		if(wz_db_fp)
			fprintf(wz_db_fp, "CHECK WARNING: %d points skipped\n",ecount);

	allocsize = Alloc_size(hdom);
	if (allocsize < 0)
		return(woolz_check_exit(allocsize, called_from, hdom));
		
	if(abs((char *) hdom - (char *) hdom->hv) <= allocsize)
		inout = IN;
	else
		inout = OUT;
	 
	switch (inout) {
	case IN:
		ecount += hdom->npoints;
		acount = (allocsize - sizeof(struct histogramdomain)) / sizeof(int);
		if (ecount > acount) {
			if(wz_db_fp)
				fprintf(wz_db_fp, "CHECK DEBUG ERROR: %d points found in %d byte space\n",
					ecount, allocsize );
			return(woolz_check_exit(HDMESIZE, called_from, hdom));
		}
	case OUT:
	default:
		break;
	}
	return(0);
}

/*
 *
 *	Function:		woolz_check_blist
 *
 *	Purpose:		Check a boundary lists integrity
 *
 *	Description:	Performs just a few checks, to check that the tree
 *					structure of this domain is traversable in both
 *					directions, and then uses the check polygon domain
 *					routine to validate all associated polygons
 *					To test tree structure this routine calls itself
 *					recursively	. To stop itself getting lost in an
 *					erroneous tree, it checks its up route from the
 *					next level down before going to that level
 *					Tests:
 *					1. Stack left
 *					2. Type
 *					3. Polygon using ckpdom
 *					4. Check subsequent chain link, next or down
 *					5. Check memory block
 *
 *	Entry:			Pointer to boundary list
 *
 *	Exit:			Error status of first detected error, or zero if ok
 *
 */
int 
woolz_check_blist(blist, called_from)
struct boundlist *blist;
char *called_from;
{
	static int blevel = 0;
	register int bstat, pstat, inout;		/* new copies of these for each level */
	struct boundlist *b2list;

/*----*/if(check_level < 1) return(0);

	if(blist == NULL && !blevel)
		return(woolz_check_exit(BDLNULL, called_from, blist));
		
/*----*/if(check_level < 2) return(0);
		
	/* 
	 * 1. stack check 
	 */
	if(wz_db_fp) {
		blevel++;
		/*
		if (freemem() <= 32) {
			fprintf(wz_db_fp, "CHECK: FATAL ERROR no stack left, level %d\n",blevel);
			fflush(wz_db_fp);
			exit(5);
		}
		*/
	}
	/* 
	 * 2,3,4. At each level check the type and the polygon before trying to find
	 * the next boundlist. Direction of tree search is down first, then left
	 * to right secondarily 
	 */
	if( (blist->type != 0) && (blist->type != 1) )
		return(woolz_check_exit(BDLETYPE, called_from, blist));
	
	if( blist->poly != NULL )
		if ( (pstat = woolz_check_pdom(blist->poly, called_from)) > 0)
			return(woolz_check_exit(pstat, called_from, blist));
		
	if( blist->down != NULL ) {
		b2list = blist->down;
/*		if( b2list->up != blist ) replace while not used by ibound !! */
		
		/* 
		 * 5. Alloc_size returns negative if memory corruption found 
		 */
		allocsize = Alloc_size(blist->down);
		if(allocsize < 0)
			return(woolz_check_exit(allocsize, called_from, blist->down));

		if( (bstat = woolz_check_blist(blist->down, called_from)) > 0)
			return(woolz_check_exit(bstat, called_from, blist));			/* return up the tree if error found */	
	}
	
	if( blist->next != NULL ) {
		b2list = blist->next;

		/* 
		 * At one level all the up pointers should be the same, and as the 
		 * first one was checked before coming down this far, this will validate
		 * them all 
		 */
/*		if( b2list->up != blist->up ) replaced while up not used by ibound */
		allocsize = Alloc_size(blist->next);
		if (allocsize < 0)
			return(woolz_check_exit(allocsize, called_from, blist->next));

		if( (bstat = woolz_check_blist(blist->next, called_from)) > 0)
			return(woolz_check_exit(bstat, called_from, blist));			/* return up the tree if error found */
	}
	if(wz_db_fp)
		blevel--;		/* recursion counter */

	return(0);		/* ok for this level */
}

/*
 *
 *	Function:		woolz_check_tdom
 *
 *	Purpose:		Validates a text OBJECT as much as possible
 *
 *	Description:	Performs consistency checks on both the text domain
 *					structure and the associated text .
 *					1. Output coordinate range
 *					2. String length sensible
 *					3. No non ascii characters contained
 *
 *	Entry:			N.B. This routine takes an OBJECT not the domain
 *
 *	Exit:			Zero if successful, or error status of first error.
 *
 */
int 
woolz_check_tdom(tobj, called_from)
struct text *tobj;
char *called_from;
{
	struct textdomain *tdom;
	register char *chr;
	register int i,len,errcnt;

/*----*/if(check_level < 1) return(0);

	if(tobj == NULL)
		return(woolz_check_exit(TDMNULL, called_from, tobj));
		
/*----*/if(check_level < 2) return(0);
	
	tdom = tobj->tdom;
	chr = tobj->text;
	
	/* 1. first check the coordinates look sensible */
	if( ckcoord( tdom->l, 0, tdom->k, 0) )
		return(woolz_check_exit(TDMECOORD, called_from, tdom));
	
	/* 2. Check length is plausible */
	len = tdom->stringlen;
	if( (len < 0) || (len > CKTDMNSTMAX) )
		return(woolz_check_exit(TDMECNT, called_from, tdom));
		
/*----*/if(check_level < 3) return(0);

	/* 3. Whizz through the text */
	errcnt = 0;
	for ( i = 0; i < len; i++ )
	{
		if( (*chr < 0x20) || (*chr > 0x7f) ) {
			if(wz_db_fp)
				fprintf(wz_db_fp, "CHECK WARNING: tdom char %x encountered\n",*chr);
				
			if (++errcnt > 4)
				i = len;		/* dont list out a massive load of errors */
		}
		chr++;
	}
	
	if(errcnt > 0)
		return(woolz_check_exit(TDMEASC, called_from, tdom));
	
	if(wz_db_fp)
		fprintf(wz_db_fp, "CHECK: text object O.K.\n");

	return(0);					/* otherwise alls well */
}

/*
 *
 *	Functions:		ckcoord, ckfcoord
 *
 *	Purpose:		Make sure that coordinates are in a plausible range
 *
 *	Description:	Checks each of the parameters are within the max
 *					likely extent, even for a large rotated object, if
 *					not then object corruption is likely to have
 *					occured	. Integer and float routines included
 *
 *	Entry:			The four coordinate extents
 *
 *	Exit:			ZERO = success.   ONE = error status.
 *
 */
static int 
ckcoord(line1, lastln, kol1, lastkl)
COORD line1, lastln, kol1, lastkl;
{
	if ( (line1  < CKLINEMIN) || (line1  > CKLINEMAX) ||
		 (lastln < CKLINEMIN) || (lastln > CKLINEMAX) ||
		 (kol1   < CKKOLMIN)  || (kol1   > CKKOLMAX)  ||
		 (lastkl < CKKOLMIN)  || (lastkl > CKKOLMAX)  ) {
		if(wz_db_fp)
			fprintf(wz_db_fp, "line1: %d  lastln: %d  kol1: %d  lastkl: %d\n",
					line1, lastln, kol1, lastkl);
		return(1);
	}
	return(0);		/* coordinates o.k. */
}

static int 
ckfcoord(line1, lastln, kol1, lastkl)
float line1, lastln, kol1, lastkl;
{
	if ( (line1  < CKLINEFLMIN) || (line1  > CKLINEMAX) ||
		 (lastln < CKLINEFLMIN) || (lastln > CKLINEMAX) ||
		 (kol1   < CKKOLFLMIN)  || (kol1   > CKKOLMAX)  ||
		 (lastkl < CKKOLFLMIN)  || (lastkl > CKKOLMAX)  ) {
		if(wz_db_fp)
			fprintf(wz_db_fp, "line1: %f  lastln: %f  kol1: %f  lastkl: %f\n",
					line1, lastln, kol1, lastkl);
		return(1);
	}
	return(0);		/* coordinates o.k. */
}

/*
 * W O O L Z _ C H E C K _ D E B U G ---
 *
 */
woolz_check_debug(fp)
FILE *fp;
{
	wz_db_fp = fp;
}

/*
 * W O O L Z _ C H E C K _ L E V E L ---
 *
 */
woolz_check_level(level)
int level;
{
	check_level = level;
}

#ifdef IDONTTHINKWENEEDTHIS
/*
 * W O O L Z _ E X I T _ F N ---
 * Purpose: 1) Output message to debug file (if debug on)
 *				or, output message to stdout (if debug is off)
 *			2) Determine if the error is fatal by calling woolz_fatal()
 *			3) If the error is fatal, the process will log the message in
 *			   the system error log file (using errlog()), and kill itself with
 *			   signal WOOLZSIG
 *			NOTE: errlog() is not used at present.
 *			4) If the error is non-fatal it returns the value of the error no.
 */ 
woolzexitfn(errno, called_from)
int errno;
char *called_from;
{
	char buf[100];

	sprintf(buf, "Woolz error no. %d [%s] detected, called from %s\n",
		errno, woolz_err_msg(errno), called_from);
	if(wz_db_fp)
		fprintf(wz_db_fp, "%s", buf);
	else
		fprintf(stdout, "%s", buf);
	if (woolz_fatal(errno)) {
		kill(getpid(),WOOLZSIG);
/*		errorlog(buf);*/ /* Can't call errorlog from here ... yet */
	}
	return(errno);
}
#endif IDONTTHINKWENEEDTHIS
	
/*
 * W O O L Z _ C H E C K _ E X I T ---
 *
 */
woolz_check_exit(errno, mesg, ptr)
int errno;
char *mesg;
char *ptr;		/* Could be idom, vdom, obj etc... pointer */
{
	
	if(wz_db_fp)
		fprintf(wz_db_fp, "Checking pointer 0x%x\n", ptr);
		
	return(woolz_exit(errno, mesg));
}
