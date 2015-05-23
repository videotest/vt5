/*
 * midptspoly.c		Jim Piper		20/1/84
 *
 * Extract mid points polygon from an object,
 * by "poor man's skeleton" technique.
 *
 * Modifications
 *
 *	11 Sep 1986		CAS		Includes
 */
#include <stdio.h>
#include <wstruct.h>


struct object *midptspoly(obj)
struct object *obj;
{
	struct object *p, *makemain();
	register struct polygondomain *pdom;
	struct polygondomain *makepolydmn();
	register struct ivertex *vtx;
	struct iwspace iwsp;
	register int kl, n;
	n = 1 + obj->idom->lastln - obj->idom->line1;
	pdom = makepolydmn(1,NULL,0,n,1);
	pdom->nvertices = n;
	vtx = pdom->vtx;
	p = makemain(10,pdom,NULL,NULL,NULL);
	initrasterscan(obj,&iwsp,0);
	while (nextinterval(&iwsp) == 0) {
		if (iwsp.nwlpos >= 1) {
			kl = iwsp.lftpos;
			/*
			 * if there is a gap in the object then must
			 * reduce the number of vertices by the number
			 * of lines skipped
			 */
			pdom->nvertices -= (iwsp.nwlpos - 1);
		}
		if (iwsp.intrmn == 0) {
			vtx->vtX = (kl + iwsp.rgtpos)>>1;
			vtx->vtY = iwsp.linpos;
			vtx++;
		}
	}
	return(p);
}
