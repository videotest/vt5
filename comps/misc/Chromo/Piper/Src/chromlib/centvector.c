/*
 * centvector.c		Jim Piper @ AUC		06-02-86
 *
 * Compute slope of centromere line from mid-points polygon
 * and position in this polygon.  Assumes the scaled *8
 * version of mid-points polygon.
 *
 * Return a vector passing through centromere point, with this slope.
 */
#include <stdio.h>
#include <wstruct.h>


struct ivector *centvector(mpol,cpos)
struct object *mpol;
int cpos;
{
	int x,y,sin,cos;
	register struct ivector *v = (struct ivector *) Malloc(sizeof(struct ivector));

	/*
	 * setnextpoint counts along polygon from 1, not from zero.
	 */
	setnextpoint(mpol->idom,&x,&y,&sin,&cos,cpos+1);
	/*
	 * Don't forget that x, y are scaled by 8
	 */
	sin /= 4;
	cos /= 4;
	v->type = 30;
	v->k1 = (x - cos)/8;
	v->l1 = (y - sin)/8;
	v->k2 = (x + cos)/8;
	v->l2 = (y + sin)/8;
	return(v);
}
