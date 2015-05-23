/*
 * polyinvert.c		Jim Piper	November 30 1983
 * polygon domain operations
 *
 * Modifications
 *
 *	 5 Nov 1987		CAS		Added missing 'breaks' to switch
 *  05 Nov 1987		BDP		Fixed handling of type 2 polygons which was a bit
 *							of an abortion
 *	08 May 1987		BDP		protection against null or empty objs
 *	13 Sep 1986		CAS		Includes
 */
#include <stdio.h>
#include <wstruct.h>

/*
 * reverse a polygon domain - should work for types 1 and 2
 * on 32-bit machines, but beware !!
 * 
 * Protection ensures doamin exists and returns status indicator otherwise.
 * bdp 8/5/87
 *
 *	Now will work for type 2, previously relied on type 1 and 2 vertices
 * being the same size, which they've never been on the mizar. bdp 5/11/87
 */
polyinvert(p)
struct polygondomain *p;
{
	register i,n,x,y;
	float fx,fy;
	register struct ivertex *vtx,*wtx;
	register struct fvertex *fvtx, *fwtx;
	
	if (p == NULL)
		return(1);
	n = p->nvertices;
	vtx = p->vtx;
	wtx = vtx + n - 1;
	n = (n+1)/2;
/*
 *	Switch added 5/11/87 bdp.
 *	The switch is outside the 'for' loop for performance reasons - For those 
 *	whe are wondering..
 */
	switch(p->type) {
		case 1:
			for (i=0; i<n; i++) {
				x = vtx->vtX;
				y = vtx->vtY;
				vtx->vtX = wtx->vtX;
				vtx->vtY = wtx->vtY;
				wtx->vtX = x;
				wtx->vtY = y;
				vtx++;
				wtx--;
			}
			break;
		case 2:
			for (i=0; i<n; i++) {
				fx = fvtx->vtX;
				fy = fvtx->vtY;
				fvtx->vtX = fwtx->vtX;
				fvtx->vtY = fwtx->vtY;
				fwtx->vtX = fx;
				fwtx->vtY = fy;
				fvtx++;
				fwtx--;
			}
			break;
	}
	return(0);
}
