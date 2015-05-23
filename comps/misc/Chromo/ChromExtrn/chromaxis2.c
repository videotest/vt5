/*
 * chromaxis2.c		26-05-86		Jim Piper
 *		Modifications
 *
 *	 7 Feb 1991		CAS		remove Mizar defs -> osk
 *	24 Sep 1986		GJP		change free to Free
 */

#include <stdio.h>
#define COORD _COORD
#include <wstruct.h>
#undef COORD
#include <windows.h>
#include "DebugObj.h"
#pragma warning(disable:4013)


#define TIP 6
#define STEP 3
#define TIPSTRAIGHT 3

#define XPAR 6
#define YPAR 40
#define XEXCPAR	30


/*
 * Find chromosome axis of vertical chromosome.
 * First find "poor man's skeleton".  If this is both long and bent,
 * find axis from skeleton if skeleton is unbranched.
 */
#if defined(_DEBUG)
struct object *_chromaxis2(obj,skel,debug)
struct debugobj *debug;
#else
struct object *_chromaxis2(obj,skel)
#endif
struct object *obj;
int *skel;
{
	register struct object *mpol, *skpol;
	struct object *pmsaxis(), *skelaxis();
	struct intervaldomain *intrp8(),*uspaxis8();
	struct polygondomain *pdom;
	register int tip, yr;

	/*
	 * find mid points polygon by new poor man's skeleton routine
	 */
	tip = (obj->idom->lastln - obj->idom->line1)/4;
	if (tip < 2)
		tip = 2;
	if (tip > TIP)
		tip = TIP;
	mpol = pmsaxis(obj,tip,STEP,TIPSTRAIGHT);
	/*
	 * features of this raw polygon for deciding whether to use skeleton:
	 * (1) height
	 * (2) x range
	 * (3) whether any points except end points lie outside x bounds
	 * (4) ratio of total x excursion to height
	 */
	yr = obj->idom->lastln - obj->idom->line1;
	*skel = 0;
	if ((yr > YPAR && xrange(mpol) > XPAR) ||
			100*xexcursion(mpol)/yr > XEXCPAR ||
			axoutside(mpol,obj) ) {
		/*
		 * skeletonisation
		 */
		skpol = skelaxis(obj);
		if (skpol != NULL) {
			freeobj(mpol);
			mpol = skpol;
			*skel = 1;
		}
	}
#if defined(_DEBUG)
{
struct polygondomain *pdom;
pdom = (struct polygondomain *)mpol->idom;
debug->shortaxis = malloc(sizeof(struct polygondomain)+pdom->maxvertices*sizeof(struct ivertex));
*debug->shortaxis = *pdom;
debug->shortaxis->vtx = (struct ivertex *)(debug->shortaxis+1);
memcpy(debug->shortaxis->vtx, pdom->vtx, pdom->nvertices*sizeof(struct ivertex));
/*for (tip = 0; tip < pdom->nvertices; tip++)
{
	char szBuff[250];
	sprintf(szBuff, "%d : (%d,%d)\n", tip, (int)pdom->vtx[tip].vtX, (int)pdom->vtx[tip].vtY);
	OutputDebugString(szBuff);
}*/
}
#endif
	pdom = (struct polygondomain *)mpol->idom;
	axissmooth(pdom,2);
#if defined(_DEBUG)
{
debug->smoothshortaxis = malloc(sizeof(struct polygondomain)+pdom->maxvertices*sizeof(struct ivertex));
*debug->smoothshortaxis = *pdom;
debug->smoothshortaxis->vtx = (struct ivertex *)(debug->smoothshortaxis+1);
memcpy(debug->smoothshortaxis->vtx, pdom->vtx, pdom->nvertices*sizeof(struct ivertex));
}
#endif
	mpol->idom = uspaxis8(pdom);
	Free(pdom);
#if defined(_DEBUG)
{
struct polygondomain *pdom;
pdom = (struct polygondomain *)mpol->idom;
debug->longaxis = malloc(sizeof(struct polygondomain)+pdom->maxvertices*sizeof(struct ivertex));
*debug->longaxis = *pdom;
debug->longaxis->vtx = (struct ivertex *)(debug->longaxis+1);
memcpy(debug->longaxis->vtx, pdom->vtx, pdom->nvertices*sizeof(struct ivertex));
}
#endif


#ifndef OSK
#ifndef WIN32
	pdom = (struct polygondomain *)mpol->idom;
	mpol->idom = intrp8(pdom);
	Free(pdom);
#endif WIN32
#endif OSK
	return(mpol);
}
