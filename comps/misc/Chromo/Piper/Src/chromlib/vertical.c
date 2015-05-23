/*
 * vertical.c		Jim Piper		20-05-86
 *
 *
 * Modifications
 *
 *	11 Sep 1986		CAS		Includes
 *  JP		14/05/86	"fipconvhull" used for FIP digitised objects
 */

#include <stdio.h>
#include <math.h>
#include <wstruct.h>
#include <chromanal.h>
#define RADIANS 57.296

double xscale = 1.0;
double yscale = 1.0;



struct chromosome *vertical(obj)
struct chromosome *obj;
{
	struct object *cvh, *convhull(), *fipconvhull();
	struct chromosome *robj, *spinsqueeze();
	struct chromplist *plist;
	struct frect *rdom;
	struct chord *ch;
	int i,j,k,n1,n2,s,c;
	double os9crud;

	if (xscale == yscale)
		cvh = convhull(obj);
	else
		cvh = fipconvhull(obj);
	obj->plist->hullarea = hullarea(cvh, &obj->plist->hullperim);
	mwrangle(cvh,&ch,&n1,&n2,&s,&c);
	if (c == 0) {
		c = 1;
		s *= 1000;
	}
	os9crud = atan((double)s / c);
	robj = spinsqueeze(obj,&os9crud,&xscale,&yscale);
	/*
	 * attach property list from original to rotated object
	 */
	robj->plist = obj->plist;
	/*
	 * fill in the property list
	 */
	plist = robj->plist;
	os9crud = RADIANS*os9crud;	/* angle through which I have been rotated */
	plist->rangle = (int) os9crud;
	freeobj(cvh);
	return(robj);
}
