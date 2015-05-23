/*
 * ibound.c		Jim Piper	8 September 1987
 *
 * Woolz structured recursive boundary function.
 *
 * Modifications
 *
 *	 7 Feb 1991		CAS		voids
 *	18 Nov 1988		dcb		woolz_check_obj() instaed of wzcheckobj()
 *	24-09-87	jimp/bdp	Check for empty/NULL input object
 */
#include <stdio.h>
#include <wstruct.h>


/*
 * structure definitions
 */
struct bound_interval {
	COORD line;
	COORD left;
	COORD right;
	struct boundlist *lb;		/* boundary component assigned */
	struct boundlist *rb;
};
struct bound_point {
	struct bound_interval *bi;	/* interval point lies in */
	int interval_props;		/* at interval end ? and which ? */
	struct boundlist *bl;		/* current boundary component */
	struct boundlist *bwas;		/* previous boundary component of this point */
	COORD l;
	COORD k;
};
/*
 * constant definitions
 */
#define LEFT_END	1
#define RIGHT_END	2
#define PIECE		0
#define HOLE		1
#define UP		1
#define DOWN		2 

/*
 * compute the recursive boundary structure of input object "obj",
 * with "wrap" points repeated in each boundary polygon to endure
 * wrap-around (usual value for drawing purposes : 1).
 * All boundary polygon arcs lie in one of the 8 directions vertical,
 * horizontal, diagonal.
 */
struct object *
ibound(obj,wrap)
struct object *obj;
int wrap;
{
	register prev_direc, line1, lastln;
	int icount, nv;
	struct iwspace iwsp;
	struct bound_interval *bi, *bitop, *makeboundints();
	register struct bound_interval *ci, *ci_ov, *ci_next;
	struct ivertex cur_bpt;
	static struct boundlist uhole = {HOLE,NULL,NULL,NULL,1,NULL};
	struct bound_point cur_pt;
	struct object *bobj, *makemain();
	struct polygondomain *makepolydmn();
	struct ivertex *vtx, *pvtx;

	/*
	 * Initial checks.
	 */
	if (woolz_check_obj(obj, "ibound") != 0 || wzemptyidom(obj->idom) > 0)
		return(NULL);

	/*
	 * copy object domain into more appropriate structure,
	 * fix so that outside pieces are inserted in universal hole.
	 * This should work fine for rectangular domains.
	 */
	icount = intcount(obj->idom);
	bi = makeboundints(obj,icount);
	bi->rb = &uhole;
#ifdef DEBUG
fprintf(stderr,"Uhole address planted %d\n",bi->rb);
#endif DEBUG
	bitop = bi + icount + 1;
	/*
	 * allocate a vertex table.  Since each interval endpoint contributes
	 * up to two points to a boundary
	 * we may need up to 4*intcount vertices.
	 */
	pvtx = (struct ivertex *)Malloc(sizeof(struct ivertex)*icount*4);
	/*
	 * object line bounds
	 */
	line1 = obj->idom->line1;
	lastln = obj->idom->lastln;

	/*
	 * Check for unprocessed interval end-point -
	 * implies that nested boundary structure is incomplete
	 */
	uhole.down = NULL;
	while (nextunprocessed(bi,bitop,&cur_pt) != 0) {
		/*
		 * plant first boundary point in vertex list
		 */
		vtx=pvtx;
		vtx->vtX = cur_pt.k;
		vtx->vtY = cur_pt.l;
		vtx++;
		nv=1;
		/*
		 * Find if piece or hole;
		 * Plant pointer back to containing element of hierarchy.
		 * If piece, previous search direction is set to UP.
		 * If hole, previous search direction is set to DOWN.
		 * In either case, this will almost immediately be reversed
		 * as the boundary is constructed.
		 */
		if (cur_pt.interval_props == LEFT_END) {
			prev_direc = UP;
		}
		else {
			prev_direc = DOWN;
		}
		/*
		 * search round the boundary for next endpoint
		 * until we get back to the beginning.
		 * While searching UP, we prefer to acquire interval left
		 * end points, while searching DOWN, the preference is to
		 * interval right end points.
		 *
		 * In the diagrams that follow, the current point is "O",
		 * and the next interval end-point in the boundary is "X".
		 * Additional points are "*" and "+".  The difference is
		 * to do with whether the point is "essential" or is added
		 * in order to restrict the boundary to the 8 directions.
		 * If either type of additional point
		 * is at an interval end-point IT MUST NOT BE RECORDED
		 * AS SUCH in the bound_interval structure.
		 * Other domain points "-".
		 */
		while (cur_pt.bwas == NULL) {
#ifdef DEBUG
			fprintf(stderr,"%3d %3d (dir %d)(int-prop %d)\n",cur_pt.l,cur_pt.k,prev_direc,cur_pt.interval_props);
#endif DEBUG
			ci = cur_pt.bi;
			if (prev_direc == DOWN) {
				ci_ov = ci + 1;
#ifdef DEBUG
				if (cur_pt.interval_props != RIGHT_END) {
					fprintf(stderr,"unexpected end-point, going down\n");
					exit(1);
				}
#endif DEBUG
			/*
			 * No overlapping interval below
			 *
			 *     X----O
			 *
			 * reverse direction
			 */
				/* last line ? */
				if (cur_pt.l == lastln) {
					newbpoint(&cur_pt,ci,&vtx,&nv,LEFT_END);
					prev_direc = UP;
					continue;	/* the outer while loop */
				}
				/* find next line */
				while (ci_ov->line <= ci->line)
					ci_ov++;
				/* find first overlapping interval, if any */
				while (ci_ov->line == ci->line+1 && ci_ov->right < ci->left-1)
					ci_ov++;
				if (ci_ov->line > ci->line+1 || ci_ov->left > ci->right+1) {
					/* nothing overlaps */
					newbpoint(&cur_pt,ci,&vtx,&nv,LEFT_END);
					prev_direc = UP;
					continue;
				}
			/*
			 * Adjacent or overlapping interval below,
			 * and another interval to right in this
			 * line which is also adjacent or overlaps.
			 *
			 *    ---O    X---    ---O    X---
			 *    ----*--+----    ----*--+
			 *
			 *    ---O    X---    ---O    X---
			 *        *--+----        *--+
			 *
			 * reverse direction
			 */
				/* find rightmost overlapping interval */
				while ((ci_ov+1)->line == ci->line+1 && (ci_ov+1)->left <= ci->right+1)
					ci_ov++;
				if (ci_ov->right > ci->right) {
					/*
					 * suitable overlapping interval in next line.
					 * check if it overlaps the next
					 * interval in this line.
					 */
					ci_next = ci + 1;
					if (ci_next->line == ci->line && ci_next->left <= ci_ov->right+1) {
						extranewbpoint(&cur_pt,ci_next,&vtx,&nv,LEFT_END);
						prev_direc = UP;
						continue;
					}
				}
			/*
			 * Overlapping or adjacent interval below,
			 * Any further interval on this line not
			 * overlapping or adjacent.
			 *
			 *    ---O             --------O         +---O
			 *    ----+---X        -----X       ----X
			 *
			 *    ----O            ----O
			 *    ----X                 +---X
			 */
#ifdef DEBUG
				if (ci_ov->right < ci->left-1) {
					fprintf(stderr,"Overlap logic chopped (ci %d, %d-%d)(ov %d, %d-%d)\n",
					ci->line,ci->left,ci->right,ci_ov->line,ci_ov->left,ci_ov->right);
					exit(1);
				}
#endif DEBUG
				newbpoint(&cur_pt,ci_ov,&vtx,&nv,RIGHT_END);
				continue;
			}
			else {
				ci_ov = ci - 1;
#ifdef DEBUG
				if (cur_pt.interval_props != LEFT_END) {
					fprintf(stderr,"unexpected end-point, going up\n");
					exit(1);
				}
#endif DEBUG
			/*
			 * No overlapping interval above
			 *
			 *     O----X
			 *
			 * reverse direction
			 */
				/* first line ? */
				if (cur_pt.l == line1) {
					newbpoint(&cur_pt,ci,&vtx,&nv,RIGHT_END);
					prev_direc = DOWN;
					continue;	/* the outer while loop */
				}
				/* find next line */
				while (ci_ov->line >= ci->line)
					ci_ov--;
				/* find first overlapping interval, if any */
				while (ci_ov->line == ci->line-1 && ci_ov->left > ci->right+1)
					ci_ov--;
				if (ci_ov->line < ci->line-1 || ci_ov->right < ci->left-1) {
					/* nothing overlaps */
					newbpoint(&cur_pt,ci,&vtx,&nv,RIGHT_END);
					prev_direc = DOWN;
					continue;
				}
			/*
			 * Adjacent or overlapping interval above,
			 * and another interval to left in this
			 * line which is also adjacent or overlaps.
			 *
			 *    ----+--*----    ----+--*
			 *    ---X    O---    ---X    O---
			 *
			 *        +--*----        +--*
			 *    ---X    O---    ---X    O---
			 *
			 * reverse direction
			 */
				/* find leftmost overlapping interval */
				while ((ci_ov-1)->line == ci->line-1 && (ci_ov-1)->right >= ci->left-1)
					ci_ov--;
				if (ci_ov->left < ci->left) {
					/*
					 * suitable overlapping interval in next line.
					 * check if it overlaps the next
					 * interval in this line.
					 */
					ci_next = ci - 1;
					if (ci_next->line == ci->line && ci_next->right >= ci_ov->left-1) {
						extranewbpoint(&cur_pt,ci_next,&vtx,&nv,RIGHT_END);
						prev_direc = DOWN;
						continue;
					}
				}
			/*
			 * Overlapping or adjacent interval above
			 * Any previous interval on this line not
			 * overlapping or adjacent.
			 *
			 *    X--+----             X----           X----
			 *        O---         O--------      O---+
			 *
			 *    X----            X---+
			 *    O----                 O---
			 */
#ifdef DEBUG
				if (ci_ov->left > ci->right+1) {
					fprintf(stderr,"Overlap logic chopped (ci %d, %d-%d)(ov %d, %d-%d)\n",
					ci->line,ci->left,ci->right,ci_ov->line,ci_ov->left,ci_ov->right);
					exit(1);
				}
#endif DEBUG
				newbpoint(&cur_pt,ci_ov,&vtx,&nv,LEFT_END);
				continue;
			}
		}
#ifdef DEBUG
		if (cur_pt.bwas != cur_pt.bl) {
			fprintf(stderr,"End of segment, but unmatched\n");
			exit(1);
		}
#endif DEBUG
		/*
		 * Attach the boundlist polygon for this component
		 */
		cur_pt.bl->poly = makepolydmn(1,pvtx,nv,nv+wrap-1,1);

		/*
		 * fix the wrap
		 */
		if (wrap > 1)
			wraparound(cur_pt.bl->poly,wrap);
#ifdef POLYPR
{
int i;
for (i=0; i<cur_pt.bl->poly->nvertices; i++)
fprintf(stderr,"%4d %4d\n",cur_pt.bl->poly->vtx[i].vtY,cur_pt.bl->poly->vtx[i].vtX);
fprintf(stderr,"---------\n\n");
}
#endif POLYPR
	}
	Free(pvtx);
	/*
	 * Make the object.  Remember that the first element of the
	 * bound_interval array is the universal hole pointer, and
	 * the top-level boundlist is at the second element.
	 */
	bobj = makemain(11,(bi+1)->lb,NULL,NULL,NULL);
	Free(bi);
	return(bobj);
}

/*
 * Make the bound_interval table.
 * Make an extra location at the start for the universal-hole pointer
 * which is also useful for storing line1-1.  Make an extra location
 * at the end for lastln+1.
 */
struct bound_interval *makeboundints(obj,icount)
struct object *obj;
int icount;
{
	register struct bound_interval *bi, *bib;
	struct iwspace iwsp;
	bi = bib = (struct bound_interval *) Calloc(sizeof(struct bound_interval),icount+2);
	bi->line = obj->idom->line1-1;
	initrasterscan(obj,&iwsp,0);
	while (nextinterval(&iwsp) == 0) {
		bi++;
		bi->left = iwsp.lftpos;
		bi->right = iwsp.rgtpos;
		bi->line = iwsp.linpos;
	}
	(++bi)->line = obj->idom->lastln+1;
	return(bib);
}


/*
 * Find the next unprocessed interval end-point, construct
 * a new boundary segment, and link appropriately to previous
 * segments.   The first interval is a special case since it
 * is ALWAYS part of the boundary of the first piece component.
 * To permit a uniform call and linking structure, a zero'th
 * interval has previously been set up to point to a "universal hole"
 * component, whose "down" pointer is set to the first boundary
 * piece by the first call to this function.
 */
nextunprocessed(bi,bitop,bp)
register struct bound_interval *bi;
struct bound_interval *bitop;
register struct bound_point *bp;
{
	register struct boundlist *bl;
	bp->bwas = NULL;
	/*
	 * look for an unprocessed interval, find enclosing piece or
	 * hole, check its down pointer.  If NULL, fill it in to point
	 * to new segment.  If non-NULL, follow it down and then
	 * sideways through the parallel segments.
	 */
	while (++bi != bitop) {
		if (bi->lb == NULL) {
			/*
			 * left end.  Previous interval right end
			 * tags enclosing hole (possibly uhole).
			 */
			bl = (struct boundlist *)Calloc(sizeof(struct boundlist),1);
			bl->type = PIECE;
			bnd_link(bl,(bi-1)->rb);
			bi->lb = bl;
			bp->bi = bi;
			bp->interval_props = LEFT_END;
			bp->bl = bl;
			bp->k = bi->left;
			bp->l = bi->line;
			return(1);
		}
		if (bi->rb == NULL) {
			/*
			 * right end.  Corresponding left end tags
			 * enclosing piece.
			 */
			bl = (struct boundlist *)Calloc(sizeof(struct boundlist),1);
			bl->type = HOLE;
			bnd_link(bl,bi->lb);
			bi->rb = bl;
			bp->bi = bi;
			bp->interval_props = RIGHT_END;
			bp->bl = bl;
			bp->k = bi->right;
			bp->l = bi->line;
			return(1);
		}
	}
	/*
	 * No further unprocessed intervals
	 */
	return(0);
}

bnd_link(bl,bp)
register struct boundlist *bl, *bp;
{
	register struct boundlist *nbr;
#ifdef DEBUG
fprintf(stderr,"Bnd_link entry bp %d bp->type %d bl->type %d\n",bp,bp->type,bl->type);
#endif DEBUG
	if (bp->type == bl->type) {
		nbr = bp;
		while (nbr->next != NULL)
			nbr = nbr->next;
		nbr->next = bl;
	}
	else if (bp->down == NULL)
		bp->down = bl;
	else {
#ifdef DEBUG
fprintf(stderr,"Bnd_link down not NULL (%d)\n",bp->down);
#endif DEBUG
		nbr = bp->down;
		while (nbr->next != NULL)
			nbr = nbr->next;
		nbr->next = bl;
	}
#ifdef DEBUG
fprintf(stderr,"Bnd_link exit\n");
#endif DEBUG
}


newbpoint(cp,bi,vtx,nv,dir)
register struct bound_point *cp;
register struct bound_interval *bi;
register struct ivertex **vtx;
int *nv, dir;
{
	struct ivertex *wtx = (*vtx)-1;
	register int xdiff, ydiff;
	switch (dir) {
		case LEFT_END:
			cp->bwas = bi->lb;
			bi->lb = cp->bl;
			cp->k = bi->left;
			break;
		case RIGHT_END:
			cp->bwas = bi->rb;
			bi->rb = cp->bl;
			cp->k = bi->right;
			break;
	}
	cp->bi = bi;
	cp->l = bi->line;
	/*
	 * Check whether an additional point must be inserted:
	 * ydiff non-zero and xdiff > 1.
	 */
	ydiff = cp->l - wtx->vtY;
	if (ydiff > 0) {
		xdiff = cp->k - wtx->vtX;
		if (xdiff > 1) {
			(*vtx)->vtX = wtx->vtX + 1;
			(*vtx)->vtY = cp->l;
			(*nv)++;
			vertcompress(vtx,nv);
			(*vtx)++;
		}
		else if (xdiff < -1) {
			(*vtx)->vtX = cp->k + 1;
			(*vtx)->vtY = wtx->vtY;
			/* don't need vertcompress() call here */
			(*vtx)++;
			(*nv)++;
		}
	}
	else if (ydiff < 0) {
		xdiff = cp->k - wtx->vtX;
		if (xdiff > 1) {
			(*vtx)->vtX = cp->k - 1;
			(*vtx)->vtY = wtx->vtY;
			/* don't need vertcompress() call here */
			(*vtx)++;
			(*nv)++;
		}
		else if (xdiff < -1) {
			(*vtx)->vtX = wtx->vtX - 1;
			(*vtx)->vtY = cp->l;
			(*nv)++;
			vertcompress(vtx,nv);
			(*vtx)++;
		}
	}
	cp->interval_props = dir;
	(*vtx)->vtX = cp->k;
	(*vtx)->vtY = cp->l;
	(*nv)++;
	vertcompress(vtx,nv);
	(*vtx)++;
}

extranewbpoint(cp,bi,vtx,nv,dir)
register struct bound_point *cp;
struct bound_interval *bi;
register struct ivertex **vtx;
int *nv, dir;
{
	switch (dir) {
		case LEFT_END:
			(*vtx)->vtX = cp->k + 1;
			(*vtx)->vtY = cp->l + 1;
			break;
		case RIGHT_END:
			(*vtx)->vtX = cp->k - 1;
			(*vtx)->vtY = cp->l - 1;
			break;
	}
#ifdef DEBUG
			fprintf(stderr,"%3d %3d (dir %d)(extra)\n",(*vtx)->vtY,(*vtx)->vtX,dir);
#endif DEBUG
	(*nv)++;
	vertcompress(vtx,nv);
	(*vtx)++;
	newbpoint(cp,bi,vtx,nv,dir);
}


/*
 * Compress colinear runs of vertices.  Since we only permit horizontal,
 * vertical, and diagonal directions, a check for non-reversal plus both
 * segments zero or both non-zero in each of X and Y is sufficient.
 * ENSURE where this function is called that vtx is the most recent vertex
 * and nv is the vertex count INCLUDING the most recent.
 */
vertcompress(vtx,nv)
struct ivertex **vtx;
int *nv;
{
	register struct ivertex *v1, *v2, *v3;
	register int x1,x2,y1,y2;

	v1 = *vtx;
	v2 = v1-1;
	/*
	 * two points may be identical if interval where up-down
	 * direction reverses is only one point long
	 */
	x1 = v1->vtX - v2->vtX;
	y1 = v1->vtY - v2->vtY;
	if (x1 != 0 || y1 != 0) {
		if (*nv < 3)
			return(0);
		v3 = v1-2;
		x2 = v2->vtX - v3->vtX;
		/* check if X reversal */
		if ((x1 <= 0 && x2 > 0) || (x1 >= 0 && x2 < 0) || (x1 != 0 && x2 == 0))
			return(0);
		y2 = v2->vtY - v3->vtY;
		/* check if Y reversal */
		if ((y1 <= 0 && y2 > 0) || (y1 >= 0 && y2 < 0) || (y1 != 0 && y2 == 0))
			return(0);
	}
	/* colinear or x1=y1=0  */
	v2->vtX = v1->vtX;
	v2->vtY = v1->vtY;
	(*vtx)--;
	(*nv)--;
}


wraparound(pdom,wrap)
register struct polygondomain *pdom;
register int wrap;
{
	register struct ivertex *vtx, *btx;
	btx = pdom->vtx;
	/*
	 * There is a default wrap of 1, included in pdom->nvertices
	 */
	wrap--;
	vtx = btx + pdom->nvertices;
	btx++;
	pdom->nvertices += wrap;
	for (; wrap>0; wrap--,btx++,vtx++) {
		vtx->vtX = btx->vtX;
		vtx->vtY = btx->vtY;
	}
}

