/*
 * centpoly.c		Jim Piper @ AUC		06-02-86
 *
 * Compute slope of centromere line from mid-points polygon
 * and position in this polygon.  Assumes the scaled *8
 * version of mid-points polygon.
 *
 * Return a polygon passing through centromere point, with this slope.
 * This polygon has only one arc and two vertices, but it is intended
 * that it be used to construct a unit-spaced polygon crossing the
 * chromosome.  To this end, it is integer scaled by *8.
 */
#include <stdio.h>
#include <wstruct.h>


struct object *centpoly(mpol,cpos)
struct object *mpol;
int cpos;
{
	int x,y,sin,cos;
	struct object *p, *makemain();
	struct polygondomain *pdom, *makepolydmn();
	struct ivertex *vtx;

	/*
	 * we only want two vertices here
	 */
	pdom = makepolydmn(1,NULL,0,2,1);
	pdom->nvertices = 2;
	vtx = pdom->vtx;
	p = makemain(10,pdom,NULL,NULL,NULL);

	/*
	 * setnextpoint counts along polygon from 1, not from zero.
	 */
	setnextpoint(mpol->idom,&x,&y,&sin,&cos,cpos+1);
	/*
	 * Don't forget that x, y are scaled by 8
	 */
	sin /= 4;
	cos /= 4;
	vtx->vtX = x - cos;
	vtx->vtY = y - sin;
	vtx++;
	vtx->vtX = x + cos;
	vtx->vtY = y + sin;
	return(p);
}
