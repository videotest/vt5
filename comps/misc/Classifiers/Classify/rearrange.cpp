#include "StdAfx.h"
#include <stdio.h>
#include "Vcv.h"

static int S_cutoff;
static double S_likratio;
static double minmcost;
static struct chchain *chnTOP;
#define LARGE 1000000.0
#define LIKRA -6.0
#define NCHAI 300
#define CUTOF 4
#define MAXCH 4

static int liksort(cllik *a,cllik *b)
{
	if (a->lval > b->lval)
		return(-1);
	else if (a->lval < b->lval)
		return(1);
	else
	    return(0);
}

/*
 * recursively pass back the re-arrangement chain with minimum accumulated cost
 */
chchain * chainval(chchain *chnj,int depth)
{
	int i, mi;
	struct chchain *c;
	double mcost;
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
void movechain(chchain *chn,classcont *clcontents)
{
	int newc,oldc,i;
	struct chromcl *chrcp;
	struct classcont *clc;
	if (chn->nchildren > 0) {
		/*
		 * relevant per-chromosome class structure
		 */
		chrcp = chn->chrcl;
		/*
		 * current and new classes
		 */
		oldc = chn->_class;
		newc = chn->child[chn->bestchild]->_class;
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
chchain *followchains(chchain **chn,chromcl *chrcp,double accum,int depth,classcont *clcontents,
	int nclass,int *clwn)
{
	struct chchain *chcn, *chcnj;
	struct chromcl *chrcj;
	struct classcont *classcontp;
	struct cllik *cllak;
	int i,j,nj;
//	int d;
	double cost,lik/*,likj*/;

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
	chcn->_class = chrcp->newclass;
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
		if (cllak->_class == chrcp->newclass) {
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
		if (clcontents[cllak->_class].n < clwn[cllak->_class]) {
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
				chcnj->_class = cllak->_class;
				chcnj->child[0] = NULL;
				chcn->child[chcn->nchildren++] = chcnj;
			}
		} 
		else {
// Andy Medvedev's FIX !!!!!!!!!!!!!!!!!!!!!!!
			classcontp = &clcontents[cllak->_class];
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
 ***************************************************************************
 * Re-arrange an initial maximum log-likelihood classification in attempt  *
 * to move objects from over-subscribed to under-subscribed classes.	   *
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
 */

void nrearrange(chromcl *chrcl,classcont *clcontents,int numobj,int *clwn,int nclass,double likratio,
	int nchains,int cutoff,int depth)
{
	classcont *classcontp;
	chchain *chchn;
	int /*clasi,clasj,nc,*/i,j,nj/*,k,minc*/;
	int /*_class,*/tobemoved;
//	double mcost;
	chromcl *chrcp;
	chchain *chn, *chnj, *bestchain;

	S_likratio = likratio;
	S_cutoff = cutoff;

	chchn = (chchain *)calloc(nchains,sizeof(struct chchain));
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
	free (chchn);
}


void rearrange(chromcl *chrcl,classcont *clcontents,int numobj,int *clwn,int nclass, int cutoff)
{
//	nrearrange(chrcl,clcontents,numobj,clwn,nclass,LIKRA,NCHAI,CUTOF,MAXCH);
	nrearrange(chrcl,clcontents,numobj,clwn,nclass,LIKRA,NCHAI,cutoff,MAXCH);
}










