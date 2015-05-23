/*
 * nprofile.c		Jim Piper		20/1/84
 *
 * Modifications
 *
 *	11 Sep 1986		CAS		Includes
 */
#include <stdio.h>
#include <wstruct.h>

/*
 * Make profile from object and input mid-points polygon.
 * Proceed as follows :
 *	"mpol" is a unit-spaced polygon in *8 scaled integer coordinates.
 *	Step along it in unit steps, and find perpendicular direction.
 *	At each step, compute desired profile value.
 * Format same as profile routine in "profile.c", but mpol can extend
 * beyond object boundaries without catastrophic effects.
 *
 *
 * Return a "histogram" type object as the profile.
 *
 * "moment" selects profile type :
 *	-2	max density
 *	-1	mean density (width defined as sum of non-zero points)
 *	 0	integrated density
 *	 1	first moment (sigma |md| / sigma m) (cf. centroid)
 *	 2	second moment (sigma mdd / sigma m) (radius of gyration)
 */
struct object *nprofile(obj, mpol, moment)
struct chromosome *obj;
struct object *mpol;
{
	struct object *prof;
	multiprofiles(obj,mpol,&prof,&moment,1);
	return(prof);
}
