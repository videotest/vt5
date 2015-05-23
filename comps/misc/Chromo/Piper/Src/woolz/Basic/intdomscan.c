/*
 *   intdomscan.c
 *   routines to traverse an interval domain of an object,
 *
 *   Note that one of four raster directions (up/down, right/left)
 *   may be specified.  Even if "left", pointers are always preset to left
 *   end of interval. If "left", then rightmost interval in line first,
 *   otherwise leftmost interval.
 *
 *   Note that some items in interval work space may be left unset if
 *   domain-type is rectangular domain.
 *
 * Modifications
 *
 *  18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	28 May 1987		BDP		For null in initraster, also set colrmn to zero
 *	05 May 1987		BDP		protection against null or empty objs
 *	12 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>


/*
initialise interval scanning
The raster parameter contains two bits, one each to indicate vertical and
horizontal direction. a value of zero is normal top to bottom, left to 
right traversal. Virtually everywhere in the system uses this form.
the field lineraster will be set to + or - 1 and used to move up or down
the object. Protection against null objects and domains has been added.
If a problem is found, then the domain type is set to zero in the work
space, this will cause nextinterval to return a non zero status . No
protection has been intorduced to cope will strange workspace pointers
as these are normally declared statically. bdp 5/5/87 */

initrasterscan(obj,iwsp,raster)
struct object	*obj;
register struct iwspace	*iwsp;
{
	register struct intervaldomain *ivdm;

	if ((obj == NULL) || (woolz_check_idom(obj->idom, "initrasterscan") != 0)) {
#ifdef WTRACE
		fprintf(stderr,"initraster scan null detected\n");
#endif WTRACE
		iwsp->objaddr = NULL;
		iwsp->dmntype = 0;	/* this will force nextinterval to return 1 */
		iwsp->colrmn = 0;	/* needed for nextintendpoint	*/
		return(1);			/* return a status flag. not always reviewed */
	}
	
	ivdm = obj->idom;		/* initialise only once object checked */
	
	/* set up iwsp */
	iwsp->objaddr = obj;
	iwsp->dmntype = ivdm->type;
	iwsp->lineraster = 1 - (raster & 2);
	iwsp->colraster = 1 - ((raster & 1) << 1);
	iwsp->linbot = (iwsp->lineraster > 0) ? ivdm->line1 : ivdm->lastln;
	iwsp->linpos = iwsp->linbot - iwsp->lineraster;
	iwsp->linrmn = ivdm->lastln - ivdm->line1;
	switch (iwsp->dmntype) {
	default :
	case 1 :		/* standard domain structure */
		iwsp->intvln = (iwsp->lineraster > 0) ? ivdm->intvlines
		    : ivdm->intvlines + iwsp->linrmn;
		/*
					 * pointer when incremented points to first line
					 * to force nextinterval to move to first line
					 */
		iwsp->intvln -= iwsp->lineraster;
		break;
	case 2 :		/* rectangular domain */
		iwsp->intvln = NULL;
		break;
	}

	iwsp->intrmn = 0;
	iwsp->colrmn = 0;
	iwsp->linrmn++;
	iwsp->intdmn = ivdm;

	return(0);		/* indicate ok to anyone wanting to check */
}




/*
get next interval in a standard object.
Protection against Null objects in the system has been combined between this
routine and initraster scan. See comments in that routine. The case statement
below has had a default added which returns 1, ie end of object. It is this 
which will be activated immediately by a domaintype of zero set up by init
rasterscan upon error . bdp 5/5/87 */

nextinterval(iwsp)
register struct iwspace	*iwsp;
{
	switch (iwsp->dmntype) {
	case 1 :	/* conventional domain */
		iwsp->nwlpos = 0;
		if (iwsp->intrmn-- <= 0) {	/* move to next line */
			do {
				iwsp->linpos += iwsp->lineraster;
				if (iwsp->linrmn-- <= 0)
					return(1);
				iwsp->intvln += iwsp->lineraster;	/* next line of intervals */
				iwsp->intrmn = iwsp->intvln->nintvs;
				iwsp->nwlpos++;		/* count of newlines since last interval */
			} while (iwsp->intrmn-- == 0);	/* until non-empty line */
			iwsp->intpos = (iwsp->colraster > 0) ? iwsp->intvln->intvs
			    : iwsp->intvln->intvs + iwsp->intrmn;
		}
		else {
			iwsp->intpos += iwsp->colraster;
		}
		iwsp->lftpos = iwsp->intpos->ileft + iwsp->intdmn->kol1;
		iwsp->colpos = iwsp->lftpos ;
		iwsp->rgtpos = iwsp->intpos->iright + iwsp->intdmn->kol1;
		break;

	case 2 :	/* rectanglar domain */
		iwsp->linpos += iwsp->lineraster;
		if (iwsp->linrmn-- <= 0)
			return(1);
		iwsp->nwlpos = 1;
		iwsp->intpos = (struct interval *) &(iwsp->intdmn->kol1);
		iwsp->lftpos = iwsp->intdmn->kol1;
		iwsp->colpos = iwsp->lftpos;
		iwsp->rgtpos = iwsp->intdmn->lastkl;
		break;

	default:
		return(1);	/* some error so return finished */
	}
	iwsp->colrmn = iwsp->rgtpos - iwsp->lftpos + 1;
	return(0);
}
