/*
 *
 *	W Z C H E C K .C  --
 *
 *	Written: B. Pinnington
 *           Image Recognition Systems
 *           720 Birchwood Boulevard
 *           Birchwood Science Park
 *           Warrington
 *           Cheshire
 *           WA3 7PX
 *
 *	Copyright (c) and intellectual property rights Image Recognition Systems
 *
 *  Date:    Unknown (But some time in 1987..)
 *
 *	Purpose:		Contains all woolz object checking routines
 *
 *	Description:	Several routines are provided for checking a whole
 *					object, or just checking a domain. In all cases
 *					the result is returned as zero for correct, and
 *					non zero for any error.
 *					It is envisaged that the stringency of checks may
 *					may be expanded as the system is proven and be
 *					combined with an exception reporting and handling
 *					mechanism.
 *
 *	Modifications:
 *
 *	 1 Oct 1987		CAS/GJP/SCG		Fixed this to work properly after
 *									some hours of wierd bug hunting
 *									Also changed header to be standard IRS format
 *
 */
#include <stdio.h>
#include <wstruct.h>

#define CKNULL		1
#define CKNULLDMN	2
#define CKINVTYPE	3
#define CKEMPTYDMN	4

/*
 *	W Z C H E C K O B J  -- 
 *
 *	Purpose:		check consistency of supplied object
 *
 *	Description:	Checks the object for being null, or for its first
 *					domain pointer being null. Also checks that type is
 *					legal. Any failure is reported with unique status
 *					Function can be further context expanded via the
 *					the case table established. Logging of errors and
 *					handling of fatal errors may be added later
 *
 *	Entry:			object to be checked
 *
 *	Exit:			0 for correct,  non-zero for error status
 *
 */
int wzcheckobj(obj)
struct object *obj;
{
	register int objstatus, objtype;
	
	objstatus = 0;			/* initialise to ok, then look for possible problems */
	if (obj == NULL)
		objstatus = CKNULL;
	else {
		switch (obj->type) {
			case 1:
			case 10:
			case 11:
			case 12:
			case 13:
			case 70:
				if (obj->idom == NULL)
					objstatus = CKNULLDMN;
				break;
			case 2:
			case 20:
			case 30:
			case 31:
			case 40:
			case 41:
				break;
			case 110:
				if (obj->plist == NULL)
					objstatus = CKNULLDMN;
				break;
			default:
				objstatus = CKINVTYPE;
		}
	}
	return(objstatus);
}

/*
 *	W Z E M P T Y I D O M  -- 
 *
 *	Purpose:		checks that supplied interval domain is not empty
 *
 *	Description:	Checks that line1 is less than or equal to lastln
 *					if not then by definition, the object is empty
 *
 *	Entry:			pointer to interval domain
 *
 *	Exit:			0 for correct,  non-zero for error status
 *
 */
int wzemptyidom(idom)
struct intervaldomain *idom;
{
	register int idomstatus;
	
	idomstatus = 0;
	if (idom == NULL)
		idomstatus = CKNULLDMN;
	else
		if (idom->line1 > idom->lastln)
			idomstatus = CKEMPTYDMN;
			
	return(idomstatus);
}

/*
 *	W Z E M P T Y P D O M  -- 
 *
 *	Purpose:		checks that supplied polygon domain is not empty
 *
 *	Description:	Checks that nvertices is greater than zero
 *					if not then by definition, the object is empty
 *
 *	Entry:			pointer to polygon domain
 *
 *	Exit:			0 for correct,  non-zero for error status
 *
 */
int wzemptypdom(pdom)
struct polygondomain *pdom;
{
	register int pdomstatus;
	
	pdomstatus = 0;
	if (pdom == NULL)
		pdomstatus = CKNULLDMN;
	else
		if (pdom->nvertices <= 0)
			pdomstatus = CKEMPTYDMN;
	return(pdomstatus);
}



