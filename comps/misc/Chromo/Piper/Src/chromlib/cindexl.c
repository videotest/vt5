/*
 * cindexl.c	Jim Piper	23/1/84
 */
#include <stdio.h>
#include <wstruct.h>

/*
 * return the length centromeric index (percentage) defined as percentage
 * of midpoints polygon length below given centromere line position.
 */
cindexl(mpol, l)
struct object * mpol;
register int l;
{
	register struct polygondomain *p;
	register int i;
	register struct ivertex *vtx;
	
	l <<= 3;	/* since mid-points poly scaled up by 8 */
	p = (struct polygondomain *) mpol->idom;
	vtx = p->vtx;
	i = 0;
	while ((vtx->vtY < l) && (++i < p->nvertices))
		vtx++;
	return (100 * (p->nvertices - i) / p->nvertices);
}
