/*
 * rearrange.c			Jim Piper		31/5/84, 20/2/91
 *
 * Copyright (C) Medical Research Council 1984, 1991.
 *
 ***************************************************************************
 * Re-arrange an initial maximum log-likelihood classification in attempt  *
 * to move objects from over-subscribed to under-subscribed classes.	   *
 *									   *
 * See J. Piper (1986), "Classification of chromosomes constrained by	   *
 * expected class size", Pattern Recognition Letters 4:391-395, where	   *
 * this method is referred to as "RC3".					   *
 *									   *
 * Iterate the following procedure:					   *
 * (1)	For every chromosome in an oversubscribed class,		   *
 *	compute best possible move to an undersubscribed class for	   *
 *	which the likelihood is one of the first "CUTOFF" values.	   *
 *	Use cost = P(i|ch) - P(k|ch), i.e. reduction in overall likelihood.*
 *	Cost of cascade = sum(costs of moves)				   *
 * (2)	Implement the overall best move					   *
 * (3)	Iterate until no oversubscribed classes.			   *
 ***************************************************************************
 * MODIFICATIONS
 * -------------
 * 20-02-91	jimp	Major re-write to make shorter, clearer code; to fix
 *			bugs, and to improve ability for experimentation :
 *			(1) Took out the old COSTMODE==1 and DEBUG things.
 *			(2) Re-wrote rearrange() and followchains().
 *			(3) Made likratio, nchains, cutoff, depth into
 *			    parameters of nrearrange().  Rearrange() left
 *			    with same parameters for compatibility, calls
 *			    nrearrange() with suitable defaults.
 *			(4) Shortened search by permitting reassignments
 *			    only if of lower likelihood than current (this
 *			    is sensible but was overlooked in original),
 *			    and by simple pruning strategy.
 */

#include <stdio.h>
#include <wstruct.h>
#include <chromanal.h>

/*
 * static variables....
 */
static int S_cutoff;
static double S_likratio;
static double minmcost;
static struct chchain *chnTOP;
#define LARGE 1000000.0
#define LIKRA -6.0
#define NCHAI 300
#define CUTOF 4
#define MAXCH 4

rearrange(chrcl,clcontents,numobj,clwn,nclass)
struct chromcl *chrcl;
struct classcont *clcontents;
int *clwn;
{
	nrearrange(chrcl,clcontents,numobj,clwn,nclass,LIKRA,NCHAI,CUTOF,MAXCH);
}


nrearrange(chrcl,clcontents,numobj,clwn,nclass,likratio,nchains,cutoff,depth)
struct chromcl *chrcl;
struct classcont *clcontents;
int *clwn;
double likratio;
int nchains,cutoff,depth;
{
	struct classcont *classcontp;
	struct chchain *chchn;
	int clasi,clasj,nc,i,j,nj,k,minc;
	int class,tobemoved;
	double mcost;
	struct chromcl *chrcp;
	struct chchain *chn, *chnj, *followchains(), *chainval(), *bestchain;

	S_likratio = likratio;
	S_cutoff = cutoff;

	chchn = (struct chchain *) Calloc(nchains,sizeof(struct chchain));
	chnTOP = chchn + nchains;

	/*
	 * Iterate until no further change
	 */
	do {
		minmcost = LARGE;
		tobemoved = 0;
		chn = chchn;
		/*
		 * Look at the contents of each class in turn
		 */
		for (i=0,classcontp=clcontents;  i<nclass; i++,classcontp++) {
			/*
			 * oversubscribed ?
			 */
			nj = classcontp->n;
			if (nj > clwn[i]) {
				/*
				 * look at each chromosome in turn
				 */
				if (nj > MAXCHILD)
					nj = MAXCHILD;
				for (j=0;  j<nj;  j++) {
					chrcp = classcontp->chcl[j];
					chnj = followchains(&chn,chrcp,(double)0.0,depth,
						clcontents,nclass,clwn);
					if (chnj != NULL) {
						chrcp->moveval = chainval(chnj,depth)->accum_cost;
						chrcp->movechain = chnj;
						if (chrcp->moveval <= minmcost) {
							bestchain = chnj;
							minmcost = chrcp->moveval;
						}
						tobemoved = 1;
					} 
				}
			}
		}
		/*
		 * implement the best move
		 */
		if (tobemoved)
			movechain(bestchain,clcontents);
	} 
	while (tobemoved);
	Free (chchn);
}



/*
 * recursively construct a re-arrangement chain, passing
 * the accumulated cost forward.
 *
 * This function is called with a chromosome class pointer chrcp.
 * The aim is to measure the loss of likelihood if this chromosome
 * is assigned to another class (subject to constraints).  There
 * are then two cases:
 *	(i) the assigned class has too few chromosomes.  Stop the chain.
 *	(ii) the assigned class has enough or more than enough chromosomes.
 *	     If the maximum chain length has been reached, prohibit this
 *	     chain, otherwise try to extend by moving a chromosome from
 *	     this class to another by calling followchains recursively.
 */
struct chchain *followchains(chn,chrcp,accum,depth,clcontents,nclass,clwn)
struct chchain **chn;
struct chromcl *chrcp;
double accum;
struct classcont *clcontents;
int *clwn;
{
	register struct chchain *chcn, *chcnj;
	struct chromcl *chrcj;
	struct classcont *classcontp;
	register struct cllik *cllak;
	register int i,j,nj;
	int d;
	double cost,lik,likj;

	if (depth <= 0)
		return(NULL);

	/*
	 * Is there sufficient room in the chain-store?
	 * If so, obtain a structure and fill it up.
	 */
	chcn = *chn;
	if (chcn >= chnTOP - S_cutoff)
		return(NULL);
	(*chn)++;
	chcn->chrcl = chrcp;
	chcn->nchildren = 0;
	chcn->class = chrcp->newclass;
	chcn->child[0] = NULL;
	chcn->accum_cost = accum;

	/*
	 * Consider each possible re-assignment of this object
	 * (of course, excluding the current assignment, and
	 * assignments of greater likelihood, which have of
	 * course been considered previously).
	 * Find the likelihood for the re-assigmnent and check
	 * for conformance with conditions S_CUTOFF and S_likratio
	 *
	 * Since the class-likelihoods have been sorted,
	 * we have first to search for the current likelihood.
	 */
	for (i=0,cllak = chrcp->cllika;  i<S_cutoff;  i++,cllak++)
		if (cllak->class == chrcp->newclass) {
			lik = cllak->lval;
			break;
		}
	for (i++,cllak++; i<S_cutoff; i++,cllak++) {
		if (cllak->lval <= S_likratio + chrcp->cllika[0].lval)
			continue;
		cost = lik - cllak->lval;
		/*
		 * No point in continuing if already
		 * there is a lower-cost cascade.
		 */
		if (cost + accum >= minmcost)
			continue;
		/*
		 * we can move "chrcp" to this class with an increment
		 * to the overall likelihood of "cost".  If this is
		 * an under-subscribed class, stop here, else
		 * consider the effect of moving each of the current
		 * members of the class to yet another class.
		 */
		if (clcontents[cllak->class].n < clwn[cllak->class]) {
			/*
			 * BEWARE - although we checked *chn against chnTOP
			 * at entry to this call to followchains(), *chn
			 * may well have been pushed by recursive calls to
			 * followchains() in the alternative path below,
			 * so we MUST repeat the check here.
			 */
			if (*chn < chnTOP - 1) {
				chcnj = (*chn)++;
				chcnj->accum_cost = accum + cost;
				chcnj->nchildren = 0;
				chcnj->class = cllak->class;
				chcnj->child[0] = NULL;
				chcn->child[chcn->nchildren++] = chcnj;
			}
		} 
		else {
// Andy Medvedev's FIX !!!!!!!!!!!!!!!!!!!!!!!
			classcontp = &clcontents[cllak->class];
// was			classcontp = &clcontents[i];
// end of fix
			nj = classcontp->n;
			if (nj > MAXCHILD)
				nj = MAXCHILD;
			for (j=0;  j<nj;  j++) {
				chrcj = classcontp->chcl[j];
				chcnj = followchains(chn,chrcj,accum+cost,depth-1,
						clcontents,nclass,clwn);
				if (chcnj != NULL)
					chcn->child[chcn->nchildren++] = chcnj;
			}
		}
	}
	if (chcn->nchildren == 0) {
		(*chn)--;
		return(NULL);
	} 
	else
		return(chcn);
}



/*
 * recursively pass back the re-arrangement chain with minimum accumulated cost
 */
struct chchain * chainval(chnj,depth)
register struct chchain *chnj;
{
	register int i, mi;
	register struct chchain *c;
	register double mcost;
	if (chnj->nchildren == 0) {
		return (chnj);
	} 
	else {
		for (i=0;  i<chnj->nchildren;  i++) {
			c = chainval(chnj->child[i],depth-1);
			if (i == 0 || c->accum_cost < mcost) {
				mcost = c->accum_cost;
				mi = i;
			}
		}
		chnj->bestchild = mi;
		chnj->accum_cost = mcost;
		/*
		 * this strange return is used subsequently
		 * when implementing a chain move.
		 */
		return(chnj->child[mi]);
	}
}



/*
 * recursively implement a move as specified in a chain.
 * The chromosome in chn->chrcl is moved to chn->class, and
 * movechain is applied to the subchain chn->child[chn->bestchild].
 */
movechain(chn,clcontents)
register struct chchain *chn;
register struct classcont *clcontents;
{
	register int newc,oldc,i;
	struct chromcl *chrcp;
	register struct classcont *clc;
	if (chn->nchildren > 0) {
		/*
		 * relevant per-chromosome class structure
		 */
		chrcp = chn->chrcl;
		/*
		 * current and new classes
		 */
		oldc = chn->class;
		newc = chn->child[chn->bestchild]->class;
		chrcp->newclass = newc;
		/*
		 * locate chromosome in relevant per-class structure
		 * and take it out.   decrement clss count.
		 */
		clc = clcontents + oldc;
		for (i=0;  i<clc->n;  i++)
			if (chrcp == clc->chcl[i])
				break;
		for (i++;  i<clc->n;  i++)
			clc->chcl[i-1] = clc->chcl[i];
		clc->n--;
		/*
		 * add chromosome to new per-class structure
		 * and increment count.
		 */
		clc = clcontents + newc;
		clc->chcl[clc->n] = chrcp;
		clc->n++;
		/*
		 * go to next level of chain and recursively rearrange
		 */
		movechain(chn->child[chn->bestchild],clcontents);
	}
}


liksort(a,b)
register struct cllik *a, *b;
{
	if (a->lval > b->lval)
		return(-1);
	else if (a->lval < b->lval)
		return(1);
	else
	    return(0);
}
