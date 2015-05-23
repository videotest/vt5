/*
 * rsupoly.c		Jim Piper	November 30 1983
 * polygon domain operations
 *
 * Modifications
 *
 *	18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	25 Nov 1987		BDP		Fixed a problem with Null polygon handling
 *	08 May 1987		BDP		protection against null or empty objs
 *	24 Sep 1986		GJP		Big Free not little free
 *	13 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>

/*
 * convert an integer polygon with unspecified inter-point
 * spacing into a real-valued polygon by constructing approximately 
 * equally spaced points and then smoothing (iter iterations).
 *
 * then move along this polygon in unit steps, and output the
 * resulting unit-spaced polygon
 * Protection added on the input object. if NULL object  supplied then a
 * null is returned . bdp 8/5/87
 */

struct object *
realsmoothunitpoly(polyobj, iter)
struct object *polyobj;
{
	struct object *newpolyobj,*makemain();
	struct polygondomain *poly,*newpoly,*unitspace();

	if (woolz_check_obj(polyobj, "realsmoothunitpoly") != 0)
		return(NULL);
	/*
	 * copy input polygon to new, real polygon with new vertices
	 * so that points spaced 1 apart
	 */
	poly = unitspace(polyobj->idom);
	/*
	 * smooth - linear filter for now (try splines later ?)
	 */
	polysmooth(poly, iter);
	/*
	 * copy smoothed polygon to new, real polygon with new vertices
	 * so that points spaced 1 apart
	 */
	newpoly = unitspace(poly);
	/*
	 * remove unwanted space and
	 * return a complete object
	 */
	/* 25/11/87 check that neither of these routines are NULL */
	
	if ( poly != NULL )
		Free(poly);
	if ( newpoly == NULL)
		return(NULL);
	else
		return(makemain(10,newpoly,NULL,NULL,polyobj));
}
