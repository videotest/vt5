/*
 * median.c	Jim Piper	July 1983
 *
 * Modifications
 *
 *	 7 Feb 1991		CAS		voids
 *	13 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>
#include <seqpar.h>

extern struct sppar sppar;




/*
 * median filter function for seqpar.
 * presently space only declared for up to 7 by 7, but easily increased
 *
 * call as median(1) for 3 by 3, median(2) for 5 by 5, median(3) for 7 by 7.
 * median(0) implements median of point and its 4 square-connected neighbours.
 */
#define MAXMEDSIZE 7
static int SIZE [MAXMEDSIZE+1] = {5, 9, 25, 49};
static int MEDIAN [MAXMEDSIZE+1] = {2, 4, 12, 24};

int median (size)
{
	int q [MAXMEDSIZE*MAXMEDSIZE];
	register j;
	register int *qp;
	register GREY *p;
	int i;
	int medcomp();

	qp = q;
	switch (size) {
	case 0:
		*qp++ = *sppar.adrptr[-1];
		*qp++ = *sppar.adrptr[1];
		p = sppar.adrptr[0] - 1;
		for (j= -1; j<= 1; j++)
			*qp++ = *p++;
		break;
	default:
		for (i= -size; i<= size; i++) {
			p = sppar.adrptr[i] - size;
			for (j= -size; j<= size; j++)
				*qp++ = *p++;
		}
		break;
	}
	sort (q, SIZE[size], sizeof(int), medcomp);
	return (q[MEDIAN[size]]);
}


static
medcomp(i, j)
int *i, *j;
{
	return (*i - *j);
}


/*
 * sort routine derived from C-library routine :
 * @(#)qsort.c	4.1 (Berkeley) 12/21/80
 * But assumed to be sorting integers into natural order, so
 * some simplificatons !
 */

static
sort(a, n)
int *a;
unsigned n;
{
	qs1(a, a+n);
}

static qs1(a, l)
int *a, *l;
{
	register int *i, *j;
	register int c, exc;
	int **k;
	int *lp, *hp;
	unsigned n;


start:
	if((n=l-a) <= 1)
		return(0);
	n /= 2;
	hp = lp = a+n;
	i = a;
	j = l-1;
	for(;;) {
		if(i < lp) {
			if((c = (*i - *lp)) == 0) {
				--lp;
				exc = *i;
				*i = *lp;
				*lp = exc;
				continue;
			}
			if(c < 0) {
				i++;
				continue;
			}
		}

loop:
		if(j > hp) {
			if((c = (*hp - *j)) == 0) {
				++hp;
				exc = *hp;
				*hp = *j;
				*j = exc;
				goto loop;
			}
			if(c > 0) {
				if(i == lp) {
					exc = *i;
					*i = *j;
					*j = *(++hp);
					*hp = exc;
					i = ++lp;
					goto loop;
				}
				c = *i;
				*i = *j;
				*j = c;
				j --;
				i ++;
				continue;
			}
			j --;
			goto loop;
		}


		if(i == lp) {
			if(lp-a >= l-hp) {
				qs1(hp+1, l);
				l = lp;
			} else {
				qs1(a, lp);
				a = hp+1;
			}
			goto start;
		}


		exc = *j;
		*j = *i;
		*i = *(--lp);
		*lp = exc;
		j = --hp;
	}
}
